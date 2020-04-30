#include "objects/VSCellObject.hpp"

#include <cassert>

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);

VSObject *vs_cell(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("cell()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSCellObject *cell = new VSCellObject(args[0]);
    INCREF_RET(AS_OBJECT(cell));
}

VSObject *vs_cell_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    assert(self != NULL);
    if (nargs != 0) {
        ERR_NARGS("cell.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }
    ENSURE_TYPE(self, T_CELL, "cell.__hash__()");
    INCREF_RET(C_INT_TO_INT((cint_t)self));
}

VSObject *vs_cell_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    assert(self != NULL);
    if (nargs != 0) {
        ERR_NARGS("cell.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }
    ENSURE_TYPE(self, T_CELL, "cell.__hash__()");
    INCREF_RET(C_STRING_TO_STRING("cell"));
}

VSObject *vs_cell_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    assert(self != NULL);
    if (nargs != 0) {
        ERR_NARGS("cell.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }
    ENSURE_TYPE(self, T_CELL, "cell.__hash__()");
    INCREF_RET(VS_NONE);
}

const str_func_map VSCellObject::vs_cell_methods = {
   {ID___hash__, vs_cell_hash},
   {ID___eq__, vs_default_eq},
   {ID___str__, vs_cell_str},
   {ID___bytes__, vs_cell_bytes}
};

VSCellObject::VSCellObject(VSObject *item) {
    this->type = T_CELL;
    this->mut = true;
    this->item = item;
    INCREF(item);
}

VSCellObject::~VSCellObject() {
    DECREF_EX(this->item);
}

bool VSCellObject::hasattr(std::string &attrname) {
    return vs_cell_methods.find(attrname) != vs_cell_methods.end();
}

VSObject *VSCellObject::getattr(std::string &attrname) {
    auto iter = vs_cell_methods.find(attrname);
    if (iter == vs_cell_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_cell_methods.at(attrname));
    INCREF_RET(attr);
}

void VSCellObject::setattr(std::string &, VSObject *) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}