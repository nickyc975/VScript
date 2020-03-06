#include "error.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSStringObject.hpp"

class VSCharObject : public VSObject
{
public:
    cchar_t _value;

    VSCharObject() : _value(0) { this->type = VSCharType; }
    VSCharObject(cchar_t val) : _value(val) { this->type = VSCharType; }
};

VSObject *vs_char_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    VSTypeObject *ttype = VS_TYPEOF(typeobj);
    VS_ENSURE_TYPE(ttype, T_TYPE, "char new");

    VSTypeObject *type = VS_AS_TYPE(typeobj);
    VS_ENSURE_TYPE(type, T_CHAR, "char new");

    vs_size_t len = VSObject::c_getlen(args);
    if (len == 0)
    {
        INCREF_RET(new VSCharObject());
    }
    else if (len > 1)
    {
        err("char.__new__() expected 0 or 1 arg but got %llu.", len);
        terminate(TERM_ERROR);
    }

    VSObject *init_val = VSObject::getitem_at(args, VS_INT_ZERO);
    VSTypeObject *init_type = VS_TYPEOF(init_val);
    if (init_type->_number_funcs == NULL || init_type->_number_funcs->__char__ == NULL)
    {
        err("can not cast type \"%s\" to type \"char\".", init_type->__name__.c_str());
        terminate(TERM_ERROR);
    }

    VSObject *val = init_type->_number_funcs->__char__(init_val);
    if (VS_TYPEOF(val)->t_type != T_CHAR)
    {
        err("%s.__char__() returned \"%s\" instead of char.", init_type->__name__.c_str(), VS_TYPEOF(val)->__name__.c_str());
        terminate(TERM_ERROR);
    }

    DECREF_EX(init_val);
    INCREF_RET(val);
}

void vs_char_init(VSObject *self, VSObject *args, VSObject *)
{
}

VSObject *vs_char_copy(const VSObject *charobj)
{
    VSTypeObject *type = VS_TYPEOF(charobj);
    VS_ENSURE_TYPE(type, T_CHAR, "char copy");

    VSCharObject *oldchar = (VSCharObject *)charobj;
    VSCharObject *newchar = new VSCharObject(oldchar->_value);
    INCREF_RET(VS_AS_OBJECT(newchar));
}

VSObject *vs_char_hash(const VSObject *charobj)
{
    VSTypeObject *type = VS_TYPEOF(charobj);

    VS_ENSURE_TYPE(type, T_CHAR, "char hash");

    return vs_int_from_cint(((VSCharObject *)charobj)->_value);
}

