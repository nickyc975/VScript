#include "error.hpp"
#include "compiler/VSCompiler.hpp"
#include "objects/VSStringObject.hpp"

#define ENTER_BLK()                                                                  \
    do {                                                                             \
        Symtable *curtable = this->symtables.empty() ? NULL : this->symtables.top(); \
        this->symtables.push(new Symtable(curtable));                                \
        INCREF(this->symtables.top());                                               \
    } while (0);

#define LEAVE_BLK()                                 \
    do {                                            \
        Symtable *curtable = this->symtables.top(); \
        this->symtables.pop();                      \
        DECREF_EX(curtable);                        \
    } while (0);

#define ENTER_FUNC(name)                                   \
    do {                                                   \
        VSObject *nameobj = vs_string_from_cstring(name);  \
        this->codeobjects.push(new VSCodeObject(nameobj)); \
        this->conststack.push(new name_addr_map());        \
        DECREF_EX(nameobj);                                \
        ENTER_BLK();                                       \
    } while (0);

#define LEAVE_FUNC()                          \
    do {                                      \
        this->codeobjects.pop();              \
        auto consts = this->conststack.top(); \
        this->conststack.pop();               \
        delete consts;                        \
        LEAVE_BLK();                          \
    } while (0);

VSCompiler::VSCompiler(name_addr_map *builtins) : builtins(builtins) {
    this->symtables = std::stack<Symtable *>();
    this->codeobjects = std::stack<VSCodeObject *>();
    this->conststack = std::stack<name_addr_map *>();
}

VSCompiler::~VSCompiler() {
}

OPCODE VSCompiler::get_b_op(TOKEN_TYPE tk) {
    switch (tk) {
        case TK_ADD:
            return OP_ADD;
        case TK_SUB:
            return OP_SUB;
        case TK_MUL:
            return OP_MUL;
        case TK_DIV:
            return OP_DIV;
        case TK_MOD:
            return OP_MOD;
        case TK_LT:
            return OP_LT;
        case TK_GT:
            return OP_GT;
        case TK_LE:
            return OP_LE;
        case TK_GE:
            return OP_GE;
        case TK_EQ:
            return OP_EQ;
        case TK_NEQ:
            return OP_NEQ;
        case TK_AND:
            return OP_AND;
        case TK_XOR:
            return OP_XOR;
        case TK_OR:
            return OP_OR;
        default:
            return OP_NOP;
    }
}

std::string VSCompiler::get_key(VSObject *value) {
    VSTypeObject *type = VS_TYPEOF(value);
    VSObject *value_strobj = type->__str__(value);
    std::string value_str = vs_string_to_cstring(value_strobj);
    DECREF(value_strobj);

    switch (type->t_type) {
        case T_NONE:
            return "__vs_none__";
        case T_BOOL:
            return "__vs_bool_" + value_str + "__";
        case T_CHAR:
            return "__vs_char_" + value_str + "__";
        case T_INT:
            return "__vs_int_" + value_str + "__";
        case T_FLOAT:
            return "__vs_float_" + value_str + "__";
        case T_STR:
            return "__vs_str_" + value_str + "__";
        default:
            return value_str;
    }
}

// static void do_store(OPCODE opcode, VSASTNode *assign_var)
// {
//     VSCodeObject *cur = codestack.top();
//     auto nonlocals = nonlocalstack.top();

//     if (opcode != OP_NOP)
//     {
//         this->gen_expr(assign_var);
//         cur->add_inst(VSInst(opcode));
//     }

//     if (assign_var->type == AST_IDENT)
//     {
//         std::string *name = assign_var->name;
//         if (cur->name_to_addr.find(*name) != cur->name_to_addr.end())
//         {
//             cur->add_inst(VSInst(OP_STORE_LOCAL, cur->name_to_addr[*name]));
//         }
//         else
//         {
//             if (nonlocals->find(*name) == nonlocals->end())
//             {
//                 cur->add_non_local(*name);
//                 (*nonlocals)[*name] = cur->nlvar_num - 1;
//             }
//             cur->add_inst(VSInst(OP_STORE_NAME, (*nonlocals)[*name]));
//         }
//     }
//     else if (assign_var->type == AST_LIST_IDX)
//     {
//         this->gen_expr(assign_var->list_index);
//         this->gen_expr(assign_var->list_name);
//         cur->add_inst(VSInst(OP_INDEX_STORE));
//     }
// }

