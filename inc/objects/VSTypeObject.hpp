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
        const_binaryfunc __eq__,
        unaryfunc __str__,
        unaryfunc __bytes__,
        ternaryfunc __call__,
        NumberFuncs *_number_funcs,
        ContainerFuncs *_container_funcs
    ) : t_type(t_type),
        __name__(__name__),
        __attrs__(__attrs__),
        __new__(__new__),
        __init__(__init__),
        __copy__(__copy__),
        __clear__(__clear__),
        __getattr__(__getattr__),
        __hasattr__(__hasattr__),
        __setattr__(__setattr__),
        __removeattr__(__removeattr__),
        __hash__(__hash__),
        __eq__(__eq__),
        __str__(__str__),
        __bytes__(__bytes__),
        __call__(__call__),
        _number_funcs(_number_funcs),
        _container_funcs(_container_funcs)
    {
    }
};

extern VSTypeObject *VSTypeType;

#endif