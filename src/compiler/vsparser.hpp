#ifndef VSPARSER_H
#define VSPARSER_H

#include "vsc.hpp"
#include "types/ASTNode.hpp"
#include "types/Token.hpp"
#include "types/SymTable.hpp"

void init_parser();
ASTNode *parse(std::vector<Token *> *tokens);

#endif