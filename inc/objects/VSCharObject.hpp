#ifndef VS_CHAR_H
#define VS_CHAR_H

#include "VSObject.hpp"

class VSCharObject;

VSTypeObject *VSCharType;

char vs_char_to_cchar(VSObject * charobj);
VSObject *vs_char_from_cchar(char charval);

#endif