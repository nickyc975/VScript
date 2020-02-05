#include <stdio.h>

#include "vslexer.h"
// #include "vsparser.h"
#include "../utils/File.h"

char CLASS_STR[][15] = {
    "ADD", "ADD_ASSIGN", "AND", "AND_ASSIGN", "ASSIGN", "BREAK", "COMMA", "CONSTANT", "CONTINUE", 
    "DIV", "DIV_ASSIGN", "ELIF", "ELSE", "EQ", "FOR", "FUNC", "GE", "GT", "IDENTIFIER", "IF", "INPUT", "LE", "LT", 
    "L_BRACK", "L_CURLY", "L_PAREN", "MOD", "MOD_ASSIGN", "MUL", "MUL_ASSIGN", "NEQ", "NOT", "OR", 
    "OR_ASSIGN", "PRINT", "RETURN", "R_BRACK", "R_CURLY", "R_PAREN", "SEMICOLON", "SUB", "SUB_ASSIGN", "VAL", 
    "VAR", "WHILE", "#"
};

char TYPE_STR[][10] = {"NONE", "BOOL", "CHAR", "INT", "FLOAT", "STRING", "LIST", "FUNC"};

void print_tk_lst(std::vector<Token *>, char *filename);

int main(int argc, char **argv)
{
    Token *token;
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
    // init_parser();
    // parse(tokenlist, parser);
    // free_tokenlist(tokenlist);
    // free_filereader(reader);
    return 0;
}

void print_tk_lst(std::vector<Token *> token_list, char *filename)
{
    FILE *tokens_txt = fopen(filename, "w");
    for (Token *token : token_list)
    {
        fprintf(tokens_txt, "%s\t", CLASS_STR[token->kind]);
        switch (token->kind)
        {
            case IDENTIFIER:
                fprintf(tokens_txt, "%s\t0x%x\n", token->identifier, token->identifier);
                break;
            case CONSTANT:
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
                        fprintf(tokens_txt, "%d\n", token->value->int_val);
                        break;
                    case FLOAT:
                        fprintf(tokens_txt, "%f\n", token->value->float_val);
                        break;
                    case STRING:
                        fprintf(tokens_txt, "%s\n", token->value->str_val);
                        break;
                    default:
                        break;
                }
                break;
            case SEMICOLON:
            case COMMA:
            case L_PAREN:
            case R_PAREN:
            case L_BRACK:
            case R_BRACK:
            case L_CURLY:
            case R_CURLY:
            default:
                fprintf(tokens_txt, "\n");
                break;
        }
    }
    fclose(tokens_txt);
}