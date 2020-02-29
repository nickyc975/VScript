#include <stack>

#include "compiler.hpp"

static SymTable<ASTNode *> *cur_table, *global_table;

// Create new table for compound statements.
#define new_table() cur_table = new SymTable<ASTNode *>(cur_table)

// Restore parent table.
#define restore_table() cur_table = cur_table->get_parent()

static unsigned int inloop = 0, infunc = 0;
static std::stack<unsigned int> inloop_stack;

// Enter loop body
#define enter_loop() inloop++

// Check if in loop
#define ensure_in_loop()                                                                         \
    if (!inloop)                                                                                 \
    {                                                                                            \
        err("line %ld, continue or break must be in while or for statement\n", get_token()->ln); \
    }

// Leave loop body
#define leave_loop() \
    if (inloop > 0)  \
    {                \
        inloop--;    \
    }

// Enter func decl
#define enter_func()           \
    inloop_stack.push(inloop); \
    inloop = 0;                \
    infunc++;

// Check if in func decl
#define ensure_in_func()                                                            \
    if (!infunc)                                                                    \
    {                                                                               \
        err("line %ld, return must be in function declaration\n", get_token()->ln); \
    }

// Leave func decl
#define leave_func()             \
    inloop = inloop_stack.top(); \
    inloop_stack.pop();          \
    if (infunc > 0)              \
    {                            \
        infunc--;                \
    }

// Ensure that there are tokens.
#define ensure_token(ret_val)            \
    if (!has_token())                    \
    {                                    \
        err("unexpected end of file\n"); \
        return ret_val;                  \
    }

static unsigned int tk_idx;
static std::vector<Token *> *tks;

static ASTNode *read_list_val();
static ASTNode *read_primary_expr();
static ASTNode *read_postfix_expr();
static ASTNode *read_unary_expr();
static ASTNode *read_mul_expr();
static ASTNode *read_add_expr();
static ASTNode *read_rel_expr();
static ASTNode *read_eql_expr();
static ASTNode *read_log_and_expr();
static ASTNode *read_log_or_expr();
static ASTNode *read_assign_expr();
static ASTNode *read_expr_list();
static ASTNode *read_expr_stmt();
static ASTNode *read_init_decl(bool is_mutable);
static ASTNode *read_init_decl_stmt();
static ASTNode *read_func_decl();
static ASTNode *read_elif();
static ASTNode *read_elif_list();
static ASTNode *read_if_stmt();
static ASTNode *read_while_stmt();
static ASTNode *read_for_stmt();
static ASTNode *read_stmt();
static ASTNode *read_cpd_stmt();
static ASTNode *read_program();

