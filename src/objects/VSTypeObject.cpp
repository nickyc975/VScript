#include "error.hpp"
#include "objects/VSTypeObject.hpp"
#include "objects/VSStringObject.hpp"

VSTypeObject::VSTypeObject(
    VSTypeObject *type,
    TYPE_TYPE t_type,
    std::string __name__,
    VSObject *__attrs__,
    ternaryfunc __new__,
    void_ternaryfunc __init__,
    const_unaryfunc __copy__,
    void_unaryfunc __clear__,
    binaryfunc __getattr__,
    binaryfunc __hasattr__,
    void_ternaryfunc __setattr__,
    void_binaryfunc __removeattr__,
    const_unaryfunc __hash__,
    const_binaryfunc __lt__,
    const_binaryfunc __eq__,
    unaryfunc __str__,
    unaryfunc __bytes__,
    ternaryfunc __call__,
    NumberFuncs *_number_funcs,
    ContainerFuncs *_container_funcs
) : t_type(t_type),
    __name__(__name__),
    __attrs__(__attrs__),
    __new__(__new__),
    __init__(__init__),
    __copy__(__copy__),
    __clear__(__clear__),
    __getattr__(__getattr__),
    __hasattr__(__hasattr__),
    __setattr__(__setattr__),
    __removeattr__(__removeattr__),
    __hash__(__hash__),
    __lt__(__lt__),
    __eq__(__eq__),
    __str__(__str__),
    __bytes__(__bytes__),
    __call__(__call__),
    _number_funcs(_number_funcs),
    _container_funcs(_container_funcs)
{
    this->type = type;
}

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

VSTypeObject VSTypeTypeStruct = VSTypeObject(
    &VSTypeTypeStruct,
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

VSTypeObject *VSTypeType = &VSTypeTypeStruct;