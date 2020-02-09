#ifndef VSC_ASTNODE_H
#define VSC_ASTNODE_H

#include "../vsc.hpp"
#include "Value.hpp"
#include "SymTable.hpp"

typedef enum
{
    AST_CONST,
    AST_IDENT,
    AST_B_EXPR,
    AST_U_EXPR,
    AST_EXPR,
    AST_EXPR_LST,
    AST_DECL,
    AST_DECL_LST,
    AST_ASSIGN,
    AST_LST_VAL,
    AST_LST_IDX,
    AST_FUNC_CALL,
    AST_FUNC_DECL,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_FOR_STMT,
    AST_CONTINUE,
    AST_BREAK,
    AST_CPD_STMT,
    AST_PROGRAM,
    AST_ELIF_LST,
    AST_RETURN,
    AST_PRINT_STMT,
    AST_INPUT_STMT,
    AST_UNKNOW
} ASTNODE_TYPE;

class ASTNode
{
public:
    // AST node type
    ASTNODE_TYPE node_type;
    ASTNODE_TYPE ext_node_type;

    // AST properties
    TYPE type;
    union {
        // identifiers
        struct
        {
            char *name;
            bool is_mutable;
        };

        // constants
        Value *value;

        // binary operate
        struct
        {
            INST b_opcode;
            ASTNode *l_operand;
            ASTNode *r_operand;
        };

        // unary operate
        struct
        {
            INST u_opcode;
            ASTNode *operand;
        };

        // expressions
        std::vector<ASTNode *> *expr_list;

        // declarations
        struct
        {
            ASTNode *var_name;
            ASTNode *init_val;
        };

        // declaration list
        std::vector<ASTNode *> *decl_list; // list of declarations

        // assignments
        struct
        {
            INST assign_opcode;
            ASTNode *assign_var;
            ASTNode *assign_val;
        };

        // list values, function/io args
        std::vector<ASTNode *> *list_vals;

        // list index
        struct
        {
            ASTNode *list_name;
            ASTNode *list_index;
        };

        // function declaration or function call
        struct
        {
            ASTNode *func_name;
            ASTNode *arg_node;
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
            ASTNode *while_stmt;
            ASTNode *while_next;
        };

        // for statements
        struct
        {
            ASTNode *for_init;
            ASTNode *for_cond;
            ASTNode *for_incr;
            ASTNode *for_body;
            ASTNode *for_next;
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

    ASTNode(ASTNODE_TYPE node_type, ASTNODE_TYPE ext_node_type);
    ~ASTNode();
};

#endif