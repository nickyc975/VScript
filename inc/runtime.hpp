#ifndef VS_RUNTIME_H
#define VS_RUNTIME_H

#include "vs.hpp"
#include <stack>

class VSCallStackFrame
{
public:
    VSCallStackFrame *prev;
    std::vector<VSObject> args;
    std::unordered_map<std::string, VSObject> vars;
};

#endif