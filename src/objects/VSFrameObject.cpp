#include "objects/VSNoneObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSCodeObject.hpp"
#include "objects/VSFrameObject.hpp"

VSFrameObject::VSFrameObject(VSObject *func)
{
    this->pc = 0;
    this->func = func;
    this->nlocals = AS_CODE(FUNC_GET_CODE(this->func))->nlvars;
    this->locals = new VSTupleObject(this->nlocals);

    for (vs_size_t i = 0; i < this->nlocals; i++)
    {
        TUPLE_SET(this->locals, i, new VSCellObject(VS_NONE));
    }
}

VSFrameObject::~VSFrameObject()
{
    
}

void VSFrameObject::eval(VSFrameObject *frame, std::stack<VSObject *> &stack) {
    VSCodeObject *code = AS_CODE(FUNC_GET_CODE(frame->func));
    while (frame->pc < code->ninsts) {
        switch (code->code[frame->pc].opcode) {
        case OP_POP: {
            VSObject *top = stack.top();
            stack.pop();
            DECREF_EX(top);
            break;
        }
        case OP_ADD: {
            break;
        }
        case OP_SUB: {
            break;
        }
        case OP_MUL: {
            break;
        }
        case OP_DIV: {
            break;
        }
        case OP_MOD: {
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
    }
}