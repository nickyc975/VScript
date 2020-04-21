#include "compiler/VSParser.hpp"

#include <cstdarg>

#include "error.hpp"
#include "objects/VSStringObject.hpp"

#define HASTOKEN() this->tokenizer->hastoken()

#define GETTOKEN() this->tokenizer->gettoken()

#define PEEKTOKEN() this->tokenizer->peektoken()

#define POPTOKEN(ntypes, ...) DECREF(this->expect(ntypes, __VA_ARGS__))

// Ensure that there are tokens.
#define ENSURE_TOKEN(ret_val)            \
    if (!HASTOKEN()) {                   \
        err("unexpected end of file\n"); \
        return ret_val;                  \
    }

// Enter loop body
#define ENTER_LOOP() this->inloop++

// Check if in loop
#define ENSURE_IN_LOOP()                                                                        \
    if (!this->inloop) {                                                                        \
        err("line %ld, continue or break must be in while or for statement\n", GETTOKEN()->ln); \
    }

// Leave loop body
#define LEAVE_LOOP()        \
    if (this->inloop > 0) { \
        this->inloop--;     \
    }

// Enter func decl
#define ENTER_FUNC()                       \
    this->inloop_stack.push(this->inloop); \
    this->inloop = 0;                      \
    this->infunc++;

// Check if in func decl
#define ENSURE_IN_FUNC()                                                           \
    if (!this->infunc) {                                                           \
        err("line %ld, return must be in function declaration\n", GETTOKEN()->ln); \
    }

// Leave func decl
#define LEAVE_FUNC()                         \
    this->inloop = this->inloop_stack.top(); \
    this->inloop_stack.pop();                \
    if (this->infunc > 0) {                  \
        this->infunc--;                      \
    }

VSParser::VSParser(VSTokenizer *tokenizer) {
    this->tokenizer = tokenizer;
    INCREF(tokenizer);

    this->infunc = 0;
    this->inloop = 0;
    this->inloop_stack = std::stack<unsigned int>();
}

VSParser::~VSParser() {
    DECREF_EX(this->tokenizer);
}

VSToken *VSParser::expect(int ntypes, ...) {
    ENSURE_TOKEN(NULL);

    bool res = false;
    VSToken *token = GETTOKEN();

    va_list types;
    va_start(types, ntypes);
    for (int i = 0; i < ntypes; i++) {
        TOKEN_TYPE type = (TOKEN_TYPE)va_arg(types, int);
        if (token->tk_type == type) {
            res = true;
            break;
        }
    }
    va_end(types);

    if (!res) {
        err("line: %ld, col: %ld, unexpected token: \"%s\"", token->ln, token->col, STRING_TO_C_STRING(token->literal).c_str());
        DECREF_EX(token);
    }

    return token;
}

void VSParser::read_func_def(FuncDeclNode *func) {
    // read func args
    ENSURE_TOKEN();
    if (PEEKTOKEN()->tk_type != TK_R_PAREN) {
        VSToken *token = this->expect(1, TK_IDENTIFIER);
        if (token != NULL) {
            IdentNode *arg = new IdentNode(token->literal);
            func->add_arg(arg);
        }

        ENSURE_TOKEN();
        while (PEEKTOKEN()->tk_type == TK_COMMA) {
            POPTOKEN(1, TK_COMMA);
            token = this->expect(1, TK_IDENTIFIER);
            if (token != NULL) {
                IdentNode *arg = new IdentNode(token->literal);
                func->add_arg(arg);
            }
            ENSURE_TOKEN();
        }
    }
    POPTOKEN(1, TK_R_PAREN);

    // read func body
    VSASTNode *func_body = this->read_cpd_stmt();
    if (func_body == NULL) {
        ENSURE_TOKEN();
        err("line: %ld, missing function body\n", PEEKTOKEN()->ln);
    }
    func->set_body(func_body);
}

