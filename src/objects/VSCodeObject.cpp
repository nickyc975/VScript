#include "error.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSTupleObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSCodeObject.hpp"

VSInst::VSInst(OPCODE opcode) : opcode(opcode), operand(0)
{
}

VSInst::VSInst(OPCODE opcode, vs_addr_t operand) : opcode(opcode), operand(operand)
{
}

VSInst &VSInst::operator=(VSInst &inst)
{
    this->opcode = inst.opcode;
    this->operand = inst.operand;
    return *this;
}

VSCodeObject::VSCodeObject(VSObject *name)
{
    this->type = VSCodeType;
    this->name = name;
    INCREF(name);

    this->ninsts = 0;
    this->nconsts = 0;
    this->nargs = 0;
    this->nlvars = 0;
    this->ncellvars = 0;
    this->nfreevars = 0;

    this->consts = vs_list_pack(0);
    this->lvars = vs_list_pack(0);
    this->cellvars = vs_list_pack(0);
    this->freevars = vs_list_pack(0);
    this->code = std::vector<VSInst>();

    // set constants
    this->add_const(VS_NONE);
}

void VSCodeObject::add_inst(VSInst inst)
{
    this->code.push_back(inst);
    this->ninsts++;
}

void VSCodeObject::add_const(VSObject *object)
{
    LIST_APPEND(this->consts, object);
    this->nconsts++;
}

void VSCodeObject::add_arg(VSObject *name)
{
    this->add_lvar(name);
    this->nargs++;
}

void VSCodeObject::add_lvar(VSObject *name)
{
    LIST_APPEND(this->lvars, name);
    this->nlvars++;
}

void VSCodeObject::add_cellvar(VSObject *name) {
    LIST_APPEND(this->cellvars, name);
    this->ncellvars++;
}

void VSCodeObject::add_freevar(VSObject *name)
{
    LIST_APPEND(this->freevars, name);
    this->nfreevars++;
}

VSObject *vs_code_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    VSTypeObject *ttype = VS_TYPEOF(typeobj);
    VS_ENSURE_TYPE(ttype, T_TYPE, "code new");

    VSTypeObject *type = VS_AS_TYPE(typeobj);
    VS_ENSURE_TYPE(type, T_CODE, "code new");

    VSObject *name;
    if (!vs_tuple_unpack(args, 1, &name))
    {
        err("code.__new__() expected 1 arg but got %lld\n", TUPLE_LEN(args));
        return NULL;
    }

    VSTypeObject *nametype = VS_TYPEOF(name);
    VS_ENSURE_TYPE(nametype, T_STR, "code.__new__()");

    INCREF_RET(VS_AS_OBJECT(new VSCodeObject(name)));
}

void vs_code_init(VSObject *, VSObject *, VSObject *)
{
}

void vs_code_clear(VSObject *codeobj)
{
    VSTypeObject *type = VS_TYPEOF(codeobj);
    VS_ENSURE_TYPE(type, T_CODE, "code.__init__()");

    VSCodeObject *code = (VSCodeObject *)codeobj;
    DECREF(code->consts);
    DECREF(code->lvars);
    DECREF(code->freevars);
}

VSObject *vs_code_str(VSObject *codeobj)
{
    VSTypeObject *type = VS_TYPEOF(codeobj);
    VS_ENSURE_TYPE(type, T_CODE, "code.__str__()");

    return vs_string_from_cstring("code");
}

VSObject *vs_code_bytes(VSObject *codeobj)
{
    VSTypeObject *type = VS_TYPEOF(codeobj);
    VS_ENSURE_TYPE(type, T_CODE, "code.__bytes__()");

    return NULL;
}

VSTypeObject *VSCodeType = new VSTypeObject(
    VSTypeType,
    T_CODE,
    "code",                  // __name__
    NULL,                    // __attrs__
    vs_code_new,             // __new__
    vs_code_init,            // __init__
    NULL,                    // __copy__
    vs_code_clear,           // __clear__
    NULL,                    // __getattr__
    NULL,                    // __hasattr__
    NULL,                    // __setattr__
    NULL,                    // __removeattr__
    vs_hash_not_implemented, // __hash__
    NULL,                    // __lt__
    vs_default_eq,           // __eq__
    vs_code_str,             // __str__
    vs_code_bytes,           // __bytes__
    NULL,                    // __call__
    NULL,                    // _number_funcs
    NULL                     // _container_funcs
);