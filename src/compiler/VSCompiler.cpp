#include "compiler/VSCompiler.hpp"
#include "error.hpp"
#include "objects/VSListObject.hpp"
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

#define ENTER_FUNC(name)                                \
    do {                                                \
        this->codeobjects.push(new VSCodeObject(name)); \
        this->conststack.push(new name_addr_map());     \
        this->namestack.push(new name_addr_map());      \
        this->symtables.push(new Symtable(NULL));       \
        auto _consts = this->conststack.top();          \
        (*_consts)["__vs_none__"] = 0;                  \
        INCREF(this->symtables.top());                  \
    } while (0);

#define LEAVE_FUNC()                          \
    do {                                      \
        this->codeobjects.pop();              \
        auto consts = this->conststack.top(); \
        this->conststack.pop();               \
        delete consts;                        \
        auto names = this->namestack.top();   \
        this->namestack.pop();                \
        delete names;                         \
        LEAVE_BLK();                          \
    } while (0);

VSCompiler::VSCompiler(name_addr_map *builtins) : builtins(builtins) {
    this->symtables = std::stack<Symtable *>();
    this->codeobjects = std::stack<VSCodeObject *>();
    this->namestack = std::stack<name_addr_map *>();
    this->conststack = std::stack<name_addr_map *>();
    this->breakposes = std::stack<std::vector<vs_addr_t> *>();
    this->continueposes = std::stack<std::vector<vs_addr_t> *>();
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
    std::string value_str = STRING_TO_C_STRING(value_strobj);
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

void VSCompiler::do_store(OPCODE opcode, VSASTNode *lval) {
    Symtable *table = this->symtables.top();
    name_addr_map *names = this->namestack.top();
    VSCodeObject *code = this->codeobjects.top();

    if (opcode != OP_NOP) {
        this->gen_expr(lval);
        code->add_inst(VSInst(opcode));
    }

    switch (lval->node_type) {
        case AST_IDENT: {
            IdentNode *name = (IdentNode *)lval;
            if (!table->contains_recur(name->name)) {
                err("name \"%s\" is not defined locally, so can not be assigned.", 
                    STRING_TO_C_STRING(name->name).c_str());
                terminate(TERM_ERROR);
            }
            SymtableEntry *entry = table->get_recur(name->name);
            if (!IS_LOCAL(entry->sym_type) || entry->sym_type == SYM_VAL) {
                err("name \"%s\" is not defined locally or is immutable, so can not be assigned.", 
                    STRING_TO_C_STRING(name->name).c_str());
                terminate(TERM_ERROR);
            }
            code->add_inst(VSInst(OP_STORE_LOCAL, entry->index));
            break;
        }
        case AST_IDX_EXPR: {
            IdxExprNode *idx_expr = (IdxExprNode *)lval;
            this->gen_expr(idx_expr->index);
            this->gen_expr(idx_expr->obj);
            code->add_inst(VSInst(OP_INDEX_STORE));
            break;
        }
        case AST_DOT_EXPR: {
            DotExprNode *dot_expr = (DotExprNode *)lval;
            this->gen_expr(dot_expr->obj);

            vs_size_t idx;
            std::string &attrname = STRING_TO_C_STRING(dot_expr->attrname->name);
            auto idx_iter = names->find(attrname);
            if (idx_iter == names->end()) {
                idx = code->nnames;
                (*names)[attrname] = code->nnames;
                code->add_name(dot_expr->attrname->name);
            } else {
                idx = idx_iter->second;
            }
            code->add_inst(VSInst(OP_STORE_ATTR, idx));
            break;
        }
        default:
            err("invalid left value");
            terminate(TERM_ERROR);
            break;
    }
}

void VSCompiler::fill_back_break_continue(vs_addr_t loop_start) {
    VSCodeObject *code = this->codeobjects.top();
    std::vector<vs_addr_t> *breaks = this->breakposes.top();
    for (auto break_pos : *breaks)
    {
        code->code[break_pos].operand = code->ninsts;
    }
    this->breakposes.pop();
    delete breaks;

    std::vector<vs_addr_t> *continues = this->continueposes.top();
    for (auto continue_pos : *continues)
    {
        code->code[continue_pos].operand = loop_start;
    }
    this->continueposes.pop();
    delete continues;
}

void VSCompiler::gen_const(VSASTNode *node) {
    VSObject *value = ((ConstNode *)node)->value;
    auto consts = conststack.top();
    VSCodeObject *code = codeobjects.top();
    std::string const_key = get_key(value);
    if (consts->find(const_key) == consts->end()) {
        (*consts)[const_key] = code->nconsts;
        code->add_const(value);
    }
    vs_addr_t index = (*consts)[const_key];
    code->add_inst(VSInst(OP_LOAD_CONST, index));
}

void VSCompiler::gen_ident(VSASTNode *node) {
    Symtable *table = this->symtables.top();
    VSCodeObject *code = this->codeobjects.top();

    IdentNode *ident = (IdentNode *)node;
    if (table->contains_recur(ident->name)) {
        SymtableEntry *entry = table->get_recur(ident->name);
        if (IS_LOCAL(entry->sym_type)) {
            code->add_inst(VSInst(OP_LOAD_LOCAL, entry->index));
        } else if (entry->sym_type == SYM_FREE) {
            code->add_inst(VSInst(OP_LOAD_FREE, entry->index));
        } else if (entry->sym_type == SYM_BUILTIN) {
            code->add_inst(VSInst(OP_LOAD_BUILTIN, entry->index));
        } else {
            entry->sym_type = SYM_FREE;
            entry->index = code->nfreevars;
            code->add_inst(VSInst(OP_LOAD_FREE, entry->index));
        }
    } else {
        SymtableEntry *entry;
        auto idx_iter = this->builtins->find(STRING_TO_C_STRING(ident->name));
        if (idx_iter != this->builtins->end()) {
            entry = new SymtableEntry(SYM_BUILTIN, ident->name, idx_iter->second, 0);
            code->add_inst(VSInst(OP_LOAD_BUILTIN, entry->index));
        } else {
            entry = new SymtableEntry(SYM_FREE, ident->name, code->nfreevars, 0);
            code->add_inst(VSInst(OP_LOAD_FREE, entry->index));
            code->add_freevar(ident->name);
        }
        table->put(ident->name, entry);
    }
}

void VSCompiler::gen_b_expr(VSASTNode *node) {
    VSCodeObject *code = this->codeobjects.top();
    BOPNode *bop_expr = (BOPNode *)node;
    this->gen_expr(bop_expr->r_operand);
    this->gen_expr(bop_expr->l_operand);
    code->add_inst(VSInst(get_b_op(bop_expr->opcode)));
}

void VSCompiler::gen_u_expr(VSASTNode *node) {
    VSCodeObject *code = this->codeobjects.top();
    UOPNode *uop_expr = (UOPNode *)node;
    this->gen_expr(uop_expr->operand);
    switch (uop_expr->opcode) {
        case TK_SUB:
            code->add_inst(VSInst(OP_NEG));
            break;
        case TK_NOT:
            code->add_inst(VSInst(OP_NOT));
            break;
        default:
            break;
    }
}

void VSCompiler::gen_idx_expr(VSASTNode *node) {
    VSCodeObject *code = this->codeobjects.top();
    IdxExprNode *idx_expr = (IdxExprNode *)node;
    this->gen_expr(idx_expr->index);
    this->gen_expr(idx_expr->obj);
    code->add_inst(VSInst(OP_INDEX_LOAD));
}

void VSCompiler::gen_tuple_decl(VSASTNode *node) {
    VSCodeObject *code = this->codeobjects.top();
    TupleDeclNode *tuple = (TupleDeclNode *)node;

    // tranverse the tuple from end to start
    int index = tuple->values.size() - 1;
    while (index >= 0) {
        this->gen_expr(tuple->values[index]);
        index--;
    }
    code->add_inst(VSInst(OP_BUILD_TUPLE, tuple->values.size()));
}

void VSCompiler::gen_list_decl(VSASTNode *node) {
    VSCodeObject *code = this->codeobjects.top();
    ListDeclNode *list = (ListDeclNode *)node;

    // tranverse the list from end to start
    int index = list->values.size() - 1;
    while (index >= 0) {
        this->gen_expr(list->values[index]);
        index--;
    }
    code->add_inst(VSInst(OP_BUILD_LIST, list->values.size()));
}

void VSCompiler::gen_dict_decl(VSASTNode *node) {
    VSCodeObject *code = this->codeobjects.top();
    DictDeclNode *dict = (DictDeclNode *)node;

    for (auto value : dict->values) {
        PairExprNode *pair_expr = (PairExprNode *)value;
        this->gen_expr(pair_expr->value);
        this->gen_expr(pair_expr->key);
        code->add_inst(VSInst(OP_BUILD_TUPLE, 2));
    }
    code->add_inst(VSInst(OP_BUILD_DICT, dict->values.size()));
}

void VSCompiler::gen_set_decl(VSASTNode *node) {
    VSCodeObject *code = this->codeobjects.top();
    SetDeclNode *set = (SetDeclNode *)node;

    // tranverse the list from end to start
    for (auto value : set->values) {
        this->gen_expr(value);
    }
    code->add_inst(VSInst(OP_BUILD_SET, set->values.size()));
}

void VSCompiler::gen_func_call(VSASTNode *node) {
    VSCodeObject *code = this->codeobjects.top();
    FuncCallNode *funccall = (FuncCallNode *)node;
    
    if (funccall->args == NULL) {
        err("internal error: func->args is NULL");
        terminate(TERM_ERROR);
    }
    if (funccall->args->node_type != AST_TUPLE_DECL) {
        this->gen_expr(funccall->args);
        code->add_inst(VSInst(OP_BUILD_TUPLE, 1));
    } else {
        this->gen_tuple_decl(funccall->args);
    }
    this->gen_expr(funccall->func);
    code->add_inst(VSInst(OP_CALL_FUNC));
}

void VSCompiler::gen_return(VSASTNode *node) {
    VSCodeObject *code = this->codeobjects.top();
    ReturnStmtNode *ret = (ReturnStmtNode *)node;

    // set none as the default return value
    if (ret->retval == NULL) {
        code->add_inst(VSInst(OP_LOAD_CONST, 0));
    } else {
        this->gen_expr(ret->retval);
    }
    code->add_inst(VSInst(OP_RET));
}

void VSCompiler::gen_break(VSASTNode *node) {
    VSCodeObject *code = this->codeobjects.top();
    auto breaks = this->breakposes.top();
    breaks->push_back(code->ninsts);
    code->add_inst(VSInst(OP_JMP, 0));
}

void VSCompiler::gen_continue(VSASTNode *node) {
    VSCodeObject *code = this->codeobjects.top();
    auto continues = this->continueposes.top();
    continues->push_back(code->ninsts);
    code->add_inst(VSInst(OP_JMP, 0));
}

void VSCompiler::gen_expr(VSASTNode *node) {
    switch (node->node_type) {
        case AST_CONST:
            this->gen_const(node);
            break;
        case AST_IDENT:
            this->gen_ident(node);
            break;
        case AST_TUPLE_DECL:
            this->gen_tuple_decl(node);
            break;
        case AST_LIST_DECL:
            this->gen_list_decl(node);
            break;
        case AST_DICT_DECL:
            this->gen_dict_decl(node);
            break;
        case AST_SET_DECL:
            this->gen_set_decl(node);
            break;
        case AST_IDX_EXPR:
            this->gen_idx_expr(node);
            break;
        case AST_FUNC_CALL:
            this->gen_func_call(node);
            break;
        case AST_B_OP_EXPR:
            this->gen_b_expr(node);
            break;
        case AST_U_OP_EXPR:
            this->gen_u_expr(node);
            break;
        case AST_FUNC_DECL:
            this->gen_func_decl(node);
            break;
        default:
            break;
    }
}

void VSCompiler::gen_expr_list(VSASTNode *node) {
    if (node->node_type == AST_EXPR_LST) {
        ExprListNode *expr_list = (ExprListNode *)node;
        for (auto expr : expr_list->values) {
            this->gen_assign_expr(expr);
        }
        return;
    }
    this->gen_assign_expr(node);
}

void VSCompiler::gen_decl_stmt(VSASTNode *node) {
    Symtable *table = this->symtables.top();
    VSCodeObject *code = this->codeobjects.top();

    InitDeclListNode *decl_list = (InitDeclListNode *)node;
    SYM_TYPE sym_type = decl_list->specifier == TK_VAR ? SYM_VAR : SYM_VAL;

    // "decl" is decl node, contains ident and init
    for (auto decl : decl_list->decls) {
        SymtableEntry *entry;
        VSObject *name_obj = decl->name->name;
        std::string &name_str = STRING_TO_C_STRING(name_obj);

        if (table->contains(name_obj)) {
            entry = table->get(name_obj);
            if (entry->sym_type != SYM_UNDEFINED) {
                err("duplicated definition of name: \"%s\"", name_str.c_str());
                terminate(TERM_ERROR);
            }
            entry->sym_type = sym_type;
            entry->index = code->nlvars;
        } else {
            entry = new SymtableEntry(sym_type, name_obj, code->nlvars, 0);
            table->put(name_obj, entry);
        }

        if (decl_list->specifier == TK_VAL && decl->init_val == NULL) {
            err("name: \"%s\" is defined as val but not assigned with any value", name_str.c_str());
            terminate(TERM_ERROR);
        }

        code->add_lvar(name_obj);
        // Assign value.
        if (decl->init_val != NULL) {
            this->gen_expr(decl->init_val);
            code->add_inst(VSInst(OP_STORE_LOCAL, entry->index));
        }
    }
}

void VSCompiler::gen_assign_expr(VSASTNode *node) {
    if (node->node_type == AST_ASSIGN_EXPR) {
        AssignExprNode *assign = (AssignExprNode *)node;
        this->gen_expr(assign->rval);
        do_store(get_b_op(assign->opcode), assign->lval);
    } else {
        this->gen_expr(node);
        VSCodeObject *code = codeobjects.top();
        code->add_inst(VSInst(OP_POP));
    }
}

void VSCompiler::gen_for_stmt(VSASTNode *node) {
    VSCodeObject *code = codeobjects.top();
    ForStmtNode *for_stmt = (ForStmtNode *)node;

    this->breakposes.push(new std::vector<vs_addr_t>());
    this->continueposes.push(new std::vector<vs_addr_t>());

    ENTER_BLK();

    if (for_stmt->init != NULL) {
        if (for_stmt->init->node_type == AST_INIT_DECL_LIST) {
            this->gen_decl_stmt(for_stmt->init);
        } else {
            this->gen_expr_list(for_stmt->init);
        }
    }

    vs_addr_t loop_start = code->ninsts;

    if (for_stmt->cond != NULL) {
        this->gen_expr(for_stmt->cond);
    } else {
        err("missing for condition");
        terminate(TERM_ERROR);
    }

    vs_addr_t jif_pos = code->ninsts;
    code->add_inst(VSInst(OP_JIF, jif_pos + 2));
    code->add_inst(VSInst(OP_JMP, 0));

    this->gen_cpd_stmt(for_stmt->body);

    if (for_stmt->incr != NULL) {
        this->gen_expr_list(for_stmt->incr);
    }

    code->add_inst(VSInst(OP_JMP, loop_start));
    // jmp is the following inst of jif, so jif + 1 is the pos of jmp.
    code->code[jif_pos + 1].operand = code->ninsts;

    this->fill_back_break_continue(loop_start);

    LEAVE_BLK();
}

void VSCompiler::gen_build_func(VSCodeObject *code, bool anonymous) {
    Symtable *p_table = this->symtables.top();
    VSCodeObject *p_code = this->codeobjects.top();

    for (vs_size_t i = 0; i < code->nfreevars; i++) {
        SymtableEntry *entry;
        VSObject *freevar = LIST_GET(code->freevars, i);
        if (p_table->contains(freevar)) {
            // free var is defined in parent code object's symtable.
            entry = p_table->get(freevar);
            if (!entry->is_cell) {
                entry->cell_index = p_code->ncellvars;
                p_code->add_cellvar(freevar);
                entry->is_cell = true;
            }
        } else {
            // freevar is currently not defined.
            entry = new SymtableEntry(SYM_UNDEFINED, freevar, 0, p_code->ncellvars);
            p_table->put(freevar, entry);
            p_code->add_cellvar(freevar);
            entry->is_cell = true;
        }
        p_code->add_inst(VSInst(OP_LOAD_CELL, entry->cell_index));
    }

    p_code->add_inst(VSInst(OP_BUILD_TUPLE, code->nfreevars));
    p_code->add_inst(VSInst(OP_LOAD_CONST, p_code->nconsts));
    p_code->add_inst(VSInst(OP_BUILD_FUNC));
    if (!anonymous) {
        p_code->add_inst(VSInst(OP_STORE_LOCAL, p_code->nlvars));
    }
}

void VSCompiler::gen_func_decl(VSASTNode *node) {
    static VSObject *ANONYMOUS_FUNC_NAME = C_STRING_TO_STRING("<__anonymous_function__>");

    Symtable *p_table = this->symtables.top();
    VSCodeObject *p_code = this->codeobjects.top();

    FuncDeclNode *func = (FuncDeclNode *)node;

    bool anonymous = func->name == NULL;
    // Add function name to parent code locals.
    VSObject *name = anonymous ? ANONYMOUS_FUNC_NAME : func->name->name;
    if (p_table->contains(name)) {
        err("duplicated definition of name: \"%s\"", STRING_TO_C_STRING(name).c_str());
        terminate(TERM_ERROR);
    }
    // create symtable entry
    p_table->put(name, new SymtableEntry(SYM_VAR, name, p_code->nlvars, 0));

    ENTER_FUNC(name);

    Symtable *table = this->symtables.top();
    // Add function code to parent code consts.
    VSCodeObject *code = this->codeobjects.top();

    // Add function arg names to function locals.
    for (auto argnode : func->args) {
        VSObject *argname;
        if (argnode->node_type == AST_IDENT) {
            argname = ((IdentNode *)argnode)->name;
        } else {
            argname = ((InitDeclNode *)argnode)->name->name;
        }
        table->put(argname, new SymtableEntry(SYM_ARG, argname, code->nlvars, 0));
        code->add_arg(argname);
    }

    // should set up cell vars first, so jump.
    vs_size_t start_pos = code->ninsts;
    code->add_inst(VSInst(OP_JMP, 0));

    // gen function body.
    this->gen_cpd_stmt(func->body);

    // default return none
    code->add_inst(VSInst(OP_LOAD_CONST, 0));
    code->add_inst(VSInst(OP_RET));

    // set up cell vars
    code->code[start_pos].operand = code->ninsts;
    for (vs_size_t i = 0; i < code->ncellvars; i++) {
        VSObject *cellvar = LIST_GET(code->cellvars, i);
        if (!table->contains(cellvar)) {
            err("internal error: cellvar \"%s\" is used by not defined", 
                STRING_TO_C_STRING(cellvar).c_str());
            terminate(TERM_ERROR);
        }

        SymtableEntry *entry = table->get(cellvar);
        if (IS_LOCAL(entry->sym_type)) {
            // cell var is defined locally.
            code->add_inst(VSInst(OP_LOAD_LOCAL_CELL, entry->index));
        } else if (entry->sym_type == SYM_FREE) {
            // cell var is a free var of current code object.
            code->add_inst(VSInst(OP_LOAD_FREE_CELL, entry->index));
        } else {
            // cell var is undefined, which means it is used by inner functions 
            // but not used or defined by current function it self. It should be a
            // free var of current function.
            entry->sym_type = SYM_FREE;
            entry->index = code->nfreevars;
            code->add_freevar(cellvar);
            code->add_inst(VSInst(OP_LOAD_FREE_CELL, entry->index));
        }
        code->add_inst(VSInst(OP_STORE_CELL, i));
    }

    // jump back to the function body start point
    code->add_inst(VSInst(OP_JMP, start_pos + 1));

    LEAVE_FUNC();

    // Add instructions to build function
    gen_build_func(code, anonymous);

    if (!anonymous) {
        p_code->add_lvar(code->name);
    }
    p_code->add_const(code);
}

void VSCompiler::gen_elif_list(VSASTNode *node) {
    VSCodeObject *code = codeobjects.top();
    auto jmp_pos = std::vector<vs_size_t>();
    ElifListNode *elif_list = (ElifListNode *)node;

    for (auto child : elif_list->elifs) {
        if (child->cond != NULL) {
            this->gen_expr(child->cond);
        } else {
            err("missing elif condition");
            terminate(TERM_ERROR);
        }

        code->add_inst(VSInst(OP_NOT));

        vs_addr_t jif_pos = code->ninsts;
        code->add_inst(VSInst(OP_JIF, 0));

        ENTER_BLK();
        this->gen_cpd_stmt(child->truestmt);
        LEAVE_BLK();

        jmp_pos.push_back(code->ninsts);
        code->add_inst(VSInst(OP_JMP, 0));

        code->code[jif_pos].operand = code->ninsts;
    }

    if (elif_list->elsestmt != NULL) {
        ENTER_BLK()
        this->gen_cpd_stmt(elif_list->elsestmt);
        LEAVE_BLK();
    }

    for (auto pos : jmp_pos) {
        code->code[pos].operand = code->ninsts;
    }
}

void VSCompiler::gen_if_stmt(VSASTNode *node) {
    VSCodeObject *code = codeobjects.top();
    IfStmtNode *if_stmt = (IfStmtNode *)node;

    if (if_stmt->cond != NULL) {
        this->gen_expr(if_stmt);
    } else {
        err("missing if condition");
        terminate(TERM_ERROR);
    }

    // if cond is true, do not jump to elif or else
    // if cond is false, jump to elif or else
    code->add_inst(VSInst(OP_NOT));

    vs_size_t jif_pos = code->ninsts;
    code->add_inst(VSInst(OP_JIF, 0));

    ENTER_BLK();
    this->gen_cpd_stmt(if_stmt->truestmt);
    LEAVE_BLK();

    // finished true stmt of if, jump to end of if-elif-else stmt
    vs_size_t jmp_pos = code->ninsts;
    code->add_inst(VSInst(OP_JMP, 0));

    // here is the begaining of elif and else
    code->code[jif_pos].operand = code->ninsts;

    if (if_stmt->falsestmt != NULL) {
        this->gen_elif_list(if_stmt->falsestmt);
    }

    code->code[jmp_pos].operand = code->ninsts;
}

void VSCompiler::gen_while_stmt(VSASTNode *node) {
    VSCodeObject *code = codeobjects.top();
    WhileStmtNode *while_stmt = (WhileStmtNode *)node;

    this->breakposes.push(new std::vector<vs_addr_t>());
    this->continueposes.push(new std::vector<vs_addr_t>());

    int loop_start = 0;
    if (while_stmt->cond != NULL) {
        this->gen_expr(while_stmt->cond);
    } else {
        err("missing while condition");
        terminate(TERM_ERROR);
    }

    vs_addr_t jif_pos = code->ninsts;
    code->add_inst(VSInst(OP_JIF, jif_pos + 2));
    code->add_inst(VSInst(OP_JMP, 0));

    ENTER_BLK();
    this->gen_cpd_stmt(while_stmt->body);
    LEAVE_BLK();

    code->add_inst(VSInst(OP_JMP, loop_start));
    code->code[jif_pos + 1].operand = code->ninsts;

    this->fill_back_break_continue(loop_start);
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

    ENTER_FUNC(C_STRING_TO_STRING("__main__"));

    Symtable *table = this->symtables.top();
    VSCodeObject *program = this->codeobjects.top();

    // should set up cell vars first, so jump.
    vs_size_t start_pos = program->ninsts;
    program->add_inst(VSInst(OP_JMP, 0));

    // generate top level code object.
    VSASTNode *astree = parser->parse();
    this->gen_cpd_stmt(astree);

    bool error = false;

    // top level code object should not have free vars
    for (vs_size_t i = 0; i < program->nfreevars; i++) {
        VSObject *freevar = LIST_GET(program->freevars, i);
        err("name: \"%s\" is not defined", STRING_TO_C_STRING(freevar).c_str());
        error = true;
    }

    // all cell vars in top level code object should be locally defined.
    for (vs_size_t i = 0; i < program->ncellvars; i++) {
        VSObject *cellvar = LIST_GET(program->cellvars, i);
        if (!table->contains(cellvar)) {
            err("internal error: cell var \"%s\" is used but not defined", 
                STRING_TO_C_STRING(cellvar).c_str());
            error = true;
        } else {
            SymtableEntry *entry = table->get(cellvar);
            if (entry->sym_type == SYM_FREE || entry->sym_type == SYM_UNDEFINED) {
                err("name: \"%s\" is not defined", STRING_TO_C_STRING(cellvar).c_str());
                error = true;
            }
        }
    }

    if (error) {
        terminate(TERM_ERROR);
    }

    // set up cell vars
    program->code[start_pos].operand = program->ninsts;
    for (vs_size_t i = 0; i < program->ncellvars; i++) {
        VSObject *cellvar = LIST_GET(program->cellvars, i);
        program->add_inst(VSInst(OP_LOAD_LOCAL_CELL, table->get(cellvar)->index));
        program->add_inst(VSInst(OP_STORE_CELL, i));
    }

    // jump back to the function body start point
    program->add_inst(VSInst(OP_JMP, start_pos + 1));

    LEAVE_FUNC();

    DECREF(parser);

    return program;
}
