#include "error.hpp"
#include "objects/VSCharObject.hpp"

class VSCharObject : public VSObject
{
public:
    cchar_t _value;

    VSCharObject() : _value(0) {this->type = VSCharType;}
    VSCharObject(cchar_t val) : _value(val) {this->type = VSCharType;}
};

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