static bool has_token()
{
    return tk_idx < tks->size();
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

static bool is_rel(TOKEN_TYPE opcode)
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

static bool ensure_lval(ASTNode *node)
{
    switch (node->type)
    {
    case AST_IDENT:
        if (!node->is_mutable)
        {
            ensure_token(false);
            err("line: %ld, \"%s\" is immutable\n", peek_token()->ln, node->name);
            return false;
        }
        break;
    case AST_LIST_IDX:
        break;
    default:
        ensure_token(false);
        err("line: %ld, illegal left value\n", peek_token()->ln);
        return false;
    }
    return true;
}

static ASTNode *cal_u_expr(TOKEN_TYPE op, ASTNode *value)
{
    VSValue *old = value->value;
    if (op == TK_SUB && is_num(old->type))
    {
        value->value = VSValue::i_neg(old);
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
    return left;
}

static ASTNode *cal_num_b_expr(TOKEN_TYPE op, ASTNode *left, ASTNode *right)
{
    if ((left->value->type != right->value->type) || !is_arith(op))
    {
        unget_token();
        b_op_err_lno(peek_token()->ln, TOKEN_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        return NULL;
    }

    switch (op)
    {
    case TK_ADD:
        left->value = VSValue::i_add(left->value, right->value);
        break;
    case TK_SUB:
        left->value = VSValue::i_sub(left->value, right->value);
        break;
    case TK_MUL:
        left->value = VSValue::i_mul(left->value, right->value);
        break;
    case TK_DIV:
        left->value = VSValue::i_div(left->value, right->value);
        break;
    case TK_MOD:
        if (left->value->type == FLOAT)
        {
            unget_token();
            b_op_err_lno(peek_token()->ln, TOKEN_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
            get_token();
            return NULL;
        }
        left->value = VSValue::i_mod(left->value, right->value);
        break;
    case TK_LT:
        left->value = VSValue::i_lt(left->value, right->value);
        break;
    case TK_GT:
        left->value = VSValue::i_gt(left->value, right->value);
        break;
    case TK_LE:
        left->value = VSValue::i_le(left->value, right->value);
        break;
    case TK_GE:
        left->value = VSValue::i_ge(left->value, right->value);
        break;
    case TK_EQ:
        left->value = VSValue::i_eq(left->value, right->value);
        break;
    case TK_NEQ:
        left->value = VSValue::i_neq(left->value, right->value);
        break;
    default:
        err("internal error\n");
        break;
    }
    return left;
}

static ASTNode *cal_str_b_expr(TOKEN_TYPE op, ASTNode *left, ASTNode *right)
{
    if ((left->value->type != right->value->type) || !(op == TK_ADD || is_cmp(op)))
    {
        unget_token();
        b_op_err_lno(peek_token()->ln, TOKEN_STR[op], TYPE_STR[left->value->type], TYPE_STR[right->value->type]);
        get_token();
        return NULL;
    }

    switch (op)
    {
    case TK_ADD:
        left->value = VSValue::i_add(left->value, right->value);
        break;
    case TK_LT:
        left->value = VSValue::i_lt(left->value, right->value);
        break;
    case TK_GT:
        left->value = VSValue::i_gt(left->value, right->value);
        break;
    case TK_LE:
        left->value = VSValue::i_le(left->value, right->value);
        break;
    case TK_GE:
        left->value = VSValue::i_ge(left->value, right->value);
        break;
    case TK_EQ:
        left->value = VSValue::i_eq(left->value, right->value);
        break;
    case TK_NEQ:
        left->value = VSValue::i_neq(left->value, right->value);
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

static ASTNode *ident_node(const std::string *name, bool is_mutable)
{
    ASTNode *node = new ASTNode(AST_IDENT);
    node->name = new std::string(*name);
    node->is_mutable = is_mutable;
    return node;
}

static ASTNode *const_node(VSValue *value)
{
    ASTNode *node = new ASTNode(AST_CONST);
    node->value = value;
    return node;
}

static ASTNode *list_val_node(std::vector<ASTNode *> *list_val)
{
    ASTNode *node = new ASTNode(AST_LIST_VAL);
    node->list_val = list_val;
    return node;
}

static ASTNode *b_expr_node(TOKEN_TYPE b_opcode, ASTNode *l_operand, ASTNode *r_operand)
{
    ASTNode *node;
    if (l_operand->type == AST_CONST && r_operand->type == AST_CONST)
    {
        node = cal_b_expr(b_opcode, l_operand, r_operand);
    }
    else
    {
        node = new ASTNode(AST_B_EXPR);
        node->b_opcode = b_opcode;
        node->l_operand = l_operand;
        node->r_operand = r_operand;
    }
    return node;
}

static ASTNode *u_expr_node(TOKEN_TYPE u_opcode, ASTNode *operand)
{
    ASTNode *node;
    if (operand->type == AST_CONST)
        node = cal_u_expr(u_opcode, operand);
    else
    {
        node = new ASTNode(AST_U_EXPR);
        node->b_opcode = u_opcode;
        node->operand = operand;
    }
    return node;
}

static ASTNode *lst_idx_node(ASTNode *list_name, ASTNode *list_index)
{
    ASTNode *node = new ASTNode(AST_LIST_IDX);
    node->list_name = list_name;
    node->list_index = list_index;
    return node;
}

static ASTNode *func_call_node(ASTNode *func_name, ASTNode *arg_list)
{
    ASTNode *node = new ASTNode(AST_FUNC_CALL);
    node->func = func_name;
    node->arg_list = arg_list;
    return node;
}

static ASTNode *assign_expr_node(TOKEN_TYPE assign_opcode, ASTNode *assign_var, ASTNode *assign_val)
{
    ASTNode *node = new ASTNode(AST_ASSIGN_EXPR);
    node->assign_opcode = assign_opcode;
    node->assign_var = assign_var;
    node->assign_val = assign_val;
    return node;
}

static ASTNode *expr_list_node(std::vector<ASTNode *> *expr_list)
{
    ASTNode *node = new ASTNode(AST_EXPR_LST);
    node->expr_list = expr_list;
    return node;
}

static ASTNode *init_decl_node(ASTNode *var_name, ASTNode *init_val)
{
    ASTNode *node = new ASTNode(AST_INIT_DECL);
    node->var_name = var_name;
    node->init_val = init_val;
    return node;
}

static ASTNode *init_decl_list_node(std::vector<ASTNode *> *decl_list)
{
    ASTNode *node = new ASTNode(AST_INIT_DECL_LIST);
    node->decl_list = decl_list;
    return node;
}

static ASTNode *func_decl_node(ASTNode *func_name, std::vector<ASTNode *> *func_params)
{
    ASTNode *node = new ASTNode(AST_FUNC_DECL);
    node->func_name = func_name;
    node->func_params = func_params;
    node->func_body = NULL;
    return node;
}

static ASTNode *if_stmt_node(ASTNode *if_cond, ASTNode *true_stmt, ASTNode *false_stmt)
{
    ASTNode *node = new ASTNode(AST_IF_STMT);
    node->if_cond = if_cond;
    node->true_smt = true_stmt;
    node->false_smt = false_stmt;
    return node;
}

static ASTNode *elif_lst_node(std::vector<ASTNode *> *elif_list, ASTNode *else_node)
{
    ASTNode *node = new ASTNode(AST_ELIF_LIST);
    node->elif_list = elif_list;
    node->else_node = else_node;
    return node;
}

static ASTNode *while_stmt_node(ASTNode *while_cond, ASTNode *while_body)
{
    ASTNode *node = new ASTNode(AST_WHILE_STMT);
    node->while_cond = while_cond;
    node->while_body = while_body;
    return node;
}

static ASTNode *for_stmt_node(ASTNode *for_init, ASTNode *for_cond, ASTNode *for_incr, ASTNode *for_body)
{
    ASTNode *node = new ASTNode(AST_FOR_STMT);
    node->for_init = for_init;
    node->for_cond = for_cond;
    node->for_incr = for_incr;
    node->for_body = for_body;
    return node;
}

static ASTNode *continue_node()
{
    ASTNode *node = new ASTNode(AST_CONTINUE);
    return node;
}

static ASTNode *break_node()
{
    ASTNode *node = new ASTNode(AST_BREAK);
    return node;
}

static ASTNode *return_node(ASTNode *ret_val)
{
    ASTNode *node = new ASTNode(AST_RETURN);
    node->ret_val = ret_val;
    return node;
}

static ASTNode *cpd_stmt_node(std::vector<ASTNode *> *stmts, SymTable<ASTNode *> *symtable)
{
    ASTNode *node = new ASTNode(AST_CPD_STMT);
    node->statements = stmts;
    node->symtable = symtable;
    return node;
}

static ASTNode *program_node(std::vector<ASTNode *> *stmts, SymTable<ASTNode *> *symtable)
{
    ASTNode *node = new ASTNode(AST_PROGRAM);
    node->statements = stmts;
    node->symtable = symtable;
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

static ASTNode *read_list_val()
{
    ASTNode *list_val = list_val_node(new std::vector<ASTNode *>());

    ASTNode *node = read_log_or_expr();
    if (node != NULL)
        list_val->list_val->push_back(node);

    while (peek_token()->type == TK_COMMA)
    {
        expect(TK_COMMA);
        node = read_log_or_expr();
        if (node != NULL)
            list_val->list_val->push_back(node);
        ensure_token(list_val);
    }
    return list_val;
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
        if (global_table->contains(*token->identifier))
        {
            node = global_table->get(*token->identifier);
        }
        else if (cur_table->contains_recur(*token->identifier))
        {
            node = cur_table->get_recur(*token->identifier);
        }
        else
        {
            err("line %ld, \"%s\" not defined\n", token->ln, token->identifier->c_str());
        }
        break;
    case TK_L_PAREN:
        expect(TK_L_PAREN);
        node = read_log_or_expr();
        expect(TK_R_PAREN);
        break;
    case TK_L_BRACK:
        expect(TK_L_BRACK);
        node = read_list_val();
        expect(TK_R_BRACK);
        break;
    default:
        break;
    }
    return node;
}

static ASTNode *read_postfix_expr()
{
    ASTNode *node = read_primary_expr();
    if (node == NULL || !has_token())
        return node;

    Token *token = peek_token();
    while (token->type == TK_L_BRACK || token->type == TK_L_PAREN)
    {
        if (token->type == TK_L_BRACK)
        {
            expect(TK_L_BRACK);
            ASTNode *lst_idx = read_log_or_expr();
            if (lst_idx == NULL)
            {
                ensure_token(node);
                err("line: %ld, invalid list index\n", peek_token()->ln);
            }
            node = lst_idx_node(node, lst_idx);
            expect(TK_R_BRACK);
        }
        else
        {
            expect(TK_L_PAREN);
            ASTNode *arg_list = read_list_val();
            if (arg_list == NULL)
            {
                ensure_token(node);
                err("line: %ld, missing args\n", peek_token()->ln);
            }
            node = func_call_node(node, arg_list);
            expect(TK_R_PAREN);
        }
        ensure_token(node);
        token = peek_token();
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

static ASTNode *read_mul_expr()
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
        node = b_expr_node(token->type, node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_add_expr()
{
    Token *token;
    ASTNode *node = read_mul_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->type == TK_ADD || peek_token()->type == TK_SUB)
    {
        token = get_token();
        ASTNode *right = read_mul_expr();
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

static ASTNode *read_rel_expr()
{
    Token *token;
    ASTNode *node = read_add_expr();
    if (node == NULL || !has_token())
        return node;
    while (is_rel(peek_token()->type))
    {
        token = get_token();
        ASTNode *right = read_add_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(token->type, node, right);
        ensure_token(node);
    }
    return node;
}

static ASTNode *read_eql_expr()
{
    Token *token;
    ASTNode *node = read_rel_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->type == TK_EQ || peek_token()->type == TK_NEQ)
    {
        token = get_token();
        ASTNode *right = read_rel_expr();
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

static ASTNode *read_log_and_expr()
{
    ASTNode *node = read_eql_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->type == TK_AND)
    {
        expect(TK_AND);
        ASTNode *right = read_eql_expr();
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

static ASTNode *read_log_or_expr()
{
    ASTNode *node = read_log_and_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->type == TK_OR)
    {
        expect(TK_OR);
        ASTNode *right = read_log_and_expr();
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
    ASTNode *node = read_log_or_expr();
    if (node == NULL || !has_token())
        return node;
    if (is_assign(peek_token()->type))
    {
        ensure_lval(node);
        TOKEN_TYPE assign_opcode = get_token()->type;
        if (assign_opcode == TK_ASSIGN)
            assign_opcode = TK_NOP;
        else
            assign_opcode = (TOKEN_TYPE)((int)assign_opcode - 1);
        ASTNode *right = read_log_or_expr();
        if (right == NULL)
        {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = assign_expr_node(assign_opcode, node, right);
    }
    return node;
}

static ASTNode *read_expr_list()
{
    ASTNode *node = read_assign_expr();
    if (!has_token() || peek_token()->type != TK_COMMA)
        return node;

    ASTNode *expr_list = expr_list_node(new std::vector<ASTNode *>());
    if (node != NULL)
        expr_list->expr_list->push_back(node);

    while (peek_token()->type == TK_COMMA)
    {
        expect(TK_COMMA);
        node = read_assign_expr();
        if (node != NULL)
            expr_list->expr_list->push_back(node);
        ensure_token(expr_list);
    }
    return expr_list;
}

static ASTNode *read_expr_stmt()
{
    ensure_token(NULL);
    if (peek_token()->type == TK_SEMICOLON)
    {
        expect(TK_SEMICOLON);
        return NULL;
    }
    ASTNode *node = read_expr_list();
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
            return read_for_stmt();
        case TK_FUNC:
            return read_func_decl();
        case TK_IF:
            return read_if_stmt();
        case TK_VAL:
        case TK_VAR:
            return read_init_decl_stmt();
        case TK_WHILE:
            return read_while_stmt();
        case TK_CONTINUE:
            expect(TK_CONTINUE);
            expect(TK_SEMICOLON);
            ensure_in_loop();
            return continue_node();
        case TK_BREAK:
            expect(TK_BREAK);
            expect(TK_SEMICOLON);
            ensure_in_loop();
            return break_node();
        case TK_RETURN:
            expect(TK_RETURN);
            node = return_node(read_log_or_expr());
            expect(TK_SEMICOLON);
            ensure_in_func();
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
        if (peek_token()->type == TK_VAL || peek_token()->type == TK_VAR)
        {
            stmt = read_init_decl_stmt();
        }
        else
        {
            stmt = read_stmt();
        }

        if (stmt != NULL)
        {
            cpd_stmt->statements->push_back(stmt);
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

    enter_loop();
    // create new symbal table for "for" loop
    new_table();

    ensure_token(NULL);
    TOKEN_TYPE type = peek_token()->type;
    ASTNode *init = NULL, *cond = NULL, *incr = NULL;

    // read for init
    if (type == TK_VAL || type == TK_VAR)
        init = read_init_decl_stmt();
    else
        init = read_expr_stmt();

    // read for cond
    cond = read_log_or_expr();
    expect(TK_SEMICOLON);

    // read for incr
    ensure_token(NULL);
    if (peek_token()->type != TK_R_PAREN)
    {
        incr = read_expr_list();
    }
    expect(TK_R_PAREN);

    ASTNode *for_stmt = for_stmt_node(init, cond, incr, read_cpd_stmt());

    // restore parent table
    restore_table();
    leave_loop();

    return for_stmt;
}

static ASTNode *read_func_decl()
{
    expect(TK_FUNC);
    Token *token = expect(TK_IDENTIFIER);
    expect(TK_L_PAREN);

    if (cur_table->contains(*token->identifier) || global_table->contains(*token->identifier))
    {
        err("line: %ld, duplicated definition of \"%s\"\n", token->ln, token->identifier->c_str());
    }

    // In case of recursive function, we must put the id in the table
    // before entering the function body.
    ASTNode *ident = ident_node(token->identifier, false);
    cur_table->put(*ident->name, ident);

    enter_func();
    // create symbol table for function
    new_table();

    std::vector<ASTNode *> *func_params = new std::vector<ASTNode *>();

    // read func args
    ensure_token(NULL);
    if (peek_token()->type != TK_R_PAREN)
    {
        std::string *arg_name = expect(TK_IDENTIFIER)->identifier;
        ASTNode *arg_ident = ident_node(arg_name, true);
        func_params->push_back(arg_ident);
        cur_table->put(*arg_name, arg_ident);

        ensure_token(NULL);
        while (peek_token()->type == TK_COMMA)
        {
            expect(TK_COMMA);
            arg_name = expect(TK_IDENTIFIER)->identifier;
            arg_ident = ident_node(arg_name, true);
            func_params->push_back(arg_ident);
            cur_table->put(*arg_name, arg_ident);
            ensure_token(NULL);
        }
    }
    expect(TK_R_PAREN);

    ASTNode *func = func_decl_node(ident, func_params);

    // read func body
    ASTNode *func_body = read_cpd_stmt();
    if (func_body == NULL)
    {
        ensure_token(NULL);
        err("line: %ld, missing function body\n", peek_token()->ln);
    }
    func->func_body = func_body;

    // restore parent table
    restore_table();
    leave_func();

    return func;
}

static ASTNode *read_elif_stmt()
{
    expect(TK_ELIF);
    expect(TK_L_PAREN);

    // create symbol table for elif statement
    new_table();

    // read elif cond
    ASTNode *cond = read_log_or_expr();

    expect(TK_R_PAREN);

    // read elif body
    ASTNode *body = read_cpd_stmt();

    // restore parent table
    restore_table();

    return if_stmt_node(cond, body, NULL);
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

        // read else body
        node = read_cpd_stmt();

        // restore parent table
        restore_table();
    }

    return elif_lst_node(elif_list, node);
}

static ASTNode *read_if_stmt()
{
    expect(TK_IF);
    expect(TK_L_PAREN);
    ASTNode *cond = read_log_or_expr();
    expect(TK_R_PAREN);

    new_table();
    ASTNode *true_stmt = read_cpd_stmt(), *false_stmt = NULL;
    restore_table();

    ensure_token(if_stmt_node(cond, true_stmt, false_stmt));
    if (peek_token()->type == TK_ELIF || peek_token()->type == TK_ELSE)
        false_stmt = read_elif_list();
    return if_stmt_node(cond, true_stmt, false_stmt);
}

static ASTNode *read_init_decl(bool is_mutable)
{
    ASTNode *init = NULL;
    Token *token = expect(TK_IDENTIFIER);
    if (cur_table->contains(*token->identifier) || global_table->contains(*token->identifier))
    {
        err("line: %ld, duplicated definition of \"%s\"\n", token->ln, token->identifier->c_str());
    }

    ASTNode *ident = ident_node(token->identifier, is_mutable);
    cur_table->put(*ident->name, ident);

    ensure_token(init_decl_node(ident, init));
    if (peek_token()->type == TK_ASSIGN)
    {
        expect(TK_ASSIGN);
        init = read_log_or_expr();
    }

    if (!is_mutable && init == NULL)
    {
        ensure_token(init_decl_node(ident, init));
        err("line: %ld, val declaration must have a initializer\n", peek_token()->ln);
    }

    return init_decl_node(ident, init);
}

static ASTNode *read_init_decl_stmt()
{
    Token *token = expect2(TK_VAR, TK_VAL);
    ASTNode *decl_list = init_decl_list_node(new std::vector<ASTNode *>());
    bool is_mutable = token->type == TK_VAR;
    ASTNode *decl = read_init_decl(is_mutable);
    if (decl != NULL)
        decl_list->decl_list->push_back(decl);

    ensure_token(decl_list);
    while (peek_token()->type == TK_COMMA)
    {
        expect(TK_COMMA);
        decl = read_init_decl(is_mutable);
        if (decl != NULL)
            decl_list->decl_list->push_back(decl);
        ensure_token(decl_list);
    }
    expect(TK_SEMICOLON);
    return decl_list;
}

static ASTNode *read_while_stmt()
{
    expect(TK_WHILE);
    expect(TK_L_PAREN);

    enter_loop();
    new_table();

    ASTNode *cond = read_log_or_expr();

    expect(TK_R_PAREN);

    if (cond == NULL)
        return NULL;

    ASTNode *while_stmt = while_stmt_node(cond, read_cpd_stmt());

    restore_table();
    leave_loop();

    return while_stmt;
}

static ASTNode *read_program()
{
    cur_table = new SymTable<ASTNode *>(NULL);
    ASTNode *program = program_node(new std::vector<ASTNode *>(), cur_table);
    while (has_token())
    {
        ASTNode *stmt = NULL;
        switch (peek_token()->type)
        {
        case TK_FOR:
            stmt = read_for_stmt();
            break;
        case TK_FUNC:
            stmt = read_func_decl();
            break;
        case TK_IF:
            stmt = read_if_stmt();
            break;
        case TK_VAL:
        case TK_VAR:
            stmt = read_init_decl_stmt();
            break;
        case TK_WHILE:
            stmt = read_while_stmt();
            break;
        case TK_BREAK:
        case TK_CONTINUE:
            ensure_in_loop();
            break;
        case TK_RETURN:
            ensure_in_func();
            break;
        default:
            stmt = read_expr_stmt();
            break;
        }

        if (stmt != NULL)
        {
            program->statements->push_back(stmt);
        }
    }
    return program;
}

ASTNode *parse(std::vector<Token *> *tokens, std::unordered_map<std::string, vs_addr_t> *globals)
{
    tk_idx = 0;
    tks = tokens;
    global_table = new SymTable<ASTNode *>(NULL);
    for (auto entry : *globals)
    {
        global_table->put(entry.first, ident_node(&(entry.first), false));
    }
    return read_program();
}