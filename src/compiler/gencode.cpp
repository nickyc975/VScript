#include <stack>

#include "compiler.hpp"

#define enter_blk(name, type) \
    codestack.push(new VSCodeObject(name, type)); \
    conststack.push(new std::unordered_map<std::string, vs_addr_t>()); \
    varnamestack.push(new std::unordered_map<std::string, vs_addr_t>());

#define leave_blk() \
    codestack.pop(); \
    delete conststack.top(); \
    conststack.pop(); \
    delete varnamestack.top(); \
    varnamestack.pop();

std::stack<VSCodeObject *> codestack;
std::stack<std::unordered_map<std::string, vs_addr_t> *> conststack;
std::stack<std::unordered_map<std::string, vs_addr_t> *> varnamestack;

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
static void gen_decl_stmt(ASTNode *node);
static void gen_assign_stmt(ASTNode *node);
static void gen_input_stmt(ASTNode *node);
static void gen_print_stmt(ASTNode *node);
static void gen_cpd_stmt(ASTNode *node);
static void gen_for_stmt(ASTNode *node);
static void gen_func_decl(ASTNode *node);
static void gen_elif_stmt(ASTNode *node);
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

static void do_store(OPCODE opcode, ASTNode *assign_var)
{
    VSCodeObject *cur = codestack.top();
    auto varnames = varnamestack.top();
    if (opcode != OP_NOP)
    {
        gen_expr(assign_var);
        cur->add_inst(VSInst(opcode));
    }
    if (assign_var->node_type == AST_IDENT)
    {
        std::string *name = assign_var->name;
        cur->add_inst(VSInst(OP_STORE_NAME, varnames->at(*name)));
    }
    else if (assign_var->node_type == AST_LST_IDX)
    {
        gen_expr(assign_var->list_index);
        gen_expr(assign_var->list_name);
        cur->add_inst(VSInst(OP_INDEX_STORE));
    }
}

static void gen_const(ASTNode *node)
{
    VSValue *value = node->value;
    auto consts = conststack.top();
    VSCodeObject *cur = codestack.top();
    if (consts->find(value->to_string()) != consts->end())
    {
        cur->add_const(new VSObject(value));
        consts->insert_or_assign(value->to_string(), cur->const_num - 1);
    }
    vs_addr_t index = consts->at(value->to_string());
    cur->add_inst(VSInst(OP_LOAD_CONST, index));
}

static void gen_ident(ASTNode *node)
{
    std::string *name = node->name;
    auto varnames = varnamestack.top();
    VSCodeObject *cur = codestack.top();
    if (varnames->find(*name) != varnames->end())
    {
        cur->add_varname(*name);
        varnames->insert_or_assign(*name, cur->lvar_num - 1);
    }
    vs_addr_t index = varnames->at(*name);
    cur->add_inst(VSInst(OP_LOAD_NAME, index));
}

static void gen_b_expr(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    gen_expr(node->r_operand);
    gen_expr(node->l_operand);
    cur->add_inst(VSInst(get_b_op(node->b_opcode)));
}

static void gen_u_expr(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    gen_expr(node->operand);
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
    gen_expr(node->list_index);
    gen_expr(node->list_name);
    cur->add_inst(VSInst(OP_INDEX_LOAD));
}

static void gen_list_val(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    // tranverse the list from end to start
    int index = node->list_vals->size() - 1;
    while (index >= 0)
    {
        gen_expr(node->list_vals->at(index));
        index--;
    }
    cur->add_inst(VSInst(OP_BUILD_LIST, node->list_vals->size()));
}

static void gen_func_call(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    gen_list_val(node->arg_node);
    gen_expr(node->func_name);
    cur->add_inst(VSInst(OP_CALL));
}

static void gen_return(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    // set none as the default return value
    if (node->ret_val == NULL)
        cur->add_inst(VSInst(OP_LOAD_CONST, 0));
    else
        gen_expr(node->ret_val);
    cur->add_inst(VSInst(OP_RET));
}

static void gen_break(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    cur->add_inst(VSInst(OP_RET));
}

static void gen_continue(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    cur->add_inst(VSInst(OP_LOOP));
}

static void gen_expr(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
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

static void gen_decl_stmt(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    auto varnames = varnamestack.top();
    // child is decl node, contains ident and init
    for (auto child : *node->decl_list)
    {
        std::string *name = child->var_name->name;
        if (varnames->find(*name) == varnames->end())
        {
            cur->add_varname(*name);
            varnames->insert_or_assign(*name, cur->lvar_num - 1);
        }
        vs_addr_t index = varnames->at(*name);
        if (child->init_val != NULL)
        {
            gen_expr(child->init_val);
            cur->add_inst(VSInst(OP_STORE_NAME, index));
        }
    }
}

static void gen_assign_stmt(ASTNode *node)
{
    gen_expr(node->assign_val);
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
        gen_expr(arg);
        cur->add_inst(VSInst(OP_PRINT));
    }
}

static void gen_for_stmt(ASTNode *node)
{
    
}

static void gen_func_decl(ASTNode *node)
{

}

static void gen_elif_stmt(ASTNode *node)
{

}

static void gen_if_stmt(ASTNode *node)
{

}

static void gen_while_stmt(ASTNode *node)
{

}

static void gen_cpd_stmt(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
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
            gen_expr(stmt);
            break;
        }
    }
}

VSCodeObject *gencode(ASTNode *astree)
{
    codestack = std::stack<VSCodeObject *>();
    conststack = std::stack<std::unordered_map<std::string, vs_addr_t> *>();
    varnamestack = std::stack<std::unordered_map<std::string, vs_addr_t> *>();

    enter_blk("__vs_main__", NORM_BLK);

    gen_cpd_stmt(astree);

    auto program = codestack.top();

    leave_blk();

    return program;
}
