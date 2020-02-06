#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "vsparser.h"
#include <string>
#include <unordered_map>

class SymTable
{
private:
    SymTable *parent;
    std::vector<SymTable *> children;
    std::unordered_map<std::string, ASTNode *> table;

public:
    SymTable(SymTable *parent);
    ~SymTable();

    void put(ASTNode *id);
    ASTNode *get(char *name);
    bool contains(char *name);
    void add_child(SymTable *child);
    static void print(SymTable *table, int indent);
};
#endif