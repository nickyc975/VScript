#ifndef VS_CELL_H
#define VS_CELL_H

#include "VSObject.hpp"

extern VSObject *vs_cell(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSCellObject : public VSObject {
public:
    bool mut;
    VSObject *item;

    VSCellObject(VSObject *item);
    ~VSCellObject();
};

#define AS_CELL(obj) ((VSCellObject *)obj)
#define VS_CELL_GET(cell) NEW_REF(VSObject *, AS_CELL(cell)->item)
#define VS_CELL_SET(cell, value)                          \
    do {                                                  \
        VSObject *item = AS_CELL(cell)->item;             \
        if (!AS_CELL(cell)->mut) {                        \
            err("modifying immutable cell at %p", cell);  \
            break;                                        \
        }                                                 \
        if (item != NULL) {                               \
            DECREF(item);                                 \
        }                                                 \
        AS_CELL(cell)->item = NEW_REF(VSObject *, value); \
    } while (0);

#endif