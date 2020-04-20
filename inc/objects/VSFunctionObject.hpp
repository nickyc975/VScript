#ifndef VS_FUNCTION_H
#define VS_FUNCTION_H

#include "VSCodeObject.hpp"
#include "VSNoneObject.hpp"
#include "VSObject.hpp"
#include "VSStringObject.hpp"
#include "VSTupleObject.hpp"

class VSFunctionObject : public VSObject {
public:
    VSStringObject *name;

    VSFunctionObject();
    ~VSFunctionObject();

    virtual VSObject *call(VSTupleObject *);
};

class VSNativeFunctionObject : public VSFunctionObject {
private:
    void *cfunc;
    cint_t nargs;
    bool const_args;
    VSObject *bind_obj;

public:
    VSNativeFunctionObject(VSStringObject *name, void *cfunc, cint_t nargs, bool const_args, VSObject *bind_obj);
    ~VSNativeFunctionObject();

    VSObject *call(VSTupleObject *args) override;
};

class VSDynamicFunctionObject : public VSFunctionObject {
private:
    VSCodeObject *code;
    VSTupleObject *cellvars;
    VSTupleObject *freevars;
    VSTupleObject *builtins;

public:
    VSDynamicFunctionObject(
        VSStringObject *name,
        VSCodeObject *code,
        VSTupleObject *cellvars,
        VSTupleObject *freevars,
        VSTupleObject *builtins);
    ~VSDynamicFunctionObject();

    VSObject *call(VSTupleObject *args) override;
};

#define NEW_NATIVE_FUNC_ATTR(obj, attrname, func, nargs, const_args) \
    (obj)->attrs[(attrname)] = AttributeDef(true, new VSNativeFunctionObject(C_STRING_TO_STRING(attrname), (void *)(func), (nargs), (const_args), (obj)));

#endif