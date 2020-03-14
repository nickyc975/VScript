#ifndef VS_FUNCTION_H
#define VS_FUNCTION_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

class VSFunctionObject : public VSObject {
public:
    /* | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
     * |f|r|nargs|c|m|                     unused                      |
     */
    unsigned int flags;
    VSObject *name;
    VSObject *code;
    VSObject *closure;
    VSObject *builtins;
    VSObject *defaults;

    // for builtin functions
    void *cfunc;

    VSFunctionObject(
        VSObject *name,
        void *cfunc,
        VSObject *closure,
        int nargs,
        bool has_retval,
        bool is_method);

    VSFunctionObject(
        VSObject *name, 
        VSObject *code, 
        VSObject *closure, 
        VSObject *builtins, 
        VSObject *defaults, 
        bool is_method);

    ~VSFunctionObject() = default;

    static VSObject *call(VSFunctionObject *callable, VSObject *args, VSObject *kwargs);
};

extern VSTypeObject *VSFunctionType;

#define AS_FUNC(func) ((VSFunctionObject *)func)

#define FUNC_IS_C_FUNC(func) ((AS_FUNC(func)->flags >> 31) & 1)
#define FUNC_SET_IS_C_FUNC(func) (AS_FUNC(func)->flags | (1 << 31))
#define FUNC_CLEAR_IS_C_FUNC(func) (AS_FUNC(func)->flags & ~(1 << 31))

#define FUNC_HAS_RETVAL(func) ((AS_FUNC(func)->flags >> 30) & 1)
#define FUNC_SET_HAS_RETVAL(func) (AS_FUNC(func)->flags | (1 << 30))
#define FUNC_CLEAR_HAS_REVAL(func) (AS_FUNC(func)->flags & ~(1 << 30))

#define FUNC_GET_NARGS(func) ((AS_FUNC(func)->flags >> 27) & 7)
#define FUNC_SET_NARGS(func, v) (AS_FUNC(func)->flags | (v << 27))

#define FUNC_USE_CLOSURE(func) ((AS_FUNC(func)->flags >> 26) & 1)
#define FUNC_SET_USE_CLOSURE(func) (AS_FUNC(func)->flags | (1 << 26))
#define FUNC_CLEAR_USE_CLOSURE(func) (AS_FUNC(func)->flags & ~(1 << 26))

#define FUNC_IS_METHOD(func) ((AS_FUNC(func)->flags >> 25) & 1)
#define FUNC_SET_IS_METHOD(func) (AS_FUNC(func)->flags | (1 << 25))
#define FUNC_CLEAR_IS_METHOD(func) (AS_FUNC(func)->flags & ~(1 << 25))

#define FUNC_GET_NAME(func) (AS_FUNC(func)->name)
#define FUNC_SET_NAME(func, fname) (AS_FUNC(func)->name = fname)

#define FUNC_GET_CODE(func) (AS_FUNC(func)->code)
#define FUNC_SET_CODE(func, fcode) (AS_FUNC(func)->code = fcode)

#define FUNC_GET_CLOSURE(func) (AS_FUNC(func)->closure)
#define FUNC_SET_CLOSURE(func, fclosure) (AS_FUNC(func)->closure = fclosure)

#define FUNC_GET_BUILTINS(func) (AS_FUNC(func)->builtins)
#define FUNC_SET_BUILTINS(func, fbuiltins) (AS_FUNC(func)->builtins = fbuiltins)

#define FUNC_GET_DEFAULTS(func) (AS_FUNC(func)->defaults)
#define FUNC_SET_DEFAULTS(func, fdefaults) (AS_FUNC(func)->defaults = fdefaults)

#define FUNC_GET_C_FUNC(func) (AS_FUNC(func)->cfunc)
#define FUNC_SET_C_FUNC(func, fcfunc) (AS_FUNC(func)->cfunc = fcfunc)

#endif