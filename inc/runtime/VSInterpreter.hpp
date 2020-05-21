#ifndef VS_INTERPRETER_H
#define VS_INTERPRETER_H

#include <stack>

#include "objects/VSCodeObject.hpp"
#include "objects/VSFrameObject.hpp"
#include "objects/VSTupleObject.hpp"

typedef std::stack<VSObject *> cpt_stack_t;

class VSInterpreter {
public:
    VSInterpreter();
    ~VSInterpreter();

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

extern const VSInterpreter INTERPRETER;

#endif