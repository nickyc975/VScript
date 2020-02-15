#ifndef VS_GLOBAL_H
#define VS_GLOBAL_H

#include "vs.hpp"

VSObjectList *global_objects();
std::vector<vs_native_func> *native_func_addrs();
std::unordered_map<std::string, vs_addr_t> *global_symbols();

#endif