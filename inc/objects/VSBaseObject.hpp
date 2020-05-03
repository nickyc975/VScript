#ifndef VS_BASE_OBJECT_H
#define VS_BASE_OBJECT_H

#include <unordered_map>

#include "VSObject.hpp"

VSObject *vs_object(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSBaseObject : public VSObject {
private:
    static const str_func_map vs_object_methods;

public:
    std::unordered_map<std::string, VSObject *> attrs;

    VSBaseObject();
    ~VSBaseObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
};

#endif