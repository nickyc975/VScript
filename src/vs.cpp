#include "vs.hpp"

#include <stdio.h>

#include <stack>

#include "compiler/VSCompiler.hpp"
#include "objects/VSFrameObject.hpp"
#include "objects/VSTupleObject.hpp"
#include "printers.hpp"
#include "runtime/builtins.hpp"
#include "runtime/VSInterpreter.hpp"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s [-s] <file>\n", *argv);
        return -1;
    }

    argc--; argv++;
    int show_gen = 0;
    if (**argv == '-' && (*argv)[1] == 's') {
        show_gen = 1;
        argc--;
        argv++;
    }

    init_printer();
    VSCompiler *compiler = new VSCompiler(builtin_addrs);
    VSCodeObject *program = compiler->compile(*argv);
    if (show_gen) {
        FILE *f = fopen("instructions.txt", "w");
        fprint_code(f, program);
        fclose(f);
    }
    VSFrameObject *frame = new VSFrameObject(program, NULL, new VSTupleObject(program->ncellvars), NULL, NULL);
    auto stack = std::stack<VSObject *>();
    INTERPRETER.eval(stack, frame);
    return 0;
}
