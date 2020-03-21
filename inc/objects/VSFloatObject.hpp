#ifndef VS_FLOAT_H
#define VS_FLOAT_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

extern VSTypeObject *VSFloatType;

class VSFloatObject : public VSObject
{
public:
    const cfloat_t _value;

    VSFloatObject() : _value(0.0L) { this->type = VSFloatType; }
    VSFloatObject(cfloat_t val) : _value(val) { this->type = VSFloatType; }
};

#define AS_FLOAT(obj) ((VSFloatObject *)obj)
#define FLOAT_TO_C_FLOAT(obj) (AS_FLOAT(obj)->_value)
#define C_FLOAT_TO_FLOAT(val) (new VSFloatObject(val))

#endif