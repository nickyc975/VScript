#include "objects/VSListObject.hpp"

#include <cstdarg>

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSSetObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);
NEW_IDENTIFIER(__add__);
NEW_IDENTIFIER(copy);
NEW_IDENTIFIER(clear);
NEW_IDENTIFIER(len);
NEW_IDENTIFIER(get);
NEW_IDENTIFIER(set);
NEW_IDENTIFIER(append);
NEW_IDENTIFIER(has_at);
NEW_IDENTIFIER(remove_at);

VSObject *vs_list(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs == 0) {
        INCREF_RET(new VSListObject(0));
    } else if (nargs == 1) {
        VSObject *obj = args[0];
        if (obj->type == T_LIST) {
            INCREF_RET(obj);
        } else if (obj->type == T_TUPLE) {
            return vs_tuple_to_list(obj);
        } else if (obj->type == T_SET) {
            VSSetObject *set = (VSSetObject *)obj;
            VSListObject *list = new VSListObject(0);
            for (auto item : set->_set) {
                LIST_APPEND(list, item);
            }
            INCREF_RET(list);
        } else {
            err("can not cast \"%s\" object to list", TYPE_STR[obj->type]);
            INCREF_RET(VS_NONE);
        }
    }

    ERR_NARGS("list()", 1, nargs);
    terminate(TERM_ERROR);
    return NULL;
}

VSObject *vs_list_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("list.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_LIST, "list.__str__()");

    std::string list_str = "[";
    VSListObject *list = (VSListObject *)self;
    for (auto obj : list->items) {
        VSObject *str = CALL_ATTR(obj, ID___str__, EMPTY_TUPLE());
        list_str.append(STRING_TO_C_STRING(str));
        list_str.append(", ");
        DECREF(str);
    }
    if (list_str.back() == ' ') {
        list_str.pop_back();
        list_str.pop_back();
    }
    list_str.append("]");

    INCREF_RET(C_STRING_TO_STRING(list_str));
}

