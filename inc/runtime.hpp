#ifndef VS_RUNTIME_H
#define VS_RUNTIME_H

#include <stack>
#include <iostream>

#include "vs.hpp"
#include "error.hpp"

typedef enum
{
    TERM_NORM,
    TERM_WARN,
    TERM_ERROR
} TERM_STATUS;

class VSCallStackFrame
{
public:
    vs_size_t pc;
    vs_size_t inst_num;
    vs_size_t lvar_num;

    VSCodeObject *code;
    VSCallStackFrame *prev;
    std::vector<VSObject> locals;
    std::unordered_map<std::string, vs_addr_t> localnames;

    VSCallStackFrame(VSCallStackFrame *prev, VSCodeObject *code);
};

int execute(VSCodeObject *code);

#endif