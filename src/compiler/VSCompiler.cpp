#include "compiler/VSCompiler.hpp"
#include "objects/VSStringObject.hpp"

#define enter_blk(name, type)                                             \
    codestack.push(new VSCodeObject(name, type));                         \
    conststack.push(new std::unordered_map<std::string, vs_addr_t>());    \
    (*conststack.top())["__vs_none__"] = 0;                               \
    (*conststack.top())["__vs_bool_true__"] = 1;                          \
    (*conststack.top())["__vs_bool_false__"] = 2;                         \
    nonlocalstack.push(new std::unordered_map<std::string, vs_addr_t>()); \

#define leave_blk()            \
    codestack.pop();           \
    delete conststack.top();   \
    conststack.pop();          \
    delete nonlocalstack.top(); \
    nonlocalstack.pop();

VSCompiler::VSCompiler(name_addr_map *builtins) : builtins(builtins) {
    this->symtables = std::stack<Symtable *>();
    this->codeobjects = std::stack<VSCodeObject *>();
    this->conststack = std::stack<name_addr_map *>();
}

VSCompiler::~VSCompiler() {

}

OPCODE VSCompiler::get_b_op(TOKEN_TYPE tk)
{
    switch (tk)
    {
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

std::string VSCompiler::get_key(VSObject *value)
{
    VSTypeObject *type = VS_TYPEOF(value);
    VSObject *value_strobj = type->__str__(value);
    std::string value_str = vs_string_to_cstring(value_strobj);
    DECREF(value_strobj);

    switch (type->t_type)
    {
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
//         gen_expr(assign_var);
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
//         gen_expr(assign_var->list_index);
//         gen_expr(assign_var->list_name);
//         cur->add_inst(VSInst(OP_INDEX_STORE));
//     }
// }

void VSCompiler::gen_const(VSASTNode *node)
{
    VSObject *value = ((ConstNode *)node)->value;
    auto consts = conststack.top();
    VSCodeObject *cur = codeobjects.top();
    std::string const_key = get_key(value);
    if (consts->find(const_key) == consts->end())
    {
        (*consts)[const_key] = cur->nconsts;
        cur->add_const(value);
    }
    vs_addr_t index = (*consts)[const_key];
    cur->add_inst(VSInst(OP_LOAD_CONST, index));
}

void VSCompiler::gen_ident(VSASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    auto nonlocals = nonlocalstack.top();

    std::string *name = node->name;
    if (globalsyms->find(*name) != globalsyms->end())
    {
        cur->add_inst(VSInst(OP_LOAD_GLOBAL, (*globalsyms)[*name]));
    }
    else if (cur->name_to_addr.find(*name) != cur->name_to_addr.end())
    {
        cur->add_inst(VSInst(OP_LOAD_LOCAL, cur->name_to_addr[*name]));
    }
    else
    {
        if (nonlocals->find(*name) == nonlocals->end())
        {
            cur->add_non_local(*name);
            (*nonlocals)[*name] = cur->nlvar_num - 1;
        }
        cur->add_inst(VSInst(OP_LOAD_NAME, (*nonlocals)[*name]));
    }
}

void VSCompiler::gen_b_expr(VSASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    gen_expr(node->r_operand);
    gen_expr(node->l_operand);
    cur->add_inst(VSInst(get_b_op(node->b_opcode)));
}

void VSCompiler::gen_u_expr(VSASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    gen_expr(node->operand);
    switch (node->u_opcode)
    {
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

void VSCompiler::gen_idx_expr(VSASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    gen_expr(node->list_index);
    gen_expr(node->list_name);
    cur->add_inst(VSInst(OP_INDEX_LOAD));
}

void VSCompiler::gen_list_decl(VSASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    // tranverse the list from end to start
    int index = node->list_val->size() - 1;
    while (index >= 0)
    {
        gen_expr(node->list_val->at(index));
        index--;
    }
    cur->add_inst(VSInst(OP_BUILD_LIST, node->list_val->size()));
}

void VSCompiler::gen_func_call(VSASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    gen_list_val(node->arg_list);
    gen_expr(node->func);
    cur->add_inst(VSInst(OP_CALL));
}

void VSCompiler::gen_return(VSASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    // set none as the default return value
    if (node->ret_val == NULL)
        cur->add_inst(VSInst(OP_LOAD_CONST, CONST_NONE_ADDR));
    else
        gen_expr(node->ret_val);
    cur->add_inst(VSInst(OP_RET));
}

void VSCompiler::gen_break(VSASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    cur->add_inst(VSInst(OP_BREAK));
}

void VSCompiler::gen_continue(VSASTNode *node)
{
    VSCodeObject *cur = codestack.top();
    cur->add_inst(VSInst(OP_CONTINUE));
}

void VSCompiler::gen_expr(VSASTNode *node)
{
    switch (node->type)
    {
    case AST_CONST:
        gen_const(node);
        break;
    case AST_IDENT:
        gen_ident(node);
        break;
    case AST_LIST_VAL:
        gen_list_val(node);
        break;
    case AST_LIST_IDX:
        gen_list_idx(node);
        break;
    case AST_FUNC_CALL:
        gen_func_call(node);
        break;
    case AST_B_EXPR:
        gen_b_expr(node);
        break;
    case AST_U_EXPR:
        gen_u_expr(node);
        break;
    default:
        break;
    }
}

void VSCompiler::gen_expr_list(VSASTNode *node)
{
    if (node->type == AST_EXPR_LST)
    {
        for (auto expr : *node->expr_list)
        {
            gen_assign_expr(expr);
        }
        return;
    }
    gen_assign_expr(node);
}

void VSCompiler::gen_decl_stmt(VSASTNode *node)
{
    VSCodeObject *cur = codeobjects.top();
    Symtable *symtable = symtables.top();

    InitDeclListNode *decl_list = (InitDeclListNode *)node;
    // "child" is decl node, contains ident and init
    for (auto child : decl_list->decls)
    {
        std::string name = child->name->name;
        if (cur->name_to_addr.find(name) == cur->name_to_addr.end())
        {
            cur->add_local(*name);
        }

        // Assign value.
        if (child->init_val != NULL)
        {
            gen_expr(child->init_val);
            cur->add_inst(VSInst(OP_STORE_LOCAL, cur->name_to_addr[*name]));
        }
    }
}

void VSCompiler::gen_assign_expr(VSASTNode *node)
{
    if (node->type == AST_ASSIGN_EXPR)
    {
        gen_expr(node->assign_val);
        do_store(get_b_op(node->assign_opcode), node->assign_var);
    }
    else
    {
        gen_expr(node);
        VSCodeObject *cur = codestack.top();
        cur->add_inst(VSInst(OP_POP));
    }
}

void VSCompiler::gen_for_stmt(VSASTNode *node)
{
    VSCodeObject *cur, *parent = codestack.top();
    parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
    parent->add_inst(VSInst(OP_EVAL));

    enter_blk("__vs_for__", LOOP_BLK);

    cur = codestack.top();
    parent->add_const(new VSObject(cur));

    if (node->for_init != NULL)
    {
        if (node->for_init->type == AST_INIT_DECL_LIST)
        {
            gen_decl_stmt(node->for_init);
        }
        else
        {
            gen_expr_list(node->for_init);
        }
    }

    cur->loop_start = cur->inst_num;

    if (node->for_cond != NULL)
    {
        gen_expr(node->for_cond);
    }
    else
    {
        cur->add_inst(VSInst(OP_LOAD_CONST, CONST_TRUE_ADDR));
    }

    vs_addr_t jif_pos = cur->inst_num;
    cur->add_inst(VSInst(OP_JIF, jif_pos + 2));
    cur->add_inst(VSInst(OP_JMP, 0));

    gen_cpd_stmt(node->for_body);

    if (node->for_incr != NULL)
    {
        gen_expr_list(node->for_incr);
    }

    cur->add_inst(VSInst(OP_JMP, cur->loop_start));
    cur->code[jif_pos + 1].operand = cur->inst_num;
    cur->add_inst(VSInst(OP_NOP));

    leave_blk();
}

void VSCompiler::gen_func_decl(VSASTNode *node)
{
    VSCodeObject *parent = codestack.top();

    // Add function name to parent code locals.
    std::string *name = node->func_name->name;
    if (parent->name_to_addr.find(*name) == parent->name_to_addr.end())
    {
        parent->add_local(*name);
    }
    parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
    parent->add_inst(VSInst(OP_STORE_LOCAL, parent->name_to_addr[*name]));

    enter_blk(*name, FUNC_BLK);

    // Add function code to parent code consts.
    VSCodeObject *cur = codestack.top();
    parent->add_const(new VSObject(cur));

    // Add function arg names to function locals.
    for (auto arg : *node->func_params)
    {
        cur->add_arg(*arg->name);
    }

    // for overflowed args
    cur->add_local("...");

    // Gen function body.
    gen_cpd_stmt(node->func_body);

    // default return none
    cur->add_inst(VSInst(OP_LOAD_CONST, CONST_NONE_ADDR));
    cur->add_inst(VSInst(OP_RET));

    leave_blk();
}

void VSCompiler::gen_elif_list(VSASTNode *node)
{
    auto jmp_pos = std::vector<vs_size_t>();
    VSCodeObject *cur, *parent = codestack.top();
    for (auto child : *node->elif_list)
    {
        if (child->if_cond != NULL)
        {
            gen_expr(child->if_cond);
        }
        else
        {
            parent->add_inst(VSInst(OP_LOAD_CONST, CONST_TRUE_ADDR));
        }

        parent->add_inst(VSInst(OP_NOT));
        parent->add_inst(VSInst(OP_JIF, parent->inst_num + 4));
        parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
        parent->add_inst(VSInst(OP_EVAL));

        jmp_pos.push_back(parent->inst_num);
        parent->add_inst(VSInst(OP_JMP, 0));

        enter_blk("__vs_elif__", NORM_BLK);

        cur = codestack.top();
        parent->add_const(new VSObject(cur));

        gen_cpd_stmt(child->true_smt);

        leave_blk();
    }

    if (node->else_node != NULL)
    {
        parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
        parent->add_inst(VSInst(OP_EVAL));

        enter_blk("__vs_else__", NORM_BLK);

        cur = codestack.top();
        parent->add_const(new VSObject(cur));

        gen_cpd_stmt(node->else_node);

        leave_blk();
    }

    for (auto pos : jmp_pos)
    {
        parent->code[pos].operand = parent->inst_num;
    }
}

void VSCompiler::gen_if_stmt(VSASTNode *node)
{
    VSCodeObject *cur, *parent = codestack.top();
    if (node->if_cond != NULL)
    {
        gen_expr(node->if_cond);
    }
    else
    {
        parent->add_inst(VSInst(OP_LOAD_CONST, CONST_TRUE_ADDR));
    }

    parent->add_inst(VSInst(OP_NOT));
    parent->add_inst(VSInst(OP_JIF, parent->inst_num + 4));
    parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
    parent->add_inst(VSInst(OP_EVAL));

    vs_size_t jmp_pos = parent->inst_num;
    parent->add_inst(VSInst(OP_JMP, 0));

    enter_blk("__vs_if__", NORM_BLK);

    cur = codestack.top();
    parent->add_const(new VSObject(cur));

    gen_cpd_stmt(node->true_smt);

    leave_blk();

    if (node->false_smt != NULL)
    {
        gen_elif_list(node->false_smt);
    }

    parent->code[jmp_pos].operand = parent->inst_num;
    parent->add_inst(VSInst(OP_NOP));
}

void VSCompiler::gen_while_stmt(VSASTNode *node)
{
    VSCodeObject *cur, *parent = codestack.top();
    parent->add_inst(VSInst(OP_LOAD_CONST, parent->const_num));
    parent->add_inst(VSInst(OP_EVAL));

    enter_blk("__vs_while__", LOOP_BLK);

    cur = codestack.top();
    parent->add_const(new VSObject(cur));

    cur->loop_start = 0;
    if (node->while_cond != NULL)
    {
        gen_expr(node->for_init);
    }
    else
    {
        cur->add_inst(VSInst(OP_LOAD_CONST, CONST_TRUE_ADDR));
    }

    vs_addr_t jif_pos = cur->inst_num;
    cur->add_inst(VSInst(OP_JIF, jif_pos + 2));
    cur->add_inst(VSInst(OP_JMP, 0));

    gen_cpd_stmt(node->while_body);

    cur->add_inst(VSInst(OP_JMP, cur->loop_start));
    cur->code[jif_pos + 1].operand = cur->inst_num;
    cur->add_inst(VSInst(OP_NOP));

    leave_blk();
}

void VSCompiler::gen_cpd_stmt(VSASTNode *node)
{
    for (auto stmt : *node->statements)
    {
        switch (stmt->type)
        {
        case AST_INIT_DECL_LIST:
            gen_decl_stmt(stmt);
            break;
        case AST_FUNC_DECL:
            gen_func_decl(stmt);
            break;
        case AST_IF_STMT:
            gen_if_stmt(stmt);
            break;
        case AST_FOR_STMT:
            gen_for_stmt(stmt);
            break;
        case AST_WHILE_STMT:
            gen_while_stmt(stmt);
            break;
        case AST_ASSIGN_EXPR:
            gen_assign_expr(stmt);
            break;
        case AST_RETURN:
            gen_return(stmt);
            break;
        case AST_BREAK:
            gen_break(stmt);
            break;
        case AST_CONTINUE:
            gen_continue(stmt);
            break;
        case AST_EXPR_LST:
        default:
            gen_expr_list(stmt);
            break;
        }
    }
}

VSCodeObject *gencode(VSASTNode *astree, std::unordered_map<std::string, vs_addr_t> *globals)
{
    globalsyms = globals;
    codestack = std::stack<VSCodeObject *>();
    conststack = std::stack<std::unordered_map<std::string, vs_addr_t> *>();
    nonlocalstack = std::stack<std::unordered_map<std::string, vs_addr_t> *>();

    enter_blk("__vs_main__", NORM_BLK);

    gen_cpd_stmt(astree);

    auto program = codestack.top();

    leave_blk();

    return program;
}
