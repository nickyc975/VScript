#ifndef VS_COMPILER_H
#define VS_COMPILER_H

#include "vs.hpp"
#include "objects/VSObject.hpp"
#include "error.hpp"
#include "SymTable.hpp"



class ASTNode
{
public:
    // AST node type
    AST_NODE_TYPE type;

    union {
        // identifiers
        struct
        {
            std::string *name;
            bool is_mutable;
        };

        // constants
        VSValue *value;

        // value list
        std::vector<ASTNode *> *list_val;

        // list index
        struct
        {
            ASTNode *list_name;
            ASTNode *list_index;
        };

        // function call
        struct
        {
            ASTNode *func;
            ASTNode *arg_list;
        };

        // unary operate
        struct
        {
            TOKEN_TYPE u_opcode;
            ASTNode *operand;
        };

        // binary operate
        struct
        {
            TOKEN_TYPE b_opcode;
            ASTNode *l_operand;
            ASTNode *r_operand;
        };

        // assignment expressions
        struct
        {
            TOKEN_TYPE assign_opcode;
            ASTNode *assign_var;
            ASTNode *assign_val;
        };

        // expressions
        std::vector<ASTNode *> *expr_list;

        // init declarations
        struct
        {
            ASTNode *var_name;
            ASTNode *init_val;
        };

        // init declaration list
        std::vector<ASTNode *> *decl_list;

        // function decalaretion
        struct
        {
            ASTNode *func_name;
            std::vector<ASTNode *> *func_params;
            ASTNode *func_body;
        };

        // if-else statements
        struct
        {
            ASTNode *if_cond;
            ASTNode *true_smt;
            ASTNode *false_smt;
        };

        // elif list
        struct
        {
            std::vector<ASTNode *> *elif_list;
            ASTNode *else_node;
        };

        // while statements
        struct
        {
            ASTNode *while_cond;
            ASTNode *while_body;
        };

        // for statements
        struct
        {
            ASTNode *for_init;
            ASTNode *for_cond;
            ASTNode *for_incr;
            ASTNode *for_body;
        };

        // program or compound statements
        struct
        {
            SymTable<ASTNode *> *symtable;
            std::vector<ASTNode *> *statements;
        };

        // return statements
        ASTNode *ret_val;
    };

    ASTNode(AST_NODE_TYPE type): type(type)
    {
    }
    ~ASTNode()
    {
    }
};

ASTNode *parse(std::vector<Token *> *tokens, std::unordered_map<std::string, vs_addr_t> *globals);
VSCodeObject *gencode(ASTNode *astree, std::unordered_map<std::string, vs_addr_t> *globals);

#endif