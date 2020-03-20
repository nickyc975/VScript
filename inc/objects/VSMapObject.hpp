#ifndef VS_MAP_H
#define VS_MAP_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

#include <unordered_map>

extern VSTypeObject *VSMapType;

class VSMapObject : public VSObject {
private:
    static size_t hash(const VSObject *obj) {
        VSObject *res = VS_TYPEOF(obj)->__hash__(obj);
        size_t h = (size_t)vs_int_to_cint(res);
        DECREF_EX(res);
        return h;
    }

    static bool equal_to(const VSObject *a, const VSObject *b) {
        if (VS_TYPEOF(a) != VS_TYPEOF(b)) {
            return false;
        }

        VSTypeObject *type = VS_TYPEOF(a);
        return (bool)BOOL_TO_C_BOOL(type->__eq__(a, b));
    }

public:
    std::unordered_map<VSObject *, VSObject *, decltype(&hash), decltype(&equal_to)> _map;

    VSMapObject() { this->type = VSMapType; }
};

// convinient macros for map operations
#define AS_MAP(obj) ((VSMapObject *)obj)
#define MAP_LEN(obj) (AS_MAP(obj)->_map.size())
#define MAP_GET(obj, key) (AS_MAP(obj)->_map[key])

#define MAP_SET(obj, key, val)                           \
    do {                                                 \
        auto _key = (key);                               \
        auto _val = (val);                               \
        auto _map = AS_MAP(obj);                         \
        if (_map->_map.find(_key) == _map->_map.end()) { \
            INCREF(_key);                                \
        } else {                                         \
            DECREF(_map->_map[_key]);                    \
        }                                                \
        _map->_map[_key] = _val;                         \
        INCREF(_val);                                    \
    } while (0);

#endif