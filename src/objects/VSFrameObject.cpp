#include "objects/VSFrameObject.hpp"

#include <cassert>

#include "objects/VSCellObject.hpp"
#include "objects/VSDictObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSSetObject.hpp"
#include "objects/VSStringObject.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);

VSObject *vs_frame_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("frame.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FRAME, "frame.__str__()");

    INCREF_RET(C_STRING_TO_STRING("frame"));
}

VSObject *vs_frame_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("frame.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FRAME, "frame.__bytes__()");

    INCREF_RET(VS_NONE);
}

const str_func_map VSFrameObject::vs_frame_methods = {
    {ID___hash__, vs_default_hash},
    {ID___eq__, vs_default_eq},
    {ID___str__, vs_frame_str},
    {ID___bytes__, vs_frame_bytes}
};

VSFrameObject::VSFrameObject(VSCodeObject *code, VSTupleObject *args, VSTupleObject *cellvars, VSTupleObject *freevars, VSFrameObject *prev) {
    this->type = T_FRAME;

    this->pc = 0;

    assert(code != NULL);
    this->code = code;
    INCREF(code);

    this->nlocals = code->nlvars;
    this->locals = new VSTupleObject(this->nlocals);
    for (vs_size_t i = 0; i < this->nlocals; i++) {
        if (i < code->nargs) {
            if (i < code->nargs - 1 || !(code->flags & VS_FUNC_VARARGS)) {
                TUPLE_SET(this->locals, i, new VSCellObject(TUPLE_GET(args, i)));
            } else {
                VSTupleObject *va_args = new VSTupleObject(TUPLE_LEN(args) - code->nargs + 1);
                for (vs_size_t j = i; j < TUPLE_LEN(args); j++) {
                    TUPLE_SET(va_args, j - i, TUPLE_GET(args, j));
                }
                TUPLE_SET(this->locals, i, new VSCellObject(va_args));
            }
        } else {
            TUPLE_SET(this->locals, i, new VSCellObject(VS_NONE));
        }
    }
    INCREF(this->locals);

    this->cellvars = cellvars;
    if (cellvars != NULL) {
        this->ncellvars = TUPLE_LEN(cellvars);
        INCREF(cellvars);
    }

    this->freevars = freevars;
    if (freevars != NULL) {
        this->nfreevars = TUPLE_LEN(freevars);
        INCREF(freevars);
    }

    this->prev = prev;
    INCREF(prev);
}

VSFrameObject::~VSFrameObject() {
    DECREF_EX(this->code);
    DECREF_EX(this->locals);
    DECREF_EX(this->cellvars);
    DECREF_EX(this->freevars);
    DECREF_EX(this->prev);
}

bool VSFrameObject::hasattr(std::string &attrname) {
    return vs_frame_methods.find(attrname) != vs_frame_methods.end();
}

VSObject *VSFrameObject::getattr(std::string &attrname) {
    auto iter = vs_frame_methods.find(attrname);
    if (iter == vs_frame_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_frame_methods.at(attrname));
    INCREF_RET(attr);
}

void VSFrameObject::setattr(std::string &, VSObject *) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}
