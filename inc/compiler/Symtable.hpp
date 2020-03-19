#ifndef VS_SYMTABLE_H
#define VS_SYMTABLE_H

#include "objects/VSObject.hpp"

#include <string>
#include <unordered_map>
#include <vector>

typedef enum {
    SYM_VAR,
    SYM_VAL,
    SYM_ARG,
    SYM_CLOSURE,
    SYM_ATTR_NAME,
    SYM_BUILTIN
} SYM_TYPE;

class SymtableEntry : public VSObject {
public:
    SYM_TYPE sym_type;
    std::string symbol;
    int index, parent_index;

    SymtableEntry(SYM_TYPE sym_type, std::string symbol, int index, int parent_index);
    ~SymtableEntry() = default;
};

class Symtable : public VSObject {
private:
    Symtable *parent;
    std::unordered_map<std::string, SymtableEntry *> table;

public:
    Symtable(Symtable *parent);
    ~Symtable();

    bool has_parent();
    Symtable *get_parent();
    void put(std::string name, SymtableEntry *entry);
    SymtableEntry *get(std::string name);
    SymtableEntry *get_recur(std::string name);
    bool contains(std::string name);
    bool contains_recur(std::string name);
};

#endif