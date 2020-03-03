#include <unordered_map>

#include "error.hpp"
#include "objects/VSMapObject.hpp"

size_t hash(const VSObject *obj)
{

}

bool equal_to(const VSObject *a, const VSObject *b)
{

}

class VSMapObject : public VSObject
{
public:
    std::unordered_map<VSObject *, VSObject *, decltype(&hash), decltype(&equal_to)> map;

    VSMapObject() {}
};

VSObject *vs_map_new()
{
    return vs_as_object(new VSMapObject());
}

void vs_map_init(VSObject *, VSObject *, VSObject *)
{
}

VSObject *vs_map_copy(VSObject *that)
{
    VSTypeObject *type = vs_typeof(that);

    vs_ensure_type(type, T_MAP, "map copy");

    VSMapObject *map = (VSMapObject *)that;
    VSMapObject *new_map = (VSMapObject *)type->__new__();
    for (auto entry : map->map)
    {
        new_map->map[entry.first] = entry.second;
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
    for (auto entry : map->map)
    {
        // entry.first is not modifiable, so just deleting it but not setting the pointer to NULL,
        // which means there will be DANGLING POINTERS!!!
        entry.first->refcnt--;
        if (entry.first->refcnt == 0)
        {
            if (vs_typeof(entry.first)->__clear__ != NULL)
            {                                     
                vs_typeof(entry.first)->__clear__(entry.first);    
            }                                      
            delete entry.first; 
        }
        decref(entry.second);
    }
    map->map.clear();
}

ContainerFuncs *map_funcs = new ContainerFuncs(
    NULL,
    NULL,
    NULL,
    NULL
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
    NULL,  // __hash__
    NULL,  // __eq__
    NULL,  // __neq__
    NULL,  // __str__
    NULL,  // __bytes__
    NULL,  // __call__
    NULL,  // _number_funcs
    map_funcs   // _container_funcs
);