#include <unordered_map>

#include "objects/VSMapObject.hpp"

size_t hash(const VSObject *obj)
{

}

bool equal_to(const VSObject *a, const VSObject *b)
{

}

VSObject *map_new()
{
    return vs_as_object(new VSMapObject());
}

class VSMapObject : public VSObject
{
public:
    std::unordered_map<VSObject *, VSObject *, decltype(&hash), decltype(&equal_to)> map;

    VSMapObject() {}
};

VSTypeObject *VSMapType = new VSTypeObject(
    "map", // __name__
    NULL,  // __attrs__
    map_new,  // __new__
    NULL,  // __init__
    NULL,  // __copy__
    NULL,  // __clear__
    NULL,  // __hash__
    NULL,  // __eq__
    NULL,  // __neq__
    NULL,  // __str__
    NULL,  // __bytes__
    NULL,  // __call__
    NULL,  // _number_funcs
    NULL   // _container_funcs
);