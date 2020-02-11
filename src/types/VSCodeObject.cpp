#include "vs.hpp"

VSCodeObject::VSCodeObject(std::string name, CODE_BLK_TYPE type) : name(name), type(type)
{
    this->arg_num = 0;
    this->inst_num = 0;
    this->lvar_num = 0;
    this->const_num = 0;
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