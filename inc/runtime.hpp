#ifndef VS_RUNTIME_H
#define VS_RUNTIME_H

#include <stack>

#include "vs.hpp"
#include "error.hpp"

class VSCallStackFrame
{
public:
    vs_addr_t pc;
    vs_size_t inst_num;
    vs_size_t const_num;
    vs_size_t arg_num;
    vs_size_t lvar_num;
    vs_size_t nlvar_num;

    VSCodeObject *code;
    VSCallStackFrame *prev;

    VSObjectList locals;

    VSCallStackFrame(VSCallStackFrame *prev, VSCodeObject *code);
    ~VSCallStackFrame();
};

int execute(VSCodeObject *code, VSObjectList *objects, std::vector<vs_native_func> *func_addrs);

#endif