#ifndef VS_FLOAT_H
#define VS_FLOAT_H

#include "VSObject.hpp"

extern VSObject *vs_float(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSFloatObject : public VSObject {
public:
    const cfloat_t _value;

    VSFloatObject(cfloat_t value);
    ~VSFloatObject();
};

#define AS_FLOAT(obj) ((VSFloatObject *)obj)
#define FLOAT_TO_C_FLOAT(obj) (AS_FLOAT(obj)->_value)
#define C_FLOAT_TO_FLOAT(val) (new VSFloatObject(val))

#endif