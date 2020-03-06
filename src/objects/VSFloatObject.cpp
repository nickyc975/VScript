#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSFloatObject.hpp"

class VSFloatObject : public VSObject
{
public:
    cfloat_t _value;

    VSFloatObject() : _value(0.0L) {this->type = VSFloatType;}
    VSFloatObject(cfloat_t val) : _value(val) {this->type = VSFloatType;}
};

VSObject *vs_float_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    VSTypeObject *ttype = VS_TYPEOF(typeobj);
    VS_ENSURE_TYPE(ttype, T_TYPE, "float new");

    VSTypeObject *type = VS_AS_TYPE(typeobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float new");

    vs_size_t len = VSObject::c_getlen(args);
    if (len == 0)
    {
        INCREF_RET(VS_AS_OBJECT(new VSFloatObject()));
    }
    if (len > 1)
    {
        err("float.__new__() expected 0 or 1 arg but got %llu.", len);
        terminate(TERM_ERROR);
    }
    
    VSObject *init_val = VSObject::getitem_at(args, VS_INT_ZERO);
    VSTypeObject *init_type = VS_TYPEOF(init_val);
    if (init_type->_number_funcs == NULL || init_type->_number_funcs->__float__ == NULL)
    {
        err("can not cast type \"%s\" to type \"float\".", init_type->__name__.c_str());
        terminate(TERM_ERROR);
    }

    VSObject *val = init_type->_number_funcs->__float__(init_val);
    if (VS_TYPEOF(val)->t_type != T_FLOAT)
    {
        err("%s.__float__() returned \"%s\" instead of \"float\".", init_type->__name__.c_str(), VS_TYPEOF(val)->__name__.c_str());
        terminate(TERM_ERROR);
    }

    DECREF_EX(init_val);
    INCREF_RET(val);
}

void vs_float_init(VSObject *floatobj, VSObject *args, VSObject *)
{
}

VSObject *vs_float_copy(const VSObject *that)
{
    VSTypeObject *type = VS_TYPEOF(that);
    VS_ENSURE_TYPE(type, T_FLOAT, "float copy");

    VSFloatObject *old_float = (VSFloatObject *)that;
    VSFloatObject *new_float = new VSFloatObject(old_float->_value);
    INCREF_RET(VS_AS_OBJECT(new_float));
}

VSObject *vs_float_hash(const VSObject *floatobj)
{
    VSTypeObject *type = VS_TYPEOF(floatobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float hash");

    cfloat_t val = ((VSFloatObject *)floatobj)->_value;
    std::size_t hash = std::hash<cfloat_t>{}(val);
    INCREF_RET(vs_int_from_cint(hash));
}

VSObject *vs_float_lt(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = VS_TYPEOF(a);

    VS_ENSURE_TYPE(a_type, T_FLOAT, "float lt");

    VSTypeObject *b_type = VS_TYPEOF(b);

    VS_ENSURE_TYPE(b_type, T_FLOAT, "float lt");

    bool res = ((VSFloatObject *)a)->_value < ((VSFloatObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_eq(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = VS_TYPEOF(a);

    VS_ENSURE_TYPE(a_type, T_FLOAT, "float eq");

    VSTypeObject *b_type = VS_TYPEOF(b);

    VS_ENSURE_TYPE(b_type, T_FLOAT, "float eq");

    bool res = ((VSFloatObject *)a)->_value == ((VSFloatObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_str(VSObject *floatobj)
{
    VSTypeObject *type = VS_TYPEOF(floatobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float str");

    cfloat_t val = ((VSFloatObject *)floatobj)->_value;
    INCREF_RET(vs_string_from_cstring(std::to_string(val)));
}

VSObject *vs_float_bytes(VSObject *floatobj)
{
    VSTypeObject *type = VS_TYPEOF(floatobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float bytes");

    return NULL;
}

inline cfloat_t vs_float_to_cfloat(VSObject *floatobj)
{
    VSTypeObject *type = VS_TYPEOF(floatobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float to cfloat");

    return ((VSFloatObject *)floatobj)->_value;
}

inline VSObject *vs_float_from_cfloat(cfloat_t floatval)
{
    INCREF_RET(VS_AS_OBJECT(new VSFloatObject(floatval)));
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

VSTypeObject *VSFloatType = new VSTypeObject(
    T_FLOAT,
    "float", // __name__
    NULL,  // __attrs__
    vs_float_new,  // __new__
    vs_float_init,  // __init__
    vs_float_copy,  // __copy__
    NULL,  // __clear__
    NULL,  // __getattr__
    NULL,  // __hasattr__
    NULL,  // __setattr__
    NULL,  // __removeattr__
    vs_float_hash,  // __hash__
    vs_float_lt, // __lt__
    vs_float_eq,  // __eq__
    vs_float_str,  // __str__
    vs_float_bytes,  // __bytes__
    NULL,  // __call__
    number_funcs,  // _number_funcs
    NULL   // _container_funcs
);
