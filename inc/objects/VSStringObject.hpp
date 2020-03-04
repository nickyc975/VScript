#ifndef VS_STRING_H
#define VS_STRING_H

#include <string>
#include "VSObject.hpp"
#include "VSTypeObject.hpp"

class VSStringObject;

extern VSTypeObject *VSStringType;

static inline std::string vs_string_to_cstring(VSObject *strobj);
static inline VSObject *vs_string_from_cstring(std::string strval);

#endif