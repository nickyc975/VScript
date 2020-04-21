#include "objects/VSStringObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSTupleObject.hpp"

VSObject *vs_string(VSObject *obj) {
    if (!HAS_ATTR(obj, "__str__")) {
        ERR_ATTR_MISSING(obj, "__str__");
        terminate(TERM_ERROR);
    }

    VSObject *str_func = GET_ATTR(obj, "__str__");
    if (!VS_IS_TYPE(str_func, T_FUNC)) {
        ERR_ATTR_IS_NOT_FUNC(obj, "__str__");
        terminate(TERM_ERROR);
    }

    VSObject *val = ((VSFunctionObject *)str_func)->call(vs_tuple_pack(0));
    if (!VS_IS_TYPE(val, T_STR)) {
        err("%s.__str__() returned \"%s\" instead of str.", TYPE_STR[obj->type], TYPE_STR[val->type]);
        terminate(TERM_ERROR);
    }

    INCREF_RET(val);
}

VSObject *vs_string_hash(const VSObject *strobj) {
    VS_ENSURE_TYPE(strobj, T_STR, "str.__hash__()");

    std::size_t hash = std::hash<std::string>{}(((VSStringObject *)strobj)->_value);
    INCREF_RET(C_INT_TO_INT(hash));
}

VSObject *vs_string_lt(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_STR, "str.__lt__()");
    VS_ENSURE_TYPE(b, T_STR, "str.__lt__()");

    cbool_t res = ((VSStringObject *)a)->_value < ((VSStringObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_eq(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_STR, "str.__eq__()");
    VS_ENSURE_TYPE(b, T_STR, "str.__eq__()");

    VSStringObject *astr = (VSStringObject *)a;
    VSStringObject *bstr = (VSStringObject *)b;

    INCREF_RET(astr->_value == bstr->_value ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_str(VSObject *strobj) {
    VS_ENSURE_TYPE(strobj, T_STR, "str.__str__()");

    INCREF_RET(strobj);
}

VSObject *vs_string_bytes(VSObject *strobj) {
    VS_ENSURE_TYPE(strobj, T_STR, "str.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_string_add(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_STR, "str.__add__()");
    VS_ENSURE_TYPE(b, T_STR, "str.__add__()");

    std::string res = ((VSStringObject *)a)->_value + ((VSStringObject *)b)->_value;
    INCREF_RET(C_STRING_TO_STRING(res));
}

VSObject *vs_string_bool(VSObject *strobj) {
    VS_ENSURE_TYPE(strobj, T_STR, "str.__bool__()");

    vs_size_t len = STRING_LEN(strobj);
    INCREF_RET(len ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_char(VSObject *strobj) {
    VS_ENSURE_TYPE(strobj, T_STR, "str.__char__()");

    VSStringObject *str = (VSStringObject *)strobj;
    vs_size_t len = str->_value.length();
    if (len == 0) {
        INCREF_RET(
            C_CHAR_TO_CHAR(
                (cchar_t)0));
    } else if (len == 1) {
        INCREF_RET(
            C_CHAR_TO_CHAR(
                (cchar_t)str->_value[0]));
    }

    err("Can not cast string \"%s\" to char", str->_value.c_str());
    terminate(TERM_ERROR);
    INCREF_RET(VS_NONE);
}

VSObject *vs_string_int(VSObject *strobj, VSObject *baseobj) {
    VS_ENSURE_TYPE(strobj, T_STR, "str.__int__()");

    int base = 0;
    if (baseobj != NULL) {
        VS_ENSURE_TYPE(baseobj, T_INT, "str.__int__()");

        base = (int)INT_TO_C_INT(baseobj);
        if (base < 0 || base == 1 || base > 36) {
            err("invalid int base: %d", base);
            terminate(TERM_ERROR);
        }
    }

    char *end = NULL;
    VSStringObject *str = (VSStringObject *)strobj;
    cint_t val = std::strtoll(str->_value.c_str(), &end, base);

    if (errno == ERANGE) {
        err("literal out of range of int: \"%s\"", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    if (end - 1 != &(str->_value.back())) {
        err("invalid literal: \"%s\"", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_INT_TO_INT(val));
}

VSObject *vs_string_float(VSObject *strobj) {
    VS_ENSURE_TYPE(strobj, T_STR, "str.__float__()");

    char *end = NULL;
    VSStringObject *str = (VSStringObject *)strobj;
    cfloat_t val = std::strtold(str->_value.c_str(), &end);

    if (errno == ERANGE) {
        errno = 0;
        err("literal out of range of float: \"%s\"", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    if (end - 1 != &(str->_value.back())) {
        err("invalid literal: \"%s\"", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_FLOAT_TO_FLOAT(val));
}

VSObject *vs_string_len(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_STR, "str.len()");

    INCREF_RET(
        C_INT_TO_INT(
            ((VSStringObject *)obj)->_value.size()));
}

VSObject *vs_string_get(VSObject *strobj, VSObject *idxobj) {
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    VS_ENSURE_TYPE(strobj, T_STR, "str.get()");
    VSStringObject *str = (VSStringObject *)strobj;

    if (idx >= str->_value.size()) {
        INDEX_OUT_OF_BOUND(idx, str->_value.size());
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_CHAR_TO_CHAR(str->_value.at(idx)));
}

VSObject *vs_string_set(VSObject *strobj, VSObject *idxobj, VSObject *charobj) {
    cchar_t char_val = CHAR_TO_C_CHAR(charobj);
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    VS_ENSURE_TYPE(strobj, T_STR, "str.set()");
    VSStringObject *str = (VSStringObject *)strobj;

    if (idx >= str->_value.size()) {
        INDEX_OUT_OF_BOUND(idx, str->_value.size());
        terminate(TERM_ERROR);
    }

    str->_value[idx] = char_val;
    INCREF_RET(VS_NONE);
}

VSObject *vs_string_append(VSObject *strobj, VSObject *charobj) {
    char char_val = CHAR_TO_C_CHAR(charobj);

    VS_ENSURE_TYPE(strobj, T_STR, "str.append()");
    VSStringObject *str = (VSStringObject *)strobj;

    str->_value.push_back(char_val);
    INCREF_RET(VS_NONE);
}

VSObject *vs_string_has(VSObject *strobj, VSObject *charobj) {
    char char_val = CHAR_TO_C_CHAR(charobj);

    VS_ENSURE_TYPE(strobj, T_STR, "str.has()");
    VSStringObject *str = (VSStringObject *)strobj;

    if (str->_value.find(char_val) != str->_value.npos) {
        INCREF_RET(VS_TRUE);
    }
    INCREF_RET(VS_FALSE);
}

VSObject *vs_string_has_at(VSObject *strobj, VSObject *idxobj) {
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    VS_ENSURE_TYPE(strobj, T_STR, "str.has_at()");
    VSStringObject *str = (VSStringObject *)strobj;

    INCREF_RET(idx < str->_value.size() ? VS_FALSE : VS_TRUE);
}

VSObject *vs_string_remove(VSObject *strobj, VSObject *charobj) {
    char char_val = CHAR_TO_C_CHAR(charobj);

    VS_ENSURE_TYPE(strobj, T_STR, "str.remove()");
    VSStringObject *str = (VSStringObject *)strobj;

    std::size_t idx = str->_value.find_first_of(char_val);
    while (idx != str->_value.npos) {
        str->_value.erase(idx, 1);
        idx = str->_value.find_first_of(char_val, idx);
    }
    INCREF_RET(VS_NONE);
}

VSObject *vs_string_remove_at(VSObject *strobj, VSObject *idxobj) {
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    VS_ENSURE_TYPE(strobj, T_STR, "str.remove_at()");
    VSStringObject *str = (VSStringObject *)strobj;

    if (idx >= str->_value.size()) {
        INDEX_OUT_OF_BOUND(idx, str->_value.size());
        terminate(TERM_ERROR);
    }

    str->_value.erase(idx, 1);
    INCREF_RET(VS_NONE);
}

VSStringObject::VSStringObject(std::string value) {
    this->type = T_STR;
    this->_value = value;

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_string_hash, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "__lt__", vs_string_lt, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_string_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_string_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_string_bytes, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__add__", vs_string_add, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bool__", vs_string_bool, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__char__", vs_string_char, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__int__", vs_string_int, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "__float__", vs_string_float, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "len", vs_string_len, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "get", vs_string_get, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "set", vs_string_set, 3, false);
    NEW_NATIVE_FUNC_ATTR(this, "append", vs_string_append, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "has", vs_string_has, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "has_at", vs_string_has_at, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "remove", vs_string_remove, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "remove_at", vs_string_remove_at, 2, false);
}

VSStringObject::~VSStringObject() {
}