VSASTNode *VSParser::read_tuple_decl_or_expr() {
    VSASTNode *node = this->read_log_or_expr();
    if (node == NULL)
        return new TupleDeclNode();

    if (PEEKTOKEN()->tk_type == TK_COMMA) {
        TupleDeclNode *tuple = new TupleDeclNode();
        tuple->append(node);

        while (PEEKTOKEN()->tk_type == TK_COMMA) {
            POPTOKEN(1, TK_COMMA);
            node = this->read_log_or_expr();
            if (node != NULL) {
                tuple->append(node);
            }
            ENSURE_TOKEN(tuple);
        }
        return tuple;
    }
    return node;
}

VSASTNode *VSParser::read_list_decl() {
    ListDeclNode *list = new ListDeclNode();

    VSASTNode *node = this->read_log_or_expr();
    if (node != NULL)
        list->append(node);

    while (PEEKTOKEN()->tk_type == TK_COMMA) {
        POPTOKEN(1, TK_COMMA);
        node = this->read_log_or_expr();
        if (node != NULL) {
            list->append(node);
        }
        ENSURE_TOKEN(list);
    }
    return list;
}

VSASTNode *VSParser::read_dict_or_set_decl() {
    VSASTNode *key = this->read_log_or_expr();
    if (key == NULL) {
        return new DictDeclNode();
    }

    if (PEEKTOKEN()->tk_type == TK_COLON) {
        POPTOKEN(1, TK_COLON);
        DictDeclNode *dict = new DictDeclNode();
        VSASTNode *value = this->read_log_or_expr();
        if (value != NULL) {
            dict->append(new PairExprNode(key, value));
        }

        while (PEEKTOKEN()->tk_type == TK_COMMA) {
            POPTOKEN(1, TK_COMMA);
            key = this->read_log_or_expr();
            POPTOKEN(1, TK_COLON);
            value = this->read_log_or_expr();
            if (key != NULL && value != NULL) {
                dict->append(new PairExprNode(key, value));
            }
            ENSURE_TOKEN(dict);
        }
        return dict;
    }

    SetDeclNode *set = new SetDeclNode();
    set->append(key);

    while (PEEKTOKEN()->tk_type == TK_COMMA) {
        POPTOKEN(1, TK_COMMA);
        key = this->read_log_or_expr();
        if (key != NULL) {
            set->append(key);
        }
        ENSURE_TOKEN(set);
    }
    return set;
}

VSASTNode *VSParser::read_lambda_decl() {
    POPTOKEN(1, TK_LAMBDA);
    POPTOKEN(1, TK_L_PAREN);

    ENTER_FUNC();

    FuncDeclNode *func = new FuncDeclNode(NULL);

    this->read_func_def(func);

    LEAVE_FUNC();

    return func;
}

VSASTNode *VSParser::read_primary_expr() {
    VSToken *token;
    VSASTNode *node = NULL;

    ENSURE_TOKEN(NULL);
    switch (PEEKTOKEN()->tk_type) {
        case TK_CONSTANT:
            token = GETTOKEN();
            node = new ConstNode(token->tk_value);
            DECREF(token);
            break;
        case TK_IDENTIFIER:
            token = GETTOKEN();
            node = new IdentNode(token->literal);
            DECREF(token);
            break;
        case TK_L_PAREN:
            POPTOKEN(1, TK_L_PAREN);
            node = this->read_tuple_decl_or_expr();
            POPTOKEN(1, TK_R_PAREN);
            break;
        case TK_L_BRACK:
            POPTOKEN(1, TK_L_BRACK);
            node = this->read_list_decl();
            POPTOKEN(1, TK_R_BRACK);
            break;
        case TK_L_CURLY:
            POPTOKEN(1, TK_L_CURLY);
            node = this->read_dict_or_set_decl();
            POPTOKEN(1, TK_R_CURLY);
            break;
        case TK_LAMBDA:
            node = this->read_lambda_decl();
        default:
            break;
    }
    return node;
}

