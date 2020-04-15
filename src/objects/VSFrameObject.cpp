#include "objects/VSFrameObject.hpp"

#include "objects/VSCodeObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"

VSFrameObject::VSFrameObject(VSCodeObject *code, VSFrameObject *prev) {
    this->type = VSFrameType;

    this->pc = 0;
    this->func = NULL;

    this->code = code;
    INCREF(this->code);

    this->nlocals = code->nlvars;

    this->locals = new VSTupleObject(this->nlocals);
    for (vs_size_t i = 0; i < this->nlocals; i++) {
        TUPLE_SET(this->locals, i, new VSCellObject(VS_NONE));
    }
    INCREF(this->locals);

    this->prev = prev;
    INCREF(prev);
}

VSFrameObject::VSFrameObject(VSFunctionObject *func, VSFrameObject *prev) {
    this->type = VSFrameType;

    this->pc = 0;
    this->func = func;
    INCREF(this->func);

    this->code = AS_CODE(func->code);
    INCREF(this->code);

    this->nlocals = AS_CODE(FUNC_GET_CODE(this->func))->nlvars;

    this->locals = new VSTupleObject(this->nlocals);
    for (vs_size_t i = 0; i < this->nlocals; i++) {
        TUPLE_SET(this->locals, i, new VSCellObject(VS_NONE));
    }
    INCREF(this->locals);

    this->prev = prev;
    INCREF(prev);
}

void vs_frame_clear(VSObject *frameobj) {
    VSTypeObject *type = VS_TYPEOF(frameobj);
    VS_ENSURE_TYPE(type, T_FRAME, "frame clear");

    VSFrameObject *frame = AS_FRAME(frameobj);
    DECREF_EX(frame->code);
    DECREF_EX(frame->func);
    DECREF_EX(frame->locals);
    DECREF_EX(frame->prev);
}

VSObject *vs_frame_str(VSObject *frameobj) {
    VSTypeObject *type = VS_TYPEOF(frameobj);
    VS_ENSURE_TYPE(type, T_FRAME, "frame str");

    INCREF_RET(C_STRING_TO_STRING("frame"));
}

VSObject *vs_frame_bytes(VSObject *frameobj) {
    VSTypeObject *type = VS_TYPEOF(frameobj);
    VS_ENSURE_TYPE(type, T_FRAME, "frame bytes");

    return NULL;
}

VSTypeObject *VSFrameType = new VSTypeObject(
    VSTypeType,
    T_FRAME,
    "frame",                  // __name__
    NULL,                     // __attrs__
    NULL,                     // __new__
    NULL,                     // __init__
    NULL,                     // __copy__
    NULL,                     // __clear__
    NULL,                     // __getattr__
    NULL,                     // __hasattr__
    NULL,                     // __setattr__
    NULL,                     // __removeattr__
    vs_hash_not_implemented,  // __hash__
    NULL,                     // __lt__
    vs_default_eq,            // __eq__
    vs_frame_str,             // __str__
    vs_frame_bytes,           // __bytes__
    NULL,                     // __call__
    NULL,                     // _number_funcs
    NULL                      // _container_funcs
);