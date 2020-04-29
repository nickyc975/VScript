#ifndef VS_INT_H
#define VS_INT_H

#include "VSObject.hpp"

extern VSObject *vs_int(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSIntObject : public VSObject {
private:
    static VSIntObject *_VS_ZERO, *_VS_ONE;
    static const str_func_map vs_int_methods;

public:
    const cint_t _value;

    VSIntObject(cint_t value);
    ~VSIntObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;

    static inline VSObject *ZERO() {
        if (_VS_ZERO == NULL) {
            _VS_ZERO = new VSIntObject(0);
            INCREF(_VS_ZERO);
        }
        return _VS_ZERO;
    }

    static inline VSObject *ONE() {
        if (_VS_ONE == NULL) {
            _VS_ONE = new VSIntObject(1);
            INCREF(_VS_ONE);
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