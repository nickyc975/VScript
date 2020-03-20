#ifndef VS_CHAR_H
#define VS_CHAR_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

extern VSTypeObject *VSCharType;

class VSCharObject : public VSObject
{
public:
    const cchar_t _value;

    VSCharObject() : _value(0) { this->type = VSCharType; }
    VSCharObject(cchar_t val) : _value(val) { this->type = VSCharType; }
};

extern cchar_t vs_char_to_cchar(VSObject * charobj);
extern VSObject *vs_char_from_cchar(cchar_t charval);

#endif