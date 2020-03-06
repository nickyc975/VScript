#include "error.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSStringObject.hpp"

class VSStringObject : public VSObject
{
public:
    std::string _value;

    VSStringObject() : _value("") {this->type = VSStringType;}
    VSStringObject(std::string val) : _value(val) {this->type = VSStringType;}
};

VSObject *vs_string_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    VSTypeObject *ttype = VS_TYPEOF(typeobj);
    VS_ENSURE_TYPE(ttype, T_TYPE, "str new");

    VSTypeObject *type = VS_AS_TYPE(typeobj);
    VS_ENSURE_TYPE(type, T_STR, "str new");

    vs_size_t len = VSObject::c_getlen(args);
    if (len == 0)
    {
        INCREF_RET(VS_AS_OBJECT(new VSStringObject()));
    }
    else if (len > 1)
    {
        err("str.__new__() expected 0 or 1 arg but got %llu.", len);
        terminate(TERM_ERROR);
    }
    
    VSObject *init_val = VSObject::getitem_at(args, VS_INT_ZERO);
    VSTypeObject *init_type = VS_TYPEOF(init_val);
    if (init_type->__str__ == NULL)
    {
        err("can not cast type \"%s\" to type \"str\".", init_type->__name__.c_str());
        terminate(TERM_ERROR);
    }

    VSObject *val = init_type->__str__(init_val);
    if (VS_TYPEOF(val)->t_type != T_STR)
    {
        err("%s.__str__() returned \"%s\" instead of str.", init_type->__name__.c_str(), VS_TYPEOF(val)->__name__.c_str());
        terminate(TERM_ERROR);
    }

    DECREF_EX(init_val);
    INCREF_RET(val);
}

void vs_string_init(VSObject *strobj, VSObject *args, VSObject *)
{
}

VSObject *vs_string_copy(const VSObject *that)
{
    VSTypeObject *type = VS_TYPEOF(that);
    VS_ENSURE_TYPE(type, T_STR, "str copy");

    INCREF_RET(
        VS_AS_OBJECT(new VSStringObject(((VSStringObject *)that)->_value))
    );
}

void vs_string_clear(VSObject *strobj)
{
    VSTypeObject *type = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(type, T_STR, "str clear");

    ((VSStringObject *)strobj)->_value.clear();
}

VSObject *vs_string_hash(const VSObject *strobj)
{
    VSTypeObject *type = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(type, T_STR, "str hash");

    std::size_t hash = std::hash<std::string>{}(((VSStringObject *)strobj)->_value);
    return vs_int_from_cint(hash);
}

