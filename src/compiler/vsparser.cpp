#include "vsparser.h"
#include "SymTable.h"

static int tk_idx;
static std::vector<Token *> *tks;
static SymTable *symtable;

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
    if (tk_idx > 0) tk_idx--;
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
    node->name = name;
    node->value = NULL;
    return node;
}

static ASTNode *const_node(Value *value)
{
    ASTNode *node = new ASTNode(AST_CONST, AST_EXPR);
    node->type = value->type;
    node->is_mutable = value->is_mutable;
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

static ASTNode *expr_lst_node(int expr_num, std::vector<ASTNode *> *expr_list)
{
    ASTNode *node = new ASTNode(AST_EXPR_LST, AST_UNKNOW);
    node->expr_num = expr_num;
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
    if (init_val != NULL)
    {
        var_name->type = init_val->type;
        var_name->value = init_val->value;
    }
    return node;
}

static ASTNode *decl_lst_node(int specifier, int decl_num, std::vector<ASTNode *> *decl_list)
{
    if (specifier != VAR && specifier != VAL)
    {
        // error = 1;
        // report_error();
        // printf("declaration specifier must be either var or val.\n");
    }
    ASTNode *node = new ASTNode(AST_DECL_LST, AST_UNKNOW);
    node->specifier = specifier;
    node->decl_num = decl_num;
    node->decl_list = decl_list;
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

static ASTNode *lst_val_node(int list_len, std::vector<ASTNode *> *list_vals)
{
    ASTNode *node = new ASTNode(AST_LST_VAL, AST_EXPR);
    node->type = LIST;
    node->list_len = list_len;
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

static ASTNode *func_call_node(ASTNode *func_name, ASTNode *arg_list)
{
    if (arg_list->node_type != AST_LST_VAL)
    {
        // error = 1;
        // report_error();
        // printf("internal error: arg_list must be of type AST_LST_VAL.\n");
    }
    ASTNode *node = new ASTNode(AST_FUNC_CALL, AST_EXPR);
    node->func_name = func_name;
    node->arg_list = arg_list;
    return node;
}

static ASTNode *func_decl_node(ASTNode *func_name, int arg_num, ASTNode *func_body)
{
    ASTNode *node = new ASTNode(AST_FUNC_DECL, AST_UNKNOW);
    node->func_name = func_name;
    node->arg_num = arg_num;
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

static ASTNode *elif_lst_node(std::vector<ASTNode *> *elif_list)
{
    ASTNode *node = new ASTNode(AST_ELIF_LST, AST_UNKNOW);
    node->elif_list = elif_list;
    node->else_node = NULL;
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

static ASTNode *jmp_node(ASTNode *curr_stmt, ASTNode *next_stmt)
{
    ASTNode *node = new ASTNode(AST_JMP, AST_UNKNOW);
    node->curr_stmt = curr_stmt;
    node->next_stmt = next_stmt;
    return node;
}

static ASTNode *stmts_node(int stmts_num, std::vector<ASTNode *> *stmts)
{
    ASTNode *node = new ASTNode(AST_STMTS, AST_UNKNOW);
    node->stmts = stmts;
    node->stmts_num = stmts_num;
    return node;
}

static ASTNode *return_node(ASTNode *ret_val)
{
    ASTNode *node = new ASTNode(AST_RETURN, AST_UNKNOW);
    node->ret_val = ret_val;
    return node;
}

static ASTNode *print_stmt_node(int var_num, std::vector<ASTNode *> *var_list)
{
    ASTNode *node = new ASTNode(AST_PRINT_STMT, AST_UNKNOW);
    node->var_num = var_num;
    node->var_list = var_list;
    return node;
}

static ASTNode *input_stmt_node(int var_num, std::vector<ASTNode *> *var_list)
{
    ASTNode *node = new ASTNode(AST_INPUT_STMT, AST_UNKNOW);
    node->var_num = var_num;
    node->var_list = var_list;
    return node;
}

static void expect(KIND kind)
{
    Token *token = get_token();
    if (token->kind != kind)
    {
        // error: expected "kind" but got "token->kind"
    }
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
    ASTNode *arg_list = lst_val_node(0, new std::vector<ASTNode *>());
    if (peek_token()->kind == R_PAREN) return arg_list;
    ASTNode *arg = read_logic_or_expr();
    if (arg == NULL)
    {
        // error: missing args
    }
    arg_list->list_vals->push_back(arg);
    arg_list->list_len++;
    while (peek_token()->kind == COMMA)
    {
        expect(COMMA);
        arg = read_logic_or_expr();
        if (arg == NULL)
        {
            // error: missing args
        }
        arg_list->list_vals->push_back(arg);
        arg_list->list_len++;
    }
    return arg_list;
}

static ASTNode *read_postfix_expr()
{
    ASTNode *node = read_primary_expr();
    if (node == NULL || !has_token()) return node;
    switch (peek_token()->kind)
    {
    case L_BRACK:
        expect(L_BRACK);
        ASTNode *lst_idx = read_expr();
        if (lst_idx == NULL)
        {
            // error: wrong list index
        }
        node = lst_idx_node(node, lst_idx);
        expect(R_BRACK);
        break;
    case L_PAREN:
        expect(L_PAREN);
        ASTNode *arg_list = read_arg_expr_list();
        if (arg_list == NULL)
        {
            // error: missing arg list
        }
        node = func_call_node(node, arg_list);
        expect(R_PAREN);
        break;
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
    if (node == NULL || !has_token()) return node;
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
    if (node == NULL || !has_token()) return node;
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
    if (node == NULL || !has_token()) return node;
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
    if (node == NULL || !has_token()) return node;
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
    if (node == NULL || !has_token()) return node;
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
    if (node == NULL || !has_token()) return node;
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
    if (node == NULL || !has_token()) return node;
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
    if (node == NULL || !has_token()) return node;

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
    ASTNode *node, *expr = expr_lst_node(0, new std::vector<ASTNode *>());
    node = read_assign_expr();
    if (node != NULL)
    {
        expr->expr_list->push_back(node);
        expr->expr_num++;
    }
    while (peek_token()->kind == COMMA)
    {
        expect(COMMA);
        node = read_assign_expr();
        if (node != NULL)
        {
            expr->expr_list->push_back(node);
            expr->expr_num++;
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

static ASTNode *read_for_stmt()
{
    
}

static ASTNode *read_func_decl()
{
    
}

static ASTNode *read_if_stmt()
{
    
}

static ASTNode *read_input_stmt()
{
    
}

static ASTNode *read_print_stmt()
{
    
}

static ASTNode *read_var_stmt()
{
    
}

static ASTNode *read_val_stmt()
{
    
}

static ASTNode *read_while_stmt()
{
    
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
            node = read_input_stmt();
            break;
        case PRINT:
            node = read_print_stmt();
            break;
        case VAL:
            node = read_val_stmt();
            break;
        case VAR:
            node = read_var_stmt();
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

static void init_parser()
{
    tk_idx = 0;
    tks = NULL;
    symtable = new SymTable(NULL);
}

static std::vector<ASTNode *> *parse(std::vector<Token *> *tokens)
{
    tks = tokens;
    return read_toplevel();
}