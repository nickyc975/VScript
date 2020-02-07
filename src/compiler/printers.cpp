#include "printers.h"

static int indent = 0;

static void print_const_node(ASTNode *node);
static void print_ident_node(ASTNode *node);
static void print_b_expr_node(ASTNode *node);
static void print_u_expr_node(ASTNode *node);
static void print_expr_lst_node(ASTNode *node);
static void print_decl_node(ASTNode *node);
static void print_decl_lst_node(ASTNode *node);
static void print_assign_node(ASTNode *node);
static void print_lst_val_node(ASTNode *node);
static void print_lst_idx_node(ASTNode *node);
static void print_func_call_node(ASTNode *node);
static void print_func_decl_node(ASTNode *node);
static void print_if_stmt_node(ASTNode *node);
static void print_elif_lst_node(ASTNode *node);
static void print_while_stmt_node(ASTNode *node);
static void print_for_stmt_node(ASTNode *node);
static void print_jmp_node(ASTNode *node);
static void print_stmts_node(ASTNode *node);
static void print_return_node(ASTNode *node);

static void print_indent()
{
    int i = 0;
    for (; i < indent; i++)
    {
        printf("    ");
    }
}

void init_printer()
{
    indent = 0;
}

void print_ast(ASTNode *root)
{
    switch (root->node_type)
    {
    case AST_CONST:
        print_const_node(root);
        break;
    case AST_IDENT:
        print_ident_node(root);
        break;
    case AST_B_EXPR:
        print_b_expr_node(root);
        break;
    case AST_U_EXPR:
        print_u_expr_node(root);
        break;
    case AST_EXPR_LST:
        print_expr_lst_node(root);
        break;
    case AST_DECL:
        print_decl_node(root);
        break;
    case AST_DECL_LST:
        print_decl_lst_node(root);
        break;
    case AST_ASSIGN:
        print_assign_node(root);
        break;
    case AST_LST_VAL:
        print_lst_val_node(root);
        break;
    case AST_LST_IDX:
        print_lst_idx_node(root);
        break;
    case AST_FUNC_CALL:
        print_func_call_node(root);
        break;
    case AST_FUNC_DECL:
        print_func_decl_node(root);
        break;
    case AST_IF_STMT:
        print_if_stmt_node(root);
        break;
    case AST_ELIF_LST:
        print_elif_lst_node(root);
        break;
    case AST_WHILE_STMT:
        print_while_stmt_node(root);
        break;
    case AST_FOR_STMT:
        print_for_stmt_node(root);
        break;
    case AST_JMP:
        print_jmp_node(root);
        break;
    case AST_STMTS:
        print_stmts_node(root);
        break;
    case AST_RETURN:
        print_return_node(root);
        break;
    default:
        break;
    }
}

static void print_const_node(ASTNode *node)
{
    print_indent();
    printf("const: ");
    switch (node->value->type)
    {
    case NONE:
        printf("none\n");
        break;
    case BOOL:
        printf("%s\n", node->value->bool_val ? "true" : "false");
        break;
    case CHAR:
        printf("%c\n", node->value->char_val);
        break;
    case INT:
        printf("%d\n", node->value->int_val);
        break;
    case FLOAT:
        printf("%f\n", node->value->float_val);
        break;
    case STRING:
        printf("%s\n", node->value->str_val);
        break;
    default:
        printf("\n");
        break;
    }
}

static void print_ident_node(ASTNode *node)
{
    print_indent();
    printf("ident: %s\n", node->name);
}

static void print_b_expr_node(ASTNode *node)
{
    print_indent();
    printf("b_expr: \n");
    indent++;
    print_ast(node->l_operand);
    print_indent();
    printf("b_opcode: %s\n", INST_STR[node->b_opcode]);
    print_ast(node->r_operand);
    indent--;
}

static void print_u_expr_node(ASTNode *node)
{
    print_indent();
    printf("u_expr: \n");
    indent++;
    print_indent();
    printf("u_opcode: %s\n", INST_STR[node->u_opcode]);
    print_ast(node->operand);
    indent--;
}

static void print_expr_lst_node(ASTNode *node)
{
    print_indent();
    printf("expr_lst: \n");
    indent++;
    for (ASTNode *child : *node->expr_list)
    {

        print_ast(child);
    }
    indent--;
}

static void print_decl_node(ASTNode *node)
{
    print_indent();
    printf("decl: \n");
    indent++;
    print_ast(node->var_name);
    if (node->init_val)
    {
        print_ast(node->init_val);
    }
    indent--;
}

static void print_decl_lst_node(ASTNode *node)
{
    print_indent();
    printf("decl_lst: \n");
    indent++;
    for (ASTNode *child : *node->decl_list)
    {

        print_ast(child);
    }
    indent--;
}

static void print_assign_node(ASTNode *node)
{
    print_indent();
    printf("assign: \n");
    indent++;
    print_ast(node->assign_var);
    print_indent();
    printf("assign_opcode: %s\n", INST_STR[node->assign_opcode]);
    print_ast(node->assign_val);
    indent--;
}

static void print_lst_val_node(ASTNode *node)
{
    print_indent();
    printf("lst_val: \n");
    indent++;
    for (ASTNode *child : *node->list_vals)
    {

        print_ast(child);
    }
    indent--;
}

static void print_lst_idx_node(ASTNode *node)
{
    print_indent();
    printf("lst_idx: \n");
    indent++;
    print_indent();
    printf("list name: \n");
    indent++;
    print_ast(node->list_name);
    indent--;
    print_indent();
    printf("list index: \n");
    indent++;
    print_ast(node->list_index);
    indent--;
    indent--;
}

static void print_func_call_node(ASTNode *node)
{
}

static void print_func_decl_node(ASTNode *node)
{
}

static void print_if_stmt_node(ASTNode *node)
{
    print_indent();
    printf("if_stmt: \n");
    indent++;
    print_ast(node->if_cond);
    if (node->true_smt != NULL)
    {
        print_ast(node->true_smt);
    }
    if (node->false_smt != NULL)
    {
        print_ast(node->false_smt);
    }
    indent--;
}

static void print_elif_lst_node(ASTNode *node)
{
    print_indent();
    printf("elif_lst: \n");
    indent++;
    for (ASTNode *child : *node->elif_list)
    {
        print_ast(child);
        // printf("%d\n", child->node_type);
        // printf("%d\n", child->stmts_num);
    }
    indent--;
    if (node->else_node != NULL)
    {
        print_ast(node->else_node);
    }
}

static void print_while_stmt_node(ASTNode *node)
{
    print_indent();
    printf("while_stmt: \n");
    indent++;
    print_ast(node->while_cond);
    print_ast(node->while_stmt);
    indent--;
}

static void print_for_stmt_node(ASTNode *node)
{
}

static void print_jmp_node(ASTNode *node)
{
    print_indent();
    printf("jmp: \n");
    indent++;
    print_ast(node->curr_stmt);
    if (node->next_stmt != NULL)
    {
        print_ast(node->next_stmt);
    }
    indent--;
}

static void print_stmts_node(ASTNode *node)
{
    print_indent();
    printf("stmts: \n");
    indent++;
    for (ASTNode *child : *node->stmts)
    {
        print_ast(child);
        // printf("%d\n", child->node_type);
        // printf("%d\n", child->stmts_num);
    }
    indent--;
}

static void print_return_node(ASTNode *node)
{
}