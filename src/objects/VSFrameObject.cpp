#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSCodeObject.hpp"
#include "objects/VSFrameObject.hpp"

VSFrameObject::VSFrameObject(VSFunctionObject *func)
{
    this->pc = 0;
    this->func = func;
    this->nlocals = AS_CODE(FUNC_GET_CODE(this->func))->nlvars;
    this->locals = new VSTupleObject(this->nlocals);

    for (vs_size_t i = 0; i < this->nlocals; i++)
    {
        TUPLE_SET(this->locals, i, new VSCellObject(VS_NONE));
    }
}

VSFrameObject::~VSFrameObject()
{
}

VSObject *vs_frame_str(VSObject *frameobj)
{
    VSTypeObject *type = VS_TYPEOF(frameobj);
    VS_ENSURE_TYPE(type, T_FRAME, "frame str");

    INCREF_RET(C_STRING_TO_STRING("frame"));
}

VSObject *vs_frame_bytes(VSObject *frameobj)
{
    VSTypeObject *type = VS_TYPEOF(frameobj);
    VS_ENSURE_TYPE(type, T_FRAME, "frame bytes");

    return NULL;
}

VSTypeObject *VSFrameType = new VSTypeObject(
    VSTypeType,
    T_FRAME,
    "frame",            // __name__
    NULL,               // __attrs__
    NULL,       // __new__
    NULL,      // __init__
    NULL,      // __copy__
    NULL,               // __clear__
    NULL,               // __getattr__
    NULL,               // __hasattr__
    NULL,               // __setattr__
    NULL,               // __removeattr__
    vs_hash_not_implemented,      // __hash__
    NULL,        // __lt__
    vs_default_eq,        // __eq__
    vs_frame_str,       // __str__
    vs_frame_bytes,     // __bytes__
    NULL,               // __call__
    NULL, // _number_funcs
    NULL                // _container_funcs
);