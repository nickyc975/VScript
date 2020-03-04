#include "objects/VSTypeObject.hpp"

VSTypeObject *VSTypeType = new VSTypeObject(
    T_TYPE,
    "type", // __name__
    NULL,  // __attrs__
    NULL,  // __new__
    NULL,  // __init__
    NULL,  // __copy__
    NULL,  // __clear__
    NULL,  // __getattr__
    NULL,  // __hasattr__
    NULL,  // __setattr__
    NULL,  // __removeattr__
    NULL,  // __hash__
    NULL,  // __eq__
    NULL,  // __str__
    NULL,  // __bytes__
    NULL,  // __call__
    NULL,  // _number_funcs
    NULL   // _container_funcs
);