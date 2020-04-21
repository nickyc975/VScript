#include "objects/VSFloatObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

VSObject *vs_float(VSObject *obj) {
    VSObject *val = CALL_ATTR(obj, "__float__", vs_tuple_pack(0));
    if (!VS_IS_TYPE(val, T_FLOAT)) {
        err("%s.__float__() returned \"%s\" instead of \"float\".", TYPE_STR[obj->type], TYPE_STR[val->type]);
        terminate(TERM_ERROR);
    }

    INCREF_RET(val);
}

VSObject *vs_float_hash(const VSObject *floatobj) {
    VS_ENSURE_TYPE(floatobj, T_FLOAT, "float.__hash__()");

    cfloat_t val = ((VSFloatObject *)floatobj)->_value;
    std::size_t hash = std::hash<cfloat_t>{}(val);
    INCREF_RET(C_INT_TO_INT(hash));
}

VSObject *vs_float_lt(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_FLOAT, "float.__lt__()");
    VS_ENSURE_TYPE(b, T_FLOAT, "float.__lt__()");

    bool res = ((VSFloatObject *)a)->_value < ((VSFloatObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_eq(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_FLOAT, "float.__eq__()");
    VS_ENSURE_TYPE(b, T_FLOAT, "float.__eq__()");

    bool res = ((VSFloatObject *)a)->_value == ((VSFloatObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_neg(VSObject *floatobj) {
    VS_ENSURE_TYPE(floatobj, T_FLOAT, "float.__neg__()");

    cfloat_t res = -((VSFloatObject *)floatobj)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_add(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_FLOAT, "float.__add__()");
    VS_ENSURE_TYPE(b, T_FLOAT, "float.__add__()");

    cfloat_t res = ((VSFloatObject *)a)->_value + ((VSFloatObject *)b)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_sub(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_FLOAT, "float.__sub__()");
    VS_ENSURE_TYPE(b, T_FLOAT, "float.__sub__()");

    cfloat_t res = ((VSFloatObject *)a)->_value - ((VSFloatObject *)b)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_mul(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_FLOAT, "float.__mul__()");
    VS_ENSURE_TYPE(b, T_FLOAT, "float.__mul__()");

    cfloat_t res = ((VSFloatObject *)a)->_value * ((VSFloatObject *)b)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_div(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_FLOAT, "float.__div__()");
    VS_ENSURE_TYPE(b, T_FLOAT, "float.__div__()");

    if (((VSFloatObject *)b)->_value == 0) {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cfloat_t res = ((VSFloatObject *)a)->_value / ((VSFloatObject *)b)->_value;
    INCREF_RET(C_FLOAT_TO_FLOAT(res));
}

VSObject *vs_float_bool(VSObject *floatobj) {
    VS_ENSURE_TYPE(floatobj, T_FLOAT, "float.__bool__()");

    cbool_t res = ((VSFloatObject *)floatobj)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_char(VSObject *floatobj) {
    VS_ENSURE_TYPE(floatobj, T_FLOAT, "float.__char__()");

    cbool_t res = ((VSFloatObject *)floatobj)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_float_int(VSObject *floatobj) {
    VS_ENSURE_TYPE(floatobj, T_FLOAT, "float.__int__()");

    cbool_t res = ((VSFloatObject *)floatobj)->_value;
    INCREF_RET(C_INT_TO_INT((cint_t)res));
}

VSObject *vs_float_float(VSObject *floatobj) {
    VS_ENSURE_TYPE(floatobj, T_FLOAT, "float.__float__()");

    INCREF_RET(floatobj);
}

VSObject *vs_float_str(VSObject *floatobj) {
    VS_ENSURE_TYPE(floatobj, T_FLOAT, "float.__str__()");

    cfloat_t val = ((VSFloatObject *)floatobj)->_value;
    INCREF_RET(C_STRING_TO_STRING(std::to_string(val)));
}

VSObject *vs_float_bytes(VSObject *floatobj) {
    VS_ENSURE_TYPE(floatobj, T_FLOAT, "float.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSFloatObject::VSFloatObject(cfloat_t value) : _value(value) {
    this->type = T_FLOAT;

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_float_hash, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "__lt__", vs_float_lt, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_float_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_float_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_float_bytes, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__neg__", vs_float_neg, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__add__", vs_float_add, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__sub__", vs_float_sub, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__mul__", vs_float_mul, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__div__", vs_float_div, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bool__", vs_float_bool, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__char__", vs_float_char, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__int__", vs_float_int, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__float__", vs_float_float, 1, false);
}
