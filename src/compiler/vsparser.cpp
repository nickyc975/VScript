#include "compiler.hpp"

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
static ASTNode *read_multTK_expr();
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

static bool is_arith(TOKEN_TYPE opcode)
{
    return opcode >= TK_ADD && opcode <= TK_NEQ;
}

static bool is_cmp(TOKEN_TYPE opcode)
{
    return opcode >= TK_LT && opcode <= TK_NEQ;
}

static bool is_logic(TOKEN_TYPE opcode)
{
    return opcode >= TK_EQ && opcode <= TK_OR;
}

static bool is_assign(TOKEN_TYPE opcode)
{
    switch (opcode)
    {
    case TK_ASSIGN:
    case TK_ADD_ASSIGN:
    case TK_SUB_ASSIGN:
    case TK_MUL_ASSIGN:
    case TK_DIV_ASSIGN:
    case TK_MOD_ASSIGN:
    case TK_AND_ASSIGN:
    case TK_OR_ASSIGN:
        return true;
    default:
        return false;
    }
}

static bool is_relation(TOKEN_TYPE opcode)
{
    switch (opcode)
    {
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
        return true;
    default:
        return false;
    }
}

static bool is_mul(TOKEN_TYPE opcode)
{
    switch (opcode)
    {
    case TK_MUL:
    case TK_DIV:
    case TK_MOD:
        return true;
    default:
        return false;
    }
}

static TOKEN_TYPE get_assign_opcode(TOKEN_TYPE opcode)
{
    switch (opcode)
    {
    case TK_ADD_ASSIGN:
        return TK_ADD;
    case TK_SUB_ASSIGN:
        return TK_SUB;
    case TK_MUL_ASSIGN:
        return TK_MUL;
    case TK_DIV_ASSIGN:
        return TK_DIV;
    case TK_MOD_ASSIGN:
        return TK_MOD;
    case TK_AND_ASSIGN:
        return TK_AND;
    case TK_OR_ASSIGN:
        return TK_OR;
    case TK_ASSIGN:
    default:
        return TK_NOP;
    }
}

static TOKEN_TYPE get_rel_opcode(TOKEN_TYPE opcode)
{
    switch (opcode)
    {
    case TK_LT:
        return TK_LT;
    case TK_LE:
        return TK_LE;
    case TK_GT:
        return TK_GT;
    case TK_GE:
        return TK_GE;
    default:
        return TK_NOP;
    }
}

