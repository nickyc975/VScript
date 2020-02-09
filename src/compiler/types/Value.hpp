#ifndef VSC_VALUE_H
#define VSC_VALUE_H

#include "../vsc.hpp"

class Value
{
public:
    TYPE type;
    union {
        bool bool_val;
        char char_val;
        long long int_val;
        double float_val;
        void *none_val = NULL;
        char *str_val;
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