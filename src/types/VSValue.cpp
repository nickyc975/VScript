#include <cstring>
#include <string>
#include <vector>

#include "vs.hpp"
#include "error.hpp"

VSValue::VSValue() : type(NONE)
{
    
}

VSValue::VSValue(vs_bool_t val) : type(BOOL), bool_val(val)
{
    this->size = sizeof(*this);
}

VSValue::VSValue(vs_char_t val) : type(CHAR), char_val(val)
{
    this->size = sizeof(*this);
}

VSValue::VSValue(vs_int_t val) : type(INT), int_val(val)
{
    this->size = sizeof(*this);
}

VSValue::VSValue(vs_float_t val) : type(FLOAT), float_val(val)
{
    this->size = sizeof(*this);
}

VSValue::VSValue(std::string val) : type(STRING)
{
    this->str_val = new std::string(val);
    this->size = sizeof(*this) + sizeof(*this->str_val);
}

VSValue *VSValue::None()
{
    static VSValue NONE_VAL = VSValue();
    return &NONE_VAL;
}

VSValue *VSValue::True()
{
    static VSValue TRUE_VAL = VSValue(true);
    return &TRUE_VAL;
}

VSValue *VSValue::False()
{
    static VSValue FALSE_VAL = VSValue(false);
    return &FALSE_VAL;
}

VSValue *VSValue::copy(VSValue *old)
{
    switch (old->type)
    {
    case CHAR:
        return new VSValue(old->char_val);
    case INT:
        return new VSValue(old->int_val);
    case FLOAT:
        return new VSValue(old->float_val);
    case STRING:
        return new VSValue(old->str_val);
    default:
        return old;
    }
}

VSValue *VSValue::i_add(VSValue *left, VSValue *right)
{
    if (left->type != right->type || !is_num_str(left->type))
        b_op_err("+", TYPE_STR[left->type], TYPE_STR[right->type]);

    switch (left->type)
    {
    case CHAR:
        return new VSValue((vs_char_t)(left->char_val + right->char_val));
    case INT:
        return new VSValue((vs_int_t)(left->int_val + right->int_val));
    case FLOAT:
        return new VSValue((vs_float_t)(left->float_val + right->float_val));
    case STRING:
    {
        return new VSValue(*left->str_val + *right->str_val);
    }
    default:
        return VSValue::None();
    }
}

VSValue *VSValue::i_sub(VSValue *left, VSValue *right)
{
    if (left->type != right->type || !is_num(left->type))
        b_op_err("-", TYPE_STR[left->type], TYPE_STR[right->type]);

    switch (left->type)
    {
    case CHAR:
        return new VSValue((vs_char_t)(left->char_val - right->char_val));
    case INT:
        return new VSValue((vs_int_t)(left->int_val - right->int_val));
    case FLOAT:
        return new VSValue((vs_float_t)(left->float_val - right->float_val));
    default:
        return VSValue::None();
    }
}

VSValue *VSValue::i_mul(VSValue *left, VSValue *right)
{
    if (left->type != right->type || !is_num(left->type))
        b_op_err("*", TYPE_STR[left->type], TYPE_STR[right->type]);

    switch (left->type)
    {
    case CHAR:
        return new VSValue((vs_char_t)(left->char_val * right->char_val));
    case INT:
        return new VSValue((vs_int_t)(left->int_val * right->int_val));
    case FLOAT:
        return new VSValue((vs_float_t)(left->float_val * right->float_val));
    default:
        return VSValue::None();
    }
}

VSValue *VSValue::i_div(VSValue *left, VSValue *right)
{
    if (left->type != right->type || !is_num(left->type))
        b_op_err("/", TYPE_STR[left->type], TYPE_STR[right->type]);

    switch (left->type)
    {
    case CHAR:
        return new VSValue((vs_char_t)(left->char_val / right->char_val));
    case INT:
        return new VSValue((vs_int_t)(left->int_val / right->int_val));
    case FLOAT:
        return new VSValue((vs_float_t)(left->float_val / right->float_val));
    default:
        return VSValue::None();
    }
}

VSValue *VSValue::i_mod(VSValue *left, VSValue *right)
{
    if (left->type != right->type || !is_int(left->type))
        b_op_err("%", TYPE_STR[left->type], TYPE_STR[right->type]);

    switch (left->type)
    {
    case CHAR:
        return new VSValue((vs_char_t)(left->char_val % right->char_val));
    case INT:
        return new VSValue((vs_int_t)(left->int_val % right->int_val));
    default:
        return VSValue::None();
    }
}

VSValue *VSValue::i_lt(VSValue *left, VSValue *right)
{
    if (left->type != right->type || !is_num_str(left->type))
        b_op_err("<", TYPE_STR[left->type], TYPE_STR[right->type]);

    switch (left->type)
    {
    case CHAR:
        return left->char_val < right->char_val ? VSValue::True() : VSValue::False();
    case INT:
        return left->int_val < right->int_val ? VSValue::True() : VSValue::False();
    case FLOAT:
        return left->float_val < right->float_val ? VSValue::True() : VSValue::False();
    case STRING:
        return *left->str_val < *right->str_val ? VSValue::True() : VSValue::False();
    default:
        return VSValue::None();
    }
}

