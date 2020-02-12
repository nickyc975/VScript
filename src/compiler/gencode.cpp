#include <stack>

#include "compiler.hpp"

#define enter_blk(name, type)                                             \
    codestack.push(new VSCodeObject(name, type));                         \
    conststack.push(new std::unordered_map<std::string, vs_addr_t>());    \
    (*conststack.top())["__vs_none__"] = 0;                               \
    (*conststack.top())["__vs_bool_true__"] = 1;                          \
    (*conststack.top())["__vs_bool_false__"] = 2;                         \
    localstack.push(new std::unordered_map<std::string, vs_addr_t>());    \
    nonlocalstack.push(new std::unordered_map<std::string, vs_addr_t>()); \

#define leave_blk()            \
    codestack.pop();           \
    delete conststack.top();   \
    conststack.pop();          \
    delete localstack.top();    \
    localstack.pop();           \
    delete nonlocalstack.top(); \
    nonlocalstack.pop();

static std::stack<VSCodeObject *> codestack;
static std::stack<std::unordered_map<std::string, vs_addr_t> *> conststack;
static std::stack<std::unordered_map<std::string, vs_addr_t> *> localstack;
static std::stack<std::unordered_map<std::string, vs_addr_t> *> nonlocalstack;

static void gen_const(ASTNode *node);
static void gen_ident(ASTNode *node);
static void gen_b_expr(ASTNode *node);
static void gen_u_expr(ASTNode *node);
static void gen_list_idx(ASTNode *node);
static void gen_list_val(ASTNode *node);
static void gen_func_call(ASTNode *node);
static void gen_return(ASTNode *node);
static void gen_break(ASTNode *node);
static void gen_continue(ASTNode *node);
static void gen_expr(ASTNode *node);
static void gen_expr_list(ASTNode *node);
static void gen_decl_stmt(ASTNode *node);
static void gen_assign_stmt(ASTNode *node);
static void gen_input_stmt(ASTNode *node);
static void gen_print_stmt(ASTNode *node);
static void gen_cpd_stmt(ASTNode *node);
static void gen_for_stmt(ASTNode *node);
static void gen_func_decl(ASTNode *node);
static void gen_elif_list(ASTNode *node);
static void gen_if_stmt(ASTNode *node);
static void gen_while_stmt(ASTNode *node);

static OPCODE get_b_op(TOKEN_TYPE tk)
{
    switch (tk)
    {
    case TK_ADD:
        return OP_ADD;
    case TK_SUB:
        return OP_SUB;
    case TK_MUL:
        return OP_MUL;
    case TK_DIV:
        return OP_DIV;
    case TK_MOD:
        return OP_MOD;
    case TK_LT:
        return OP_LT;
    case TK_GT:
        return OP_GT;
    case TK_LE:
        return OP_LE;
    case TK_GE:
        return OP_GE;
    case TK_EQ:
        return OP_EQ;
    case TK_NEQ:
        return OP_NEQ;
    case TK_AND:
        return OP_AND;
    case TK_OR:
        return OP_OR;
    default:
        return OP_NOP;
    }
}

static std::string get_key(VSValue *value)
{
    switch (value->type)
    {
    case NONE:
        return "__vs_none__";
    case BOOL:
        return "__vs_bool_" + value->to_string() + "__";
    case CHAR:
        return "__vs_char_" + value->to_string() + "__";
    case INT:
        return "__vs_int_" + value->to_string() + "__";
    case FLOAT:
        return "__vs_float_" + value->to_string() + "__";
    case STRING:
        return "__vs_str_" + value->to_string() + "__";
    default:
        return value->to_string();
    }
}

static void do_store(OPCODE opcode, ASTNode *assign_var)
{
    VSCodeObject *cur = codestack.top();
    auto locals = localstack.top();
    auto nonlocals = nonlocalstack.top();

    if (opcode != OP_NOP)
    {
        gen_expr_list(assign_var);
        cur->add_inst(VSInst(opcode));
    }

    if (assign_var->node_type == AST_IDENT)
    {
        std::string *name = assign_var->name;
        if (locals->find(*name) != locals->end())
        {
            cur->add_inst(VSInst(OP_STORE_LOCAL, (*locals)[*name]));
        }
        else
        {
            if (nonlocals->find(*name) == nonlocals->end())
            {
                cur->add_non_local(*name);
                (*nonlocals)[*name] = cur->nlvar_num - 1;
            }
            cur->add_inst(VSInst(OP_STORE_NAME, (*nonlocals)[*name]));
        }
    }
    else if (assign_var->node_type == AST_LST_IDX)
    {
        gen_expr_list(assign_var->list_index);
        gen_expr_list(assign_var->list_name);
        cur->add_inst(VSInst(OP_INDEX_STORE));
    }
}

