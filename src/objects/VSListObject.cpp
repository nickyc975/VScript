#include "error.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSListObject.hpp"

#include <vector>

class VSListObject : public VSObject
{
public:
    std::vector<VSObject *> items;

    VSListObject()
    {
        this->type = VSListType;
        this->items = std::vector<VSObject *>();
    }

    ~VSListObject() = default;
};

VSObject *vs_list_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    VSTypeObject *typetype = VS_TYPEOF(typeobj);
    VS_ENSURE_TYPE(typetype, T_TYPE, "list.__new__()");

    VSTypeObject *type = (VSTypeObject *)typeobj;
    VS_ENSURE_TYPE(type, T_LIST, "list.__new__()");

    vs_size_t nargs = VSObject::c_getlen(args);
    VSListObject *list = new VSListObject();
    for (vs_size_t i = 0; i < nargs; i++)
    {
        VSObject *idx = vs_int_from_cint(i);
        VSObject *item = VSObject::getitem_at(args, idx);
        list->items.push_back(item);
        INCREF(item);
        DECREF(idx);
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
    VSListObject *new_list = new VSListObject();

    for (vs_size_t i = 0; i < new_list->items.size(); i++)
    {
        VSObject *item = old_list->items[i];
        new_list->items.push_back(item);
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

}

VSObject *vs_list_len(VSObject *listobj)
{
}

VSObject *vs_list_get(VSObject *listobj, VSObject *idxobj)
{
}

void vs_list_set(VSObject *listobj, VSObject *idxobj, VSObject *item)
{
}

void vs_list_append(VSObject *listobj, VSObject *item)
{
}

VSObject *vs_list_has(VSObject *listobj, VSObject *item)
{

}

VSObject *vs_list_has_at(VSObject *listobj, VSObject *idxobj)
{

}

void vs_list_remove(VSObject *listobj, VSObject *item)
{

}

void vs_list_remove_at(VSObject *listobj, VSObject *idxobj)
{

}

VSObject *vs_list_add(VSObject *a, VSObject *b)
{

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