void VSCompiler::gen_const(VSASTNode *node) {
    VSObject *value = ((ConstNode *)node)->value;
    auto consts = conststack.top();
    VSCodeObject *cur = codeobjects.top();
    std::string const_key = get_key(value);
    if (consts->find(const_key) == consts->end()) {
        (*consts)[const_key] = cur->nconsts;
        cur->add_const(value);
    }
    vs_addr_t index = (*consts)[const_key];
    cur->add_inst(VSInst(OP_LOAD_CONST, index));
}

void VSCompiler::gen_ident(VSASTNode *node) {
    VSCodeObject *cur = codestack.top();
    auto nonlocals = nonlocalstack.top();

    std::string *name = node->name;
    if (globalsyms->find(*name) != globalsyms->end()) {
        cur->add_inst(VSInst(OP_LOAD_GLOBAL, (*globalsyms)[*name]));
    } else if (cur->name_to_addr.find(*name) != cur->name_to_addr.end()) {
        cur->add_inst(VSInst(OP_LOAD_LOCAL, cur->name_to_addr[*name]));
    } else {
        if (nonlocals->find(*name) == nonlocals->end()) {
            cur->add_non_local(*name);
            (*nonlocals)[*name] = cur->nlvar_num - 1;
        }
        cur->add_inst(VSInst(OP_LOAD_NAME, (*nonlocals)[*name]));
    }
}

void VSCompiler::gen_b_expr(VSASTNode *node) {
    VSCodeObject *cur = codestack.top();
    this->gen_expr(node->r_operand);
    this->gen_expr(node->l_operand);
    cur->add_inst(VSInst(get_b_op(node->b_opcode)));
}

void VSCompiler::gen_u_expr(VSASTNode *node) {
    VSCodeObject *cur = codestack.top();
    this->gen_expr(node->operand);
    switch (node->u_opcode) {
        case TK_SUB:
            cur->add_inst(VSInst(OP_NEG));
            break;
        case TK_NOT:
            cur->add_inst(VSInst(OP_NOT));
            break;
        default:
            break;
    }
}

void VSCompiler::gen_idx_expr(VSASTNode *node) {
    VSCodeObject *cur = codestack.top();
    this->gen_expr(node->list_index);
    this->gen_expr(node->list_name);
    cur->add_inst(VSInst(OP_INDEX_LOAD));
}

void VSCompiler::gen_list_decl(VSASTNode *node) {
    VSCodeObject *cur = codestack.top();
    // tranverse the list from end to start
    int index = node->list_val->size() - 1;
    while (index >= 0) {
        this->gen_expr(node->list_val->at(index));
        index--;
    }
    cur->add_inst(VSInst(OP_BUILD_LIST, node->list_val->size()));
}

void VSCompiler::gen_func_call(VSASTNode *node) {
    VSCodeObject *cur = codestack.top();
    this->gen_list_val(node->arg_list);
    this->gen_expr(node->func);
    cur->add_inst(VSInst(OP_CALL));
}

void VSCompiler::gen_return(VSASTNode *node) {
    VSCodeObject *cur = codestack.top();
    // set none as the default return value
    if (node->ret_val == NULL)
        cur->add_inst(VSInst(OP_LOAD_CONST, CONST_NONE_ADDR));
    else
        this->gen_expr(node->ret_val);
    cur->add_inst(VSInst(OP_RET));
}

void VSCompiler::gen_break(VSASTNode *node) {
    VSCodeObject *cur = codestack.top();
    cur->add_inst(VSInst(OP_BREAK));
}

void VSCompiler::gen_continue(VSASTNode *node) {
    VSCodeObject *cur = codestack.top();
    cur->add_inst(VSInst(OP_CONTINUE));
}

void VSCompiler::gen_expr(VSASTNode *node) {
    switch (node->type) {
        case AST_CONST:
            this->gen_const(node);
            break;
        case AST_IDENT:
            this->gen_ident(node);
            break;
        case AST_LIST_VAL:
            this->gen_list_val(node);
            break;
        case AST_LIST_IDX:
            this->gen_list_idx(node);
            break;
        case AST_FUNC_CALL:
            this->gen_func_call(node);
            break;
        case AST_B_EXPR:
            this->gen_b_expr(node);
            break;
        case AST_U_EXPR:
            this->gen_u_expr(node);
            break;
        default:
            break;
    }
}

