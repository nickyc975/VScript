#include "objects/VSFrameObject.hpp"

#include <cassert>

#include "objects/VSCodeObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"

VSObject *vs_frame_str(VSObject *frameobj) {
    VS_ENSURE_TYPE(frameobj, T_FRAME, "frame str");

    INCREF_RET(C_STRING_TO_STRING("frame"));
}

VSObject *vs_frame_bytes(VSObject *frameobj) {
    VS_ENSURE_TYPE(frameobj, T_FRAME, "frame bytes");

    INCREF_RET(VS_NONE);
}

VSFrameObject::VSFrameObject(VSCodeObject *code, VSTupleObject *args, VSTupleObject *cellvars, VSTupleObject *freevars, VSTupleObject *builtins, VSFrameObject *prev) {
    this->type = T_FRAME;

    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_default_eq, 2, true);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_frame_str, 1, false);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_frame_bytes, 1, false);

    this->pc = 0;

    assert(code != NULL);
    this->code = code;
    INCREF(code);

    this->nlocals = code->nlvars;
    this->locals = new VSTupleObject(this->nlocals);
    for (vs_size_t i = 0; i < this->nlocals; i++) {
        if (i < code->nargs) {
            TUPLE_SET(this->locals, i, new VSCellObject(TUPLE_GET(args, i)));
        } else {
            TUPLE_SET(this->locals, i, new VSCellObject(VS_NONE));
        }
    }
    INCREF(this->locals);

    if (cellvars != NULL) {
        this->ncellvars = TUPLE_LEN(cellvars);
        this->cellvars = cellvars;
        INCREF(cellvars);
    }

    if (freevars != NULL) {
        this->nfreevars = TUPLE_LEN(freevars);
        this->freevars = freevars;
        INCREF(freevars);
    }

    if (builtins != NULL) {
        this->nbuiltins = TUPLE_LEN(builtins);
        this->builtins = builtins;
        INCREF(builtins);
    }

    this->prev = prev;
    INCREF(prev);
}

VSFrameObject::~VSFrameObject() {
    DECREF_EX(this->code);
    DECREF_EX(this->locals);
    DECREF_EX(this->cellvars);
    DECREF_EX(this->freevars);
    DECREF_EX(this->builtins);
    DECREF_EX(this->prev);
}

typedef std::stack<VSObject *> cpt_stack_t;

inline VSObject *_stack_pop(cpt_stack_t &stack) {
    if (stack.empty()) {
        err("Internal error: unexpected empty compute stack");
        terminate(TERM_ERROR);
    }

    VSObject *value = stack.top();
    stack.pop();
    return value;
}

inline void *_stack_push(cpt_stack_t &stack, VSObject *value) {
    if (value == NULL) {
        err("Internal error: pushing NULL into compute stack");
        terminate(TERM_ERROR);
    }

    INCREF(value);
    stack.push(value);
}

#define STACK_TOP(stack) (stack.top())
#define STACK_POP(stack) _stack_pop(stack)
#define STACK_PUSH(stack, value) _stack_push(stack, value)

#define DO_B_OP(op, l_val, r_val, res)                                                     \
    NEW_ID(__##op##__);                                                                    \
    if (!HAS_ATTR(l_val, ID__##op##__)) {                                                  \
        err("\"%s\" object does not has attr: \"__" #op "__\"", TYPE_STR[l_val->type]);    \
        terminate(TERM_ERROR);                                                             \
    }                                                                                      \
    VSObject *op_func = GET_ATTR(l_val, ID__##op##__);                                     \
    if (!VS_IS_TYPE(op_func, T_FUNC)) {                                                    \
        err("attr \"__" #op "__\" of type \"%s\" is not function", TYPE_STR[l_val->type]); \
        terminate(TERM_ERROR);                                                             \
    }                                                                                      \
    res = ((VSFunctionObject *)op_func)->call(vs_tuple_pack(1, r_val));

void VSFrameObject::eval(std::stack<VSObject *> &stack) {
    while (this->pc < this->code->ninsts) {
        switch (code->code[this->pc].opcode) {
            case OP_POP: {
                VSObject *top = STACK_POP(stack);
                DECREF_EX(top);
                break;
            }
            case OP_ADD: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = NULL;
                DO_B_OP(add, l_val, r_val, res);
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_SUB: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = NULL;
                DO_B_OP(sub, l_val, r_val, res);
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_MUL: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = NULL;
                DO_B_OP(mul, l_val, r_val, res);
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_DIV: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = NULL;
                DO_B_OP(div, l_val, r_val, res);
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_MOD: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = NULL;
                DO_B_OP(mod, l_val, r_val, res);
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_LT: {
                break;
            }
            case OP_GT: {
                break;
            }
            case OP_LE: {
                break;
            }
            case OP_GE: {
                break;
            }
            case OP_EQ: {
                break;
            }
            case OP_NEQ: {
                break;
            }
            case OP_AND: {
                break;
            }
            case OP_XOR: {
                break;
            }
            case OP_OR: {
                break;
            }
            case OP_NOT: {
                break;
            }
            case OP_NEG: {
                break;
            }
            case OP_BUILD_TUPLE: {
                break;
            }
            case OP_BUILD_LIST: {
                break;
            }
            case OP_BUILD_DICT: {
                break;
            }
            case OP_BUILD_SET: {
                break;
            }
            case OP_INDEX_LOAD: {
                break;
            }
            case OP_INDEX_STORE: {
                break;
            }
            case OP_LOAD_LOCAL: {
                break;
            }
            case OP_LOAD_FREE: {
                break;
            }
            case OP_LOAD_CELL: {
                break;
            }
            case OP_LOAD_LOCAL_CELL: {
                break;
            }
            case OP_LOAD_FREE_CELL: {
                break;
            }
            case OP_LOAD_ATTR: {
                break;
            }
            case OP_STORE_LOCAL: {
                break;
            }
            case OP_STORE_FREE: {
                break;
            }
            case OP_STORE_CELL: {
                break;
            }
            case OP_STORE_ATTR: {
                break;
            }
            case OP_LOAD_CONST: {
                break;
            }
            case OP_LOAD_BUILTIN: {
                break;
            }
            case OP_JMP: {
                break;
            }
            case OP_JIF: {
                break;
            }
            case OP_BUILD_FUNC: {
                break;
            }
            case OP_CALL_FUNC: {
                break;
            }
            case OP_RET: {
                break;
            }
            case OP_NOP: {
                break;
            }
            default:
                break;
        }
        this->pc++;
    }
}
