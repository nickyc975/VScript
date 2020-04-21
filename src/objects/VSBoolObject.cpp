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

VSObject *vs_bool(VSObject *obj) {
    VSObject *val = CALL_ATTR(obj, "__bool__", vs_tuple_pack(0));
    if (!VS_IS_TYPE(val, T_BOOL)) {
        err("%s.__bool__() returned \"%s\" instead of bool.", TYPE_STR[obj->type], TYPE_STR[val->type]);
        terminate(TERM_ERROR);
    }

    INCREF_RET(val);
}

VSObject *vs_bool_hash(const VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_BOOL, "bool.__hash__()");
    INCREF_RET(C_INT_TO_INT((cint_t)BOOL_TO_C_BOOL(obj)));
}

VSObject *vs_bool_lt(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_BOOL, "bool.__lt__()");
    VS_ENSURE_TYPE(b, T_BOOL, "bool.__lt__()");

    bool res = ((VSBoolObject *)a)->_value < ((VSBoolObject *)b)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_eq(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_BOOL, "bool.__eq__()");
    VS_ENSURE_TYPE(b, T_BOOL, "bool.__eq__()");

    bool res = ((VSBoolObject *)a)->_value == ((VSBoolObject *)b)->_value;
    INCREF_RET(C_BOOL_TO_BOOL(res));
}

VSObject *vs_bool_str(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_BOOL, "bool.__str__()");

    INCREF_RET(C_STRING_TO_STRING(BOOL_TO_C_BOOL(obj) ? "true" : "false"));
}

VSObject *vs_bool_bytes(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_BOOL, "bool.__bytes__()");
    INCREF_RET(VS_NONE);
}

VSObject *vs_bool_not(VSObject *boolobj) {
    VS_ENSURE_TYPE(boolobj, T_BOOL, "bool.__not__()");

    INCREF_RET(
        C_BOOL_TO_BOOL(
            !BOOL_TO_C_BOOL(boolobj)));
}

VSObject *vs_bool_and(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_BOOL, "bool.__and__()");
    VS_ENSURE_TYPE(b, T_BOOL, "bool.__and__()");

    VSBoolObject *boola = (VSBoolObject *)a;
    VSBoolObject *boolb = (VSBoolObject *)b;

    INCREF_RET(C_BOOL_TO_BOOL(boola->_value && boolb->_value));
}

VSObject *vs_bool_or(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_BOOL, "bool.__or__()");
    VS_ENSURE_TYPE(b, T_BOOL, "bool.__or__()");

    VSBoolObject *boola = (VSBoolObject *)a;
    VSBoolObject *boolb = (VSBoolObject *)b;

    INCREF_RET(C_BOOL_TO_BOOL(boola->_value || boolb->_value));
}

VSObject *vs_bool_xor(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_BOOL, "bool.__xor__()");
    VS_ENSURE_TYPE(b, T_BOOL, "bool.__xor__()");

    VSBoolObject *boola = (VSBoolObject *)a;
    VSBoolObject *boolb = (VSBoolObject *)b;

    INCREF_RET(C_BOOL_TO_BOOL(boola->_value ^ boolb->_value));
}

VSObject *vs_bool_bool(VSObject *boolobj) {
    VS_ENSURE_TYPE(boolobj, T_BOOL, "bool.__bool__()");

    INCREF_RET(boolobj);
}

VSObject *vs_bool_char(VSObject *boolobj) {
    VS_ENSURE_TYPE(boolobj, T_BOOL, "bool.__char__()");

    INCREF_RET(C_CHAR_TO_CHAR((cchar_t)BOOL_TO_C_BOOL(boolobj)));
}

VSObject *vs_bool_int(VSObject *boolobj) {
    VS_ENSURE_TYPE(boolobj, T_BOOL, "bool.__int__()");

    INCREF_RET(C_INT_TO_INT((cint_t)BOOL_TO_C_BOOL(boolobj)));
}

VSObject *vs_bool_float(VSObject *boolobj) {
    VS_ENSURE_TYPE(boolobj, T_BOOL, "bool.__float__()");

    INCREF_RET(C_FLOAT_TO_FLOAT((cfloat_t)BOOL_TO_C_BOOL(boolobj)));
}

VSBoolObject::VSBoolObject(cbool_t value) : _value(value) {
    this->type = T_BOOL;

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_bool_hash, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "__lt__", vs_bool_lt, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_bool_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_bool_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_bool_bytes, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__not__", vs_bool_not, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__and__", vs_bool_and, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__or__", vs_bool_or, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__xor__", vs_bool_xor, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bool__", vs_bool_bool, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__char__", vs_bool_char, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__int__", vs_bool_int, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__float__", vs_bool_float, 1, false);
}