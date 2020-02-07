#include "vsparser.h"

static int tk_idx;
static std::vector<Token *> *tks;

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
static ASTNode *read_init_decl();
static ASTNode *read_decl_stmt();
static ASTNode *read_while_stmt();
static std::vector<ASTNode *> *read_toplevel();

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

static int isarithop(INST inst)
{
    return inst >= I_ADD && inst <= I_NEQ;
}

static int iscmp(INST inst)
{
    return inst >= I_LT && inst <= I_NEQ;
}

static int islogicop(INST inst)
{
    return inst >= I_EQ && inst <= I_NOT;
}

static int is_num(TYPE type)
{
    return type == CHAR || type == INT || type == FLOAT;
}

static TYPE valid_b_type(INST inst, TYPE l_type, TYPE r_type)
{
    static char *error_msg = "opcode: %s can not be applied to type %s and type %s\n";

    if (isarithop(inst) && is_num(l_type) && is_num(r_type))
    {
        if (iscmp(inst))
        {
            return BOOL;
        }
        return l_type == FLOAT || r_type == FLOAT ? FLOAT : INT;
    }

    if (islogicop(inst) && l_type == BOOL && r_type == BOOL)
    {
        return BOOL;
    }

    // error = 1;
    // report_error();
    // printf(error_msg, INSTO_S[inst], TYPE_STR[l_type], TYPE_STR[r_type]);
    return NONE;
}

