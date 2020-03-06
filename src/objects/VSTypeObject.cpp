#include "error.hpp"
#include "objects/VSTypeObject.hpp"
#include "objects/VSStringObject.hpp"

VSObject *vs_type_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    return NULL;
}

void vs_type_init(VSObject *typeobj, VSObject *args, VSObject *)
{
    typeobj->type = VSTypeType;
}

VSObject *vs_type_str(VSObject *typeobj)
{
    VS_ENSURE_TYPE(VS_TYPEOF(typeobj), T_TYPE, "type.__str__()");
    return vs_string_from_cstring(((VSTypeObject *)typeobj)->__name__);
}

VSTypeObject *VSTypeType = new VSTypeObject(
    T_TYPE,
    "type", // __name__
    NULL,   // __attrs__
    vs_type_new,   // __new__
    vs_type_init,   // __init__
    NULL,   // __copy__
    NULL,   // __clear__
    NULL,   // __getattr__
    NULL,   // __hasattr__
    NULL,   // __setattr__
    NULL,   // __removeattr__
    NULL,   // __hash__
    NULL,   // __lt__
    NULL,   // __eq__
    NULL,   // __str__
    NULL,   // __bytes__
    NULL,   // __call__
    NULL,   // _number_funcs
    NULL    // _container_funcs
);