#include "error.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSBoolObject.hpp"

class VSBoolObject : public VSObject
{
public:
    uint8_t _value;

    VSBoolObject() : _value(0) {this->type = VSBoolType;}
    VSBoolObject(uint8_t val) : _value(val ? 1 : 0) {this->type = VSBoolType;}
};

VSObject *vs_bool_hash(const VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_BOOL, "bool hash");

    return vs_int_from_cint(((VSBoolObject *)obj)->_value);
}

VSObject *vs_bool_eq(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = vs_typeof(a);

    vs_ensure_type(a_type, T_BOOL, "bool eq");

    VSTypeObject *b_type = vs_typeof(b);

    vs_ensure_type(b_type, T_BOOL, "bool eq");

    bool res = ((VSBoolObject *)a)->_value == ((VSBoolObject *)b)->_value;
    return vs_as_object((res ? VS_TRUE : VS_FALSE));
}

VSObject *vs_bool_str(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_BOOL, "bool to str");

    return NULL;
}

VSObject *vs_bool_bytes(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_BOOL, "bool to bytes");

    return NULL;
}

uint8_t vs_bool_to_cbool(VSObject * boolobj)
{
    VSTypeObject *type = vs_typeof(boolobj);
    vs_ensure_type(type, T_BOOL, "to c bool");
    return ((VSBoolObject *)boolobj)->_value;
}

VSObject *vs_bool_from_cbool(uint8_t boolval)
{
    return vs_as_object((boolval ? VS_TRUE : VS_FALSE));
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

VSTypeObject *VSBoolType = new VSTypeObject(
    T_BOOL,
    "bool", // __name__
    NULL,  // __attrs__
    NULL,  // __new__
    NULL,  // __init__
    NULL,  // __copy__
    NULL,  // __clear__
    NULL,  // __getattr__
    NULL,  // __hasattr__
    NULL,  // __setattr__
    NULL,  // __removeattr__
    vs_bool_hash,  // __hash__
    vs_bool_eq,  // __eq__
    vs_bool_str,  // __str__
    vs_bool_bytes,  // __bytes__
    NULL,  // __call__
    number_funcs,  // _number_funcs
    NULL   // _container_funcs
);

VSBoolObject *VS_TRUE = new VSBoolObject(1);
VSBoolObject *VS_FALSE = new VSBoolObject(0);