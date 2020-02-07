#ifndef VALUE_H
#define VALUE_H

#include "vsc.h"

class Value
{
public:
    const TYPE type;
    const union {
        bool bool_val;
        char char_val;
        long long int_val;
        double float_val;
        void *none_val = NULL;
        std::vector<char> *str_val;
        std::vector<Value *> *list_val;
    };

    Value();
    Value(bool value);
    Value(char value);
    Value(long long value);
    Value(double value);
    Value(char *value);
    Value(std::vector<Value *> *value);
    ~Value();
};

#endif