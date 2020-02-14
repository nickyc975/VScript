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
    delete file;
    if (show_lex)
    {
        FILE *f = fopen("tokens.txt", "w");
        fprint_tokens(f, token_list);
        fclose(f);
    }
    ASTNode *astree = parse(&token_list);
    if (show_parse)
    {
        FILE *f = fopen("astree.txt", "w");
        fprint_astree(f, astree);
        fclose(f);
    }
    VSCodeObject *program = gencode(astree);
    if (show_gen)
    {
        FILE *f = fopen("instructions.txt", "w");
        fprint_code(f, program);
        fclose(f);
    }
    execute(program);
    return 0;
}
