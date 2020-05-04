#include "runtime/builtins.hpp"

#include "error.hpp"
#include "objects/VSBaseObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSDictObject.hpp"
#include "objects/VSFileObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSSetObject.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSTupleObject.hpp"

static const VSFileObject *VS_STDIN = NEW_REF(
    VSFileObject *, new VSFileObject(stdin, C_STRING_TO_STRING("stdin")));

static const VSFileObject *VS_STDOUT = NEW_REF(
    VSFileObject *, new VSFileObject(stdout, C_STRING_TO_STRING("stdout")));

static void vs_print_impl(VSObject *obj) {
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

static VSObject *vs_input(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs > 1) {
        ERR_NARGS("input()", 1, nargs);
        terminate(TERM_ERROR);
    } else if (nargs == 1) {
        vs_print_impl(args[0]);
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

static VSObject *vs_print(VSObject *, VSObject *const *args, vs_size_t nargs) {
    for (vs_size_t i = 0; i < nargs; i++) {
        vs_print_impl(args[i]);
        printf(" ");
    }
    printf("\n");
    INCREF_RET(VS_NONE);
}

static VSObject *vs_hasattr(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 2) {
        ERR_NARGS("hasattr()", 2, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *obj = args[0];
    VSObject *attrname = args[1];

    ENSURE_TYPE(attrname, T_STR, "as attrname");

    bool res = obj->hasattr(STRING_TO_C_STRING(attrname));
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

static VSObject *vs_getattr(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 2) {
        ERR_NARGS("getattr()", 2, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *obj = args[0];
    VSObject *attrname = args[1];

    ENSURE_TYPE(attrname, T_STR, "as attrname");

    return obj->getattr(STRING_TO_C_STRING(attrname));
}

static VSObject *vs_setattr(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 3) {
        ERR_NARGS("setattr()", 3, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *obj = args[0];
    VSObject *attrname = args[1];
    VSObject *attrvalue = args[2];

    obj->setattr(STRING_TO_C_STRING(attrname), attrvalue);
    INCREF_RET(VS_NONE);
}

static VSObject *vs_removeattr(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 2) {
        ERR_NARGS("removeattr()", 2, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *obj = args[0];
    VSObject *attrname = args[1];

    obj->setattr(STRING_TO_C_STRING(attrname), NULL);
    INCREF_RET(VS_NONE);
}

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
    {"object", 12},
    {"hasattr", 13},
    {"getattr", 14},
    {"setattr", 15},
    {"removeattr", 16},
    {"stdin", 17},
    {"stdout", 18}};

name_addr_map *builtin_addrs = &_builtin_addrs_struct;

VSTupleObject *builtins = vs_tuple_pack(
    19,
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("input"), vs_input)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("print"), vs_print)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("open"), vs_open)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("bool"), vs_bool)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("char"), vs_char)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("int"), vs_int)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("float"), vs_float)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("str"), vs_str)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("tuple"), vs_tuple)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("list"), vs_list)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("dict"), vs_dict)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("set"), vs_set)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("object"), vs_object)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("hasattr"), vs_hasattr)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("getattr"), vs_getattr)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("setattr"), vs_setattr)),
    AS_OBJECT(new VSNativeFunctionObject(NULL, C_STRING_TO_STRING("removeattr"), vs_removeattr)),
    AS_OBJECT(VS_STDIN),
    AS_OBJECT(VS_STDOUT));