void VSCompiler::gen_expr_list(VSASTNode *node) {
    if (node->type == AST_EXPR_LST) {
        for (auto expr : *node->expr_list) {
            this->gen_assign_expr(expr);
        }
        return;
    }
    this->gen_assign_expr(node);
}

void VSCompiler::gen_decl_stmt(VSASTNode *node) {
    VSCodeObject *cur = codeobjects.top();
    Symtable *symtable = symtables.top();

    InitDeclListNode *decl_list = (InitDeclListNode *)node;
    // "child" is decl node, contains ident and init
    for (auto child : decl_list->decls) {
        std::string name = child->name->name;
        if (cur->name_to_addr.find(name) == cur->name_to_addr.end()) {
            cur->add_local(*name);
        }

        // Assign value.
        if (child->init_val != NULL) {
            this->gen_expr(child->init_val);
            cur->add_inst(VSInst(OP_STORE_LOCAL, cur->name_to_addr[*name]));
        }
    }
}

void VSCompiler::gen_assign_expr(VSASTNode *node) {
    if (node->type == AST_ASSIGN_EXPR) {
        this->gen_expr(node->assign_val);
        do_store(get_b_op(node->assign_opcode), node->assign_var);
    } else {
        this->gen_expr(node);
        VSCodeObject *cur = codestack.top();
        cur->add_inst(VSInst(OP_POP));
    }
}

void VSCompiler::gen_for_stmt(VSASTNode *node) {
    VSCodeObject *cur = codeobjects.top();
    ForStmtNode *for_stmt = (ForStmtNode *)node;

    ENTER_BLK();
    if (for_stmt->init != NULL) {
        if (for_stmt->init->node_type == AST_INIT_DECL_LIST) {
            this->gen_decl_stmt(for_stmt->init);
        } else {
            this->gen_expr_list(for_stmt->init);
        }
    }

    vs_addr_t loop_start = cur->ninsts;

    if (for_stmt->cond != NULL) {
        this->gen_expr(for_stmt->cond);
    } else {
        err("missing for condition");
        terminate(TERM_ERROR);
    }

    vs_addr_t jif_pos = cur->ninsts;
    cur->add_inst(VSInst(OP_JIF, jif_pos + 2));
    cur->add_inst(VSInst(OP_JMP, 0));

    this->gen_cpd_stmt(for_stmt->body);

    if (for_stmt->incr != NULL) {
        this->gen_expr_list(for_stmt->incr);
    }

    cur->add_inst(VSInst(OP_JMP, loop_start));
    // jmp is the following inst of jif, so jif + 1 is the pos of jmp.
    cur->code[jif_pos + 1].operand = cur->ninsts;

    LEAVE_BLK();
}

void VSCompiler::gen_func_decl(VSASTNode *node) {
    VSCodeObject *parent = codestack.top();

    // Add function name to parent code locals.
    std::string *name = node->func_name->name;
    if (parent->name_to_addr.find(*name) == parent->name_to_addr.end()) {
        parent->add_local(*name);
    }
    parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
    parent->add_inst(VSInst(OP_STORE_LOCAL, parent->name_to_addr[*name]));

    ENTER_BLK(*name, FUNC_BLK);

    // Add function code to parent code consts.
    VSCodeObject *cur = codestack.top();
    parent->add_const(new VSObject(cur));

    // Add function arg names to function locals.
    for (auto arg : *node->func_params) {
        cur->add_arg(*arg->name);
    }

    // for overflowed args
    cur->add_local("...");

    // this->gen function body.
    this->gen_cpd_stmt(node->func_body);

    // default return none
    cur->add_inst(VSInst(OP_LOAD_CONST, CONST_NONE_ADDR));
    cur->add_inst(VSInst(OP_RET));

    LEAVE_BLK();
}

void VSCompiler::gen_elif_list(VSASTNode *node) {
    VSCodeObject *cur = codeobjects.top();
    auto jmp_pos = std::vector<vs_size_t>();
    ElifListNode *elif_list = (ElifListNode *)node;

    for (auto child : elif_list->elifs) {
        if (child->cond != NULL) {
            this->gen_expr(child->cond);
        } else {
            err("missing elif condition");
            terminate(TERM_ERROR);
        }

        cur->add_inst(VSInst(OP_NOT));

        vs_addr_t jif_pos = cur->ninsts;
        cur->add_inst(VSInst(OP_JIF,  0));

        ENTER_BLK();
        this->gen_cpd_stmt(child->truestmt);
        LEAVE_BLK();

        jmp_pos.push_back(cur->ninsts);
        cur->add_inst(VSInst(OP_JMP, 0));

        cur->code[jif_pos].operand = cur->ninsts;
    }

    if (elif_list->elsestmt != NULL) {
        ENTER_BLK()
        this->gen_cpd_stmt(elif_list->elsestmt);
        LEAVE_BLK();
    }

    for (auto pos : jmp_pos) {
         cur->code[pos].operand =  cur-> ninsts;
    }
}

