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
    this->inst_num = 0;
    this->const_num = 0;
    this->arg_num = 0;
    this->lvar_num = 0;
    this->nlvar_num = 0;

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

void VSCodeObject::add_arg(std::string name)
{
    this->add_local(name);
    this->arg_num++;
}

void VSCodeObject::add_local(std::string name)
{
    this->local_names.push_back(name);
    this->lvar_num++;
}

void VSCodeObject::add_non_local(std::string name)
{
    this->non_local_names.push_back(name);
    this->nlvar_num++;
}