#ifndef VS_OBJECTS
#define VS_OBJECTS

#include "vs.hpp"
#include "code.hpp"

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
    OBJ_TYPE,
    OBJ_NONE,
    OBJ_BOOL,
    OBJ_CHAR,
    OBJ_INT,
    OBJ_FLOAT,
    OBJ_STR,
    OBJ_CODE,
    OBJ_LIST,
    OBJ_FUNC
} OBJECT_TYPE;

static char *OBJ_STR[] = {"DATA", "CODE", "LIST"};

typedef enum
{
    NORM_BLK,
    FUNC_BLK,
    LOOP_BLK
} CODE_BLK_TYPE;

static char *CODE_BLK_STR[] = {"NORM", "FUNC", "LOOP"};

class VSType;

class VSObject
{
public:
    VSType *type;
    vs_size_t size;
    vs_size_t refcnt;

    VSObject(){refcnt = 0;}
};

class VSType : public VSObject
{
public:
    OBJECT_TYPE type_id;
    std::string type_name;

    std::vector<std::string> attr_names;
    std::unordered_map<std::string, vs_addr_t> attr_indices;

    void (*_decref_func)(VSObject *obj);
    void (*_incref_func)(VSObject *obj);

    VSObject *(*to_str_func)();
    VSObject *(*copy_func)(VSObject *that);

    const char *(*to_bytes_func)();
    const std::string (*to_string_func)();
};

class VSValueObject : public VSObject
{
public:
    virtual VSObject *i_add(VSObject *that) {return NULL;}
    virtual VSObject *i_sub(VSObject *that) {return NULL;}
    virtual VSObject *i_mul(VSObject *that) {return NULL;}
    virtual VSObject *i_div(VSObject *that) {return NULL;}
    virtual VSObject *i_mod(VSObject *that) {return NULL;}
    virtual VSObject *i_lt(VSObject *that) {return NULL;}
    virtual VSObject *i_gt(VSObject *that) {return NULL;}
    virtual VSObject *i_le(VSObject *that) {return NULL;}
    virtual VSObject *i_ge(VSObject *that) {return NULL;}
    virtual VSObject *i_eq(VSObject *that) {return NULL;}
    virtual VSObject *i_neq(VSObject *that) {return NULL;}
    virtual VSObject *i_and(VSObject *that) {return NULL;}
    virtual VSObject *i_or(VSObject *that) {return NULL;}
    virtual VSObject *i_not(VSObject *that) {return NULL;}
    virtual VSObject *i_neg(VSObject *that) {return NULL;}
};

class VSIntObject : public VSObject
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

    VSValue *to_char();
    VSValue *to_int();
    VSValue *to_float();
    VSValue *to_str();

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