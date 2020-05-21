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

public:
    VSObject *self;
    vs_native_func func;

    VSNativeFunctionObject(VSObject *self, VSStringObject *name, vs_native_func func);
    ~VSNativeFunctionObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;

    VSObject *call(VSTupleObject *args) override;
};

class VSDynamicFunctionObject : public VSFunctionObject {
private:
    static const str_func_map vs_dynamic_func_methods;

public:
    VSCodeObject *code;
    VSTupleObject *cellvars;
    VSTupleObject *freevars;

    int flags;

    VSDynamicFunctionObject(VSStringObject *name, VSCodeObject *code, VSTupleObject *freevars, int flags);
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

    VSObject *func = obj->getattr(attrname);
    if (!IS_TYPE(func, T_FUNC)) {
        err("attribute \"%s\" of \"%s\" object is not function", attrname.c_str(), TYPE_STR[obj->type]);
        terminate(TERM_ERROR);
    }

    VSObject *res = ((VSFunctionObject *)func)->call(args);
    DECREF_EX(func);
    return res;
}

#define CALL_ATTR(obj, attrname, args) _CALL_ATTR(obj, attrname, args)

#endif