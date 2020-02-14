#include "runtime.hpp"

VSCallStackFrame::VSCallStackFrame(VSCallStackFrame *prev, VSCodeObject *code)
{
    this->pc = 0;
    this->inst_num = code->inst_num;
    this->const_num = code->const_num;
    this->arg_num = code->arg_num;
    this->lvar_num = code->lvar_num;
    this->nlvar_num = code->nlvar_num;
    this->code = code;
    this->prev = prev;

    this->locals = VSObjectList();

    for (vs_size_t i = 0; i < this->lvar_num; i++)
    {
        this->locals.push(VSObject());
    }
}

VSCallStackFrame::~VSCallStackFrame()
{
    
}