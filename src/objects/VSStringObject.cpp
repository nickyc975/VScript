#include "objects/VSStringObject.hpp"

#include <errno.h>

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSTupleObject.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__lt__);
NEW_IDENTIFIER(__gt__);
NEW_IDENTIFIER(__le__);
NEW_IDENTIFIER(__ge__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);
NEW_IDENTIFIER(__add__);
NEW_IDENTIFIER(__bool__);
NEW_IDENTIFIER(__char__);
NEW_IDENTIFIER(__int__);
NEW_IDENTIFIER(__float__);
NEW_IDENTIFIER(copy);
NEW_IDENTIFIER(clear);
NEW_IDENTIFIER(len);
NEW_IDENTIFIER(get);
NEW_IDENTIFIER(set);
NEW_IDENTIFIER(append);
NEW_IDENTIFIER(has);
NEW_IDENTIFIER(has_at);
NEW_IDENTIFIER(remove);
NEW_IDENTIFIER(remove_at);
NEW_IDENTIFIER(split);
NEW_IDENTIFIER(substr);
NEW_IDENTIFIER(locate);

VSObject *vs_str(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs == 0) {
        INCREF_RET(new VSStringObject(""));
    } else if (nargs == 1) {
        VSObject *obj = args[0];
        VSObject *val = CALL_ATTR(obj, ID___str__, EMPTY_TUPLE());
        if (!IS_TYPE(val, T_STR)) {
            err("%s.__str__() returned \"%s\" instead of str.", TYPE_STR[obj->type], TYPE_STR[val->type]);
            terminate(TERM_ERROR);
        }

        INCREF_RET(val);
    }

    ERR_NARGS("str()", 1, nargs);
    terminate(TERM_ERROR);
    return NULL;
}

VSObject *vs_string_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("str.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_STR, "str.__hash__()");

    std::size_t hash = std::hash<std::string>{}(((VSStringObject *)self)->_value);
    INCREF_RET(C_INT_TO_INT(hash));
}

