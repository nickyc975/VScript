#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"

class VSIntObject : public VSObject
{
public:
    cint_t _value;

    VSIntObject() : _value(0) {this->type = VSIntType;}
    VSIntObject(cint_t val) : _value(val) {this->type = VSIntType;}
};

VSObject *vs_int_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    VSTypeObject *ttype = VS_TYPEOF(typeobj);
    VS_ENSURE_TYPE(ttype, T_TYPE, "int new");

    VSTypeObject *type = VS_AS_TYPE(typeobj);
    VS_ENSURE_TYPE(type, T_INT, "int new");

    vs_size_t len = VSObject::c_getlen(args);
    if (len == 0)
    {
        INCREF_RET(VS_AS_OBJECT(new VSIntObject()));
    }
    else if (len > 1)
    {
        err("int.__new__() expected 0 or 1 arg but got %llu.", len);
        terminate(TERM_ERROR);
    }

    VSObject *init_val = VSObject::getitem_at(args, VS_INT_ZERO);
    VSTypeObject *init_type = VS_TYPEOF(init_val);
    if (init_type->_number_funcs == NULL || init_type->_number_funcs->__int__ == NULL)
    {
        err("can not cast type \"%s\" to type \"int\".", init_type->__name__.c_str());
        terminate(TERM_ERROR);
    }

    VSObject *val = init_type->_number_funcs->__int__(init_val);
    if (VS_TYPEOF(val)->t_type != T_INT)
    {
        err("%s.__int__() returned \"%s\" instead of int.", init_type->__name__.c_str(), VS_TYPEOF(val)->__name__.c_str());
        terminate(TERM_ERROR);
    }

    DECREF_EX(init_val);
    INCREF_RET(val);
}

void vs_int_init(VSObject *, VSObject *, VSObject *)
{
}

VSObject *vs_int_copy(const VSObject *that)
{
    VSTypeObject *type = VS_TYPEOF(that);
    VS_ENSURE_TYPE(type, T_INT, "int copy");

    VSIntObject *old_int = (VSIntObject *)that;
    VSIntObject *new_int = new VSIntObject(old_int->_value);
    INCREF_RET(VS_AS_OBJECT(new_int));
}

VSObject *vs_int_hash(const VSObject *obj)
{
    VSTypeObject *type = VS_TYPEOF(obj);

    VS_ENSURE_TYPE(type, T_INT, "int hash");

    INCREF_RET(type->__copy__(obj));
}

VSObject *vs_int_lt(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = VS_TYPEOF(a);

    VS_ENSURE_TYPE(a_type, T_INT, "int lt");

    VSTypeObject *b_type = VS_TYPEOF(b);

    VS_ENSURE_TYPE(b_type, T_INT, "int lt");

    bool res = ((VSIntObject *)a)->_value < ((VSIntObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_eq(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = VS_TYPEOF(a);

    VS_ENSURE_TYPE(a_type, T_INT, "int eq");

    VSTypeObject *b_type = VS_TYPEOF(b);

    VS_ENSURE_TYPE(b_type, T_INT, "int eq");

    bool res = ((VSIntObject *)a)->_value == ((VSIntObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_str(VSObject *obj)
{
    VSTypeObject *type = VS_TYPEOF(obj);
    VS_ENSURE_TYPE(type, T_INT, "int to str");

    cint_t val = ((VSIntObject *)obj)->_value;
    INCREF_RET(vs_string_from_cstring(std::to_string(val)));
}

VSObject *vs_int_bytes(VSObject *obj)
{
    VSTypeObject *type = VS_TYPEOF(obj);
    VS_ENSURE_TYPE(type, T_INT, "int to bytes");

    return NULL;
}

inline cint_t vs_int_to_cint(VSObject *intobj)
{
    VS_ENSURE_TYPE(VS_TYPEOF(intobj), T_INT, "to c int");
    return ((VSIntObject *)intobj)->_value;
}

inline VSObject *vs_int_from_cint(cint_t intval)
{
    INCREF_RET(new VSIntObject(intval));
}

NumberFuncs *number_funcs = new NumberFuncs(
    NULL, // __not__
    NULL, // __neg__
    NULL, // __add__
    NULL, // __sub__
    NULL, // __mul__
    NULL, // __div__
    NULL, // __mod__
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
    vs_int_lt, // __lt__
    vs_int_eq,  // __eq__
    vs_int_str,  // __str__
    vs_int_bytes,  // __bytes__
    NULL,  // __call__
    number_funcs,  // _number_funcs
    NULL   // _container_funcs
);

VSObject *VS_INT_ZERO = VS_AS_OBJECT(new VSIntObject(0));
VSObject *VS_INT_ONE = VS_AS_OBJECT(new VSIntObject(1));