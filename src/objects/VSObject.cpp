#include "objects/VSObject.hpp"

#include "objects/VSBoolObject.hpp"

VSObject *vs_default_eq(const VSObject *a, const VSObject *b) {
    return C_BOOL_TO_BOOL(a == b);
}