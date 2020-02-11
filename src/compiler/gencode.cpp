#include <stack>

#include "compiler.hpp"

std::stack<VSCodeObject *> codestack;
std::stack<std::unordered_map<std::string, vs_size_t> *> conststack;

static void gen_primary_expr(ASTNode *node);
static void gen_arg_expr_list(ASTNode *node);
static void gen_postfix_expr(ASTNode *node);
static void gen_unary_expr(ASTNode *node);
static void gen_multi_expr(ASTNode *node);
static void gen_additive_expr(ASTNode *node);
static void gen_relational_expr(ASTNode *node);
static void gen_equality_expr(ASTNode *node);
static void gen_logic_and_expr(ASTNode *node);
static void gen_logic_or_expr(ASTNode *node);
static void gen_assign_expr(ASTNode *node);
static void gen_expr(ASTNode *node);
static void gen_expr_stmt(ASTNode *node);
static void gen_stmt(ASTNode *node);
static void gen_io_stmt(ASTNode *node);
static void gen_initializer_list(ASTNode *node);
static void gen_initializer(ASTNode *node);
static void gen_init_decl(ASTNode *node);
static void gen_decl_stmt(ASTNode *node);
static void gen_cpd_stmt(ASTNode *node);

static VSCodeObject *gen_for_stmt(ASTNode *node);
static VSCodeObject *gen_func_decl(ASTNode *node);
static VSCodeObject *gen_elif_stmt(ASTNode *node);
static VSCodeObject *gen_if_stmt(ASTNode *node);
static VSCodeObject *gen_while_stmt(ASTNode *node);

static void gen_decl_stmt(ASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    // child is decl node, contains ident and init
    for (auto child : *node->decl_list)
    {
        cur->add_varname(*(child->var_name->name));
        if (child->init_val != NULL)
        {
            gen_expr(child->init_val);
            cur->add_inst(VSInst(OP_STORE_NAME, *(child->var_name->name)));
        }
    }
}

static void gen_cpd_stmt(ASTNode *node)
{
    VSObject *object;
    VSCodeObject *cur = codestack.top();
    for (auto stmt : *node->statements)
    {
        switch (stmt->node_type)
        {
        case AST_DECL_LST:
            gen_decl_stmt(stmt);
            break;
        case AST_FUNC_DECL:
            object = new VSObject(gen_func_decl(stmt));
            cur->add_const(object);
            break;
        case AST_IF_STMT:
            object = new VSObject(gen_if_stmt(stmt));
            cur->add_const(object);
            break;
        case AST_FOR_STMT:
            object = new VSObject(gen_for_stmt(stmt));
            cur->add_const(object);
            break;
        case AST_WHILE_STMT:
            object = new VSObject(gen_while_stmt(stmt));
            cur->add_const(object);
            break;
        case AST_INPUT_STMT:
        case AST_PRINT_STMT:
            gen_io_stmt(stmt);
            break;
        case AST_EXPR_LST:
            gen_expr_stmt(stmt);
            break;
        case AST_ASSIGN:
            gen_assign_expr(stmt);
            break;
        default:
            break;
        }
    }
}

VSCodeObject *gencode(ASTNode *astree)
{
    codestack = std::stack<VSCodeObject *>();
    conststack = std::stack<std::unordered_map<std::string, vs_size_t> *>();

    VSCodeObject *program = new VSCodeObject("__vs_main__", NORM_BLK);

    codestack.push(program);

    gen_cpd_stmt(astree);

    return codestack.top();
}
