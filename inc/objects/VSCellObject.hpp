#ifndef VS_CELL_H
#define VS_CELL_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

extern VSTypeObject *VSCellType;

class VSCellObject : public VSObject {
public:
    VSObject *item;

    VSCellObject(VSObject *item);
    ~VSCellObject() = default;
};

extern VSObject *vs_cell_get(VSObject *cellobj);
extern int vs_cell_set(VSObject *cellobj, VSObject *item);

#define AS_CELL(obj) ((VSCellObject *)obj)
#define VS_CELL_GET(cell) NEW_REF(VSObject *, AS_CELL(cell)->item)
#define VS_CELL_SET(cell, value)                          \
    do {                                                  \
        VSObject *item = AS_CELL(cell)->item;             \
        if (item != NULL) {                               \
            DECREF(item);                                 \
        }                                                 \
        AS_CELL(cell)->item = NEW_REF(VSObject *, value); \
    } while(0);

#endif