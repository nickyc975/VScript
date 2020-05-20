#ifndef VS_INTERPRETOR_H
#define VS_INTERPRETOR_H

#include <stack>

#include "objects/VSCodeObject.hpp"
#include "objects/VSFrameObject.hpp"
#include "objects/VSTupleObject.hpp"

typedef std::stack<VSObject *> cpt_stack_t;

class VSInterpretor {
public:
    VSTupleObject *builtins;

    VSInterpretor(VSTupleObject *builtins);
    ~VSInterpretor();

    void exec(
        vs_addr_t &pc,
        cpt_stack_t &stack,
        VSCodeObject *code, 
        VSTupleObject *locals, 
        VSTupleObject *freevars, 
        VSTupleObject *cellvars, 
        VSTupleObject *globals
    );

    void eval(cpt_stack_t &stack, VSFrameObject *frame);
};

extern const VSInterpretor INTERPRETOR;

#endif