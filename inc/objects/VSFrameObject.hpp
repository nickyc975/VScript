#ifndef VS_FRAME_H
#define VS_FRAME_H

#include "VSObject.hpp"
#include "VSCodeObject.hpp"
#include "VSTupleObject.hpp"

#include <stack>

class VSFrameObject : public VSObject {
private:
    static const str_func_map vs_frame_methods;

public:
    vs_addr_t pc;

    VSCodeObject *code;
    vs_size_t nlocals;
    VSTupleObject *locals;
    vs_size_t ncellvars;
    VSTupleObject *cellvars;
    vs_size_t nfreevars;
    VSTupleObject *freevars;

    VSFrameObject *prev;

    VSFrameObject(
        VSCodeObject *code, 
        VSTupleObject *args,
        VSTupleObject *cellvars, 
        VSTupleObject *freevars, 
        VSFrameObject *prev);
    ~VSFrameObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
};

#endif