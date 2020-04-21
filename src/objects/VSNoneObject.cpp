#include "objects/VSNoneObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSStringObject.hpp"

VSNoneObject *VSNoneObject::_VS_NONE = NULL;

VSObject *vs_none_hash(const VSObject *noneobj) {
    VS_ENSURE_TYPE(noneobj, T_NONE, "none.__hash__()");

    INCREF_RET(C_INT_TO_INT(0));
}

VSObject *vs_none_eq(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_NONE, "none.__eq__()");
    VS_ENSURE_TYPE(b, T_NONE, "none.__eq__()");

    INCREF_RET(VS_TRUE);
}

VSObject *vs_none_str(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_NONE, "none.__str__()");

    INCREF_RET(C_STRING_TO_STRING("none"));
}

VSObject *vs_none_bytes(VSObject *obj) {
    VS_ENSURE_TYPE(obj, T_NONE, "none.__bytes__()");

    return NULL;
}

VSNoneObject::VSNoneObject() {
    this->type = T_NONE;

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_none_hash, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_none_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_none_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_none_bytes, 1, false);
}
