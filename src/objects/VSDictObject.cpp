#include "objects/VSDictObject.hpp"

#include "error.hpp"
#include "objects/VSStringObject.hpp"

VSObject *vs_dict() {
    INCREF_RET(new VSDictObject());
}

VSObject *vs_dict_str(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_DICT, "dict.__str__()");

    INCREF_RET(C_STRING_TO_STRING("dict"));
}

VSObject *vs_dict_bytes(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_DICT, "dict.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_dict_copy(const VSObject *that) {
    VS_ENSURE_TYPE(that, T_DICT, "dict.copy()");

    VSDictObject *dict = (VSDictObject *)that;
    VSDictObject *new_dict = new VSDictObject();
    for (auto entry : dict->_dict) {
        new_dict->_dict[entry.first] = entry.second;
        INCREF(entry.first);
        INCREF(entry.second);
    }
    INCREF_RET(VS_AS_OBJECT(new_dict));
}

VSObject *vs_dict_clear(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_DICT, "dict.clear()");

    VSDictObject *dict = (VSDictObject *)obj;
    for (auto entry : dict->_dict) {
        // entry.first is not modifiable, so just deleting it but not setting the pointer to NULL,
        // which means there will be DANGLING POINTERS!!!
        DECREF(entry.first);
        DECREF_EX(entry.second);
    }
    dict->_dict.clear();
}

VSObject *vs_dict_len(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_DICT, "dict.len()");

    INCREF_RET(
        C_INT_TO_INT(
            ((VSDictObject *)obj)->_dict.size()));
}

VSObject *vs_dict_get(VSObject *obj, VSObject *key) {
    VS_ENSURE_TYPE(obj, T_DICT, "dict.get()");

    VSDictObject *dict = (VSDictObject *)obj;
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

VSObject *vs_dict_set(VSObject *obj, VSObject *key, VSObject *value) {
    VS_ENSURE_TYPE(obj, T_DICT, "dict.set()");

    VSDictObject *dict = (VSDictObject *)obj;
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

VSObject *vs_dict_has_at(VSObject *obj, VSObject *key) {
    VS_ENSURE_TYPE(obj, T_DICT, "dict.has_at()");

    VSDictObject *dict = (VSDictObject *)obj;
    auto iter = dict->_dict.find(key);
    INCREF_RET(C_BOOL_TO_BOOL(iter != dict->_dict.end()));
}

VSObject *vs_dict_remove_at(VSObject *obj, VSObject *key) {
    VS_ENSURE_TYPE(obj, T_DICT, "dict.remove_at()");

    VSDictObject *dict = (VSDictObject *)obj;
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

    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_default_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_dict_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_dict_bytes, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "copy", vs_dict_copy, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "clear", vs_dict_clear, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "len", vs_dict_len, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "get", vs_dict_get, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "set", vs_dict_set, 3, false);
    NEW_NATIVE_FUNC_ATTR(this, "has_at", vs_dict_has_at, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "remove_at", vs_dict_remove_at, 2, false);
}

VSDictObject::~VSDictObject() {
    DECREF(vs_dict_clear(this));
}