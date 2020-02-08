#include "SymTable.hpp"

#define print_indent(indent) for(int i = 0; i < indent; i++) printf("    ");

SymTable::SymTable(SymTable *parent): parent(parent)
{

}

SymTable::~SymTable()
{

}

bool SymTable::has_parent()
{
    return this->parent != NULL;
}

SymTable *SymTable::get_parent()
{
    return this->parent;
}

void SymTable::put(ASTNode *ident)
{
    this->table[std::string(ident->name)] = ident;
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

ASTNode *SymTable::get_recur(char *name)
{
    ASTNode *node = this->get(name);
    if (node == NULL && this->has_parent())
    {
        return this->parent->get_recur(name);
    }
    return node;
}

bool SymTable::contains(char *name)
{
    return this->table.find(std::string(name)) != this->table.end();
}

bool SymTable::contains_recur(char *name)
{
    bool result = this->contains(name);
    if (result == false && this->has_parent())
    {
        return this->parent->contains_recur(name);
    }
    return result;
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