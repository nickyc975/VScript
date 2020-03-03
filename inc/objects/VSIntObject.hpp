#ifndef VS_INT_H
#define VS_INT_H

#include "VSObject.hpp"

class VSIntObject;

VSIntObject *VS_INT_ZERO;
VSIntObject *VS_INT_ONE;

VSTypeObject *VSIntType;

int64_t vs_int_to_cint(VSObject *intobj);

VSObject *vs_int_from_cint(int64_t intval);

#endif