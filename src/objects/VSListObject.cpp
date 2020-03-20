#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSTupleObject.hpp"

#include <cstdarg>

VSObject *vs_list_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    VSTypeObject *typetype = VS_TYPEOF(typeobj);
    VS_ENSURE_TYPE(typetype, T_TYPE, "list.__new__()");

    VSTypeObject *type = (VSTypeObject *)typeobj;
    VS_ENSURE_TYPE(type, T_LIST, "list.__new__()");

    vs_size_t nargs = TUPLE_LEN(args);
    VSListObject *list = new VSListObject(nargs);
    for (vs_size_t i = 0; i < nargs; i++)
    {
        VSObject *item = TUPLE_GET(args, i);
        list->items[i] = item;
        INCREF(item);
    }
    INCREF_RET(VS_AS_OBJECT(list));
}

void vs_list_init(VSObject *, VSObject *, VSObject *)
{
}

VSObject *vs_list_copy(const VSObject *that)
{
    VSTypeObject *type = VS_TYPEOF(that);
    VS_ENSURE_TYPE(type, T_LIST, "list.__copy__()");

    VSListObject *old_list = (VSListObject *)that;
    VSListObject *new_list = new VSListObject(old_list->items.size());

    for (vs_size_t i = 0; i < old_list->items.size(); i++)
    {
        VSObject *item = old_list->items[i];
        new_list->items[i] = item;
        INCREF(item);
    }
    INCREF_RET(VS_AS_OBJECT(new_list));
}

VSObject *vs_list_str(VSObject *listobj)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list.__str__()");

    return vs_string_from_cstring("list");
}

VSObject *vs_list_bytes(VSObject *listobj)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list.__bytes__()");

    return NULL;
}

void vs_list_clear(VSObject *listobj)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list.__clear__()");

    VSListObject *list = (VSListObject *)listobj;
    for (auto item : list->items)
    {
        DECREF_EX(item);
    }
    list->items.clear();
}

VSObject *vs_list_len(VSObject *listobj)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list.__len__()");

    VSListObject *list = (VSListObject *)listobj;

    return vs_int_from_cint((cint_t)list->items.size());
}

VSObject *vs_list_get(VSObject *listobj, VSObject *idxobj)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list.__get__()");

    VSTypeObject *idxtype = VS_TYPEOF(idxobj);
    VS_ENSURE_TYPE(idxtype, T_INT, "as list index");

    VSListObject *list = (VSListObject *)listobj;
    vs_size_t idx = (vs_size_t)vs_int_to_cint(idxobj);
    if (idx >= list->items.size())
    {
        INDEX_OUT_OF_BOUND(idx, list->items.size());
        terminate(TERM_ERROR);
    }

    INCREF_RET(list->items[idx]);
}

void vs_list_set(VSObject *listobj, VSObject *idxobj, VSObject *item)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list.__set__()");

    VSTypeObject *idxtype = VS_TYPEOF(idxobj);
    VS_ENSURE_TYPE(idxtype, T_INT, "as list index");

    VSListObject *list = (VSListObject *)listobj;
    vs_size_t idx = (vs_size_t)vs_int_to_cint(idxobj);
    if (idx >= list->items.size())
    {
        INDEX_OUT_OF_BOUND(idx, list->items.size());
        terminate(TERM_ERROR);
    }

    list->items[idx] = item;
    INCREF(item);
}

void vs_list_append(VSObject *listobj, VSObject *item)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list.__append__()");

    VSListObject *list = (VSListObject *)listobj;
    list->items.push_back(item);
    INCREF(item);
}

// TODO: implement list.__has__()
VSObject *vs_list_has(VSObject *listobj, VSObject *item)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list.__has__()");

    VSListObject *list = (VSListObject *)listobj;
    INCREF_RET(VS_TRUE);
}

VSObject *vs_list_has_at(VSObject *listobj, VSObject *idxobj)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list.__has_at__()");

    VSTypeObject *idxtype = VS_TYPEOF(idxobj);
    VS_ENSURE_TYPE(idxtype, T_INT, "as list index");

    VSListObject *list = (VSListObject *)listobj;
    vs_size_t idx = (vs_size_t)vs_int_to_cint(idxobj);

    INCREF_RET(idx < list->items.size() ? VS_TRUE : VS_FALSE);
}

// TODO: implement list.__remove__()
void vs_list_remove(VSObject *listobj, VSObject *item)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list.__remove__()");

    VSListObject *list = (VSListObject *)listobj;
}

