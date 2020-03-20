#include "error.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSObject.hpp"

VSObject *vs_hash_not_implemented(const VSObject *obj)
{
    err("%s.__hash__() not implemented.", VS_TYPEOF(obj)->__name__.c_str());
    terminate(TERM_ERROR);
    INCREF_RET(VS_ZERO);
}

VSObject *vs_default_eq(const VSObject *a, const VSObject *b)
{
    return C_BOOL_TO_BOOL(a == b);
}