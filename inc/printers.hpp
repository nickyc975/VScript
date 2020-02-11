#ifndef PRINTERS_H
#define PRINTERS_H

#include "compiler.hpp"

void init_printer();
void print_ast(ASTNode *root);
void fprint_code(FILE *file, VSCodeObject *code);

#endif