VSObject *vs_list_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("list.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_LIST, "list.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_list_add(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("list.__add__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_LIST, "list.__add__()");
    ENSURE_TYPE(that, T_LIST, "list.__add__()");

    VSListObject *list_a = (VSListObject *)self;
    VSListObject *list_b = (VSListObject *)that;
    VSListObject *list = new VSListObject(list_a->items.size() + list_b->items.size());

    vs_size_t idx = 0;
    for (auto item : list_a->items) {
        list->items[idx] = item;
        INCREF(item);
        idx++;
    }

    for (auto item : list_b->items) {
        list->items[idx] = item;
        INCREF(item);
        idx++;
    }

    INCREF_RET(AS_OBJECT(list));
}

VSObject *vs_list_copy(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("list.copy()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_LIST, "list.copy()");

    VSListObject *old_list = (VSListObject *)self;
    VSListObject *new_list = new VSListObject(old_list->items.size());

    for (vs_size_t i = 0; i < old_list->items.size(); i++) {
        VSObject *item = old_list->items[i];
        new_list->items[i] = item;
        INCREF(item);
    }
    INCREF_RET(AS_OBJECT(new_list));
}

VSObject *vs_list_clear(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("list.clear()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_LIST, "list.clear()");

    VSListObject *list = (VSListObject *)self;
    for (auto item : list->items) {
        DECREF_EX(item);
    }
    list->items.clear();
    INCREF_RET(VS_NONE);
}

VSObject *vs_list_len(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("list.len()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_LIST, "list.len()");

    VSListObject *list = (VSListObject *)self;

    INCREF_RET(C_INT_TO_INT((cint_t)list->items.size()));
}

VSObject *vs_list_get(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("list.get()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    ENSURE_TYPE(self, T_LIST, "list.get()");
    ENSURE_TYPE(idxobj, T_INT, "as list index");

    VSListObject *list = (VSListObject *)self;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);
    if (idx >= list->items.size()) {
        INDEX_OUT_OF_BOUND(idx, list->items.size());
        terminate(TERM_ERROR);
    }

    INCREF_RET(list->items[idx]);
}

VSObject *vs_list_set(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 2) {
        ERR_NARGS("list.set()", 2, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    VSObject *item = args[1];
    ENSURE_TYPE(self, T_LIST, "list.set()");
    ENSURE_TYPE(idxobj, T_INT, "as list index");

    VSListObject *list = (VSListObject *)self;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);
    if (idx >= list->items.size()) {
        INDEX_OUT_OF_BOUND(idx, list->items.size());
        terminate(TERM_ERROR);
    }

    DECREF(list->items[idx]);
    list->items[idx] = item;
    INCREF(item);

    INCREF_RET(VS_NONE);
}

VSObject *vs_list_append(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("list.append()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_LIST, "list.append()");

    VSObject *item = args[0];
    VSListObject *list = (VSListObject *)self;
    list->items.push_back(item);
    INCREF(item);
    INCREF_RET(VS_NONE);
}

// TODO: implement list.__has__()
VSObject *vs_list_has(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("list.has()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_LIST, "list.has()");

    // VSObject *item = args[0];
    // VSListObject *list = (VSListObject *)self;
    INCREF_RET(VS_FALSE);
}

VSObject *vs_list_has_at(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("list.has_at()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    ENSURE_TYPE(self, T_LIST, "list.has_at()");
    ENSURE_TYPE(idxobj, T_INT, "as list index");

    VSListObject *list = (VSListObject *)self;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    INCREF_RET(idx < list->items.size() ? VS_TRUE : VS_FALSE);
}

// TODO: implement list.__remove__()
VSObject *vs_list_remove(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("list.remove()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_LIST, "list.remove()");

    // VSListObject *list = (VSListObject *)self;
    INCREF_RET(VS_NONE);
}

VSObject *vs_list_remove_at(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("list.remove_at()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    ENSURE_TYPE(self, T_LIST, "list.remove_at()");
    ENSURE_TYPE(idxobj, T_INT, "as list index");

    VSListObject *list = (VSListObject *)self;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    if (idx >= list->items.size()) {
        INDEX_OUT_OF_BOUND(idx, list->items.size());
        terminate(TERM_ERROR);
    }

    DECREF_EX(list->items[idx]);
    list->items.erase(list->items.begin() + idx);
    INCREF_RET(VS_NONE);
}

VSListObject *vs_list_pack(vs_size_t nitems, ...) {
    VSListObject *list = new VSListObject(nitems);

    va_list args;
    va_start(args, nitems);
    for (vs_size_t i = 0; i < nitems; i++) {
        VSObject *item = va_arg(args, VSObject *);
        list->items[i] = item;
        INCREF(item);
    }
    va_end(args);

    INCREF_RET(list);
}

int vs_list_unpack(VSObject *listobj, vs_size_t nitems, ...) {
    ENSURE_TYPE(listobj, T_LIST, "list unpack");

    VSListObject *list = (VSListObject *)listobj;
    if (nitems >= list->items.size()) {
        INDEX_OUT_OF_BOUND(nitems, list->items.size());
        return -1;
    }

    va_list args;
    va_start(args, nitems);
    for (vs_size_t i = 0; i < nitems; i++) {
        VSObject **item = va_arg(args, VSObject **);
        *item = list->items[i];
        INCREF(*item);
    }
    va_end(args);
    return 0;
}

VSObject *vs_list_to_tuple(VSObject *listobj) {
    ENSURE_TYPE(listobj, T_LIST, "list to tuple");

    VSListObject *list = (VSListObject *)listobj;

    vs_size_t len = LIST_LEN(list);
    VSTupleObject *tuple = new VSTupleObject(len);
    for (vs_size_t i = 0; i < len; i++) {
        VSObject *item = LIST_GET(list, i);
        TUPLE_SET(tuple, i, item);
    }
    INCREF_RET(tuple);
}

const str_func_map VSListObject::vs_list_methods = {
    {ID___hash__, vs_default_hash},
    {ID___eq__, vs_default_eq},
    {ID___str__, vs_list_str},
    {ID___bytes__, vs_list_bytes},
    {ID___add__, vs_list_add},
    {ID_copy, vs_list_copy},
    {ID_clear, vs_list_clear},
    {ID_len, vs_list_len},
    {ID_get, vs_list_get},
    {ID_set, vs_list_set},
    {ID_append, vs_list_append},
    {ID_has_at, vs_list_has_at},
    {ID_remove_at, vs_list_remove_at}};

VSListObject::VSListObject(vs_size_t nitems) {
    this->type = T_LIST;
    this->items = std::vector<VSObject *>(nitems);
}

VSListObject::~VSListObject() {
    DECREF(vs_list_clear(this, NULL, 0));
}

bool VSListObject::hasattr(std::string &attrname) {
    return vs_list_methods.find(attrname) != vs_list_methods.end();
}

VSObject *VSListObject::getattr(std::string &attrname) {
    auto iter = vs_list_methods.find(attrname);
    if (iter == vs_list_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_list_methods.at(attrname));
    INCREF_RET(attr);
}

void VSListObject::setattr(std::string &, VSObject *) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}
