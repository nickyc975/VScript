#ifndef VS_TUPLE_H
#define VS_TUPLE_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

class VSTupleObject;

extern VSTypeObject *VSTupleType;

extern VSObject *vs_tuple_pack(vs_size_t nitems, ...);
extern void vs_tuple_unpack(VSObject *tupleobj, vs_size_t nitems, ...);

#endif