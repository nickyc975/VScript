#include "objects/VSSetObject.hpp"

#include "error.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSStringObject.hpp"

VSObject *vs_set(VSObject *obj) {
    if (obj == NULL) {
        INCREF_RET(new VSSetObject());
    } else if (obj->type == T_TUPLE) {
        VSSetObject *set = new VSSetObject();
        for (vs_size_t i = 0; i < TUPLE_LEN(obj); i++) {
            SET_APPEND(set, TUPLE_GET(obj, i));
        }
        INCREF_RET(set);
    } else if (obj->type == T_LIST) {
        VSSetObject *set = new VSSetObject();
        for (vs_size_t i = 0; i < LIST_LEN(obj); i++) {
            SET_APPEND(set, LIST_GET(obj, i));
        }
        INCREF_RET(set);
    } else {
        err("can not cast \"%s\" object to set", TYPE_STR[obj->type]);
        INCREF_RET(VS_NONE);
    }
}

VSObject *vs_set_str(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_SET, "set.__str__()");

    INCREF_RET(C_STRING_TO_STRING("set"));
}

VSObject *vs_set_bytes(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_SET, "set.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_set_copy(const VSObject *that) {
    VS_ENSURE_TYPE(that, T_SET, "set.copy()");

    VSSetObject *set = (VSSetObject *)that;
    VSSetObject *new_set = new VSSetObject();
    for (auto item : set->_set) {
        new_set->_set.insert(item);
        INCREF(item);
    }
    INCREF_RET(VS_AS_OBJECT(new_set));
}

VSObject *vs_set_clear(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_SET, "set.clear()");

    VSSetObject *set = (VSSetObject *)obj;
    for (auto item : set->_set) {
        DECREF(item);
    }
    set->_set.clear();
}

VSObject *vs_set_len(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_SET, "set.len()");

    INCREF_RET(
        C_INT_TO_INT(
            ((VSSetObject *)obj)->_set.size()));
}

VSObject *vs_set_append(VSObject *obj, VSObject *item) {
    VS_ENSURE_TYPE(obj, T_SET, "set.append()");

    VSSetObject *set = (VSSetObject *)obj;
    auto res = set->_set.insert(item);
    if (res.second) {
        INCREF(item);
    }
    INCREF_RET(VS_NONE);
}

VSObject *vs_set_has(VSObject *obj, VSObject *item) {
    VS_ENSURE_TYPE(obj, T_SET, "set.has()");

    VSSetObject *set = (VSSetObject *)obj;
    auto iter = set->_set.find(item);
    INCREF_RET(C_BOOL_TO_BOOL(iter != set->_set.end()));
}

VSObject *vs_set_remove(VSObject *obj, VSObject *item) {
    VS_ENSURE_TYPE(obj, T_SET, "set.remove()");

    VSSetObject *set = (VSSetObject *)obj;
    auto iter = set->_set.find(item);
    if (iter != set->_set.end()) {
        DECREF(*iter);
        set->_set.erase(iter);
    }
    INCREF_RET(VS_NONE);
}

VSSetObject::VSSetObject() {
    this->type = T_SET;
    this->_set = std::unordered_set<VSObject *, __set_hash__, __set_equal_to__>();

    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_default_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_set_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_set_bytes, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "copy", vs_set_copy, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "clear", vs_set_clear, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "len", vs_set_len, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "append", vs_set_append, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "has", vs_set_has, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "remove", vs_set_remove, 2, false);
}

VSSetObject::~VSSetObject() {
    DECREF(vs_set_clear(this));
}