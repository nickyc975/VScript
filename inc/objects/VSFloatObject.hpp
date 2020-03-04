#ifndef VS_FLOAT_H
#define VS_FLOAT_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

class VSFloatObject;

extern VSTypeObject *VSFloatType;

extern cfloat_t vs_float_to_cfloat(VSObject *floatobj);
extern VSObject *vs_float_from_cfloat(cfloat_t floatval);

#endif