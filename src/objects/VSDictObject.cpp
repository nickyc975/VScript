#include "objects/VSDictObject.hpp"

#include "error.hpp"
#include "objects/VSStringObject.hpp"

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

    VS_ENSURE_TYPE(self, T_DICT, "dict.__str__()");

    std::string dict_str = "{";
    VSDictObject *dict = (VSDictObject *)self;
    for (auto entry : dict->_dict) {
        VSObject *str = CALL_ATTR(entry.first, "__str__", vs_tuple_pack(0));
        dict_str += STRING_TO_C_STRING(str);
        DECREF_EX(str);

        dict_str += ": ";

        str = CALL_ATTR(entry.second, "__str__", vs_tuple_pack(0));
        dict_str += STRING_TO_C_STRING(str);
        DECREF_EX(str);

        dict_str += ", ";
    }
    if (dict_str.back() == ' ') {
        dict_str.pop_back();
        dict_str.pop_back();
    }
    dict_str += "}";

    INCREF_RET(C_STRING_TO_STRING(dict_str));
}

VSObject *vs_dict_bytes(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("dict.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_DICT, "dict.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_dict_copy(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("dict.copy()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_DICT, "dict.copy()");

    VSDictObject *dict = (VSDictObject *)self;
    VSDictObject *new_dict = new VSDictObject();
    for (auto entry : dict->_dict) {
        new_dict->_dict[entry.first] = entry.second;
        INCREF(entry.first);
        INCREF(entry.second);
    }
    INCREF_RET(VS_AS_OBJECT(new_dict));
}

VSObject *vs_dict_clear(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("dict.clear()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_DICT, "dict.clear()");

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

    VS_ENSURE_TYPE(self, T_DICT, "dict.len()");

    INCREF_RET(
        C_INT_TO_INT(
            ((VSDictObject *)self)->_dict.size()));
}

VSObject *vs_dict_get(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("dict.get()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_DICT, "dict.get()");

    VSObject *key = args[0];
    VSDictObject *dict = (VSDictObject *)self;
    auto iter = dict->_dict.find(key);
    if (iter != dict->_dict.end()) {
        INCREF_RET(iter->second);
    } else {
        VSObject *strobj = CALL_ATTR(key, "__str__", vs_tuple_pack(0));
        VS_ENSURE_TYPE(strobj, T_STR, "as __str__() result");

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

    VS_ENSURE_TYPE(self, T_DICT, "dict.set()");

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

    VS_ENSURE_TYPE(self, T_DICT, "dict.has_at()");

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

    VS_ENSURE_TYPE(self, T_DICT, "dict.remove_at()");

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

VSDictObject::VSDictObject() {
    this->type = T_DICT;
    this->_dict = std::unordered_map<VSObject *, VSObject *, __dict_hash__, __dict_equal_to__>();

    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_default_eq);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_dict_str);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_dict_bytes);
    NEW_NATIVE_FUNC_ATTR(this, "copy", vs_dict_copy);
    NEW_NATIVE_FUNC_ATTR(this, "clear", vs_dict_clear);
    NEW_NATIVE_FUNC_ATTR(this, "len", vs_dict_len);
    NEW_NATIVE_FUNC_ATTR(this, "get", vs_dict_get);
    NEW_NATIVE_FUNC_ATTR(this, "set", vs_dict_set);
    NEW_NATIVE_FUNC_ATTR(this, "has_at", vs_dict_has_at);
    NEW_NATIVE_FUNC_ATTR(this, "remove_at", vs_dict_remove_at);
}

VSDictObject::~VSDictObject() {
    DECREF(vs_dict_clear(this, NULL, 0));
}