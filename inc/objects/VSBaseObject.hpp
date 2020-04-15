#ifndef VS_BASE_OBJECT_H
#define VS_BASE_OBJECT_H

#include "VSObject.hpp"
#include "VSMapObject.hpp"
#include "VSTypeObject.hpp"

extern VSTypeObject *VSBaseObjectType;

class VSBaseObject : public VSObject {
public:
    VSMapObject *attrs;

    VSBaseObject(VSMapObject *attrs) {
        this->type = VSBaseObjectType;
        if (attrs == NULL) {
            this->attrs = new VSMapObject();
        } else {
            this->attrs = attrs;
        }
        INCREF(this->attrs);
    }
    ~VSBaseObject() = default;
};

#define GET_ATTR(obj, attrname) MAP_GET(((VSBaseObject *)obj)->attrs, (attrname))
#define SET_ATTR(obj, attrname, value) MAP_SET(((VSBaseObject *)obj)->attrs, (attrname), (value))
#define HAS_ATTR(obj, attrname) MAP_HAS(((VSBaseObject *)obj)->attrs, (attrname))

#endif