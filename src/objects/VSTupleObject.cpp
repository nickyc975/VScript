#include "objects/VSTupleObject.hpp"

#include <cstdarg>
#include <cstring>

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSListObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSSetObject.hpp"
#include "objects/VSStringObject.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);
NEW_IDENTIFIER(__add__);
NEW_IDENTIFIER(copy);
NEW_IDENTIFIER(len);
NEW_IDENTIFIER(get);
NEW_IDENTIFIER(has_at);

VSTupleObject *VSTupleObject::_EMPTY_TUPLE = NULL;

VSObject *vs_tuple(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs == 0) {
        INCREF_RET(new VSTupleObject(0));
    } else if (nargs == 1) {
        VSObject *obj = args[0];
        if (obj->type == T_TUPLE) {
            INCREF_RET(obj);
        } else if (obj->type == T_LIST) {
            return vs_list_to_tuple(obj);
        } else if (obj->type == T_SET) {
            int i = 0;
            VSSetObject *set = (VSSetObject *)obj;
            VSTupleObject *tuple = new VSTupleObject(set->_set.size());
            for (auto item : set->_set) {
                TUPLE_SET(tuple, i, item);
                i++;
            }
            INCREF_RET(tuple);
        } else {
            err("can not cast \"%s\" object to tuple", TYPE_STR[obj->type]);
            INCREF_RET(VS_NONE);
        }
    }

    ERR_NARGS("tuple()", 1, nargs);
    terminate(TERM_ERROR);
    return NULL;
}

VSObject *vs_tuple_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("tuple.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_TUPLE, "tuple.__str__()");

    std::string tuple_str = "(";
    VSTupleObject *tuple = (VSTupleObject *)self;
    for (vs_size_t i = 0; i < tuple->nitems; i++) {
        VSObject *str = CALL_ATTR(tuple->items[i], ID___str__, EMPTY_TUPLE());
        tuple_str.append(STRING_TO_C_STRING(str));
        tuple_str.append(", ");
        DECREF(str);
    }
    if (tuple_str.back() == ' ') {
        tuple_str.pop_back();
        tuple_str.pop_back();
    }
    tuple_str.append(")");

    INCREF_RET(C_STRING_TO_STRING(tuple_str));
}

VSObject *vs_tuple_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("tuple.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_TUPLE, "tuple.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_tuple_add(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("tuple.__add__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    ENSURE_TYPE(self, T_TUPLE, "tuple.__add__()");
    ENSURE_TYPE(that, T_TUPLE, "tuple.__add__()");

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

    INCREF_RET(AS_OBJECT(tuple));
}

VSObject *vs_tuple_copy(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("tuple.__copy__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_TUPLE, "tuple.__copy__()");

    VSTupleObject *old_tuple = (VSTupleObject *)self;
    VSTupleObject *new_tuple = new VSTupleObject(old_tuple->nitems);

    for (vs_size_t i = 0; i < new_tuple->nitems; i++) {
        VSObject *item = old_tuple->items[i];
        new_tuple->items[i] = item;
        INCREF(item);
    }
    INCREF_RET(AS_OBJECT(new_tuple));
}

VSObject *vs_tuple_len(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("tuple.__len__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_TUPLE, "tuple.__len__()");

    cint_t len = ((VSTupleObject *)self)->nitems;
    INCREF_RET(C_INT_TO_INT(len));
}

VSObject *vs_tuple_get(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("tuple.get()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    ENSURE_TYPE(self, T_TUPLE, "tuple.get()");
    ENSURE_TYPE(idxobj, T_INT, "as tuple index");

    VSTupleObject *tuple = (VSTupleObject *)self;
    vs_size_t idx = (vs_size_t)INT_TO_C_INT(idxobj);
    if (idx >= tuple->nitems) {
        INDEX_OUT_OF_BOUND(idx, tuple->nitems);
        terminate(TERM_ERROR);
    }

    INCREF_RET(tuple->items[idx]);
}

// TODO: implement tuple.__has__()
VSObject *vs_tuple_has(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("tuple.has()", 1, nargs);
        terminate(TERM_ERROR);
    }

    // VSObject *item = args[0];
    ENSURE_TYPE(self, T_TUPLE, "tuple.has()");

    INCREF_RET(VS_FALSE);
}

VSObject *vs_tuple_has_at(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("tuple.has_at()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *idxobj = args[0];
    ENSURE_TYPE(self, T_TUPLE, "tuple.has_at()");
    ENSURE_TYPE(idxobj, T_INT, "as tuple index");

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
    ENSURE_TYPE(tupleobj, T_TUPLE, "tuple unpack");

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
    ENSURE_TYPE(tupleobj, T_TUPLE, "tuple to list");

    VSTupleObject *tuple = (VSTupleObject *)tupleobj;

    vs_size_t len = TUPLE_LEN(tuple);
    VSListObject *list = new VSListObject(len);
    for (vs_size_t i = 0; i < len; i++) {
        VSObject *item = TUPLE_GET(tuple, i);
        LIST_SET(list, i, item);
    }
    INCREF_RET(list);
}

const str_func_map VSTupleObject::vs_tuple_methods = {
    {ID___hash__, vs_default_hash},
    {ID___eq__, vs_default_eq},
    {ID___str__, vs_tuple_str},
    {ID___bytes__, vs_tuple_bytes},
    {ID___add__, vs_tuple_add},
    {ID_copy, vs_tuple_copy},
    {ID_len, vs_tuple_len},
    {ID_get, vs_tuple_get},
    {ID_has_at, vs_tuple_has_at}};

VSTupleObject::VSTupleObject(vs_size_t nitems) {
    this->type = T_TUPLE;
    this->nitems = nitems;

    size_t size = sizeof(VSObject *) * nitems;
    this->items = (VSObject **)malloc(size);
    if (this->items == NULL) {
        err("unable to malloc memory of size: %lu\n", size);
        terminate(TERM_ERROR);
    }
    memset(this->items, 0, size);
}

VSTupleObject::~VSTupleObject() {
    for (vs_size_t i = 0; i < this->nitems; i++) {
        DECREF(this->items[i]);
    }
    free(this->items);
}

bool VSTupleObject::hasattr(std::string &attrname) {
    return vs_tuple_methods.find(attrname) != vs_tuple_methods.end();
}

VSObject *VSTupleObject::getattr(std::string &attrname) {
    auto iter = vs_tuple_methods.find(attrname);
    if (iter == vs_tuple_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_tuple_methods.at(attrname));
    INCREF_RET(attr);
}

void VSTupleObject::setattr(std::string &, VSObject *) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}
