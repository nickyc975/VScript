#include "error.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSStringObject.hpp"

class VSCharObject : public VSObject
{
public:
    cchar_t _value;

    VSCharObject() : _value(0) {this->type = VSCharType;}
    VSCharObject(cchar_t val) : _value(val) {this->type = VSCharType;}
};

VSObject *vs_char_hash(const VSObject *charobj)
{
    VSTypeObject *type = vs_typeof(charobj);

    vs_ensure_type(type, T_CHAR, "char hash");

    return vs_int_from_cint(((VSCharObject *)charobj)->_value);
}

VSObject *vs_char_eq(const VSObject *a, const VSObject *b)
{
    VSTypeObject *a_type = vs_typeof(a);

    vs_ensure_type(a_type, T_BOOL, "char eq");

    VSTypeObject *b_type = vs_typeof(b);

    vs_ensure_type(b_type, T_BOOL, "char eq");

    bool res = ((VSCharObject *)a)->_value == ((VSCharObject *)b)->_value;
    return res ? VS_TRUE : VS_FALSE;
}

VSObject *vs_char_str(VSObject *charobj)
{
    VSTypeObject *type = vs_typeof(charobj);
    vs_ensure_type(type, T_CHAR, "char to str");

    auto str = std::string();
    str.push_back(((VSCharObject *)charobj)->_value);
    return vs_string_from_cstring(str);
}

VSObject *vs_char_bytes(VSObject *charobj)
{
    VSTypeObject *type = vs_typeof(charobj);

    vs_ensure_type(type, T_CHAR, "char to bytes");

    return NULL;
}

VSObject *vs_char_bool(VSObject *charobj)
{
    VSTypeObject *type = vs_typeof(charobj);
    vs_ensure_type(type, T_CHAR, "__bool__()");

    cchar_t val = ((VSCharObject *)charobj)->_value;
    return val ? VS_TRUE : VS_FALSE;
}

VSObject *vs_char_char(VSObject *charobj)
{
    VSTypeObject *type = vs_typeof(charobj);
    vs_ensure_type(type, T_CHAR, "__char__()");

    return charobj;
}

VSObject *vs_char_int(VSObject *charobj)
{
    VSTypeObject *type = vs_typeof(charobj);
    vs_ensure_type(type, T_CHAR, "__int__()");

    return vs_int_from_cint((cint_t)vs_char_to_cchar(charobj));
}

cchar_t vs_char_to_cchar(VSObject * charobj)
{
    VSTypeObject *type = vs_typeof(charobj);
    vs_ensure_type(type, T_CHAR, "cchar_t to cchar");

    return ((VSCharObject *)charobj)->_value;
}

VSObject *vs_char_from_cchar(cchar_t charval)
{
    return vs_as_object(new VSCharObject(charval));
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
    vs_char_bool, // __bool__
    vs_char_char, // __char__
    vs_char_int, // __int__
    NULL  // __float__
);

VSTypeObject *VSCharType = new VSTypeObject(
    T_CHAR,
    "char", // __name__
    NULL,  // __attrs__
    NULL,  // __new__
    NULL,  // __init__
    NULL,  // __copy__
    NULL,  // __clear__
    NULL,  // __getattr__
    NULL,  // __hasattr__
    NULL,  // __setattr__
    NULL,  // __removeattr__
    vs_char_hash,  // __hash__
    vs_char_eq,  // __eq__
    vs_char_str,  // __str__
    vs_char_bytes,  // __bytes__
    NULL,  // __call__
    number_funcs,  // _number_funcs
    NULL   // _container_funcs
);