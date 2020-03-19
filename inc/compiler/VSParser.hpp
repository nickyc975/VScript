#ifndef VS_PARSER_H
#define VS_PARSER_H

#include "vs.hpp"
#include "compiler/VSTokenizer.hpp"
#include "compiler/VSASTNode.hpp"
#include "objects/VSObject.hpp"

#include <stack>

class VSParser : VSObject {
private:
    VSTokenizer *tokenizer;
    unsigned int inloop, infunc;
    std::stack<unsigned int> inloop_stack;

    void expect(int ntypes, ...);
    VSASTNode *read_tuple_decl_or_expr();
    VSASTNode *read_list_decl();
    VSASTNode *read_dict_or_set_decl();
    VSASTNode *read_dot_expr();
    VSASTNode *read_primary_expr();
    VSASTNode *read_postfix_expr();
    VSASTNode *read_unary_expr();
    VSASTNode *read_mul_expr();
    VSASTNode *read_add_expr();
    VSASTNode *read_rel_expr();
    VSASTNode *read_eql_expr();
    VSASTNode *read_log_and_expr();
    VSASTNode *read_log_xor_expr();
    VSASTNode *read_log_or_expr();
    VSASTNode *read_assign_expr();
    VSASTNode *read_expr_list();
    VSASTNode *read_expr_stmt();
    VSASTNode *read_init_decl();
    VSASTNode *read_init_decl_stmt();
    VSASTNode *read_func_decl();
    VSASTNode *read_class_decl();
    VSASTNode *read_meth_decl();
    VSASTNode *read_elif();
    VSASTNode *read_elif_list();
    VSASTNode *read_if_stmt();
    VSASTNode *read_while_stmt();
    VSASTNode *read_for_stmt();
    VSASTNode *read_stmt();
    VSASTNode *read_cpd_stmt();
    VSASTNode *read_program();

public:
    VSParser(VSTokenizer *tokenizer);
    ~VSParser();

    VSASTNode *parse();
};

#endif