static TOKEN_TYPE get_mul_opcode(TOKEN_TYPE opcode)
{
    switch (opcode)
    {
    case TK_MUL:
        return TK_MUL;
    case TK_DIV:
        return TK_DIV;
    case TK_MOD:
        return TK_MOD;
    default:
        return TK_NOP;
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

static ASTNode *cal_u_expr(TOKEN_TYPE op, ASTNode *value)
{
    VSValue *old = value->value;
    if (op == TK_SUB && is_num(old->type))
    {
        value->value = VSValue::i_neg(old);
        delete old;
    }
    else if (op == TK_NOT && old->type == BOOL)
    {
        value->value = VSValue::i_not(old);
    }
    else
    {
        unget_token();
        u_op_err_lno(peek_token()->ln, TOKEN_STR[op], TYPE_STR[old->type]);
        get_token();
    }
    return value;
}

static ASTNode *cal_bool_b_expr(TOKEN_TYPE op, ASTNode *left, ASTNode *right)
{
    if ((left->value->type != right->value->type) || !is_logic(op))
    {
        unget_token();
        b_op_err_lno(peek_token()->ln, TOKEN_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        delete left;
        delete right;
        return NULL;
    }

    switch (op)
    {
    case TK_EQ:
        left->value = VSValue::i_eq(left->value, right->value);
        break;
    case TK_NEQ:
        left->value = VSValue::i_neq(left->value, right->value);
        break;
    case TK_AND:
        left->value = VSValue::i_and(left->value, right->value);
        break;
    case TK_OR:
        left->value = VSValue::i_or(left->value, right->value);
        break;
    default:
        err("internal error\n");
        break;
    }
    delete right;
    return left;
}

static ASTNode *cal_num_b_expr(TOKEN_TYPE op, ASTNode *left, ASTNode *right)
{
    if ((left->value->type != right->value->type) || !is_arith(op))
    {
        unget_token();
        b_op_err_lno(peek_token()->ln, TOKEN_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        delete left;
        delete right;
        return NULL;
    }

    VSValue *old = left->value;
    switch (op)
    {
    case TK_ADD:
        left->value = VSValue::i_add(left->value, right->value);
        delete old;
        break;
    case TK_SUB:
        left->value = VSValue::i_sub(left->value, right->value);
        delete old;
        break;
    case TK_MUL:
        left->value = VSValue::i_mul(left->value, right->value);
        delete old;
        break;
    case TK_DIV:
        left->value = VSValue::i_div(left->value, right->value);
        delete old;
        break;
    case TK_MOD:
        if (left->value->type == FLOAT)
        {
            unget_token();
            b_op_err_lno(peek_token()->ln, TOKEN_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
            get_token();
            delete left;
            delete right;
            return NULL;
        }
        left->value = VSValue::i_mod(left->value, right->value);
        delete old;
        break;
    case TK_LT:
        left->value = VSValue::i_lt(left->value, right->value);
        delete old;
        break;
    case TK_GT:
        left->value = VSValue::i_gt(left->value, right->value);
        delete old;
        break;
    case TK_LE:
        left->value = VSValue::i_le(left->value, right->value);
        delete old;
        break;
    case TK_GE:
        left->value = VSValue::i_ge(left->value, right->value);
        delete old;
        break;
    case TK_EQ:
        left->value = VSValue::i_eq(left->value, right->value);
        delete old;
        break;
    case TK_NEQ:
        left->value = VSValue::i_neq(left->value, right->value);
        delete old;
        break;
    default:
        err("internal error\n");
        break;
    }
    delete right;
    return left;
}

static ASTNode *cal_str_b_expr(TOKEN_TYPE op, ASTNode *left, ASTNode *right)
{
    if ((left->value->type != right->value->type) || !(op == TK_ADD || is_cmp(op)))
    {
        unget_token();
        b_op_err_lno(peek_token()->ln, TOKEN_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        delete left;
        delete right;
        return NULL;
    }

    VSValue *old = left->value;
    switch (op)
    {
    case TK_ADD:
        left->value = VSValue::i_add(left->value, right->value);
        delete old;
        break;
    case TK_LT:
        left->value = VSValue::i_lt(left->value, right->value);
        delete old;
        break;
    case TK_GT:
        left->value = VSValue::i_gt(left->value, right->value);
        delete old;
        break;
    case TK_LE:
        left->value = VSValue::i_le(left->value, right->value);
        delete old;
        break;
    case TK_GE:
        left->value = VSValue::i_ge(left->value, right->value);
        delete old;
        break;
    case TK_EQ:
        left->value = VSValue::i_eq(left->value, right->value);
        delete old;
        break;
    case TK_NEQ:
        left->value = VSValue::i_neq(left->value, right->value);
        delete old;
        break;
    default:
        err("internal error\n");
        break;
    }
    return left;
}

static ASTNode *cal_b_expr(TOKEN_TYPE op, ASTNode *left, ASTNode *right)
{
    switch (left->value->type)
    {
    case BOOL:
        return cal_bool_b_expr(op, left, right);
    case CHAR:
    case INT:
    case FLOAT:
        return cal_num_b_expr(op, left, right);
    case STRING:
        return cal_str_b_expr(op, left, right);
    default:
        unget_token();
        b_op_err_lno(peek_token()->ln, TOKEN_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        return NULL;
    }
}

static ASTNode *ident_node(std::string *name, bool is_mutable)
{
    ASTNode *node = new ASTNode(AST_IDENT, AST_EXPR);
    node->name = new std::string(*name);
    node->is_mutable = is_mutable;
    return node;
}

static ASTNode *const_node(VSValue *value)
{
    ASTNode *node = new ASTNode(AST_CONST, AST_EXPR);
    node->value = value;
    return node;
}

static ASTNode *b_expr_node(TOKEN_TYPE b_opcode, ASTNode *l_operand, ASTNode *r_operand)
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

static ASTNode *u_expr_node(TOKEN_TYPE u_opcode, ASTNode *operand)
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

static ASTNode *assign_node(TOKEN_TYPE assign_opcode, ASTNode *assign_var, ASTNode *assign_val)
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

static Token *expect(TOKEN_TYPE type)
{
    ensure_token(NULL);
    Token *token = get_token();
    if (token->type != type)
    {
        err("line: %ld, expected \"%s\" but got \"%s\"\n", token->ln, TOKEN_STR[type], TOKEN_STR[token->type]);
    }
    return token;
}

static Token *expect2(TOKEN_TYPE type1, TOKEN_TYPE type2)
{
    ensure_token(NULL);
    Token *token = get_token();
    if (token->type != type1 && token->type != type2)
    {
        err("line: %ld, expected \"%s\" or \"%s\" but got \"%s\"\n", token->ln, TOKEN_STR[type1], TOKEN_STR[type2], TOKEN_STR[token->type]);
    }
    return token;
}

static ASTNode *read_primary_expr()
{
    Token *token;
    ASTNode *node = NULL;

    ensure_token(NULL);
    switch (peek_token()->type)
    {
    case TK_CONSTANT:
        node = const_node(get_token()->value);
        break;
    case TK_IDENTIFIER:
        token = get_token();
        if (!cur_table->contains_recur(*token->identifier))
        {
            err("line %ld, \"%s\" not defined\n", token->ln, token->identifier);
            break;
        }
        node = cur_table->get_recur(*token->identifier);
        break;
    case TK_L_PAREN:
        expect(TK_L_PAREN);
        node = read_expr();
        expect(TK_R_PAREN);
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
    if (peek_token()->type == TK_R_PAREN)
        return arg_list;
    ASTNode *arg = read_logic_or_expr();
    if (arg == NULL)
    {
        ensure_token(arg_list);
        err("line: %ld, missing args\n", peek_token()->ln);
    }
    arg_list->list_vals->push_back(arg);
    ensure_token(arg_list);
    while (peek_token()->type == TK_COMMA)
    {
        expect(TK_COMMA);
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
    
    switch (peek_token()->type)
    {
    case TK_L_BRACK:
    {
        expect(TK_L_BRACK);
        ASTNode *lst_idx = read_expr();
        if (lst_idx == NULL)
        {
            ensure_token(node);
            err("line: %ld, invalid list index\n", peek_token()->ln);
        }
        node = lst_idx_node(node, lst_idx);
        expect(TK_R_BRACK);
        break;
    }
    case TK_L_PAREN:
    {
        expect(TK_L_PAREN);
        ASTNode *arg_list = read_arg_expr_list();
        if (arg_list == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing args\n", peek_token()->ln);
        }
        node = func_call_node(node, arg_list);
        expect(TK_R_PAREN);
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
    if (peek_token()->type == TK_SUB || peek_token()->type == TK_NOT)
    {
        token = get_token();
    }
    ASTNode *node = read_postfix_expr();
    if (node == NULL || !has_token())
        return node;
    if (token != NULL)
    {
        node = u_expr_node(token->type == TK_SUB ? TK_SUB : TK_NOT, node);
    }
    return node;
}

static ASTNode *read_multTK_expr()
{
    Token *token;
    ASTNode *node = read_unary_expr();
    if (node == NULL || !has_token())
        return node;
    while (is_mul(peek_token()->type))
    {
        token = get_token();
        ASTNode *right = read_unary_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(get_mul_opcode(token->type), node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_additive_expr()
{
    Token *token;
    ASTNode *node = read_multTK_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->type == TK_ADD || peek_token()->type == TK_SUB)
    {
        token = get_token();
        ASTNode *right = read_multTK_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(token->type == TK_ADD ? TK_ADD : TK_SUB, node, right);
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
    while (is_relation(peek_token()->type))
    {
        token = get_token();
        ASTNode *right = read_additive_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(get_rel_opcode(token->type), node, right);
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
    while (peek_token()->type == TK_EQ || peek_token()->type == TK_NEQ)
    {
        token = get_token();
        ASTNode *right = read_relational_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(token->type == TK_EQ ? TK_EQ : TK_NEQ, node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_logic_and_expr()
{
    ASTNode *node = read_equality_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->type == TK_AND)
    {
        expect(TK_AND);
        ASTNode *right = read_equality_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(TK_AND, node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_logic_or_expr()
{
    ASTNode *node = read_logic_and_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->type == TK_OR)
    {
        expect(TK_OR);
        ASTNode *right = read_logic_and_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(TK_OR, node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_assign_expr()
{
    ASTNode *node = read_logic_or_expr();
    if (node == NULL || !has_token())
        return node;
    if (is_assign(peek_token()->type))
    {
        ensure_lval(node);
        TOKEN_TYPE assign_opcode = get_assign_opcode(get_token()->type);
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
    if (!has_token() || peek_token()->type != TK_COMMA)
        return node;

    ASTNode *expr = expr_lst_node(new std::vector<ASTNode *>());
    if (node != NULL)
        expr->expr_list->push_back(node);

    while (peek_token()->type == TK_COMMA)
    {
        expect(TK_COMMA);
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
    if (peek_token()->type == TK_SEMICOLON)
    {
        expect(TK_SEMICOLON);
        return NULL;
    }
    ASTNode *node = read_expr();
    expect(TK_SEMICOLON);
    return node;
}

static ASTNode *read_stmt()
{
    ASTNode *node = NULL;
    if (has_token())
    {
        switch (peek_token()->type)
        {
        case TK_FOR:
            need_fill_back = true;
            return read_for_stmt();
        case TK_FUNC:
            return read_func_decl();
        case TK_IF:
            need_fill_back = true;
            return read_if_stmt();
        case TK_INPUT:
        case TK_PRINT:
            return read_io_stmt();
        case TK_VAL:
        case TK_VAR:
            return read_decl_stmt();
        case TK_WHILE:
            need_fill_back = true;
            return read_while_stmt();
        case TK_CONTINUE:
            expect(TK_CONTINUE);
            expect(TK_SEMICOLON);
            ensure_in_iter();
            return continue_node();
        case TK_BREAK:
            expect(TK_BREAK);
            expect(TK_SEMICOLON);
            ensure_in_iter();
            return break_node();
        case TK_RETURN:
            expect(TK_RETURN);
            node = return_node(read_expr());
            expect(TK_SEMICOLON);
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
    expect(TK_L_CURLY);
    ASTNode *stmt = NULL;
    ASTNode *cpd_stmt = cpd_stmt_node(new std::vector<ASTNode *>(), cur_table);
    
    ensure_token(cpd_stmt);
    while (peek_token()->type != TK_R_CURLY)
    {
        bool old_nd_fl_bk = need_fill_back;
        need_fill_back = false;
        if (peek_token()->type == TK_VAL || peek_token()->type == TK_VAR)
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
    expect(TK_R_CURLY);
    return cpd_stmt;
}

static ASTNode *read_for_stmt()
{
    expect(TK_FOR);
    expect(TK_L_PAREN);

    // create new symbal table for "for" loop
    new_table();

    ensure_token(NULL);
    TOKEN_TYPE type = peek_token()->type;
    ASTNode *init = NULL, *cond = NULL, *incr = NULL, *body = NULL;
    if (type == TK_VAL || type == TK_VAR)
        init = read_decl_stmt();
    else
        init = read_expr_stmt();
    cond = read_expr_stmt();

    ensure_token(NULL);
    if (peek_token()->type != TK_R_PAREN)
    {
        incr = read_expr();
    }
    expect(TK_R_PAREN);
    body = read_cpd_stmt();
    ASTNode *for_stmt = for_stmt_node(init, cond, incr, body, NULL);
    cur_table->top = for_stmt;

    // restore parent table
    restore_table();

    return for_stmt;
}

static ASTNode *read_func_decl()
{
    expect(TK_FUNC);
    Token *token = expect(TK_IDENTIFIER);
    expect(TK_L_PAREN);

    if (cur_table->contains(*token->identifier))
    {
        err("line: %ld, duplicated definition of \"%s\"\n", token->ln, token->identifier);
    }

    // In case of recursive function, we must put the id in the table
    // before entering the function body.
    ASTNode *ident = ident_node(token->identifier, false);
    cur_table->put(*ident->name, ident);

    // create symbol table for function
    new_table();

    ASTNode *arg_node = lst_val_node(new std::vector<ASTNode *>());

    ensure_token(NULL);
    if (peek_token()->type != TK_R_PAREN)
    {
        std::string *arg_name = expect(TK_IDENTIFIER)->identifier;
        ASTNode *arg_ident = ident_node(arg_name, true);
        arg_node->list_vals->push_back(arg_ident);
        cur_table->put(*arg_name, arg_ident);

        ensure_token(NULL);
        while (peek_token()->type == TK_COMMA)
        {
            expect(TK_COMMA);
            arg_name = expect(TK_IDENTIFIER)->identifier;
            arg_ident = ident_node(arg_name, true);
            arg_node->list_vals->push_back(arg_ident);
            cur_table->put(*arg_name, arg_ident);
            ensure_token(NULL);
        }
    }
    expect(TK_R_PAREN);

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
    expect(TK_ELIF);
    expect(TK_L_PAREN);

    // create symbol table for elif statement
    new_table();

    ASTNode *cond = read_logic_or_expr();
    expect(TK_R_PAREN);
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
    while (peek_token()->type == TK_ELIF)
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
    if (peek_token()->type == TK_ELSE)
    {
        expect(TK_ELSE);

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
    expect(TK_IF);
    expect(TK_L_PAREN);
    ASTNode *cond = read_logic_or_expr();
    expect(TK_R_PAREN);

    new_table();
    ASTNode *true_stmt = read_cpd_stmt(), *false_stmt = NULL;
    restore_table();

    ensure_token(if_stmt_node(cond, true_stmt, false_stmt));
    if (peek_token()->type == TK_ELIF)
        false_stmt = read_elif_list();
    else if (peek_token()->type == TK_ELSE)
    {
        expect(TK_ELSE);

        new_table();
        false_stmt = read_cpd_stmt();
        restore_table();
    }
    return if_stmt_node(cond, true_stmt, false_stmt);
}

static ASTNode *read_io_stmt()
{
    Token *token = expect2(TK_INPUT, TK_PRINT);
    expect(TK_L_PAREN);
    ASTNode *arg_list = read_arg_expr_list();
    expect(TK_R_PAREN);
    expect(TK_SEMICOLON);
    ASTNode *node;
    if (token->type == TK_INPUT)
    {
        for (auto arg : *arg_list->list_vals)
        {
            ensure_lval(arg);
        }
        node = input_stmt_node(arg_list->list_vals);
    }
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
    while (peek_token()->type == TK_COMMA)
    {
        expect(TK_COMMA);
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
    if (peek_token()->type == TK_L_BRACK)
    {
        expect(TK_L_BRACK);
        ASTNode *node = read_initializer_list();
        ensure_token(NULL);
        if (peek_token()->type == TK_COMMA)
            expect(TK_COMMA);
        expect(TK_R_BRACK);
        return node;
    }
    return read_logic_or_expr();
}

static ASTNode *read_init_decl(bool is_mutable)
{
    ASTNode *init = NULL;
    Token *token = expect(TK_IDENTIFIER);
    if (cur_table->contains(*token->identifier))
    {
        err("line: %ld, duplicated definition of \"%s\"\n", token->ln, token->identifier);
    }

    ASTNode *ident = ident_node(token->identifier, is_mutable);
    cur_table->put(*ident->name, ident);

    ensure_token(decl_node(ident, init));
    if (peek_token()->type == TK_ASSIGN)
    {
        expect(TK_ASSIGN);
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
    Token *token = expect2(TK_VAR, TK_VAL);
    ASTNode *decl_lst = decl_lst_node(new std::vector<ASTNode *>());

    bool is_mutable = token->type == TK_VAR;
    ASTNode *decl = read_init_decl(is_mutable);
    if (decl != NULL)
        decl_lst->decl_list->push_back(decl);

    ensure_token(decl_lst);
    while (peek_token()->type == TK_COMMA)
    {
        expect(TK_COMMA);
        decl = read_init_decl(is_mutable);
        if (decl != NULL)
            decl_lst->decl_list->push_back(decl);
        ensure_token(decl_lst);
    }
    expect(TK_SEMICOLON);
    return decl_lst;
}

static ASTNode *read_while_stmt()
{
    expect(TK_WHILE);
    expect(TK_L_PAREN);
    new_table();
    ASTNode *cond = read_logic_or_expr();
    expect(TK_R_PAREN);
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
        switch (peek_token()->type)
        {
        case TK_FOR:
            stmt = read_for_stmt();
            cur_nd_fl_bk = true;
            break;
        case TK_FUNC:
            stmt = read_func_decl();
            break;
        case TK_IF:
            stmt = read_if_stmt();
            cur_nd_fl_bk = true;
            break;
        case TK_INPUT:
        case TK_PRINT:
            stmt = read_io_stmt();
            break;
        case TK_VAL:
        case TK_VAR:
            stmt = read_decl_stmt();
            break;
        case TK_WHILE:
            stmt = read_while_stmt();
            cur_nd_fl_bk = true;
            break;
        case TK_BREAK:
        case TK_CONTINUE:
            get_token();
            ensure_in_iter();
            break;
        case TK_RETURN:
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

ASTNode *parse(std::vector<Token *> *tokens)
{
    tk_idx = 0;
    tks = tokens;
    need_fill_back = false;
    prev_stmt = NULL;
    return read_program();
}