#include "objects/VSObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSNoneObject.hpp"

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

VSObject *vs_default_hash(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("default __hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_INT_TO_INT((cint_t)self));
}

VSObject *vs_default_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("default __eq__()", 1, nargs);
        INCREF_RET(VS_NONE);
    }

    INCREF_RET(C_BOOL_TO_BOOL(self == args[0]));
}