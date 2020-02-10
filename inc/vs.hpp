#ifndef VS_H
#define VS_H

#include <vector>
#include <string>
#include <unordered_map>

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
    // no arg, operate top 1 or 2 VSObject in the stack
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,
    OP_EQ,
    OP_NEQ,
    OP_AND,
    OP_OR,
    OP_NOT,

    // 1 arg, list length, create a list of objects in the stack
    OP_BUILD_LIST,

    // no arg, load the value at index in the list
    OP_INDEX_LOAD,

    // no arg, store the value at index of the list
    OP_INDEX_STORE,

    // no arg append a value at the end of the list
    OP_APPEND,

    // 1 arg, load the object with the name at the top of the stack
    OP_LOAD_NAME,

    // 1 arg, load the object at index in the const vector
    OP_LOAD_CONST,

    // 1 arg, store stack top to name
    OP_STORE_NAME,

    // 1 arg, store stack top to const
    OP_STORE_CONST,

    // no arg, jump to stack top
    OP_JMP,

    // no arg, jump to stack second if stack top
    OP_JIF,

    // no arg, call stack top
    OP_CALL,

    // 1 arg, put the arg at stack top
    OP_RET,

    // no arg, read a value from cmd and push it into stack
    OP_INPUT,

    // no arg, print stack stop
    OP_PRINT,
    OP_NOP
} OPCODE;

static char *OPCODE_STR[] =
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
    "LOAD_NAME",
    "LOAD_CONST",
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
    CODE,
    LIST
} OBJECT_TYPE;

static char *OBJ_STR[] = {"DATA", "CODE", "LIST"};

class VSValue;
class VSCodeObject;

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
        VSCodeObject *codeblock;
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

    VSMemItem(){refcnt = 0;}

    virtual char *to_bytes(){return NULL;}
    virtual std::string to_string(){return std::string();}
};

class VSValue: VSMemItem
{
private:
    VSValue();
    VSValue(vs_bool_t bool_val);

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
    VSValue(std::string val);

    char *to_bytes();
    std::string to_string();

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
    const OPCODE opcode;

    // const addr
    const vs_addr_t addr;

    // var name
    const std::string name;

    VSInst(OPCODE opcode, vs_addr_t addr);
    VSInst(OPCODE opcode, std::string name);
    ~VSInst();

    char *to_bytes();
    std::string to_string();
};

class VSCodeObject: VSMemItem
{
public:
    std::string name;
    vs_size_t arg_num;
    std::vector<VSInst> code;
    std::vector<VSObject> consts;
    std::vector<std::string> varnames;

    VSCodeObject();
    ~VSCodeObject();

    char *to_bytes();
    std::string to_string();
};

#endif