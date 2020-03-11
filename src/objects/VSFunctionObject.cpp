#include "objects/VSFunctionObject.hpp"
#include "error.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"

#include <cassert>

VSFunctionObject::VSFunctionObject(
    VSObject *name,
    void *cfunc,
    VSObject *closure,
    int nargs,
    bool has_retval,
    bool is_method) {
    // Start function body.
    assert(name != NULL);
    VS_ENSURE_TYPE(VS_TYPEOF(name), T_STR, "as function name");
    this->name = NEW_REF(VSObject *, name);
    this->type = VSFunctionType;

    this->flags = 0;

    assert(cfunc != NULL);
    this->cfunc = cfunc;
    FUNC_SET_IS_C_FUNC(this);

    if (closure != NULL) {
        this->closure = NEW_REF(VSObject *, closure);
        FUNC_SET_USE_CLOSURE(this);
    } else if (!is_method) {
        this->closure = NEW_REF(VSObject *, VS_NONE);
    } else {
        err("internal error: method \"%s\" not bound to object\n", vs_string_to_cstring(name).c_str());
        terminate(TERM_ERROR);
    }

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

VSFunctionObject::VSFunctionObject(
    VSObject *name,
    VSObject *code,
    VSObject *closure,
    VSObject *builtins,
    VSObject *defaults,
    bool is_method) {
    // Start function body.
    assert(name != NULL);
    VS_ENSURE_TYPE(VS_TYPEOF(name), T_STR, "as function name");
    this->name = NEW_REF(VSObject *, name);
    this->type = VSFunctionType;

    this->flags = 0;

    assert(code != NULL);
    VS_ENSURE_TYPE(VS_TYPEOF(code), T_CODE, "as function code");
    this->code = NEW_REF(VSObject *, code);

    if (closure != NULL) {
        this->closure = NEW_REF(VSObject *, closure);
        FUNC_SET_USE_CLOSURE(this);
    } else if (!is_method) {
        this->closure = NEW_REF(VSObject *, VS_NONE);
    } else {
        err("internal error: method \"%s\" not bound to object\n", vs_string_to_cstring(name).c_str());
        terminate(TERM_ERROR);
    }

    this->builtins = NEW_REF(VSObject *, builtins == NULL ? VS_NONE : builtins);
    this->defaults = NEW_REF(VSObject *, defaults == NULL ? VS_NONE : defaults);

    if (is_method) {
        FUNC_SET_IS_METHOD(this);
    }
}

VSObject *VSFunctionObject::call(VSFunctionObject *callable, VSObject *args, VSObject *kwargs) {
    bool is_c_func = FUNC_IS_C_FUNC(callable);
    bool is_method = FUNC_IS_METHOD(callable);

    if (is_c_func) {
        switch (FUNC_GET_NARGS(callable)) {
            case 1: {
                if (is_method) {
                    
                }
            }
            case 2:
            case 3:
            case 4:
            default:
                break;
        }
    }
    return NULL;
}