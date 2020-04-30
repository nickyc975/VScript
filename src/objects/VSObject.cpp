#include "objects/VSObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSNoneObject.hpp"

VSObject::VSObject() {
    this->refcnt = 0;
}

VSObject::~VSObject() {
}

bool VSObject::hasattr(std::string &) {
    return false;
}

VSObject *VSObject::getattr(std::string &) {
    INCREF_RET(VS_NONE);
}

void VSObject::setattr(std::string &, VSObject *) {
    return;
}

VSObject *vs_default_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("default __hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_INT_TO_INT((cint_t)self));
}

VSObject *vs_default_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("default __eq__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_BOOL_TO_BOOL(self == args[0]));
}