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
    OP_XOR,
    OP_OR,
    OP_NOT,
    OP_NEG,

    // 1 arg, tuple size, create a tuple of objects in the stack
    OP_BUILD_TUPLE,

    // 1 arg, list length, create a list of objects in the stack
    OP_BUILD_LIST,

    // 1 arg, pair num in dict, create a dict of pairs in the stack
    OP_BUILD_DICT,

    // 1 arg, set size, create a set of objects in the stack
    OP_BUILD_SET,

    // |  obj  |
    // | index |
    // no arg, load the value at index (stack top) of object (following top)
    OP_INDEX_LOAD,

    /* |  obj  |
     * | index |
     * | value |
     * no arg, store the value at index (stack top) of object (following top)
     */
    OP_INDEX_STORE,

    // 1 arg, load the local object indicated by the arg
    OP_LOAD_LOCAL,

    // 1 arg, load the closure object indicated by the arg
    OP_LOAD_FREE,

    // 1 arg, load the cell of cell var at arg
    OP_LOAD_CELL,

    // 1 arg, load the cell of local var at arg
    OP_LOAD_LOCAL_CELL,

    // 1 arg, load the cell of free var at arg
    OP_LOAD_FREE_CELL,

    // |  obj  |
    // 1 arg, load the attr (name indicated by the arg) of the stack top
    OP_LOAD_ATTR,

    // 1 arg, store stack top to local indicated by the arg
    OP_STORE_LOCAL,

    // 1 arg, store stack top to closure at arg
    OP_STORE_FREE,

    // 1 arg, store cell at arg
    OP_STORE_CELL,

    /* |  obj  |
     * | value |
     */
    // 1 arg, store stack top to the attr (name indicated by arg) of the obj following top
    OP_STORE_ATTR,

    // 1 arg, load the object at index in the const vector
    OP_LOAD_CONST,

    // 1 arg, load the builtin object indicated by the arg
    OP_LOAD_BUILTIN,

    // 1 arg, jump to point in current block
    OP_JMP,

    // 1 arg, jump to point in current block if stack top is true
    OP_JIF,

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
        "XOR",
        "OR",
        "NOT",
        "NEG",
        "BUILD_TUPLE",
        "BUILD_LIST",
        "BUILD_DICT",
        "BUILD_SET",
        "INDEX_LOAD",
        "INDEX_STORE",
        "LOAD_LOCAL",
        "LOAD_FREE",
        "LOAD_CELL",
        "LOAD_LOCAL_CELL",
        "LOAD_FREE_CELL",
        "LOAD_ATTR",
        "STORE_LOCAL",
        "STORE_FREE",
        "STORE_CELL",
        "STORE_ATTR",
        "LOAD_CONST",
        "LOAD_BUILTIN",
        "JMP",
        "JIF",
        "BUILD_FUNC",
        "CALL_FUNC",
        "RET",
        "NOP"
    };