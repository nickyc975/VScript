#include "objects/VSFunctionObject.hpp"

#include <cassert>
#include <stack>

#include "objects/VSFrameObject.hpp"
#include "objects/VSStringObject.hpp"

#include "runtime/VSInterpretor.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);
NEW_IDENTIFIER(__call__);

/* Base function type definition */
VSFunctionObject::VSFunctionObject() {
    this->type = T_FUNC;
}

VSFunctionObject::~VSFunctionObject() {
}

VSObject *VSFunctionObject::call(VSTupleObject *) {
    INCREF_RET(VS_NONE);
}

/* begin native function attributes */
VSObject *vs_native_func_str(VSObject *funcobj, VSObject *const *, vs_size_t nargs) {
    ENSURE_TYPE(funcobj, T_FUNC, "nativefunc.__str__()");
    if (nargs != 0) {
        ERR_NARGS("nativefunc.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *func = (VSFunctionObject *)funcobj;
    INCREF_RET(C_STRING_TO_STRING("native function: " + func->name->_value));
}

VSObject *vs_native_func_bytes(VSObject *funcobj, VSObject *const *, vs_size_t nargs) {
    ENSURE_TYPE(funcobj, T_FUNC, "nativefunc.__bytes__()");
    if (nargs != 0) {
        ERR_NARGS("nativefunc.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    INCREF_RET(VS_NONE);
}
/* end native function attributes */

const str_func_map VSNativeFunctionObject::vs_native_func_methods = {
    {ID___hash__, vs_default_hash},
    {ID___eq__, vs_default_eq},
    {ID___str__, vs_native_func_str},
    {ID___bytes__, vs_native_func_bytes}
};

VSNativeFunctionObject::VSNativeFunctionObject(VSObject *self, VSStringObject *name, vs_native_func func) {
    this->name = name;
    this->func = func;
    this->self = self;
    INCREF(name);
    INCREF(self);
}

VSNativeFunctionObject::~VSNativeFunctionObject() {
    DECREF_EX(this->name);
    DECREF_EX(this->self);
}

bool VSNativeFunctionObject::hasattr(std::string &attrname) {
    if (attrname == ID___call__) {
        return true;
    }

    return vs_native_func_methods.find(attrname) != vs_native_func_methods.end();
}

VSObject *VSNativeFunctionObject::getattr(std::string &attrname) {
    if (attrname == ID___call__) {
        INCREF_RET(this);
    }

    auto iter = vs_native_func_methods.find(attrname);
    if (iter == vs_native_func_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_native_func_methods.at(attrname));
    INCREF_RET(attr);
}

void VSNativeFunctionObject::setattr(std::string &, VSObject *) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}

VSObject *VSNativeFunctionObject::call(VSTupleObject *args) {
    assert(args != NULL);
    ENSURE_TYPE(args, T_TUPLE, "as args");
    VSObject *res = this->func(this->self, args->items, args->nitems);
    // check result
    DECREF_EX(args);
    return res;
}

/* begin dynamic function attributes */
VSObject *vs_dynamic_func_str(VSObject *funcobj, VSObject *const *, vs_size_t nargs) {
    ENSURE_TYPE(funcobj, T_FUNC, "dynamicfunc.__str__()");
    if (nargs != 0) {
        ERR_NARGS("dynamicfunc.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *func = (VSFunctionObject *)funcobj;
    INCREF_RET(C_STRING_TO_STRING("dynamic function: " + func->name->_value));
}

VSObject *vs_dynamic_func_bytes(VSObject *funcobj, VSObject *const *, vs_size_t nargs) {
    ENSURE_TYPE(funcobj, T_FUNC, "dynamicfunc.__bytes__()");
    if (nargs != 0) {
        ERR_NARGS("dynamicfunc.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    INCREF_RET(VS_NONE);
}
/* end dynamic function attributes */

const str_func_map VSDynamicFunctionObject::vs_dynamic_func_methods = {
    {ID___hash__, vs_default_hash},
    {ID___eq__, vs_default_eq},
    {ID___str__, vs_dynamic_func_str},
    {ID___bytes__, vs_dynamic_func_bytes}
};

/* Dynamic function type definition */
VSDynamicFunctionObject::VSDynamicFunctionObject(VSStringObject *name, VSCodeObject *code, VSTupleObject *freevars, int flags) {
    this->name = NEW_REF(VSStringObject *, name);
    this->code = NEW_REF(VSCodeObject *, code);
    this->freevars = NEW_REF(VSTupleObject *, freevars);

    this->cellvars = new VSTupleObject(code->ncellvars);
    for (vs_addr_t i = 0; i < code->ncellvars; i++) {
        TUPLE_SET(this->cellvars, i, VS_NONE);
    }
    INCREF(this->cellvars);

    this->flags = flags;
}

VSDynamicFunctionObject::~VSDynamicFunctionObject() {
    DECREF_EX(this->name);
    DECREF_EX(this->code);
    DECREF_EX(this->cellvars);
    DECREF_EX(this->freevars);
}

bool VSDynamicFunctionObject::hasattr(std::string &attrname) {
    if (attrname == ID___call__) {
        return true;
    }

    return vs_dynamic_func_methods.find(attrname) != vs_dynamic_func_methods.end();
}

VSObject *VSDynamicFunctionObject::getattr(std::string &attrname) {
    if (attrname == ID___call__) {
        INCREF_RET(this);
    }

    auto iter = vs_dynamic_func_methods.find(attrname);
    if (iter == vs_dynamic_func_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_dynamic_func_methods.at(attrname));
    INCREF_RET(attr);
}

void VSDynamicFunctionObject::setattr(std::string &, VSObject *) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}

VSObject *VSDynamicFunctionObject::call(VSTupleObject *args) {
    assert(args != NULL);
    vs_size_t nargs = TUPLE_LEN(args);
    bool va_args = VS_FUNC_VARARGS & this->flags;

    if (va_args && nargs < this->code->nargs - 1) {
        ERR_NARGS(this->name->_value.c_str(), this->code->nargs - 1, nargs);
        terminate(TERM_ERROR);
    } else if (!va_args && nargs != this->code->nargs) {
        ERR_NARGS(this->name->_value.c_str(), this->code->nargs, nargs);
        terminate(TERM_ERROR);
    }

    std::stack<VSObject *> stack = std::stack<VSObject *>();
    VSFrameObject *frame = new VSFrameObject(
        this->code, args, this->cellvars, this->freevars, NULL);
    DECREF_EX(args);

    INCREF(frame);
    INTERPRETOR.eval(stack, frame);
    DECREF(frame);

    if (stack.empty()) {
        INCREF_RET(VS_NONE);
    } else if (stack.size() == 1) {
        return stack.top();
    } else {
        err("Internal error: more than 1 object left in compute stack: %ld", stack.size());
        INCREF_RET(VS_NONE);
    }
}