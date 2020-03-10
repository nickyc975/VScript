#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

#include <cstdarg>

class VSTupleObject : public VSObject
{
public:
    vs_size_t nitems;
    VSObject **items;

    VSTupleObject(vs_size_t nitems)
    {
        this->type = VSTupleType;

        this->nitems = nitems;
        this->items = (VSObject **)malloc(sizeof(VSObject *) * nitems);
        if (items == NULL)
        {
            err("unable to malloc memory of size: %llu\n", sizeof(VSObject *) * nitems);
            terminate(TERM_ERROR);
        }
    }

    ~VSTupleObject()
    {
        for (vs_size_t i = 0; i < this->nitems; i++)
        {
            DECREF(this->items[i]);
        }
        free(this->items);
    }
};

VSObject *vs_tuple_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    VSTypeObject *typetype = VS_TYPEOF(typeobj);
    VS_ENSURE_TYPE(typetype, T_TYPE, "tuple.__new__()");

    VSTypeObject *type = (VSTypeObject *)typeobj;
    VS_ENSURE_TYPE(type, T_TUPLE, "tuple.__new__()");

    vs_size_t nargs = VSObject::c_getlen(args);
    VSTupleObject *tuple = new VSTupleObject(nargs);
    for (vs_size_t i = 0; i < nargs; i++)
    {
        VSObject *idx = vs_int_from_cint(i);
        VSObject *item = VSObject::getitem_at(args, idx);
        tuple->items[i] = item;
        INCREF(item);
        DECREF(idx);
    }
    INCREF_RET(VS_AS_OBJECT(tuple));
}

void vs_tuple_init(VSObject *, VSObject *, VSObject *)
{
}

VSObject *vs_tuple_copy(const VSObject *that)
{
    VSTypeObject *type = VS_TYPEOF(that);
    VS_ENSURE_TYPE(type, T_TUPLE, "tuple.__copy__()");

    VSTupleObject *old_tuple = (VSTupleObject *)that;
    VSTupleObject *new_tuple = new VSTupleObject(old_tuple->nitems);

    for (vs_size_t i = 0; i < new_tuple->nitems; i++)
    {
        VSObject *item = old_tuple->items[i];
        new_tuple->items[i] = item;
        INCREF(item);
    }
    INCREF_RET(VS_AS_OBJECT(new_tuple));
}

VSObject *vs_tuple_str(VSObject *tupleobj)
{
    VSTypeObject *type = VS_TYPEOF(tupleobj);
    VS_ENSURE_TYPE(type, T_TUPLE, "tuple.__str__()");

    return vs_string_from_cstring("tuple");
}

VSObject *vs_tuple_bytes(VSObject *tupleobj)
{
    VSTypeObject *type = VS_TYPEOF(tupleobj);
    VS_ENSURE_TYPE(type, T_TUPLE, "tuple.__bytes__()");

    return NULL;
}

VSObject *vs_tuple_len(VSObject *tupleobj)
{
    VSTypeObject *type = VS_TYPEOF(tupleobj);
    VS_ENSURE_TYPE(type, T_TUPLE, "tuple.__len__()");

    cint_t len = ((VSTupleObject *)tupleobj)->nitems;
    return vs_int_from_cint(len);
}

VSObject *vs_tuple_get(VSObject *tupleobj, VSObject *idxobj)
{
    VSTypeObject *type = VS_TYPEOF(tupleobj);
    VS_ENSURE_TYPE(type, T_TUPLE, "tuple.__get__()");

    VSTypeObject *idxtype = VS_TYPEOF(idxobj);
    VS_ENSURE_TYPE(idxtype, T_INT, "as tuple index");

    VSTupleObject *tuple = (VSTupleObject *)tupleobj;
    vs_size_t idx = (vs_size_t)vs_int_to_cint(idxobj);
    if (idx >= tuple->nitems)
    {
        INDEX_OUT_OF_BOUND(idx, tuple->nitems);
        terminate(TERM_ERROR);
    }

    INCREF_RET(tuple->items[idx]);
}

// TODO: implement tuple.__has__()
VSObject *vs_tuple_has(VSObject *tupleobj, VSObject *item)
{
    VSTypeObject *type = VS_TYPEOF(tupleobj);
    VS_ENSURE_TYPE(type, T_TUPLE, "tuple.__has__()");

    INCREF_RET(VS_FALSE);
}

