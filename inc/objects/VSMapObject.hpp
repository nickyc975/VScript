#ifndef VS_MAP_H
#define VS_MAP_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"

#include <unordered_map>

extern VSTypeObject *VSMapType;

class VSMapObject : public VSObject {
private:
    struct __map_hash__ {
        std::size_t operator() (const VSObject *o) const {
            VSObject *res = VS_TYPEOF(o)->__hash__(o);
            std::size_t h = (std::size_t)INT_TO_C_INT(res);
            DECREF_EX(res);
            return h;
        }
    };

    struct __map_equal_to__ {
        bool operator() (const VSObject *a, const VSObject *b) const {
            if (VS_TYPEOF(a) != VS_TYPEOF(b)) {
                return false;
            }

            VSTypeObject *type = VS_TYPEOF(a);
            return (bool)BOOL_TO_C_BOOL(type->__eq__(a, b));
        }
    };

public:
    std::unordered_map<VSObject *, VSObject *, __map_hash__, __map_equal_to__> _map;

    VSMapObject() {
        this->type = VSMapType;
        this->_map = std::unordered_map<VSObject *, VSObject *, __map_hash__, __map_equal_to__>();
    }
};

// convinient macros for map operations
#define AS_MAP(obj) ((VSMapObject *)obj)
#define MAP_LEN(obj) (AS_MAP(obj)->_map.size())
#define MAP_GET(obj, key) (AS_MAP(obj)->_map[key])
#define MAP_HAS(obj, key) (AS_MAP(obj)->_map.find(key) != AS_MAP(obj)->_map.end())

#define MAP_SET(obj, key, val)        \
    do {                              \
        auto _key = (key);            \
        auto _val = (val);            \
        auto _map = AS_MAP(obj);      \
        if (MAP_HAS(obj, _key)) {     \
            DECREF(_map->_map[_key]); \
        } else {                      \
            INCREF(_key);             \
        }                             \
        _map->_map[_key] = _val;      \
        INCREF(_val);                 \
    } while (0);

#endif