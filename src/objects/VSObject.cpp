#include "objects/VSObject.hpp"

#include "objects/VSBoolObject.hpp"

AttributeDef::AttributeDef(bool readonly, VSObject *attribute) {
    this->readonly = readonly;
    this->attribute = attribute;
    INCREF(this->attribute);
}

AttributeDef::~AttributeDef() {
    DECREF_EX(this->attribute);
}

VSObject::VSObject() {
    this->refcnt = 0;
    this->attrs = str_attr_map();
}

VSObject::~VSObject() {
    for (auto entry : this->attrs) {
        if (entry.second != NULL) {
            delete entry.second;
        }
    }
}

VSObject *vs_default_eq(const VSObject *a, const VSObject *b) {
    return C_BOOL_TO_BOOL(a == b);
}