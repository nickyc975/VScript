#ifndef VS_H
#define VS_H

#include <vector>
#include <string>
#include <unordered_map>

#define is_num_str(type) (type != NONE && type != BOOL)
#define is_num(type) (is_num_str(type) && type != STRING)
#define is_int(type) (is_num(type) && type != FLOAT)

#define CONST_NONE_ADDR 0
#define CONST_TRUE_ADDR 1
#define CONST_FALSE_ADDR 2

typedef bool vs_bool_t;
typedef char vs_char_t;
typedef long long vs_int_t;
typedef long double vs_float_t;
typedef unsigned int vs_id_t;
typedef unsigned int vs_size_t;
typedef unsigned int vs_addr_t;

#endif