static TYPE valid_u_type(INST inst, TYPE type)
{
    static char *error_msg = "opcode: %s can not be applied to type %s\n";
    if ((is_num(type) && isarithop(inst)) || (type == BOOL && islogicop(inst)))
    {
        return type;
    }

    // error = 1;
    // report_error();
    // printf(error_msg, INSTO_S[inst], TYPE_STR[type]);
    return NONE;
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

static bool valid_lval(ASTNode *node)
{
}

ASTNode::ASTNode(ASTNODE_TYPE node_type, ASTNODE_TYPE ext_node_type)
    : node_type(node_type), ext_node_type(ext_node_type)
{
}

ASTNode::~ASTNode()
{
}

static ASTNode *ident_node(char *name)
{
    ASTNode *node = new ASTNode(AST_IDENT, AST_EXPR);
    node->type = NONE;
    node->name = (char *)malloc(strlen(name));
    strcpy(node->name, name);
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
    ASTNode *node = new ASTNode(AST_B_EXPR, AST_EXPR);
    node->type = valid_b_type(b_opcode, l_operand->type, r_operand->type);
    node->b_opcode = b_opcode;
    node->l_operand = l_operand;
    node->r_operand = r_operand;
    return node;
}

static ASTNode *u_expr_node(INST u_opcode, ASTNode *operand)
{

    ASTNode *node = new ASTNode(AST_U_EXPR, AST_EXPR);
    node->type = valid_u_type(u_opcode, operand->type);
    node->b_opcode = u_opcode;
    node->operand = operand;
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
    if (var_name->node_type != AST_IDENT)
    {
        // error = 1;
        // report_error();
        // printf("unable to assign value to %d.\n", var_name->node_type);
    }
    ASTNode *node = new ASTNode(AST_DECL, AST_UNKNOW);
    node->var_name = var_name;
    node->init_val = init_val;
    return node;
}

static ASTNode *decl_lst_node(KIND specifier, std::vector<ASTNode *> *decl_list)
{
    if (specifier != VAR && specifier != VAL)
    {
        // error = 1;
        // report_error();
        // printf("declaration specifier must be either var or val.\n");
    }
    ASTNode *node = new ASTNode(AST_DECL_LST, AST_UNKNOW);
    node->specifier = specifier;
    node->decl_list = decl_list;
    return node;
}

static ASTNode *assign_node(INST assign_opcode, ASTNode *assign_var, ASTNode *assign_val)
{
    ASTNode *node = new ASTNode(AST_ASSIGN, AST_UNKNOW);
    if (assign_opcode == I_NOP)
    {
        assign_var->type = assign_val->type;
    }
    else
    {
        assign_var->type = valid_b_type(assign_opcode, assign_var->type, assign_val->type);
    }

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
    if (list_name->type != LIST)
    {
        // error = 1;
        // report_error();
        // printf("type %s is not indexable.\n", TYPE_STR[list_name->type]);
    }
    if (list_index->type != INT && list_index->type != CHAR)
    {
        // error = 1;
        // report_error();
        // printf("list index can not be type %d.\n", TYPE_STR[list_index->type]);
    }
    ASTNode *node = new ASTNode(AST_LST_IDX, AST_EXPR);
    node->list_name = list_name;
    node->list_index = list_index;
    return node;
}

static ASTNode *func_call_node(ASTNode *func_name, ASTNode *arg_node)
{
    if (arg_node->node_type != AST_LST_VAL)
    {
        // error = 1;
        // report_error();
        // printf("internal error: arg_list must be of type AST_LST_VAL.\n");
    }
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
    if (if_cond->type != BOOL)
    {
        // error = 1;
        // report_error();
        // printf("condition expression must be bool, not %s.\n", TYPE_STR[if_cond->type]);
    }
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

static ASTNode *while_stmt_node(ASTNode *while_cond, ASTNode *while_stmt)
{
    if (while_cond->type != BOOL)
    {
        // error = 1;
        // report_error();
        // printf("condition expression must be bool, not %s.\n", TYPE_STR[while_cond->type]);
    }
    ASTNode *node = new ASTNode(AST_WHILE_STMT, AST_UNKNOW);
    node->while_cond = while_cond;
    node->while_stmt = while_stmt;
    return node;
}

static ASTNode *for_stmt_node(ASTNode *for_init, ASTNode *for_cond, ASTNode *for_incr, ASTNode *for_body)
{
    if (for_cond->type != BOOL)
    {
        // error = 1;
        // report_error();
        // printf("condition expression must be bool, not %s.\n", TYPE_STR[for_cond->type]);
    }
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

static ASTNode *cpd_stmt_node(std::vector<ASTNode *> *cpd_stmt)
{
    ASTNode *node = new ASTNode(AST_STMTS, AST_UNKNOW);
    node->cpd_stmt = cpd_stmt;
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
    Token *token = get_token();
    if (token->kind != kind)
    {
        // error: expected "kind" but got "token->kind"
    }
    return token;
}

static ASTNode *read_primary_expr()
{
    ASTNode *node = NULL;
    switch (peek_token()->kind)
    {
    case CONSTANT:
        node = const_node(get_token()->value);
        break;
    case IDENTIFIER:
        node = ident_node(get_token()->identifier);
        break;
    case L_PAREN:
        expect(L_PAREN);
        node = read_expr();
        expect(R_PAREN);
        break;
    default:
        // error: does not expect token: peek_token()
        break;
    }
    return node;
}

static ASTNode *read_arg_expr_list()
{
    Token *token;
    ASTNode *arg_list = lst_val_node(new std::vector<ASTNode *>());
    if (peek_token()->kind == R_PAREN)
        return arg_list;
    ASTNode *arg = read_logic_or_expr();
    if (arg == NULL)
    {
        // error: missing args
    }
    arg_list->list_vals->push_back(arg);
    while (peek_token()->kind == COMMA)
    {
        expect(COMMA);
        arg = read_logic_or_expr();
        if (arg == NULL)
        {
            // error: missing args
        }
        arg_list->list_vals->push_back(arg);
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
            // error: wrong list index
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
            // error: missing arg list
        }
        node = func_call_node(node, arg_list);
        expect(R_PAREN);
        break;
    }
    default:
        // error: does not expect token: peek_token()
        break;
    }
    return node;
}

static ASTNode *read_unary_expr()
{
    Token *token = NULL;
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
            // error: missing right value
        }
        node = b_expr_node(get_mul_opcode(token->kind), node, right);
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
            // error: missing right value
        }
        node = b_expr_node(token->kind == ADD ? I_ADD : I_SUB, node, right);
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
            // error: missing right value
        }
        node = b_expr_node(get_rel_opcode(token->kind), node, right);
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
            // error: missing right value
        }
        node = b_expr_node(token->kind == EQ ? I_EQ : I_NEQ, node, right);
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
            // error: missing right value
        }
        node = b_expr_node(I_AND, node, right);
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
            // error: missing right value
        }
        node = b_expr_node(I_OR, node, right);
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
        valid_lval(node);
        INST assign_opcode = get_assign_opcode(get_token()->kind);
        ASTNode *right = read_logic_or_expr();
        if (right == NULL)
        {
            // error: missing right value
        }
        node = assign_node(assign_opcode, node, right);
    }
    return node;
}

