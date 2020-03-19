#ifndef VS_H
#define VS_H

#include <string>
#include <unordered_map>

typedef uint8_t cbool_t;
typedef char cchar_t;
typedef int64_t cint_t;
typedef long double cfloat_t;
typedef unsigned long long vs_size_t;
typedef unsigned long long vs_addr_t;

typedef std::unordered_map<std::string, vs_addr_t> name_addr_map;

#endif