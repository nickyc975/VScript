#include "Value.hpp"

Value::Value(): type(NONE)
{
}

Value::Value(bool value): type(BOOL), bool_val(value)
{
}

Value::Value(char value): type(CHAR), char_val(value)
{
}

Value::Value(long long value): type(INT), int_val(value)
{
}

Value::Value(double value): type(FLOAT), float_val(value)
{
}

Value::Value(char *value): type(STRING), str_val(value)
{
}

Value::Value(std::vector<Value *> *value): type(LIST)
{
    this->list_val = value == NULL ? new std::vector<Value *>() : value;
}

Value::~Value()
{
    if (this->type == STRING)
    {
        delete this->str_val;
    }
    else if (this->type == LIST)
    {
        delete this->list_val;
    }
}