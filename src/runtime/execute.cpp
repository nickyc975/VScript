#include "runtime.hpp"

#define print_indent(indent)         \
    for (int i = 0; i < indent; i++) \
    {                                \
        printf(" ");                 \
    }

#define check_list_index(index)                                                    \
    if (index.type != OBJ_DATA)                                                    \
    {                                                                              \
        err("object type \"%s\" can not be index\n", OBJ_STR[index.type]);         \
        terminate(TERM_ERROR);                                                     \
    }                                                                              \
    if (index.value->type != INT)                                                  \
    {                                                                              \
        err("value type \"%s\" can not be index\n", TYPE_STR[index.value->type]);  \
        terminate(TERM_ERROR);                                                     \
    }                                                                              \
    if (index.value->int_val < 0 || index.value->int_val >= list.objlist->length()) \
    {                                                                              \
        err("invalid list index: %lld\n", index.value->int_val);                   \
        terminate(TERM_ERROR);                                                     \
    }

#define leave_blk()                     \
    VSCallStackFrame *temp = cur_frame; \
    cur_frame = temp->prev;             \
    delete temp;

static VSCallStackFrame *cur_frame;
static std::stack<VSObject> cmptstack;

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
static void do_eval();
static void do_to_char();
static void do_to_int();
static void do_to_float();
static void do_to_str();
static void do_build_list(vs_size_t size);
static void do_index_load();
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
    object.incref();
    cmptstack.push(object);
}

static VSObject pop()
{
    VSObject object = cmptstack.top();
    cmptstack.pop();
    return object;
}

static void find_blk_type(CODE_BLK_TYPE type)
{
    while (cur_frame != NULL && cur_frame->code->type != type)
    {
        leave_blk();
    }

    if (cur_frame == NULL)
    {
        warn("missing code block type: \"%s\"\n", CODE_BLK_STR[type]);
        terminate(TERM_WARN);
    }
}

static void print_obj(VSObject object, int indent)
{
    print_indent(indent);
    if (object.type == OBJ_DATA)
    {
        printf("%s", object.value->to_string().c_str());
    }
    else if (object.type == OBJ_CODE)
    {
        printf("%s %s", CODE_BLK_STR[object.codeblock->type], object.codeblock->name.c_str());
    }
    else
    {
        printf("[\n");
        for (auto o : object.objlist->data)
        {
            print_obj(o, indent + 1);
            printf(",\n");
        }
        print_indent(indent);
        printf("]\n");
    }
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
    else if (left.type == OBJ_LIST && right.type == OBJ_LIST)
    {
        result.type = OBJ_LIST;
        result.objlist = new VSObjectList();
        for (auto o : left.objlist->data)
        {
            result.objlist->push(o);
        }
        for (auto o : right.objlist->data)
        {
            result.objlist->push(o);
        }
    }
    else
    {
        err("Runtime error: can not apply \"+\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \"-\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \"*\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \"/\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \"%\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \"<\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \">\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \"<=\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \">=\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \"==\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \"!=\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \"&\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \"|\" on objects.\n");
        terminate(TERM_ERROR);
    }
    left.decref();
    right.decref();
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
        err("Runtime error: can not apply \"!\" on objects.\n");
        terminate(TERM_ERROR);
    }
    value.decref();
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
        err("Runtime error: can not apply \"-\" on objects.\n");
        terminate(TERM_ERROR);
    }
    value.decref();
    push(result);
}

static void do_eval()
{
    // TODO: implement eval
    push(VSObject(VSValue::None()));
}

static void do_to_char()
{
    VSObject value = pop();
    VSObject result = VSObject();
    if (value.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value =value.value->to_char();
    }
    else
    {
        err("Runtime error: can not cast object type \"%s\" to CHAR.\n", OBJ_STR[value.type]);
        terminate(TERM_ERROR);
    }
    value.decref();
    push(result);
}

static void do_to_int()
{
    VSObject value = pop();
    VSObject result = VSObject();
    if (value.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value =value.value->to_int();
    }
    else
    {
        err("Runtime error: can not cast object type \"%s\" to INT.\n", OBJ_STR[value.type]);
        terminate(TERM_ERROR);
    }
    value.decref();
    push(result);
}

static void do_to_float()
{
    VSObject value = pop();
    VSObject result = VSObject();
    if (value.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value =value.value->to_float();
    }
    else
    {
        err("Runtime error: can not cast object type \"%s\" to FLOAT.\n", OBJ_STR[value.type]);
        terminate(TERM_ERROR);
    }
    value.decref();
    push(result);
}

