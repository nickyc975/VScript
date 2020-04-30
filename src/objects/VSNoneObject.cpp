#include "objects/VSNoneObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);

VSNoneObject *VSNoneObject::_VS_NONE = NULL;

VSObject *vs_none_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("none.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_NONE, "none.__str__()");

    INCREF_RET(C_STRING_TO_STRING("none"));
}

VSObject *vs_none_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("none.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_NONE, "none.__bytes__()");

    INCREF_RET(VS_NONE);
}

const str_func_map VSNoneObject::vs_none_methods = {
    {ID___hash__, vs_default_hash},
    {ID___eq__, vs_default_eq},
    {ID___str__, vs_none_str},
    {ID___bytes__, vs_none_bytes}
};

VSNoneObject::VSNoneObject() {
    this->type = T_NONE;
}

bool VSNoneObject::hasattr(std::string &attrname) {
    return vs_none_methods.find(attrname) != vs_none_methods.end();
}

VSObject *VSNoneObject::getattr(std::string &attrname) {
    auto iter = vs_none_methods.find(attrname);
    if (iter == vs_none_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_none_methods.at(attrname));
    INCREF_RET(attr);
}

void VSNoneObject::setattr(std::string &, VSObject *) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}