VSObject *vs_char_lt(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = VS_TYPEOF(a);

    VS_ENSURE_TYPE(a_type, T_BOOL, "char lt");

    VSTypeObject *b_type = VS_TYPEOF(b);

    VS_ENSURE_TYPE(b_type, T_BOOL, "char lt");

    cbool_t res = ((VSCharObject *)a)->_value < ((VSCharObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_eq(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = VS_TYPEOF(a);

    VS_ENSURE_TYPE(a_type, T_BOOL, "char eq");

    VSTypeObject *b_type = VS_TYPEOF(b);

    VS_ENSURE_TYPE(b_type, T_BOOL, "char eq");

    bool res = ((VSCharObject *)a)->_value == ((VSCharObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_str(VSObject *charobj)
{
    VSTypeObject *type = VS_TYPEOF(charobj);
    VS_ENSURE_TYPE(type, T_CHAR, "char to str");

    auto str = std::string();
    str.push_back(((VSCharObject *)charobj)->_value);
    return vs_string_from_cstring(str);
}

VSObject *vs_char_bytes(VSObject *charobj)
{
    VSTypeObject *type = VS_TYPEOF(charobj);

    VS_ENSURE_TYPE(type, T_CHAR, "char to bytes");

    return NULL;
}

VSObject *vs_char_neg(VSObject *charobj)
{
    VSTypeObject *type = VS_TYPEOF(charobj);
    VS_ENSURE_TYPE(type, T_CHAR, "char.__neg__()");

    cchar_t res = -((VSCharObject *)charobj)->_value;
    return vs_char_from_cchar(res);
}

VSObject *vs_char_add(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_CHAR, "char.__add__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_CHAR, "char.__add__()");

    cchar_t res = ((VSCharObject *)a)->_value + ((VSCharObject *)b)->_value;
    return vs_char_from_cchar(res);
}

VSObject *vs_char_sub(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_CHAR, "char.__sub__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_CHAR, "char.__sub__()");

    cchar_t res = ((VSCharObject *)a)->_value - ((VSCharObject *)b)->_value;
    return vs_char_from_cchar(res);
}

VSObject *vs_char_mul(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_CHAR, "char.__mul__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_CHAR, "char.__mul__()");

    cchar_t res = ((VSCharObject *)a)->_value * ((VSCharObject *)b)->_value;
    return vs_char_from_cchar(res);
}

VSObject *vs_char_div(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_CHAR, "char.__div__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_CHAR, "char.__div__()");

    if (((VSCharObject *)b)->_value == 0)
    {
        err("divided by zero\n");
        terminate(TERM_ERROR);
    }

    cchar_t res = ((VSCharObject *)a)->_value / ((VSCharObject *)b)->_value;
    return vs_char_from_cchar(res);
}

VSObject *vs_char_mod(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_CHAR, "char.__mod__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_CHAR, "char.__mod__()");

    if (((VSCharObject *)b)->_value == 0)
    {
        err("mod by zero\n");
        terminate(TERM_ERROR);
    }

    cchar_t res = ((VSCharObject *)a)->_value % ((VSCharObject *)b)->_value;
    return vs_char_from_cchar(res);
}

VSObject *vs_char_bool(VSObject *charobj)
{
    VSTypeObject *type = VS_TYPEOF(charobj);
    VS_ENSURE_TYPE(type, T_CHAR, "__bool__()");

    cchar_t val = ((VSCharObject *)charobj)->_value;
    INCREF_RET(val ? VS_TRUE : VS_FALSE);
}

VSObject *vs_char_char(VSObject *charobj)
{
    VSTypeObject *type = VS_TYPEOF(charobj);
    VS_ENSURE_TYPE(type, T_CHAR, "__char__()");

    INCREF_RET(charobj);
}

VSObject *vs_char_int(VSObject *charobj, VSObject *base)
{
    VSTypeObject *type = VS_TYPEOF(charobj);
    VS_ENSURE_TYPE(type, T_CHAR, "__int__()");

    if (base != NULL)
    {
        err("char.__int__() expected 1 arg but got 2\n");
        terminate(TERM_ERROR);
    }

    return vs_int_from_cint((cint_t)vs_char_to_cchar(charobj));
}

VSObject *vs_char_float(VSObject *charobj)
{
    VSTypeObject *type = VS_TYPEOF(charobj);
    VS_ENSURE_TYPE(type, T_CHAR, "__float__()");

    return vs_float_from_cfloat((cfloat_t)vs_char_to_cchar(charobj));
}

inline cchar_t vs_char_to_cchar(VSObject *charobj)
{
    VSTypeObject *type = VS_TYPEOF(charobj);
    VS_ENSURE_TYPE(type, T_CHAR, "cchar_t to cchar");

    return ((VSCharObject *)charobj)->_value;
}

inline VSObject *vs_char_from_cchar(cchar_t charval)
{
    INCREF_RET(VS_AS_OBJECT(new VSCharObject(charval)));
}

NumberFuncs *number_funcs = new NumberFuncs(
    NULL,         // __not__
    vs_char_neg,  // __neg__
    vs_char_add,  // __add__
    vs_char_sub,  // __sub__
    vs_char_mul,  // __mul__
    vs_char_div,  // __div__
    vs_char_mod,  // __mod__
    NULL,         // __and__
    NULL,         // __or__
    vs_char_bool, // __bool__
    vs_char_char, // __char__
    vs_char_int,  // __int__
    vs_char_float // __float__
);

VSTypeObject *VSCharType = new VSTypeObject(
    T_CHAR,
    "char",        // __name__
    NULL,          // __attrs__
    vs_char_new,   // __new__
    vs_char_init,  // __init__
    vs_char_copy,  // __copy__
    NULL,          // __clear__
    NULL,          // __getattr__
    NULL,          // __hasattr__
    NULL,          // __setattr__
    NULL,          // __removeattr__
    vs_char_hash,  // __hash__
    vs_char_lt,    // __lt__
    vs_char_eq,    // __eq__
    vs_char_str,   // __str__
    vs_char_bytes, // __bytes__
    NULL,          // __call__
    number_funcs,  // _number_funcs
    NULL           // _container_funcs
);