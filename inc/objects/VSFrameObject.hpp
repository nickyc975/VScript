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
    ~VSFrameObject();
};

#define AS_FRAME(obj) ((VSFrameObject *)(obj))
#define FRAME_GET_FUNC(frame) (AS_FRAME(frame)->func)
#define FRAME_GET_NLOCALS(frame) (TUPLE_LEN(AS_FRAME(frame)->locals))
#define FRAME_GET_LOCAL(frame, idx) (VS_CELL_GET((TUPLE_GET(AS_FRAME(frame)->locals, (idx)))))
#define FRAME_SET_LOCAL(frame, idx, val) (VS_CELL_SET((TUPLE_GET(AS_FRAME(frame)->locals, (idx))), val))

#endif