VSObject *vs_tuple_has_at(VSObject *tupleobj, VSObject *idxobj)
{
    VSTypeObject *type = VS_TYPEOF(tupleobj);
    VS_ENSURE_TYPE(type, T_TUPLE, "tuple.__has_at__()");

    VSTypeObject *idxtype = VS_TYPEOF(idxobj);
    VS_ENSURE_TYPE(idxtype, T_INT, "as tuple index");

    VSTupleObject *tuple = (VSTupleObject *)tupleobj;
    vs_size_t idx = (vs_size_t)vs_int_to_cint(idxobj);

    INCREF_RET(idx < tuple->nitems ? VS_TRUE : VS_FALSE);
}

VSObject *vs_tuple_add(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_TUPLE, "tuple.__add__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_TUPLE, "tuple.__add__()");

    VSTupleObject *tuple_a = (VSTupleObject *)a;
    VSTupleObject *tuple_b = (VSTupleObject *)b;

    VSTupleObject *tuple = new VSTupleObject(tuple_a->nitems + tuple_b->nitems);
    for (vs_size_t i = 0; i < tuple_a->nitems; i++)
    {
        VSObject *item = tuple_a->items[i];
        tuple->items[i] = item;
        INCREF(item);
    }

    for (vs_size_t i = 0; i < tuple_b->nitems; i++)
    {
        VSObject *item = tuple_b->items[i];
        tuple->items[tuple_a->nitems + i] = item;
        INCREF(item);
    }

    INCREF_RET(VS_AS_OBJECT(tuple));
}

VSObject *vs_tuple_pack(vs_size_t nitems, ...)
{
    VSTupleObject *tuple = new VSTupleObject(nitems);

    va_list args;
    va_start(args, nitems);
    for (vs_size_t i = 0; i < nitems; i++)
    {
        VSObject *item = va_arg(args, VSObject *);
        tuple->items[i] = item;
        INCREF(item);
    }
    va_end(args);

    INCREF_RET(VS_AS_OBJECT(tuple));
}

void vs_tuple_unpack(VSObject *tupleobj, vs_size_t nitems, ...)
{
    VSTypeObject *type = VS_TYPEOF(tupleobj);
    VS_ENSURE_TYPE(type, T_TUPLE, "tuple unpack");

    VSTupleObject *tuple = (VSTupleObject *)tupleobj;
    if (nitems >= tuple->nitems)
    {
        INDEX_OUT_OF_BOUND(nitems, tuple->nitems);
        terminate(TERM_ERROR);
    }

    va_list args;
    va_start(args, nitems);
    for (vs_size_t i = 0; i < nitems; i++)
    {
        VSObject **item = va_arg(args, VSObject **);
        *item = tuple->items[i];
        INCREF(*item);
    }
    va_end(args);
}

NumberFuncs *tuple_number_funcs = new NumberFuncs(
    NULL,         // __not__
    NULL,         // __neg__
    vs_tuple_add, // __add__
    NULL,         // __sub__
    NULL,         // __mul__
    NULL,         // __div__
    NULL,         // __mod__
    NULL,         // __and__
    NULL,         // __or__
    NULL,         // __bool__
    NULL,         // __char__
    NULL,         // __int__
    NULL          // __float__
);

ContainerFuncs *tuple_container_funcs = new ContainerFuncs(
    vs_tuple_len,    // __len__
    vs_tuple_get,    // __get__
    NULL,            // __set__
    NULL,            // __append__
    vs_tuple_has,    // __has__
    vs_tuple_has_at, // __has_at__
    NULL,            // __remove__
    NULL             // __remove_at__
);

VSTypeObject *VSTupleType = new VSTypeObject(
    T_TUPLE,
    "tuple",                 // __name__
    NULL,                    // __attrs__
    vs_tuple_new,            // __new__
    vs_tuple_init,           // __init__
    vs_tuple_copy,           // __copy__
    NULL,                    // __clear__
    NULL,                    // __getattr__
    NULL,                    // __hasattr__
    NULL,                    // __setattr__
    NULL,                    // __removeattr__
    vs_hash_not_implemented, // __hash__
    NULL,                    // __lt__
    vs_default_eq,           // __eq__
    vs_tuple_str,            // __str__
    vs_tuple_bytes,          // __bytes__
    NULL,                    // __call__
    tuple_number_funcs,      // _number_funcs
    tuple_container_funcs    // _container_funcs
);