#ifndef VS_SYMTABLE_H
#define VS_SYMTABLE_H

#include "../vsc.hpp"
#include "ASTNode.hpp"

class SymTable
{
private:
    SymTable *parent;
    std::vector<SymTable *> children;
    std::unordered_map<std::string, ASTNode *> table;

public:
    ASTNode *top;

    SymTable(SymTable *parent);
    ~SymTable();

    bool has_parent();
    SymTable *get_parent();
    void put(ASTNode *ident);
    ASTNode *get(char *name);
    ASTNode *get_recur(char *name);
    bool contains(char *name);
    bool contains_recur(char *name);
    void add_child(SymTable *child);
    static void print(SymTable *table, int indent);
};

#endif