VSASTNode *VSParser::read_postfix_expr() {
    VSASTNode *node = this->read_primary_expr();
    if (node == NULL || !HASTOKEN())
        return node;

    VSToken *token = PEEKTOKEN();
    while (token->tk_type == TK_L_BRACK || token->tk_type == TK_L_PAREN || token->tk_type == TK_DOT) {
        if (token->tk_type == TK_L_BRACK) {
            POPTOKEN(1, TK_L_BRACK);
            VSASTNode *index = this->read_log_or_expr();
            if (index == NULL) {
                ENSURE_TOKEN(node);
                err("line: %ld, invalid list index\n", PEEKTOKEN()->ln);
                terminate(TERM_ERROR);
            }
            node = new IdxExprNode(node, index);
            POPTOKEN(1, TK_R_BRACK);
        } else if (token->tk_type == TK_L_PAREN) {
            POPTOKEN(1, TK_L_PAREN);
            VSASTNode *args = this->read_tuple_decl_or_expr();
            node = new FuncCallNode(node, args);
            POPTOKEN(1, TK_R_PAREN);
        } else {
            POPTOKEN(1, TK_DOT);
            ENSURE_TOKEN(node);
            token = this->expect(1, TK_IDENTIFIER);
            if (token != NULL) {
                node = new DotExprNode(node, new IdentNode(token->literal));
            }
        }
        ENSURE_TOKEN(node);
        token = PEEKTOKEN();
    }
    return node;
}

VSASTNode *VSParser::read_unary_expr() {
    VSToken *token = NULL;
    ENSURE_TOKEN(NULL);
    if (PEEKTOKEN()->tk_type == TK_SUB || PEEKTOKEN()->tk_type == TK_NOT) {
        token = GETTOKEN();
    }
    VSASTNode *node = this->read_postfix_expr();
    if (node == NULL || !HASTOKEN())
        return node;
    if (token != NULL) {
        node = new UOPNode(token->tk_type, node);
    }
    DECREF(token);
    return node;
}

VSASTNode *VSParser::read_mul_expr() {
    VSToken *token = NULL;
    VSASTNode *node = this->read_unary_expr();
    if (node == NULL || !HASTOKEN())
        return node;
    while (is_mul(PEEKTOKEN()->tk_type)) {
        token = GETTOKEN();
        VSASTNode *right = this->read_unary_expr();
        if (right == NULL) {
            ENSURE_TOKEN(node);
            err("line: %ld, missing right value\n", PEEKTOKEN()->ln);
        }
        node = new BOPNode(token->tk_type, node, right);
        ENSURE_TOKEN(node);
    }
    DECREF(token);
    return node;
}

VSASTNode *VSParser::read_add_expr() {
    VSToken *token = NULL;
    VSASTNode *node = this->read_mul_expr();
    if (node == NULL || !HASTOKEN())
        return node;
    while (PEEKTOKEN()->tk_type == TK_ADD || PEEKTOKEN()->tk_type == TK_SUB) {
        token = GETTOKEN();
        VSASTNode *right = this->read_mul_expr();
        if (right == NULL) {
            ENSURE_TOKEN(node);
            err("line: %ld, missing right value\n", PEEKTOKEN()->ln);
        }
        node = new BOPNode(token->tk_type, node, right);
        ENSURE_TOKEN(node);
    }
    DECREF(token);
    return node;
}

VSASTNode *VSParser::read_rel_expr() {
    VSToken *token = NULL;
    VSASTNode *node = this->read_add_expr();
    if (node == NULL || !HASTOKEN())
        return node;
    while (is_rel(PEEKTOKEN()->tk_type)) {
        token = GETTOKEN();
        VSASTNode *right = this->read_add_expr();
        if (right == NULL) {
            ENSURE_TOKEN(node);
            err("line: %ld, missing right value\n", PEEKTOKEN()->ln);
        }
        node = new BOPNode(token->tk_type, node, right);
        ENSURE_TOKEN(node);
    }
    DECREF(token);
    return node;
}

VSASTNode *VSParser::read_eql_expr() {
    VSToken *token = NULL;
    VSASTNode *node = this->read_rel_expr();
    if (node == NULL || !HASTOKEN())
        return node;
    while (PEEKTOKEN()->tk_type == TK_EQ || PEEKTOKEN()->tk_type == TK_NEQ) {
        token = GETTOKEN();
        VSASTNode *right = this->read_rel_expr();
        if (right == NULL) {
            ENSURE_TOKEN(node);
            err("line: %ld, missing right value\n", PEEKTOKEN()->ln);
        }
        node = new BOPNode(token->tk_type, node, right);
        ENSURE_TOKEN(node);
    }
    DECREF(token);
    return node;
}

