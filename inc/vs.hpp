#ifndef VS_H
#define VS_H

#define is_num_str(type) (type != NONE && type != BOOL)
#define is_num(type) (is_num_str(type) && type != STRING)
#define is_int(type) (is_num(type) && type != FLOAT)

typedef bool vs_bool_t;
typedef char vs_char_t;
typedef long long vs_int_t;
typedef long double vs_float_t;
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
    STRING
} VALUE_TYPE;

static char *TYPE_STR[] = {"NONE", "BOOL", "CHAR", "INT", "FLOAT", "STRING"};

typedef enum
{
    DATA,
    FUNC,
    LIST
} OBJECT_TYPE;

static char *OBJ_STR[] = {"DATA", "CODE", "LIST"};

class VSValue;
class VSFunction;

class VSObject
{
private:
    static vs_id_t id;

public:
    vs_id_t obj_id;
    OBJECT_TYPE type;

    union
    {
        VSValue *value;
        VSFunction *function;
        std::vector<VSObject> *obj_list;
    };

    VSObject();
    ~VSObject();
};

class VSMemItem
{
public:
    vs_size_t size;
    vs_size_t refcnt;

    VSMemItem()
    {
        refcnt = 0;
    }

    ~VSMemItem()
    {
    }

    virtual char *to_bytes();
    virtual std::string to_string();
};

class VSValue: VSMemItem
{
private:
    VSValue();
    VSValue(vs_bool_t bool_val);
    static VSValue *NONE_VAL, *TRUE_VAL, *FALSE_VAL;

public:
    const VALUE_TYPE type;
    const union
    {
        const vs_bool_t bool_val;
        const vs_char_t char_val;
        const vs_int_t int_val;
        const vs_float_t float_val;
        const std::string *str_val;
    };

    VSValue(vs_char_t val);
    VSValue(vs_int_t val);
    VSValue(vs_float_t val);
    VSValue(std::string *val);

    static VSValue *None();
    static VSValue *True();
    static VSValue *False();
    static VSValue *copy(VSValue *old);
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
};

class VSInst: VSMemItem
{
public:
    INST_TYPE inst;
    vs_addr_t operand;

    VSInst(INST_TYPE inst, vs_addr_t operand);
    ~VSInst();
};

class VSFunction: VSMemItem
{
public:
    std::string name;
    vs_size_t arg_num;
    vs_size_t lvar_num;
    std::vector<VSInst> code;
    std::vector<VSObject> consts;
    std::vector<std::string> varnames;

    VSFunction();
    ~VSFunction();
};

#endif