static void gen_const(ASTNode *node)
{
    VSValue *value = node->value;
    auto consts = conststack.top();
    VSCodeObject *cur = codestack.top();
    std::string const_key = get_key(value);
    if (consts->find(const_key) == consts->end())
    {
        cur->add_const(new VSObject(value));
        (*consts)[const_key] = cur->const_num - 1;
    }
    vs_addr_t index = (*consts)[const_key];
    cur->add_inst(VSInst(OP_LOAD_CONST, index));
}

static void gen_ident(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    auto locals = localstack.top();
    auto nonlocals = nonlocalstack.top();

    std::string *name = node->name;
    if (locals->find(*name) != locals->end())
    {
        cur->add_inst(VSInst(OP_LOAD_LOCAL, (*locals)[*name]));
    }
    else
    {
        if (nonlocals->find(*name) == nonlocals->end())
        {
            cur->add_non_local(*name);
            (*nonlocals)[*name] = cur->nlvar_num - 1;
        }
        cur->add_inst(VSInst(OP_LOAD_NAME, (*nonlocals)[*name]));
    }
}

static void gen_b_expr(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    gen_expr_list(node->r_operand);
    gen_expr_list(node->l_operand);
    cur->add_inst(VSInst(get_b_op(node->b_opcode)));
}

static void gen_u_expr(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    gen_expr_list(node->operand);
    switch (node->u_opcode)
    {
    case TK_SUB:
        cur->add_inst(VSInst(OP_NEG));
        break;
    case TK_NOT:
        cur->add_inst(VSInst(OP_NOT));
        break;
    default:
        break;
    }
}

static void gen_list_idx(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    gen_expr_list(node->list_index);
    gen_expr_list(node->list_name);
    cur->add_inst(VSInst(OP_INDEX_LOAD));
}

static void gen_list_val(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    // tranverse the list from end to start
    int index = node->list_vals->size() - 1;
    while (index >= 0)
    {
        gen_expr_list(node->list_vals->at(index));
        index--;
    }
    cur->add_inst(VSInst(OP_BUILD_LIST, node->list_vals->size()));
}

static void gen_func_call(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    gen_list_val(node->arg_node);
    gen_expr_list(node->func_name);
    cur->add_inst(VSInst(OP_CALL));
}

static void gen_return(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    // set none as the default return value
    if (node->ret_val == NULL)
        cur->add_inst(VSInst(OP_LOAD_CONST, CONST_NONE_ADDR));
    else
        gen_expr_list(node->ret_val);
    cur->add_inst(VSInst(OP_RET));
}

static void gen_break(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    cur->add_inst(VSInst(OP_BREAK));
}

static void gen_continue(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    cur->add_inst(VSInst(OP_CONTINUE));
}

static void gen_expr(ASTNode *node)
{
    switch (node->node_type)
    {
    case AST_CONST:
        gen_const(node);
        break;
    case AST_IDENT:
        gen_ident(node);
        break;
    case AST_B_EXPR:
        gen_b_expr(node);
        break;
    case AST_U_EXPR:
        gen_u_expr(node);
        break;
    case AST_LST_IDX:
        gen_list_idx(node);
        break;
    case AST_LST_VAL:
        gen_list_val(node);
        break;
    case AST_FUNC_CALL:
        gen_func_call(node);
        break;
    default:
        break;
    }
}

static void gen_expr_list(ASTNode *node)
{
    if (node->node_type == AST_EXPR_LST)
    {
        for (auto expr : *node->expr_list)
        {
            gen_expr(expr);
        }
        return;
    }
    gen_expr(node);
}

