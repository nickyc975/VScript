#ifndef VS_INTERPRETOR_H
#define VS_INTERPRETOR_H

#include <stack>

#include "objects/VSCodeObject.hpp"
#include "objects/VSFrameObject.hpp"
#include "objects/VSTupleObject.hpp"

typedef std::stack<VSObject *> cpt_stack_t;

class VSInterpretor {
public:
    VSInterpretor();
    ~VSInterpretor();

    void exec(
        cpt_stack_t &stack,
        vs_addr_t &pc,
        VSCodeObject *code, 
        VSTupleObject *locals, 
        VSTupleObject *freevars, 
        VSTupleObject *cellvars, 
        VSTupleObject *globals
    ) const;

    void eval(cpt_stack_t &stack, VSFrameObject *frame) const;
};

extern const VSInterpretor INTERPRETOR;

#endif