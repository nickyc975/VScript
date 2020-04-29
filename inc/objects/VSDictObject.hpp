#ifndef VS_DICT_H
#define VS_DICT_H

#include <string>
#include <unordered_map>

#include "VSObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSTupleObject.hpp"

extern VSObject *vs_dict(VSObject *, VSObject *const *, vs_size_t nargs);

class VSDictObject : public VSObject {
private:
    static const str_func_map vs_dict_methods;

    struct __dict_hash__ {
        std::size_t operator()(const VSObject *o) const {
            NEW_IDENTIFIER(__hash__);
            VSObject *res = CALL_ATTR(const_cast<VSObject *>(o), ID___hash__, EMPTY_TUPLE());
            ENSURE_TYPE(res, T_INT, "as __hash__() result");

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

            NEW_IDENTIFIER(__eq__);
            VSObject *resobj = CALL_ATTR(const_cast<VSObject *>(a), ID___eq__, vs_tuple_pack(1, b));
            ENSURE_TYPE(resobj, T_BOOL, "as __eq__() result");

            bool res = (bool)BOOL_TO_C_BOOL(resobj);
            DECREF_EX(resobj);
            return res;
        }
    };

public:
    std::unordered_map<VSObject *, VSObject *, __dict_hash__, __dict_equal_to__> _dict;

    VSDictObject();
    ~VSDictObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
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