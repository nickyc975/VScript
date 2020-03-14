#ifndef VS_TYPE_H
#define VS_TYPE_H

#include "VSObject.hpp"

class VSTypeObject : public VSObject
{
public:
    TYPE_TYPE t_type;
    std::string __name__;

    VSObject *__attrs__;

    ternaryfunc __new__;
    void_ternaryfunc __init__;
    const_unaryfunc __copy__;
    void_unaryfunc __clear__;

    binaryfunc __getattr__;
    binaryfunc __hasattr__;
    void_ternaryfunc __setattr__;
    void_binaryfunc __removeattr__;

    const_unaryfunc __hash__;
    const_binaryfunc __lt__;
    const_binaryfunc __eq__;

    unaryfunc __str__;
    unaryfunc __bytes__;

    // function types
    ternaryfunc __call__;

    // number types
    NumberFuncs *_number_funcs;

    // container types
    ContainerFuncs *_container_funcs;

    VSTypeObject(
        VSTypeObject *type,
        TYPE_TYPE t_type,
        std::string __name__,
        VSObject *__attrs__,
        ternaryfunc __new__,
        void_ternaryfunc __init__,
        const_unaryfunc __copy__,
        void_unaryfunc __clear__,
        binaryfunc __getattr__,
        binaryfunc __hasattr__,
        void_ternaryfunc __setattr__,
        void_binaryfunc __removeattr__,
        const_unaryfunc __hash__,
        const_binaryfunc __lt__,
        const_binaryfunc __eq__,
        unaryfunc __str__,
        unaryfunc __bytes__,
        ternaryfunc __call__,
        NumberFuncs *_number_funcs,
        ContainerFuncs *_container_funcs
    );
};

extern VSTypeObject *VSTypeType;

#endif