#ifndef VS_CHAR_H
#define VS_CHAR_H

#include "VSObject.hpp"
#include "VSTypeObject.hpp"

class VSCharObject;

extern VSTypeObject *VSCharType;

static inline cchar_t vs_char_to_cchar(VSObject * charobj);
static inline VSObject *vs_char_from_cchar(cchar_t charval);

#endif