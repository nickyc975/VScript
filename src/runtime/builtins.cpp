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
#include "objects/VSTupleObject.hpp"

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
    {"tuple", 8},
    {"list", 9},
    {"dict", 10},
    {"set", 11},
    {"hasattr", 12},
    {"getattr", 13},
    {"setattr", 14},
    {"removeattr", 15}};

name_addr_map *builtin_addrs = &_builtin_addrs_struct;

VSTupleObject *builtins = vs_tuple_pack(
    12,
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("input"), __vs_input__)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("print"), __vs_print__)),
    AS_OBJECT(NULL),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("bool"), vs_bool)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("char"), vs_char)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("int"), vs_int)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("float"), vs_float)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("str"), vs_str)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("tuple"), vs_tuple)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("list"), vs_list)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("dict"), vs_dict)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("set"), vs_set)));

static void __vs_print_impl__(VSObject *obj) {
    NEW_IDENTIFIER(__str__);
    VSObject *objstr = CALL_ATTR(obj, ID___str__, EMPTY_TUPLE());
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
    INCREF_RET(C_STRING_TO_STRING(str));
}

static VSObject *__vs_print__(VSObject *, VSObject *const *args, vs_size_t nargs) {
    for (vs_size_t i = 0; i < nargs; i++) {
        __vs_print_impl__(args[i]);
        printf(" ");
    }
    printf("\n");
    INCREF_RET(VS_NONE);
}
