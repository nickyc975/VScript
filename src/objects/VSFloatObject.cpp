#include "objects/VSFloatObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

VSObject *vs_float(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs == 0) {
        INCREF_RET(new VSFloatObject(0.0));
    } else if (nargs == 1) {
        VSObject *obj = args[0];
        VSObject *val = CALL_ATTR(obj, "__float__", vs_tuple_pack(0));
        if (!VS_IS_TYPE(val, T_FLOAT)) {
            err("%s.__float__() returned \"%s\" instead of \"float\".", TYPE_STR[obj->type], TYPE_STR[val->type]);
            terminate(TERM_ERROR);
        }

        INCREF_RET(val);
    } else {
        ERR_NARGS("float()", 1, nargs);
        terminate(TERM_ERROR);
    }
}

VSObject *vs_float_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FLOAT, "float.__hash__()");

    cfloat_t val = ((VSFloatObject *)self)->_value;
    std::size_t hash = std::hash<cfloat_t>{}(val);
    INCREF_RET(C_INT_TO_INT(hash));
}

VSObject *vs_float_lt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__lt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_FLOAT, "float.__lt__()");
    VS_ENSURE_TYPE(that, T_FLOAT, "float.__lt__()");

    bool res = ((VSFloatObject *)self)->_value < ((VSFloatObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_gt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__gt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_FLOAT, "float.__gt__()");
    VS_ENSURE_TYPE(that, T_FLOAT, "float.__gt__()");

    bool res = ((VSFloatObject *)self)->_value > ((VSFloatObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_le(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__le__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_FLOAT, "float.__le__()");
    VS_ENSURE_TYPE(that, T_FLOAT, "float.__le__()");

    bool res = ((VSFloatObject *)self)->_value <= ((VSFloatObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_ge(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__ge__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_FLOAT, "float.__ge__()");
    VS_ENSURE_TYPE(that, T_FLOAT, "float.__ge__()");

    bool res = ((VSFloatObject *)self)->_value >= ((VSFloatObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__eq__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_FLOAT, "float.__eq__()");
    VS_ENSURE_TYPE(that, T_FLOAT, "float.__eq__()");

    bool res = ((VSFloatObject *)self)->_value == ((VSFloatObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_neg(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__neg__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FLOAT, "float.__neg__()");

    cfloat_t res = -((VSFloatObject *)self)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_add(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__add__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_FLOAT, "float.__add__()");
    VS_ENSURE_TYPE(that, T_FLOAT, "float.__add__()");

    cfloat_t res = ((VSFloatObject *)self)->_value + ((VSFloatObject *)that)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_sub(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__sub__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_FLOAT, "float.__sub__()");
    VS_ENSURE_TYPE(that, T_FLOAT, "float.__sub__()");

    cfloat_t res = ((VSFloatObject *)self)->_value - ((VSFloatObject *)that)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_mul(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__mul__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_FLOAT, "float.__mul__()");
    VS_ENSURE_TYPE(that, T_FLOAT, "float.__mul__()");

    cfloat_t res = ((VSFloatObject *)self)->_value * ((VSFloatObject *)that)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_div(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("float.__div__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_FLOAT, "float.__div__()");
    VS_ENSURE_TYPE(that, T_FLOAT, "float.__div__()");

    if (((VSFloatObject *)that)->_value == 0) {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cfloat_t res = ((VSFloatObject *)self)->_value / ((VSFloatObject *)that)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_bool(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__bool__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FLOAT, "float.__bool__()");

    cfloat_t res = ((VSFloatObject *)self)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_char(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__char__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FLOAT, "float.__char__()");

    cfloat_t res = ((VSFloatObject *)self)->_value;
    INCREF_RET(C_CHAR_TO_CHAR((cchar_t)res));
}

VSObject *vs_float_int(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__int__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FLOAT, "float.__int__()");

    cfloat_t res = ((VSFloatObject *)self)->_value;
    INCREF_RET(C_INT_TO_INT((cint_t)res));
}

VSObject *vs_float_float(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__float__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FLOAT, "float.__float__()");

    INCREF_RET(self);
}

VSObject *vs_float_str(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FLOAT, "float.__str__()");

    cfloat_t val = ((VSFloatObject *)self)->_value;
    INCREF_RET(C_STRING_TO_STRING(std::to_string(val)));
}

VSObject *vs_float_bytes(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("float.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FLOAT, "float.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSFloatObject::VSFloatObject(cfloat_t value) : _value(value) {
    this->type = T_FLOAT;

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_float_hash);
    NEW_NATIVE_FUNC_ATTR(this, "__lt__", vs_float_lt);
    NEW_NATIVE_FUNC_ATTR(this, "__gt__", vs_float_gt);
    NEW_NATIVE_FUNC_ATTR(this, "__le__", vs_float_le);
    NEW_NATIVE_FUNC_ATTR(this, "__ge__", vs_float_ge);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_float_eq);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_float_str);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_float_bytes);
    NEW_NATIVE_FUNC_ATTR(this, "__neg__", vs_float_neg);
    NEW_NATIVE_FUNC_ATTR(this, "__add__", vs_float_add);
    NEW_NATIVE_FUNC_ATTR(this, "__sub__", vs_float_sub);
    NEW_NATIVE_FUNC_ATTR(this, "__mul__", vs_float_mul);
    NEW_NATIVE_FUNC_ATTR(this, "__div__", vs_float_div);
    NEW_NATIVE_FUNC_ATTR(this, "__bool__", vs_float_bool);
    NEW_NATIVE_FUNC_ATTR(this, "__char__", vs_float_char);
    NEW_NATIVE_FUNC_ATTR(this, "__int__", vs_float_int);
    NEW_NATIVE_FUNC_ATTR(this, "__float__", vs_float_float);
}
