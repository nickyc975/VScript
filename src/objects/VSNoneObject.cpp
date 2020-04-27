#include "objects/VSNoneObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"

VSNoneObject *VSNoneObject::_VS_NONE = NULL;

VSObject *vs_none_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("none.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_NONE, "none.__str__()");

    INCREF_RET(C_STRING_TO_STRING("none"));
}

VSObject *vs_none_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("none.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_NONE, "none.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSNoneObject::VSNoneObject() {
    this->type = T_NONE;

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_default_hash);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_default_eq);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_none_str);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_none_bytes);
}
