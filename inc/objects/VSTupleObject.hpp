#ifndef VS_TUPLE_H
#define VS_TUPLE_H

#include "VSObject.hpp"
#include "error.hpp"

extern VSObject *vs_tuple(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSTupleObject : public VSObject {
private:
    static const str_func_map vs_tuple_methods;

public:
    vs_size_t nitems;
    VSObject **items;

    VSTupleObject(vs_size_t nitems);
    ~VSTupleObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
};

extern const VSTupleObject *_EMPTY_TUPLE;
extern VSTupleObject *vs_tuple_pack(vs_size_t nitems, ...);
extern int vs_tuple_unpack(VSObject *tupleobj, vs_size_t nitems, ...);
extern VSObject *vs_tuple_to_list(VSObject *tupleobj);

#define EMPTY_TUPLE() NEW_REF(VSTupleObject *, _EMPTY_TUPLE)

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