static void gen_decl_stmt(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    auto locals = localstack.top();

    // "child" is decl node, contains ident and init
    for (auto child : *node->decl_list)
    {
        std::string *name = child->var_name->name;
        if (locals->find(*name) == locals->end())
        {
            cur->add_local(*name);
            (*locals)[*name] = cur->lvar_num - 1;
        }

        // Assign value.
        if (child->init_val != NULL)
        {
            gen_expr_list(child->init_val);
            cur->add_inst(VSInst(OP_STORE_LOCAL, (*locals)[*name]));
        }
    }
}

static void gen_assign_stmt(ASTNode *node)
{
    gen_expr_list(node->assign_val);
    do_store(get_b_op(node->assign_opcode), node->assign_var);
}

static void gen_input_stmt(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    for (auto arg : *node->list_vals)
    {
        cur->add_inst(VSInst(OP_INPUT));
        do_store(OP_NOP, arg);
    }
}

static void gen_print_stmt(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    for (auto arg : *node->list_vals)
    {
        gen_expr_list(arg);
        cur->add_inst(VSInst(OP_PRINT));
    }
}

static void gen_for_stmt(ASTNode *node)
{
    VSCodeObject *cur, *parent = codestack.top();
    parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
    parent->add_inst(VSInst(OP_GOTO));

    enter_blk("__vs_for__", LOOP_BLK);

    cur = codestack.top();
    parent->add_const(new VSObject(cur));

    if (node->for_init != NULL)
    {
        if (node->for_init->node_type == AST_DECL_LST)
        {
            gen_decl_stmt(node->for_init);
        }
        else
        {
            gen_expr_list(node->for_init);
        }
    }

    cur->loop_start = cur->inst_num;

    if (node->for_cond != NULL)
    {
        gen_expr_list(node->for_cond);
    }
    else
    {
        cur->add_inst(VSInst(OP_LOAD_CONST, CONST_TRUE_ADDR));
    }

    vs_addr_t jif_pos = cur->inst_num;
    cur->add_inst(VSInst(OP_JIF, jif_pos + 2));
    cur->add_inst(VSInst(OP_JMP, 0));

    gen_cpd_stmt(node->for_body);

    if (node->for_incr != NULL)
    {
        gen_expr_list(node->for_incr);
    }

    cur->add_inst(VSInst(OP_JMP, cur->loop_start));
    cur->code[jif_pos + 1].operand = cur->inst_num;
    cur->add_inst(VSInst(OP_NOP));

    leave_blk();
}

static void gen_func_decl(ASTNode *node)
{
    VSCodeObject *parent = codestack.top();
    auto locals = localstack.top();
    auto nonlocals = nonlocalstack.top();

    // Add function name to parent code locals.
    std::string *name = node->func_name->name;
    if (locals->find(*name) == locals->end())
    {
        parent->add_local(*name);
        (*locals)[*name] = parent->lvar_num - 1;
    }
    parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
    parent->add_inst(VSInst(OP_STORE_LOCAL, (*locals)[*name]));

    enter_blk("__vs_func__", FUNC_BLK);

    // Add function code to parent code consts.
    locals = localstack.top();
    VSCodeObject *cur = codestack.top();
    parent->add_const(new VSObject(cur));

    // Add function arg names to function locals.
    for (auto arg : *node->arg_node->list_vals)
    {
        cur->add_arg(*arg->name);
        (*locals)[*arg->name] = cur->lvar_num - 1;
    }

    // Gen function body.
    gen_cpd_stmt(node->func_body);

    leave_blk();
}

static void gen_elif_list(ASTNode *node)
{
    auto jmp_pos = std::vector<vs_size_t>();
    VSCodeObject *cur, *parent = codestack.top();
    for (auto child : *node->elif_list)
    {
        if (child->if_cond != NULL)
        {
            gen_expr_list(child->if_cond);
        }
        else
        {
            parent->add_inst(VSInst(OP_LOAD_CONST, CONST_TRUE_ADDR));
        }

        parent->add_inst(VSInst(OP_NOT));
        parent->add_inst(VSInst(OP_JIF, parent->inst_num + 4));
        parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
        parent->add_inst(VSInst(OP_GOTO));

        jmp_pos.push_back(parent->inst_num);
        parent->add_inst(VSInst(OP_JMP, 0));

        enter_blk("__vs_elif__", NORM_BLK);

        cur = codestack.top();
        parent->add_const(new VSObject(cur));

        gen_cpd_stmt(child->true_smt);

        leave_blk();
    }

    if (node->else_node != NULL)
    {
        parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
        parent->add_inst(VSInst(OP_GOTO));

        enter_blk("__vs_else__", NORM_BLK);

        cur = codestack.top();
        parent->add_const(new VSObject(cur));

        gen_cpd_stmt(node->else_node);

        leave_blk();
    }

    for (auto pos : jmp_pos)
    {
        parent->code[pos].operand = parent->inst_num;
    }
}

