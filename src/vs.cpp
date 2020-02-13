#include <stdio.h>

#include "vs.hpp"
#include "compiler.hpp"
#include "runtime.hpp"
#include "printers.hpp"
#include "File.hpp"

int main(int argc, char **argv)
{
    File *file;
    std::vector<Token *> token_list;
    int show_lex = 0, show_parse = 0, show_gen = 0;
    argc--;
    argv++;
    if (argc > 0 && **argv == '-' && (*argv)[1] == 'l')
    {
        show_lex = 1;
        argc--;
        argv++;
    }
    if (argc > 0 && **argv == '-' && (*argv)[1] == 'p')
    {
        show_parse = 1;
        argc--;
        argv++;
    }
    if (argc > 0 && **argv == '-' && (*argv)[1] == 's')
    {
        show_gen = 1;
        argc--;
        argv++;
    }
    if (argc < 1)
    {
        printf("usage: vsc [-l] [-p] [-s] file ...\n");
        return -1;
    }
    file = new File(fopen(*argv, "r"));
    tokenize(file, token_list);
    if (show_lex)
    {
        fprint_tokens(fopen("tokens.txt", "w"), token_list);
    }
    ASTNode *astree = parse(&token_list);
    if (show_parse)
    {
        fprint_astree(fopen("astree.txt", "w"), astree);
    }
    VSCodeObject *program = gencode(astree);
    if (show_gen)
    {
        fprint_code(fopen("instructions.vss", "w"), program);
    }
    execute(program);
    return 0;
}
