#include "objects/VSCellObject.hpp"

#include <cassert>

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"

VSObject *vs_cell(VSObject *item) {
    assert(item != NULL);
    VSCellObject *cell = new VSCellObject(item);
    INCREF_RET(VS_AS_OBJECT(cell));
}

VSObject *vs_cell_hash(const VSObject *cellobj) {
    assert(cellobj != NULL);
    VS_ENSURE_TYPE(cellobj, T_CELL, "cell.__hash__()");
    INCREF_RET(C_INT_TO_INT((cint_t)cellobj));
}

VSObject *vs_cell_eq(const VSObject *a, const VSObject *b) {
    assert(a != NULL);
    assert(b != NULL);
    VS_ENSURE_TYPE(a, T_CELL, "cell.__eq__()");
    VS_ENSURE_TYPE(b, T_CELL, "cell.__eq__()");

    INCREF_RET(C_BOOL_TO_BOOL(a == b));
}

VSObject *vs_cell_str(VSObject *cellobj) {
    assert(cellobj != NULL);
    VS_ENSURE_TYPE(cellobj, T_CELL, "cell.__str__()");
    INCREF_RET(C_STRING_TO_STRING("cell"));
}

VSObject *vs_cell_bytes(VSObject *cellobj) {
    assert(cellobj != NULL);
    VS_ENSURE_TYPE(cellobj, T_CELL, "cell.__bytes__()");
    INCREF_RET(VS_NONE);
}

VSCellObject::VSCellObject(VSObject *item) {
    this->type = T_CELL;
    this->mut = true;
    this->item = item;
    INCREF(item);

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_cell_hash, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_cell_eq, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_cell_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_cell_bytes, 1, false);
}

VSCellObject::~VSCellObject() {
    DECREF_EX(this->item);
}