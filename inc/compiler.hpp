#ifndef VS_COMPILER_H
#define VS_COMPILER_H

#include "vs.hpp"
#include "objects/VSObject.hpp"
#include "File.hpp"
#include "error.hpp"
#include "SymTable.hpp"

typedef enum
{
    AST_CONST,
    AST_IDENT,
    AST_LIST_VAL,
    AST_LIST_IDX,
    AST_FUNC_CALL,
    AST_B_EXPR,
    AST_U_EXPR,
    AST_ASSIGN_EXPR,
    AST_EXPR_LST,
    AST_INIT_DECL,
    AST_INIT_DECL_LIST,
    AST_FUNC_DECL,
    AST_ELIF_LIST,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_FOR_STMT,
    AST_CONTINUE,
    AST_BREAK,
    AST_RETURN,
    AST_STMT,
    AST_CPD_STMT,
    AST_PROGRAM
} AST_NODE_TYPE;

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