VSValue *VSValue::i_gt(VSValue *left, VSValue *right)
{
    if (left->type != right->type || !is_num_str(left->type))
        b_op_err(">", TYPE_STR[left->type], TYPE_STR[right->type]);

    switch (left->type)
    {
    case CHAR:
        return left->char_val > right->char_val ? VSValue::True() : VSValue::False();
    case INT:
        return left->int_val > right->int_val ? VSValue::True() : VSValue::False();
    case FLOAT:
        return left->float_val > right->float_val ? VSValue::True() : VSValue::False();
    case STRING:
        return *left->str_val > *right->str_val ? VSValue::True() : VSValue::False();
    default:
        return VSValue::None();
    }
}

VSValue *VSValue::i_le(VSValue *left, VSValue *right)
{
    if (left->type != right->type || !is_num_str(left->type))
        b_op_err("<=", TYPE_STR[left->type], TYPE_STR[right->type]);

    switch (left->type)
    {
    case CHAR:
        return left->char_val <= right->char_val ? VSValue::True() : VSValue::False();
    case INT:
        return left->int_val <= right->int_val ? VSValue::True() : VSValue::False();
    case FLOAT:
        return left->float_val <= right->float_val ? VSValue::True() : VSValue::False();
    case STRING:
        return *left->str_val <= *right->str_val ? VSValue::True() : VSValue::False();
    default:
        return VSValue::None();
    }
}

VSValue *VSValue::i_ge(VSValue *left, VSValue *right)
{
    if (left->type != right->type || !is_num_str(left->type))
        b_op_err(">=", TYPE_STR[left->type], TYPE_STR[right->type]);

    switch (left->type)
    {
    case CHAR:
        return left->char_val >= right->char_val ? VSValue::True() : VSValue::False();
    case INT:
        return left->int_val >= right->int_val ? VSValue::True() : VSValue::False();
    case FLOAT:
        return left->float_val >= right->float_val ? VSValue::True() : VSValue::False();
    case STRING:
        return *left->str_val >= *right->str_val ? VSValue::True() : VSValue::False();
    default:
        return VSValue::None();
    }
}

VSValue *VSValue::i_eq(VSValue *left, VSValue *right)
{
    if (left->type != right->type)
        return VSValue::False();
    
    switch (left->type)
    {
    case NONE:
        return VSValue::True();
    case BOOL:
        return left->bool_val == right->bool_val ? VSValue::True() : VSValue::False();
    case CHAR:
        return left->char_val == right->char_val ? VSValue::True() : VSValue::False();
    case INT:
        return left->int_val == right->int_val ? VSValue::True() : VSValue::False();
    case FLOAT:
        return left->float_val == right->float_val ? VSValue::True() : VSValue::False();
    case STRING:
        return *left->str_val == *right->str_val ? VSValue::True() : VSValue::False();
    default:
        return VSValue::None();
    }
}

VSValue *VSValue::i_neq(VSValue *left, VSValue *right)
{
    if (left->type != right->type)
        return VSValue::True();
    
    switch (left->type)
    {
    case NONE:
        return VSValue::False();
    case BOOL:
        return left->bool_val != right->bool_val ? VSValue::True() : VSValue::False();
    case CHAR:
        return left->char_val != right->char_val ? VSValue::True() : VSValue::False();
    case INT:
        return left->int_val != right->int_val ? VSValue::True() : VSValue::False();
    case FLOAT:
        return left->float_val != right->float_val ? VSValue::True() : VSValue::False();
    case STRING:
        return *left->str_val != *right->str_val ? VSValue::True() : VSValue::False();
    default:
        return VSValue::None();
    }
}

VSValue *VSValue::i_and(VSValue *left, VSValue *right)
{
    if (left->type != right->type || left->type != BOOL)
        b_op_err("&", TYPE_STR[left->type], TYPE_STR[right->type]);
    return left->bool_val && right->bool_val ? VSValue::True() : VSValue::False();
}

VSValue *VSValue::i_or(VSValue *left, VSValue *right)
{
    if (left->type != right->type || left->type != BOOL)
        b_op_err("|", TYPE_STR[left->type], TYPE_STR[right->type]);
    return left->bool_val || right->bool_val ? VSValue::True() : VSValue::False();
}

VSValue *VSValue::i_not(VSValue *val)
{
    if (val->type != BOOL)
        u_op_err("!", TYPE_STR[val->type]);
    return val->bool_val ? VSValue::False() : VSValue::True();
}

VSValue *VSValue::i_neg(VSValue *val)
{
    if (!is_num(val->type))
        u_op_err("-", TYPE_STR[val->type]);

    switch (val->type)
    {
    case CHAR:
        return new VSValue((vs_char_t)(-val->char_val));
    case INT:
        return new VSValue((vs_int_t)(-val->int_val));
    case FLOAT:
        return new VSValue((vs_float_t)(-val->float_val));
    default:
        return VSValue::None();
    }
}

const char *VSValue::to_bytes()
{
    return this->to_string().c_str();
}

const std::string VSValue::to_string()
{
    switch (this->type)
    {
    case NONE:
        return "__vs_none__";
    case BOOL:
        return "__vs_bool_" + std::to_string(this->bool_val) + "__";
    case CHAR:
        return "__vs_char_" + std::to_string(this->char_val) + "__";
    case INT:
        return "__vs_int_" + std::to_string(this->int_val) + "__";
    case FLOAT:
        return "__vs_float_" + std::to_string(this->float_val) + "__";
    case STRING:
        return "__vs_string_" + *this->str_val + "__";
    default:
        return "__vs_undefined__";
    }
}