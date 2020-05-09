#ifndef VS_COROUTINE_H
#define VS_COROUTINE_H

#include <stack>

#include "VSObject.hpp"
#include "VSFrameObject.hpp"

class VSCoroutineObject : public VSObject {
private:
    static const str_func_map vs_coroutine_methods;

public:
    VSFrameObject *frame;
    VSObject *yield_value;
    std::stack<VSObject *> stack;

    VSCoroutineObject(VSFrameObject *frame);
    ~VSCoroutineObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
};

#endif