VSASTNode *VSParser::read_log_and_expr() {
    VSASTNode *node = this->read_eql_expr();
    if (node == NULL || !HASTOKEN())
        return node;
    while (PEEKTOKEN()->tk_type == TK_AND) {
        POPTOKEN(1, TK_AND);
        VSASTNode *right = this->read_eql_expr();
        if (right == NULL) {
            ENSURE_TOKEN(node);
            err("line: %ld, missing right value\n", PEEKTOKEN()->ln);
        }
        node = new BOPNode(TK_AND, node, right);
        ENSURE_TOKEN(node);
    }
    return node;
}

VSASTNode *VSParser::read_log_xor_expr() {
    VSASTNode *node = this->read_log_and_expr();
    if (node == NULL || !HASTOKEN())
        return node;
    while (PEEKTOKEN()->tk_type == TK_XOR) {
        POPTOKEN(1, TK_XOR);
        VSASTNode *right = this->read_log_and_expr();
        if (right == NULL) {
            ENSURE_TOKEN(node);
            err("line: %ld, missing right value\n", PEEKTOKEN()->ln);
        }
        node = new BOPNode(TK_XOR, node, right);
        ENSURE_TOKEN(node);
    }
    return node;
}

VSASTNode *VSParser::read_log_or_expr() {
    VSASTNode *node = this->read_log_xor_expr();
    if (node == NULL || !HASTOKEN())
        return node;
    while (PEEKTOKEN()->tk_type == TK_OR) {
        POPTOKEN(1, TK_OR);
        VSASTNode *right = this->read_log_xor_expr();
        if (right == NULL) {
            ENSURE_TOKEN(node);
            err("line: %ld, missing right value\n", PEEKTOKEN()->ln);
        }
        node = new BOPNode(TK_OR, node, right);
        ENSURE_TOKEN(node);
    }
    return node;
}

VSASTNode *VSParser::read_assign_expr() {
    VSToken *token = NULL;
    VSASTNode *node = this->read_log_or_expr();
    if (node == NULL || !HASTOKEN())
        return node;
    if (is_assign(PEEKTOKEN()->tk_type)) {
        token = GETTOKEN();
        TOKEN_TYPE assign_opcode = token->tk_type;
        if (assign_opcode == TK_ASSIGN)
            assign_opcode = TK_NOP;
        else
            assign_opcode = (TOKEN_TYPE)((int)assign_opcode - 1);
        VSASTNode *right = this->read_log_or_expr();
        if (right == NULL) {
            ENSURE_TOKEN(node);
            err("line: %ld, missing right value\n", PEEKTOKEN()->ln);
        }
        node = new AssignExprNode(assign_opcode, node, right);
    }
    DECREF(token);
    return node;
}

VSASTNode *VSParser::read_expr_list() {
    VSASTNode *node = this->read_assign_expr();
    if (!HASTOKEN() || PEEKTOKEN()->tk_type != TK_COMMA)
        return node;

    ExprListNode *expr_list = new ExprListNode();
    if (node != NULL)
        expr_list->append(node);

    while (PEEKTOKEN()->tk_type == TK_COMMA) {
        POPTOKEN(1, TK_COMMA);
        node = this->read_assign_expr();
        if (node != NULL)
            expr_list->append(node);
        ENSURE_TOKEN(expr_list);
    }
    return expr_list;
}

VSASTNode *VSParser::read_expr_stmt() {
    ENSURE_TOKEN(NULL);
    if (PEEKTOKEN()->tk_type == TK_SEMICOLON) {
        this->expect(1, TK_SEMICOLON);
        return NULL;
    }
    VSASTNode *node = this->read_expr_list();
    this->expect(1, TK_SEMICOLON);
    return node;
}

