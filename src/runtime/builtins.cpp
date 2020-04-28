#include "runtime/builtins.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSDictObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSSetObject.hpp"
#include "objects/VSStringObject.hpp"

static VSObject *__vs_input__(VSObject *, VSObject *const *args, vs_size_t nargs);
static VSObject *__vs_print__(VSObject *, VSObject *const *args, vs_size_t nargs);

name_addr_map _builtin_addrs_struct = {
    {"input", 0},
    {"print", 1},
    {"open", 2},
    {"bool", 3},
    {"char", 4},
    {"int", 5},
    {"float", 6},
    {"str", 7},
    {"hasattr", 8},
    {"getattr", 9},
    {"setattr", 10},
    {"removeattr", 11}};

name_addr_map *builtin_addrs = &_builtin_addrs_struct;

VSTupleObject *builtins = vs_tuple_pack(
    8,
    VS_AS_OBJECT(new VSNativeFunctionObject("input", __vs_input__, NULL)),
    VS_AS_OBJECT(new VSNativeFunctionObject("print", __vs_print__, NULL)),
    VS_AS_OBJECT(NULL),
    VS_AS_OBJECT(new VSNativeFunctionObject("bool", vs_bool, NULL)),
    VS_AS_OBJECT(new VSNativeFunctionObject("char", vs_char, NULL)),
    VS_AS_OBJECT(new VSNativeFunctionObject("int", vs_int, NULL)),
    VS_AS_OBJECT(new VSNativeFunctionObject("float", vs_float, NULL)),
    VS_AS_OBJECT(new VSNativeFunctionObject("str", vs_str, NULL)));

static void __vs_print_impl__(VSObject *obj) {
    VSObject *objstr = CALL_ATTR(obj, "__str__", EMPTY_TUPLE());
    if (objstr->type != T_STR) {
        err("__str__() of \"%s\" object returned \"%s\" instead of str object", TYPE_STR[obj->type], TYPE_STR[objstr->type]);
        terminate(TERM_ERROR);
    }

    std::string &str = ((VSStringObject *)objstr)->_value;
    printf("%s", str.c_str());

    DECREF_EX(objstr);
}

static VSObject *__vs_input__(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs > 1) {
        ERR_NARGS("input()", 1, nargs);
        terminate(TERM_ERROR);
    } else if (nargs == 1) {
        __vs_print_impl__(args[0]);
    }

    std::string str = std::string();
    char c = getchar();
    while (c != '\n' && c != '\r') {
        str.append(1, c);
        c = getchar();
    }
    if (c == '\r') {
        c = getchar();
        if (c != '\n') {
            ungetc(c, stdin);
        }
    }
    return C_STRING_TO_STRING(str);
}

static VSObject *__vs_print__(VSObject *, VSObject *const *args, vs_size_t nargs) {
    for (vs_size_t i = 0; i < nargs; i++) {
        __vs_print_impl__(args[i]);
        printf(" ");
    }
    printf("\n");
    return VS_NONE;
}
