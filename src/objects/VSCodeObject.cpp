#include "objects/VSCodeObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSTupleObject.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);

VSObject *vs_code_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("code.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_CODE, "code.__str__()");

    INCREF_RET(C_STRING_TO_STRING("code"));
}

VSObject *vs_code_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("code.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_CODE, "code.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSInst::VSInst(OPCODE opcode) : opcode(opcode), operand(0) {
}

VSInst::VSInst(OPCODE opcode, vs_addr_t operand) : opcode(opcode), operand(operand) {
}

VSInst &VSInst::operator=(VSInst &inst) {
    this->opcode = inst.opcode;
    this->operand = inst.operand;
    return *this;
}

const str_func_map VSCodeObject::vs_code_methods = {
    {ID___hash__, vs_default_hash},
    {ID___eq__, vs_default_eq},
    {ID___str__, vs_code_str},
    {ID___bytes__, vs_code_bytes}};

VSCodeObject::VSCodeObject(VSStringObject *name) {
    this->type = T_CODE;
    this->name = name;
    INCREF(name);

    this->flags = 0;
    this->ninsts = 0;
    this->nconsts = 0;
    this->nargs = 0;
    this->nlvars = 0;
    this->nnames = 0;
    this->ncellvars = 0;
    this->nfreevars = 0;

    this->consts = vs_list_pack(0);
    this->lvars = vs_list_pack(0);
    this->names = vs_list_pack(0);
    this->cellvars = vs_list_pack(0);
    this->freevars = vs_list_pack(0);
    this->code = std::vector<VSInst>();

    // set constants
    this->add_const(VS_NONE);
}

VSCodeObject::~VSCodeObject() {
    DECREF_EX(this->name);
    DECREF_EX(this->consts);
    DECREF_EX(this->lvars);
    DECREF_EX(this->names);
    DECREF_EX(this->cellvars);
    DECREF_EX(this->freevars);
}

bool VSCodeObject::hasattr(std::string &attrname) {
    return vs_code_methods.find(attrname) != vs_code_methods.end();
}

VSObject *VSCodeObject::getattr(std::string &attrname) {
    auto iter = vs_code_methods.find(attrname);
    if (iter == vs_code_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_code_methods.at(attrname));
    INCREF_RET(attr);
}

void VSCodeObject::setattr(std::string &, VSObject *) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}

void VSCodeObject::add_inst(VSInst inst) {
    this->code.push_back(inst);
    this->ninsts++;
}

void VSCodeObject::add_const(VSObject *object) {
    LIST_APPEND(this->consts, object);
    this->nconsts++;
}

void VSCodeObject::add_arg(VSObject *name) {
    this->add_lvar(name);
    this->nargs++;
}

void VSCodeObject::add_lvar(VSObject *name) {
    LIST_APPEND(this->lvars, name);
    this->nlvars++;
}

void VSCodeObject::add_name(VSObject *name) {
    LIST_APPEND(this->names, name);
    this->nnames++;
}

void VSCodeObject::add_cellvar(VSObject *name) {
    LIST_APPEND(this->cellvars, name);
    this->ncellvars++;
}

void VSCodeObject::add_freevar(VSObject *name) {
    LIST_APPEND(this->freevars, name);
    this->nfreevars++;
}