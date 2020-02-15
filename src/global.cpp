#include "error.hpp"
#include "global.hpp"

#define expect_nargs(name, nargs, object)                                                                  \
    if (object.type != OBJ_LIST || object.objlist->length() != nargs)                                      \
    {                                                                                                      \
        err("native function \"%s\" needs %d arg(s) but got %d\n", name, nargs, object.objlist->length()); \
        terminate(TERM_ERROR);                                                                             \
    }

#define __vs_print_indent__(indent)  \
    for (int i = 0; i < indent; i++) \
    {                                \
        printf(" ");                 \
    }

static VSObject __vs_input__(VSObject object);
static VSObject __vs_print__(VSObject object);
static VSObject __vs_to_char__(VSObject object);
static VSObject __vs_to_int__(VSObject object);
static VSObject __vs_to_float__(VSObject object);
static VSObject __vs_to_str__(VSObject object);
static VSObject __vs_append__(VSObject object);
static VSObject __vs_len__(VSObject object);
static VSObject __vs_eval__(VSObject object);

static std::vector<std::string> symbols = {
    "input", "print", "char", "int", "float", "str", "append", "len", "eval"
};

static std::vector<vs_native_func> native_funcs = {
    &__vs_input__, &__vs_print__, &__vs_to_char__, &__vs_to_int__, &__vs_to_float__, &__vs_to_str__,
    &__vs_append__, &__vs_len__, &__vs_eval__
};

static void __vs_print_obj__(VSObject object, int indent)
{
    __vs_print_indent__(indent);
    if (object.type == OBJ_DATA)
    {
        printf("%s", object.value->to_string().c_str());
    }
    else if (object.type == OBJ_CODE)
    {
        printf("%s %s", CODE_BLK_STR[object.codeblock->type], object.codeblock->name.c_str());
    }
    else
    {
        printf("[");
        for (auto o : object.objlist->data)
        {
            __vs_print_obj__(o, indent + 1);
            printf(", ");
        }
        __vs_print_indent__(indent);
        printf("]");
    }
}

static VSObject __vs_input__(VSObject object)
{
    std::string str = std::string();
    char c = getchar();
    while (c != '\n' && c != '\r')
    {
        str.append(1, c);
        c = getchar();
    }
    if (c == '\r')
    {
        c = getchar();
        if (c != '\n')
        {
            ungetc(c, stdin);
        }
    }
    VSValue *value = new VSValue(str);
    return VSObject(value);
}

static VSObject __vs_print__(VSObject object)
{
    for (auto obj : object.objlist->data)
    {
        __vs_print_obj__(obj, 0);
    }
    return VSObject(VSValue::None());
}

static VSObject __vs_to_char__(VSObject object)
{
    expect_nargs("char()", 1, object);

    VSObject result = VSObject();
    VSObject arg = object.objlist->get(0);
    if (arg.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = arg.value->to_char();
    }
    else
    {
        err("Runtime error: can not cast object type \"%s\" to CHAR.\n", OBJ_STR[arg.type]);
        terminate(TERM_ERROR);
    }
    return result;
}

static VSObject __vs_to_int__(VSObject object)
{
    expect_nargs("int()", 1, object);

    VSObject result = VSObject();
    VSObject arg = object.objlist->get(0);
    if (arg.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = arg.value->to_int();
    }
    else
    {
        err("Runtime error: can not cast object type \"%s\" to INT.\n", OBJ_STR[object.type]);
        terminate(TERM_ERROR);
    }
    return result;
}

static VSObject __vs_to_float__(VSObject object)
{
    expect_nargs("float()", 1, object);

    VSObject result = VSObject();
    VSObject arg = object.objlist->get(0);
    if (arg.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = arg.value->to_float();
    }
    else
    {
        err("Runtime error: can not cast object type \"%s\" to FLOAT.\n", OBJ_STR[object.type]);
        terminate(TERM_ERROR);
    }
    return result;
}

static VSObject __vs_to_str__(VSObject object)
{
    expect_nargs("str()", 1, object);

    VSObject result = VSObject();
    VSObject arg = object.objlist->get(0);
    if (arg.type == OBJ_DATA)
    {
        result.type = OBJ_DATA;
        result.value = arg.value->to_str();
    }
    else
    {
        err("Runtime error: can not cast object type \"%s\" to STRING.\n", OBJ_STR[object.type]);
        terminate(TERM_ERROR);
    }
    return result;
}

static VSObject __vs_append__(VSObject object)
{
    expect_nargs("append()", 2, object);

    VSObject list = object.objlist->get(0);
    VSObject value = object.objlist->get(1);

    if (list.type != OBJ_LIST)
    {
        err("object type \"%s\" can not be appended\n", OBJ_STR[list.type]);
        terminate(TERM_ERROR);
    }

    list.objlist->push(value);
    return list;
}

static VSObject __vs_len__(VSObject object)
{
    expect_nargs("len()", 1, object);

    vs_int_t len = 0;
    VSObject arg = object.objlist->get(0);
    if (arg.type == OBJ_DATA)
    {
        if (arg.value->type == STRING)
        {
            len = arg.value->str_val->size();
        }
        else
        {
            err("Runtime error: can not calculate length of value type \"%s\".\n", TYPE_STR[arg.value->type]);
            terminate(TERM_ERROR);
        }
    }
    else if (arg.type == OBJ_LIST)
    {
        len = arg.objlist->length();
    }
    else
    {
        err("Runtime error: can not calculate length of object type \"%s\".\n", OBJ_STR[object.type]);
        terminate(TERM_ERROR);
    }
    return VSObject(new VSValue(len));
}

static VSObject __vs_eval__(VSObject object)
{
    expect_nargs("eval()", 1, object);
    return VSObject(VSValue::None());
}

VSObjectList *global_objects()
{
    VSObjectList *objlist = new VSObjectList();
    for (vs_size_t i = 0; i < native_funcs.size(); i++)
    {
        VSCodeObject *codeblock = new VSCodeObject(symbols[i], FUNC_BLK);
        if (symbols[i] == "input")
        {
            codeblock->add_inst(VSInst(OP_CALL_NATIVE, 1));
        }
        codeblock->add_inst(VSInst(OP_CALL_NATIVE, i));
        codeblock->add_local("...");
        objlist->push(VSObject(codeblock));
    }
    return objlist;
}

std::vector<vs_native_func> *native_func_addrs()
{
    auto result = new std::vector<vs_native_func>();
    for (auto addr : native_funcs)
    {
        result->push_back(addr);
    }
    return result;
}

std::unordered_map<std::string, vs_addr_t> *global_symbols()
{
    auto result = new std::unordered_map<std::string, vs_addr_t>();
    for (vs_size_t i = 0; i < symbols.size(); i++)
    {
        (*result)[symbols[i]] = i;
    }
    return result;
}