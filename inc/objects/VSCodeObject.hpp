#ifndef VS_CODE_OBJECT_H
#define VS_CODE_OBJECT_H

#include "opcode.hpp"
#include "VSObject.hpp"
#include "VSTypeObject.hpp"

#include <vector>

class VSInst
{
public:
    OPCODE opcode;
    vs_addr_t operand;

    VSInst(OPCODE opcode);
    VSInst(OPCODE opcode, vs_addr_t operand);
    ~VSInst() = default;

    VSInst &operator=(VSInst &inst);
};

class VSCodeObject : public VSObject
{
public:
    vs_size_t ninsts;
    vs_size_t nconsts;
    vs_size_t nargs;
    vs_size_t nlvars;
    vs_size_t nfreevars;

    VSObject *name;
    VSObject *consts;
    VSObject *lvars;
    VSObject *freevars;
    std::vector<VSInst> code;

    VSCodeObject(VSObject *name);
    ~VSCodeObject() = default;

    void add_inst(VSInst inst);
    void add_const(VSObject *object);
    void add_arg(VSObject *name);
    void add_lvar(VSObject *name);
    void add_freevar(VSObject *name);
};

extern VSTypeObject *VSCodeType;

#endif