#ifndef VS_FLOAT_H
#define VS_FLOAT_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

class VSFloatObject;

extern VSTypeObject *VSFloatType;

static inline cfloat_t vs_float_to_cfloat(VSObject *floatobj);
static inline VSObject *vs_float_from_cfloat(cfloat_t floatval);

#endif