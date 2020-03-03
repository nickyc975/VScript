#include <unordered_map>

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSMapObject.hpp"

size_t hash(const VSObject *obj)
{
    VSObject *res = vs_typeof(obj)->__hash__(obj);
    size_t h = (size_t)vs_int_to_cint(res);
    decref_ex(res);
    return h;
}

bool equal_to(const VSObject *a, const VSObject *b)
{
    if (vs_typeof(a) != vs_typeof(b))
    {
        return false;
    }
    
    VSTypeObject *type = vs_typeof(a);
    return (bool)vs_bool_to_cbool(type->__eq__(a, b));
}

class VSMapObject : public VSObject
{
public:
    std::unordered_map<VSObject *, VSObject *, decltype(&hash), decltype(&equal_to)> _map;

    VSMapObject() {this->type = VSMapType;}
};

VSObject *vs_map_new()
{
    return vs_as_object(new VSMapObject());
}

void vs_map_init(VSObject *, VSObject *, VSObject *)
{
}

VSObject *vs_map_copy(const VSObject *that)
{
    VSTypeObject *type = vs_typeof(that);

    vs_ensure_type(type, T_MAP, "map copy");

    VSMapObject *map = (VSMapObject *)that;
    VSMapObject *new_map = (VSMapObject *)type->__new__();
    for (auto entry : map->_map)
    {
        new_map->_map[entry.first] = entry.second;
        incref(entry.first);
        incref(entry.second);
    }
    return vs_as_object(new_map);
}

void vs_map_clear(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_MAP, "map clear");

    VSMapObject *map = (VSMapObject *)obj;
    for (auto entry : map->_map)
    {
        // entry.first is not modifiable, so just deleting it but not setting the pointer to NULL,
        // which means there will be DANGLING POINTERS!!!
        decref(entry.first);
        decref_ex(entry.second);
    }
    map->_map.clear();
}

VSObject *vs_map_len(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_MAP, "map len");

    return vs_int_from_cint(((VSMapObject *)obj)->_map.size());
}

VSObject *vs_map_get(VSObject *obj, VSObject *key)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_MAP, "map get");

    VSMapObject *map = (VSMapObject *)obj;
    auto iter = map->_map.find(key);
    if (iter != map->_map.end())
    {
        return iter->second;
    }
    else
    {
        // err("key \"%s\" not found.", vs_str_to_cstr(vs_typeof(key)->__str__(key)));
        terminate(TERM_ERROR);
    }
    return NULL;
}

void vs_map_set(VSObject *obj, VSObject *key, VSObject *value)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_MAP, "map put");

    VSMapObject *map = (VSMapObject *)obj;
    auto iter = map->_map.find(key);
    if (iter != map->_map.end())
    {
        iter->second = value;
        incref(value);
    }
    else
    {
        map->_map[key] = value;
        incref(key);
        incref(value);
    }
}

VSObject *vs_map_has_at(VSObject *obj, VSObject *key)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_MAP, "map get");

    VSMapObject *map = (VSMapObject *)obj;
    auto iter = map->_map.find(key);
    return vs_bool_from_cbool(iter != map->_map.end());
}

void vs_map_remove_at(VSObject *obj, VSObject *key)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_MAP, "map get");

    VSMapObject *map = (VSMapObject *)obj;
    auto iter = map->_map.find(key);
    if (iter != map->_map.end())
    {
        decref(iter->first);
        decref_ex(iter->second);
        map->_map.erase(iter);
    }
}

VSObject *vs_map_str(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_MAP, "map to str");

    return NULL;
}

VSObject *vs_map_bytes(VSObject *obj)
{
    VSTypeObject *type = vs_typeof(obj);

    vs_ensure_type(type, T_MAP, "map to bytes");

    return NULL;
}

ContainerFuncs *map_funcs = new ContainerFuncs(
    vs_map_len,
    vs_map_get,
    vs_map_set,
    NULL,
    NULL,
    vs_map_has_at,
    NULL,
    vs_map_remove_at
);

VSTypeObject *VSMapType = new VSTypeObject(
    T_MAP,
    "map", // __name__
    NULL,  // __attrs__
    vs_map_new,  // __new__
    vs_map_init,  // __init__
    vs_map_copy,  // __copy__
    vs_map_clear,  // __clear__
    NULL,  // __getattr__
    NULL,  // __hasattr__
    NULL,  // __setattr__
    NULL,  // __removeattr__
    vs_hash_not_implemented,  // __hash__
    vs_default_eq,  // __eq__
    vs_map_str,  // __str__
    vs_map_bytes,  // __bytes__
    NULL,  // __call__
    NULL,  // _number_funcs
    map_funcs   // _container_funcs
);