#ifndef VS_SYMTABLE_H
#define VS_SYMTABLE_H

#include "objects/VSObject.hpp"
#include "objects/VSMapObject.hpp"

typedef enum {
    SYM_VAR,
    SYM_VAL,
    SYM_ARG,
    SYM_FREE,
    SYM_BUILTIN,
    SYM_UNDEFINED
} SYM_TYPE;

#define IS_LOCAL(tp) (tp == SYM_VAR || tp == SYM_VAL || tp == SYM_ARG)

class SymtableEntry : public VSObject {
public:
    VSObject *symbol;
    SYM_TYPE sym_type;
    bool is_cell;
    int index, cell_index;

    SymtableEntry(SYM_TYPE sym_type, VSObject *symbol, int index, int cell_index);
    ~SymtableEntry();
};

class Symtable : public VSObject {
private:
    Symtable *parent;
    VSMapObject *table;

public:
    Symtable(Symtable *parent);
    ~Symtable();

    bool has_parent();
    Symtable *get_parent();
    void put(VSObject *name, SymtableEntry *entry);
    SymtableEntry *get(VSObject *name);
    SymtableEntry *get_recur(VSObject *name);
    bool contains(VSObject *name);
    bool contains_recur(VSObject *name);
};

#endif