#ifndef VS_STRING_H
#define VS_STRING_H

#include <string>

#include "VSObject.hpp"

extern VSObject *vs_str(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSStringObject : public VSObject {
public:
    std::string _value;

    VSStringObject(std::string value);
    ~VSStringObject();
};

#define AS_STRING(obj) ((VSStringObject *)obj)
#define C_STRING_TO_STRING(str) (new VSStringObject(str))
#define STRING_TO_C_STRING(obj) (AS_STRING(obj)->_value)

// convinient macros for string operations
#define STRING_LEN(obj) (AS_STRING(obj)->_value.length())
#define STRING_GET(obj, idx) (AS_STRING(obj)->_value[idx])
#define STRING_SET(obj, idx, val) (AS_STRING(obj)->_value[idx] = val)
#define STRING_APPEND(obj, val) (AS_STRING(obj)->_value.push_back(val))

#endif