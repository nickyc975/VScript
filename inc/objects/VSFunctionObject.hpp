#ifndef VS_FUNCTION_H
#define VS_FUNCTION_H

#include "VSCodeObject.hpp"
#include "VSNoneObject.hpp"
#include "VSObject.hpp"
#include "VSTupleObject.hpp"
#include "error.hpp"

class VSFunctionObject : public VSObject {
public:
    VSStringObject *name;

    VSFunctionObject();
    ~VSFunctionObject();

    virtual VSObject *call(VSTupleObject *);
};

class VSNativeFunctionObject : public VSFunctionObject {
private:
    static const str_func_map vs_native_func_methods;

    VSObject *self;
    vs_native_func func;

public:
    VSNativeFunctionObject(VSObject *self, VSStringObject *name, vs_native_func func);
    ~VSNativeFunctionObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;

    VSObject *call(VSTupleObject *args) override;
};

#define VS_FUNC_VARARGS 0x1

class VSDynamicFunctionObject : public VSFunctionObject {
private:
    static const str_func_map vs_dynamic_func_methods;

    VSCodeObject *code;
    VSTupleObject *cellvars;
    VSTupleObject *freevars;
    VSTupleObject *builtins;

    int flags;

public:
    VSDynamicFunctionObject(
        VSStringObject *name,
        VSCodeObject *code,
        VSTupleObject *freevars,
        VSTupleObject *builtins,
        int flags);
    ~VSDynamicFunctionObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;

    VSObject *call(VSTupleObject *args) override;
};

inline VSObject *_CALL_ATTR(VSObject *obj, std::string &attrname, VSTupleObject *args) {
    if (!obj->hasattr(attrname)) {
        ERR_NO_ATTR(obj, attrname);
        terminate(TERM_ERROR);
    }

    VSObject *_func = obj->getattr(attrname);
    if (!IS_TYPE(_func, T_FUNC)) {
        err("attribute \"%s\" of \"%s\" object is not function", attrname.c_str(), TYPE_STR[obj->type]);
        terminate(TERM_ERROR);
    }

    return ((VSFunctionObject *)_func)->call(args);
}

#define CALL_ATTR(obj, attrname, args) _CALL_ATTR(obj, attrname, args)

#endif