#ifndef VS_BYTES_H
#define VS_BYTES_H

#include <vector>

#include "VSObject.hpp"

extern VSObject *vs_bytes(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSBytesObject : public VSObject {
private:
    static const str_func_map vs_bytes_methods;

public:
    std::vector<cbyte_t> _value;

    VSBytesObject(vs_size_t len);
    ~VSBytesObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
};

inline void vs_bytes_append(vs_size_t len, ...);

#define AS_BYTES(obj) ((VSBytesObject *)obj)
#define BYTES_LEN(obj) (AS_BYTES(obj)->_value.size())

#endif