static void gen_if_stmt(ASTNode *node)
{
    VSCodeObject *cur, *parent = codestack.top();
    if (node->if_cond != NULL)
    {
        gen_expr_list(node->if_cond);
    }
    else
    {
        parent->add_inst(VSInst(OP_LOAD_CONST, CONST_TRUE_ADDR));
    }

    parent->add_inst(VSInst(OP_NOT));
    parent->add_inst(VSInst(OP_JIF, parent->inst_num + 4));
    parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
    parent->add_inst(VSInst(OP_GOTO));

    vs_size_t jmp_pos = parent->inst_num;
    parent->add_inst(VSInst(OP_JMP, 0));

    enter_blk("__vs_if__", NORM_BLK);

    cur = codestack.top();
    parent->add_const(new VSObject(cur));

    gen_cpd_stmt(node->true_smt);

    leave_blk();

    if (node->false_smt != NULL)
    {
        gen_elif_list(node->false_smt);
    }

    parent->code[jmp_pos].operand = parent->inst_num;
    parent->add_inst(VSInst(OP_NOP));
}

static void gen_while_stmt(ASTNode *node)
{
    VSCodeObject *cur, *parent = codestack.top();
    parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
    parent->add_inst(VSInst(OP_GOTO));

    enter_blk("__vs_while__", LOOP_BLK);

    cur = codestack.top();
    parent->add_const(new VSObject(cur));

    cur->loop_start = 0;
    if (node->while_cond != NULL)
    {
        gen_expr_list(node->for_init);
    }
    else
    {
        cur->add_inst(VSInst(OP_LOAD_CONST, CONST_TRUE_ADDR));
    }

    vs_addr_t jif_pos = cur->inst_num;
    cur->add_inst(VSInst(OP_JIF, jif_pos + 2));
    cur->add_inst(VSInst(OP_JMP, 0));

    gen_cpd_stmt(node->while_stmt);

    cur->add_inst(VSInst(OP_JMP, cur->loop_start));
    cur->code[jif_pos + 1].operand = cur->inst_num;
    cur->add_inst(VSInst(OP_NOP));

    leave_blk();
}

static void gen_cpd_stmt(ASTNode *node)
{
    for (auto stmt : *node->statements)
    {
        switch (stmt->node_type)
        {
        case AST_DECL_LST:
            gen_decl_stmt(stmt);
            break;
        case AST_FUNC_DECL:
            gen_func_decl(stmt);
            break;
        case AST_IF_STMT:
            gen_if_stmt(stmt);
            break;
        case AST_FOR_STMT:
            gen_for_stmt(stmt);
            break;
        case AST_WHILE_STMT:
            gen_while_stmt(stmt);
            break;
        case AST_INPUT_STMT:
            gen_input_stmt(stmt);
            break;
        case AST_PRINT_STMT:
            gen_print_stmt(stmt);
            break;
        case AST_ASSIGN:
            gen_assign_stmt(stmt);
            break;
        case AST_RETURN:
            gen_return(stmt);
            break;
        case AST_BREAK:
            gen_break(stmt);
            break;
        case AST_CONTINUE:
            gen_continue(stmt);
            break;
        case AST_EXPR_LST:
        default:
            gen_expr_list(stmt);
            break;
        }
    }
}

VSCodeObject *gencode(ASTNode *astree)
{
    codestack = std::stack<VSCodeObject *>();
    conststack = std::stack<std::unordered_map<std::string, vs_addr_t> *>();
    localstack = std::stack<std::unordered_map<std::string, vs_addr_t> *>();
    nonlocalstack = std::stack<std::unordered_map<std::string, vs_addr_t> *>();

    enter_blk("__vs_main__", NORM_BLK);

    gen_cpd_stmt(astree);

    auto program = codestack.top();

    leave_blk();

    return program;
}
