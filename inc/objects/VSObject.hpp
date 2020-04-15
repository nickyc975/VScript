#ifndef VS_OBJECTS
#define VS_OBJECTS

#include "vs.hpp"

#include <cstdlib>

typedef enum {
    T_TYPE,
    T_CLASS,
    T_NONE,
    T_BOOL,
    T_CHAR,
    T_INT,
    T_FLOAT,
    T_STR,
    T_CELL,
    T_LIST,
    T_TUPLE,
    T_MAP,
    T_SET,
    T_CODE,
    T_FRAME,
    T_FUNC,
    T_OBJECT
} TYPE_TYPE;

class VSObject {
public:
    VSObject *type;
    vs_size_t refcnt;

    VSObject() {
        this->type = NULL;
        this->refcnt = 0;
    }
};

typedef VSObject *(*unaryfunc)(VSObject *);
typedef VSObject *(*binaryfunc)(VSObject *, VSObject *);
typedef VSObject *(*ternaryfunc)(VSObject *, VSObject *, VSObject *);
typedef VSObject *(*quaternaryfunc)(VSObject *, VSObject *, VSObject *, VSObject *);

typedef VSObject *(*const_unaryfunc)(const VSObject *);
typedef VSObject *(*const_binaryfunc)(const VSObject *, const VSObject *);
typedef VSObject *(*const_ternaryfunc)(const VSObject *, const VSObject *, const VSObject *);
typedef VSObject *(*const_quaternaryfunc)(const VSObject *, const VSObject *, const VSObject *, const VSObject *);

typedef void (*void_unaryfunc)(VSObject *);
typedef void (*void_binaryfunc)(VSObject *, VSObject *);
typedef void (*void_ternaryfunc)(VSObject *, VSObject *, VSObject *);
typedef void (*void_quaternaryfunc)(VSObject *, VSObject *, VSObject *, VSObject *);

typedef void (*void_const_unaryfunc)(const VSObject *);
typedef void (*void_const_binaryfunc)(const VSObject *, const VSObject *);
typedef void (*void_const_ternaryfunc)(const VSObject *, const VSObject *, const VSObject *);
typedef void (*void_const_quaternaryfunc)(const VSObject *, const VSObject *, const VSObject *, const VSObject *);

class NumberFuncs {
public:
    unaryfunc __not__;
    unaryfunc __neg__;
    binaryfunc __add__;
    binaryfunc __sub__;
    binaryfunc __mul__;
    binaryfunc __div__;
    binaryfunc __mod__;
    binaryfunc __and__;
    binaryfunc __or__;

    // type conversion
    unaryfunc __bool__;
    unaryfunc __char__;
    binaryfunc __int__;
    unaryfunc __float__;

    NumberFuncs(
        unaryfunc __not__,
        unaryfunc __neg__,
        binaryfunc __add__,
        binaryfunc __sub__,
        binaryfunc __mul__,
        binaryfunc __div__,
        binaryfunc __mod__,
        binaryfunc __and__,
        binaryfunc __or__,
        unaryfunc __bool__,
        unaryfunc __char__,
        binaryfunc __int__,
        unaryfunc __float__
    ) : __not__(__not__),
        __neg__(__neg__),
        __add__(__add__),
        __sub__(__sub__),
        __mul__(__mul__),
        __div__(__div__),
        __mod__(__mod__),
        __and__(__and__),
        __or__(__or__),
        __bool__(__bool__),
        __char__(__char__),
        __int__(__int__),
        __float__(__float__)
    {
    }
};

class ContainerFuncs {
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

inline VSObject *_NEW_REF(VSObject *obj) {
    obj->refcnt++;
    return obj;
}

#define VS_TYPEOF(obj) ((VSTypeObject *)(obj->type))

#define VS_AS_OBJECT(obj) ((VSObject *)obj)

#define VS_AS_TYPE(obj) ((VSTypeObject *)obj)

#define VS_ENSURE_TYPE(type, ttype, op)                                          \
    if (type->t_type != ttype) {                                                 \
        err("Can not apply \"" op "\" on type \"%s\".", type->__name__.c_str()); \
        terminate(TERM_ERROR);                                                   \
    }

#define INCREF(obj)                      \
    do {                                 \
        if (obj != NULL) {               \
            VS_AS_OBJECT(obj)->refcnt++; \
        }                                \
    } while (0);

#define INCREF_RET(obj)  \
    do {                 \
        auto _obj = obj; \
        INCREF(_obj);    \
        return _obj;     \
    } while (0);

#define NEW_REF(type, obj) (type) _NEW_REF(VS_AS_OBJECT(obj))

#define DECREF(obj)                                                                  \
    do {                                                                             \
        auto _obj = obj;                                                             \
        if (_obj != NULL) {                                                          \
            VS_AS_OBJECT(_obj)->refcnt--;                                            \
            if (VS_AS_OBJECT(_obj)->refcnt == 0) {                                   \
                if (VS_TYPEOF(_obj) != NULL && VS_TYPEOF(_obj)->__clear__ != NULL) { \
                    VS_TYPEOF(_obj)->__clear__(_obj);                                \
                }                                                                    \
                delete _obj;                                                         \
            }                                                                        \
        }                                                                            \
    } while (0);

#define DECREF_EX(obj)                                                             \
    do {                                                                           \
        if (obj != NULL) {                                                         \
            VS_AS_OBJECT(obj)->refcnt--;                                           \
            if (VS_AS_OBJECT(obj)->refcnt == 0) {                                  \
                if (VS_TYPEOF(obj) != NULL && VS_TYPEOF(obj)->__clear__ != NULL) { \
                    VS_TYPEOF(obj)->__clear__(obj);                                \
                }                                                                  \
                delete obj;                                                        \
                obj = NULL;                                                        \
            }                                                                      \
        }                                                                          \
    } while (0);

#endif