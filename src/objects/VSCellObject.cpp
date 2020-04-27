#include "objects/VSCellObject.hpp"

#include <cassert>

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"

VSObject *vs_cell(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("cell()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSCellObject *cell = new VSCellObject(args[0]);
    INCREF_RET(VS_AS_OBJECT(cell));
}

VSObject *vs_cell_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    assert(self != NULL);
    if (nargs != 0) {
        ERR_NARGS("cell.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }
    VS_ENSURE_TYPE(self, T_CELL, "cell.__hash__()");
    INCREF_RET(C_INT_TO_INT((cint_t)self));
}

VSObject *vs_cell_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    assert(self != NULL);
    if (nargs != 0) {
        ERR_NARGS("cell.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }
    VS_ENSURE_TYPE(self, T_CELL, "cell.__hash__()");
    INCREF_RET(C_STRING_TO_STRING("cell"));
}

VSObject *vs_cell_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    assert(self != NULL);
    if (nargs != 0) {
        ERR_NARGS("cell.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }
    VS_ENSURE_TYPE(self, T_CELL, "cell.__hash__()");
    INCREF_RET(VS_NONE);
}

VSCellObject::VSCellObject(VSObject *item) {
    this->type = T_CELL;
    this->mut = true;
    this->item = item;
    INCREF(item);

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_cell_hash);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_default_eq);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_cell_str);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_cell_bytes);
}

VSCellObject::~VSCellObject() {
    DECREF_EX(this->item);
}