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

#define VS_AS_OBJECT(obj) ((VSObject *)obj)

#define VS_IS_TYPE(obj, ttype) (VS_AS_OBJECT(obj)->type == ttype)

#define VS_ENSURE_TYPE(obj, ttype, op)                                        \
    if (!VS_IS_TYPE(obj, ttype)) {                                            \
        err("Can not apply \"" op "\" on type \"%s\".", TYPE_STR[obj->type]); \
        terminate(TERM_ERROR);                                                \
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

#define DECREF(obj)                                \
    do {                                           \
        auto _obj = obj;                           \
        if (_obj != NULL) {                        \
            VS_AS_OBJECT(_obj)->refcnt--;          \
            if (VS_AS_OBJECT(_obj)->refcnt == 0) { \
                delete _obj;                       \
            }                                      \
        }                                          \
    } while (0);

#define DECREF_EX(obj)                            \
    do {                                          \
        if (obj != NULL) {                        \
            VS_AS_OBJECT(obj)->refcnt--;          \
            if (VS_AS_OBJECT(obj)->refcnt == 0) { \
                delete obj;                       \
                obj = NULL;                       \
            }                                     \
        }                                         \
    } while (0);

class VSObject;

class AttributeDef {
public:
    bool readonly;
    VSObject *attribute;

    AttributeDef(bool readonly, VSObject *attribute);
    ~AttributeDef();
};

typedef std::unordered_map<std::string, AttributeDef *> str_attr_map;

class VSObject {
public:
    TYPE type;
    vs_size_t refcnt;
    str_attr_map attrs;

    VSObject();
    ~VSObject();
};

inline VSObject *_NEW_REF(VSObject *obj) {
    if (obj != NULL) {
        obj->refcnt++;
    }
    return obj;
}

VSObject *vs_default_hash(VSObject *self, VSObject *const *args, vs_size_t nargs);
VSObject *vs_default_eq(VSObject *self, VSObject *const *args, vs_size_t nargs);

// static string management
#define NEW_ID(str) static std::string ID##str = #str;

// attribute operation macros
#define GET_ATTR(obj, attrname) ((obj)->attrs[(attrname)]->attribute)

#define SET_ATTR(obj, attrname, value)                               \
    do {                                                             \
        auto _obj = (obj);                                           \
        auto _attrname = (attrname);                                 \
        auto _value = (value);                                       \
        auto _iter = _obj->attrs.find(_attrname);                    \
        if (_iter != _obj->attrs.end()) {                            \
            if (!_iter->second->readonly) {                          \
                DECREF_EX(_iter->second->attribute);                 \
                _obj->attrs[_attrname]->attribute = _value;          \
                INCREF(_value);                                      \
            }                                                        \
        } else {                                                     \
            _obj->attrs[_attrname] = new AttributeDef(false, _value) \
        }                                                            \
    } while (0);

#define HAS_ATTR(obj, attrname) ((obj)->attrs.find(attrname) != (obj)->attrs.end())

#define ERR_ATTR_MISSING(obj, attrname) \
    err("\"%s\" object does not has attr \"%s\"", TYPE_STR[obj->type], attrname.c_str());

#define ERR_ATTR_READONLY(obj, attrname) \
    err("attr \"%s\" of \"%s\" object is readonly", attrname.c_str(), TYPE_STR[obj->type]);

#define ERR_ATTR_IS_NOT_FUNC(obj, attrname) \
    err("attr \"%s\" of \"%s\" object is not function", attrname.c_str(), TYPE_STR[obj->type]);

#endif