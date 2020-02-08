#ifndef VSLEXER_H

#define VSLEXER_H

#include "vsc.hpp"
#include "types/Token.hpp"
#include "types/File.hpp"

void tokenize(File *file, std::vector<Token *> &tokens);

#endif