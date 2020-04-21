#ifndef VS_COMPILER_H
#define VS_COMPILER_H

#include <stack>
#include <unordered_map>
#include <vector>

#include "compiler/Symtable.hpp"
#include "compiler/VSASTNode.hpp"
#include "compiler/VSParser.hpp"
#include "compiler/VSTokenizer.hpp"
#include "objects/VSCodeObject.hpp"
#include "objects/VSObject.hpp"

class VSCompiler : public VSObject {
private:
    name_addr_map *builtins;
    std::stack<Symtable *> symtables;
    std::stack<VSCodeObject *> codeobjects;
    std::stack<name_addr_map *> namestack;
    std::stack<name_addr_map *> conststack;
    std::stack<std::vector<vs_addr_t> *> breakposes;
    std::stack<std::vector<vs_addr_t> *> continueposes;

    void do_store(OPCODE opcode, VSASTNode *lval);
    void fill_back_break_continue(vs_addr_t loop_start);
    void set_up_cellvars();
    void gen_build_func(VSCodeObject *code, bool anonymous);
    void gen_const(VSASTNode *node);
    void gen_ident(VSASTNode *node);
    void gen_tuple_decl(VSASTNode *node);
    void gen_list_decl(VSASTNode *node);
    void gen_dict_decl(VSASTNode *node);
    void gen_set_decl(VSASTNode *node);
    void gen_b_expr(VSASTNode *node);
    void gen_u_expr(VSASTNode *node);
    void gen_idx_expr(VSASTNode *node);
    void gen_dot_expr(VSASTNode *node);
    void gen_func_call(VSASTNode *node);
    void gen_return(VSASTNode *node);
    void gen_break(VSASTNode *node);
    void gen_continue(VSASTNode *node);
    void gen_expr(VSASTNode *node);
    void gen_expr_list(VSASTNode *node);
    void gen_decl_stmt(VSASTNode *node);
    void gen_assign_expr(VSASTNode *node);
    void gen_cpd_stmt(VSASTNode *node);
    void gen_for_stmt(VSASTNode *node);
    void gen_func_decl(VSASTNode *node);
    void gen_elif_list(VSASTNode *node);
    void gen_if_stmt(VSASTNode *node);
    void gen_while_stmt(VSASTNode *node);

    static OPCODE get_b_op(TOKEN_TYPE tk);
    static std::string get_key(VSObject *value);

public:
    VSCompiler(name_addr_map *builtins);
    ~VSCompiler();

    VSCodeObject *compile(std::string filename);
};

#endif