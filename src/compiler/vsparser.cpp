#include "vsparser.hpp"

// Create new table for compound statements.
#define new_table() cur_table = new SymTable<ASTNode *>(cur_table)

// Restore parent table.
#define restore_table() cur_table = cur_table->get_parent()

// Ensure that there are tokens.
#define ensure_token(ret_val) \
    if (!has_token()) \
    { \
        err("unexpected end of file\n");\
        return ret_val;\
    }

static unsigned int tk_idx;
static std::vector<Token *> *tks;
static SymTable<ASTNode *> *cur_table;

static bool need_fill_back;
static ASTNode *prev_stmt;

static ASTNode *read_primary_expr();
static ASTNode *read_arg_expr_list();
static ASTNode *read_postfix_expr();
static ASTNode *read_unary_expr();
static ASTNode *read_multi_expr();
static ASTNode *read_additive_expr();
static ASTNode *read_relational_expr();
static ASTNode *read_equality_expr();
static ASTNode *read_logic_and_expr();
static ASTNode *read_logic_or_expr();
static ASTNode *read_assign_expr();
static ASTNode *read_expr();
static ASTNode *read_expr_stmt();
static ASTNode *read_stmt();
static ASTNode *read_cpd_stmt();
static ASTNode *read_for_stmt();
static ASTNode *read_func_decl();
static ASTNode *read_elif_stmt();
static ASTNode *read_elif_list();
static ASTNode *read_if_stmt();
static ASTNode *read_io_stmt();
static ASTNode *read_initializer_list();
static ASTNode *read_initializer();
static ASTNode *read_init_decl(bool is_mutable);
static ASTNode *read_decl_stmt();
static ASTNode *read_while_stmt();
static ASTNode *read_program();

static bool has_token()
{
    return tk_idx < tks->size();
}

static int tokens_left()
{
    return tks->size() - tk_idx;
}

static Token *get_token()
{
    return tks->at(tk_idx++);
}

static void unget_token()
{
    if (tk_idx > 0)
        tk_idx--;
}

static Token *peek_token()
{
    return tks->at(tk_idx);
}

static Token *next_token()
{
    return tks->at(tk_idx + 1);
}

static bool is_arith(INST inst)
{
    return inst >= I_ADD && inst <= I_NEQ;
}

static bool is_cmp(INST inst)
{
    return inst >= I_LT && inst <= I_NEQ;
}

static bool is_logic(INST inst)
{
    return inst >= I_EQ && inst <= I_OR;
}

static bool is_assign(KIND kind)
{
    switch (kind)
    {
    case ASSIGN:
    case ADD_ASSIGN:
    case SUB_ASSIGN:
    case MUL_ASSIGN:
    case DIV_ASSIGN:
    case MOD_ASSIGN:
    case AND_ASSIGN:
    case OR_ASSIGN:
        return true;
    default:
        return false;
    }
}

static bool is_relation(KIND kind)
{
    switch (kind)
    {
    case LT:
    case LE:
    case GT:
    case GE:
        return true;
    default:
        return false;
    }
}

static bool is_mul(KIND kind)
{
    switch (kind)
    {
    case MUL:
    case DIV:
    case MOD:
        return true;
    default:
        return false;
    }
}

static INST get_assign_opcode(KIND kind)
{
    switch (kind)
    {
    case ADD_ASSIGN:
        return I_ADD;
    case SUB_ASSIGN:
        return I_SUB;
    case MUL_ASSIGN:
        return I_MUL;
    case DIV_ASSIGN:
        return I_DIV;
    case MOD_ASSIGN:
        return I_MOD;
    case AND_ASSIGN:
        return I_AND;
    case OR_ASSIGN:
        return I_OR;
    case ASSIGN:
    default:
        return I_NOP;
    }
}

static INST get_rel_opcode(KIND kind)
{
    switch (kind)
    {
    case LT:
        return I_LT;
    case LE:
        return I_LE;
    case GT:
        return I_GT;
    case GE:
        return I_GE;
    default:
        return I_NOP;
    }
}

static INST get_mul_opcode(KIND kind)
{
    switch (kind)
    {
    case MUL:
        return I_MUL;
    case DIV:
        return I_DIV;
    case MOD:
        return I_MOD;
    default:
        return I_NOP;
    }
}

static bool ensure_lval(ASTNode *node)
{
    switch (node->node_type)
    {
    case AST_IDENT:
        if (!node->is_mutable)
        {
            ensure_token(false);
            err("line: %ld, \"%s\" is immutable\n", peek_token()->ln, node->name);
            return false;
        }
        break;
    case AST_LST_IDX:
        break;
    default:
        ensure_token(false);
        err("line: %ld, illegal left value\n", peek_token()->ln);
        return false;
    }
    return true;
}

static bool ensure_in_iter()
{
    SymTable<ASTNode *> *temp = cur_table;
    while (temp != NULL)
    {
        if (temp->top->node_type == AST_WHILE_STMT || temp->top->node_type == AST_FOR_STMT)
        {
            return true;
        }
        temp = temp->get_parent();
    }
    ensure_token(false);
    err("line %ld, continue or break must be in while or for statement\n", peek_token()->ln);
    return false;
}

