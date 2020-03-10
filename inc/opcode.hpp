typedef enum
{
    // no arg, remove stack top
    OP_POP,
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

    // 1 arg, load the global object indicated by the arg
    OP_LOAD_BUILTIN,

    // no arg, eval stack top
    OP_EVAL,

    // 1 arg, jump to point in current block
    OP_JMP,

    // 1 arg, jump to point in current block if stack top is true
    OP_JIF,

    // break current loop
    OP_BREAK,

    // goto loop_start of current loop
    OP_CONTINUE,

    // build VScript function
    OP_BUILD_FUNC,

    // no arg, call stack top
    OP_CALL_FUNC,

    // no arg, return
    OP_RET,

    OP_NOP
} OPCODE;

static char *OPCODE_STR[] =
    {
        "POP",
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
        "LOAD_LOCAL",
        "LOAD_NAME",
        "STORE_LOCAL",
        "STORE_NAME",
        "LOAD_CONST",
        "LOAD_BUILTIN",
        "EVAL",
        "JMP",
        "JIF",
        "BREAK",
        "CONTINUE",
        "BUILD_FUNC",
        "CALL_FUNC",
        "RET",
        "NOP"
    };