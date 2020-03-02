#ifndef VS_CODE_OBJECT_H
#define VS_CODE_OBJECT_H

#include "code.hpp"
#include "VSObject.hpp"
#include "VSMapObject.hpp"
#include "VSListObject.hpp"

typedef enum
{
    NORM_BLK,
    FUNC_BLK,
    LOOP_BLK
} CODE_BLK_TYPE;

static char *CODE_BLK_STR[] = {"NORM", "FUNC", "LOOP"};

class VSInst : public VSObject
{
public:
    OPCODE opcode;
    // constant addr
    vs_addr_t operand;

    VSInst(OPCODE opcode);
    VSInst(OPCODE opcode, vs_addr_t operand);
};

class VSCodeObject : public VSObject
{
public:
    const std::string name;
    const CODE_BLK_TYPE type;

    vs_size_t inst_num;
    vs_size_t const_num;
    vs_size_t arg_num;
    vs_size_t lvar_num;
    vs_size_t nlvar_num;

    // For loop block, this indicates the start point of the loop body.
    vs_addr_t loop_start;

    VSObject *consts;
    VSObject *code;
    VSObject *local_names;
    VSObject *non_local_names;
    VSObject *name_to_addr;

    VSCodeObject(std::string name, CODE_BLK_TYPE type);

    void add_inst(VSInst inst);
    void add_const(VSObject *object);
    void add_arg(std::string name);
    void add_local(std::string name);
    void add_non_local(std::string name);
};

#endif