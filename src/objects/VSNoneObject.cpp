#include "error.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSStringObject.hpp"

class VSNoneObject : public VSObject
{
public:
    VSNoneObject() {this->type = VSNoneType;}
};

VSObject *vs_none_eq(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = vs_typeof(a);

    vs_ensure_type(a_type, T_NONE, "none eq");

    VSTypeObject *b_type = vs_typeof(b);

    vs_ensure_type(b_type, T_NONE, "none eq");

    return VS_TRUE;
}

VSObject *vs_none_str(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);
    vs_ensure_type(type, T_NONE, "none to str");

    return vs_string_from_cstring("None");
}

VSObject *vs_none_bytes(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_NONE, "none to bytes");

    return NULL;
}

NumberFuncs *number_funcs = new NumberFuncs(
    NULL, // __not__
    NULL, // __neg__
    NULL, // __add__
    NULL, // __sub__
    NULL, // __mul__
    NULL, // __div__
    NULL, // __mod__
    NULL, // __lt__
    NULL, // __gt__
    NULL, // __le__
    NULL, // __ge__
    NULL, // __and__
    NULL, // __or__
    NULL, // __bool__
    NULL, // __char__
    NULL, // __int__
    NULL  // __float__
);

VSTypeObject *VSNoneType = new VSTypeObject(
    T_NONE,
    "none", // __name__
    NULL,  // __attrs__
    NULL,  // __new__
    NULL,  // __init__
    NULL,  // __copy__
    NULL,  // __clear__
    NULL,  // __getattr__
    NULL,  // __hasattr__
    NULL,  // __setattr__
    NULL,  // __removeattr__
    NULL,  // __hash__
    vs_none_eq,  // __eq__
    vs_none_str,  // __str__
    vs_none_bytes,  // __bytes__
    NULL,  // __call__
    number_funcs,  // _number_funcs
    NULL   // _container_funcs
);

VSObject *VS_NONE = vs_as_object(new VSNoneObject());