#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"

class VSIntObject : public VSObject
{
public:
    int64_t _value;

    VSIntObject() : _value(0) {this->type = VSIntType;}
    VSIntObject(int64_t val) : _value(val) {this->type = VSIntType;}
};

VSObject *vs_int_new()
{
    return vs_as_object(new VSIntObject());
}

void vs_int_init(VSObject *obj, VSObject *args, VSObject *)
{
    vs_size_t len = VSObject::c_getlen(args);
    if (len > 1)
    {
        err("int.__init__() expected 0 or 1 arg but got %llu.", len);
        terminate(TERM_ERROR);
    }

    if (len == 0)
        return;
    
    VSObject *init_val = VSObject::getitem_at(args, VS_INT_ZERO);
    VSTypeObject *init_type = vs_typeof(init_val);
    if (init_type->_number_funcs == NULL 
        || init_type->_number_funcs->__int__ == NULL)
    {
        err("can not cast type \"%s\" to type \"int\".", init_type->__name__.c_str());
        terminate(TERM_ERROR);
    }

    VSObject *val = init_type->_number_funcs->__int__(init_val);
    incref(val);
    if (vs_typeof(val)->t_type != T_INT)
    {
        err("%s.__int__() returned \"%s\" instead of int.", init_type->__name__.c_str(), vs_typeof(val)->__name__.c_str());
        terminate(TERM_ERROR);
    }

    ((VSIntObject *)obj)->_value = ((VSIntObject *)val)->_value;
    decref_ex(val);
}

VSObject *vs_int_copy(const VSObject *that)
{
    VSTypeObject *type = vs_typeof(that);

    vs_ensure_type(type, T_INT, "int copy");

    VSIntObject *old_int = (VSIntObject *)that;
    VSIntObject *new_int = (VSIntObject *)type->__new__();
    new_int->_value = old_int->_value;
    return vs_as_object(new_int);
}

VSObject *vs_int_hash(const VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_INT, "int hash");

    return type->__copy__(obj);
}

VSObject *vs_int_eq(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = vs_typeof(a);

    vs_ensure_type(a_type, T_INT, "int eq");

    VSTypeObject *b_type = vs_typeof(b);

    vs_ensure_type(b_type, T_INT, "int eq");

    bool res = ((VSIntObject *)a)->_value == ((VSIntObject *)b)->_value;
    return res ? VS_TRUE : VS_FALSE;
}

VSObject *vs_int_str(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_INT, "int to str");

    return NULL;
}

VSObject *vs_int_bytes(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_INT, "int to bytes");

    return NULL;
}

int64_t vs_int_to_cint(VSObject *intobj)
{
    vs_ensure_type(vs_typeof(intobj), T_INT, "to c int");
    return ((VSIntObject *)intobj)->_value;
}

VSObject *vs_int_from_cint(int64_t intval)
{
    return new VSIntObject(intval);
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

VSTypeObject *VSIntType = new VSTypeObject(
    T_INT,
    "int", // __name__
    NULL,  // __attrs__
    vs_int_new,  // __new__
    vs_int_init,  // __init__
    vs_int_copy,  // __copy__
    NULL,  // __clear__
    NULL,  // __getattr__
    NULL,  // __hasattr__
    NULL,  // __setattr__
    NULL,  // __removeattr__
    vs_int_hash,  // __hash__
    vs_int_eq,  // __eq__
    vs_int_str,  // __str__
    vs_int_bytes,  // __bytes__
    NULL,  // __call__
    number_funcs,  // _number_funcs
    NULL   // _container_funcs
);

VSObject *VS_INT_ZERO = vs_as_object(new VSIntObject(0));
VSObject *VS_INT_ONE = vs_as_object(new VSIntObject(1));