VSObject *vs_string_lt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.__lt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_STR, "str.__lt__()");
    ENSURE_TYPE(that, T_STR, "str.__lt__()");

    cbool_t res = ((VSStringObject *)self)->_value < ((VSStringObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_gt(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.__gt__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_STR, "str.__gt__()");
    ENSURE_TYPE(that, T_STR, "str.__gt__()");

    cbool_t res = ((VSStringObject *)self)->_value > ((VSStringObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_le(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.__le__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_STR, "str.__le__()");
    ENSURE_TYPE(that, T_STR, "str.__le__()");

    cbool_t res = ((VSStringObject *)self)->_value <= ((VSStringObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_ge(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.__ge__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_STR, "str.__ge__()");
    ENSURE_TYPE(that, T_STR, "str.__ge__()");

    cbool_t res = ((VSStringObject *)self)->_value >= ((VSStringObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.__eq__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_STR, "str.__eq__()");
    ENSURE_TYPE(that, T_STR, "str.__eq__()");

    cbool_t res = ((VSStringObject *)self)->_value == ((VSStringObject *)that)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("str.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_STR, "str.__str__()");

    std::string &str_val = STRING_TO_C_STRING(self);
    INCREF_RET(C_STRING_TO_STRING(str_val));
}

VSObject *vs_string_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("str.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_STR, "str.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_string_add(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.__add__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_STR, "str.__add__()");
    ENSURE_TYPE(that, T_STR, "str.__add__()");

    std::string res = ((VSStringObject *)self)->_value + ((VSStringObject *)that)->_value;
    INCREF_RET(C_STRING_TO_STRING(res));
}

VSObject *vs_string_bool(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("str.__bool__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_STR, "str.__bool__()");

    vs_size_t len = STRING_LEN(self);
    INCREF_RET(len ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_char(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("str.__char__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_STR, "str.__char__()");

    VSStringObject *str = (VSStringObject *)self;
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

VSObject *vs_string_int(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs > 1) {
        ERR_NARGS("str.__int__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_STR, "str.__int__()");

    int base = 0;
    if (nargs == 1) {
        VSObject *baseobj = args[0];
        ENSURE_TYPE(baseobj, T_INT, "str.__int__()");

        base = (int)INT_TO_C_INT(baseobj);
        if (base < 0 || base == 1 || base > 36) {
            err("invalid int base: %d for str.__int__()", base);
            terminate(TERM_ERROR);
        }
    }

    char *end = NULL;
    VSStringObject *str = (VSStringObject *)self;
    cint_t val = std::strtoll(str->_value.c_str(), &end, base);

    if (errno == ERANGE) {
        err("literal out of range of int: \"%s\" in str.__int__()", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    if (end - 1 != &(str->_value.back())) {
        err("invalid literal: \"%s\" in str.__int__()", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_INT_TO_INT(val));
}

VSObject *vs_string_float(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("str.__float__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_STR, "str.__float__()");

    char *end = NULL;
    VSStringObject *str = (VSStringObject *)self;
    cfloat_t val = std::strtold(str->_value.c_str(), &end);

    if (errno == ERANGE) {
        errno = 0;
        err("literal out of range of float: \"%s\" in str.__float__()", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    if (end - 1 != &(str->_value.back())) {
        err("invalid literal: \"%s\" in str.__float__()", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_FLOAT_TO_FLOAT(val));
}

VSObject *vs_string_copy(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("str.copy()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_STR, "str.copy()");

    VSStringObject *new_str = new VSStringObject(STRING_TO_C_STRING(self));
    INCREF_RET(new_str);
}

VSObject *vs_string_clear(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("str.clear()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_STR, "str.clear()");

    ((VSStringObject *)self)->_value.clear();
    INCREF_RET(VS_NONE);
}

VSObject *vs_string_len(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("str.len()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_STR, "str.len()");

    INCREF_RET(
        C_INT_TO_INT(
            ((VSStringObject *)self)->_value.length()));
}

VSObject *vs_string_get(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.get()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    ENSURE_TYPE(self, T_STR, "str.get()");
    ENSURE_TYPE(idxobj, T_INT, "as str index");

    VSStringObject *str = (VSStringObject *)self;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    if (idx >= str->_value.size()) {
        INDEX_OUT_OF_BOUND(idx, str->_value.size());
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_CHAR_TO_CHAR(str->_value.at(idx)));
}

VSObject *vs_string_set(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 2) {
        ERR_NARGS("str.set()", 2, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    VSObject *charobj = args[1];
    ENSURE_TYPE(self, T_STR, "str.set()");
    ENSURE_TYPE(idxobj, T_INT, "as str index");
    ENSURE_TYPE(charobj, T_CHAR, "as string content");

    VSStringObject *str = (VSStringObject *)self;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);
    cchar_t char_val = CHAR_TO_C_CHAR(charobj);

    if (idx >= str->_value.size()) {
        INDEX_OUT_OF_BOUND(idx, str->_value.size());
        terminate(TERM_ERROR);
    }

    str->_value[idx] = char_val;
    INCREF_RET(VS_NONE);
}

VSObject *vs_string_append(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.append()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *charobj = args[0];
    ENSURE_TYPE(self, T_STR, "str.append()");
    ENSURE_TYPE(charobj, T_CHAR, "as string content");

    VSStringObject *str = (VSStringObject *)self;
    char char_val = CHAR_TO_C_CHAR(charobj);

    str->_value.push_back(char_val);
    INCREF_RET(VS_NONE);
}

VSObject *vs_string_has(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.has()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *charobj = args[0];
    ENSURE_TYPE(self, T_STR, "str.has()");
    ENSURE_TYPE(charobj, T_CHAR, "as string content");

    VSStringObject *str = (VSStringObject *)self;
    char char_val = CHAR_TO_C_CHAR(charobj);

    if (str->_value.find(char_val) != str->_value.npos) {
        INCREF_RET(VS_TRUE);
    }
    INCREF_RET(VS_FALSE);
}

VSObject *vs_string_has_at(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.has_at()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    ENSURE_TYPE(self, T_STR, "str.has_at()");
    ENSURE_TYPE(idxobj, T_INT, "as string index");

    VSStringObject *str = (VSStringObject *)self;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    INCREF_RET(idx < str->_value.size() ? VS_FALSE : VS_TRUE);
}

VSObject *vs_string_remove(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.remove()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *charobj = args[0];
    ENSURE_TYPE(self, T_STR, "str.remove()");
    ENSURE_TYPE(charobj, T_CHAR, "as string content");

    VSStringObject *str = (VSStringObject *)self;
    char char_val = CHAR_TO_C_CHAR(charobj);

    std::size_t idx = str->_value.find_first_of(char_val);
    while (idx != str->_value.npos) {
        str->_value.erase(idx, 1);
        idx = str->_value.find_first_of(char_val, idx);
    }
    INCREF_RET(VS_NONE);
}

VSObject *vs_string_remove_at(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.remove_at()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    ENSURE_TYPE(self, T_STR, "str.remove_at()");
    ENSURE_TYPE(idxobj, T_INT, "as string index");

    VSStringObject *str = (VSStringObject *)self;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    if (idx >= str->_value.size()) {
        INDEX_OUT_OF_BOUND(idx, str->_value.size());
        terminate(TERM_ERROR);
    }

    str->_value.erase(idx, 1);
    INCREF_RET(VS_NONE);
}

VSObject *vs_string_split(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.split()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *sepobj = args[0];
    ENSURE_TYPE(self, T_STR, "str.split()");
    ENSURE_TYPE(sepobj, T_STR, "as seperator of str.split()");

    std::string &str = ((VSStringObject *)self)->_value;
    std::string &sep = ((VSStringObject *)sepobj)->_value;

    std::size_t sep_len = sep.length();
    std::size_t start = 0, end = str.find(sep, start);

    VSListObject *res = new VSListObject(0);
    while (end != str.npos)
    {
        LIST_APPEND(res, C_STRING_TO_STRING(str.substr(start, end - start)));
        start = end + sep_len;
        end = str.find(sep, start);
    }
    
    if (start < str.length()) {
        LIST_APPEND(res, C_STRING_TO_STRING(str.substr(start, str.length() - start)));
    }

    INCREF_RET(res);
}

VSObject *vs_string_substr(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 2) {
        ERR_NARGS("str.substr()", 2, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *startobj = args[0];
    VSObject *lengthobj = args[1];
    ENSURE_TYPE(self, T_STR, "str.substr()");
    ENSURE_TYPE(startobj, T_INT, "as start pos of str.substr()");
    ENSURE_TYPE(lengthobj, T_INT, "as length of str.substr()");

    std::string &str = ((VSStringObject *)self)->_value;
    cint_t start = ((VSIntObject *)startobj)->_value;
    cint_t length = ((VSIntObject *)lengthobj)->_value;

    if (start < 0 || ((size_t)start) >= str.length()) {
        err("invalid substr start pos: %lld in str.substr()", start);
        terminate(TERM_ERROR);
    }

    if (length < 0) {
        err("invalid substr length: %lld in str.substr()", length);
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_STRING_TO_STRING(str.substr(start, length)));
}

VSObject *vs_string_locate(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("str.locate()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *contentobj = args[0];
    ENSURE_TYPE(self, T_STR, "str.locate()");

    cint_t pos = -1;
    std::string &str = ((VSStringObject *)self)->_value;
    if (contentobj->type == T_CHAR) {
        cchar_t char_val = ((VSCharObject *)contentobj)->_value;
        pos = str.find(char_val);
    } else if (contentobj->type == T_STR) {
        std::string &str_val = ((VSStringObject *)contentobj)->_value;
        pos = str.find(str_val);
    } else {
        err("Can not apply \"as string content\" on type \"%s\".", TYPE_STR[contentobj->type]); 
        terminate(TERM_ERROR);   
    }

    pos = ((size_t)pos) == str.npos ? -1 : pos;
    INCREF_RET(C_INT_TO_INT(pos));
}

const str_func_map VSStringObject::vs_str_methods = {
    {ID___hash__, vs_string_hash},
    {ID___lt__, vs_string_lt},
    {ID___gt__, vs_string_gt},
    {ID___le__, vs_string_le},
    {ID___ge__, vs_string_ge},
    {ID___eq__, vs_string_eq},
    {ID___str__, vs_string_str},
    {ID___bytes__, vs_string_bytes},
    {ID___add__, vs_string_add},
    {ID___bool__, vs_string_bool},
    {ID___char__, vs_string_char},
    {ID___int__, vs_string_int},
    {ID___float__, vs_string_float},
    {ID_copy, vs_string_copy},
    {ID_clear, vs_string_clear},
    {ID_len, vs_string_len},
    {ID_get, vs_string_get},
    {ID_set, vs_string_set},
    {ID_append, vs_string_append},
    {ID_has, vs_string_has},
    {ID_has_at, vs_string_has_at},
    {ID_remove, vs_string_remove},
    {ID_remove_at, vs_string_remove_at},
    {ID_split, vs_string_split},
    {ID_substr, vs_string_substr},
    {ID_locate, vs_string_locate}
};

VSStringObject::VSStringObject(std::string value) {
    this->type = T_STR;
    this->_value = value;
}

VSStringObject::~VSStringObject() {
}

bool VSStringObject::hasattr(std::string &attrname) {
    return vs_str_methods.find(attrname) != vs_str_methods.end();
}

VSObject *VSStringObject::getattr(std::string &attrname) {
    auto iter = vs_str_methods.find(attrname);
    if (iter == vs_str_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_str_methods.at(attrname));
    INCREF_RET(attr);
}

void VSStringObject::setattr(std::string &, VSObject *) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}