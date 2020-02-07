#ifndef VSC_DEFINE

#define VSC_DEFINE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

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
    FUNC_T,
    UNKNOWN
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

static char INST_STR[][10] = {
    "ADD", "SUB", "MUL", "DIV", "MOD", "LT", "GT", "LE", "GE", "EQ", 
    "NEQ", "AND", "OR", "NOT", "ASSIGN", "PUSH", "POP", "JMP", "JIF", "CALL", "RET", "INPUT", 
    "PRINT", "NOP"
};

#endif