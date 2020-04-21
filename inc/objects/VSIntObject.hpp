#ifndef VS_INT_H
#define VS_INT_H

#include "VSObject.hpp"

class VSIntObject : public VSObject {
private:
    static VSIntObject *_VS_ZERO, *_VS_ONE;

public:
    const cint_t _value;

    VSIntObject(cint_t value);
    ~VSIntObject();

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

#define VS_ZERO VSIntObject::ZERO()
#define VS_ONE VSIntObject::ONE()

#define AS_INT(obj) ((VSIntObject *)obj)
#define INT_TO_C_INT(obj) (AS_INT(obj)->_value)
#define C_INT_TO_INT(val) (new VSIntObject(val))

#endif