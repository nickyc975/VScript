#include "objects/VSCharObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

VSObject *vs_char(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *obj = args[0];
    VSObject *val = CALL_ATTR(obj, "__char__", EMPTY_TUPLE());
    if (!VS_IS_TYPE(val, T_CHAR)) {
        err("%s.__char__() returned \"%s\" instead of char.", TYPE_STR[obj->type], TYPE_STR[val->type]);
        terminate(TERM_ERROR);
    }

    INCREF_RET(val);
}

VSObject *vs_char_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_CHAR, "char.__hash__()");

    INCREF_RET(C_INT_TO_INT((((VSCharObject *)self)->_value)));
}

VSObject *vs_char_lt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__lt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_BOOL, "char.__lt__()");
    VS_ENSURE_TYPE(that, T_BOOL, "char.__lt__()");

    cbool_t res = ((VSCharObject *)self)->_value < ((VSCharObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_gt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__gt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_BOOL, "char.__gt__()");
    VS_ENSURE_TYPE(that, T_BOOL, "char.__gt__()");

    cbool_t res = ((VSCharObject *)self)->_value > ((VSCharObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_le(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__le__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_BOOL, "char.__le__()");
    VS_ENSURE_TYPE(that, T_BOOL, "char.__le__()");

    cbool_t res = ((VSCharObject *)self)->_value <= ((VSCharObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_ge(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__ge__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_BOOL, "char.__ge__()");
    VS_ENSURE_TYPE(that, T_BOOL, "char.__ge__()");

    cbool_t res = ((VSCharObject *)self)->_value >= ((VSCharObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__eq__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_BOOL, "char.__eq__()");
    VS_ENSURE_TYPE(that, T_BOOL, "char.__eq__()");

    cbool_t res = ((VSCharObject *)self)->_value == ((VSCharObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_CHAR, "char.__str__()");

    auto str = std::string();
    str.push_back(((VSCharObject *)self)->_value);
    INCREF_RET(C_STRING_TO_STRING((str)));
}

VSObject *vs_char_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_CHAR, "char.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_char_neg(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__neg__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_CHAR, "char.__neg__()");

    cchar_t res = -((VSCharObject *)self)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_add(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__add__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_BOOL, "char.__add__()");
    VS_ENSURE_TYPE(that, T_BOOL, "char.__add__()");

    cchar_t res = ((VSCharObject *)self)->_value + ((VSCharObject *)that)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_sub(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__sub__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_BOOL, "char.__sub__()");
    VS_ENSURE_TYPE(that, T_BOOL, "char.__sub__()");

    cchar_t res = ((VSCharObject *)self)->_value - ((VSCharObject *)that)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_mul(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__mul__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_BOOL, "char.__mul__()");
    VS_ENSURE_TYPE(that, T_BOOL, "char.__mul__()");

    cchar_t res = ((VSCharObject *)self)->_value * ((VSCharObject *)that)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_div(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__div__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_BOOL, "char.__div__()");
    VS_ENSURE_TYPE(that, T_BOOL, "char.__div__()");

    if (((VSCharObject *)that)->_value == 0) {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cchar_t res = ((VSCharObject *)self)->_value / ((VSCharObject *)that)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_mod(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("char.__mod__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_BOOL, "char.__mod__()");
    VS_ENSURE_TYPE(that, T_BOOL, "char.__mod__()");

    if (((VSCharObject *)that)->_value == 0) {
        err("mod by zero\n");
        terminate(TERM_ERROR);
    }

    cchar_t res = ((VSCharObject *)self)->_value % ((VSCharObject *)self)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_bool(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__bool__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_CHAR, "char.__bool__()");

    cchar_t val = ((VSCharObject *)self)->_value;
    INCREF_RET(val ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_char(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__char__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_CHAR, "char.__char__()");

    INCREF_RET(self);
}

VSObject *vs_char_int(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__int__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_CHAR, "char.__int__()");

    INCREF_RET(C_INT_TO_INT((cint_t)CHAR_TO_C_CHAR(self)));
}

VSObject *vs_char_float(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("char.__float__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_CHAR, "char.__float__()");

    INCREF_RET(C_FLOAT_TO_FLOAT(((cfloat_t)CHAR_TO_C_CHAR(self))));
}

VSCharObject::VSCharObject(cchar_t value) : _value(value) {
    this->type = T_CHAR;

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_char_hash);
    NEW_NATIVE_FUNC_ATTR(this, "__lt__", vs_char_lt);
    NEW_NATIVE_FUNC_ATTR(this, "__gt__", vs_char_gt);
    NEW_NATIVE_FUNC_ATTR(this, "__le__", vs_char_le);
    NEW_NATIVE_FUNC_ATTR(this, "__ge__", vs_char_ge);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_char_eq);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_char_str);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_char_bytes);
    NEW_NATIVE_FUNC_ATTR(this, "__neg__", vs_char_neg);
    NEW_NATIVE_FUNC_ATTR(this, "__add__", vs_char_add);
    NEW_NATIVE_FUNC_ATTR(this, "__sub__", vs_char_sub);
    NEW_NATIVE_FUNC_ATTR(this, "__mul__", vs_char_mul);
    NEW_NATIVE_FUNC_ATTR(this, "__div__", vs_char_div);
    NEW_NATIVE_FUNC_ATTR(this, "__mod__", vs_char_mod);
    NEW_NATIVE_FUNC_ATTR(this, "__bool__", vs_char_bool);
    NEW_NATIVE_FUNC_ATTR(this, "__char__", vs_char_char);
    NEW_NATIVE_FUNC_ATTR(this, "__int__", vs_char_int);
    NEW_NATIVE_FUNC_ATTR(this, "__float__", vs_char_float);
}

VSCharObject::~VSCharObject() {
}