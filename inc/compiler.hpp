#ifndef VS_COMPILER_H
#define VS_COMPILER_H

#include "vs.hpp"
#include "File.hpp"
#include "error.hpp"
#include "SymTable.hpp"

typedef enum
{
    // constant
    TK_CONSTANT,
    
    // identifier
    TK_IDENTIFIER,

    // operators
    TK_ADD,
    TK_ADD_ASSIGN,
    TK_SUB,
    TK_SUB_ASSIGN,
    TK_MUL,
    TK_MUL_ASSIGN,
    TK_DIV,
    TK_DIV_ASSIGN,
    TK_MOD,
    TK_MOD_ASSIGN,
    TK_GE,
    TK_GT,
    TK_LE,
    TK_LT,
    TK_EQ,
    TK_NEQ,
    TK_AND,
    TK_AND_ASSIGN,
    TK_OR,
    TK_OR_ASSIGN,
    TK_NOT,
    TK_ASSIGN,

    // declaration keywords
    TK_VAL,
    TK_VAR,
    TK_FUNC,
    TK_RETURN,

    // if statement keywords
    TK_IF,
    TK_ELIF,
    TK_ELSE,
    
    // for loop
    TK_FOR,

    // while loop
    TK_WHILE,
    
    // jump keywords
    TK_BREAK,
    TK_CONTINUE,
    
    // ,
    TK_COMMA,

    // ;
    TK_SEMICOLON,
    
    // ()
    TK_L_PAREN,
    TK_R_PAREN,
    
    // []
    TK_L_BRACK,
    TK_R_BRACK,

    // {}
    TK_L_CURLY,
    TK_R_CURLY,

    // end
    TK_NOP,
    TK_END
} TOKEN_TYPE;

static char *TOKEN_STR[] = 
{
    "CONSTANT",
    "IDENTIFIER",
    "ADD",
    "ADD_ASSIGN",
    "SUB",
    "SUB_ASSIGN",
    "MUL",
    "MUL_ASSIGN",
    "DIV",
    "DIV_ASSIGN",
    "MOD",
    "MOD_ASSIGN",
    "GE",
    "GT",
    "LE",
    "LT",
    "EQ",
    "NEQ",
    "AND",
    "AND_ASSIGN",
    "OR",
    "OR_ASSIGN",
    "NOT",
    "ASSIGN",
    "VAL",
    "VAR",
    "FUNC",
    "RETURN",
    "IF",
    "ELIF",
    "ELSE",
    "FOR",
    "WHILE",
    "BREAK",
    "CONTINUE",
    "COMMA",
    "SEMICOLON",
    "L_PAREN",
    "R_PAREN",
    "L_BRACK",
    "R_BRACK",
    "L_CURLY",
    "R_CURLY",
    "NOP",
    "END"
};

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

class Token
{
public:
    // token properties
    TOKEN_TYPE type;
    long long ln, col;

    // token content
    std::string *identifier;
    VSValue *value;

    Token(long long ln, long long col) : ln(ln), col(col)
    {
    }

    ~Token()
    {
    }
};

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

void tokenize(File *file, std::vector<Token *> &tokens);
ASTNode *parse(std::vector<Token *> *tokens);
VSCodeObject *gencode(ASTNode *astree);

#endif