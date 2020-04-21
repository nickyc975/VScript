#include "objects/VSCharObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

VSObject *vs_char(VSObject *obj) {
    VSObject *val = CALL_ATTR(obj, "__char__", vs_tuple_pack(0));
    if (!VS_IS_TYPE(val, T_CHAR)) {
        err("%s.__char__() returned \"%s\" instead of char.", TYPE_STR[obj->type], TYPE_STR[val->type]);
        terminate(TERM_ERROR);
    }

    INCREF_RET(val);
}

VSObject *vs_char_hash(const VSObject *charobj) {
    VS_ENSURE_TYPE(charobj, T_CHAR, "char.__hash__()");

    INCREF_RET(C_INT_TO_INT((((VSCharObject *)charobj)->_value)));
}

VSObject *vs_char_lt(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_BOOL, "char.__lt__()");
    VS_ENSURE_TYPE(b, T_BOOL, "char.__lt__()");

    cbool_t res = ((VSCharObject *)a)->_value < ((VSCharObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_eq(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_BOOL, "char.__eq__()");
    VS_ENSURE_TYPE(b, T_BOOL, "char.__eq__()");

    bool res = ((VSCharObject *)a)->_value == ((VSCharObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_str(VSObject *charobj) {
    VS_ENSURE_TYPE(charobj, T_CHAR, "char.__str__()");

    auto str = std::string();
    str.push_back(((VSCharObject *)charobj)->_value);
    INCREF_RET(C_STRING_TO_STRING((str)));
}

VSObject *vs_char_bytes(VSObject *charobj) {
    VS_ENSURE_TYPE(charobj, T_CHAR, "char.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_char_neg(VSObject *charobj) {
    VS_ENSURE_TYPE(charobj, T_CHAR, "char.__neg__()");

    cchar_t res = -((VSCharObject *)charobj)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_add(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_CHAR, "char.__add__()");
    VS_ENSURE_TYPE(b, T_CHAR, "char.__add__()");

    cchar_t res = ((VSCharObject *)a)->_value + ((VSCharObject *)b)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_sub(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_CHAR, "char.__sub__()");
    VS_ENSURE_TYPE(b, T_CHAR, "char.__sub__()");

    cchar_t res = ((VSCharObject *)a)->_value - ((VSCharObject *)b)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_mul(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_CHAR, "char.__mul__()");
    VS_ENSURE_TYPE(b, T_CHAR, "char.__mul__()");

    cchar_t res = ((VSCharObject *)a)->_value * ((VSCharObject *)b)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_div(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_CHAR, "char.__div__()");
    VS_ENSURE_TYPE(b, T_CHAR, "char.__div__()");

    if (((VSCharObject *)b)->_value == 0) {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cchar_t res = ((VSCharObject *)a)->_value / ((VSCharObject *)b)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_mod(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_CHAR, "char.__mod__()");
    VS_ENSURE_TYPE(b, T_CHAR, "char.__mod__()");

    if (((VSCharObject *)b)->_value == 0) {
        err("mod by zero\n");
        terminate(TERM_ERROR);
    }

    cchar_t res = ((VSCharObject *)a)->_value % ((VSCharObject *)b)->_value;
    INCREF_RET(C_CHAR_TO_CHAR(res));
}

VSObject *vs_char_bool(VSObject *charobj) {
    VS_ENSURE_TYPE(charobj, T_CHAR, "char.__bool__()");

    cchar_t val = ((VSCharObject *)charobj)->_value;
    INCREF_RET(val ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_char(VSObject *charobj) {
    VS_ENSURE_TYPE(charobj, T_CHAR, "char.__char__()");

    INCREF_RET(charobj);
}

VSObject *vs_char_int(VSObject *charobj) {
    VS_ENSURE_TYPE(charobj, T_CHAR, "char.__int__()");

    INCREF_RET(C_INT_TO_INT((cint_t)CHAR_TO_C_CHAR(charobj)));
}

VSObject *vs_char_float(VSObject *charobj) {
    VS_ENSURE_TYPE(charobj, T_CHAR, "char.__float__()");

    INCREF_RET(C_FLOAT_TO_FLOAT(((cfloat_t)CHAR_TO_C_CHAR(charobj))));
}

VSCharObject::VSCharObject(cchar_t value) : _value(value) {
    this->type = T_CHAR;

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_char_hash, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "__lt__", vs_char_lt, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_char_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_char_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_char_bytes, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__neg__", vs_char_neg, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__add__", vs_char_add, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__sub__", vs_char_sub, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__mul__", vs_char_mul, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__div__", vs_char_div, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__mod__", vs_char_mod, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bool__", vs_char_bool, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__char__", vs_char_char, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__int__", vs_char_int, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__float__", vs_char_float, 1, false);
}

VSCharObject::~VSCharObject() {
}