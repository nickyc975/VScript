#ifndef PRINTERS_H
#define PRINTERS_H

#include "compiler.hpp"

void init_printer();
void fprint_tokens(FILE *file, std::vector<Token *> tokens);
void fprint_astree(FILE *file, ASTNode *astree);
void fprint_code(FILE *file, VSCodeObject *code);

#endif