static void do_to_str()
{
    VSObject value = pop();
    VSObject result = VSObject();
    if (value.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value =value.value->to_str();
    }
    else
    {
        err("Runtime error: can not cast object type \"%s\" to STRING.\n", OBJ_STR[value.type]);
        terminate(TERM_ERROR);
    }
    value.decref();
    push(result);
}

static void do_build_list(vs_size_t size)
{
    VSObject object = VSObject(new VSObjectList());
    for (vs_size_t i = 0; i < size && !cmptstack.empty(); i++)
    {
        VSObject obj = pop();
        object.objlist->push(obj);
        obj.decref();
    }
    push(object);
}

static void do_index_load()
{
    VSObject list = pop();
    VSObject index = pop();
    if (list.type != OBJ_LIST)
    {
        err("object type \"%s\" can not be indexed\n", OBJ_STR[list.type]);
        terminate(TERM_ERROR);
    }

    check_list_index(index);
    push(list.objlist->get(index.value->int_val));
    index.decref();
    list.decref();
}

static void do_index_store()
{
    VSObject list = pop();
    VSObject index = pop();
    VSObject value = pop();
    if (list.type != OBJ_LIST)
    {
        err("object type \"%s\" can not be indexed\n", OBJ_STR[list.type]);
        terminate(TERM_ERROR);
    }

    check_list_index(index);

    list.objlist->put(index.value->int_val, value);
    value.decref();
    index.decref();
    list.decref();
}

static void do_append()
{
    VSObject list = pop();
    VSObject value = pop();
    if (list.type != OBJ_LIST)
    {
        err("object type \"%s\" can not be indexed\n", OBJ_STR[list.type]);
        terminate(TERM_ERROR);
    }

    list.objlist->push(value);
    value.decref();
    list.decref();
}

static void do_load_local(vs_addr_t addr)
{
    if (addr >= cur_frame->lvar_num)
    {
        err("invalid local var addr: %u, number of names: %u\n", addr, cur_frame->lvar_num);
        terminate(TERM_ERROR);
    }
    VSObject object = cur_frame->locals.get(addr);
    push(object);
}

static void do_store_local(vs_addr_t addr)
{
    VSObject object = pop();
    if (addr >= cur_frame->lvar_num)
    {
        err("invalid local var addr: %u, number of names: %u\n", addr, cur_frame->lvar_num);
        terminate(TERM_ERROR);
    }

    cur_frame->locals.put(addr, object);
}

static void do_load_name(vs_addr_t addr)
{
    if (addr > cur_frame->nlvar_num)
    {
        err("invalid non-local name addr: %u, number of names: %u\n", addr, cur_frame->nlvar_num);
        terminate(TERM_ERROR);
    }

    VSCallStackFrame *temp = cur_frame->prev;
    std::string name = cur_frame->code->non_local_names[addr];
    while (temp != NULL)
    {
        auto iter = temp->code->name_to_addr.find(name);
        if (iter != temp->code->name_to_addr.end())
        {
            push(temp->locals.get(iter->second));
            return;
        }
        temp = temp->prev;
    }
    err("name \"%s\" undefined\n", name.c_str());
    terminate(TERM_ERROR);
}

static void do_store_name(vs_addr_t addr)
{
    if (addr > cur_frame->nlvar_num)
    {
        err("invalid non-local name addr: %u, number of names: %u\n", addr, cur_frame->nlvar_num);
        terminate(TERM_ERROR);
    }

    VSCallStackFrame *temp = cur_frame->prev;
    std::string name = cur_frame->code->non_local_names[addr];
    while (temp != NULL)
    {
        auto iter = temp->code->name_to_addr.find(name);
        if (iter != temp->code->name_to_addr.end())
        {
            VSObject obj = pop();
            temp->locals.put(iter->second, obj);
            obj.decref();
            return;
        }
        temp = temp->prev;
    }
    err("name \"%s\" undefined\n", name.c_str());
    terminate(TERM_ERROR);
}

static void do_load_const(vs_addr_t addr)
{
    if (addr >= cur_frame->const_num)
    {
        err("invalid const addr: %u, number of consts: %u\n", addr, cur_frame->const_num);
        terminate(TERM_ERROR);
    }
    VSObject object = cur_frame->code->consts.get(addr);
    push(object);
}

static void do_goto()
{
    VSObject object = pop();
    if (object.type != OBJ_CODE)
    {
        err("can not goto object type \"%s\"\n", OBJ_STR[object.type]);
        terminate(TERM_ERROR);
    }
    cur_frame = new VSCallStackFrame(cur_frame, object.codeblock);
    object.decref();
}

