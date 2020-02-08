#ifndef VSC_TOKEN_H
#define VSC_TOKEN_H

#include "../vsc.hpp"
#include "Value.hpp"

class Token
{
public:
    long long ln, col;
    KIND kind;
    char *identifier;
    Value *value;

    Token(long long ln, long long col) : ln(ln), col(col)
    {
    }

    ~Token()
    {
        if (this->value != NULL)
        {
            delete this->value;
        }

        if (this->identifier != NULL)
        {
            free(this->identifier);
        }
    }
};

#endif