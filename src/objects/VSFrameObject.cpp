#include "objects/VSFrameObject.hpp"

#include <cassert>

#include "objects/VSCodeObject.hpp"
#include "objects/VSDictObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSSetObject.hpp"
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

inline void _stack_push(cpt_stack_t &stack, VSObject *value) {
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

void VSFrameObject::eval(std::stack<VSObject *> &stack) {
    while (this->pc < this->code->ninsts) {
        VSInst inst = code->code[this->pc];
        switch (inst.opcode) {
            case OP_POP: {
                VSObject *top = STACK_POP(stack);
                DECREF_EX(top);
                break;
            }
            case OP_ADD: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__add__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_SUB: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__sub__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_MUL: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__mul__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_DIV: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__div__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_MOD: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__mod__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_LT: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__lt__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_GT: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__gt__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_LE: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__le__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_GE: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__ge__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_EQ: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__eq__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_NEQ: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__eq__", vs_tuple_pack(1, r_val));
                res = CALL_ATTR(res, "__not__", vs_tuple_pack(0));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_AND: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__and__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_XOR: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__xor__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_OR: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, "__or__", vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_NOT: {
                VSObject *val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(val, "__not__", vs_tuple_pack(0));
                STACK_PUSH(stack, res);
                DECREF(val);
                break;
            }
            case OP_NEG: {
                VSObject *val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(val, "__neg__", vs_tuple_pack(0));
                STACK_PUSH(stack, res);
                DECREF(val);
                break;
            }
            case OP_BUILD_TUPLE: {
                vs_size_t nitems = inst.operand;
                VSTupleObject *tuple = new VSTupleObject(nitems);
                for (vs_size_t i = 0; i < nitems; i++) {
                    VSObject *item = STACK_POP(stack);
                    tuple->items[i] = item;
                }
                STACK_PUSH(stack, tuple);
                break;
            }
            case OP_BUILD_LIST: {
                vs_size_t nitems = inst.operand;
                VSListObject *list = new VSListObject(nitems);
                for (vs_size_t i = 0; i < nitems; i++) {
                    VSObject *item = STACK_POP(stack);
                    list->items[i] = item;
                }
                STACK_PUSH(stack, list);
                break;
            }
            case OP_BUILD_DICT: {
                vs_size_t npairs = inst.operand;
                VSDictObject *dict = new VSDictObject();
                for (vs_size_t i = 0; i < npairs; i++) {
                    VSObject *pair = STACK_POP(stack);
                    if (pair->type != T_TUPLE || TUPLE_LEN(pair) != 2) {
                        err("Internal error: BUILD_DICT arguments are not binary tuples");
                        terminate(TERM_ERROR);
                    }

                    VSObject *key = TUPLE_GET(pair, 0), *value = TUPLE_GET(pair, 1);
                    DICT_SET(dict, key, value);
                    DECREF_EX(pair);
                }
                STACK_PUSH(stack, dict);
                break;
            }
            case OP_BUILD_SET: {
                vs_size_t nitems = inst.operand;
                VSSetObject *set = new VSSetObject();
                for (vs_size_t i = 0; i < nitems; i++) {
                    VSObject *item = STACK_POP(stack);
                    auto res = set->_set.insert(item);
                    if (!res.second) {
                        DECREF_EX(item);
                    }
                }
                STACK_PUSH(stack, set);
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
