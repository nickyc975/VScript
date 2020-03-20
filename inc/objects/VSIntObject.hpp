#ifndef VS_INT_H
#define VS_INT_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

extern VSTypeObject *VSIntType;

class VSIntObject : public VSObject
{
private:
    static VSIntObject *_VS_ZERO, *_VS_ONE;
public:
    const cint_t _value;

    VSIntObject() : _value(0) { this->type = VSIntType; }
    VSIntObject(cint_t val) : _value(val) { this->type = VSIntType; }

    static inline VSObject *ZERO() {
        if (_VS_ZERO == NULL) {
            _VS_ZERO = new VSIntObject(0);
        }
        return _VS_ZERO;
    }

    static inline VSObject *ONE() {
        if (_VS_ONE == NULL) {
            _VS_ONE = new VSIntObject(1);
        }
        return _VS_ONE;
    }
};

extern cint_t vs_int_to_cint(VSObject *intobj);
extern VSObject *vs_int_from_cint(cint_t intval);

#define VS_ZERO VSIntObject::ZERO()
#define VS_ONE VSIntObject::ONE()

#endif