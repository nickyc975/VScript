#ifndef VS_H
#define VS_H

typedef unsigned int vs_id_t;
typedef unsigned int vs_size_t;
typedef unsigned int vs_addr_t;

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
    I_BLD_LST,
    I_INDEX,
    I_APPEND,
    I_LOAD,
    I_STORE,
    I_JMP,
    I_JIF,
    I_CALL,
    I_RET,
    I_INPUT,
    I_PRINT,
    I_NOP
} INST_TYPE;

static char *INST_STR[] =
{
    "ADD",
    "SUB",
    "MUL",
    "DIV",
    "MOD",
    "LT",
    "GT",
    "LE",
    "GE",
    "EQ",
    "NEQ",
    "AND",
    "OR",
    "NOT",
    "BLD_LIST",
    "INDEX",
    "APPEND",
    "LOAD",
    "STORE",
    "JMP",
    "JIF",
    "CALL",
    "RET",
    "INPUT",
    "PRINT",
    "NOP"
};

typedef enum
{
    NONE,
    BOOL,
    CHAR,
    INT,
    FLOAT,
    STRING,
    LIST,
    FUNC
} VALUE_TYPE;

static char *TYPE_STR[] = {"NONE", "BOOL", "CHAR", "INT", "FLOAT", "STRING", "LIST", "FUNC"};

class VSObject
{
private:
    static vs_id_t id;

public:
    vs_id_t obj_id;
    vs_size_t ref_cnt;

    VSObject();
    ~VSObject();

    virtual std::string to_bytes();
};

class VSValue : VSObject
{
private:
    VSValue();
    static VSValue *NONE_VAL;
public:
    const VALUE_TYPE type;
    const union
    {
        bool bool_val;
        char char_val;
        long long int_val;
        long double float_val;
        std::string *str_val;
        std::vector<VSObject *> *list_val;
    };

    VSValue(bool val);
    VSValue(char val);
    VSValue(long long val);
    VSValue(long double val);
    VSValue(std::string *val);
    VSValue(std::vector<VSObject *> *val);

    static VSValue *none();
    static VSValue *i_add(VSValue *left, VSValue *right);
    static VSValue *i_sub(VSValue *left, VSValue *right);
    static VSValue *i_mul(VSValue *left, VSValue *right);
    static VSValue *i_div(VSValue *left, VSValue *right);
    static VSValue *i_mod(VSValue *left, VSValue *right);
    static VSValue *i_lt(VSValue *left, VSValue *right);
    static VSValue *i_gt(VSValue *left, VSValue *right);
    static VSValue *i_le(VSValue *left, VSValue *right);
    static VSValue *i_ge(VSValue *left, VSValue *right);
    static VSValue *i_eq(VSValue *left, VSValue *right);
    static VSValue *i_neq(VSValue *left, VSValue *right);
    static VSValue *i_and(VSValue *left, VSValue *right);
    static VSValue *i_or(VSValue *left, VSValue *right);
    static VSValue *i_not(VSValue *val);
    static VSValue *i_neg(VSValue *val);
    static VSValue *i_idx(VSValue *list, VSValue *index);
    static VSValue *i_idx(VSValue *list, VSValue *index, VSValue *new_val);
    static VSValue *i_apd(VSValue *list, VSValue *new_val);
};

class VSInst : VSObject
{
public:
    INST_TYPE inst;
    vs_addr_t operand;

    VSInst(INST_TYPE inst, vs_addr_t operand);
    ~VSInst();
};

class VSCodeBlock : VSObject
{
public:
    std::string name;
    vs_size_t arg_num;
    vs_size_t lvar_num;
    std::vector<VSInst> code;
    std::vector<VSValue *> consts;
    std::vector<VSCodeBlock *> blocks;
    std::vector<std::string> varnames;

    VSCodeBlock();
    ~VSCodeBlock();
};

#endif