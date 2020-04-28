#include "objects/VSBoolObject.hpp"

#include "error.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

VSBoolObject *VSBoolObject::_VS_TRUE = NULL;
VSBoolObject *VSBoolObject::_VS_FALSE = NULL;

VSObject *vs_bool(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs == 0) {
        INCREF_RET(VS_FALSE);
    }

    if (nargs == 1) {
        VSObject *obj = args[0];
        VSObject *val = CALL_ATTR(obj, "__bool__", EMPTY_TUPLE());
        if (!VS_IS_TYPE(val, T_BOOL)) {
            err("%s.__bool__() returned \"%s\" instead of bool.", TYPE_STR[obj->type], TYPE_STR[val->type]);
            terminate(TERM_ERROR);
        }

        INCREF_RET(val);
    }

    ERR_NARGS("bool()", 1, nargs);
    terminate(TERM_ERROR);
}

VSObject *vs_bool_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    VS_ENSURE_TYPE(self, T_BOOL, "bool.__hash__()");

    if (nargs != 0) {
        ERR_NARGS("bool.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_INT_TO_INT((cint_t)BOOL_TO_C_BOOL(self)));
}

VSObject *vs_bool_lt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__lt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    VS_ENSURE_TYPE(self, T_BOOL, "bool.__lt__()");
    VS_ENSURE_TYPE(that, T_BOOL, "bool.__lt__()");

    bool res = ((VSBoolObject *)self)->_value < ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_gt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__gt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    VS_ENSURE_TYPE(self, T_BOOL, "bool.__gt__()");
    VS_ENSURE_TYPE(that, T_BOOL, "bool.__gt__()");

    bool res = ((VSBoolObject *)self)->_value > ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_le(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__le__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    VS_ENSURE_TYPE(self, T_BOOL, "bool.__le__()");
    VS_ENSURE_TYPE(that, T_BOOL, "bool.__le__()");

    bool res = ((VSBoolObject *)self)->_value <= ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_ge(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__ge__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    VS_ENSURE_TYPE(self, T_BOOL, "bool.__ge__()");
    VS_ENSURE_TYPE(that, T_BOOL, "bool.__ge__()");

    bool res = ((VSBoolObject *)self)->_value >= ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__eq__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    VS_ENSURE_TYPE(self, T_BOOL, "bool.__eq__()");
    VS_ENSURE_TYPE(that, T_BOOL, "bool.__eq__()");

    bool res = ((VSBoolObject *)self)->_value == ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_BOOL, "bool.__str__()");

    INCREF_RET(C_STRING_TO_STRING(BOOL_TO_C_BOOL(self) ? "true" : "false"));
}

VSObject *vs_bool_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_BOOL, "bool.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_bool_not(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__not__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_BOOL, "bool.__not__()");

    INCREF_RET(
        C_BOOL_TO_BOOL(
            !BOOL_TO_C_BOOL(self)));
}

VSObject *vs_bool_and(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__and__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    VS_ENSURE_TYPE(self, T_BOOL, "bool.__and__()");
    VS_ENSURE_TYPE(that, T_BOOL, "bool.__and__()");

    bool res = ((VSBoolObject *)self)->_value && ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_or(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__or__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    VS_ENSURE_TYPE(self, T_BOOL, "bool.__or__()");
    VS_ENSURE_TYPE(that, T_BOOL, "bool.__or__()");

    bool res = ((VSBoolObject *)self)->_value || ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_xor(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("bool.__xor__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[1];
    VS_ENSURE_TYPE(self, T_BOOL, "bool.__xor__()");
    VS_ENSURE_TYPE(that, T_BOOL, "bool.__xor__()");

    bool res = ((VSBoolObject *)self)->_value ^ ((VSBoolObject *)that)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_bool(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__bool__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_BOOL, "bool.__bool__()");

    INCREF_RET(self);
}

VSObject *vs_bool_char(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__char__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_BOOL, "bool.__char__()");

    INCREF_RET(C_CHAR_TO_CHAR((cchar_t)BOOL_TO_C_BOOL(self)));
}

VSObject *vs_bool_int(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__int__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_BOOL, "bool.__int__()");

    INCREF_RET(C_INT_TO_INT((cint_t)BOOL_TO_C_BOOL(self)));
}

VSObject *vs_bool_float(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("bool.__float__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_BOOL, "bool.__float__()");

    INCREF_RET(C_FLOAT_TO_FLOAT((cfloat_t)BOOL_TO_C_BOOL(self)));
}

VSBoolObject::VSBoolObject(cbool_t value) : _value(value) {
    this->type = T_BOOL;

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_bool_hash);
    NEW_NATIVE_FUNC_ATTR(this, "__lt__", vs_bool_lt);
    NEW_NATIVE_FUNC_ATTR(this, "__gt__", vs_bool_gt);
    NEW_NATIVE_FUNC_ATTR(this, "__le__", vs_bool_le);
    NEW_NATIVE_FUNC_ATTR(this, "__ge__", vs_bool_ge);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_bool_eq);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_bool_str);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_bool_bytes);
    NEW_NATIVE_FUNC_ATTR(this, "__not__", vs_bool_not);
    NEW_NATIVE_FUNC_ATTR(this, "__and__", vs_bool_and);
    NEW_NATIVE_FUNC_ATTR(this, "__or__", vs_bool_or);
    NEW_NATIVE_FUNC_ATTR(this, "__xor__", vs_bool_xor);
    NEW_NATIVE_FUNC_ATTR(this, "__bool__", vs_bool_bool);
    NEW_NATIVE_FUNC_ATTR(this, "__char__", vs_bool_char);
    NEW_NATIVE_FUNC_ATTR(this, "__int__", vs_bool_int);
    NEW_NATIVE_FUNC_ATTR(this, "__float__", vs_bool_float);
}