static void do_jmp(vs_addr_t addr)
{
    if (addr >= cur_frame->inst_num)
    {
        err("invalid jmp target: %u, number of insts: %u\n", addr, cur_frame->inst_num);
        terminate(TERM_ERROR);
    }
    cur_frame->pc = addr;
}

static void do_jif(vs_addr_t addr)
{
    VSObject cond = pop();
    if (cond.type != OBJ_DATA)
    {
        err("object type \"%s\" can not be condition\n", OBJ_STR[cond.type]);
        terminate(TERM_ERROR);
    }
    if (cond.value->type != BOOL)
    {
        err("value type \"%s\" can not be condition\n", TYPE_STR[cond.value->type]);
        terminate(TERM_ERROR);
    }
    if (addr >= cur_frame->inst_num)
    {
        err("invalid jif target: %u, number of insts: %u\n", addr, cur_frame->inst_num);
        terminate(TERM_ERROR);
    }
    if (cond.value->bool_val)
    {
        cur_frame->pc = addr;
    }
    cond.decref();
}

static void do_break()
{
    find_blk_type(LOOP_BLK);
    leave_blk();

    if (cur_frame == NULL)
    {
        warn("directly ending with break\n");
        terminate(TERM_WARN);
    }
}

static void do_continue()
{
    find_blk_type(LOOP_BLK);
    cur_frame->pc = cur_frame->code->loop_start;
}

static void do_call()
{
    VSObject func = pop();
    VSObject args = pop();
    if (func.type != OBJ_CODE)
    {
        err("can not call object type: \"%s\"\n", OBJ_STR[func.type]);
        terminate(TERM_ERROR);
    }

    if (func.codeblock->type != FUNC_BLK)
    {
        err("can not call code block type: \"%s\"\n", CODE_BLK_STR[func.codeblock->type]);
        terminate(TERM_ERROR);
    }

    if (args.type != OBJ_LIST)
    {
        err("object type \"%s\" can not be function arg list\n", OBJ_STR[args.type]);
        terminate(TERM_ERROR);
    }

    if (args.objlist->length() != func.codeblock->arg_num)
    {
        err("function \"%s\" expects %u args but got only %u args\n",
            func.codeblock->name.c_str(), func.codeblock->arg_num, args.objlist->length());
        terminate(TERM_ERROR);
    }

    cur_frame = new VSCallStackFrame(cur_frame, func.codeblock);
    for (vs_size_t i = 0; i < args.objlist->length(); i++)
    {
        VSObject obj = args.objlist->get(i);
        cur_frame->locals.put(i, obj);
    }
    args.decref();
    func.decref();
}

static void do_ret()
{
    find_blk_type(FUNC_BLK);
    leave_blk();

    if (cur_frame == NULL)
    {
        warn("directly ending with return\n");
        terminate(TERM_WARN);
    }
}

static void do_input()
{
    std::string str = std::string();
    char c = getchar();
    while (c != '\n' && c != '\r')
    {
        str.append(1, c);
        c = getchar();
    }
    if (c == '\r')
    {
        c = getchar();
        if (c != '\n')
        {
            ungetc(c, stdin);
        }
    }
    VSValue *value = new VSValue(str);
    push(VSObject(value));
}

static void do_print()
{
    VSObject object = pop();
    print_obj(object, 0);
    object.decref();
}

static void eval()
{
    while (cur_frame->pc < cur_frame->inst_num)
    {
        vs_addr_t addr = cur_frame->pc++;
        VSInst inst = cur_frame->code->code[addr];
        // note("%s\n", OPCODE_STR[inst.opcode]);
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
        case OP_EVAL:
            do_eval();
            break;
        case OP_CHAR:
            do_to_char();
            break;
        case OP_INT:    
            do_to_int();
            break;
        case OP_FLOAT:
            do_to_float();
            break;
        case OP_STR:
            do_to_str();
            break;
        case OP_BUILD_LIST:
            do_build_list(inst.operand);
            break;
        case OP_INDEX_LOAD:
            do_index_load();
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
    }
}

static void terminate(TERM_STATUS status)
{
    exit(status);
}

int execute(VSCodeObject *code)
{
    cmptstack = std::stack<VSObject>();
    cur_frame = new VSCallStackFrame(NULL, code);

    while (cur_frame != NULL)
    {
        eval();
        leave_blk();
    }
    return 0;
}