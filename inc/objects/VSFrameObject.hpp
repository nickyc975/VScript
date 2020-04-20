#ifndef VS_FRAME_H
#define VS_FRAME_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"
#include "VSCellObject.hpp"
#include "VSCodeObject.hpp"
#include "VSTupleObject.hpp"
#include "VSFunctionObject.hpp"

#include <stack>

class VSFrameObject : public VSObject {
private:
    vs_addr_t pc;

    VSCodeObject *code;
    vs_size_t nlocals;
    VSTupleObject *locals;
    vs_size_t ncellvars;
    VSTupleObject *cellvars;
    vs_size_t nfreevars;
    VSTupleObject *freevars;
    vs_size_t nbuiltins;
    VSTupleObject *builtins;

    VSFrameObject *prev;

public:
    VSFrameObject(
        VSCodeObject *code, 
        VSTupleObject *args,
        VSTupleObject *cellvars, 
        VSTupleObject *freevars, 
        VSTupleObject *builtins, 
        VSFrameObject *prev);
    ~VSFrameObject();

    void eval(std::stack<VSObject *> &stack);
};

#endif