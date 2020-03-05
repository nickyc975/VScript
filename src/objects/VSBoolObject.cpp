#include "error.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSStringObject.hpp"

class VSBoolObject : public VSObject
{
public:
    cbool_t _value;

    VSBoolObject() : _value(0) {this->type = VSBoolType;}
    VSBoolObject(cbool_t val) : _value(val ? 1 : 0) {this->type = VSBoolType;}
};

VSObject *VS_TRUE = vs_as_object(new VSBoolObject(1));
VSObject *VS_FALSE = vs_as_object(new VSBoolObject(0));

VSObject *vs_bool_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    VSTypeObject *ttype = vs_typeof(typeobj);
    vs_ensure_type(ttype, T_TYPE, "bool new");

    VSTypeObject *type = vs_as_type(typeobj);
    vs_ensure_type(type, T_BOOL, "bool new");

    vs_size_t len = VSObject::c_getlen(args);
    if (len == 0)
    {
        return VS_FALSE;
    }
    else if (len > 1)
    {
        err("bool.__new__() expected 0 or 1 arg but got %llu.", len);
        terminate(TERM_ERROR);
    }

    VSObject *init_val = VSObject::getitem_at(args, VS_INT_ZERO);
    VSTypeObject *init_type = vs_typeof(init_val);
    if (init_type->_number_funcs == NULL || init_type->_number_funcs->__bool__ == NULL)
    {
        err("can not cast type \"%s\" to type \"bool\".", init_type->__name__.c_str());
        terminate(TERM_ERROR);
    }

    VSObject *val = init_type->_number_funcs->__bool__(init_val);
    if (vs_typeof(val)->t_type != T_BOOL)
    {
        err("%s.__bool__() returned \"%s\" instead of bool.", init_type->__name__.c_str(), vs_typeof(val)->__name__.c_str());
        terminate(TERM_ERROR);
    }

    return val;
}

void vs_bool_init(VSObject *, VSObject *, VSObject *)
{
}

VSObject *vs_bool_copy(const VSObject *boolobj)
{
    return vs_bool_from_cbool(is_true(const_cast<VSObject *>(boolobj)));
}

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
    return res ? VS_TRUE : VS_FALSE;
}

VSObject *vs_bool_str(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);
    vs_ensure_type(type, T_BOOL, "bool to str");

    return vs_string_from_cstring(is_true(obj) ? "true" : "false");
}

VSObject *vs_bool_bytes(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_BOOL, "bool to bytes");

    return NULL;
}

VSObject *vs_bool_not(VSObject *boolobj)
{
    VSTypeObject *type = vs_typeof(boolobj);
    vs_ensure_type(type, T_BOOL, "not");

    return is_true(boolobj) ? VS_FALSE : VS_TRUE;
}

VSObject *vs_bool_and(VSObject *a, VSObject *b)
{
    VSTypeObject *a_type = vs_typeof(a);

    vs_ensure_type(a_type, T_BOOL, "and");

    VSTypeObject *b_type = vs_typeof(b);

    vs_ensure_type(b_type, T_BOOL, "and");

    return vs_bool_from_cbool(is_true(a) && is_true(b));
}

VSObject *vs_bool_or(VSObject *a, VSObject *b)
{
    VSTypeObject *a_type = vs_typeof(a);

    vs_ensure_type(a_type, T_BOOL, "and");

    VSTypeObject *b_type = vs_typeof(b);

    vs_ensure_type(b_type, T_BOOL, "and");

    return vs_bool_from_cbool(is_true(a) || is_true(b));
}

VSObject *vs_bool_bool(VSObject *boolobj)
{
    VSTypeObject *type = vs_typeof(boolobj);
    vs_ensure_type(type, T_BOOL, "__bool__()");

    return boolobj;
}

VSObject *vs_bool_int(VSObject *boolobj)
{
    VSTypeObject *type = vs_typeof(boolobj);
    vs_ensure_type(type, T_BOOL, "__int__()");

    return vs_int_from_cint((cint_t)vs_bool_to_cbool(boolobj));
}

inline cbool_t vs_bool_to_cbool(VSObject * boolobj)
{
    VSTypeObject *type = vs_typeof(boolobj);
    vs_ensure_type(type, T_BOOL, "to c bool");
    return ((VSBoolObject *)boolobj)->_value;
}

inline VSObject *vs_bool_from_cbool(cbool_t boolval)
{
    return boolval ? VS_TRUE : VS_FALSE;
}

inline cbool_t is_true(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);
    vs_ensure_type(type, T_BOOL, "is true");
    return (cbool_t)(obj == VS_TRUE);
}

NumberFuncs *number_funcs = new NumberFuncs(
    vs_bool_not, // __not__
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
    vs_bool_and, // __and__
    vs_bool_or, // __or__
    vs_bool_bool, // __bool__
    NULL, // __char__
    vs_bool_int, // __int__
    NULL  // __float__
);

VSTypeObject *VSBoolType = new VSTypeObject(
    T_BOOL,
    "bool", // __name__
    NULL,  // __attrs__
    vs_bool_new,  // __new__
    vs_bool_init,  // __init__
    vs_bool_copy,  // __copy__
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