static ASTNode *read_expr()
{
    ASTNode *node, *expr = expr_lst_node(new std::vector<ASTNode *>());
    node = read_assign_expr();
    if (node != NULL)
    {
        expr->expr_list->push_back(node);
    }
    while (peek_token()->kind == COMMA)
    {
        expect(COMMA);
        node = read_assign_expr();
        if (node != NULL)
        {
            expr->expr_list->push_back(node);
        }
    }
    return expr;
}

static ASTNode *read_expr_stmt()
{
    if (peek_token()->kind == SEMICOLON)
    {
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
            return read_for_stmt();
        case FUNC:
            return read_func_decl();
        case IF:
            return read_if_stmt();
        case INPUT:
        case PRINT:
            return read_io_stmt();
        case VAL:
        case VAR:
            return read_decl_stmt();
        case WHILE:
            return read_while_stmt();
        case CONTINUE:
            expect(CONTINUE);
            expect(SEMICOLON);
            return continue_node();
        case BREAK:
            expect(BREAK);
            expect(SEMICOLON);
            return break_node(); // TODO: implement break
        case RETURN:
            expect(RETURN);
            node = return_node(read_expr());
            expect(SEMICOLON);
            return node;
        default:
            return read_expr_stmt();
        }
    }
    // error: missing token
    return NULL;
}

static ASTNode *read_cpd_stmt()
{
    expect(L_CURLY);
    ASTNode *stmt = NULL;
    ASTNode *cpd_stmt = cpd_stmt_node(new std::vector<ASTNode *>());
    while (peek_token()->kind != R_CURLY)
    {
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
            cpd_stmt->cpd_stmt->push_back(stmt);
        }
    }
    expect(R_CURLY);
    return cpd_stmt;
}

static ASTNode *read_for_stmt()
{
}

static ASTNode *read_func_decl()
{
    expect(FUNC);
    Token *token = expect(IDENTIFIER);
    expect(L_PAREN);
    ASTNode *arg_node = lst_val_node(new std::vector<ASTNode *>());
    if (peek_token()->kind != R_PAREN)
    {
        Token *arg = expect(IDENTIFIER);
        arg_node->list_vals->push_back(ident_node(arg->identifier));
        while (peek_token()->kind == COMMA)
        {
            expect(COMMA);
            arg = expect(IDENTIFIER);
            arg_node->list_vals->push_back(ident_node(arg->identifier));
        }
    }
    expect(R_PAREN);
    ASTNode *func_body = read_cpd_stmt();
    if (func_body == NULL)
    {
        // error: missing function body
    }
    return func_decl_node(ident_node(token->identifier), arg_node, func_body);
}

static ASTNode *read_elif_stmt()
{
    expect(ELIF);
    expect(L_PAREN);
    ASTNode *cond = read_logic_or_expr();
    expect(R_PAREN);
    ASTNode *body = read_cpd_stmt();
    return if_stmt_node(cond, body, NULL);
}

