#include "error.hpp"
#include "objects/VSCharObject.hpp"

class VSCharObject : public VSObject
{
public:
    char _value;

    VSCharObject() : _value(0) {this->type = VSCharType;}
    VSCharObject(char val) : _value(val) {this->type = VSCharType;}
};

char vs_char_to_cchar(VSObject * charobj)
{
    VSTypeObject *type = vs_typeof(charobj);
    vs_ensure_type(type, T_CHAR, "char to cchar");

    return ((VSCharObject *)charobj)->_value;
}

VSObject *vs_char_from_cchar(char charval)
{
    return vs_as_object(new VSCharObject(charval));
}