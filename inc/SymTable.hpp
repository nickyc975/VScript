#ifndef VS_SYMTABLE_H
#define VS_SYMTABLE_H

#include <string>
#include <vector>
#include <unordered_map>

template<typename V>
class SymTable
{
private:
    SymTable *parent;
    std::vector<SymTable *> children;
    std::unordered_map<std::string, V> table;

public:
    V top;

    SymTable(SymTable *parent);
    ~SymTable();

    bool has_parent();
    SymTable *get_parent();
    void put(char *name, V value);
    V get(char *name);
    V get_recur(char *name);
    bool contains(char *name);
    bool contains_recur(char *name);
    void add_child(SymTable *child);
};

template<typename V>
SymTable<V>::SymTable(SymTable *parent): parent(parent)
{

}

template<typename V>
SymTable<V>::~SymTable()
{

}

template<typename V>
bool SymTable<V>::has_parent()
{
    return this->parent != NULL;
}

template<typename V>
SymTable<V> *SymTable<V>::get_parent()
{
    return this->parent;
}

template<typename V>
void SymTable<V>::put(char *name, V value)
{
    this->table[std::string(name)] = value;
}

template<typename V>
V SymTable<V>::get(char *name)
{
    std::string sname = std::string(name);
    if (this->table.find(sname) != this->table.end())
    {
        return this->table[sname];
    }
    return NULL;
}

template<typename V>
V SymTable<V>::get_recur(char *name)
{
    V node = this->get(name);
    if (node == NULL && this->has_parent())
    {
        return this->parent->get_recur(name);
    }
    return node;
}

template<typename V>
bool SymTable<V>::contains(char *name)
{
    return this->table.find(std::string(name)) != this->table.end();
}

template<typename V>
bool SymTable<V>::contains_recur(char *name)
{
    bool result = this->contains(name);
    if (result == false && this->has_parent())
    {
        return this->parent->contains_recur(name);
    }
    return result;
}

template<typename V>
void SymTable<V>::add_child(SymTable *child)
{
    if (child != NULL)
    {
        this->children.push_back(child);
    }
}

#endif