static bool ensure_in_func_decl()
{
    SymTable<ASTNode *> *temp = cur_table;
    while (temp != NULL)
    {
        if (temp->top->node_type == AST_FUNC_DECL)
        {
            return true;
        }
        temp = temp->get_parent();
    }
    ensure_token(false);
    err("line %ld, return must be in function declaration\n", peek_token()->ln);
    return false;
}

static void fill_back(ASTNode *prev, ASTNode *next)
{
    switch (prev->node_type)
    {
    case AST_FOR_STMT:
        prev->for_next = next;
        break;
    case AST_WHILE_STMT:
        prev->while_next = next;
        break;
    case AST_IF_STMT:
        if (prev->false_smt == NULL)
        {
            prev->false_smt = next;
        }
        else if (prev->false_smt->node_type == AST_ELIF_LST && prev->false_smt->else_node == NULL)
        {
            prev->false_smt->else_node = next;
        }
        break;
    default:
        break;
    }
}

static ASTNode *cal_u_expr(INST op, ASTNode *value)
{
    if (op == I_NOT && value->value->type == BOOL)
    {
        value->value->bool_val = !value->value->bool_val;
    }
    else if (op == I_SUB)
    {
        switch (value->value->type)
        {
        case CHAR:
            value->value->char_val = -value->value->char_val;
            break;
        case INT:
            value->value->int_val = -value->value->int_val;
            break;
        case FLOAT:
            value->value->float_val = -value->value->float_val;
            break;
        default:
            break;
        }
    }
    else
    {
        unget_token();
        err("line: %ld, can not apply operation \"%s\" on type \"%s\"\n", 
            peek_token()->ln, INST_STR[op], TYPE_STR[value->value->type]);
        get_token();
    }
    return value;
}

