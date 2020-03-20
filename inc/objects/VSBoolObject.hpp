#ifndef VS_BOOL_H
#define VS_BOOL_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

extern VSTypeObject *VSBoolType;

class VSBoolObject : public VSObject {
private:
    static VSBoolObject *_VS_TRUE, *_VS_FALSE;
    VSBoolObject(cbool_t val) : _value(val ? 1 : 0) { this->type = VSBoolType; }

public:
    const cbool_t _value;

    static inline VSObject *TRUE() {
        if (_VS_TRUE == NULL) {
            _VS_TRUE = new VSBoolObject(1);
        }
        return _VS_TRUE;
    }

    static inline VSObject *FALSE() {
        if (_VS_FALSE == NULL) {
            _VS_FALSE = new VSBoolObject(0);
        }
        return _VS_FALSE;
    }
};

#define VS_TRUE VSBoolObject::TRUE()
#define VS_FALSE VSBoolObject::FALSE()

#define C_BOOL_TO_BOOL(val) ((val) ? VS_TRUE : VS_FALSE)
#define BOOL_TO_C_BOOL(obj) (((VSBoolObject *)(obj))->_value)

#endif