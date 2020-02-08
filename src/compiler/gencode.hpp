#ifndef GENCODE_H
#define GENCODE_H

#include "vsc.hpp"
#include "types/Value.hpp"
#include "types/ASTNode.hpp"
#include "vsparser.hpp"

void gencode(std::vector<ASTNode *> *astree, std::vector<char *> code);

#endif