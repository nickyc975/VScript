#ifndef VS_TOKENIZER_H
#define VS_TOKENIZER_H

#include "vs.hpp"
#include "objects/VSObject.hpp"

#include <string>

#define IS_NUMBER(c) ((c) >= '0' && (c) <= '9')

#define IS_LETTER(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

#define IS_WORD_CHAR(c) (IS_LETTER(c) || IS_NUMBER(c) || (c) == '_')

#define IS_QUOTE(c) ((c) == '\'' || (c) == '\"')

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
    TK_XOR,
    TK_XOR_ASSIGN,
    TK_OR,
    TK_OR_ASSIGN,
    TK_NOT,
    TK_ASSIGN,

    // declaration keywords
    TK_VAL,
    TK_VAR,
    TK_FUNC,
    TK_LAMBDA,
    TK_RETURN,
    TK_CLASS,
    TK_METH,

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

    // .
    TK_DOT,

    // ;
    TK_SEMICOLON,

    // :
    TK_COLON,
    
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
    "XOR",
    "XOR_ASSIGN",
    "NOT",
    "ASSIGN",
    "VAL",
    "VAR",
    "FUNC",
    "LAMBDA",
    "RETURN",
    "CLASS",
    "METH",
    "IF",
    "ELIF",
    "ELSE",
    "FOR",
    "WHILE",
    "BREAK",
    "CONTINUE",
    "COMMA",
    "DOT",
    "SEMICOLON",
    "COLON",
    "L_PAREN",
    "R_PAREN",
    "L_BRACK",
    "R_BRACK",
    "L_CURLY",
    "R_CURLY",
    "NOP",
    "END"
};

class VSToken : public VSObject {
public:
    TOKEN_TYPE tk_type;
    VSObject *tk_value;
    VSObject *literal;
    long long ln, col;

    VSToken(TOKEN_TYPE tk_type, VSObject *tk_value, VSObject *literal, long long ln, long long col);
    ~VSToken();
};

class VSTokenizer : public VSObject {
private:
    FILE *file;
    VSToken *peek;
    long long ln, col;

    // char level operation
    char getchar();
    int ungetchar();
    char peekchar();
    int seek(int steps);
    static char escape(char c);

    // string level operation
    int getnum(std::string &str);
    int getword(std::string &str);
    int getquoted(std::string &str);
    int getstr(std::string &str, int len);

    VSToken *reco_none(std::string &literal);
    VSToken *reco_bool(std::string &literal);
    VSToken *reco_char(std::string &literal);
    VSToken *reco_num(std::string &literal);
    VSToken *reco_str(std::string &literal);
    VSToken *reco_kwd(std::string &literal);

public:
    VSTokenizer(FILE *file);
    ~VSTokenizer();

    bool hastoken();
    VSToken *gettoken();
    VSToken *peektoken();
};

inline bool is_arith(TOKEN_TYPE opcode)
{
    return opcode >= TK_ADD && opcode <= TK_NEQ;
}

inline bool is_cmp(TOKEN_TYPE opcode)
{
    return opcode >= TK_LT && opcode <= TK_NEQ;
}

inline bool is_logic(TOKEN_TYPE opcode)
{
    return opcode >= TK_EQ && opcode <= TK_OR;
}

inline bool is_assign(TOKEN_TYPE opcode)
{
    switch (opcode)
    {
    case TK_ASSIGN:
    case TK_ADD_ASSIGN:
    case TK_SUB_ASSIGN:
    case TK_MUL_ASSIGN:
    case TK_DIV_ASSIGN:
    case TK_MOD_ASSIGN:
    case TK_AND_ASSIGN:
    case TK_OR_ASSIGN:
        return true;
    default:
        return false;
    }
}

inline bool is_rel(TOKEN_TYPE opcode)
{
    switch (opcode)
    {
    case TK_LT:
    case TK_LE:
    case TK_GT:
    case TK_GE:
        return true;
    default:
        return false;
    }
}

inline bool is_mul(TOKEN_TYPE opcode)
{
    switch (opcode)
    {
    case TK_MUL:
    case TK_DIV:
    case TK_MOD:
        return true;
    default:
        return false;
    }
}

#endif