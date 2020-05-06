#ifndef VS_CODE_OBJECT_H
#define VS_CODE_OBJECT_H

#include <vector>

#include "VSObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSStringObject.hpp"
#include "opcode.hpp"

class VSInst {
public:
    OPCODE opcode;
    vs_addr_t operand;

    VSInst(OPCODE opcode);
    VSInst(OPCODE opcode, vs_addr_t operand);
    ~VSInst() = default;

    VSInst &operator=(VSInst &inst);
};

#define VS_FUNC_VARARGS 0x1

class VSCodeObject : public VSObject {
private:
    static const str_func_map vs_code_methods;

public:
    int flags;
    vs_size_t ninsts;
    vs_size_t nconsts;
    vs_size_t nargs;
    vs_size_t nlvars;
    vs_size_t nnames;
    vs_size_t ncellvars;
    vs_size_t nfreevars;

    VSStringObject *name;
    VSListObject *consts;
    VSListObject *lvars;
    VSListObject *names;
    VSListObject *cellvars;
    VSListObject *freevars;
    std::vector<VSInst> code;

    VSCodeObject(VSStringObject *name);
    ~VSCodeObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;

    void add_inst(VSInst inst);
    void add_const(VSObject *object);
    void add_arg(VSObject *name);
    void add_lvar(VSObject *name);
    void add_name(VSObject *name);
    void add_cellvar(VSObject *name);
    void add_freevar(VSObject *name);
};

#define AS_CODE(obj) ((VSCodeObject *)(obj))

#endif