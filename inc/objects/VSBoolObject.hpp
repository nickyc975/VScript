#ifndef VS_BOOL_H
#define VS_BOOL_H

#include "VSObject.hpp"

extern VSObject *vs_bool(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSBoolObject : public VSObject {
private:
    static const str_func_map vs_bool_methods;
    static VSBoolObject *_VS_TRUE, *_VS_FALSE;
    VSBoolObject(cbool_t value);

public:
    const cbool_t _value;

    static inline VSObject *TRUE() {
        if (_VS_TRUE == NULL) {
            _VS_TRUE = new VSBoolObject(1);
            INCREF(_VS_TRUE);
        }
        return _VS_TRUE;
    }

    static inline VSObject *FALSE() {
        if (_VS_FALSE == NULL) {
            _VS_FALSE = new VSBoolObject(0);
            INCREF(_VS_FALSE);
        }
        return _VS_FALSE;
    }

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
};

#define VS_TRUE VSBoolObject::TRUE()
#define VS_FALSE VSBoolObject::FALSE()

#define C_BOOL_TO_BOOL(val) ((val) ? VS_TRUE : VS_FALSE)
#define BOOL_TO_C_BOOL(obj) (((VSBoolObject *)(obj))->_value)

#endif