VSASTNode *VSParser::read_stmt() {
    VSASTNode *node = NULL;
    if (HASTOKEN()) {
        switch (PEEKTOKEN()->tk_type) {
            case TK_FOR:
                return this->read_for_stmt();
            case TK_FUNC:
                return this->read_func_decl();
            case TK_IF:
                return this->read_if_stmt();
            case TK_VAL:
            case TK_VAR:
                return this->read_init_decl_stmt();
            case TK_WHILE:
                return this->read_while_stmt();
            case TK_CONTINUE:
                POPTOKEN(1, TK_CONTINUE);
                POPTOKEN(1, TK_SEMICOLON);
                ENSURE_IN_LOOP();
                return new ContinueStmtNode();
            case TK_BREAK:
                POPTOKEN(1, TK_BREAK);
                POPTOKEN(1, TK_SEMICOLON);
                ENSURE_IN_LOOP();
                return new BreakStmtNode();
            case TK_RETURN:
                POPTOKEN(1, TK_RETURN);
                node = new ReturnStmtNode(this->read_log_or_expr());
                POPTOKEN(1, TK_SEMICOLON);
                ENSURE_IN_FUNC();
                return node;
            default:
                return read_expr_stmt();
        }
    }
    err("unexpected end of file");
    return NULL;
}

VSASTNode *VSParser::read_cpd_stmt() {
    POPTOKEN(1, TK_L_CURLY);
    VSASTNode *stmt = NULL;
    CpdStmtNode *cpd_stmt = new CpdStmtNode();

    ENSURE_TOKEN(cpd_stmt);
    while (PEEKTOKEN()->tk_type != TK_R_CURLY) {
        if (PEEKTOKEN()->tk_type == TK_VAL || PEEKTOKEN()->tk_type == TK_VAR) {
            stmt = this->read_init_decl_stmt();
        } else {
            stmt = this->read_stmt();
        }

        if (stmt != NULL) {
            cpd_stmt->append(stmt);
        }
        ENSURE_TOKEN(cpd_stmt);
    }
    POPTOKEN(1, TK_R_CURLY);
    return cpd_stmt;
}

VSASTNode *VSParser::read_for_stmt() {
    POPTOKEN(1, TK_FOR);
    POPTOKEN(1, TK_L_PAREN);

    ENTER_LOOP();

    ENSURE_TOKEN(NULL);
    TOKEN_TYPE type = PEEKTOKEN()->tk_type;
    VSASTNode *init = NULL, *cond = NULL, *incr = NULL;

    // read for init
    if (type == TK_VAL || type == TK_VAR)
        init = this->read_init_decl_stmt();
    else
        init = this->read_expr_stmt();

    // read for cond
    cond = this->read_log_or_expr();
    POPTOKEN(1, TK_SEMICOLON);

    // read for incr
    ENSURE_TOKEN(NULL);
    if (PEEKTOKEN()->tk_type != TK_R_PAREN) {
        incr = this->read_expr_list();
    }
    POPTOKEN(1, TK_R_PAREN);

    VSASTNode *for_stmt = new ForStmtNode(init, cond, incr, this->read_cpd_stmt());

    LEAVE_LOOP();

    return for_stmt;
}

VSASTNode *VSParser::read_func_decl() {
    POPTOKEN(1, TK_FUNC);
    VSToken *token = this->expect(1, TK_IDENTIFIER);
    if (token == NULL) {
        return NULL;
    }
    POPTOKEN(1, TK_L_PAREN);

    ENTER_FUNC();

    FuncDeclNode *func = new FuncDeclNode(new IdentNode(token->literal));

    this->read_func_def(func);

    LEAVE_FUNC();

    return func;
}

VSASTNode *VSParser::read_elif_stmt() {
    POPTOKEN(1, TK_ELIF);
    POPTOKEN(1, TK_L_PAREN);

    // read elif cond
    VSASTNode *cond = this->read_log_or_expr();

    POPTOKEN(1, TK_R_PAREN);

    // read elif body
    VSASTNode *body = this->read_cpd_stmt();

    return new IfStmtNode(cond, body, NULL);
}

VSASTNode *VSParser::read_elif_list() {
    VSASTNode *node = NULL;
    ElifListNode *elif_list = new ElifListNode();

    ENSURE_TOKEN(elif_list);
    while (PEEKTOKEN()->tk_type == TK_ELIF) {
        node = this->read_elif_stmt();
        if (node != NULL) {
            elif_list->add_elif((IfStmtNode *)node);
            node = NULL;
        }
        ENSURE_TOKEN(elif_list);
    }

    if (PEEKTOKEN()->tk_type == TK_ELSE) {
        POPTOKEN(1, TK_ELSE);
        // read else body
        node = this->read_cpd_stmt();
    }
    elif_list->set_else(node);

    return elif_list;
}