static ASTNode *read_elif_list()
{
    ASTNode *node = NULL;
    std::vector<ASTNode *> *elif_list = new std::vector<ASTNode *>();
    while (peek_token()->kind == ELIF)
    {
        node = read_elif_stmt();
        if (node != NULL)
        {
            elif_list->push_back(node);
            node = NULL;
        }
    }

    if (peek_token()->kind = ELSE)
    {
        expect(ELSE);
        node = read_cpd_stmt();
    }

    return elif_lst_node(elif_list, node);
}

static ASTNode *read_if_stmt()
{
    expect(IF);
    expect(L_PAREN);
    ASTNode *cond = read_logic_or_expr();
    expect(R_PAREN);
    ASTNode *true_stmt = read_cpd_stmt(), *false_stmt = NULL;
    if (peek_token()->kind == ELIF)
        false_stmt = read_elif_list();
    else if (peek_token()->kind == ELSE)
    {
        expect(ELSE);
        false_stmt = read_cpd_stmt();
    }
    return if_stmt_node(cond, true_stmt, false_stmt);
}

static ASTNode *read_io_stmt()
{
    Token *token = get_token();
    if (token->kind != INPUT && token->kind != PRINT)
    {
        unget_token();
        // error: expect input or print
    }
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
    while (peek_token()->kind == COMMA)
    {
        expect(COMMA);
        init = read_initializer();
        if (init != NULL)
        {
            init_list->list_vals->push_back(init);
        }
    }
    return init_list;
}

static ASTNode *read_initializer()
{
    if (peek_token()->kind == L_BRACK)
    {
        expect(L_BRACK);
        ASTNode *node = read_initializer_list();
        if (peek_token()->kind == COMMA)
            expect(COMMA);
        expect(R_BRACK);
        return node;
    }
    return read_logic_or_expr();
}

static ASTNode *read_init_decl()
{
    ASTNode *node = NULL;
    Token *token = expect(IDENTIFIER);
    if (peek_token()->kind == ASSIGN)
    {
        expect(ASSIGN);
        node = read_initializer();
    }
    return decl_node(ident_node(token->identifier), node);
}

static ASTNode *read_decl_stmt()
{
    Token *token = get_token();
    ASTNode *decl_lst = decl_lst_node(token->kind, new std::vector<ASTNode *>());
    ASTNode *decl = read_init_decl();
    if (decl != NULL)
        decl_lst->decl_list->push_back(decl);
    while (peek_token()->kind == COMMA)
    {
        expect(COMMA);
        decl = read_init_decl();
        if (decl != NULL)
            decl_lst->decl_list->push_back(decl);
    }
    expect(SEMICOLON);
    return decl_lst;
}

static ASTNode *read_while_stmt()
{
    expect(WHILE);
    expect(L_PAREN);
    ASTNode *cond = read_logic_or_expr();
    expect(R_PAREN);
    ASTNode *body = read_cpd_stmt();
    if (cond == NULL || body == NULL)
        return NULL;
    return while_stmt_node(cond, body);
}

static std::vector<ASTNode *> *read_toplevel()
{
    ASTNode *node = NULL;
    std::vector<ASTNode *> *stmts = new std::vector<ASTNode *>();
    while (has_token())
    {
        switch (peek_token()->kind)
        {
        case FOR:
            node = read_for_stmt();
            break;
        case FUNC:
            node = read_func_decl();
            break;
        case IF:
            node = read_if_stmt();
            break;
        case INPUT:
        case PRINT:
            node = read_io_stmt();
            break;
        case VAL:
        case VAR:
            node = read_decl_stmt();
            break;
        case WHILE:
            node = read_while_stmt();
            break;
        case END:
            break;
        default:
            node = read_expr_stmt();
            break;
        }

        if (node != NULL)
        {
            stmts->push_back(node);
            node = NULL;
        }
    }
    return stmts;
}

void init_parser()
{
    tk_idx = 0;
    tks = NULL;
}

std::vector<ASTNode *> *parse(std::vector<Token *> *tokens)
{
    tks = tokens;
    return read_toplevel();
}