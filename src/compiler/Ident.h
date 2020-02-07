#ifndef IDENT_H
#define IDENT_H

#include "vsc.h"
#include "Value.h"

class Ident
{
private:
    char *name;
    bool is_mut;
    TYPE type;
    Value *value;

public:
    Ident(char *name, bool is_mut);
    ~Ident();
    const char *getname();
    const bool is_mutable();
    Value *getvalue();
    void assign(Value *value);
};

#endif