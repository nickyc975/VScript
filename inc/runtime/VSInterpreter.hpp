#ifndef VS_INTERPRETER_H
#define VS_INTERPRETER_H

#include "objects/VSObject.hpp"
#include "objects/VSTypeObject.hpp"
#include "objects/VSCodeObject.hpp"
#include "objects/VSFrameObject.hpp"
#include "objects/VSFunctionObject.hpp"

#include <stack>

typedef std::stack<VSObject *> cpt_stack_t;

class VSInterpreter {
private:
    static const vs_addr_t MAX_CALL_DEPTH = 512;

    VSObject *do_add(VSObject *l_val, VSObject *r_val);

    void eval(cpt_stack_t &stack);
    VSObject *call_function(VSFunctionObject *func, VSObject *args);
    VSObject *call_method(VSFunctionObject *meth, VSObject *self, VSObject *args);

public:
    vs_addr_t call_depth;
    VSTupleObject *builtins;
    VSFrameObject *current_frame;

    VSInterpreter(VSTupleObject *builtins);
    ~VSInterpreter();

    void interpret(VSCodeObject *code);
};

#endif