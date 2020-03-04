#ifndef VS_STRING_H
#define VS_STRING_H

#include <string>
#include "VSObject.hpp"

class VSStringObject;

VSTypeObject *VSStringType;

std::string vs_string_to_cstring(VSObject *strobj);
VSObject *vs_string_from_cstring(std::string strval);

#endif