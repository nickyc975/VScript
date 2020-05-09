#ifndef VS_FRAME_H
#define VS_FRAME_H

#include "VSObject.hpp"
#include "VSCellObject.hpp"
#include "VSCodeObject.hpp"
#include "VSTupleObject.hpp"
#include "VSFunctionObject.hpp"

#include <stack>

class VSFrameObject : public VSObject {
private:
    static const str_func_map vs_frame_methods;

    vs_addr_t pc;

    VSCodeObject *code;
    vs_size_t nlocals;
    VSTupleObject *locals;
    vs_size_t ncellvars;
    VSTupleObject *cellvars;
    vs_size_t nfreevars;
    VSTupleObject *freevars;
    vs_size_t nbuiltins;
    VSTupleObject *builtins;

    VSFrameObject *prev;

public:
    VSFrameObject(
        VSCodeObject *code, 
        VSTupleObject *args,
        VSTupleObject *cellvars, 
        VSTupleObject *freevars, 
        VSTupleObject *builtins, 
        VSFrameObject *prev);
    ~VSFrameObject();

    void eval(std::stack<VSObject *> &stack);
    bool done();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
};

#endif