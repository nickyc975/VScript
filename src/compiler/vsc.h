#include <stdio.h>
#include <stdlib.h>
#include <vector>

#ifndef VSC_DEFINE

#define VSC_DEFINE

typedef enum
{
    ADD,
    ADD_ASSIGN,
    AND,
    AND_ASSIGN,
    ASSIGN,
    BREAK,
    COMMA,
    CONSTANT,
    CONTINUE,
    DIV,
    DIV_ASSIGN,
    ELIF,
    ELSE,
    EQ,
    FOR,
    FUNC,
    GE,
    GT,
    IDENTIFIER,
    IF,
    INPUT,
    LE,
    LT,
    L_BRACK,
    L_CURLY,
    L_PAREN,
    MOD,
    MOD_ASSIGN,
    MUL,
    MUL_ASSIGN,
    NEQ,
    NOT,
    OR,
    OR_ASSIGN,
    PRINT,
    RETURN,
    R_BRACK,
    R_CURLY,
    R_PAREN,
    SEMICOLON,
    SUB,
    SUB_ASSIGN,
    VAL,
    VAR,
    WHILE,
    END
} KIND;

typedef enum
{
    NONE,
    BOOL,
    CHAR,
    INT,
    FLOAT,
    STRING,
    LIST,
    FUNC_T
} TYPE;

typedef enum
{
    I_ADD,
    I_SUB,
    I_MUL,
    I_DIV,
    I_MOD,
    I_LT,
    I_GT,
    I_LE,
    I_GE,
    I_EQ,
    I_NEQ,
    I_AND,
    I_OR,
    I_NOT,
    I_ASSIGN,
    I_PUSH,
    I_POP,
    I_JMP,
    I_JIF,
    I_CALL,
    I_RET,
    I_INPUT,
    I_PRINT,
    I_NOP
} INST;

static char CLASS_STR[][15] = {
    "ADD", "ADD_ASSIGN", "AND", "AND_ASSIGN", "ASSIGN", "BREAK", "COMMA", "CONSTANT", "CONTINUE", 
    "DIV", "DIV_ASSIGN", "ELIF", "ELSE", "EQ", "FOR", "FUNC", "GE", "GT", "IDENTIFIER", "IF", "INPUT", "LE", "LT", 
    "L_BRACK", "L_CURLY", "L_PAREN", "MOD", "MOD_ASSIGN", "MUL", "MUL_ASSIGN", "NEQ", "NOT", "OR", 
    "OR_ASSIGN", "PRINT", "RETURN", "R_BRACK", "R_CURLY", "R_PAREN", "SEMICOLON", "SUB", "SUB_ASSIGN", "VAL", 
    "VAR", "WHILE", "#"
};

static char TYPE_STR[][10] = {"NONE", "BOOL", "CHAR", "INT", "FLOAT", "STRING", "LIST", "FUNC"};

class Value
{
public:
    TYPE type;
    union {
        bool bool_val;
        char char_val;
        long long int_val;
        double float_val;

        struct
        {
            char *str_val;
            unsigned int str_len;
        };

        struct
        {
            unsigned int list_len;
            std::vector<Value *> *list_val;
        };

        struct
        {
            unsigned int arg_num;
            unsigned int func_entry;
        };
    };
    bool is_mutable;

    Value(TYPE type) : type(type)
    {
    }

    ~Value()
    {
        if (this->type == STRING && this->str_val != NULL)
        {
            free(this->str_val);
        }
        else if (this->type == LIST && this->list_val != NULL)
        {
            for (Value *value : *(this->list_val))
            {
                delete value;
            }
            delete this->list_val;
        }
    }
};

#endif