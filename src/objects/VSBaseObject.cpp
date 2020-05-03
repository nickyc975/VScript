#include "objects/VSBaseObject.hpp"

#include "error.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"

VSObject *vs_object(VSObject *, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("object()", 0, nargs);
        terminate(TERM_ERROR);
    }

    INCREF_RET(new VSBaseObject());
}

VSObject *vs_object_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("object.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_OBJECT, "object.__str__()");

    INCREF_RET(C_STRING_TO_STRING("<object at " + std::to_string((unsigned long)self) + ">"));
}

VSObject *vs_object_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("object.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_OBJECT, "object.__bytes__()");

    INCREF_RET(VS_NONE);
}

const str_func_map VSBaseObject::vs_object_methods = {
    {"__hash__", vs_default_hash},
    {"__eq__", vs_default_eq},
    {"__str__", vs_object_str},
    {"__bytes__", vs_object_bytes}};

VSBaseObject::VSBaseObject() {
    this->type = T_OBJECT;
    this->attrs = std::unordered_map<std::string, VSObject *>();
}

VSBaseObject::~VSBaseObject() {
}

bool VSBaseObject::hasattr(std::string &attrname) {
    if (this->attrs.find(attrname) != this->attrs.end()) {
        return true;
    } else if (vs_object_methods.find(attrname) != vs_object_methods.end()) {
        return true;
    }
    return false;
}

VSObject *VSBaseObject::getattr(std::string &attrname) {
    if (this->attrs.find(attrname) != this->attrs.end()) {
        INCREF_RET(this->attrs[attrname]);
    } else if (vs_object_methods.find(attrname) != vs_object_methods.end()) {
        VSFunctionObject *attr = new VSNativeFunctionObject(
            this, C_STRING_TO_STRING(attrname), vs_object_methods.at(attrname));
        INCREF_RET(attr);
    }

    ERR_NO_ATTR(this, attrname);
    terminate(TERM_ERROR);
    return NULL;
}

void VSBaseObject::setattr(std::string &attrname, VSObject *attrvalue) {
    auto iter = this->attrs.find(attrname);
    if (iter != this->attrs.end()) {
        DECREF_EX(iter->second);
    }

    if (attrvalue == NULL) {
        this->attrs.erase(attrname);
    } else {
        this->attrs[attrname] = attrvalue;
        INCREF(attrvalue);
    }
}