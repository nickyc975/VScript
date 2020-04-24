#include "vs.hpp"

#include <stdio.h>

#include <stack>

#include "compiler/VSCompiler.hpp"
#include "objects/VSFrameObject.hpp"
#include "objects/VSTupleObject.hpp"
#include "printers.hpp"

int main(int argc, char **argv) {
    int show_lex = 0, show_parse = 0, show_gen = 0;
    argc--;
    argv++;
    if (argc > 0 && **argv == '-' && (*argv)[1] == 'l') {
        show_lex = 1;
        argc--;
        argv++;
    }
    if (argc > 0 && **argv == '-' && (*argv)[1] == 'p') {
        show_parse = 1;
        argc--;
        argv++;
    }
    if (argc > 0 && **argv == '-' && (*argv)[1] == 's') {
        show_gen = 1;
        argc--;
        argv++;
    }
    if (argc < 1) {
        printf("usage: vsc [-l] [-p] [-s] file ...\n");
        return -1;
    }

    if (show_lex) {
        FILE *f = fopen("tokens.txt", "w");
        // fprint_tokens(f, token_list);
        fclose(f);
    }

    if (show_parse) {
        FILE *f = fopen("astree.txt", "w");
        // fprint_astree(f, astree);
        fclose(f);
    }
    init_printer();
    VSCompiler *compiler = new VSCompiler(new name_addr_map());
    VSCodeObject *program = compiler->compile(*argv);
    if (show_gen) {
        FILE *f = fopen("instructions.txt", "w");
        fprint_code(f, program);
        fclose(f);
    }
    VSFrameObject *frame = new VSFrameObject(program, NULL, new VSTupleObject(program->ncellvars), NULL, NULL, NULL);
    auto stack = std::stack<VSObject *>();
    frame->eval(stack);
    printf("%d\n", stack.size());
    return 0;
}
