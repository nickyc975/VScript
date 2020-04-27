#ifndef VS_FUNCTION_H
#define VS_FUNCTION_H

#include "error.hpp"

#include "VSCodeObject.hpp"
#include "VSNoneObject.hpp"
#include "VSObject.hpp"
#include "VSTupleObject.hpp"

class VSFunctionObject : public VSObject {
public:
    std::string name;

    VSFunctionObject();
    ~VSFunctionObject();

    virtual VSObject *call(VSTupleObject *);
};

class VSNativeFunctionObject : public VSFunctionObject {
private:
    VSObject *self;
    vs_native_func func;

    VSNativeFunctionObject(std::string name, vs_native_func func);

public:
    VSNativeFunctionObject(std::string name, vs_native_func func, VSObject *self);
    ~VSNativeFunctionObject();

    VSObject *call(VSTupleObject *args) override;
};

#define VS_FUNC_VARARGS 0x1

class VSDynamicFunctionObject : public VSFunctionObject {
private:
    VSCodeObject *code;
    VSTupleObject *cellvars;
    VSTupleObject *freevars;
    VSTupleObject *builtins;

    int flags;

public:
    VSDynamicFunctionObject(
        std::string name,
        VSCodeObject *code,
        VSTupleObject *freevars,
        VSTupleObject *builtins,
        int flags);
    ~VSDynamicFunctionObject();

    VSObject *call(VSTupleObject *args) override;
};

inline VSObject *_CALL_ATTR(VSObject *obj, std::string attrname, VSTupleObject *args) {
    if (!HAS_ATTR(obj, attrname)) {
        ERR_ATTR_IS_NOT_FUNC(obj, attrname);
        terminate(TERM_ERROR);
    }

    VSObject *_func = GET_ATTR(obj, attrname);
    if (!VS_IS_TYPE(_func, T_FUNC)) {
        ERR_ATTR_IS_NOT_FUNC(obj, attrname);
        terminate(TERM_ERROR);
    }

    return ((VSFunctionObject *)_func)->call(args);
}

#define CALL_ATTR(obj, attrname, args) _CALL_ATTR(obj, attrname, args)

#define NEW_NATIVE_FUNC_ATTR(obj, attrname, func) \
    (obj)->attrs[(attrname)] = new AttributeDef(true, new VSNativeFunctionObject((attrname), (func), (obj)));

#endif