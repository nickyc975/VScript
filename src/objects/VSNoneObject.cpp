#include "error.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSStringObject.hpp"

VSObject *vs_none_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    INCREF_RET(VS_NONE);
}

void vs_none_init(VSObject *, VSObject *, VSObject *)
{
}

VSObject *vs_none_copy(const VSObject *noneobj)
{
    VSTypeObject *type = VS_TYPEOF(noneobj);
    VS_ENSURE_TYPE(type, T_NONE, "none copy");
    INCREF_RET(VS_NONE);
}

VSObject *vs_none_eq(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = VS_TYPEOF(a);

    VS_ENSURE_TYPE(a_type, T_NONE, "none eq");

    VSTypeObject *b_type = VS_TYPEOF(b);

    VS_ENSURE_TYPE(b_type, T_NONE, "none eq");

    INCREF_RET(VS_TRUE);
}

VSObject *vs_none_str(VSObject *obj)
{
    VSTypeObject *type = VS_TYPEOF(obj);
    VS_ENSURE_TYPE(type, T_NONE, "none to str");

    return vs_string_from_cstring("None");
}

VSObject *vs_none_bytes(VSObject *obj)
{
    VSTypeObject *type = VS_TYPEOF(obj);

    VS_ENSURE_TYPE(type, T_NONE, "none to bytes");

    return NULL;
}

VSTypeObject *VSNoneType = new VSTypeObject(
    VSTypeType,
    T_NONE,
    "none",                  // __name__
    NULL,                    // __attrs__
    vs_none_new,             // __new__
    vs_none_init,            // __init__
    vs_none_copy,            // __copy__
    NULL,                    // __clear__
    NULL,                    // __getattr__
    NULL,                    // __hasattr__
    NULL,                    // __setattr__
    NULL,                    // __removeattr__
    vs_hash_not_implemented, // __hash__
    NULL,                    // __lt__
    vs_none_eq,              // __eq__
    vs_none_str,             // __str__
    vs_none_bytes,           // __bytes__
    NULL,                    // __call__
    NULL,                    // _number_funcs
    NULL                     // _container_funcs
);