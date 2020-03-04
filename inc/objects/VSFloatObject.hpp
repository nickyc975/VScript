#ifndef VS_FLOAT_H
#define VS_FLOAT_H

#include "VSObject.hpp"

class VSFloatObject;

extern VSTypeObject *VSFloatType;

cfloat_t vs_float_to_cfloat(VSObject *floatobj);
VSObject *vs_float_from_cfloat(cfloat_t floatval);

#endif