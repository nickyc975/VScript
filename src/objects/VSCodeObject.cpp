#include "objects/VSCodeObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

VSObject *vs_code_hash(const VSObject *codeobj) {
    VS_ENSURE_TYPE(codeobj, T_CODE, "code.__hash__()");
    INCREF_RET(C_INT_TO_INT((cint_t)codeobj));
}

VSObject *vs_code_eq(const VSObject *a, const VSObject *b) {
    VS_ENSURE_TYPE(a, T_CODE, "code.__eq__()");
    VS_ENSURE_TYPE(b, T_CODE, "code.__eq__()");

    INCREF_RET(C_BOOL_TO_BOOL(a == b));
}

VSObject *vs_code_str(VSObject *codeobj) {
    VS_ENSURE_TYPE(codeobj, T_CODE, "code.__str__()");

    INCREF_RET(C_STRING_TO_STRING("code"));
}

VSObject *vs_code_bytes(VSObject *codeobj) {
    VS_ENSURE_TYPE(codeobj, T_CODE, "code.__bytes__()");

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

VSCodeObject::VSCodeObject(VSObject *name) {
    this->type = T_CODE;
    this->name = name;
    INCREF(name);

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

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_code_hash, 1, true);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_code_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_code_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_code_bytes, 1, false);
}

VSCodeObject::~VSCodeObject() {
    DECREF_EX(this->name);
    DECREF_EX(this->consts);
    DECREF_EX(this->lvars);
    DECREF_EX(this->names);
    DECREF_EX(this->cellvars);
    DECREF_EX(this->freevars);
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