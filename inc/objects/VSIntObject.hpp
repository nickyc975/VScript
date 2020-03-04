#ifndef VS_INT_H
#define VS_INT_H

#include "VSObject.hpp"

class VSIntObject;

extern VSObject *VS_INT_ZERO;
extern VSObject *VS_INT_ONE;

extern VSTypeObject *VSIntType;

cint_t vs_int_to_cint(VSObject *intobj);

VSObject *vs_int_from_cint(cint_t intval);

#endif