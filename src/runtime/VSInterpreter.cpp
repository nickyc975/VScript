#include "runtime/VSInterpreter.hpp"

#include <cassert>

#include "runtime/builtins.hpp"
#include "objects/VSCellObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSDictObject.hpp"
#include "objects/VSSetObject.hpp"


NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__lt__);
NEW_IDENTIFIER(__gt__);
NEW_IDENTIFIER(__le__);
NEW_IDENTIFIER(__ge__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);
NEW_IDENTIFIER(__call__);
NEW_IDENTIFIER(__neg__);
NEW_IDENTIFIER(__add__);
NEW_IDENTIFIER(__sub__);
NEW_IDENTIFIER(__mul__);
NEW_IDENTIFIER(__div__);
NEW_IDENTIFIER(__mod__);
NEW_IDENTIFIER(__not__);
NEW_IDENTIFIER(__and__);
NEW_IDENTIFIER(__or__);
NEW_IDENTIFIER(__xor__);
NEW_IDENTIFIER(__bool__);
NEW_IDENTIFIER(__char__);
NEW_IDENTIFIER(__int__);
NEW_IDENTIFIER(__float__);
NEW_IDENTIFIER(get);
NEW_IDENTIFIER(set);

VSInterpreter::VSInterpreter() {
}

VSInterpreter::~VSInterpreter() {
}

inline VSObject *_stack_pop(cpt_stack_t &stack) {
    if (stack.empty()) {
        err("Internal error: unexpected empty compute stack");
        terminate(TERM_ERROR);
    }

    VSObject *value = stack.top(); stack.pop();
    return value;
}

inline void _stack_push(cpt_stack_t &stack, VSObject *value) {
    if (value == NULL) {
        err("Internal error: pushing NULL into compute stack");
        terminate(TERM_ERROR);
    }

    stack.push(value);
    return;
}

#define STACK_TOP(stack) (stack.top())
#define STACK_POP(stack) _stack_pop(stack)
#define STACK_PUSH(stack, value) _stack_push(stack, value)
#define STACK_PUSH_INCREF(stack, value) \
    do {                                \
        auto __value = (value);         \
        _stack_push(stack, __value);    \
        INCREF(__value);                \
    } while (0);