VSObject *vs_string_lt(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = VS_TYPEOF(a);
    VS_ENSURE_TYPE(a_type, T_STR, "str lt");

    VSTypeObject *b_type = VS_TYPEOF(b);
    VS_ENSURE_TYPE(b_type, T_STR, "str lt");

    cbool_t res = ((VSStringObject *)a)->_value < ((VSStringObject *)b)->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_eq(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = VS_TYPEOF(a);
    VS_ENSURE_TYPE(a_type, T_STR, "str eq");

    VSTypeObject *b_type = VS_TYPEOF(b);
    VS_ENSURE_TYPE(b_type, T_STR, "str eq");

    VSStringObject *astr = (VSStringObject *)a;
    VSStringObject *bstr = (VSStringObject *)b;

    INCREF_RET(astr->_value == bstr->_value ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_str(VSObject *strobj)
{
    VSTypeObject *type = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(type, T_STR, "str str");

    INCREF_RET(strobj);
}

VSObject *vs_string_bytes(VSObject *strobj)
{
    VSTypeObject *type = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(type, T_STR, "str bytes");

    return NULL;
}

VSObject *vs_string_add(VSObject *a, VSObject *b)
{
    VSTypeObject *atype = VS_TYPEOF(a);
    VS_ENSURE_TYPE(atype, T_STR, "str.__add__()");

    VSTypeObject *btype = VS_TYPEOF(b);
    VS_ENSURE_TYPE(btype, T_STR, "str.__add__()");

    std::string res = ((VSStringObject *)a)->_value + ((VSStringObject *)b)->_value;
    return vs_string_from_cstring(res);
}

VSObject *vs_string_bool(VSObject *strobj)
{
    VSTypeObject *type = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(type, T_STR, "str.__bool__");

    vs_size_t len = VSObject::c_getlen(strobj);
    INCREF_RET(len ? VS_TRUE : VS_FALSE);
}

VSObject *vs_string_char(VSObject *strobj)
{
    VSTypeObject *type = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(type, T_STR, "str.__char__");

    VSStringObject *str = (VSStringObject *)strobj;
    vs_size_t len = str->_value.length();
    if (len == 0)
    {
        return vs_char_from_cchar((cchar_t)0);
    }
    else if (len == 1)
    {
        return vs_char_from_cchar((cchar_t)str->_value[0]);
    }

    err("Can not cast string \"%s\" to char.", str->_value.c_str());
    terminate(TERM_ERROR);
}

VSObject *vs_string_int(VSObject *strobj, VSObject *baseobj)
{
    VSTypeObject *type = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(type, T_STR, "str.__int__");

    int base = 0;
    if (baseobj != NULL)
    {
        VS_ENSURE_TYPE(VS_TYPEOF(baseobj), T_INT, "str.__int__()");

        base = (int)vs_int_to_cint(baseobj);
        if (base < 0 || base == 1 || base > 36)
        {
            err("invalid int base: %d\n", base);
            terminate(TERM_ERROR);
        }
    }

    char *end = NULL;
    VSStringObject *str = (VSStringObject *)strobj;
    cint_t val = std::strtoll(str->_value.c_str(), &end, base);

    if (errno == ERANGE)
    {
        err("literal out of range of int: \"%s\"", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    if (end - 1 != &(str->_value.back()))
    {
        err("invalid literal: \"%s\"", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    return vs_int_from_cint(val);
}

VSObject *vs_string_float(VSObject *strobj)
{
    VSTypeObject *type = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(type, T_STR, "str.__float__");

    char *end = NULL;
    VSStringObject *str = (VSStringObject *)strobj;
    cfloat_t val = std::strtold(str->_value.c_str(), &end);

    if (errno == ERANGE)
    {
        errno = 0;
        err("literal out of range of float: \"%s\"", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    if (end - 1 != &(str->_value.back()))
    {
        err("invalid literal: \"%s\"", str->_value.c_str());
        terminate(TERM_ERROR);
    }

    return vs_float_from_cfloat(val);
}

VSObject *vs_string_len(VSObject *obj)
{
    VSTypeObject *type = VS_TYPEOF(obj);
    VS_ENSURE_TYPE(type, T_STR, "str len");

    return vs_int_from_cint(((VSStringObject *)obj)->_value.size());
}

VSObject *vs_string_get(VSObject *strobj, VSObject *posobj)
{
    int64_t pos = vs_int_to_cint(posobj);

    VSTypeObject *stype = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(stype, T_STR, "str get");
    VSStringObject *str = (VSStringObject *)strobj;

    if (pos < 0)
    {
        negative_index(pos);
        terminate(TERM_ERROR);
    }

    if (pos >= str->_value.size())
    {
        index_out_of_bound(pos, str->_value.size());
        terminate(TERM_ERROR);
    }

    return vs_char_from_cchar(str->_value.at(pos));
}

void vs_string_set(VSObject *strobj, VSObject *posobj, VSObject *charobj)
{
    int64_t pos = vs_int_to_cint(posobj);
    char char_val = vs_char_to_cchar(charobj);

    VSTypeObject *stype = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(stype, T_STR, "str set");
    VSStringObject *str = (VSStringObject *)strobj;

    if (pos < 0)
    {
        negative_index(pos);
        terminate(TERM_ERROR);
    }

    if (pos >= str->_value.size())
    {
        index_out_of_bound(pos, str->_value.size());
        terminate(TERM_ERROR);
    }

    str->_value[pos] = char_val;
}

void vs_string_append(VSObject *strobj, VSObject *charobj)
{
    char char_val = vs_char_to_cchar(charobj);

    VSTypeObject *stype = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(stype, T_STR, "str append");
    VSStringObject *str = (VSStringObject *)strobj;

    str->_value.push_back(char_val);
}

VSObject *vs_string_has(VSObject *strobj, VSObject *charobj)
{
    char char_val = vs_char_to_cchar(charobj);

    VSTypeObject *stype = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(stype, T_STR, "str has");
    VSStringObject *str = (VSStringObject *)strobj;

    if (str->_value.find(char_val) != str->_value.npos)
    {
        INCREF_RET(VS_TRUE);
    }
    INCREF_RET(VS_FALSE);
}

VSObject *vs_string_has_at(VSObject *strobj, VSObject *posobj)
{
    int64_t pos = vs_int_to_cint(posobj);

    VSTypeObject *stype = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(stype, T_STR, "str has at");
    VSStringObject *str = (VSStringObject *)strobj;

    if (pos < 0 || pos >= str->_value.size())
    {
        INCREF_RET(VS_TRUE);
    }

    INCREF_RET(VS_FALSE);
}

void vs_string_remove(VSObject *strobj, VSObject *charobj)
{
    char char_val = vs_char_to_cchar(charobj);

    VSTypeObject *stype = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(stype, T_STR, "str remove");
    VSStringObject *str = (VSStringObject *)strobj;

    std::size_t pos = str->_value.find_first_of(char_val);
    while (pos != str->_value.npos)
    {
        str->_value.erase(pos, 1);
        pos = str->_value.find_first_of(char_val, pos);
    }
}

void vs_string_remove_at(VSObject *strobj, VSObject *posobj)
{
    int64_t pos = vs_int_to_cint(posobj);

    VSTypeObject *stype = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(stype, T_STR, "str remove at");
    VSStringObject *str = (VSStringObject *)strobj;

    if (pos < 0)
    {
        negative_index(pos);
        terminate(TERM_ERROR);
    }

    if (pos >= str->_value.size())
    {
        index_out_of_bound(pos, str->_value.size());
        terminate(TERM_ERROR);
    }

    str->_value.erase(pos, 1);
}

inline std::string vs_string_to_cstring(VSObject *strobj)
{
    VSTypeObject *type = VS_TYPEOF(strobj);
    VS_ENSURE_TYPE(type, T_STR, "str to cstr");

    return ((VSStringObject *)strobj)->_value;
}

inline VSObject *vs_string_from_cstring(std::string strval)
{
    INCREF_RET(VS_AS_OBJECT(new VSStringObject(strval)));
}

NumberFuncs *number_funcs = new NumberFuncs(
    NULL, // __not__
    NULL, // __neg__
    vs_string_add, // __add__
    NULL, // __sub__
    NULL, // __mul__
    NULL, // __div__
    NULL, // __mod__
    NULL, // __and__
    NULL, // __or__
    vs_string_bool, // __bool__
    vs_string_char, // __char__
    vs_string_int, // __int__
    vs_string_float  // __float__
);

ContainerFuncs *string_funcs = new ContainerFuncs(
    vs_string_len,
    vs_string_get,
    vs_string_set,
    vs_string_append,
    vs_string_has,
    vs_string_has_at,
    vs_string_remove,
    vs_string_remove_at
);

VSTypeObject *VSStringType = new VSTypeObject(
    T_STR,
    "str", // __name__
    NULL,  // __attrs__
    vs_string_new,  // __new__
    vs_string_init,  // __init__
    vs_string_copy,  // __copy__
    vs_string_clear,  // __clear__
    NULL,  // __getattr__
    NULL,  // __hasattr__
    NULL,  // __setattr__
    NULL,  // __removeattr__
    vs_string_hash,  // __hash__
    vs_string_lt, // __lt__
    vs_string_eq,  // __eq__
    vs_string_str,  // __str__
    vs_string_bytes,  // __bytes__
    NULL,  // __call__
    number_funcs,  // _number_funcs
    string_funcs   // _container_funcs
);