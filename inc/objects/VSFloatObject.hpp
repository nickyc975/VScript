#ifndef VS_FLOAT_H
#define VS_FLOAT_H

#include "VSObject.hpp"

extern VSObject *vs_float(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSFloatObject : public VSObject {
private:
    static const str_func_map vs_float_methods;

public:
    const cfloat_t _value;

    VSFloatObject(cfloat_t value);
    ~VSFloatObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
};

#define AS_FLOAT(obj) ((VSFloatObject *)obj)
#define FLOAT_TO_C_FLOAT(obj) (AS_FLOAT(obj)->_value)
#define C_FLOAT_TO_FLOAT(val) (new VSFloatObject(val))

#endif