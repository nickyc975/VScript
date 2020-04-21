#include "objects/VSListObject.hpp"

#include <cstdarg>

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

VSObject *vs_list(VSObject *obj) {
    if (obj == NULL) {
        INCREF_RET(new VSListObject(0));
    } else if (obj->type == T_LIST) {
        INCREF_RET(obj);
    } else if (obj->type == T_TUPLE) {
        return vs_tuple_to_list(obj);
    } else {
        err("can not cast \"%s\" object to list", TYPE_STR[obj->type]);
        INCREF_RET(VS_NONE);
    }
}

VSObject *vs_list_str(VSObject *listobj) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list.__str__()");

    INCREF_RET(C_STRING_TO_STRING("list"));
}

VSObject *vs_list_bytes(VSObject *listobj) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_list_add(VSObject *a, VSObject *b) {
    VS_ENSURE_TYPE(a, T_LIST, "list.__add__()");
    VS_ENSURE_TYPE(b, T_LIST, "list.__add__()");

    VSListObject *list_a = (VSListObject *)a;
    VSListObject *list_b = (VSListObject *)b;
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

    INCREF_RET(VS_AS_OBJECT(list));
}

VSObject *vs_list_copy(const VSObject *that) {
    VS_ENSURE_TYPE(that, T_LIST, "list.copy()");

    VSListObject *old_list = (VSListObject *)that;
    VSListObject *new_list = new VSListObject(old_list->items.size());

    for (vs_size_t i = 0; i < old_list->items.size(); i++) {
        VSObject *item = old_list->items[i];
        new_list->items[i] = item;
        INCREF(item);
    }
    INCREF_RET(VS_AS_OBJECT(new_list));
}

VSObject *vs_list_clear(VSObject *listobj) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list.clear()");

    VSListObject *list = (VSListObject *)listobj;
    for (auto item : list->items) {
        DECREF_EX(item);
    }
    list->items.clear();
    INCREF_RET(VS_NONE);
}

VSObject *vs_list_len(VSObject *listobj) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list.len()");

    VSListObject *list = (VSListObject *)listobj;

    INCREF_RET(C_INT_TO_INT((cint_t)list->items.size()));
}

VSObject *vs_list_get(VSObject *listobj, VSObject *idxobj) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list.get()");
    VS_ENSURE_TYPE(idxobj, T_INT, "as list index");

    VSListObject *list = (VSListObject *)listobj;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);
    if (idx >= list->items.size()) {
        INDEX_OUT_OF_BOUND(idx, list->items.size());
        terminate(TERM_ERROR);
    }

    INCREF_RET(list->items[idx]);
}

VSObject *vs_list_set(VSObject *listobj, VSObject *idxobj, VSObject *item) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list.set()");
    VS_ENSURE_TYPE(idxobj, T_INT, "as list index");

    VSListObject *list = (VSListObject *)listobj;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);
    if (idx >= list->items.size()) {
        INDEX_OUT_OF_BOUND(idx, list->items.size());
        terminate(TERM_ERROR);
    }

    list->items[idx] = item;
    INCREF(item);
    INCREF_RET(VS_NONE);
}

VSObject *vs_list_append(VSObject *listobj, VSObject *item) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list.append()");

    VSListObject *list = (VSListObject *)listobj;
    list->items.push_back(item);
    INCREF(item);
    INCREF_RET(VS_NONE);
}

// TODO: implement list.__has__()
VSObject *vs_list_has(VSObject *listobj, VSObject *item) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list.has()");

    VSListObject *list = (VSListObject *)listobj;
    INCREF_RET(VS_TRUE);
}

VSObject *vs_list_has_at(VSObject *listobj, VSObject *idxobj) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list.has_at()");
    VS_ENSURE_TYPE(idxobj, T_INT, "as list index");

    VSListObject *list = (VSListObject *)listobj;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    INCREF_RET(idx < list->items.size() ? VS_TRUE : VS_FALSE);
}

// TODO: implement list.__remove__()
VSObject *vs_list_remove(VSObject *listobj, VSObject *item) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list.remove()");

    VSListObject *list = (VSListObject *)listobj;
    INCREF_RET(VS_NONE);
}

VSObject *vs_list_remove_at(VSObject *listobj, VSObject *idxobj) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list.remove_at()");
    VS_ENSURE_TYPE(idxobj, T_INT, "as list index");

    VSListObject *list = (VSListObject *)listobj;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    if (idx >= list->items.size()) {
        INDEX_OUT_OF_BOUND(idx, list->items.size());
        terminate(TERM_ERROR);
    }

    DECREF_EX(list->items[idx]);
    list->items.erase(list->items.begin() + idx);
    INCREF_RET(VS_NONE);
}

VSObject *vs_list_pack(vs_size_t nitems, ...) {
    VSListObject *list = new VSListObject(nitems);

    va_list args;
    va_start(args, nitems);
    for (vs_size_t i = 0; i < nitems; i++) {
        VSObject *item = va_arg(args, VSObject *);
        list->items[i] = item;
        INCREF(item);
    }
    va_end(args);

    INCREF_RET(VS_AS_OBJECT(list));
}

int vs_list_unpack(VSObject *listobj, vs_size_t nitems, ...) {
    VS_ENSURE_TYPE(listobj, T_LIST, "list unpack");

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
    VS_ENSURE_TYPE(listobj, T_LIST, "list to tuple");

    VSListObject *list = (VSListObject *)listobj;

    vs_size_t len = LIST_LEN(list);
    VSTupleObject *tuple = new VSTupleObject(len);
    for (vs_size_t i = 0; i < len; i++) {
        VSObject *item = LIST_GET(list, i);
        TUPLE_SET(tuple, i, item);
    }
    INCREF_RET(tuple);
}

VSListObject::VSListObject(vs_size_t nitems) {
    this->type = T_LIST;
    this->items = std::vector<VSObject *>(nitems);

    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_default_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_list_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_list_bytes, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__add__", vs_list_add, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "copy", vs_list_copy, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "clear", vs_list_clear, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "len", vs_list_len, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "get", vs_list_get, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "set", vs_list_set, 3, false);
    NEW_NATIVE_FUNC_ATTR(this, "append", vs_list_append, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "has_at", vs_list_has_at, 2, false);
    NEW_NATIVE_FUNC_ATTR(this, "remove_at", vs_list_remove_at, 2, false);
}

VSListObject::~VSListObject() {
    DECREF(vs_list_clear(this));
}
