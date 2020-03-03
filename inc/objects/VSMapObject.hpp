#ifndef VS_MAP_H
#define VS_MAP_H

#include "VSObject.hpp"

class VSMapObject;
extern VSTypeObject *VSMapType;

#define vs_map_len(map) vs_typeof(map)->_container_funcs->__len__(map)
#define vs_map_get(map, key) vs_typeof(map)->_container_funcs->__get__(map, key)
#define vs_map_put(map, key, value) vs_typeof(map)->_container_funcs->__put__(map, key, value)
#define vs_map_contains(map, key) vs_typeof(map)->_container_funcs->__contains__(map, key)
#define vs_map_remove(map, key) vs_typeof(map)->_container_funcs->__remove__(map, key)

#endif