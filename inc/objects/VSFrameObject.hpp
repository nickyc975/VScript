#ifndef VS_FRAME_H
#define VS_FRAME_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"
#include "VSCellObject.hpp"
#include "VSTupleObject.hpp"
#include "VSFunctionObject.hpp"

extern VSTypeObject *VSFrameType;

class VSFrameObject : VSObject {
public:
    vs_addr_t pc;
    VSFunctionObject *func;
    vs_addr_t nlocals;
    VSTupleObject *locals;

    VSFrameObject(VSFunctionObject *func);
    ~VSFrameObject() = default;
};

#define AS_FRAME(obj) ((VSFrameObject *)(obj))
#define FRAME_GET_FUNC(frame) (AS_FRAME(frame)->func)
#define FRAME_GET_NLOCALS(frame) (TUPLE_LEN(AS_FRAME(frame)->locals))

#define FRAME_GET_LOCAL(frame, idx) (VS_CELL_GET((TUPLE_GET(AS_FRAME(frame)->locals, (idx)))))
#define FRAME_GET_LOCAL_CELL(frame, idx) (TUPLE_GET(AS_FRAME(frame)->locals, (idx)))
#define FRAME_SET_LOCAL(frame, idx, val) (VS_CELL_SET((TUPLE_GET(AS_FRAME(frame)->locals, (idx))), val))
#define FRAME_SET_LOCAL_CELL(frame, idx, cell) (TUPLE_SET(AS_FRAME(frame)->locals, (idx), (cell)))

#define FRAME_GET_FREE(frame, idx) (VS_CELL_GET(TUPLE_GET(FUNC_GET_FREEVARS((AS_FRAME(frame))->func), (idx))))
#define FRAME_GET_FREE_CELL(frame, idx) (TUPLE_GET(FUNC_GET_FREEVARS((AS_FRAME(frame))->func), (idx)))

#define FRAME_GET_BUILTIN(frame, idx) (TUPLE_GET(FUNC_GET_BUILTINS((AS_FRAME(frame))->func), (idx)))

#endif