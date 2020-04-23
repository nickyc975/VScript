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

VSObject *vs_int(VSObject *obj, VSObject *base) {
    VSObject *val = CALL_ATTR(obj, "__int__", vs_tuple_pack(1, base));
    if (!VS_IS_TYPE(val, T_INT)) {
        err("%s.__int__() returned \"%s\" instead of int.", TYPE_STR[obj->type], TYPE_STR[val->type]);
        terminate(TERM_ERROR);
    }

    INCREF_RET(val);
}

VSObject *vs_int_hash(const VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_INT, "int.__hash__()");

    INCREF_RET(new VSIntObject(((VSIntObject *)obj)->_value));
}

VSObject *vs_int_lt(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_INT, "int.__lt__()");
    VS_ENSURE_TYPE(b, T_INT, "int.__lt__()");

    bool res = ((VSIntObject *)a)->_value < ((VSIntObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_gt(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_INT, "int.__gt__()");
    VS_ENSURE_TYPE(b, T_INT, "int.__gt__()");

    bool res = ((VSIntObject *)a)->_value > ((VSIntObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_le(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_INT, "int.__le__()");
    VS_ENSURE_TYPE(b, T_INT, "int.__le__()");

    bool res = ((VSIntObject *)a)->_value <= ((VSIntObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_ge(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_INT, "int.__ge__()");
    VS_ENSURE_TYPE(b, T_INT, "int.__ge__()");

    bool res = ((VSIntObject *)a)->_value >= ((VSIntObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_eq(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_INT, "int.__eq__()");
    VS_ENSURE_TYPE(b, T_INT, "int.__eq__()");

    bool res = ((VSIntObject *)a)->_value == ((VSIntObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_str(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_INT, "int.__str__()");

    cint_t val = ((VSIntObject *)obj)->_value;
    INCREF_RET(C_STRING_TO_STRING(std::to_string(val)));
}

VSObject *vs_int_bytes(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_INT, "int.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_int_neg(VSObject *intobj) {
    VS_ENSURE_TYPE(intobj, T_INT, "int.__neg__()");

    cint_t res = -((VSIntObject *)intobj)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_add(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_INT, "int.__add__()");
    VS_ENSURE_TYPE(b, T_INT, "int.__add__()");

    cint_t res = ((VSIntObject *)a)->_value + ((VSIntObject *)b)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_sub(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_INT, "int.__sub__()");
    VS_ENSURE_TYPE(b, T_INT, "int.__sub__()");

    cint_t res = ((VSIntObject *)a)->_value - ((VSIntObject *)b)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_mul(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_INT, "int.__mul__()");
    VS_ENSURE_TYPE(b, T_INT, "int.__mul__()");

    cint_t res = ((VSIntObject *)a)->_value * ((VSIntObject *)b)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_div(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_INT, "int.__div__()");
    VS_ENSURE_TYPE(b, T_INT, "int.__div__()");

    if (((VSIntObject *)b)->_value == 0) {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cint_t res = ((VSIntObject *)a)->_value / ((VSIntObject *)b)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_mod(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_INT, "int.__mod__()");
    VS_ENSURE_TYPE(b, T_INT, "int.__mod__()");

    if (((VSIntObject *)b)->_value == 0) {
        err("mod by zero\n");
        terminate(TERM_ERROR);
    }

    cint_t res = ((VSIntObject *)a)->_value % ((VSIntObject *)b)->_value;
    INCREF_RET(C_INT_TO_INT(res));
}

VSObject *vs_int_bool(VSObject *intobj) {
    VS_ENSURE_TYPE(intobj, T_INT, "int.__bool__()");

    cbool_t res = ((VSIntObject *)intobj)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_char(VSObject *intobj) {
    VS_ENSURE_TYPE(intobj, T_INT, "int.__char__()");

    cbool_t res = ((VSIntObject *)intobj)->_value;
    INCREF_RET(C_CHAR_TO_CHAR((cchar_t)res));
}

VSObject *vs_int_int(VSObject *intobj) {
    VS_ENSURE_TYPE(intobj, T_INT, "int.__int__()");

    INCREF_RET(intobj);
}

VSObject *vs_int_float(VSObject *intobj) {
    VS_ENSURE_TYPE(intobj, T_INT, "int.__float__()");

    cbool_t res = ((VSIntObject *)intobj)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT((cfloat_t)res));
}

VSIntObject::VSIntObject(cint_t value) : _value(value) {
    this->type = T_INT;

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_int_hash, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "__lt__", vs_int_lt, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__gt__", vs_int_gt, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__le__", vs_int_le, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__ge__", vs_int_ge, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_int_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_int_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_int_bytes, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__neg__", vs_int_neg, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__add__", vs_int_add, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__sub__", vs_int_sub, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__mul__", vs_int_mul, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__div__", vs_int_div, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__mod__", vs_int_mod, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bool__", vs_int_bool, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__char__", vs_int_char, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__int__", vs_int_int, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__float__", vs_int_float, 1, false);
}