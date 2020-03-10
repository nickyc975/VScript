#ifndef VS_LIST_H
#define VS_LIST_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

class VSListObject;

extern VSTypeObject *VSListType;

extern VSObject *vs_list_pack(vs_size_t nitems, ...);
extern int vs_list_unpack(VSObject *listobj, vs_size_t nitems, ...);

#endif