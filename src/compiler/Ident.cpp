#include "Ident.h"

Ident::Ident(char *name, bool is_mut)
{
    this->name = (char *)malloc(strlen(name) + 1);
    strcpy(this->name, name);
    this->is_mut = is_mut;
    this->value = NULL;
    this->type = UNKNOWN;
}

Ident::~Ident()
{
    free(this->name);
}

const char *Ident::getname()
{
    return this->name;
}

const bool Ident::is_mutable()
{
    return this->is_mut;
}

Value *Ident::getvalue()
{
    return this->value;
}

void Ident::assign(Value *value)
{
    if (!this->is_mut)
    {
        // error: immutable
    }
    this->value = value;
    this->type = UNKNOWN;
    if (value != NULL)
    {
        this->type = value->type;
    }
}