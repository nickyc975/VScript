#include "compiler.hpp"

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
static void gen_cpd_stmt(ASTNode *node);
static void gen_for_stmt(ASTNode *node);
static void gen_func_decl(ASTNode *node);
static void gen_elif_stmt(ASTNode *node);
static void gen_elif_list(ASTNode *node);
static void gen_if_stmt(ASTNode *node);
static void gen_io_stmt(ASTNode *node);
static void gen_initializer_list(ASTNode *node);
static void gen_initializer(ASTNode *node);
static void gen_init_decl(ASTNode *node);
static void gen_decl_stmt(ASTNode *node);
static void gen_while_stmt(ASTNode *node);
static VSCodeObject *gen_function(ASTNode *node);

static void gen_decl_stmt(ASTNode *node)
{

}

static VSCodeObject *gen_function(ASTNode *node)
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
    return gen_function(astree);
}
