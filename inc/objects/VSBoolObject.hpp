#ifndef VS_BOOL_H
#define VS_BOOL_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

class VSBoolObject;

extern VSObject *VS_TRUE;
extern VSObject *VS_FALSE;

extern VSTypeObject *VSBoolType;

static inline cbool_t vs_bool_to_cbool(VSObject * boolobj);
static inline VSObject *vs_bool_from_cbool(cbool_t boolval);
static inline cbool_t is_true(VSObject *obj);

#endif