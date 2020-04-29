#ifndef VS_OBJECTS
#define VS_OBJECTS

#include <cstdlib>

#include "vs.hpp"

typedef enum {
    T_NONE,
    T_BOOL,
    T_CHAR,
    T_INT,
    T_FLOAT,
    T_STR,
    T_BYTES,
    T_LIST,
    T_TUPLE,
    T_DICT,
    T_SET,
    T_FUNC,
    T_OBJECT,
    T_CELL,
    T_CODE,
    T_FRAME,
    T_FILE
} TYPE;

static char *TYPE_STR[] = {
    "none",
    "bool",
    "char",
    "int",
    "float",
    "str",
    "bytes",
    "list",
    "tuple",
    "dict",
    "set",
    "func",
    "object",
    "cell",
    "code",
    "frame",
    "file"};

// static string management
#define NEW_IDENTIFIER(str) static std::string ID_##str = #str;

class VSObject {
public:
    TYPE type;
    vs_size_t refcnt;

    VSObject();
    virtual ~VSObject();

    virtual bool hasattr(std::string &attrname);
    virtual VSObject *getattr(std::string &attrname);
    virtual void setattr(std::string &attrname, VSObject *attrvalue);
};

// Native function interface
typedef VSObject *(*vs_native_func)(VSObject *, VSObject *const *, vs_size_t);

typedef std::unordered_map<std::string, vs_native_func> str_func_map;

inline VSObject *_NEW_REF(VSObject *obj) {
    if (obj != NULL) {
        obj->refcnt++;
    }
    return obj;
}

VSObject *vs_default_hash(VSObject *self, VSObject *const *args, vs_size_t nargs);
VSObject *vs_default_eq(VSObject *self, VSObject *const *args, vs_size_t nargs);

#define AS_OBJECT(obj) ((VSObject *)obj)

#define IS_TYPE(obj, ttype) (AS_OBJECT(obj)->type == ttype)

#define ENSURE_TYPE(obj, ttype, op)                                           \
    if (!IS_TYPE(obj, ttype)) {                                               \
        err("Can not apply \"" op "\" on type \"%s\".", TYPE_STR[obj->type]); \
        terminate(TERM_ERROR);                                                \
    }

#define INCREF(obj)                   \
    do {                              \
        if (obj != NULL) {            \
            AS_OBJECT(obj)->refcnt++; \
        }                             \
    } while (0);

#define INCREF_RET(obj)  \
    do {                 \
        auto _obj = obj; \
        INCREF(_obj);    \
        return _obj;     \
    } while (0);

#define NEW_REF(type, obj) (type) _NEW_REF(AS_OBJECT(obj))

#define DECREF(obj)                             \
    do {                                        \
        auto _obj = obj;                        \
        if (_obj != NULL) {                     \
            AS_OBJECT(_obj)->refcnt--;          \
            if (AS_OBJECT(_obj)->refcnt == 0) { \
                delete _obj;                    \
            }                                   \
        }                                       \
    } while (0);

#define DECREF_EX(obj)                         \
    do {                                       \
        if (obj != NULL) {                     \
            AS_OBJECT(obj)->refcnt--;          \
            if (AS_OBJECT(obj)->refcnt == 0) { \
                delete obj;                    \
                obj = NULL;                    \
            }                                  \
        }                                      \
    } while (0);

#define ERR_NO_ATTR(obj, attrname) \
    err("\"%s\" object does not has attribute \"%s\"", TYPE_STR[obj->type], attrname.c_str());

#endif