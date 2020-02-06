#include "SymTable.h"

#define print_indent(indent) for(int i = 0; i < indent; i++) printf("    ");

SymTable::SymTable(SymTable *parent): parent(parent)
{

}

SymTable::~SymTable()
{

}

void SymTable::put(ASTNode *id)
{
    if (id->node_type == AST_IDENT)
    {
        this->table[std::string(id->name)] = id;
    }
}

ASTNode *SymTable::get(char *name)
{
    std::string sname = std::string(name);
    if (this->table.find(sname) != this->table.end())
    {
        return this->table[sname];
    }
    return NULL;
}

bool SymTable::contains(char *name)
{
    return this->table.find(std::string(name)) != this->table.end();
}

void SymTable::add_child(SymTable *child)
{
    if (child != NULL)
    {
        this->children.push_back(child);
    }
}

void SymTable::print(SymTable *table, int indent)
{
    print_indent(indent);
    printf("table: \n");
    indent++;
    for (std::pair<std::string, ASTNode *> entry : table->table)
    {
        print_indent(indent);
        printf("%s\t%s\n", entry.first.c_str(), TYPE_STR[entry.second->type]);
    }
    
    for (SymTable *child : table->children)
    {
        print(child, indent);
    }
}