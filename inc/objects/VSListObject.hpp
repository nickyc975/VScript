#ifndef VS_LIST_H
#define VS_LIST_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

#include <vector>

extern VSTypeObject *VSListType;

class VSListObject : public VSObject {
public:
    std::vector<VSObject *> items;

    VSListObject() {
        this->type = VSListType;
        this->items = std::vector<VSObject *>();
    }

    VSListObject(vs_size_t nitems) {
        this->type = VSListType;
        this->items = std::vector<VSObject *>(nitems);
    }

    ~VSListObject() = default;
};

extern VSObject *vs_list_pack(vs_size_t nitems, ...);
extern int vs_list_unpack(VSObject *listobj, vs_size_t nitems, ...);
extern VSObject *vs_list_to_tuple(VSObject *listobj);

// convinient macros for list operations
#define AS_LIST(obj) ((VSListObject *)obj)
#define LIST_LEN(obj) (AS_LIST(obj)->items.size())
#define LIST_GET(obj, idx) (AS_LIST(obj)->items[idx])

#define LIST_SET(obj, idx, val)        \
    do {                               \
        auto _new = (val);             \
        INCREF(_new);                  \
        auto _list = AS_LIST(obj);     \
        auto _old = _list->items[idx]; \
        DECREF(_old);                  \
        _list->items[idx] = _new;      \
    } while (0);

#define LIST_APPEND(obj, val)         \
    do {                              \
        auto _val = (val);            \
        INCREF(_val);                 \
        auto _list = AS_LIST(obj);    \
        _list->items.push_back(_val); \
    } while (0);

#endif