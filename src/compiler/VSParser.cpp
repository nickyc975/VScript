#include "error.hpp"
#include "compiler/VSParser.hpp"

#include <cstdarg>

#define HASTOKEN() this->tokenizer->hastoken()

#define GETTOKEN() this->tokenizer->gettoken()

#define PEEKTOKEN() this->tokenizer->peektoken()

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
#define ENSURE_IN_FUN()                                                            \
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

void VSParser::expect(int ntypes, ...) {
    ENSURE_TOKEN();

    bool res = false;
    VSToken *token = GETTOKEN();

    va_list types;
    va_start(types, ntypes);
    for (int i = 0; i < ntypes; i++) {
        TOKEN_TYPE type = va_arg(types, TOKEN_TYPE);
        if (token->tk_type == type) {
            res = true;
            break;
        }
    }
    va_end(types);

    if (!res) {
        err("line: %ld, col: %ld, unexpected token: \"%s\"", token->ln, token->col, token->literal.c_str());
        terminate(TERM_ERROR);
    }
    DECREF(token);
}

VSASTNode *VSParser::read_tuple_decl_or_expr() {
    VSASTNode *node = this->read_log_or_expr();
    if (node == NULL)
        return new TupleDeclNode();

    if (PEEKTOKEN()->tk_type == TK_COMMA) {
        TupleDeclNode *tuple = new TupleDeclNode();
        tuple->append(node);

        while (PEEKTOKEN()->tk_type == TK_COMMA) {
            this->expect(1, TK_COMMA);
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
        this->expect(1, TK_COMMA);
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
        this->expect(1, TK_COLON);
        DictDeclNode *dict = new DictDeclNode();
        VSASTNode *value = this->read_log_or_expr();
        if (value != NULL) {
            dict->append(new PairExprNode(key, value));
        }

        while (PEEKTOKEN()->tk_type == TK_COMMA)
        {
            this->expect(1, TK_COMMA);
            key = this->read_log_or_expr();
            this->expect(1, TK_COLON);
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

    while (PEEKTOKEN()->tk_type == TK_COMMA)
    {
        this->expect(1, TK_COMMA);
        key = this->read_log_or_expr();
        if (key != NULL) {
            set->append(key);
        }
        ENSURE_TOKEN(set);
    }
    return set;
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
            this->expect(1, TK_L_PAREN);
            node = this->read_tuple_decl_or_expr();
            this->expect(1, TK_R_PAREN);
            break;
        case TK_L_BRACK:
            this->expect(1, TK_L_BRACK);
            node = this->read_list_decl();
            this->expect(1, TK_R_BRACK);
            break;
        case TK_FUNC:
            node = this->read_func_decl();
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
            this->expect(1, TK_L_BRACK);
            VSASTNode *index = this->read_log_or_expr();
            if (index == NULL) {
                ENSURE_TOKEN(node);
                err("line: %ld, invalid list index\n", PEEKTOKEN()->ln);
                terminate(TERM_ERROR);
            }
            node = new IdxExprNode(node, index);
            this->expect(1, TK_R_BRACK);
        } else if (token->tk_type == TK_L_PAREN) {
            this->expect(1, TK_L_PAREN);
            VSASTNode *args = this->read_tuple_decl_or_expr();
            node = new FuncCallNode(node, args);
            this->expect(1, TK_R_PAREN);
        } else {
            this->expect(1, TK_DOT);
            ENSURE_TOKEN(node);
            token = GETTOKEN();
            if (token->tk_type != TK_IDENTIFIER) {
                err("line: %ld, col: %ld, unexpected token: \"%s\"", token->ln, token->col, token->literal.c_str());
                DECREF(token);
                terminate(TERM_ERROR);
            }
            node = new DotExprNode(node, new IdentNode(token->literal));
        }
        ENSURE_TOKEN(node);
        token = PEEKTOKEN();
    }
    return node;
}

VSASTNode *VSParser::read_unary_expr() {
    Token *token = NULL;
    ensure_token(NULL);
    if (peek_token()->type == TK_SUB || peek_token()->type == TK_NOT) {
        token = get_token();
    }
    VSASTNode *node = read_postfix_expr();
    if (node == NULL || !has_token())
        return node;
    if (token != NULL) {
        node = u_expr_node(token->type == TK_SUB ? TK_SUB : TK_NOT, node);
    }
    return node;
}

VSASTNode *VSParser::read_mul_expr() {
    Token *token;
    VSASTNode *node = read_unary_expr();
    if (node == NULL || !has_token())
        return node;
    while (is_mul(peek_token()->type)) {
        token = get_token();
        VSASTNode *right = read_unary_expr();
        if (right == NULL) {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(token->type, node, right);
        ensure_token(node);
    }
    return node;
}

VSASTNode *VSParser::read_add_expr() {
    Token *token;
    VSASTNode *node = read_mul_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->type == TK_ADD || peek_token()->type == TK_SUB) {
        token = get_token();
        VSASTNode *right = read_mul_expr();
        if (right == NULL) {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(token->type == TK_ADD ? TK_ADD : TK_SUB, node, right);
        ensure_token(node);
    }
    return node;
}

VSASTNode *VSParser::read_rel_expr() {
    Token *token;
    VSASTNode *node = read_add_expr();
    if (node == NULL || !has_token())
        return node;
    while (is_rel(peek_token()->type)) {
        token = get_token();
        VSASTNode *right = read_add_expr();
        if (right == NULL) {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(token->type, node, right);
        ensure_token(node);
    }
    return node;
}

VSASTNode *VSParser::read_eql_expr() {
    Token *token;
    VSASTNode *node = read_rel_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->type == TK_EQ || peek_token()->type == TK_NEQ) {
        token = get_token();
        VSASTNode *right = read_rel_expr();
        if (right == NULL) {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(token->type == TK_EQ ? TK_EQ : TK_NEQ, node, right);
        ensure_token(node);
    }
    return node;
}

VSASTNode *VSParser::read_log_and_expr() {
    VSASTNode *node = read_eql_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->type == TK_AND) {
        expect(TK_AND);
        VSASTNode *right = read_eql_expr();
        if (right == NULL) {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(TK_AND, node, right);
        ensure_token(node);
    }
    return node;
}

VSASTNode *VSParser::read_log_or_expr() {
    VSASTNode *node = read_log_and_expr();
    if (node == NULL || !has_token())
        return node;
    while (peek_token()->type == TK_OR) {
        expect(TK_OR);
        VSASTNode *right = read_log_and_expr();
        if (right == NULL) {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = b_expr_node(TK_OR, node, right);
        ensure_token(node);
    }
    return node;
}

VSASTNode *VSParser::read_assign_expr() {
    VSASTNode *node = read_log_or_expr();
    if (node == NULL || !has_token())
        return node;
    if (is_assign(peek_token()->type)) {
        ensure_lval(node);
        TOKEN_TYPE assign_opcode = get_token()->type;
        if (assign_opcode == TK_ASSIGN)
            assign_opcode = TK_NOP;
        else
            assign_opcode = (TOKEN_TYPE)((int)assign_opcode - 1);
        VSASTNode *right = read_log_or_expr();
        if (right == NULL) {
            ensure_token(node);
            err("line: %ld, missing right value\n", peek_token()->ln);
        }
        node = assign_expr_node(assign_opcode, node, right);
    }
    return node;
}

VSASTNode *VSParser::read_expr_list() {
    VSASTNode *node = read_assign_expr();
    if (!has_token() || peek_token()->type != TK_COMMA)
        return node;

    VSASTNode *expr_list = expr_list_node(new std::vector<VSASTNode *>());
    if (node != NULL)
        expr_list->expr_list->push_back(node);

    while (peek_token()->type == TK_COMMA) {
        expect(TK_COMMA);
        node = read_assign_expr();
        if (node != NULL)
            expr_list->expr_list->push_back(node);
        ensure_token(expr_list);
    }
    return expr_list;
}

VSASTNode *VSParser::read_expr_stmt() {
    ensure_token(NULL);
    if (peek_token()->type == TK_SEMICOLON) {
        expect(TK_SEMICOLON);
        return NULL;
    }
    VSASTNode *node = read_expr_list();
    expect(TK_SEMICOLON);
    return node;
}

VSASTNode *VSParser::read_stmt() {
    VSASTNode *node = NULL;
    if (has_token()) {
        switch (peek_token()->type) {
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

VSASTNode *VSParser::read_cpd_stmt() {
    expect(TK_L_CURLY);
    VSASTNode *stmt = NULL;
    VSASTNode *cpd_stmt = cpd_stmt_node(new std::vector<VSASTNode *>(), cur_table);

    ensure_token(cpd_stmt);
    while (peek_token()->type != TK_R_CURLY) {
        if (peek_token()->type == TK_VAL || peek_token()->type == TK_VAR) {
            stmt = read_init_decl_stmt();
        } else {
            stmt = read_stmt();
        }

        if (stmt != NULL) {
            cpd_stmt->statements->push_back(stmt);
        }
        ensure_token(cpd_stmt);
    }
    expect(TK_R_CURLY);
    return cpd_stmt;
}

VSASTNode *VSParser::read_for_stmt() {
    expect(TK_FOR);
    expect(TK_L_PAREN);

    enter_loop();
    // create new symbal table for "for" loop
    new_table();

    ensure_token(NULL);
    TOKEN_TYPE type = peek_token()->type;
    VSASTNode *init = NULL, *cond = NULL, *incr = NULL;

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
    if (peek_token()->type != TK_R_PAREN) {
        incr = read_expr_list();
    }
    expect(TK_R_PAREN);

    VSASTNode *for_stmt = for_stmt_node(init, cond, incr, read_cpd_stmt());

    // restore parent table
    restore_table();
    leave_loop();

    return for_stmt;
}

VSASTNode *VSParser::read_func_decl() {
    expect(TK_FUNC);
    Token *token = expect(TK_IDENTIFIER);
    expect(TK_L_PAREN);

    if (cur_table->contains(*token->identifier) || global_table->contains(*token->identifier)) {
        err("line: %ld, duplicated definition of \"%s\"\n", token->ln, token->identifier->c_str());
    }

    // In case of recursive function, we must put the id in the table
    // before entering the function body.
    VSASTNode *ident = ident_node(token->identifier, false);
    cur_table->put(*ident->name, ident);

    enter_func();
    // create symbol table for function
    new_table();

    std::vector<VSASTNode *> *func_params = new std::vector<VSASTNode *>();

    // read func args
    ensure_token(NULL);
    if (peek_token()->type != TK_R_PAREN) {
        std::string *arg_name = expect(TK_IDENTIFIER)->identifier;
        VSASTNode *arg_ident = ident_node(arg_name, true);
        func_params->push_back(arg_ident);
        cur_table->put(*arg_name, arg_ident);

        ensure_token(NULL);
        while (peek_token()->type == TK_COMMA) {
            expect(TK_COMMA);
            arg_name = expect(TK_IDENTIFIER)->identifier;
            arg_ident = ident_node(arg_name, true);
            func_params->push_back(arg_ident);
            cur_table->put(*arg_name, arg_ident);
            ensure_token(NULL);
        }
    }
    expect(TK_R_PAREN);

    VSASTNode *func = func_decl_node(ident, func_params);

    // read func body
    VSASTNode *func_body = read_cpd_stmt();
    if (func_body == NULL) {
        ensure_token(NULL);
        err("line: %ld, missing function body\n", peek_token()->ln);
    }
    func->func_body = func_body;

    // restore parent table
    restore_table();
    leave_func();

    return func;
}

VSASTNode *VSParser::read_elif_stmt() {
    expect(TK_ELIF);
    expect(TK_L_PAREN);

    // create symbol table for elif statement
    new_table();

    // read elif cond
    VSASTNode *cond = read_log_or_expr();

    expect(TK_R_PAREN);

    // read elif body
    VSASTNode *body = read_cpd_stmt();

    // restore parent table
    restore_table();

    return if_stmt_node(cond, body, NULL);
}

VSASTNode *VSParser::read_elif_list() {
    VSASTNode *node = NULL;
    std::vector<VSASTNode *> *elif_list = new std::vector<VSASTNode *>();

    ensure_token(elif_lst_node(elif_list, node));
    while (peek_token()->type == TK_ELIF) {
        node = read_elif_stmt();
        if (node != NULL) {
            elif_list->push_back(node);
            node = NULL;
        }
        ensure_token(elif_lst_node(elif_list, node));
    }

    ensure_token(elif_lst_node(elif_list, node));
    if (peek_token()->type == TK_ELSE) {
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

VSASTNode *VSParser::read_if_stmt() {
    expect(TK_IF);
    expect(TK_L_PAREN);
    VSASTNode *cond = read_log_or_expr();
    expect(TK_R_PAREN);

    new_table();
    VSASTNode *true_stmt = read_cpd_stmt(), *false_stmt = NULL;
    restore_table();

    ensure_token(if_stmt_node(cond, true_stmt, false_stmt));
    if (peek_token()->type == TK_ELIF || peek_token()->type == TK_ELSE)
        false_stmt = read_elif_list();
    return if_stmt_node(cond, true_stmt, false_stmt);
}

VSASTNode *VSParser::read_init_decl(bool is_mutable) {
    VSASTNode *init = NULL;
    Token *token = expect(TK_IDENTIFIER);
    if (cur_table->contains(*token->identifier) || global_table->contains(*token->identifier)) {
        err("line: %ld, duplicated definition of \"%s\"\n", token->ln, token->identifier->c_str());
    }

    VSASTNode *ident = ident_node(token->identifier, is_mutable);
    cur_table->put(*ident->name, ident);

    ensure_token(init_decl_node(ident, init));
    if (peek_token()->type == TK_ASSIGN) {
        expect(TK_ASSIGN);
        init = read_log_or_expr();
    }

    if (!is_mutable && init == NULL) {
        ensure_token(init_decl_node(ident, init));
        err("line: %ld, val declaration must have a initializer\n", peek_token()->ln);
    }

    return init_decl_node(ident, init);
}

VSASTNode *VSParser::read_init_decl_stmt() {
    Token *token = expect2(TK_VAR, TK_VAL);
    VSASTNode *decl_list = init_decl_list_node(new std::vector<VSASTNode *>());
    bool is_mutable = token->type == TK_VAR;
    VSASTNode *decl = read_init_decl(is_mutable);
    if (decl != NULL)
        decl_list->decl_list->push_back(decl);

    ensure_token(decl_list);
    while (peek_token()->type == TK_COMMA) {
        expect(TK_COMMA);
        decl = read_init_decl(is_mutable);
        if (decl != NULL)
            decl_list->decl_list->push_back(decl);
        ensure_token(decl_list);
    }
    expect(TK_SEMICOLON);
    return decl_list;
}

VSASTNode *VSParser::read_while_stmt() {
    expect(TK_WHILE);
    expect(TK_L_PAREN);

    enter_loop();
    new_table();

    VSASTNode *cond = read_log_or_expr();

    expect(TK_R_PAREN);

    if (cond == NULL)
        return NULL;

    VSASTNode *while_stmt = while_stmt_node(cond, read_cpd_stmt());

    restore_table();
    leave_loop();

    return while_stmt;
}

VSASTNode *VSParser::read_program() {
    cur_table = new SymTable<VSASTNode *>(NULL);
    VSASTNode *program = program_node(new std::vector<VSASTNode *>(), cur_table);
    while (has_token()) {
        VSASTNode *stmt = NULL;
        switch (peek_token()->type) {
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

        if (stmt != NULL) {
            program->statements->push_back(stmt);
        }
    }
    return program;
}

VSASTNode *VSParser::parse(std::vector<Token *> *tokens, std::unordered_map<std::string, vs_addr_t> *globals) {
    tk_idx = 0;
    tks = tokens;
    global_table = new SymTable<VSASTNode *>(NULL);
    for (auto entry : *globals) {
        global_table->put(entry.first, ident_node(&(entry.first), false));
    }
    return read_program();
}