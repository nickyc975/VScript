#include "objects/VSTypeObject.hpp"
#include "compiler/Symtable.hpp"

SymtableEntry::SymtableEntry(SYM_TYPE sym_type, VSObject *symbol, int index, int cell_index) : 
    sym_type(sym_type), symbol(symbol), index(index), cell_index(cell_index) {
        this->is_cell = NULL;
        INCREF(symbol);
}

SymtableEntry::~SymtableEntry() {
    DECREF_EX(this->symbol);
}

Symtable::Symtable(Symtable *parent) : parent(parent) {
    this->table = NEW_REF(VSMapObject *, new VSMapObject());
    INCREF(parent);
}

Symtable::~Symtable() {
    DECREF(this->parent);
    DECREF(this->table);
}

bool Symtable::has_parent() {
    return this->parent != NULL;
}

Symtable *Symtable::get_parent() {
    return this->parent;
}

void Symtable::put(VSObject *name, SymtableEntry *value) {
    MAP_SET(this->table, name, value);
}

SymtableEntry *Symtable::get(VSObject *name) {
    if (MAP_HAS(this->table, name)) {
        return (SymtableEntry *)MAP_GET(this->table, name);
    }
    return NULL;
}


SymtableEntry *Symtable::get_recur(VSObject *name) {
    SymtableEntry *node = this->get(name);
    if (node == NULL && this->has_parent()) {
        return this->parent->get_recur(name);
    }
    return node;
}


bool Symtable::contains(VSObject *name) {
    return MAP_HAS(this->table, name);
}


bool Symtable::contains_recur(VSObject *name) {
    bool result = this->contains(name);
    if (result == false && this->has_parent()) {
        return this->parent->contains_recur(name);
    }
    return result;
}