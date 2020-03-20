#include "error.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

VSObject *vs_int_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    VSTypeObject *ttype = VS_TYPEOF(typeobj);
    VS_ENSURE_TYPE(ttype, T_TYPE, "int new");

    VSTypeObject *type = VS_AS_TYPE(typeobj);
    VS_ENSURE_TYPE(type, T_INT, "int new");

    vs_size_t len = TUPLE_LEN(args);
    if (len == 0)
    {
        INCREF_RET(VS_AS_OBJECT(new VSIntObject()));
    }
    else if (len > 2)
    {
        err("int.__new__() expected at most 2 args but got %llu.", len);
        terminate(TERM_ERROR);
    }

    VSObject *init_val = TUPLE_GET(args, 0);
    VSTypeObject *init_type = VS_TYPEOF(init_val);
    if (init_type->_number_funcs == NULL || init_type->_number_funcs->__int__ == NULL)
    {
        err("can not cast type \"%s\" to type \"int\".", init_type->__name__.c_str());
        terminate(TERM_ERROR);
    }

    VSObject *base = NULL;
    if (len == 2)
    {
        base = TUPLE_GET(args, 1);
    }

    VSObject *val = init_type->_number_funcs->__int__(init_val, base);
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

    return vs_int_copy(obj);
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
    return vs_string_from_cstring(std::to_string(val));
}

VSObject *vs_int_bytes(VSObject *obj)
{
    VSTypeObject *type = VS_TYPEOF(obj);
    VS_ENSURE_TYPE(type, T_INT, "int to bytes");

    return NULL;
}

VSObject *vs_int_neg(VSObject *intobj)
{
    VSTypeObject *type = VS_TYPEOF(intobj);
    VS_ENSURE_TYPE(type, T_INT, "int.__neg__()");

    cint_t res = -((VSIntObject *)intobj)->_value;
    return vs_int_from_cint(res);
}

VSObject *vs_int_add(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_INT, "int.__add__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_INT, "int.__add__()");

    cint_t res = ((VSIntObject *)a)->_value + ((VSIntObject *)b)->_value;
    return vs_int_from_cint(res);
}

VSObject *vs_int_sub(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_INT, "int.__sub__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_INT, "int.__sub__()");

    cint_t res = ((VSIntObject *)a)->_value - ((VSIntObject *)b)->_value;
    return vs_int_from_cint(res);
}

VSObject *vs_int_mul(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_INT, "int.__mul__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_INT, "int.__mul__()");

    cint_t res = ((VSIntObject *)a)->_value * ((VSIntObject *)b)->_value;
    return vs_int_from_cint(res);
}

VSObject *vs_int_div(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_INT, "int.__div__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_INT, "int.__div__()");

    if (((VSIntObject *)b)->_value == 0)
    {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cint_t res = ((VSIntObject *)a)->_value / ((VSIntObject *)b)->_value;
    return vs_int_from_cint(res);
}

VSObject *vs_int_mod(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_INT, "int.__mod__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_INT, "int.__mod__()");

    if (((VSIntObject *)b)->_value == 0)
    {
        err("mod by zero\n");
        terminate(TERM_ERROR);
    }

    cint_t res = ((VSIntObject *)a)->_value % ((VSIntObject *)b)->_value;
    return vs_int_from_cint(res);
}

VSObject *vs_int_bool(VSObject *intobj)
{
    VSTypeObject *type = VS_TYPEOF(intobj);
    VS_ENSURE_TYPE(type, T_INT, "int.__bool__()");

    cbool_t res = ((VSIntObject *)intobj)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_int_char(VSObject *intobj)
{
    VSTypeObject *type = VS_TYPEOF(intobj);
    VS_ENSURE_TYPE(type, T_INT, "int.__char__()");

    cbool_t res = ((VSIntObject *)intobj)->_value;
    return vs_char_from_cchar((cchar_t)res);
}

VSObject *vs_int_int(VSObject *intobj, VSObject *base)
{
    VSTypeObject *type = VS_TYPEOF(intobj);
    VS_ENSURE_TYPE(type, T_INT, "int.__int__()");

    if (base != NULL)
    {
        err("int.__int__() expected 1 arg but got 2\n");
        terminate(TERM_ERROR);
    }

    INCREF_RET(intobj);
}

VSObject *vs_int_float(VSObject *intobj)
{
    VSTypeObject *type = VS_TYPEOF(intobj);
    VS_ENSURE_TYPE(type, T_INT, "int.__float__()");

    cbool_t res = ((VSIntObject *)intobj)->_value;
    return vs_float_from_cfloat((cfloat_t)res);
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

NumberFuncs *int_number_funcs = new NumberFuncs(
    NULL,        // __not__
    vs_int_neg,  // __neg__
    vs_int_add,  // __add__
    vs_int_sub,  // __sub__
    vs_int_mul,  // __mul__
    vs_int_div,  // __div__
    vs_int_mod,  // __mod__
    NULL,        // __and__
    NULL,        // __or__
    vs_int_bool, // __bool__
    vs_int_char, // __char__
    vs_int_int,  // __int__
    vs_int_float // __float__
);

VSTypeObject *VSIntType = new VSTypeObject(
    VSTypeType,
    T_INT,
    "int",            // __name__
    NULL,             // __attrs__
    vs_int_new,       // __new__
    vs_int_init,      // __init__
    vs_int_copy,      // __copy__
    NULL,             // __clear__
    NULL,             // __getattr__
    NULL,             // __hasattr__
    NULL,             // __setattr__
    NULL,             // __removeattr__
    vs_int_hash,      // __hash__
    vs_int_lt,        // __lt__
    vs_int_eq,        // __eq__
    vs_int_str,       // __str__
    vs_int_bytes,     // __bytes__
    NULL,             // __call__
    int_number_funcs, // _number_funcs
    NULL              // _container_funcs
);