#include "runtime/VSInterpreter.hpp"

#include "error.hpp"
#include "objects/VSBaseObject.hpp"
#include "objects/VSMapObject.hpp"
#include "objects/VSStringObject.hpp"

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

VSInterpreter::VSInterpreter(VSTupleObject *builtins) {
    this->call_depth = 0;

    this->builtins = builtins;
    INCREF(builtins);

    this->current_frame = NULL;
}

VSInterpreter::~VSInterpreter() {
    DECREF_EX(this->builtins);
    DECREF_EX(this->current_frame);
}

void VSInterpreter::interpret(VSCodeObject *code) {
    this->current_frame = new VSFrameObject(code, NULL);
    cpt_stack_t stack = cpt_stack_t();
    this->eval(stack);
}

#define DO_B_OP(op, l_val, r_val, res)                                                          \
    NEW_ID(__##op##__);                                                                         \
    VSTypeObject *type = VS_TYPEOF(l_val);                                                      \
    if (type->_number_funcs != NULL && type->_number_funcs->__##op##__ != NULL) {               \
        res = type->_number_funcs->__##op##__(l_val, r_val);                                    \
    } else if (type->t_type == T_OBJECT) {                                                      \
        VSBaseObject *l_obj = (VSBaseObject *)l_val;                                            \
        if (!HAS_ATTR(l_obj, ID__##op##__)) {                                                   \
            err("\"%s\" object does not has attr: \"__" #op "__\"", type->__name__.c_str());    \
            terminate(TERM_ERROR);                                                              \
        }                                                                                       \
        VSObject *op_func = GET_ATTR(l_obj, ID__##op##__);                                      \
        if (op_func->type != VSFunctionType) {                                                  \
            err("attr \"__" #op "__\" of type \"%s\" is not function", type->__name__.c_str()); \
            terminate(TERM_ERROR);                                                              \
        }                                                                                       \
        res = this->call_function((VSFunctionObject *)op_func, vs_tuple_pack(2, l_val, r_val)); \
    } else {                                                                                    \
        err("can not apply \"" #op "\" on type \"%s\"", type->__name__.c_str());                \
        terminate(TERM_ERROR);                                                                  \
    }

void VSInterpreter::eval(cpt_stack_t &stack) {
    VSFrameObject *frame = this->current_frame;
    VSCodeObject *code = frame->code;
    while (frame->pc < code->ninsts) {
        switch (code->code[frame->pc].opcode) {
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
        frame->pc++;
    }
}