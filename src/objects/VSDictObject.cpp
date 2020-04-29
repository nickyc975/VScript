#include "objects/VSDictObject.hpp"

#include "error.hpp"
#include "objects/VSStringObject.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);
NEW_IDENTIFIER(copy);
NEW_IDENTIFIER(clear);
NEW_IDENTIFIER(len);
NEW_IDENTIFIER(get);
NEW_IDENTIFIER(set);
NEW_IDENTIFIER(has_at);
NEW_IDENTIFIER(remove_at);

VSObject *vs_dict(VSObject *, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("dict()", 0, nargs);
        terminate(TERM_ERROR);
    }

    INCREF_RET(new VSDictObject());
}

VSObject *vs_dict_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("dict.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_DICT, "dict.__str__()");

    std::string dict_str = "{";
    VSDictObject *dict = (VSDictObject *)self;
    for (auto entry : dict->_dict) {
        VSObject *str = CALL_ATTR(entry.first, ID___str__, EMPTY_TUPLE());
        dict_str.append(STRING_TO_C_STRING(str));
        DECREF_EX(str);

        dict_str.append(": ");

        str = CALL_ATTR(entry.second, ID___str__, EMPTY_TUPLE());
        dict_str.append(STRING_TO_C_STRING(str));
        DECREF_EX(str);

        dict_str.append(", ");
    }
    if (dict_str.back() == ' ') {
        dict_str.pop_back();
        dict_str.pop_back();
    }
    dict_str.append("}");

    INCREF_RET(C_STRING_TO_STRING(dict_str));
}

VSObject *vs_dict_bytes(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("dict.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_DICT, "dict.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_dict_copy(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("dict.copy()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_DICT, "dict.copy()");

    VSDictObject *dict = (VSDictObject *)self;
    VSDictObject *new_dict = new VSDictObject();
    for (auto entry : dict->_dict) {
        new_dict->_dict[entry.first] = entry.second;
        INCREF(entry.first);
        INCREF(entry.second);
    }
    INCREF_RET(AS_OBJECT(new_dict));
}

VSObject *vs_dict_clear(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("dict.clear()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_DICT, "dict.clear()");

    VSDictObject *dict = (VSDictObject *)self;
    for (auto entry : dict->_dict) {
        // entry.first is not modifiable, so just deleting it but not setting the pointer to NULL,
        // which means there will be DANGLING POINTERS!!!
        DECREF(entry.first);
        DECREF_EX(entry.second);
    }
    dict->_dict.clear();
    INCREF_RET(VS_NONE);
}

VSObject *vs_dict_len(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("dict.len()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_DICT, "dict.len()");

    INCREF_RET(
        C_INT_TO_INT(
            ((VSDictObject *)self)->_dict.size()));
}

VSObject *vs_dict_get(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("dict.get()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_DICT, "dict.get()");

    VSObject *key = args[0];
    VSDictObject *dict = (VSDictObject *)self;
    auto iter = dict->_dict.find(key);
    if (iter != dict->_dict.end()) {
        INCREF_RET(iter->second);
    } else {
        VSObject *strobj = CALL_ATTR(key, ID___str__, EMPTY_TUPLE());
        ENSURE_TYPE(strobj, T_STR, "as __str__() result");

        err("key \"%s\" not found.", STRING_TO_C_STRING(strobj).c_str());
        terminate(TERM_ERROR);
    }
    return NULL;
}

VSObject *vs_dict_set(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 2) {
        ERR_NARGS("dict.set()", 2, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_DICT, "dict.set()");

    VSObject *key = args[0];
    VSObject *value = args[1];
    VSDictObject *dict = (VSDictObject *)self;
    auto iter = dict->_dict.find(key);
    if (iter != dict->_dict.end()) {
        iter->second = value;
        INCREF(value);
    } else {
        dict->_dict[key] = value;
        INCREF(key);
        INCREF(value);
    }
    INCREF_RET(VS_NONE);
}

VSObject *vs_dict_has_at(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("dict.has_at()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_DICT, "dict.has_at()");

    VSObject *key = args[0];
    VSDictObject *dict = (VSDictObject *)self;
    auto iter = dict->_dict.find(key);
    INCREF_RET(C_BOOL_TO_BOOL(iter != dict->_dict.end()));
}

VSObject *vs_dict_remove_at(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("dict.remove_at()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_DICT, "dict.remove_at()");

    VSObject *key = args[0];
    VSDictObject *dict = (VSDictObject *)self;
    auto iter = dict->_dict.find(key);
    if (iter != dict->_dict.end()) {
        DECREF(iter->first);
        DECREF_EX(iter->second);
        dict->_dict.erase(iter);
    }
    INCREF_RET(VS_NONE);
}

const str_func_map VSDictObject::vs_dict_methods = {
    {ID___hash__, vs_default_hash},
    {ID___eq__, vs_default_eq},
    {ID___str__, vs_dict_str},
    {ID___bytes__, vs_dict_bytes},
    {ID_copy, vs_dict_copy},
    {ID_clear, vs_dict_clear},
    {ID_len, vs_dict_len},
    {ID_get, vs_dict_get},
    {ID_set, vs_dict_set},
    {ID_has_at, vs_dict_has_at},
    {ID_remove_at, vs_dict_remove_at}
};

VSDictObject::VSDictObject() {
    this->type = T_DICT;
    this->_dict = std::unordered_map<VSObject *, VSObject *, __dict_hash__, __dict_equal_to__>();
}

VSDictObject::~VSDictObject() {
    DECREF(vs_dict_clear(this, NULL, 0));
}

bool VSDictObject::hasattr(std::string &attrname) {
    return vs_dict_methods.find(attrname) != vs_dict_methods.end();
}

VSObject *VSDictObject::getattr(std::string &attrname) {
    auto iter = vs_dict_methods.find(attrname);
    if (iter == vs_dict_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_dict_methods.at(attrname));
    INCREF_RET(attr);
}

void VSDictObject::setattr(std::string &attrname, VSObject *attrvalue) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}