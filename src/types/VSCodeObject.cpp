#include "vs.hpp"

VSInst::VSInst(OPCODE opcode): opcode(opcode), operand(0)
{
}

VSInst::VSInst(OPCODE opcode, vs_addr_t operand): opcode(opcode), operand(operand)
{
}

const char *VSInst::to_bytes()
{
    return this->to_string().c_str();
}

const std::string VSInst::to_string()
{
    return std::string(OPCODE_STR[this->opcode]) + "\t" + std::to_string(this->operand);
}

VSCodeObject::VSCodeObject(std::string name, CODE_BLK_TYPE type) : name(name), type(type)
{
    this->arg_num = 0;
    this->inst_num = 0;
    this->lvar_num = 0;
    this->const_num = 0;

    // set constants
    this->add_const(new VSObject(VSValue::None()));
    this->add_const(new VSObject(VSValue::True()));
    this->add_const(new VSObject(VSValue::False()));
}

void VSCodeObject::add_inst(VSInst inst)
{
    this->code.push_back(inst);
    this->inst_num++;
}

void VSCodeObject::add_const(VSObject *object)
{
    this->consts.push_back(*object);
    this->const_num++;
}

void VSCodeObject::add_varname(std::string varname)
{
    this->varnames.push_back(varname);
    this->lvar_num++;
}

void VSCodeObject::add_argname(std::string argname)
{
    this->add_varname(name);
    this->arg_num++;
}