void VSCompiler::gen_if_stmt(VSASTNode *node) {
    VSCodeObject *cur = codeobjects.top();
    IfStmtNode *if_stmt = (IfStmtNode *)node;

    if (if_stmt->cond != NULL) {
        this->gen_expr(if_stmt);
    } else {
        err("missing if condition");
        terminate(TERM_ERROR);
    }

    // if cond is true, do not jump to elif or else
    // if cond is false, jump to elif or else
    cur->add_inst(VSInst(OP_NOT));

    vs_size_t jif_pos = cur->ninsts;
    cur->add_inst(VSInst(OP_JIF, 0));

    ENTER_BLK();
    this->gen_cpd_stmt(if_stmt->truestmt);
    LEAVE_BLK();

    // finished true stmt of if, jump to end of if-elif-else stmt
    vs_size_t jmp_pos = cur->ninsts;
    cur->add_inst(VSInst(OP_JMP, 0));

    // here is the begaining of elif and else
    cur->code[jif_pos].operand = cur->ninsts;

    if (if_stmt->falsestmt != NULL) {
        this->gen_elif_list(if_stmt->falsestmt);
    }

    cur->code[jmp_pos].operand = cur->ninsts;
}

void VSCompiler::gen_while_stmt(VSASTNode *node) {
    VSCodeObject *cur = codeobjects.top();
    WhileStmtNode *while_stmt = (WhileStmtNode *)node;

    int loop_start = 0;
    if (while_stmt->cond != NULL) {
        this->gen_expr(while_stmt->cond);
    } else {
        err("missing while condition");
        terminate(TERM_ERROR);
    }

    vs_addr_t jif_pos = cur->ninsts;
    cur->add_inst(VSInst(OP_JIF, jif_pos + 2));
    cur->add_inst(VSInst(OP_JMP, 0));

    ENTER_BLK();
    this->gen_cpd_stmt(while_stmt->body);
    LEAVE_BLK();

    cur->add_inst(VSInst(OP_JMP, loop_start));
    cur->code[jif_pos + 1].operand = cur->ninsts;
}

void VSCompiler::gen_cpd_stmt(VSASTNode *node) {
    CpdStmtNode *cpd_stmt = (CpdStmtNode *)node;
    for (auto stmt : cpd_stmt->values) {
        switch (stmt->node_type) {
            case AST_INIT_DECL_LIST:
                this->gen_decl_stmt(stmt);
                break;
            case AST_FUNC_DECL:
                this->gen_func_decl(stmt);
                break;
            case AST_CLASS_DECL:
                // TODO: implement gen_class_decl()
                break;
            case AST_METH_DECL:
                // TODO: implement gen_meth_decl()
                break;
            case AST_IF_STMT:
                this->gen_if_stmt(stmt);
                break;
            case AST_FOR_STMT:
                this->gen_for_stmt(stmt);
                break;
            case AST_WHILE_STMT:
                this->gen_while_stmt(stmt);
                break;
            case AST_ASSIGN_EXPR:
                this->gen_assign_expr(stmt);
                break;
            case AST_RETURN:
                this->gen_return(stmt);
                break;
            case AST_BREAK:
                this->gen_break(stmt);
                break;
            case AST_CONTINUE:
                this->gen_continue(stmt);
                break;
            case AST_EXPR_LST:
            default:
                this->gen_expr_list(stmt);
                break;
        }
    }
}

VSCodeObject *VSCompiler::compile(std::string filename) {
    VSTokenizer *tokenizer = new VSTokenizer(fopen(filename.c_str(), "r"));
    VSParser *parser = new VSParser(tokenizer);

    INCREF(parser);

    ENTER_FUNC("__main__");

    this->gen_cpd_stmt(parser->parse());

    auto program = codeobjects.top();

    LEAVE_FUNC();

    DECREF(parser);

    return program;
}
