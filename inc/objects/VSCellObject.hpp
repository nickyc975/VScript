#ifndef VS_CELL_H
#define VS_CELL_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

extern VSTypeObject *VSCellType;

class VSCellObject : public VSObject {
public:
    bool mut;
    VSObject *item;

    VSCellObject(VSObject *item) {
        this->mut = true;
        this->item = item;
        this->type = VSCellType;
        INCREF(item);
        INCREF(VSCellType);
    }

    ~VSCellObject() = default;
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