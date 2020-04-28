#include "objects/VSIntObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

VSIntObject *VSIntObject::_VS_ZERO = NULL;
VSIntObject *VSIntObject::_VS_ONE = NULL;

VSObject *vs_int(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs == 0) {
        INCREF_RET(VS_ZERO);
    } else if (nargs == 1) {
        VSObject *obj = args[0];
        VSObject *val = CALL_ATTR(obj, "__int__", EMPTY_TUPLE());
        if (!VS_IS_TYPE(val, T_INT)) {
            err("%s.__int__() returned \"%s\" instead of int.", TYPE_STR[obj->type], TYPE_STR[val->type]);
            terminate(TERM_ERROR);
        }

        INCREF_RET(val);
    } else if (nargs == 2) {
        VSObject *obj = args[0];
        VSObject *base = args[1];
        VSObject *val = CALL_ATTR(obj, "__int__", vs_tuple_pack(1, base));
        if (!VS_IS_TYPE(val, T_INT)) {
            err("%s.__int__() returned \"%s\" instead of int.", TYPE_STR[obj->type], TYPE_STR[val->type]);
            terminate(TERM_ERROR);
        }

        INCREF_RET(val);
    }

    ERR_NARGS("int()", 2, nargs);
    terminate(TERM_ERROR);
}

VSObject *vs_int_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_INT, "int.__hash__()");

    INCREF_RET(new VSIntObject(((VSIntObject *)self)->_value));
}

VSObject *vs_int_lt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__lt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_INT, "int.__lt__()");
    VS_ENSURE_TYPE(that, T_INT, "int.__lt__()");

    bool res = ((VSIntObject *)self)->_value < ((VSIntObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_gt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__gt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_INT, "int.__gt__()");
    VS_ENSURE_TYPE(that, T_INT, "int.__gt__()");

    bool res = ((VSIntObject *)self)->_value > ((VSIntObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_le(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__le__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_INT, "int.__le__()");
    VS_ENSURE_TYPE(that, T_INT, "int.__le__()");

    bool res = ((VSIntObject *)self)->_value <= ((VSIntObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_ge(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__ge__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_INT, "int.__ge__()");
    VS_ENSURE_TYPE(that, T_INT, "int.__ge__()");

    bool res = ((VSIntObject *)self)->_value >= ((VSIntObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__eq__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_INT, "int.__eq__()");
    VS_ENSURE_TYPE(that, T_INT, "int.__eq__()");

    bool res = ((VSIntObject *)self)->_value == ((VSIntObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_INT, "int.__str__()");

    cint_t val = ((VSIntObject *)self)->_value;
    INCREF_RET(C_STRING_TO_STRING(std::to_string(val)));
}

VSObject *vs_int_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_INT, "int.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_int_neg(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__neg__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_INT, "int.__neg__()");

    cint_t res = -((VSIntObject *)self)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_add(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__add__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_INT, "int.__add__()");
    VS_ENSURE_TYPE(that, T_INT, "int.__add__()");

    cint_t res = ((VSIntObject *)self)->_value + ((VSIntObject *)that)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_sub(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__sub__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_INT, "int.__sub__()");
    VS_ENSURE_TYPE(that, T_INT, "int.__sub__()");

    cint_t res = ((VSIntObject *)self)->_value - ((VSIntObject *)that)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_mul(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__mul__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_INT, "int.__mul__()");
    VS_ENSURE_TYPE(that, T_INT, "int.__mul__()");

    cint_t res = ((VSIntObject *)self)->_value * ((VSIntObject *)that)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_div(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__div__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_INT, "int.__div__()");
    VS_ENSURE_TYPE(that, T_INT, "int.__div__()");

    if (((VSIntObject *)that)->_value == 0) {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cint_t res = ((VSIntObject *)self)->_value / ((VSIntObject *)that)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_mod(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("int.__mod__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_INT, "int.__mod__()");
    VS_ENSURE_TYPE(that, T_INT, "int.__mod__()");

    if (((VSIntObject *)that)->_value == 0) {
        err("mod by zero\n");
        terminate(TERM_ERROR);
    }

    cint_t res = ((VSIntObject *)self)->_value % ((VSIntObject *)that)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_bool(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__bool__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_INT, "int.__bool__()");

    cint_t res = ((VSIntObject *)self)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_char(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__char__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_INT, "int.__char__()");

    cint_t res = ((VSIntObject *)self)->_value;
    INCREF_RET(C_CHAR_TO_CHAR((cchar_t)res));
}

VSObject *vs_int_int(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__int__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_INT, "int.__int__()");

    INCREF_RET(self);
}

VSObject *vs_int_float(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("int.__float__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_INT, "int.__float__()");

    cint_t res = ((VSIntObject *)self)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT((cfloat_t)res));
}

VSIntObject::VSIntObject(cint_t value) : _value(value) {
    this->type = T_INT;

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_int_hash);
    NEW_NATIVE_FUNC_ATTR(this, "__lt__", vs_int_lt);
    NEW_NATIVE_FUNC_ATTR(this, "__gt__", vs_int_gt);
    NEW_NATIVE_FUNC_ATTR(this, "__le__", vs_int_le);
    NEW_NATIVE_FUNC_ATTR(this, "__ge__", vs_int_ge);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_int_eq);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_int_str);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_int_bytes);
    NEW_NATIVE_FUNC_ATTR(this, "__neg__", vs_int_neg);
    NEW_NATIVE_FUNC_ATTR(this, "__add__", vs_int_add);
    NEW_NATIVE_FUNC_ATTR(this, "__sub__", vs_int_sub);
    NEW_NATIVE_FUNC_ATTR(this, "__mul__", vs_int_mul);
    NEW_NATIVE_FUNC_ATTR(this, "__div__", vs_int_div);
    NEW_NATIVE_FUNC_ATTR(this, "__mod__", vs_int_mod);
    NEW_NATIVE_FUNC_ATTR(this, "__bool__", vs_int_bool);
    NEW_NATIVE_FUNC_ATTR(this, "__char__", vs_int_char);
    NEW_NATIVE_FUNC_ATTR(this, "__int__", vs_int_int);
    NEW_NATIVE_FUNC_ATTR(this, "__float__", vs_int_float);
}