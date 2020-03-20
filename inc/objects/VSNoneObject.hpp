#ifndef VS_NONE_H
#define VS_NONE_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

extern VSTypeObject *VSNoneType;

class VSNoneObject : public VSObject
{
private:
    static VSNoneObject *_VS_NONE;

    VSNoneObject() { this->type = VSNoneType; }

public:
    static inline VSObject *NONE() {
        if (_VS_NONE == NULL) {
            _VS_NONE = new VSNoneObject();
        }
        return _VS_NONE;
    }
};

#define VS_NONE VSNoneObject::NONE()

#endif