#ifndef VS_CHAR_H
#define VS_CHAR_H

#include "VSObject.hpp"

extern VSObject *vs_char(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSCharObject : public VSObject {
public:
    const cchar_t _value;

    VSCharObject(cchar_t value);
    ~VSCharObject();
};

#define AS_CHAR(obj) ((VSCharObject *)obj)
#define CHAR_TO_C_CHAR(obj) (AS_CHAR(obj)->_value)
#define C_CHAR_TO_CHAR(val) (new VSCharObject(val))

#endif