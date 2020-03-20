#ifndef VS_TUPLE_H
#define VS_TUPLE_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

extern VSTypeObject *VSTupleType;

class VSTupleObject : public VSObject {
public:
    vs_size_t nitems;
    VSObject **items;

    VSTupleObject(vs_size_t nitems) {
        this->type = VSTupleType;

        this->nitems = nitems;
        this->items = (VSObject **)malloc(sizeof(VSObject *) * nitems);
        if (items == NULL) {
            err("unable to malloc memory of size: %llu\n", sizeof(VSObject *) * nitems);
            terminate(TERM_ERROR);
        }
    }

    ~VSTupleObject() {
        for (vs_size_t i = 0; i < this->nitems; i++) {
            DECREF(this->items[i]);
        }
        free(this->items);
    }
};

extern VSObject *vs_tuple_pack(vs_size_t nitems, ...);
extern int vs_tuple_unpack(VSObject *tupleobj, vs_size_t nitems, ...);
extern VSObject *vs_tuple_to_list(VSObject *tupleobj);

// convinient macros for tuple operations
#define AS_TUPLE(obj) ((VSTupleObject *)obj)
#define TUPLE_LEN(obj) (AS_TUPLE(obj)->nitems)
#define TUPLE_GET(obj, idx) (AS_TUPLE(obj)->items[idx])

#define TUPLE_SET(obj, idx, val)        \
    do {                                \
        auto _new = (val);              \
        INCREF(_new);                   \
        auto _tuple = AS_TUPLE(obj);    \
        auto _old = _tuple->items[idx]; \
        DECREF(_old);                   \
        _tuple->items[idx] = _new;      \
    } while (0);

#endif