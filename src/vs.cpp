#include <stdio.h>

#include "vs.hpp"
#include "compiler.hpp"
#include "runtime.hpp"
#include "printers.hpp"
#include "File.hpp"

void print_tk_lst(std::vector<Token *>, char *filename);

int main(int argc, char **argv)
{
    File *file;
    std::vector<Token *> token_list;
    int lexer = 0, parser = 0;
    argc--;
    argv++;

    if (argc > 0 && **argv == '-' && (*argv)[1] == 'l')
    {
        lexer = 1;
        argc--;
        argv++;
    }
    if (argc > 0 && **argv == '-' && (*argv)[1] == 'p')
    {
        parser = 1;
        argc--;
        argv++;
    }
    if (argc < 1)
    {
        printf("usage: vsc [-l] [-p] file ...\n");
        return -1;
    }

    file = new File(fopen(*argv, "r"));
    tokenize(file, token_list);
    if (lexer)
    {
        print_tk_lst(token_list, (char *)"test/tokens.txt");
    }
    ASTNode *astree = parse(&token_list);
    if (parser)
    {
        print_ast(astree);
    }
    VSCodeObject *program = gencode(astree);
    fprint_code(fopen((char *)"test/hello.vss", "w"), program);
    execute(program);
    return 0;
}

void print_tk_lst(std::vector<Token *> token_list, char *filename)
{
    FILE *tokens_txt = fopen(filename, "w");
    for (Token *token : token_list)
    {
        fprintf(tokens_txt, "%s\t%lld\t%lld\t", TOKEN_STR[token->type], token->ln, token->col);
        switch (token->type)
        {
            case TK_IDENTIFIER:
                fprintf(tokens_txt, "%s\n", token->identifier->c_str());
                break;
            case TK_CONSTANT:
                fprintf(tokens_txt, "%s\t", TYPE_STR[token->value->type]);
                switch (token->value->type)
                {
                    case NONE:
                        fprintf(tokens_txt, "none\n");
                        break;
                    case BOOL:
                        fprintf(tokens_txt, "%s\n",token->value->bool_val ? "true" : "false");
                        break;
                    case CHAR:
                        fprintf(tokens_txt, "%c\n", token->value->char_val);
                        break;
                    case INT:
                        fprintf(tokens_txt, "%lld\n", token->value->int_val);
                        break;
                    case FLOAT:
                        fprintf(tokens_txt, "%lf\n", token->value->float_val);
                        break;
                    case STRING:
                        fprintf(tokens_txt, "%s\n", token->value->str_val->c_str());
                        break;
                    default:
                        break;
                }
                break;
            case TK_SEMICOLON:
            case TK_COMMA:
            case TK_L_PAREN:
            case TK_R_PAREN:
            case TK_L_BRACK:
            case TK_R_BRACK:
            case TK_L_CURLY:
            case TK_R_CURLY:
            default:
                fprintf(tokens_txt, "\n");
                break;
        }
    }
    fclose(tokens_txt);
}