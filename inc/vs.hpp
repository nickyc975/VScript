#ifndef VS_H
#define VS_H

#include <vector>
#include <string>
#include <unordered_map>

#define is_num_str(type) (type != NONE && type != BOOL)
#define is_num(type) (is_num_str(type) && type != STRING)
#define is_int(type) (is_num(type) && type != FLOAT)

#define CONST_NONE_ADDR 0
#define CONST_TRUE_ADDR 1
#define CONST_FALSE_ADDR 2

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
    OP_NEG,

    // 1 arg, list length, create a list of objects in the stack
    OP_BUILD_LIST,

    // no arg, load the value at index in the list
    OP_INDEX_LOAD,

    // no arg, store the value at index of the list
    OP_INDEX_STORE,

    // no arg append a value at the end of the list
    OP_APPEND,

    // 1 arg, load the local object indicated by the arg
    OP_LOAD_LOCAL,

    // 1 arg, load the object with the name indicated by the arg
    OP_LOAD_NAME,

    // 1 arg, store stack top to local indicated by the arg
    OP_STORE_LOCAL,

    // 1 arg, store stack top to name indicated by the arg
    OP_STORE_NAME,

    // 1 arg, load the object at index in the const vector
    OP_LOAD_CONST,

    // no arg, go to stack top
    OP_GOTO,

    // 1 arg, jump to point in current block
    OP_JMP,

    // 1 arg, jump to point in current block if stack top is true
    OP_JIF,

    // break current loop
    OP_BREAK,

    // goto loop_start of current loop
    OP_CONTINUE,

    // no arg, call stack top
    OP_CALL,

    // no arg, end current block
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
        "NEG",
        "BUILD_LIST",
        "INDEX_LOAD",
        "INDEX_STORE",
        "APPEND",
        "LOAD_LOCAL",
        "LOAD_NAME",
        "STORE_LOCAL",
        "STORE_NAME",
        "LOAD_CONST",
        "GOTO",
        "JMP",
        "JIF",
        "BREAK",
        "CONTINUE",
        "CALL",
        "RET",
        "INPUT",
        "PRINT",
        "NOP"};

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
    OBJ_DATA,
    OBJ_CODE,
    OBJ_LIST
} OBJECT_TYPE;

static char *OBJ_STR[] = {"DATA", "CODE", "LIST"};

typedef enum
{
    NORM_BLK,
    FUNC_BLK,
    LOOP_BLK
} CODE_BLK_TYPE;

static char *CODE_BLK_STR[] = {"NORM", "FUNC", "LOOP"};

class VSValue;
class VSObjectList;
class VSCodeObject;

class VSObject
{
public:
    static vs_id_t id;
    OBJECT_TYPE type;
    const vs_id_t objid;

    union {
        VSValue *value;
        VSObjectList *objlist;
        VSCodeObject *codeblock;
    };

    VSObject();
    VSObject(VSValue *value);
    VSObject(VSObjectList *objlist);
    VSObject(VSCodeObject *codeblock);

    VSObject &operator=(const VSObject &that);

    void incref();
    void decref();
};

class VSMemItem
{
public:
    vs_size_t size;
    vs_size_t refcnt;

    VSMemItem() { refcnt = 0; }
    virtual ~VSMemItem() {}

    virtual const char *to_bytes() { return NULL; }
    virtual const std::string to_string() { return std::string(); }
};

class VSValue : public VSMemItem
{
private:
    VSValue();
    VSValue(vs_bool_t bool_val);

public:
    const VALUE_TYPE type;
    const union {
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
    ~VSValue();

    const char *to_bytes();
    const std::string to_string();

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

class VSObjectList : public VSMemItem
{
public:
    std::vector<VSObject> data;

    VSObjectList();
    ~VSObjectList();

    VSObjectList &operator=(const VSObjectList &that);

    void push(VSObject object);
    VSObject back();
    void pop();

    void put(vs_addr_t pos, VSObject object);
    VSObject get(vs_addr_t pos);
    void remove(vs_addr_t pos);

    vs_size_t length();
    vs_bool_t empty();
    void clear();
};

class VSInst : public VSMemItem
{
public:
    OPCODE opcode;
    // constant addr
    vs_addr_t operand;

    VSInst(OPCODE opcode);
    VSInst(OPCODE opcode, vs_addr_t operand);

    const char *to_bytes();
    const std::string to_string();
};

class VSCodeObject : public VSMemItem
{
public:
    const std::string name;
    const CODE_BLK_TYPE type;

    vs_size_t inst_num;
    vs_size_t const_num;
    vs_size_t arg_num;
    vs_size_t lvar_num;
    vs_size_t nlvar_num;

    // For loop block, this indicates the start point of the loop body.
    vs_addr_t loop_start;

    VSObjectList consts;
    std::vector<VSInst> code;
    std::vector<std::string> local_names;
    std::vector<std::string> non_local_names;
    std::unordered_map<std::string, vs_addr_t> name_to_addr;

    VSCodeObject(std::string name, CODE_BLK_TYPE type);

    void add_inst(VSInst inst);
    void add_const(VSObject *object);
    void add_arg(std::string name);
    void add_local(std::string name);
    void add_non_local(std::string name);
};

#endif