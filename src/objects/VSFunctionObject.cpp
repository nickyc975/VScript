#include "objects/VSFunctionObject.hpp"

#include <cassert>

#include "error.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"

VSFunctionObject::VSFunctionObject(VSObject *name, void *cfunc, int nargs, bool has_retval, bool is_method) {
    assert(name != NULL);

    this->type = VSFunctionType;
    VS_ENSURE_TYPE(VS_TYPEOF(name), T_STR, "as function name");
    this->name = NEW_REF(VSObject *, name);
    this->type = VSFunctionType;

    this->flags = 0;

    assert(cfunc != NULL);
    this->cfunc = cfunc;
    FUNC_SET_IS_C_FUNC(this);

    if (nargs < 1 || nargs > 4) {
        err("invalid internal function arg count: %d\n", nargs);
        terminate(TERM_ERROR);
    }
    FUNC_SET_NARGS(this, nargs);

    if (has_retval) {
        FUNC_SET_HAS_RETVAL(this);
    }

    if (is_method) {
        FUNC_SET_IS_METHOD(this);
    }
}

VSFunctionObject::VSFunctionObject(VSObject *name, VSObject *code, VSObject *cellvars, VSObject *freevars, VSObject *defaults, bool is_method) {
    assert(name != NULL);

    this->type = VSFunctionType;
    VS_ENSURE_TYPE(VS_TYPEOF(name), T_STR, "as function name");
    this->name = NEW_REF(VSObject *, name);
    this->type = VSFunctionType;

    this->flags = 0;

    assert(code != NULL);
    VS_ENSURE_TYPE(VS_TYPEOF(code), T_CODE, "as function code");
    this->code = NEW_REF(VSObject *, code);

    if (cellvars != NULL) {
        this->cellvars = NEW_REF(VSObject *, cellvars);
    }

    if (freevars != NULL) {
        this->freevars = NEW_REF(VSObject *, freevars);
    }

    this->defaults = NEW_REF(VSObject *, defaults == NULL ? VS_NONE : defaults);

    if (is_method) {
        FUNC_SET_IS_METHOD(this);
    }
}

VSObject *vs_func_str(VSObject *funcobj) {
    VSTypeObject *type = VS_TYPEOF(funcobj);
    VS_ENSURE_TYPE(type, T_FUNC, "function str");

    INCREF_RET(C_STRING_TO_STRING("function"));
}

VSObject *vs_func_bytes(VSObject *funcobj) {
    VSTypeObject *type = VS_TYPEOF(funcobj);
    VS_ENSURE_TYPE(type, T_FUNC, "function bytes");

    return NULL;
}

VSTypeObject *VSFunctionType = new VSTypeObject(
    VSTypeType,
    T_FUNC,
    "function",               // __name__
    NULL,                     // __attrs__
    NULL,                     // __new__
    NULL,                     // __init__
    NULL,                     // __copy__
    NULL,                     // __clear__
    NULL,                     // __getattr__
    NULL,                     // __hasattr__
    NULL,                     // __setattr__
    NULL,                     // __removeattr__
    vs_hash_not_implemented,  // __hash__
    NULL,                     // __lt__
    vs_default_eq,            // __eq__
    vs_func_str,              // __str__
    vs_func_bytes,            // __bytes__
    NULL,                     // __call__
    NULL,                     // _number_funcs
    NULL                      // _container_funcs
);