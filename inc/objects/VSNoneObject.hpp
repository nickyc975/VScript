#ifndef VS_NONE_H
#define VS_NONE_H

#include "VSObject.hpp"

class VSNoneObject : public VSObject {
private:
    static VSNoneObject *_VS_NONE;
    static const str_func_map vs_none_methods;

    VSNoneObject();

public:
    static inline VSObject *NONE() {
        if (_VS_NONE == NULL) {
            _VS_NONE = new VSNoneObject();
        }
        return _VS_NONE;
    }

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
};

#define VS_NONE VSNoneObject::NONE()

#endif