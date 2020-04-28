#include "objects/VSFunctionObject.hpp"

#include <cassert>
#include <stack>

#include "objects/VSFrameObject.hpp"
#include "objects/VSStringObject.hpp"

/* begin function attributes */
VSObject *vs_func_str(VSObject *funcobj, VSObject *const *, vs_size_t nargs) {
    VS_ENSURE_TYPE(funcobj, T_FUNC, "func.__str__()");
    if (nargs != 0) {
        ERR_NARGS("func.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *func = (VSFunctionObject *)funcobj;
    INCREF_RET(C_STRING_TO_STRING("function: " + func->name));
}

VSObject *vs_func_bytes(VSObject *funcobj, VSObject *const *, vs_size_t nargs) {
    VS_ENSURE_TYPE(funcobj, T_FUNC, "func.__bytes__()");
    if (nargs != 0) {
        ERR_NARGS("func.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    INCREF_RET(VS_NONE);
}
/* end function attributes */

/* Base function type definition */
VSFunctionObject::VSFunctionObject() {
    this->type = T_FUNC;
}

VSFunctionObject::~VSFunctionObject() {
}

VSObject *VSFunctionObject::call(VSTupleObject *args) {
    INCREF_RET(VS_NONE);
}

/* Native function type definition */
VSNativeFunctionObject::VSNativeFunctionObject(std::string name, vs_native_func func) {
    this->name = name;
    this->func = func;
}

VSNativeFunctionObject::VSNativeFunctionObject(std::string name, vs_native_func func, VSObject *self) {
    this->name = name;
    this->func = func;
    this->self = NEW_REF(VSObject *, self);

    /* Using NEW_NATIVE_FUNC_ATTR() will cause infinite recursion. */
    auto *eq_func = new VSNativeFunctionObject("__eq__", vs_default_eq);
    auto *str_func = new VSNativeFunctionObject("__str__", vs_func_str);
    auto *bytes_func = new VSNativeFunctionObject("__bytes__", vs_func_bytes);

    eq_func->self = NEW_REF(VSObject *, this);
    str_func->self = NEW_REF(VSObject *, this);
    bytes_func->self = NEW_REF(VSObject *, this);

    this->attrs["__eq__"] = new AttributeDef(true, eq_func);
    this->attrs["__str__"] = new AttributeDef(true, str_func);
    this->attrs["__bytes__"] = new AttributeDef(true, bytes_func);

    // eq_func->attrs["__eq__"] = new AttributeDef(true, eq_func);
    // eq_func->attrs["__str__"] = new AttributeDef(true, str_func);
    // eq_func->attrs["__bytes__"] = new AttributeDef(true, bytes_func);

    // str_func->attrs["__eq__"] = new AttributeDef(true, eq_func);
    // str_func->attrs["__str__"] = new AttributeDef(true, str_func);
    // str_func->attrs["__bytes__"] = new AttributeDef(true, bytes_func);

    // bytes_func->attrs["__eq__"] = new AttributeDef(true, eq_func);
    // bytes_func->attrs["__str__"] = new AttributeDef(true, str_func);
    // bytes_func->attrs["__bytes__"] = new AttributeDef(true, bytes_func);
}

VSNativeFunctionObject::~VSNativeFunctionObject() {
    DECREF_EX(this->self);
}

VSObject *VSNativeFunctionObject::call(VSTupleObject *args) {
    assert(args != NULL);
    VS_ENSURE_TYPE(args, T_TUPLE, "as args");
    VSObject *res = this->func(this->self, args->items, args->nitems);
    // check result
    DECREF_EX(args);
    return res;
}

/* Dynamic function type definition */
VSDynamicFunctionObject::VSDynamicFunctionObject(std::string name, VSCodeObject *code, VSTupleObject *freevars, VSTupleObject *builtins, int flags) {
    this->name = name;
    this->code = NEW_REF(VSCodeObject *, code);
    this->freevars = NEW_REF(VSTupleObject *, freevars);
    this->builtins = NEW_REF(VSTupleObject *, builtins);

    this->cellvars = new VSTupleObject(code->ncellvars);
    for (vs_addr_t i = 0; i < code->ncellvars; i++) {
        TUPLE_SET(this->cellvars, i, VS_NONE);
    }
    INCREF(this->cellvars);

    this->flags = flags;

    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_default_eq);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_func_str);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_func_bytes);
}

VSDynamicFunctionObject::~VSDynamicFunctionObject() {
    DECREF_EX(this->code);
    DECREF_EX(this->cellvars);
    DECREF_EX(this->freevars);
    DECREF_EX(this->builtins);
}

VSObject *VSDynamicFunctionObject::call(VSTupleObject *args) {
    assert(args != NULL);
    vs_size_t nargs = TUPLE_LEN(args);
    if (nargs < this->code->nargs || (nargs > this->code->nargs && !(VS_FUNC_VARARGS & this->flags))) {
        ERR_NARGS(this->name.c_str(), this->code->nargs, nargs);
        terminate(TERM_ERROR);
    }

    std::stack<VSObject *> stack = std::stack<VSObject *>();
    VSFrameObject *frame = new VSFrameObject(this->code, args, this->cellvars, this->freevars, this->builtins, NULL);
    DECREF_EX(args);

    frame->eval(stack);

    if (stack.empty()) {
        INCREF_RET(VS_NONE);
    } else if (stack.size() == 1) {
        return stack.top();
    } else {
        err("Internal error: more than 1 object left in compute stack: %ld", stack.size());
        INCREF_RET(VS_NONE);
    }
}