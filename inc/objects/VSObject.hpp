#ifndef VS_OBJECTS
#define VS_OBJECTS

#include "vs.hpp"

#define vs_typeof(obj) ((VSTypeObject *)obj->type)

#define vs_as_object(obj) ((VSObject *)obj)

#define vs_ensure_type(type, ttype, op)                                          \
    if (type->t_type != ttype)                                                   \
    {                                                                            \
        err("Can not apply \"" op "\" on type \"%s\".", type->__name__.c_str()); \
        terminate(TERM_ERROR);                                                   \
    }

#define incref(obj) obj->refcnt++;

#define decref(obj) \
    obj->refcnt--;                             \
    if (obj->refcnt == 0)                      \
    {                                          \
        if (vs_typeof(obj)->__clear__ != NULL) \
        {                                      \
            vs_typeof(obj)->__clear__(obj);    \
        }                                      \
        delete obj;                            \
    }

#define decref_ex(obj)                         \
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

    // wrappers for container functions
    static VSObject *getlen(VSObject *obj);
    static vs_size_t c_getlen(VSObject *obj);

    static VSObject *getitem_at(VSObject *container, VSObject *key);
    static void setitem_at(VSObject *container, VSObject *key, VSObject *value);
    static VSObject *hasitem_at(VSObject *container, VSObject *key);
    static void removeitem_at(VSObject *container, VSObject *key);

    static void appenditem(VSObject *container, VSObject *item);
    static VSObject *hasitem(VSObject *container, VSObject *item);
    static void removeitem(VSObject *container, VSObject *item);
};

typedef VSObject *(*unaryfunc)(VSObject *);
typedef VSObject *(*binaryfunc)(VSObject *, VSObject *);
typedef VSObject *(*ternaryfunc)(VSObject *, VSObject *, VSObject *);

typedef VSObject *(*const_unaryfunc)(const VSObject *);
typedef VSObject *(*const_binaryfunc)(const VSObject *, const VSObject *);
typedef VSObject *(*const_ternaryfunc)(const VSObject *, const VSObject *, const VSObject *);

typedef void (*void_unaryfunc)(VSObject *);
typedef void (*void_binaryfunc)(VSObject *, VSObject *);
typedef void (*void_ternaryfunc)(VSObject *, VSObject *, VSObject *);

typedef void (*void_const_unaryfunc)(const VSObject *);
typedef void (*void_const_binaryfunc)(const VSObject *, const VSObject *);
typedef void (*void_const_ternaryfunc)(const VSObject *, const VSObject *, const VSObject *);

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
        __char__(__char__),
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
    void_ternaryfunc __set__;
    void_binaryfunc __append__;
    binaryfunc __has__;
    binaryfunc __has_at__;
    void_binaryfunc __remove__;
    void_binaryfunc __remove_at__;

    ContainerFuncs(
        unaryfunc __len__,
        binaryfunc __get__,
        void_ternaryfunc __set__,
        void_binaryfunc __append__,
        binaryfunc __has__,
        binaryfunc __has_at__,
        void_binaryfunc __remove__,
        void_binaryfunc __remove_at__
    ) : __len__(__len__),
        __get__(__get__),
        __set__(__set__),
        __append__(__append__),
        __has__(__has__),
        __has_at__(__has_at__),
        __remove__(__remove__),
        __remove_at__(__remove_at__)
    {
    }
};

VSObject *vs_hash_not_implemented(const VSObject *obj);
VSObject *vs_default_eq(const VSObject *a, const VSObject *b);

#endif