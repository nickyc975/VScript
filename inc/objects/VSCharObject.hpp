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

#define AS_CHAR(obj) ((VSCharObject *)obj)
#define CHAR_TO_C_CHAR(obj) (AS_CHAR(obj)->_value)
#define C_CHAR_TO_CHAR(val) (new VSCharObject(val))

#endif