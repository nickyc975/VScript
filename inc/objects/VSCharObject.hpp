#ifndef VS_CHAR_H
#define VS_CHAR_H

#include "VSObject.hpp"

class VSCharObject;

extern VSTypeObject *VSCharType;

cchar_t vs_char_to_cchar(VSObject * charobj);
VSObject *vs_char_from_cchar(cchar_t charval);

#endif