#ifndef VS_EXECUTOR_H
#define VS_EXECUTOR_H

#include "objects/VSObject.hpp"
#include "objects/VSTypeObject.hpp"
#include "objects/VSFrameObject.hpp"
#include "objects/VSFunctionObject.hpp"

#include <stack>

extern VSTypeObject *VSExecutorType;

class VSExecutor : public VSObject {
public:
    std::stack<VSFrameObject *> call_stack;

    VSObject *eval(VSFrameObject *frame);
    VSObject *call_func(VSFunctionObject *func, VSObject *args);
    VSObject *call_meth(VSFunctionObject *meth, VSObject *self, VSObject *args);
};

#endif