#include "objects/VSTupleObject.hpp"

#include <cstdarg>
#include <cstring>

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"

const VSTupleObject *_EMPTY_TUPLE = vs_tuple_pack(0);

VSObject *vs_tuple(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs == 0) {
        INCREF_RET(new VSTupleObject(0));
    } else if (nargs == 1) {
        VSObject *obj = args[0];
        if (obj->type == T_TUPLE) {
            INCREF_RET(obj);
        } else if (obj->type == T_LIST) {
            return vs_list_to_tuple(obj);
        } else {
            err("can not cast \"%s\" object to tuple", TYPE_STR[obj->type]);
            INCREF_RET(VS_NONE);
        }
    }

    ERR_NARGS("tuple()", 1, nargs);
    terminate(TERM_ERROR);
}

VSObject *vs_tuple_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("tuple.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_TUPLE, "tuple.__str__()");

    INCREF_RET(C_STRING_TO_STRING("tuple"));
}

VSObject *vs_tuple_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("tuple.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_TUPLE, "tuple.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_tuple_add(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("tuple.__add__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    VS_ENSURE_TYPE(self, T_TUPLE, "tuple.__add__()");
    VS_ENSURE_TYPE(that, T_TUPLE, "tuple.__add__()");

    VSTupleObject *tuple_a = (VSTupleObject *)self;
    VSTupleObject *tuple_b = (VSTupleObject *)that;

    VSTupleObject *tuple = new VSTupleObject(tuple_a->nitems + tuple_b->nitems);
    for (vs_size_t i = 0; i < tuple_a->nitems; i++) {
        VSObject *item = tuple_a->items[i];
        tuple->items[i] = item;
        INCREF(item);
    }

    for (vs_size_t i = 0; i < tuple_b->nitems; i++) {
        VSObject *item = tuple_b->items[i];
        tuple->items[tuple_a->nitems + i] = item;
        INCREF(item);
    }

    INCREF_RET(VS_AS_OBJECT(tuple));
}

VSObject *vs_tuple_copy(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("tuple.__copy__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_TUPLE, "tuple.__copy__()");

    VSTupleObject *old_tuple = (VSTupleObject *)self;
    VSTupleObject *new_tuple = new VSTupleObject(old_tuple->nitems);

    for (vs_size_t i = 0; i < new_tuple->nitems; i++) {
        VSObject *item = old_tuple->items[i];
        new_tuple->items[i] = item;
        INCREF(item);
    }
    INCREF_RET(VS_AS_OBJECT(new_tuple));
}

VSObject *vs_tuple_len(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("tuple.__len__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_TUPLE, "tuple.__len__()");

    cint_t len = ((VSTupleObject *)self)->nitems;
    INCREF_RET(C_INT_TO_INT(len));
}

VSObject *vs_tuple_get(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("tuple.get()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    VS_ENSURE_TYPE(self, T_TUPLE, "tuple.get()");
    VS_ENSURE_TYPE(idxobj, T_INT, "as tuple index");

    VSTupleObject *tuple = (VSTupleObject *)self;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);
    if (idx >= tuple->nitems) {
        INDEX_OUT_OF_BOUND(idx, tuple->nitems);
        terminate(TERM_ERROR);
    }

    INCREF_RET(tuple->items[idx]);
}

// TODO: implement tuple.__has__()
VSObject *vs_tuple_has(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("tuple.has()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *item = args[0];
    VS_ENSURE_TYPE(self, T_TUPLE, "tuple.has()");

    INCREF_RET(VS_FALSE);
}

VSObject *vs_tuple_has_at(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("tuple.has_at()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    VS_ENSURE_TYPE(self, T_TUPLE, "tuple.has_at()");
    VS_ENSURE_TYPE(idxobj, T_INT, "as tuple index");

    VSTupleObject *tuple = (VSTupleObject *)self;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);

    INCREF_RET(idx < tuple->nitems ? VS_TRUE : VS_FALSE);
}

VSTupleObject *vs_tuple_pack(vs_size_t nitems, ...) {
    VSTupleObject *tuple = new VSTupleObject(nitems);

    va_list args;
    va_start(args, nitems);
    for (vs_size_t i = 0; i < nitems; i++) {
        VSObject *item = va_arg(args, VSObject *);
        tuple->items[i] = item;
        INCREF(item);
    }
    va_end(args);

    INCREF_RET(tuple);
}

int vs_tuple_unpack(VSObject *tupleobj, vs_size_t nitems, ...) {
    VS_ENSURE_TYPE(tupleobj, T_TUPLE, "tuple unpack");

    VSTupleObject *tuple = (VSTupleObject *)tupleobj;
    if (nitems >= tuple->nitems) {
        INDEX_OUT_OF_BOUND(nitems, tuple->nitems);
        return -1;
    }

    va_list args;
    va_start(args, nitems);
    for (vs_size_t i = 0; i < nitems; i++) {
        VSObject **item = va_arg(args, VSObject **);
        *item = tuple->items[i];
        INCREF(*item);
    }
    va_end(args);
    return 0;
}

VSObject *vs_tuple_to_list(VSObject *tupleobj) {
    VS_ENSURE_TYPE(tupleobj, T_TUPLE, "tuple to list");

    VSTupleObject *tuple = (VSTupleObject *)tupleobj;

    vs_size_t len = TUPLE_LEN(tuple);
    VSListObject *list = new VSListObject(len);
    for (vs_size_t i = 0; i < len; i++) {
        VSObject *item = TUPLE_GET(tuple, i);
        LIST_SET(list, i, item);
    }
    INCREF_RET(list);
}

VSTupleObject::VSTupleObject(vs_size_t nitems) {
    this->type = T_TUPLE;
    this->nitems = nitems;

    size_t size = sizeof(VSObject *) * nitems;
    this->items = (VSObject **)malloc(size);
    if (this->items == NULL) {
        err("unable to malloc memory of size: %lu\n", size);
        terminate(TERM_ERROR);
    }
    memset(this->items, NULL, size);

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_default_hash);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_default_eq);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_tuple_str);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_tuple_bytes);
    NEW_NATIVE_FUNC_ATTR(this, "__add__", vs_tuple_add);
    NEW_NATIVE_FUNC_ATTR(this, "copy", vs_tuple_copy);
    NEW_NATIVE_FUNC_ATTR(this, "len", vs_tuple_len);
    NEW_NATIVE_FUNC_ATTR(this, "get", vs_tuple_get);
    NEW_NATIVE_FUNC_ATTR(this, "has_at", vs_tuple_has_at);
}

VSTupleObject::~VSTupleObject() {
    for (vs_size_t i = 0; i < this->nitems; i++) {
        DECREF(this->items[i]);
    }
    free(this->items);
}