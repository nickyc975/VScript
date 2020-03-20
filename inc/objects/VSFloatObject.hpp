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

extern cfloat_t vs_float_to_cfloat(VSObject *floatobj);
extern VSObject *vs_float_from_cfloat(cfloat_t floatval);

#endif