void VSInterpreter::exec(
    cpt_stack_t &stack, vs_addr_t &pc, VSCodeObject *code, VSTupleObject *locals,
    VSTupleObject *freevars, VSTupleObject *cellvars, VSTupleObject *globals) const {

    vs_size_t nlocals = locals == NULL ? 0 : TUPLE_LEN(locals);
    vs_size_t nfreevars = freevars == NULL ? 0 : TUPLE_LEN(freevars);
    vs_size_t ncellvars = cellvars == NULL ? 0 : TUPLE_LEN(cellvars);
    vs_size_t nglobals = globals == NULL ? 0 : TUPLE_LEN(globals);

    while (pc < code->ninsts) {
        VSInst inst = code->code[pc];
        switch (inst.opcode) {
            case OP_POP: {
                VSObject *top = STACK_POP(stack);
                DECREF_EX(top);
                break;
            }
            case OP_ADD: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___add__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_SUB: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___sub__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_MUL: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___mul__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_DIV: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___div__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_MOD: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___mod__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_LT: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___lt__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_GT: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___gt__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_LE: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___le__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_GE: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___ge__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_EQ: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___eq__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_NEQ: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *temp = CALL_ATTR(l_val, ID___eq__, vs_tuple_pack(1, r_val));
                VSObject *res = CALL_ATTR(temp, ID___not__, EMPTY_TUPLE());
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                DECREF(temp);
                break;
            }
            case OP_AND: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___and__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_XOR: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___xor__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_OR: {
                VSObject *l_val = STACK_POP(stack);
                VSObject *r_val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(l_val, ID___or__, vs_tuple_pack(1, r_val));
                STACK_PUSH(stack, res);
                DECREF(l_val);
                DECREF(r_val);
                break;
            }
            case OP_NOT: {
                VSObject *val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(val, ID___not__, EMPTY_TUPLE());
                STACK_PUSH(stack, res);
                DECREF(val);
                break;
            }
            case OP_NEG: {
                VSObject *val = STACK_POP(stack);
                VSObject *res = CALL_ATTR(val, ID___neg__, EMPTY_TUPLE());
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
                STACK_PUSH_INCREF(stack, tuple);
                break;
            }
            case OP_BUILD_LIST: {
                vs_size_t nitems = inst.operand;
                VSListObject *list = new VSListObject(nitems);
                for (vs_size_t i = 0; i < nitems; i++) {
                    VSObject *item = STACK_POP(stack);
                    list->items[i] = item;
                }
                STACK_PUSH_INCREF(stack, list);
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
                    DECREF(pair);
                }
                STACK_PUSH_INCREF(stack, dict);
                break;
            }
            case OP_BUILD_SET: {
                vs_size_t nitems = inst.operand;
                VSSetObject *set = new VSSetObject();
                for (vs_size_t i = 0; i < nitems; i++) {
                    VSObject *item = STACK_POP(stack);
                    auto res = set->_set.insert(item);
                    if (!res.second) {
                        DECREF(item);
                    }
                }
                STACK_PUSH_INCREF(stack, set);
                break;
            }
            case OP_INDEX_LOAD: {
                VSObject *obj = STACK_POP(stack);
                VSObject *idx = STACK_POP(stack);
                VSObject *val = CALL_ATTR(obj, ID_get, vs_tuple_pack(1, idx));
                STACK_PUSH(stack, val);
                DECREF(obj);
                break;
            }
            case OP_INDEX_STORE: {
                VSObject *obj = STACK_POP(stack);
                VSObject *idx = STACK_POP(stack);
                VSObject *val = STACK_POP(stack);
                CALL_ATTR(obj, ID_set, vs_tuple_pack(2, idx, val));
                DECREF(obj);
                DECREF(idx);
                DECREF(val);
                break;
            }
            case OP_LOAD_LOCAL: {
                vs_addr_t idx = inst.operand;
                if (idx >= nlocals) {
                    err("Internal error: invalid local var index: %llu, max: %llu", idx, nlocals - 1);
                    terminate(TERM_ERROR);
                }
                VSObject *local = TUPLE_GET(locals, idx);
                STACK_PUSH_INCREF(stack, VS_CELL_GET(local));
                break;
            }
            case OP_LOAD_FREE: {
                vs_addr_t idx = inst.operand;
                if (idx >= nfreevars) {
                    err("Internal error: invalid free var index: %llu, max: %llu", idx, nfreevars - 1);
                    terminate(TERM_ERROR);
                }
                VSObject *free = TUPLE_GET(freevars, idx);
                STACK_PUSH_INCREF(stack, VS_CELL_GET(free));
                break;
            }
            case OP_LOAD_CELL: {
                vs_addr_t idx = inst.operand;
                if (idx >= ncellvars) {
                    err("Internal error: invalid cell var index: %llu, max: %llu", idx, ncellvars - 1);
                    terminate(TERM_ERROR);
                }
                VSObject *cell = TUPLE_GET(cellvars, idx);
                STACK_PUSH_INCREF(stack, cell);
                break;
            }
            case OP_LOAD_LOCAL_CELL: {
                vs_addr_t idx = inst.operand;
                if (idx >= nlocals) {
                    err("Internal error: invalid local var index: %llu, max: %llu", idx, nlocals - 1);
                    terminate(TERM_ERROR);
                }
                VSObject *local = TUPLE_GET(locals, idx);
                STACK_PUSH_INCREF(stack, local);
                break;
            }
            case OP_LOAD_FREE_CELL: {
                vs_addr_t idx = inst.operand;
                if (idx >= nfreevars) {
                    err("Internal error: invalid free var index: %llu, max: %llu", idx, nfreevars - 1);
                    terminate(TERM_ERROR);
                }
                VSObject *free = TUPLE_GET(freevars, idx);
                STACK_PUSH_INCREF(stack, free);
                break;
            }
            case OP_LOAD_ATTR: {
                VSObject *obj = STACK_POP(stack);
                vs_addr_t idx = inst.operand;
                if (idx >= code->nnames) {
                    err("Internal error: invalid name index: %llu, max: %llu", idx, code->nnames - 1);
                    terminate(TERM_ERROR);
                }

                VSObject *attrnameobj = LIST_GET(code->names, idx);
                std::string &attrname = STRING_TO_C_STRING(attrnameobj);
                if (!obj->hasattr(attrname)) {
                    ERR_NO_ATTR(obj, attrname);
                    terminate(TERM_ERROR);
                }

                VSObject *attr = obj->getattr(attrname);
                STACK_PUSH(stack, attr);
                DECREF(obj);
                break;
            }
            case OP_STORE_LOCAL: {
                vs_addr_t idx = inst.operand;
                VSObject *val = STACK_POP(stack);
                if (idx >= nlocals) {
                    err("Internal error: invalid local var index: %llu, max: %llu", idx, nlocals - 1);
                    terminate(TERM_ERROR);
                }

                VSObject *cell = TUPLE_GET(locals, idx);
                VS_CELL_SET(cell, val);
                DECREF(val);
                break;
            }
            case OP_STORE_FREE: {
                vs_addr_t idx = inst.operand;
                VSObject *val = STACK_POP(stack);
                if (idx >= nfreevars) {
                    err("Internal error: invalid free var index: %llu, max: %llu", idx, nfreevars - 1);
                    terminate(TERM_ERROR);
                }

                VSObject *cell = TUPLE_GET(freevars, idx);
                VS_CELL_SET(cell, val);
                DECREF(val);
                break;
            }
            case OP_STORE_CELL: {
                vs_addr_t idx = inst.operand;
                VSObject *val = STACK_POP(stack);
                if (idx >= ncellvars) {
                    err("Internal error: invalid cell var index: %llu, max: %llu", idx, ncellvars - 1);
                    terminate(TERM_ERROR);
                }

                TUPLE_SET(cellvars, idx, val);
                DECREF(val);
                break;
            }
            case OP_STORE_ATTR: {
                vs_addr_t idx = inst.operand;
                VSObject *obj = STACK_POP(stack);
                VSObject *attrvalue = STACK_POP(stack);
                if (idx >= code->nnames) {
                    err("Internal error: invalid name index: %llu, max: %llu", idx, code->nnames - 1);
                    terminate(TERM_ERROR);
                }

                VSObject *attrnameobj = LIST_GET(code->names, idx);
                std::string &attrname = STRING_TO_C_STRING(attrnameobj);
                if (!obj->hasattr(attrname)) {
                    ERR_NO_ATTR(obj, attrname);
                    terminate(TERM_ERROR);
                }

                obj->setattr(attrname, attrvalue);
                DECREF(attrvalue);
                DECREF(obj);
                break;
            }
            case OP_LOAD_CONST: {
                vs_addr_t idx = inst.operand;
                if (idx >= code->nconsts) {
                    err("Internal error: invalid const index: %llu, max: %llu", idx, code->nconsts - 1);
                    terminate(TERM_ERROR);
                }

                VSObject *obj = LIST_GET(code->consts, idx);
                STACK_PUSH_INCREF(stack, obj);
                break;
            }
            case OP_LOAD_BUILTIN: {
                vs_addr_t idx = inst.operand;
                if (idx >= nbuiltins) {
                    err("Internal error: invalid builtin index: %llu, max: %llu", idx, nbuiltins - 1);
                    terminate(TERM_ERROR);
                }

                VSObject *obj = TUPLE_GET(builtins, idx);
                STACK_PUSH_INCREF(stack, obj);
                break;
            }
            case OP_JMP: {
                vs_addr_t target = inst.operand;
                if (target >= code->ninsts) {
                    err("Internal error: invalid jump target: %llu, max: %llu", target, code->ninsts - 1);
                    terminate(TERM_ERROR);
                }

                pc = target;
                pc--;
                break;
            }
            case OP_JIF: {
                vs_addr_t target = inst.operand;
                VSObject *obj = STACK_POP(stack);
                if (obj->type != T_BOOL) {
                    err("Internal error: jump condition can not be \"%s\" object", TYPE_STR[obj->type]);
                    terminate(TERM_ERROR);
                }

                if (BOOL_TO_C_BOOL(obj)) {
                    pc = target;
                    pc--;
                }
                DECREF(obj);
                break;
            }
            case OP_BUILD_FUNC: {
                VSObject *codeobj = STACK_POP(stack);
                VSObject *freevars = STACK_POP(stack);
                VSCodeObject *code = (VSCodeObject *)codeobj;

                VSFunctionObject *func = new VSDynamicFunctionObject(code->name, code, AS_TUPLE(freevars), code->flags);
                STACK_PUSH_INCREF(stack, func);
                DECREF(freevars);
                DECREF(code);
                break;
            }
            case OP_CALL_FUNC: {
                VSObject *func = STACK_POP(stack);
                VSObject *args = STACK_POP(stack);

                if (!func->hasattr(ID___call__)) {
                    err("\"%s\" object is not callable", TYPE_STR[func->type]);
                    terminate(TERM_ERROR);
                }

                VSObject *__call__ = func->getattr(ID___call__);

                if (__call__->type != T_FUNC) {
                    err("attribute \"__call__\" of \"%s\" object is not function", TYPE_STR[func->type]);
                    terminate(TERM_ERROR);
                }

                if (args->type != T_TUPLE) {
                    err("Internal error: function arg list can not be \"%s\" object", TYPE_STR[func->type]);
                    terminate(TERM_ERROR);
                }

                VSObject *res = ((VSFunctionObject *)__call__)->call((VSTupleObject *)args);
                STACK_PUSH(stack, res);
                DECREF(__call__);
                DECREF(func);
                break;
            }
            case OP_RET: {
                return;
            }
            case OP_NOP: {
                break;
            }
            default:
                break;
        }
        pc++;
    }
}

void VSInterpreter::eval(cpt_stack_t &stack, VSFrameObject *frame) const {
    this->exec(stack, frame->pc, frame->code, frame->locals, frame->freevars, frame->cellvars, NULL);
}

const VSInterpreter INTERPRETER = VSInterpreter();