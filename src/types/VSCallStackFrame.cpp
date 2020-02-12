#include "runtime.hpp"

VSCallStackFrame::VSCallStackFrame(VSCallStackFrame *prev, VSCodeObject *code)
{
    this->pc = 0;
    this->inst_num = code->inst_num;
    this->lvar_num = code->lvar_num;
    this->code = code;
    this->prev = prev;

    this->locals = std::vector<VSObject>();
    this->localnames = std::unordered_map<std::string, vs_addr_t>();

    for (int i = 0; i < this->lvar_num; i++)
    {
        this->locals.push_back(VSObject());
        this->localnames[code->local_names[i]] = i;
    }
}