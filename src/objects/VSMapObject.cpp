#include "error.hpp"
#include "objects/VSStringObject.hpp"
#include "objects/VSMapObject.hpp"

VSObject *vs_map_new(VSObject *typeobj, VSObject *args, VSObject *)
{
    INCREF_RET(VS_AS_OBJECT(new VSMapObject()));
}

void vs_map_init(VSObject *, VSObject *, VSObject *)
{
}

VSObject *vs_map_copy(const VSObject *that)
{
    VSTypeObject *type = VS_TYPEOF(that);

    VS_ENSURE_TYPE(type, T_MAP, "map copy");

    VSMapObject *map = (VSMapObject *)that;
    VSMapObject *new_map = new VSMapObject();
    for (auto entry : map->_map)
    {
        new_map->_map[entry.first] = entry.second;
        INCREF(entry.first);
        INCREF(entry.second);
    }
    INCREF_RET(VS_AS_OBJECT(new_map));
}

void vs_map_clear(VSObject *obj)
{
    VSTypeObject *type = VS_TYPEOF(obj);

    VS_ENSURE_TYPE(type, T_MAP, "map clear");

    VSMapObject *map = (VSMapObject *)obj;
    for (auto entry : map->_map)
    {
        // entry.first is not modifiable, so just deleting it but not setting the pointer to NULL,
        // which means there will be DANGLING POINTERS!!!
        DECREF(entry.first);
        DECREF_EX(entry.second);
    }
    map->_map.clear();
}

VSObject *vs_map_len(VSObject *obj)
{
    VSTypeObject *type = VS_TYPEOF(obj);

    VS_ENSURE_TYPE(type, T_MAP, "map len");

    INCREF_RET(
        C_INT_TO_INT(
            ((VSMapObject *)obj)->_map.size()));
}

VSObject *vs_map_get(VSObject *obj, VSObject *key)
{
    VSTypeObject *type = VS_TYPEOF(obj);

    VS_ENSURE_TYPE(type, T_MAP, "map get");

    VSMapObject *map = (VSMapObject *)obj;
    auto iter = map->_map.find(key);
    if (iter != map->_map.end())
    {
        INCREF_RET(iter->second);
    }
    else
    {
        err("key \"%s\" not found.", STRING_TO_C_STRING(VS_TYPEOF(key)->__str__(key)).c_str());
        terminate(TERM_ERROR);
    }
    return NULL;
}

void vs_map_set(VSObject *obj, VSObject *key, VSObject *value)
{
    VSTypeObject *type = VS_TYPEOF(obj);

    VS_ENSURE_TYPE(type, T_MAP, "map put");

    VSMapObject *map = (VSMapObject *)obj;
    auto iter = map->_map.find(key);
    if (iter != map->_map.end())
    {
        iter->second = value;
        INCREF(value);
    }
    else
    {
        map->_map[key] = value;
        INCREF(key);
        INCREF(value);
    }
}

VSObject *vs_map_has_at(VSObject *obj, VSObject *key)
{
    VSTypeObject *type = VS_TYPEOF(obj);

    VS_ENSURE_TYPE(type, T_MAP, "map get");

    VSMapObject *map = (VSMapObject *)obj;
    auto iter = map->_map.find(key);
    INCREF_RET(C_BOOL_TO_BOOL(iter != map->_map.end()));
}

void vs_map_remove_at(VSObject *obj, VSObject *key)
{
    VSTypeObject *type = VS_TYPEOF(obj);

    VS_ENSURE_TYPE(type, T_MAP, "map get");

    VSMapObject *map = (VSMapObject *)obj;
    auto iter = map->_map.find(key);
    if (iter != map->_map.end())
    {
        DECREF(iter->first);
        DECREF_EX(iter->second);
        map->_map.erase(iter);
    }
}

VSObject *vs_map_str(VSObject *obj)
{
    VSTypeObject *type = VS_TYPEOF(obj);

    VS_ENSURE_TYPE(type, T_MAP, "map to str");

    INCREF_RET(C_STRING_TO_STRING("map"));
}

VSObject *vs_map_bytes(VSObject *obj)
{
    VSTypeObject *type = VS_TYPEOF(obj);

    VS_ENSURE_TYPE(type, T_MAP, "map to bytes");

    return NULL;
}

ContainerFuncs *map_container_funcs = new ContainerFuncs(
    vs_map_len,      // __len__
    vs_map_get,      // __get__
    vs_map_set,      // __set__
    NULL,            // __append__
    NULL,            // __has__
    vs_map_has_at,   // __has_at__
    NULL,            // __remove__
    vs_map_remove_at // __remove_at__
);

VSTypeObject *VSMapType = new VSTypeObject(
    VSTypeType,
    T_MAP,
    "map",                   // __name__
    NULL,                    // __attrs__
    vs_map_new,              // __new__
    vs_map_init,             // __init__
    vs_map_copy,             // __copy__
    vs_map_clear,            // __clear__
    NULL,                    // __getattr__
    NULL,                    // __hasattr__
    NULL,                    // __setattr__
    NULL,                    // __removeattr__
    vs_hash_not_implemented, // __hash__
    NULL,                    // __lt__
    vs_default_eq,           // __eq__
    vs_map_str,              // __str__
    vs_map_bytes,            // __bytes__
    NULL,                    // __call__
    NULL,                    // _number_funcs
    map_container_funcs      // _container_funcs
);