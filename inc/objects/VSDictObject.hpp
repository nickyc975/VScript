#ifndef VS_DICT_H
#define VS_DICT_H

#include <string>
#include <unordered_map>

#include "VSObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSTupleObject.hpp"

VSObject *vs_dict();

class VSDictObject : public VSObject {
private:
    struct __dict_hash__ {
        std::size_t operator()(const VSObject *o) const {
            if (!HAS_ATTR(o, "__hash__")) {
                ERR_ATTR_MISSING(o, "__hash__");
                terminate(TERM_ERROR);
            }

            VSObject *hash_func = GET_ATTR(o, "__hash__");
            if (!VS_IS_TYPE(hash_func, T_FUNC)) {
                ERR_ATTR_IS_NOT_FUNC(o, "__hash__");
                terminate(TERM_ERROR);
            }

            VSObject *res = ((VSFunctionObject *)hash_func)->call(vs_tuple_pack(0));
            std::size_t h = (std::size_t)INT_TO_C_INT(res);
            DECREF_EX(res);
            return h;
        }
    };

    struct __dict_equal_to__ {
        bool operator()(const VSObject *a, const VSObject *b) const {
            if (a->type != b->type) {
                return false;
            }

            if (!HAS_ATTR(a, "__eq__")) {
                ERR_ATTR_MISSING(a, "__eq__");
                terminate(TERM_ERROR);
            }

            VSObject *eq_func = GET_ATTR(a, "__eq__");
            if (!VS_IS_TYPE(eq_func, T_FUNC)) {
                ERR_ATTR_IS_NOT_FUNC(a, "__eq__");
                terminate(TERM_ERROR);
            }

            VSObject *resobj = ((VSFunctionObject *)eq_func)->call(vs_tuple_pack(1, b));
            bool res = (bool)BOOL_TO_C_BOOL(resobj);
            DECREF_EX(resobj);
            return res;
        }
    };

public:
    std::unordered_map<VSObject *, VSObject *, __dict_hash__, __dict_equal_to__> _dict;

    VSDictObject();
    ~VSDictObject();
};

// convinient macros for dict operations
#define AS_DICT(obj) ((VSDictObject *)obj)
#define DICT_LEN(obj) (AS_DICT(obj)->_dict.size())
#define DICT_GET(obj, key) (AS_DICT(obj)->_dict[key])
#define DICT_HAS(obj, key) (AS_DICT(obj)->_dict.find(key) != AS_DICT(obj)->_dict.end())

#define DICT_SET(obj, key, val)        \
    do {                              \
        auto _key = (key);            \
        auto _val = (val);            \
        auto _dict = AS_DICT(obj);      \
        if (DICT_HAS(obj, _key)) {     \
            DECREF(_dict->_dict[_key]); \
        } else {                      \
            INCREF(_key);             \
        }                             \
        _dict->_dict[_key] = _val;      \
        INCREF(_val);                 \
    } while (0);

#endif