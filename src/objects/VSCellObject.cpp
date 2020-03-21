#include "objects/VSCellObject.hpp"
#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

#include <cassert>

VSObject *vs_cell_new(VSObject *typeobj, VSObject *args, VSObject *) {
    assert(typeobj != NULL);
    VS_ENSURE_TYPE(VS_TYPEOF(typeobj), T_TYPE, "as type");
    VS_ENSURE_TYPE(VS_AS_TYPE(typeobj), T_CELL, "as cell type");

    if (args == NULL) {
        INCREF_RET(new VSCellObject(VS_NONE));
    }

    vs_size_t len = TUPLE_LEN(args);
    if (len == 0) {
        INCREF_RET(new VSCellObject(VS_NONE));
    } else if (len > 1) {
        err("cell.__new__() expects 1 arg but got %lld\n", len);
        INCREF_RET(VS_NONE);
    }

    VSObject *item = NULL;
    vs_tuple_unpack(args, 1, &item);
    if (item == NULL) {
        err("bad internal call: got NULL argument in cell.__new__()\n");
        INCREF_RET(VS_NONE);
    }

    VSCellObject *cell = new VSCellObject(item);
    DECREF(item);
    INCREF_RET(VS_AS_OBJECT(cell));
}

void vs_cell_init(VSObject *, VSObject *, VSObject *) {
}

VSObject *vs_cell_copy(const VSObject *that) {
    assert(that != NULL);
    VS_ENSURE_TYPE(VS_TYPEOF(that), T_CELL, "cell.__copy__()");

    VSCellObject *old = (VSCellObject *)that;
    VSCellObject *cell = new VSCellObject(old->item);
    INCREF_RET(VS_AS_OBJECT(cell));
}

void vs_cell_clear(VSObject *cellobj) {
    assert(cellobj != NULL);
    VS_ENSURE_TYPE(VS_TYPEOF(cellobj), T_CELL, "cell.__clear__()");

    VSCellObject *cell = AS_CELL(cellobj);
    DECREF(cell->item);
    cell->item = NULL;
}

VSObject *vs_cell_hash(const VSObject *cellobj) {
    assert(cellobj != NULL);
    VS_ENSURE_TYPE(VS_TYPEOF(cellobj), T_CELL, "cell.__hash__()");
    INCREF_RET(C_INT_TO_INT((cint_t)cellobj));
}

VSObject *vs_cell_eq(const VSObject *a, const VSObject *b) {
    assert(a != NULL);
    assert(b != NULL);
    VS_ENSURE_TYPE(VS_TYPEOF(a), T_CELL, "cell.__eq__()");
    VS_ENSURE_TYPE(VS_TYPEOF(b), T_CELL, "cell.__eq__()");

    INCREF_RET(C_BOOL_TO_BOOL(a == b));
}

VSObject *vs_cell_str(VSObject *cellobj) {
    assert(cellobj != NULL);
    VS_ENSURE_TYPE(VS_TYPEOF(cellobj), T_CELL, "cell.__str__()");
    INCREF_RET(C_STRING_TO_STRING("cell"));
}

VSObject *vs_cell_bytes(VSObject *cellobj) {
    assert(cellobj != NULL);
    VS_ENSURE_TYPE(VS_TYPEOF(cellobj), T_CELL, "cell.__bytes__()");
    INCREF_RET(VS_NONE);
}

VSTypeObject *VSCellType = new VSTypeObject(
    VSTypeType,
    T_CELL,
    "cell",         // __name__
    NULL,           // __attrs__
    vs_cell_new,    // __new__
    vs_cell_init,   // __init__
    vs_cell_copy,   // __copy__
    vs_cell_clear,  // __clear__
    NULL,           // __getattr__
    NULL,           // __hasattr__
    NULL,           // __setattr__
    NULL,           // __removeattr__
    vs_cell_hash,   // __hash__
    NULL,           // __lt__
    vs_cell_eq,     // __eq__
    vs_cell_str,    // __str__
    vs_cell_bytes,  // __bytes__
    NULL,           // __call__
    NULL,           // _number_funcs
    NULL            // _container_funcs
);