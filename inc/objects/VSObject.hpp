#ifndef VS_OBJECTS
#define VS_OBJECTS

#include "vs.hpp"

#define vs_typeof(obj) ((VSTypeObject *)obj->type)

#define vs_as_object(obj) ((VSObject *)obj)

#define vs_ensure_type(type, ttype, op)                                         \
    if (type->t_type != ttype)                                                  \
    {                                                                            \
        err("Can not apply \"" op "\" on type \"%s\".", type->__name__.c_str()); \
        terminate(TERM_ERROR);                                                   \
    }

#define incref(obj) obj->refcnt++;

#define decref(obj)                            \
    obj->refcnt--;                             \
    if (obj->refcnt == 0)                      \
    {                                          \
        if (vs_typeof(obj)->__clear__ != NULL) \
        {                                      \
            vs_typeof(obj)->__clear__(obj);    \
        }                                      \
        delete obj;                            \
        obj = NULL;                            \
    }

typedef enum
{
    T_TYPE,
    T_CLASS,
    T_NONE,
    T_BOOL,
    T_CHAR,
    T_INT,
    T_FLOAT,
    T_STR,
    T_LIST,
    T_MAP,
    T_SET,
    T_CODE,
    T_FRAME,
    T_FUNC,
    T_METH
} TYPE_TYPE;

class VSObject
{
public:
    VSObject *type;
    vs_size_t refcnt;

    VSObject() { refcnt = 0; }
};

typedef VSObject *(*noargfunc)();
typedef VSObject *(*unaryfunc)(VSObject *);
typedef VSObject *(*binaryfunc)(VSObject *, VSObject *);
typedef VSObject *(*ternaryfunc)(VSObject *, VSObject *, VSObject *);

typedef void (*void_unaryfunc)(VSObject *);
typedef void (*void_binaryfunc)(VSObject *, VSObject *);
typedef void (*void_ternaryfunc)(VSObject *, VSObject *, VSObject *);

class NumberFuncs
{
public:
    unaryfunc __not__;
    unaryfunc __neg__;
    binaryfunc __add__;
    binaryfunc __sub__;
    binaryfunc __mul__;
    binaryfunc __div__;
    binaryfunc __mod__;
    binaryfunc __lt__;
    binaryfunc __gt__;
    binaryfunc __le__;
    binaryfunc __ge__;
    binaryfunc __eq__;
    binaryfunc __neq__;
    binaryfunc __and__;
    binaryfunc __or__;

    // type conversion
    unaryfunc __bool__;
    unaryfunc __char__;
    unaryfunc __int__;
    unaryfunc __float__;

    NumberFuncs(
        unaryfunc __not__,
        unaryfunc __neg__,
        binaryfunc __add__,
        binaryfunc __sub__,
        binaryfunc __mul__,
        binaryfunc __div__,
        binaryfunc __mod__,
        binaryfunc __lt__,
        binaryfunc __gt__,
        binaryfunc __le__,
        binaryfunc __ge__,
        binaryfunc __and__,
        binaryfunc __or__,
        unaryfunc __bool__,
        unaryfunc __char__,
        unaryfunc __int__,
        unaryfunc __float__
    ) : __not__(__not__),
        __neg__(__neg__),
        __add__(__add__),
        __sub__(__sub__),
        __mul__(__mul__),
        __div__(__div__),
        __mod__(__mod__),
        __lt__(__lt__),
        __gt__(__gt__),
        __le__(__le__),
        __ge__(__ge__),
        __and__(__and__),
        __or__(__or__),
        __bool__(__bool__),
        __char__(__bool__),
        __int__(__int__),
        __float__(__float__)
    {
    }
};

class ContainerFuncs
{
public:
    unaryfunc __len__;
    binaryfunc __get__;
    void_ternaryfunc __put__;
    binaryfunc __contains__;
    void_binaryfunc __remove__;

    ContainerFuncs(
        binaryfunc __get__,
        void_ternaryfunc __put__,
        binaryfunc __contains__,
        void_binaryfunc __remove__
    ) : __get__(__get__),
        __put__(__put__),
        __contains__(__contains__),
        __remove__(__remove__)
    {
    }
};

class VSTypeObject : public VSObject
{
public:
    TYPE_TYPE t_type;
    std::string __name__;

    VSObject *__attrs__;

    noargfunc __new__;
    void_ternaryfunc __init__;
    unaryfunc __copy__;
    void_unaryfunc __clear__;

    binaryfunc __getattr__;
    binaryfunc __hasattr__;
    void_ternaryfunc __setattr__;
    void_binaryfunc __removeattr__;

    unaryfunc __hash__;
    binaryfunc __eq__;
    binaryfunc __neq__;

    unaryfunc __str__;
    unaryfunc __bytes__;

    // function types
    ternaryfunc __call__;

    // number types
    NumberFuncs *_number_funcs;

    // container types
    ContainerFuncs *_container_funcs;

    VSTypeObject(
        TYPE_TYPE t_type,
        std::string __name__,
        VSObject *__attrs__,
        noargfunc __new__,
        void_ternaryfunc __init__,
        unaryfunc __copy__,
        void_unaryfunc __clear__,
        binaryfunc __getattr__,
        binaryfunc __hasattr__,
        void_ternaryfunc __setattr__,
        void_binaryfunc __removeattr__,
        unaryfunc __hash__,
        binaryfunc __eq__,
        binaryfunc __neq__,
        unaryfunc __str__,
        unaryfunc __bytes__,
        ternaryfunc __call__,
        NumberFuncs *_number_funcs,
        ContainerFuncs *_container_funcs
    ) : t_type(t_type),
    __name__(__name__),
    __attrs__(__attrs__),
    __new__(__new__),
    __init__(__init__),
    __copy__(__copy__),
    __clear__(__clear__),
    __getattr__(__getattr__),
    __hasattr__(__hasattr__),
    __setattr__(__setattr__),
    __removeattr__(__removeattr__),
    __hash__(__hash__),
    __eq__(__eq__),
    __neq__(__neq__),
    __str__(__str__),
    __bytes__(__bytes__),
    __call__(__call__),
    _number_funcs(_number_funcs),
    _container_funcs(_container_funcs)
    {
    }
};

#endif