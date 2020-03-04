#ifndef VS_STRING_H
#define VS_STRING_H

#include <string>
#include "VSObject.hpp"
#include "VSTypeObject.hpp"

class VSStringObject;

extern VSTypeObject *VSStringType;

extern std::string vs_string_to_cstring(VSObject *strobj);
extern VSObject *vs_string_from_cstring(std::string strval);

#endif