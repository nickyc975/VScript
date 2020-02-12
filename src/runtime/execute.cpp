#include "runtime.hpp"

static std::stack<VSObject> cmptstack;
static std::stack<VSCallStackFrame *> callstack;

static void push(VSObject object);
static VSObject pop();

static void do_add();
static void do_sub();
static void do_mul();
static void do_div();
static void do_mod();
static void do_lt();
static void do_gt();
static void do_le();
static void do_ge();
static void do_eq();
static void do_neq();
static void do_and();
static void do_or();
static void do_not();
static void do_neg();
static void do_build_list();
static void do_index_load(vs_size_t size);
static void do_index_store();
static void do_append();
static void do_load_local(vs_addr_t addr);
static void do_store_local(vs_addr_t addr);
static void do_load_name(vs_addr_t addr);
static void do_store_name(vs_addr_t addr);
static void do_load_const(vs_addr_t addr);
static void do_goto();
static void do_jmp(vs_addr_t addr);
static void do_jif(vs_addr_t addr);
static void do_break();
static void do_continue();
static void do_call();
static void do_ret();
static void do_input();
static void do_print();

static void eval();
static void terminate(TERM_STATUS status);

static void push(VSObject object)
{
    cmptstack.push(object);
}

static VSObject pop()
{
    VSObject object = cmptstack.top();
    cmptstack.pop();
    return object;
}

static void do_add()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_add(left.value, right.value);
    }
    else if (left.type == OBJ_LIST && left.type == OBJ_LIST)
    {
        result.type = OBJ_LIST;
        result.obj_list = new std::vector<VSObject>();
        result.obj_list->assign(left.obj_list->begin(), left.obj_list->end());
        result.obj_list->assign(right.obj_list->begin(), right.obj_list->end());
    }
    else
    {
        err("Runtime error: can not apply \"+\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_sub()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_sub(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \"-\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_mul()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_mul(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \"*\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_div()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_div(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \"/\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_mod()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_mod(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \"%\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_lt()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_lt(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \"<\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_gt()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_gt(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \">\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_le()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_le(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \"<=\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_ge()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_ge(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \">=\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_eq()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_eq(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \"==\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_neq()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_neq(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \"!=\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_and()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_and(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \"&\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_or()
{
    VSObject left = pop();
    VSObject right = pop();
    VSObject result = VSObject();
    if (left.type == OBJ_DATA && right.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_or(left.value, right.value);
    }
    else
    {
        err("Runtime error: can not apply \"|\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_not()
{
    VSObject value = pop();
    VSObject result = VSObject();
    if (value.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_not(value.value);
    }
    else
    {
        err("Runtime error: can not apply \"!\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_neg()
{
    VSObject value = pop();
    VSObject result = VSObject();
    if (value.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = VSValue::i_neg(value.value);
    }
    else
    {
        err("Runtime error: can not apply \"-\" on objects.");
        terminate(TERM_ERROR);
    }
    push(result);
}

static void do_build_list(){}

static void do_index_load(vs_size_t size){}

static void do_index_store(){}

static void do_append(){}

static void do_load_local(vs_addr_t addr)
{
    VSCallStackFrame *cur_frame = callstack.top();
    if (addr >= cur_frame->lvar_num)
    {
        err("Internal error!");
        terminate(TERM_ERROR);
    }
    VSObject object = cur_frame->locals[addr];
    push(object);
}

static void do_store_local(vs_addr_t addr)
{
    VSObject object = pop();
    VSCallStackFrame *cur_frame = callstack.top();
    if (addr >= cur_frame->lvar_num)
    {
        err("Internal error!");
        terminate(TERM_ERROR);
    }
    cur_frame->locals[addr] = object;
}

static void do_load_name(vs_addr_t addr){}

static void do_store_name(vs_addr_t addr){}

static void do_load_const(vs_addr_t addr)
{
    VSCallStackFrame *cur_frame = callstack.top();
    if (addr >= cur_frame->lvar_num)
    {
        err("Internal error!");
        terminate(TERM_ERROR);
    }
    VSObject object = cur_frame->code->consts[addr];
    push(object);
}

static void do_goto(){}

static void do_jmp(vs_addr_t addr){}

static void do_jif(vs_addr_t addr){}

static void do_break(){}

static void do_continue(){}

static void do_call(){}

static void do_ret(){}

static void do_input()
{

}

static void do_print()
{
    VSObject object = pop();
    if (object.type == OBJ_DATA)
    {
        std::cout << object.value->to_string();
    }
}

static void eval()
{
    VSCallStackFrame *cur_frame = callstack.top();
    while (cur_frame->pc < cur_frame->inst_num)
    {
        vs_addr_t addr = cur_frame->pc;
        VSInst inst = cur_frame->code->code[addr];
        switch (inst.opcode)
        {
        case OP_ADD:
            do_add();
            break;
        case OP_SUB:
            do_sub();
            break;
        case OP_MUL:
            do_mul();
            break;
        case OP_DIV:
            do_div();
            break;
        case OP_MOD:
            do_mod();
            break;
        case OP_LT:
            do_lt();
            break;
        case OP_GT:
            do_gt();
            break;
        case OP_LE:
            do_le();
            break;
        case OP_GE:
            do_ge();
            break;
        case OP_EQ:
            do_eq();
            break;
        case OP_NEQ:
            do_neq();
            break;
        case OP_AND:
            do_and();
            break;
        case OP_OR:
            do_or();
            break;
        case OP_NOT:
            do_not();
            break;
        case OP_NEG:
            do_neg();
            break;
        case OP_BUILD_LIST:
            do_build_list();
            break;
        case OP_INDEX_LOAD:
            do_index_load(inst.operand);
            break;
        case OP_INDEX_STORE:
            do_index_store();
            break;
        case OP_APPEND:
            do_append();
            break;
        case OP_LOAD_LOCAL:
            do_load_local(inst.operand);
            break;
        case OP_LOAD_NAME:
            do_load_name(inst.operand);
            break;
        case OP_STORE_LOCAL:
            do_store_local(inst.operand);
            break;
        case OP_STORE_NAME:
            do_store_name(inst.operand);
            break;
        case OP_LOAD_CONST:
            do_load_const(inst.operand);
            break;
        case OP_GOTO:
            do_goto();
            break;
        case OP_JMP:
            do_jmp(inst.operand);
            break;
        case OP_JIF:
            do_jif(inst.operand);
            break;
        case OP_BREAK:
            do_break();
            break;
        case OP_CONTINUE:
            do_continue();
            break;
        case OP_CALL:
            do_call();
            break;
        case OP_RET:
            do_ret();
            break;
        case OP_INPUT:
            do_input();
            break;
        case OP_PRINT:
            do_print();
            break;
        case OP_NOP:
        default:
            break;
        }
        cur_frame->pc++;
    }
}

static void terminate(TERM_STATUS status)
{
    exit(status);
}

int execute(VSCodeObject *code)
{
    cmptstack = std::stack<VSObject>();
    callstack = std::stack<VSCallStackFrame *>();
    callstack.push(new VSCallStackFrame(NULL, code));
    eval();
}