static ASTNode *cal_bool_b_expr(INST op, ASTNode *left, ASTNode *right)
{
    if ((left->value->type != right->value->type) || !is_logic(op))
    {
        unget_token();
        err("line: %ld, can not apply operation \"%s\" on type \"%s\" and \"%s\"\n", 
            peek_token()->ln, INST_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        return NULL;
    }

    switch (op)
    {
    case I_EQ:
        left->value->bool_val = left->value->bool_val == right->value->bool_val;
        break;
    case I_NEQ:
        left->value->bool_val = left->value->bool_val != right->value->bool_val;
        break;
    case I_AND:
        left->value->bool_val = left->value->bool_val && right->value->bool_val;
        break;
    case I_OR:
        left->value->bool_val = left->value->bool_val || right->value->bool_val;
        break;
    default:
        err("internal error\n");
        break;
    }
    return left;
}

static ASTNode *cal_char_b_expr(INST op, ASTNode *left, ASTNode *right)
{
    if ((left->value->type != right->value->type) || !is_arith(op))
    {
        unget_token();
        err("line: %ld, can not apply operation \"%s\" on type \"%s\" and \"%s\"\n", 
            peek_token()->ln, INST_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        return NULL;
    }

    switch (op)
    {
    case I_ADD:
        left->value->char_val = left->value->char_val + right->value->char_val;
        break;
    case I_SUB:
        left->value->char_val = left->value->char_val - right->value->char_val;
        break;
    case I_MUL:
        left->value->char_val = left->value->char_val * right->value->char_val;
        break;
    case I_DIV:
        left->value->char_val = left->value->char_val / right->value->char_val;
        break;
    case I_MOD:
        left->value->char_val = left->value->char_val % right->value->char_val;
        break;
    case I_LT:
        left->value->bool_val = left->value->char_val < right->value->char_val;
        left->value->type = BOOL;
        break;
    case I_GT:
        left->value->bool_val = left->value->char_val > right->value->char_val;
        left->value->type = BOOL;
        break;
    case I_LE:
        left->value->bool_val = left->value->char_val <= right->value->char_val;
        left->value->type = BOOL;
        break;
    case I_GE:
        left->value->bool_val = left->value->char_val >= right->value->char_val;
        left->value->type = BOOL;
        break;
    case I_EQ:
        left->value->bool_val = left->value->char_val == right->value->char_val;
        left->value->type = BOOL;
        break;
    case I_NEQ:
        left->value->bool_val = left->value->char_val != right->value->char_val;
        left->value->type = BOOL;
        break;
    default:
        err("internal error\n");
        break;
    }
    return left;
}

static ASTNode *cal_int_b_expr(INST op, ASTNode *left, ASTNode *right)
{
    if ((left->value->type != right->value->type) || !is_arith(op))
    {
        unget_token();
        err("line: %ld, can not apply operation \"%s\" on type \"%s\" and \"%s\"\n", 
            peek_token()->ln, INST_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        return NULL;
    }

    switch (op)
    {
    case I_ADD:
        left->value->int_val = left->value->int_val + right->value->int_val;
        break;
    case I_SUB:
        left->value->int_val = left->value->int_val - right->value->int_val;
        break;
    case I_MUL:
        left->value->int_val = left->value->int_val * right->value->int_val;
        break;
    case I_DIV:
        left->value->int_val = left->value->int_val / right->value->int_val;
        break;
    case I_MOD:
        left->value->int_val = left->value->int_val % right->value->int_val;
        break;
    case I_LT:
        left->value->bool_val = left->value->int_val < right->value->int_val;
        left->value->type = BOOL;
        break;
    case I_GT:
        left->value->bool_val = left->value->int_val > right->value->int_val;
        left->value->type = BOOL;
        break;
    case I_LE:
        left->value->bool_val = left->value->int_val <= right->value->int_val;
        left->value->type = BOOL;
        break;
    case I_GE:
        left->value->bool_val = left->value->int_val >= right->value->int_val;
        left->value->type = BOOL;
        break;
    case I_EQ:
        left->value->bool_val = left->value->int_val == right->value->int_val;
        left->value->type = BOOL;
        break;
    case I_NEQ:
        left->value->bool_val = left->value->int_val != right->value->int_val;
        left->value->type = BOOL;
        break;
    default:
        err("internal error\n");
        break;
    }
    return left;
}

static ASTNode *cal_float_b_expr(INST op, ASTNode *left, ASTNode *right)
{
    if ((left->value->type != right->value->type) || !is_arith(op) || op == I_MOD)
    {
        unget_token();
        err("line: %ld, can not apply operation \"%s\" on type \"%s\" and \"%s\"\n", 
            peek_token()->ln, INST_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        return NULL;
    }

    switch (op)
    {
    case I_ADD:
        left->value->float_val = left->value->float_val + right->value->float_val;
        break;
    case I_SUB:
        left->value->float_val = left->value->float_val - right->value->float_val;
        break;
    case I_MUL:
        left->value->float_val = left->value->float_val * right->value->float_val;
        break;
    case I_DIV:
        left->value->float_val = left->value->float_val / right->value->float_val;
        break;
    case I_LT:
        left->value->bool_val = left->value->float_val < right->value->float_val;
        left->value->type = BOOL;
        break;
    case I_GT:
        left->value->bool_val = left->value->float_val > right->value->float_val;
        left->value->type = BOOL;
        break;
    case I_LE:
        left->value->bool_val = left->value->float_val <= right->value->float_val;
        left->value->type = BOOL;
        break;
    case I_GE:
        left->value->bool_val = left->value->float_val >= right->value->float_val;
        left->value->type = BOOL;
        break;
    case I_EQ:
        left->value->bool_val = left->value->float_val == right->value->float_val;
        left->value->type = BOOL;
        break;
    case I_NEQ:
        left->value->bool_val = left->value->float_val != right->value->float_val;
        left->value->type = BOOL;
        break;
    default:
        err("internal error\n");
        break;
    }
    return left;
}

static ASTNode *cal_str_b_expr(INST op, ASTNode *left, ASTNode *right)
{
    if ((left->value->type != right->value->type) || !(op == I_ADD || is_cmp(op)))
    {
        unget_token();
        err("line: %ld, can not apply operation \"%s\" on type \"%s\" and \"%s\"\n", 
            peek_token()->ln, INST_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        return NULL;
    }

    unsigned int len;
    switch (op)
    {
    case I_ADD:
        len = strlen(left->value->str_val) + strlen(left->value->str_val);
        left->value->str_val = (char *)realloc(left->value->str_val, len + 1);
        strcat(left->value->str_val, right->value->str_val);
        break;
    case I_LT:
        left->value->bool_val = strcmp(left->value->str_val, right->value->str_val) < 0;
        left->value->type = BOOL;
        break;
    case I_GT:
        left->value->bool_val = strcmp(left->value->str_val, right->value->str_val) > 0;
        left->value->type = BOOL;
        break;
    case I_LE:
        left->value->bool_val = strcmp(left->value->str_val, right->value->str_val) <= 0;
        left->value->type = BOOL;
        break;
    case I_GE:
        left->value->bool_val = strcmp(left->value->str_val, right->value->str_val) >= 0;
        left->value->type = BOOL;
        break;
    case I_EQ:
        left->value->bool_val = strcmp(left->value->str_val, right->value->str_val) == 0;
        left->value->type = BOOL;
        break;
    case I_NEQ:
        left->value->bool_val = strcmp(left->value->str_val, right->value->str_val) != 0;
        left->value->type = BOOL;
        break;
    default:
        err("internal error\n");
        break;
    }
    return left;
}

static ASTNode *cal_b_expr(INST op, ASTNode *left, ASTNode *right)
{
    switch (left->value->type)
    {
    case BOOL:
        return cal_bool_b_expr(op, left, right);
    case CHAR:
        return cal_char_b_expr(op, left, right);
    case INT:
        return cal_int_b_expr(op, left, right);
    case FLOAT:
        return cal_float_b_expr(op, left, right);
    case STRING:
        return cal_str_b_expr(op, left, right);
    default:
        unget_token();
        err("line: %ld, can not apply operation \"%s\" on type \"%s\" and \"%s\"\n", 
            peek_token()->ln, INST_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        return NULL;
    }
}

static ASTNode *ident_node(char *name, bool is_mutable)
{
    ASTNode *node = new ASTNode(AST_IDENT, AST_EXPR);
    node->type = NONE;
    node->name = (char *)malloc(strlen(name));
    strcpy(node->name, name);
    node->is_mutable = is_mutable;
    return node;
}

static ASTNode *const_node(Value *value)
{
    ASTNode *node = new ASTNode(AST_CONST, AST_EXPR);
    node->type = value->type;
    node->value = value;
    return node;
}

static ASTNode *b_expr_node(INST b_opcode, ASTNode *l_operand, ASTNode *r_operand)
{
    ASTNode *node;
    if (l_operand->node_type == AST_CONST && r_operand->node_type == AST_CONST)
    {
        node = cal_b_expr(b_opcode, l_operand, r_operand);
    }
    else
    {
        node = new ASTNode(AST_B_EXPR, AST_EXPR);
        node->b_opcode = b_opcode;
        node->l_operand = l_operand;
        node->r_operand = r_operand;
    }
    return node;
}

static ASTNode *u_expr_node(INST u_opcode, ASTNode *operand)
{
    ASTNode *node;
    if (operand->node_type == AST_CONST)
        node = cal_u_expr(u_opcode, operand);
    else
    {
        node = new ASTNode(AST_U_EXPR, AST_EXPR);
        node->b_opcode = u_opcode;
        node->operand = operand;
    }
    return node;
}

static ASTNode *expr_lst_node(std::vector<ASTNode *> *expr_list)
{
    ASTNode *node = new ASTNode(AST_EXPR_LST, AST_UNKNOW);
    node->expr_list = expr_list;
    return node;
}

static ASTNode *decl_node(ASTNode *var_name, ASTNode *init_val)
{
    ASTNode *node = new ASTNode(AST_DECL, AST_UNKNOW);
    node->var_name = var_name;
    node->init_val = init_val;
    return node;
}

static ASTNode *decl_lst_node(std::vector<ASTNode *> *decl_list)
{
    ASTNode *node = new ASTNode(AST_DECL_LST, AST_UNKNOW);
    node->decl_list = decl_list;
    return node;
}

static ASTNode *assign_node(INST assign_opcode, ASTNode *assign_var, ASTNode *assign_val)
{
    ASTNode *node = new ASTNode(AST_ASSIGN, AST_UNKNOW);
    node->assign_opcode = assign_opcode;
    node->assign_var = assign_var;
    node->assign_val = assign_val;
    return node;
}

static ASTNode *lst_val_node(std::vector<ASTNode *> *list_vals)
{
    ASTNode *node = new ASTNode(AST_LST_VAL, AST_EXPR);
    node->type = LIST;
    node->list_vals = list_vals;
    return node;
}

static ASTNode *lst_idx_node(ASTNode *list_name, ASTNode *list_index)
{
    ASTNode *node = new ASTNode(AST_LST_IDX, AST_EXPR);
    node->list_name = list_name;
    node->list_index = list_index;
    return node;
}

static ASTNode *func_call_node(ASTNode *func_name, ASTNode *arg_node)
{
    ASTNode *node = new ASTNode(AST_FUNC_CALL, AST_EXPR);
    node->func_name = func_name;
    node->arg_node = arg_node;
    return node;
}

static ASTNode *func_decl_node(ASTNode *func_name, ASTNode *arg_node, ASTNode *func_body)
{
    ASTNode *node = new ASTNode(AST_FUNC_DECL, AST_UNKNOW);
    node->func_name = func_name;
    node->arg_node = arg_node;
    node->func_body = func_body;
    return node;
}

static ASTNode *if_stmt_node(ASTNode *if_cond, ASTNode *true_stmt, ASTNode *false_stmt)
{
    ASTNode *node = new ASTNode(AST_IF_STMT, AST_UNKNOW);
    node->if_cond = if_cond;
    node->true_smt = true_stmt;
    node->false_smt = false_stmt;
    return node;
}

static ASTNode *elif_lst_node(std::vector<ASTNode *> *elif_list, ASTNode *else_node)
{
    ASTNode *node = new ASTNode(AST_ELIF_LST, AST_UNKNOW);
    node->elif_list = elif_list;
    node->else_node = else_node;
    return node;
}

static ASTNode *while_stmt_node(ASTNode *while_cond, ASTNode *while_stmt, ASTNode *while_next)
{
    ASTNode *node = new ASTNode(AST_WHILE_STMT, AST_UNKNOW);
    node->while_cond = while_cond;
    node->while_stmt = while_stmt;
    return node;
}

static ASTNode *for_stmt_node(ASTNode *for_init, ASTNode *for_cond, ASTNode *for_incr, ASTNode *for_body, ASTNode *for_next)
{
    ASTNode *node = new ASTNode(AST_FOR_STMT, AST_UNKNOW);
    node->for_init = for_init;
    node->for_cond = for_cond;
    node->for_incr = for_incr;
    node->for_body = for_body;
    return node;
}

static ASTNode *continue_node()
{
    ASTNode *node = new ASTNode(AST_CONTINUE, AST_UNKNOW);
    return node;
}

static ASTNode *break_node()
{
    ASTNode *node = new ASTNode(AST_BREAK, AST_UNKNOW);
    return node;
}

static ASTNode *cpd_stmt_node(std::vector<ASTNode *> *stmts, SymTable<ASTNode *> *symtable)
{
    ASTNode *node = new ASTNode(AST_CPD_STMT, AST_UNKNOW);
    node->statements = stmts;
    node->symtable = symtable;
    return node;
}

static ASTNode *program_node(std::vector<ASTNode *> *stmts, SymTable<ASTNode *> *symtable)
{
    ASTNode *node = new ASTNode(AST_PROGRAM, AST_UNKNOW);
    node->statements = stmts;
    node->symtable = symtable;
    return node;
}

static ASTNode *return_node(ASTNode *ret_val)
{
    ASTNode *node = new ASTNode(AST_RETURN, AST_UNKNOW);
    node->ret_val = ret_val;
    return node;
}

static ASTNode *print_stmt_node(std::vector<ASTNode *> *arg_list)
{
    ASTNode *node = new ASTNode(AST_PRINT_STMT, AST_UNKNOW);
    node->list_vals = arg_list;
    return node;
}

static ASTNode *input_stmt_node(std::vector<ASTNode *> *arg_list)
{
    ASTNode *node = new ASTNode(AST_INPUT_STMT, AST_UNKNOW);
    node->list_vals = arg_list;
    return node;
}

static Token *expect(KIND kind)
{
    ensure_token(NULL);
    Token *token = get_token();
    if (token->kind != kind)
    {
        err("line: %ld, expected \"%s\" but got \"%s\"\n", token->ln, KIND_STR[kind], KIND_STR[token->kind]);
    }
    return token;
}

static Token *expect2(KIND kind1, KIND kind2)
{
    ensure_token(NULL);
    Token *token = get_token();
    if (token->kind != kind1 && token->kind != kind2)
    {
        err("line: %ld, expected \"%s\" or \"%s\" but got \"%s\"\n", token->ln, KIND_STR[kind1], KIND_STR[kind1], KIND_STR[token->kind]);
    }
    return token;
}

static ASTNode *read_primary_expr()
{
    Token *token;
    ASTNode *node = NULL;

    ensure_token(NULL);
    switch (peek_token()->kind)
    {
    case CONSTANT:
        node = const_node(get_token()->value);
        break;
    case IDENTIFIER:
        token = get_token();
        if (!cur_table->contains_recur(token->identifier))
        {
            err("line %ld, \"%s\" not defined\n", token->ln, token->identifier);
            break;
        }
        node = cur_table->get_recur(token->identifier);
        break;
    case L_PAREN:
        expect(L_PAREN);
        node = read_expr();
        expect(R_PAREN);
        break;
    default:
        break;
    }
    return node;
}

static ASTNode *read_arg_expr_list()
{
    ASTNode *arg_list = lst_val_node(new std::vector<ASTNode *>());
    ensure_token(arg_list);
    if (peek_token()->kind == R_PAREN)
        return arg_list;
    ASTNode *arg = read_logic_or_expr();
    if (arg == NULL)
    {
        ensure_token(arg_list);
        err("line: %ld, missing args\n", peek_token()->ln);
    }
    arg_list->list_vals->push_back(arg);
    ensure_token(arg_list);
    while (peek_token()->kind == COMMA)
    {
        expect(COMMA);
        arg = read_logic_or_expr();
        if (arg == NULL)
        {
            ensure_token(arg_list);
            err("line: %ld, missing args\n", peek_token()->ln);
        }
        arg_list->list_vals->push_back(arg);
        ensure_token(arg_list);
    }
    return arg_list;
}

static ASTNode *read_postfix_expr()
{
    ASTNode *node = read_primary_expr();
    if (node == NULL || !has_token())
        return node;
    
    switch (peek_token()->kind)
    {
    case L_BRACK:
    {
        expect(L_BRACK);
        ASTNode *lst_idx = read_expr();
        if (lst_idx == NULL)
        {
            ensure_token(node);
            err("line: %ld, invalid list index\n", peek_token()->ln);
        }
        node = lst_idx_node(node, lst_idx);
        expect(R_BRACK);
        break;
    }
    case L_PAREN:
    {
        expect(L_PAREN);
        ASTNode *arg_list = read_arg_expr_list();
        if (arg_list == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing args\n", peek_token()->ln);
        }
        node = func_call_node(node, arg_list);
        expect(R_PAREN);
        break;
    }
    default:
        break;
    }
    return node;
}

static ASTNode *read_unary_expr()
{
    Token *token = NULL;
    ensure_token(NULL);
    if (peek_token()->kind == SUB || peek_token()->kind == NOT)
    {
        token = get_token();
    }
    ASTNode *node = read_postfix_expr();
    if (node == NULL || !has_token())
        return node;
    if (token != NULL)
    {
        node = u_expr_node(token->kind == SUB ? I_SUB : I_NOT, node);
    }
    return node;
}

static ASTNode *read_multi_expr()
{
    Token *token;
    ASTNode *node = read_unary_expr();
    if (node == NULL || !has_token())
        return node;
    while (is_mul(peek_token()->kind))
    {
        token = get_token();
        ASTNode *right = read_unary_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(get_mul_opcode(token->kind), node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_additive_expr()
{
    Token *token;
    ASTNode *node = read_multi_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->kind == ADD || peek_token()->kind == SUB)
    {
        token = get_token();
        ASTNode *right = read_multi_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(token->kind == ADD ? I_ADD : I_SUB, node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_relational_expr()
{
    Token *token;
    ASTNode *node = read_additive_expr();
    if (node == NULL || !has_token())
        return node;
    while (is_relation(peek_token()->kind))
    {
        token = get_token();
        ASTNode *right = read_additive_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(get_rel_opcode(token->kind), node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_equality_expr()
{
    Token *token;
    ASTNode *node = read_relational_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->kind == EQ || peek_token()->kind == NEQ)
    {
        token = get_token();
        ASTNode *right = read_relational_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(token->kind == EQ ? I_EQ : I_NEQ, node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_logic_and_expr()
{
    ASTNode *node = read_equality_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->kind == AND)
    {
        expect(AND);
        ASTNode *right = read_equality_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(I_AND, node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_logic_or_expr()
{
    ASTNode *node = read_logic_and_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->kind == OR)
    {
        expect(OR);
        ASTNode *right = read_logic_and_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(I_OR, node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_assign_expr()
{
    ASTNode *node = read_logic_or_expr();
    if (node == NULL || !has_token())
        return node;
    if (is_assign(peek_token()->kind))
    {
        ensure_lval(node);
        INST assign_opcode = get_assign_opcode(get_token()->kind);
        ASTNode *right = read_logic_or_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = assign_node(assign_opcode, node, right);
    }
    return node;
}

static ASTNode *read_expr()
{
    ASTNode *node = read_assign_expr();
    if (!has_token() || peek_token()->kind != COMMA)
        return node;

    ASTNode *expr = expr_lst_node(new std::vector<ASTNode *>());
    if (node != NULL)
        expr->expr_list->push_back(node);

    while (peek_token()->kind == COMMA)
    {
        expect(COMMA);
        node = read_assign_expr();
        if (node != NULL)
            expr->expr_list->push_back(node);
        ensure_token(expr);
    }
    return expr;
}

static ASTNode *read_expr_stmt()
{
    ensure_token(NULL);
    if (peek_token()->kind == SEMICOLON)
    {
        expect(SEMICOLON);
        return NULL;
    }
    ASTNode *node = read_expr();
    expect(SEMICOLON);
    return node;
}

static ASTNode *read_stmt()
{
    ASTNode *node = NULL;
    if (has_token())
    {
        switch (peek_token()->kind)
        {
        case FOR:
            need_fill_back = true;
            return read_for_stmt();
        case FUNC:
            return read_func_decl();
        case IF:
            need_fill_back = true;
            return read_if_stmt();
        case INPUT:
        case PRINT:
            return read_io_stmt();
        case VAL:
        case VAR:
            return read_decl_stmt();
        case WHILE:
            need_fill_back = true;
            return read_while_stmt();
        case CONTINUE:
            expect(CONTINUE);
            expect(SEMICOLON);
            ensure_in_iter();
            return continue_node();
        case BREAK:
            expect(BREAK);
            expect(SEMICOLON);
            ensure_in_iter();
            return break_node();
        case RETURN:
            expect(RETURN);
            node = return_node(read_expr());
            expect(SEMICOLON);
            ensure_in_func_decl();
            return node;
        default:
            return read_expr_stmt();
        }
    }
    err("unexpected end of file\n");
    return NULL;
}

static ASTNode *read_cpd_stmt()
{
    expect(L_CURLY);
    ASTNode *stmt = NULL;
    ASTNode *cpd_stmt = cpd_stmt_node(new std::vector<ASTNode *>(), cur_table);
    
    ensure_token(cpd_stmt);
    while (peek_token()->kind != R_CURLY)
    {
        bool old_nd_fl_bk = need_fill_back;
        need_fill_back = false;
        if (peek_token()->kind == VAL || peek_token()->kind == VAR)
        {
            stmt = read_decl_stmt();
        }
        else
        {
            stmt = read_stmt();
        }

        if (stmt != NULL)
        {
            cpd_stmt->statements->push_back(stmt);
            if (old_nd_fl_bk && prev_stmt != NULL)
            {
                fill_back(prev_stmt, stmt);
            }

            if (need_fill_back)
            {
                prev_stmt = stmt;
            }
        }
        ensure_token(cpd_stmt);
    }
    expect(R_CURLY);
    return cpd_stmt;
}

static ASTNode *read_for_stmt()
{
    expect(FOR);
    expect(L_PAREN);

    // create new symbal table for "for" loop
    new_table();

    ensure_token(NULL);
    KIND kind = peek_token()->kind;
    ASTNode *init = NULL, *cond = NULL, *incr = NULL, *body = NULL;
    if (kind == VAL || kind == VAR)
        init = read_decl_stmt();
    else
        init = read_expr_stmt();
    cond = read_expr_stmt();

    ensure_token(NULL);
    if (peek_token()->kind != R_PAREN)
    {
        incr = read_expr();
    }
    expect(R_PAREN);
    body = read_cpd_stmt();
    ASTNode *for_stmt = for_stmt_node(init, cond, incr, body, NULL);
    cur_table->top = for_stmt;

    // restore parent table
    restore_table();

    return for_stmt;
}

static ASTNode *read_func_decl()
{
    expect(FUNC);
    Token *token = expect(IDENTIFIER);
    expect(L_PAREN);

    if (cur_table->contains(token->identifier))
    {
        err("line: %ld, duplicated definition of \"%s\"\n", token->ln, token->identifier);
    }

    // In case of recursive function, we must put the id in the table
    // before entering the function body.
    ASTNode *ident = ident_node(token->identifier, false);
    cur_table->put(ident->name, ident);

    // create symbol table for function
    new_table();

    ASTNode *arg_node = lst_val_node(new std::vector<ASTNode *>());

    ensure_token(NULL);
    if (peek_token()->kind != R_PAREN)
    {
        Token *spec = expect2(VAR, VAL);
        char *arg_name = expect(IDENTIFIER)->identifier;
        ASTNode *arg_ident = ident_node(arg_name, spec->kind == VAR);
        arg_node->list_vals->push_back(arg_ident);
        cur_table->put(arg_ident->name, arg_ident);

        ensure_token(NULL);
        while (peek_token()->kind == COMMA)
        {
            expect(COMMA);
            spec = expect2(VAR, VAL);
            arg_name = expect(IDENTIFIER)->identifier;
            arg_ident = ident_node(arg_name, spec->kind == VAR);
            arg_node->list_vals->push_back(arg_ident);
            cur_table->put(arg_ident->name, arg_ident);
            ensure_token(NULL);
        }
    }
    expect(R_PAREN);

    ASTNode *func = func_decl_node(ident, arg_node, NULL);
    cur_table->top = func;
    ASTNode *func_body = read_cpd_stmt();
    if (func_body == NULL)
    {
        ensure_token(NULL);
        err("line: %ld, missing function body\n", peek_token()->ln);
    }
    func->func_body = func_body;
    // restore parent table
    restore_table();

    return func;
}

static ASTNode *read_elif_stmt()
{
    expect(ELIF);
    expect(L_PAREN);

    // create symbol table for elif statement
    new_table();

    ASTNode *cond = read_logic_or_expr();
    expect(R_PAREN);
    ASTNode *body = read_cpd_stmt();
    ASTNode *if_stmt = if_stmt_node(cond, body, NULL);
    cur_table->top = if_stmt;

    // restore parent table
    restore_table();

    return if_stmt;
}

static ASTNode *read_elif_list()
{
    ASTNode *node = NULL;
    std::vector<ASTNode *> *elif_list = new std::vector<ASTNode *>();

    ensure_token(elif_lst_node(elif_list, node));
    while (peek_token()->kind == ELIF)
    {
        node = read_elif_stmt();
        if (node != NULL)
        {
            elif_list->push_back(node);
            node = NULL;
        }
        ensure_token(elif_lst_node(elif_list, node));
    }

    ensure_token(elif_lst_node(elif_list, node));
    if (peek_token()->kind == ELSE)
    {
        expect(ELSE);

        // create new table for else statement
        new_table();

        node = read_cpd_stmt();
        cur_table->top = node;

        // restore parent table
        restore_table();
    }

    return elif_lst_node(elif_list, node);
}

static ASTNode *read_if_stmt()
{
    expect(IF);
    expect(L_PAREN);
    ASTNode *cond = read_logic_or_expr();
    expect(R_PAREN);

    new_table();
    ASTNode *true_stmt = read_cpd_stmt(), *false_stmt = NULL;
    restore_table();

    ensure_token(if_stmt_node(cond, true_stmt, false_stmt));
    if (peek_token()->kind == ELIF)
        false_stmt = read_elif_list();
    else if (peek_token()->kind == ELSE)
    {
        expect(ELSE);

        new_table();
        false_stmt = read_cpd_stmt();
        restore_table();
    }
    return if_stmt_node(cond, true_stmt, false_stmt);
}

static ASTNode *read_io_stmt()
{
    Token *token = expect2(INPUT, PRINT);
    expect(L_PAREN);
    ASTNode *arg_list = read_arg_expr_list();
    expect(R_PAREN);
    expect(SEMICOLON);
    ASTNode *node;
    if (token->kind == INPUT)
        node = input_stmt_node(arg_list->list_vals);
    else
        node = print_stmt_node(arg_list->list_vals);
    delete arg_list;
    return node;
}

static ASTNode *read_initializer_list()
{
    ASTNode *init_list = lst_val_node(new std::vector<ASTNode *>());
    ASTNode *init = read_initializer();
    if (init != NULL)
    {
        init_list->list_vals->push_back(init);
    }

    ensure_token(init_list);
    while (peek_token()->kind == COMMA)
    {
        expect(COMMA);
        init = read_initializer();
        if (init != NULL)
        {
            init_list->list_vals->push_back(init);
        }
        ensure_token(init_list);
    }
    return init_list;
}

static ASTNode *read_initializer()
{
    ensure_token(NULL);
    if (peek_token()->kind == L_BRACK)
    {
        expect(L_BRACK);
        ASTNode *node = read_initializer_list();
        ensure_token(NULL);
        if (peek_token()->kind == COMMA)
            expect(COMMA);
        expect(R_BRACK);
        return node;
    }
    return read_logic_or_expr();
}

static ASTNode *read_init_decl(bool is_mutable)
{
    ASTNode *init = NULL;
    Token *token = expect(IDENTIFIER);
    if (cur_table->contains(token->identifier))
    {
        err("line: %ld, duplicated definition of \"%s\"\n", token->ln, token->identifier);
    }

    ASTNode *ident = ident_node(token->identifier, is_mutable);
    cur_table->put(ident->name, ident);

    ensure_token(decl_node(ident, init));
    if (peek_token()->kind == ASSIGN)
    {
        expect(ASSIGN);
        init = read_initializer();
    }

    if (!is_mutable && init == NULL)
    {
        ensure_token(decl_node(ident, init));
        err("line: %ld, val declaration must have a initializer\n", peek_token()->ln);
    }

    return decl_node(ident, init);
}

static ASTNode *read_decl_stmt()
{
    Token *token = expect2(VAR, VAL);
    ASTNode *decl_lst = decl_lst_node(new std::vector<ASTNode *>());

    bool is_mutable = token->kind == VAR;
    ASTNode *decl = read_init_decl(is_mutable);
    if (decl != NULL)
        decl_lst->decl_list->push_back(decl);

    ensure_token(decl_lst);
    while (peek_token()->kind == COMMA)
    {
        expect(COMMA);
        decl = read_init_decl(is_mutable);
        if (decl != NULL)
            decl_lst->decl_list->push_back(decl);
        ensure_token(decl_lst);
    }
    expect(SEMICOLON);
    return decl_lst;
}

static ASTNode *read_while_stmt()
{
    expect(WHILE);
    expect(L_PAREN);
    new_table();
    ASTNode *cond = read_logic_or_expr();
    expect(R_PAREN);
    ASTNode *body = read_cpd_stmt();
    if (cond == NULL || body == NULL)
        return NULL;
    ASTNode *while_stmt = while_stmt_node(cond, body, NULL);
    cur_table->top = while_stmt;
    restore_table();
    return while_stmt;
}

static ASTNode *read_program()
{
    cur_table = new SymTable<ASTNode *>(NULL);
    ASTNode *program = program_node(new std::vector<ASTNode *>(), cur_table);
    cur_table->top = program;
    while (has_token())
    {
        ASTNode *stmt = NULL;
        bool cur_nd_fl_bk = false;
        switch (peek_token()->kind)
        {
        case FOR:
            stmt = read_for_stmt();
            cur_nd_fl_bk = true;
            break;
        case FUNC:
            stmt = read_func_decl();
            break;
        case IF:
            stmt = read_if_stmt();
            cur_nd_fl_bk = true;
            break;
        case INPUT:
        case PRINT:
            stmt = read_io_stmt();
            break;
        case VAL:
        case VAR:
            stmt = read_decl_stmt();
            break;
        case WHILE:
            stmt = read_while_stmt();
            cur_nd_fl_bk = true;
            break;
        case BREAK:
        case CONTINUE:
            get_token();
            ensure_in_iter();
            break;
        case RETURN:
            get_token();
            ensure_in_func_decl();
            break;
        default:
            stmt = read_expr_stmt();
            break;
        }

        if (stmt != NULL)
        {
            program->statements->push_back(stmt);
            if (need_fill_back && prev_stmt != NULL)
            {
                fill_back(prev_stmt, stmt);
            }
            need_fill_back = cur_nd_fl_bk;
            prev_stmt = stmt;
        }
    }
    return program;
}

void init_parser()
{
    tk_idx = 0;
    tks = NULL;
    need_fill_back = false;
    prev_stmt = NULL;
}

ASTNode *parse(std::vector<Token *> *tokens)
{
    tks = tokens;
    return read_program();
}