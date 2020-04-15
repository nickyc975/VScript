#include "objects/VSBaseObject.hpp"

VSObject *vs_object_new(VSObject *, VSObject *, VSObject *) {
    INCREF_RET(new VSBaseObject(NULL));
}

void vs_object_init(VSObject *, VSObject *, VSObject *) {
}

VSObject *vs_object_copy(const VSObject *oldobj) {
    return NULL;
}

void vs_object_clear(VSObject *obj) {
}

VSObject *vs_object_getattr(VSObject *obj, VSObject *attrname) {
    return NULL;
}

VSObject *vs_object_hasattr(VSObject *obj, VSObject *attrname) {
    return NULL;
}

void vs_object_setattr(VSObject *obj, VSObject *attrname, VSObject *value) {
}

void vs_object_removeattr(VSObject *obj, VSObject *attrname) {
}

VSObject *vs_object_str(VSObject *obj) {
    return NULL;
}

VSObject *vs_object_bytes(VSObject *obj) {
    return NULL;
}

VSTypeObject *VSBaseObjectType = new VSTypeObject(
    VSTypeType,
    T_OBJECT,
    "object",                 // __name__
    NULL,                     // __attrs__
    vs_object_new,            // __new__
    vs_object_init,           // __init__
    vs_object_copy,           // __copy__
    vs_object_clear,          // __clear__
    vs_object_getattr,        // __getattr__
    vs_object_hasattr,        // __hasattr__
    vs_object_setattr,        // __setattr__
    vs_object_removeattr,     // __removeattr__
    vs_hash_not_implemented,  // __hash__
    NULL,                     // __lt__
    vs_default_eq,            // __eq__
    vs_object_str,            // __str__
    vs_object_bytes,          // __bytes__
    NULL,                     // __call__
    NULL,                     // _number_funcs
    NULL                      // _container_funcs
);