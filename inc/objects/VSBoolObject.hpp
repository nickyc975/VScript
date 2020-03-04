#ifndef VS_BOOL_H
#define VS_BOOL_H

#include "VSObject.hpp"

class VSBoolObject;

extern VSObject *VS_TRUE;
extern VSObject *VS_FALSE;

extern VSTypeObject *VSBoolType;

cbool_t vs_bool_to_cbool(VSObject * boolobj);

VSObject *vs_bool_from_cbool(cbool_t boolval);

static inline cbool_t is_true(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);
    vs_ensure_type(type, T_BOOL, "is true");
    return (cbool_t)(obj == VS_TRUE);
}

#endif