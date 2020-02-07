#include "vsc.h"
#include "Value.h"
#include "../utils/File.h"

#ifndef VSLEXER_H

#define VSLEXER_H

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

void tokenize(File *file, std::vector<Token *> &tokens);

#endif