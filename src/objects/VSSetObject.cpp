#include "objects/VSSetObject.hpp"

#include "error.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSStringObject.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);
NEW_IDENTIFIER(copy);
NEW_IDENTIFIER(clear);
NEW_IDENTIFIER(len);
NEW_IDENTIFIER(append);
NEW_IDENTIFIER(has);
NEW_IDENTIFIER(remove);

VSObject *vs_set(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs == 0) {
        INCREF_RET(new VSSetObject());
    } else if (nargs == 1) {
        VSObject *obj = args[0];
        if (obj->type == T_TUPLE) {
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

    ERR_NARGS("set()", 1, nargs);
    terminate(TERM_ERROR);
    return NULL;
}

VSObject *vs_set_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("set.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_SET, "set.__str__()");

    std::string set_str = "{";
    VSSetObject *set = (VSSetObject *)self;
    for (auto obj : set->_set) {
        VSObject *str = CALL_ATTR(obj, ID___str__, EMPTY_TUPLE());
        set_str.append(STRING_TO_C_STRING(str));
        set_str.append(", ");
        DECREF(str);
    }
    if (set_str.back() == ' ') {
        set_str.pop_back();
        set_str.pop_back();
    }
    set_str.append("}");

    INCREF_RET(C_STRING_TO_STRING(set_str));
}

VSObject *vs_set_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("set.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_SET, "set.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_set_copy(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("set.copy()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_SET, "set.copy()");

    VSSetObject *set = (VSSetObject *)self;
    VSSetObject *new_set = new VSSetObject();
    for (auto item : set->_set) {
        new_set->_set.insert(item);
        INCREF(item);
    }
    INCREF_RET(AS_OBJECT(new_set));
}

VSObject *vs_set_clear(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("set.clear()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_SET, "set.clear()");

    VSSetObject *set = (VSSetObject *)self;
    for (auto item : set->_set) {
        DECREF(item);
    }
    set->_set.clear();
    INCREF_RET(VS_NONE);
}

VSObject *vs_set_len(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("set.len()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_SET, "set.len()");

    INCREF_RET(
        C_INT_TO_INT(
            ((VSSetObject *)self)->_set.size()));
}

VSObject *vs_set_append(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("set.append()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_SET, "set.append()");

    VSObject *item = args[0];
    VSSetObject *set = (VSSetObject *)self;
    auto res = set->_set.insert(item);
    if (res.second) {
        INCREF(item);
    }
    INCREF_RET(VS_NONE);
}

VSObject *vs_set_has(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("set.has()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_SET, "set.has()");

    VSObject *item = args[0];
    VSSetObject *set = (VSSetObject *)self;
    auto iter = set->_set.find(item);
    INCREF_RET(C_BOOL_TO_BOOL(iter != set->_set.end()));
}

VSObject *vs_set_remove(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("set.remove()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_SET, "set.remove()");

    VSObject *item = args[0];
    VSSetObject *set = (VSSetObject *)self;
    auto iter = set->_set.find(item);
    if (iter != set->_set.end()) {
        DECREF(*iter);
        set->_set.erase(iter);
    }
    INCREF_RET(VS_NONE);
}

const str_func_map VSSetObject::vs_set_methods = {
    {ID___hash__, vs_default_hash},
    {ID___eq__, vs_default_eq},
    {ID___str__, vs_set_str},
    {ID___bytes__, vs_set_bytes},
    {ID_copy, vs_set_copy},
    {ID_clear, vs_set_clear},
    {ID_len, vs_set_len},
    {ID_append, vs_set_append},
    {ID_has, vs_set_has},
    {ID_remove, vs_set_remove}
};

VSSetObject::VSSetObject() {
    this->type = T_SET;
    this->_set = std::unordered_set<VSObject *, __set_hash__, __set_equal_to__>();
}

VSSetObject::~VSSetObject() {
    DECREF(vs_set_clear(this, NULL, 0));
}

bool VSSetObject::hasattr(std::string &attrname) {
    return vs_set_methods.find(attrname) != vs_set_methods.end();
}

VSObject *VSSetObject::getattr(std::string &attrname) {
    auto iter = vs_set_methods.find(attrname);
    if (iter == vs_set_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_set_methods.at(attrname));
    INCREF_RET(attr);
}

void VSSetObject::setattr(std::string &, VSObject *) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}
