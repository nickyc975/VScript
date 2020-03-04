#ifndef VS_BOOL_H
#define VS_BOOL_H

#include "VSObject.hpp"

class VSBoolObject;

extern VSObject *VS_TRUE;
extern VSObject *VS_FALSE;

extern VSTypeObject *VSBoolType;

uint8_t vs_bool_to_cbool(VSObject * boolobj);

VSObject *vs_bool_from_cbool(uint8_t boolval);

#endif