VSASTNode *VSParser::read_if_stmt() {
    POPTOKEN(1, TK_IF);
    POPTOKEN(1, TK_L_PAREN);
    VSASTNode *cond = this->read_log_or_expr();
    POPTOKEN(1, TK_R_PAREN);

    VSASTNode *true_stmt = this->read_cpd_stmt(), *false_stmt = NULL;

    ENSURE_TOKEN(new IfStmtNode(cond, true_stmt, false_stmt));
    if (PEEKTOKEN()->tk_type == TK_ELIF || PEEKTOKEN()->tk_type == TK_ELSE)
        false_stmt = read_elif_list();
    return new IfStmtNode(cond, true_stmt, false_stmt);
}

VSASTNode *VSParser::read_init_decl() {
    VSASTNode *init = NULL;
    VSToken *token = this->expect(1, TK_IDENTIFIER);
    if (token == NULL) {
        return NULL;
    }
    IdentNode *ident = new IdentNode(token->literal);

    ENSURE_TOKEN(new InitDeclNode(ident, init));
    if (PEEKTOKEN()->tk_type == TK_ASSIGN) {
        POPTOKEN(1, TK_ASSIGN);
        init = this->read_log_or_expr();
    }

    return new InitDeclNode(ident, init);
}

VSASTNode *VSParser::read_init_decl_stmt() {
    VSToken *token = this->expect(2, TK_VAR, TK_VAL);
    if (token == NULL) {
        return NULL;
    }

    InitDeclListNode *decl_list = new InitDeclListNode(token->tk_type);
    InitDeclNode *decl = (InitDeclNode *)this->read_init_decl();
    if (decl != NULL) {
        decl_list->add_decl(decl);
    }

    ENSURE_TOKEN(decl_list);
    while (PEEKTOKEN()->tk_type == TK_COMMA) {
        POPTOKEN(1, TK_COMMA);
        decl = (InitDeclNode *)this->read_init_decl();
        if (decl != NULL) {
            decl_list->add_decl(decl);
        }
        ENSURE_TOKEN(decl_list);
    }
    POPTOKEN(1, TK_SEMICOLON);
    return decl_list;
}

VSASTNode *VSParser::read_while_stmt() {
    POPTOKEN(1, TK_WHILE);
    POPTOKEN(1, TK_L_PAREN);

    ENTER_LOOP();

    long long ln = PEEKTOKEN()->ln;
    VSASTNode *cond = this->read_log_or_expr();
    if (cond == NULL) {
        err("line: %ld, missing while condition", ln);
        return NULL;
    }
    POPTOKEN(1, TK_R_PAREN);
    VSASTNode *while_stmt = new WhileStmtNode(cond, read_cpd_stmt());

    LEAVE_LOOP();

    return while_stmt;
}

VSASTNode *VSParser::read_program() {
    ProgramNode *program = new ProgramNode();
    while (HASTOKEN()) {
        VSASTNode *stmt = NULL;
        switch (PEEKTOKEN()->tk_type) {
            case TK_FOR:
                stmt = this->read_for_stmt();
                break;
            case TK_FUNC:
                stmt = this->read_func_decl();
                break;
            case TK_IF:
                stmt = this->read_if_stmt();
                break;
            case TK_VAL:
            case TK_VAR:
                stmt = this->read_init_decl_stmt();
                break;
            case TK_WHILE:
                stmt = this->read_while_stmt();
                break;
            case TK_BREAK:
            case TK_CONTINUE:
                ENSURE_IN_LOOP();
                break;
            case TK_RETURN:
                ENSURE_IN_FUNC();
                break;
            default:
                stmt = this->read_expr_stmt();
                break;
        }

        if (stmt != NULL) {
            program->append(stmt);
        }
    }
    return program;
}

VSASTNode *VSParser::parse() {
    return this->read_program();
}