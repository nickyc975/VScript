#ifndef VS_SET_H
#define VS_SET_H

#include <string>
#include <unordered_set>

#include "VSObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSTupleObject.hpp"

extern VSObject *vs_set(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSSetObject : public VSObject {
private:
    struct __set_hash__ {
        std::size_t operator()(const VSObject *o) const {
            VSObject *res = CALL_ATTR(const_cast<VSObject *>(o), "__hash__", vs_tuple_pack(0));
            VS_ENSURE_TYPE(res, T_INT, "as __hash__() result");

            std::size_t h = (std::size_t)INT_TO_C_INT(res);
            DECREF_EX(res);
            return h;
        }
    };

    struct __set_equal_to__ {
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
    std::unordered_set<VSObject *, __set_hash__, __set_equal_to__> _set;

    VSSetObject();
    ~VSSetObject();
};

// convinient macros for set operations
#define AS_SET(obj) ((VSSetObject *)obj)
#define SET_LEN(obj) (AS_SET(obj)->_set.size())
#define SET_HAS(obj, item) (AS_SET(obj)->_set.find(item) != AS_SET(obj)->_set.end())

#define SET_APPEND(obj, item)                 \
    do {                                      \
        auto _item = (item);                  \
        auto _set = AS_SET(obj);              \
        auto _res = _set->_set.insert(_item); \
        if (_res.second) {                    \
            INCREF(_item);                    \
        }                                     \
    } while (0);

#endif