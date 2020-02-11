#include "printers.hpp"

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
static void print_io_stmt_node(ASTNode *node);
static void print_continue_node(ASTNode *node);
static void print_break_node(ASTNode *node);
static void print_cpd_stmt_node(ASTNode *node);
static void print_return_node(ASTNode *node);

static void fprint_indent(FILE *file)
{
    int i = 0;
    for (; i < indent; i++)
    {
        fprintf(file, "    ");
    }
}

static void print_indent()
{
    fprint_indent(stdout);
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
    case AST_CONTINUE:
        print_continue_node(root);
        break;
    case AST_BREAK:
        print_break_node(root);
        break;
    case AST_PROGRAM:
    case AST_CPD_STMT:
        print_cpd_stmt_node(root);
        break;
    case AST_RETURN:
        print_return_node(root);
        break;
    case AST_INPUT_STMT:
    case AST_PRINT_STMT:
        print_io_stmt_node(root);
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
        printf("%lld\n", node->value->int_val);
        break;
    case FLOAT:
        printf("%f\n", node->value->float_val);
        break;
    case STRING:
        printf("%s\n", node->value->str_val->c_str());
        break;
    default:
        printf("\n");
        break;
    }
}

static void print_ident_node(ASTNode *node)
{
    print_indent();
    printf("ident: %s\n", node->name->c_str());
}

static void print_b_expr_node(ASTNode *node)
{
    print_indent();
    printf("b_expr: \n");
    indent++;
    print_ast(node->l_operand);
    print_indent();
    printf("b_opcode: %s\n", TOKEN_STR[node->b_opcode]);
    print_ast(node->r_operand);
    indent--;
}

static void print_u_expr_node(ASTNode *node)
{
    print_indent();
    printf("u_expr: \n");
    indent++;
    print_indent();
    printf("u_opcode: %s\n", TOKEN_STR[node->u_opcode]);
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
    printf("assign_opcode: %s\n", TOKEN_STR[node->assign_opcode]);
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
    print_indent();
    printf("func_call: \n");
    indent++;
    print_indent();
    printf("func name: \n");
    indent++;
    print_ast(node->func_name);
    indent--;
    print_indent();
    printf("func args: \n");
    indent++;
    print_ast(node->arg_node);
    indent--;
    indent--;
}

static void print_func_decl_node(ASTNode *node)
{
    print_indent();
    printf("func_decl: \n");
    indent++;
    print_ast(node->func_name);
    print_ast(node->arg_node);
    print_ast(node->func_body);
    indent--;
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
    print_indent();
    printf("for_stmt: \n");
    indent++;
    print_ast(node->for_init);
    print_ast(node->for_cond);
    print_ast(node->for_incr);
    print_ast(node->for_body);
    indent--;
}

static void print_io_stmt_node(ASTNode *node)
{
    print_indent();
    printf("%s: \n", node->node_type == AST_INPUT_STMT ? "input" : "print");
    indent++;
    for (ASTNode *child : *node->list_vals)
    {
        print_ast(child);
    }
    indent--;
}

static void print_continue_node(ASTNode *node)
{
    print_indent();
    printf("continue\n");
}

static void print_break_node(ASTNode *node)
{
    print_indent();
    printf("break\n");
}

static void print_cpd_stmt_node(ASTNode *node)
{
    print_indent();
    printf("stmts: \n");
    indent++;
    for (ASTNode *child : *node->statements)
    {
        print_ast(child);
        // printf("%d\n", child->node_type);
        // printf("%d\n", child->stmts_num);
    }
    indent--;
}

static void print_return_node(ASTNode *node)
{
    print_indent();
    printf("return: \n");
    indent++;
    print_ast(node->ret_val);
    indent--;
}

void fprint_code(FILE *file, VSCodeObject *code)
{
    int count = 0;
    VSObject *object;
    fprint_indent(file);
    fprintf(file, "%s: \n", code->name.c_str());
    for (auto inst : code->code)
    {
        fprint_indent(file);
        fprintf(file, "%d: %s\t", count, OPCODE_STR[inst.opcode]);
        switch (inst.opcode)
        {
        case OP_LOAD_NAME:
        case OP_STORE_NAME:
            fprintf(file, "%s\n", code->varnames[inst.operand].c_str());
            break;
        case OP_STORE_CONST:
        case OP_LOAD_CONST:
            object = &(code->consts[inst.operand]);
            if (object->type == OBJ_DATA)
            {
                fprintf(file, "%s\n", object->value->to_string().c_str());
            }
            else if (object->type == OBJ_CODE)
            {
                fprintf(file, "%s\n", object->codeblock->name.c_str());
            }
            else
            {
                fprintf(file, "list\n");
            }
            break;
        case OP_JIF:
        case OP_JMP:
        case OP_BUILD_LIST:
            fprintf(file, "%u\n", inst.operand);
            break;
        default:
            fprintf(file, "\n");
            break;
        }
        count++;
    }

    indent++;
    for (auto obj : code->consts)
    {
        if (obj.type == OBJ_CODE)
        {
            fprint_code(file, obj.codeblock);
        }
    }
    indent--;
}
