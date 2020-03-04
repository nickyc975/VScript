#include "error.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSStringObject.hpp"

class VSStringObject : public VSObject
{
public:
    std::string _value;

    VSStringObject() : _value("") {this->type = VSStringType;}
    VSStringObject(std::string val) : _value(val) {this->type = VSStringType;}
};

VSObject *vs_string_len(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);
    vs_ensure_type(type, T_STR, "str len");

    return vs_int_from_cint(((VSStringObject *)obj)->_value.size());
}

VSObject *vs_string_get(VSObject *strobj, VSObject *posobj)
{
    int64_t pos = vs_int_to_cint(posobj);

    VSTypeObject *stype = vs_typeof(strobj);
    vs_ensure_type(stype, T_STR, "str get");
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

    VSTypeObject *stype = vs_typeof(strobj);
    vs_ensure_type(stype, T_STR, "str get");
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

    VSTypeObject *stype = vs_typeof(strobj);
    vs_ensure_type(stype, T_STR, "str get");
    VSStringObject *str = (VSStringObject *)strobj;

    str->_value.push_back(char_val);
}

VSObject *vs_string_has_at(VSObject *strobj, VSObject *posobj)
{
    int64_t pos = vs_int_to_cint(posobj);

    VSTypeObject *stype = vs_typeof(strobj);
    vs_ensure_type(stype, T_STR, "str get");
    VSStringObject *str = (VSStringObject *)strobj;

    if (pos < 0 || pos >= str->_value.size())
    {
        return vs_as_object(VS_FALSE);
    }

    return vs_as_object(VS_TRUE);
}

void vs_string_remove_at(VSObject *strobj, VSObject *posobj)
{
    int64_t pos = vs_int_to_cint(posobj);

    VSTypeObject *stype = vs_typeof(strobj);
    vs_ensure_type(stype, T_STR, "str get");
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

std::string vs_string_to_cstring(VSObject *strobj)
{
    VSTypeObject *type = vs_typeof(strobj);
    vs_ensure_type(type, T_STR, "str to cstr");

    return ((VSStringObject *)strobj)->_value;
}

VSObject *vs_string_from_cstring(std::string strval)
{
    return vs_as_object(new VSStringObject(strval));
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

ContainerFuncs *string_funcs = new ContainerFuncs(
    vs_string_len,
    vs_string_get,
    vs_string_set,
    vs_string_append,
    NULL,
    vs_string_has_at,
    NULL,
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
    vs_string_eq,  // __eq__
    vs_string_str,  // __str__
    vs_string_bytes,  // __bytes__
    NULL,  // __call__
    number_funcs,  // _number_funcs
    string_funcs   // _container_funcs
);