#include "objects/VSTypeObject.hpp"
#include "compiler/Symtable.hpp"

SymtableEntry::SymtableEntry(SYM_TYPE sym_type, std::string symbol, int index, int parent_index) : 
    sym_type(sym_type), symbol(symbol), index(index), parent_index(parent_index) {
}

Symtable::Symtable(Symtable *parent) : parent(parent) {
    INCREF(parent);
}

Symtable::~Symtable() {
    DECREF(this->parent);
    for (auto pair : this->table) {
        DECREF(pair.second);
    }
}


bool Symtable::has_parent() {
    return this->parent != NULL;
}


Symtable *Symtable::get_parent() {
    return this->parent;
}


void Symtable::put(std::string name, SymtableEntry *value) {
    this->table[name] = value;
    INCREF(value);
}


SymtableEntry *Symtable::get(std::string name) {
    if (this->table.find(name) != this->table.end()) {
        return this->table[name];
    }
    return NULL;
}


SymtableEntry *Symtable::get_recur(std::string name) {
    SymtableEntry *node = this->get(name);
    if (node == NULL && this->has_parent()) {
        return this->parent->get_recur(name);
    }
    return node;
}


bool Symtable::contains(std::string name) {
    return this->table.find(name) != this->table.end();
}


bool Symtable::contains_recur(std::string name) {
    bool result = this->contains(name);
    if (result == false && this->has_parent()) {
        return this->parent->contains_recur(name);
    }
    return result;
}