void vs_list_remove_at(VSObject *listobj, VSObject *idxobj)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list.__remove_at__()");

    VSTypeObject *idxtype = VS_TYPEOF(idxobj);
    VS_ENSURE_TYPE(idxtype, T_INT, "as list index");

    VSListObject *list = (VSListObject *)listobj;
    vs_size_t idx = (vs_size_t)vs_int_to_cint(idxobj);

    if (idx >= list->items.size())
    {
        INDEX_OUT_OF_BOUND(idx, list->items.size());
        terminate(TERM_ERROR);
    }

    DECREF_EX(list->items[idx]);
    list->items.erase(list->items.begin() + idx);
}

VSObject *vs_list_add(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_LIST, "list.__add__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_LIST, "list.__add__()");

    VSListObject *list_a = (VSListObject *)a;
    VSListObject *list_b = (VSListObject *)b;
    VSListObject *list = new VSListObject(list_a->items.size() + list_b->items.size());

    vs_size_t idx = 0;
    for (auto item : list_a->items)
    {
        list->items[idx] = item;
        INCREF(item);
        idx++;
    }

    for (auto item : list_b->items)
    {
        list->items[idx] = item;
        INCREF(item);
        idx++;
    }

    INCREF_RET(VS_AS_OBJECT(list));
}

VSObject *vs_list_pack(vs_size_t nitems, ...)
{
    VSListObject *list = new VSListObject(nitems);

    va_list args;
    va_start(args, nitems);
    for (vs_size_t i = 0; i < nitems; i++)
    {
        VSObject *item = va_arg(args, VSObject *);
        list->items[i] = item;
        INCREF(item);
    }
    va_end(args);

    INCREF_RET(VS_AS_OBJECT(list));
}

int vs_list_unpack(VSObject *listobj, vs_size_t nitems, ...)
{
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list unpack");

    VSListObject *list = (VSListObject *)listobj;
    if (nitems >= list->items.size())
    {
        INDEX_OUT_OF_BOUND(nitems, list->items.size());
        return -1;
    }

    va_list args;
    va_start(args, nitems);
    for (vs_size_t i = 0; i < nitems; i++)
    {
        VSObject **item = va_arg(args, VSObject **);
        *item = list->items[i];
        INCREF(*item);
    }
    va_end(args);
    return 0;
}

VSObject *vs_list_to_tuple(VSObject *listobj) {
    VSTypeObject *type = VS_TYPEOF(listobj);
    VS_ENSURE_TYPE(type, T_LIST, "list to tuple");

    VSListObject *list = (VSListObject *)listobj;

    vs_size_t len = LIST_LEN(list);
    VSTupleObject *tuple = new VSTupleObject(len);
    for (vs_size_t i = 0; i < len; i++) {
        VSObject *item = LIST_GET(list, i);
        TUPLE_SET(tuple, i, item);
    }
    INCREF_RET(tuple);
}

NumberFuncs *list_number_funcs = new NumberFuncs(
    NULL,        // __not__
    NULL,        // __neg__
    vs_list_add, // __add__
    NULL,        // __sub__
    NULL,        // __mul__
    NULL,        // __div__
    NULL,        // __mod__
    NULL,        // __and__
    NULL,        // __or__
    NULL,        // __bool__
    NULL,        // __char__
    NULL,        // __int__
    NULL         // __float__
);

ContainerFuncs *list_container_funcs = new ContainerFuncs(
    vs_list_len,      // __len__
    vs_list_get,      // __get__
    vs_list_set,      // __set__
    vs_list_append,   // __append__
    vs_list_has,      // __has__
    vs_list_has_at,   // __has_at__
    vs_list_remove,   // __remove__
    vs_list_remove_at // __remove_at__
);

VSTypeObject *VSListType = new VSTypeObject(
    VSTypeType,
    T_LIST,
    "list",                  // __name__
    NULL,                    // __attrs__
    vs_list_new,             // __new__
    vs_list_init,            // __init__
    vs_list_copy,            // __copy__
    vs_list_clear,           // __clear__
    NULL,                    // __getattr__
    NULL,                    // __hasattr__
    NULL,                    // __setattr__
    NULL,                    // __removeattr__
    vs_hash_not_implemented, // __hash__
    NULL,                    // __lt__
    vs_default_eq,           // __eq__
    vs_list_str,             // __str__
    vs_list_bytes,           // __bytes__
    NULL,                    // __call__
    list_number_funcs,       // _number_funcs
    list_container_funcs     // _container_funcs
);