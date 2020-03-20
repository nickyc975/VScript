#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSTupleObject.hpp"

VSObject *vs_float_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    VSTypeObject *ttype = VS_TYPEOF(typeobj);
    VS_ENSURE_TYPE(ttype, T_TYPE, "float new");

    VSTypeObject *type = VS_AS_TYPE(typeobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float new");

    vs_size_t len = TUPLE_LEN(args);
    if (len == 0)
    {
        INCREF_RET(VS_AS_OBJECT(new VSFloatObject()));
    }
    if (len > 1)
    {
        err("float.__new__() expected 0 or 1 arg but got %llu.", len);
        terminate(TERM_ERROR);
    }

    VSObject *init_val = TUPLE_GET(args, 0);
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
    return vs_int_from_cint(hash);
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

VSObject *vs_float_neg(VSObject *floatobj)
{
    VSTypeObject *type = VS_TYPEOF(floatobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float.__neg__()");

    cfloat_t res = -((VSFloatObject *)floatobj)->_value;
    return vs_float_from_cfloat(res);
}

VSObject *vs_float_add(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_FLOAT, "float.__add__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_FLOAT, "float.__add__()");

    cfloat_t res = ((VSFloatObject *)a)->_value + ((VSFloatObject *)b)->_value;
    return vs_float_from_cfloat(res);
}

VSObject *vs_float_sub(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_FLOAT, "float.__sub__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_FLOAT, "float.__sub__()");

    cfloat_t res = ((VSFloatObject *)a)->_value - ((VSFloatObject *)b)->_value;
    return vs_float_from_cfloat(res);
}

VSObject *vs_float_mul(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_FLOAT, "float.__mul__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_FLOAT, "float.__mul__()");

    cfloat_t res = ((VSFloatObject *)a)->_value * ((VSFloatObject *)b)->_value;
    return vs_float_from_cfloat(res);
}

VSObject *vs_float_div(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_FLOAT, "float.__div__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_FLOAT, "float.__div__()");

    if (((VSFloatObject *)b)->_value == 0)
    {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cfloat_t res = ((VSFloatObject *)a)->_value / ((VSFloatObject *)b)->_value;
    return vs_float_from_cfloat(res);
}

VSObject *vs_float_bool(VSObject *floatobj)
{
    VSTypeObject *type = VS_TYPEOF(floatobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float.__bool__()");

    cbool_t res = ((VSFloatObject *)floatobj)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_float_char(VSObject *floatobj)
{
    VSTypeObject *type = VS_TYPEOF(floatobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float.__char__()");

    cbool_t res = ((VSFloatObject *)floatobj)->_value;
    return vs_char_from_cchar((cchar_t)res);
}

VSObject *vs_float_int(VSObject *floatobj, VSObject *base)
{
    VSTypeObject *type = VS_TYPEOF(floatobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float.__int__()");

    if (base != NULL)
    {
        err("float.__int__() expected 1 arg but got 2\n");
        terminate(TERM_ERROR);
    }

    cbool_t res = ((VSFloatObject *)floatobj)->_value;
    return vs_int_from_cint((cint_t)res);
}

VSObject *vs_float_float(VSObject *floatobj)
{
    VSTypeObject *type = VS_TYPEOF(floatobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float.__float__()");

    INCREF_RET(floatobj);
}

VSObject *vs_float_str(VSObject *floatobj)
{
    VSTypeObject *type = VS_TYPEOF(floatobj);
    VS_ENSURE_TYPE(type, T_FLOAT, "float str");

    cfloat_t val = ((VSFloatObject *)floatobj)->_value;
    return vs_string_from_cstring(std::to_string(val));
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

NumberFuncs *float_number_funcs = new NumberFuncs(
    NULL,          // __not__
    vs_float_neg,  // __neg__
    vs_float_add,  // __add__
    vs_float_sub,  // __sub__
    vs_float_mul,  // __mul__
    vs_float_div,  // __div__
    NULL,          // __mod__
    NULL,          // __and__
    NULL,          // __or__
    vs_float_bool, // __bool__
    vs_float_char, // __char__
    vs_float_int,  // __int__
    vs_float_float // __float__
);

VSTypeObject *VSFloatType = new VSTypeObject(
    VSTypeType,
    T_FLOAT,
    "float",            // __name__
    NULL,               // __attrs__
    vs_float_new,       // __new__
    vs_float_init,      // __init__
    vs_float_copy,      // __copy__
    NULL,               // __clear__
    NULL,               // __getattr__
    NULL,               // __hasattr__
    NULL,               // __setattr__
    NULL,               // __removeattr__
    vs_float_hash,      // __hash__
    vs_float_lt,        // __lt__
    vs_float_eq,        // __eq__
    vs_float_str,       // __str__
    vs_float_bytes,     // __bytes__
    NULL,               // __call__
    float_number_funcs, // _number_funcs
    NULL                // _container_funcs
);
