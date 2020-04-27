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
            VSObject *res = CALL_ATTR(const_cast<VSObject *>(o), "__hash__", vs_tuple_pack(0));
            VS_ENSURE_TYPE(res, T_INT, "as __hash__() result");

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

            VSObject *resobj = CALL_ATTR(const_cast<VSObject *>(a), "__eq__", vs_tuple_pack(1, b));
            VS_ENSURE_TYPE(resobj, T_BOOL, "as __eq__() result");

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