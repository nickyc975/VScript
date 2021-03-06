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
    static const str_func_map vs_set_methods;

    struct __set_hash__ {
        std::size_t operator()(const VSObject *o) const {
            NEW_IDENTIFIER(__hash__);
            VSObject *res = CALL_ATTR(const_cast<VSObject *>(o), ID___hash__, EMPTY_TUPLE());
            if (!IS_TYPE(res, T_INT)) {
                err("%s.__hash__() returned \"%s\" instead of int", TYPE_STR[o->type], TYPE_STR[res->type]);
                terminate(TERM_ERROR);
            }

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

            NEW_IDENTIFIER(__eq__);
            VSObject *resobj = CALL_ATTR(
                const_cast<VSObject *>(a), ID___eq__, vs_tuple_pack(1, const_cast<VSObject *>(b)));
            if (!IS_TYPE(resobj, T_BOOL)) {
                err("%s.__eq__() returned \"%s\" instead of bool", TYPE_STR[a->type], TYPE_STR[resobj->type]);
                terminate(TERM_ERROR);
            }

            bool res = (bool)BOOL_TO_C_BOOL(resobj);
            DECREF_EX(resobj);
            return res;
        }
    };

public:
    std::unordered_set<VSObject *, __set_hash__, __set_equal_to__> _set;

    VSSetObject();
    ~VSSetObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
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