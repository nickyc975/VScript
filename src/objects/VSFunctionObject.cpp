#include "objects/VSFunctionObject.hpp"

#include <cassert>
#include <stack>

#include "objects/VSFrameObject.hpp"

#define ERR_NARGS(expected, actual)                                \
    err("Unexpected nargs: %ld, expected: %ld", actual, expected); \
    terminate(TERM_ERROR);

/* begin function attributes */
VSObject *vs_func_str(VSObject *funcobj) {
    VS_ENSURE_TYPE(funcobj, T_FUNC, "func.__str__()");

    VSFunctionObject *func = (VSFunctionObject *)funcobj;
    INCREF_RET(C_STRING_TO_STRING("function: " + func->name->_value));
}

VSObject *vs_func_bytes(VSObject *funcobj) {
    VS_ENSURE_TYPE(funcobj, T_FUNC, "func.__bytes__()");

    INCREF_RET(VS_NONE);
}
/* end function attributes */

/* Base function type definition */
VSFunctionObject::VSFunctionObject() {
    this->type = T_FUNC;

    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_default_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_func_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_func_bytes, 1, false);
}

VSFunctionObject::~VSFunctionObject() {
}

VSObject *VSFunctionObject::call(VSTupleObject *args) {
    INCREF_RET(VS_NONE);
}

/* Native function type definition */
VSNativeFunctionObject::VSNativeFunctionObject(VSStringObject *name, void *cfunc, cint_t nargs, bool const_args, VSObject *bind_obj) {
    assert(name != NULL);

    this->name = NEW_REF(VSStringObject *, name);
    this->cfunc = cfunc;
    this->nargs = nargs;
    this->const_args = const_args;
    this->bind_obj = NEW_REF(VSObject *, bind_obj);
}

VSNativeFunctionObject::~VSNativeFunctionObject() {
    DECREF_EX(this->name);
    DECREF_EX(this->bind_obj);
}

VSObject *VSNativeFunctionObject::call(VSTupleObject *args) {
    assert(args != NULL);
    VS_ENSURE_TYPE(args, T_TUPLE, "as args");

    bool is_method = this->bind_obj != NULL;

    cint_t expected_nargs = this->nargs;
    if (is_method) {
        expected_nargs--;
        if (expected_nargs < 0) {
            ERR_NARGS(0, 1);
        }
    }

    cint_t actual_nargs = (cint_t)TUPLE_LEN(args);
    if (expected_nargs != actual_nargs) {
        ERR_NARGS(expected_nargs, actual_nargs);
    }

    VSObject *res = NULL;
    switch (this->nargs) {
        case 1: {
            VSObject *arg0 = is_method ? this->bind_obj : TUPLE_GET(args, 0);
            if (this->const_args) {
                res = ((const_unaryfunc)this->cfunc)(arg0);
            } else {
                res = ((unaryfunc)this->cfunc)(arg0);
            }
            break;
        }
        case 2: {
            VSObject *arg0 = is_method ? this->bind_obj : TUPLE_GET(args, 0);
            VSObject *arg1 = is_method ? TUPLE_GET(args, 0) : TUPLE_GET(args, 1);
            if (this->const_args) {
                res = ((const_binaryfunc)this->cfunc)(arg0, arg1);
            } else {
                res = ((binaryfunc)this->cfunc)(arg0, arg1);
            }
            break;
        }
        case 3: {
            VSObject *arg0 = is_method ? this->bind_obj : TUPLE_GET(args, 0);
            VSObject *arg1 = is_method ? TUPLE_GET(args, 0) : TUPLE_GET(args, 1);
            VSObject *arg2 = is_method ? TUPLE_GET(args, 1) : TUPLE_GET(args, 2);
            if (this->const_args) {
                res = ((const_ternaryfunc)this->cfunc)(arg0, arg1, arg2);
            } else {
                res = ((ternaryfunc)this->cfunc)(arg0, arg1, arg2);
            }
            break;
        }
        case 4: {
            VSObject *arg0 = is_method ? this->bind_obj : TUPLE_GET(args, 0);
            VSObject *arg1 = is_method ? TUPLE_GET(args, 0) : TUPLE_GET(args, 1);
            VSObject *arg2 = is_method ? TUPLE_GET(args, 1) : TUPLE_GET(args, 2);
            VSObject *arg3 = is_method ? TUPLE_GET(args, 2) : TUPLE_GET(args, 3);
            if (this->const_args) {
                res = ((const_quaternaryfunc)this->cfunc)(arg0, arg1, arg2, arg3);
            } else {
                res = ((quaternaryfunc)this->cfunc)(arg0, arg1, arg2, arg3);
            }
            break;
        }
        default:
            // Unreachable
            res = NEW_REF(VSObject *, VS_NONE);
            break;
    }
    return res;
}

/* Dynamic function type definition */
VSDynamicFunctionObject::VSDynamicFunctionObject(VSStringObject *name, VSCodeObject *code, VSTupleObject *cellvars, VSTupleObject *freevars, VSTupleObject *builtins) {
    this->name = NEW_REF(VSStringObject *, name);
    this->code = NEW_REF(VSCodeObject *, code);
    this->cellvars = NEW_REF(VSTupleObject *, cellvars);
    this->freevars = NEW_REF(VSTupleObject *, freevars);
    this->builtins = NEW_REF(VSTupleObject *, builtins);
}

VSDynamicFunctionObject::~VSDynamicFunctionObject() {
    DECREF_EX(this->name);
    DECREF_EX(this->code);
    DECREF_EX(this->cellvars);
    DECREF_EX(this->freevars);
    DECREF_EX(this->builtins);
}

VSObject *VSDynamicFunctionObject::call(VSTupleObject *args) {
    assert(args != NULL);
    cint_t nargs = TUPLE_LEN(args);
    if (nargs != this->code->nargs) {
        ERR_NARGS(this->code->nargs, nargs);
    }

    std::stack<VSObject *> stack = std::stack<VSObject *>();
    VSFrameObject *frame = new VSFrameObject(this->code, args, this->cellvars, this->freevars, this->builtins, NULL);
    frame->eval(stack);

    if (stack.empty()) {
        INCREF_RET(VS_NONE);
    } else if (stack.size() == 1) {
        return stack.top();
    } else {
        err("Internal error: more than 1 object left in compute stack: %ld", stack.size());
        terminate(TERM_ERROR);
    }
}