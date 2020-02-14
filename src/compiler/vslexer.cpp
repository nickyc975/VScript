#include <cstring>

#include "compiler.hpp"

#define BUFFER_SIZE 128

#define STRING_BASE_LEN 16

#define ERROR_BUFFER_SIZE 128

char buffer[BUFFER_SIZE];

char error_buffer[ERROR_BUFFER_SIZE];

char escape(char c)
{
    switch (c)
    {
        case 'n':
            return '\n';
        case 't':
            return '\t';
        case 'r':
            return '\r';
        case '0':
            return '\0';
        default:
            return c;
    }
}

int recognize_id(File *file, Token *token)
{
    int len = file->getword(buffer, BUFFER_SIZE);
    token->identifier = new std::string(buffer);
    token->type = TK_IDENTIFIER;
    return 0;
}

int recognize_none(File *file, Token *token)
{
    int len = file->getword(buffer, BUFFER_SIZE);
    if (len == 4 && !strcmp("none", buffer))
    {
        token->type = TK_CONSTANT;
        token->value = VSValue::None();
        return 0;
    }
    file->seek(-len);
    return len;
}

int recognize_bool(File *file, Token *token)
{
    int offset = 0;
    int len = file->getword(buffer, BUFFER_SIZE);
    if (!strcmp("true", buffer))
    {
        token->type = TK_CONSTANT;
        token->value = VSValue::True();
    }
    else if (!strcmp("false", buffer))
    {
        token->type = TK_CONSTANT;
        token->value = VSValue::False();
    }
    else
    {
        // Undo reading operation.
        file->seek(-len);
        offset = len;
    }
    return offset;
}

int recognize_num(File *file, Token *token)
{
    token->type = TK_CONSTANT;
    file->getnum(buffer, BUFFER_SIZE);
    if (strchr(buffer, '.') != NULL)
    {
        token->value = new VSValue((long double)atof(buffer));
    }
    else
    {
        token->value = new VSValue(atoll(buffer));
    }
    return 0;
}

int recognize_char(File *file, Token *token)
{
    int len = file->getstr(buffer, 5);

    if (!is_quote(buffer[0])
            || (buffer[1] == '\\' && !is_quote(buffer[3]))
            || (buffer[1] != '\\' && !is_quote(buffer[2]))) {
        file->seek(-len);
        return len;
    }
    
    token->type = TK_CONSTANT;
    if (buffer[1] == '\\') {
        token->value = new VSValue(escape(buffer[2]));
    }
    else
    {
        token->value = new VSValue(buffer[1]);
        file->seek(-1);
    }
    return 0;
}

int recognize_str(File *file, Token *token)
{
    char c;
    int len = 0, offset = 0, cur_size = STRING_BASE_LEN - 1;
    char *str_buffer = (char *)malloc(STRING_BASE_LEN);
    if (file->getchar() != '\"') {
        file->seek(-1);
        return 1;
    }

    c = file->getchar();
    offset++;
    while(str_buffer && c != EOF && c != '\n' && c != '\"') {
        if (c == '\\')
        {
            c = escape(file->getchar());
            offset++;
        }
        str_buffer[len] = c;
        len++;
        if (len >= cur_size)
        {
            cur_size = cur_size + len / 4 + 1;
            str_buffer = (char *)realloc(str_buffer, cur_size);
            cur_size -= 1;
        }
        c = file->getchar();
        offset++;
    }

    offset += 1;
    if (c == '\"')
    {
        str_buffer[len] = '\0';
        token->type = TK_CONSTANT;
        token->value = new VSValue(std::string(str_buffer));
        free(str_buffer);
        return 0;
    }

    file->seek(-offset);
    return offset - 1;
}

int recognize_keyword(File *file, Token *token)
{
    int offset = 0;
    int len = file->getword(buffer, BUFFER_SIZE);
    switch (buffer[0])
    {
        case 'c':
            if (!strcmp(buffer, "continue"))
            {
                token->type = TK_CONTINUE;
            }
            else if (!strcmp(buffer, "char"))
            {
                token->type = TK_CHAR;
            }
            else
            {
                offset = len;
            }
            break;
        case 'i':
            if (!strcmp(buffer, "if"))
            {
                token->type = TK_IF;
            }
            else if (!strcmp(buffer, "input"))
            {
                token->type = TK_INPUT;
            }
            else if (!strcmp(buffer, "int"))
            {
                token->type = TK_INT;
            }
            else
            {
                offset = len;
            }
            break;
        case 'e':
            if (!strcmp(buffer, "else"))
            {
                token->type = TK_ELSE;
            }
            else if (!strcmp(buffer, "elif"))
            {
                token->type = TK_ELIF;
            }
            else if (!strcmp(buffer, "eval"))
            {
                token->type = TK_EVAL;
            }
            else
            {
                offset = len;
            }
            break;
        case 'w':
            if (!strcmp(buffer, "while"))
            {
                token->type = TK_WHILE;
            }
            else
            {
                offset = len;
            }
            break;
        case 'f':
            if (!strcmp(buffer, "for"))
            {
                token->type = TK_FOR;
            }
            else if (!strcmp(buffer, "func"))
            {
                token->type = TK_FUNC;
            }
            else if (!strcmp(buffer, "float"))
            {
                token->type = TK_FLOAT;
            }
            else
            {
                offset = len;
            }
            break;
        case 'r':
            if (!strcmp(buffer, "return"))
            {
                token->type = TK_RETURN;
            }
            else
            {
                offset = len;
            }
            break;
        case 'b':
            if (!strcmp(buffer, "break"))
            {
                token->type = TK_BREAK;
            }
            else
            {
                offset = len;
            }
            break;
        case 'v':
            if (!strcmp(buffer, "val"))
            {
                token->type = TK_VAL;
            }
            else if (!strcmp(buffer, "var"))
            {
                token->type = TK_VAR;
            }
            else
            {
                offset = len;
            }
            break;
        case 'p':
            if (!strcmp(buffer, "print"))
            {
                token->type = TK_PRINT;
            }
            else
            {
                offset = len;
            }
            break;
        case 's':
            if (!strcmp(buffer, "str"))
            {
                token->type = TK_STR;
            }
            else
            {
                offset = len;
            }
        default:
            offset = len;
            break;
    }
    file->seek(-offset);
    return offset;
}

void tokenize(File *file, std::vector<Token *> &tokens)
{
    int offset = 0;
    char tk_char = file->nextchar();

    while (tk_char != EOF)
    {
        Token *token = new Token(file->cur_ln(), file->cur_col());
        if (is_number(tk_char))
        {
            offset = recognize_num(file, token);
            tokens.push_back(token);
            goto nextchar;
        }

        if (tk_char == 't' || tk_char == 'f')
        {
            offset = recognize_bool(file, token);
            if (!offset)
            {
                tokens.push_back(token);
                goto nextchar;
            }
        }

        if (tk_char == 'n')
        {
            offset = recognize_none(file, token);
            if (!offset)
            {
                tokens.push_back(token);
                goto nextchar;
            }
        }

        if (is_word(tk_char))
        {
            offset = recognize_keyword(file, token);
            if (!offset)
            {
                tokens.push_back(token);
            }
            else
            {
                offset = recognize_id(file, token);
                tokens.push_back(token);
            }
            goto nextchar;
        }

        if (tk_char == '\'')
        {
            offset = recognize_char(file, token);
            if (!offset) {
                tokens.push_back(token);
            } else {
                offset = file->getstr(error_buffer, offset);
                err("line: %d, col: %d, \"%s\"\n", file->cur_ln(), file->cur_col(), error_buffer);
            }
            goto nextchar;
        }

        if (tk_char == '\"')
        {
            offset = recognize_str(file, token);
            if (!offset) {
                tokens.push_back(token);
            } else {
                offset = file->getstr(error_buffer, offset);
                err("line: %d, col: %d, \"%s\"\n", file->cur_ln(), file->cur_col(), error_buffer);
            }
            goto nextchar;
        }
        
        tk_char = file->getchar();
        switch (tk_char)
        {
            case '=':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->type = TK_EQ;
                } else {
                    token->type = TK_ASSIGN;
                }
                tokens.push_back(token);
                break;
            case '+':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->type = TK_ADD_ASSIGN;
                } else {
                    token->type = TK_ADD;
                }
                tokens.push_back(token);
                break;
            case '-':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->type = TK_SUB_ASSIGN;
                } else {
                    token->type = TK_SUB;
                }
                tokens.push_back(token);
                break;
            case '*':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->type = TK_MUL_ASSIGN;
                } else {
                    token->type = TK_MUL;
                }
                tokens.push_back(token);
                break;
            case '/':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->type = TK_DIV_ASSIGN;
                } else if (file->nextchar() == '/') {
                    while (file->nextchar() != EOF && file->getchar() != '\n');
                    break;
                } else {
                    token->type = TK_DIV;
                }
                tokens.push_back(token);
                break;
            case '%':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->type = TK_MOD_ASSIGN;
                } else {
                    token->type = TK_MOD;
                }
                tokens.push_back(token);
                break;
            case '&':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->type = TK_AND_ASSIGN;
                } else {
                    token->type = TK_AND;
                }
                tokens.push_back(token);
                break;
            case '|':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->type = TK_OR_ASSIGN;
                } else {
                    token->type = TK_OR;
                }
                tokens.push_back(token);
                break;
            case '!':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->type = TK_NEQ;
                } else {
                    token->type = TK_NOT;
                }
                tokens.push_back(token);
                break;
            case '<':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->type = TK_LE;
                } else {
                    token->type = TK_LT;
                }
                tokens.push_back(token);
                break;
            case '>':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->type = TK_GE;
                } else {
                    token->type = TK_GT;
                }
                tokens.push_back(token);
                break;
            case ',':
                token->type = TK_COMMA;
                tokens.push_back(token);
                break;
            case ';':
                token->type = TK_SEMICOLON;
                tokens.push_back(token);
                break;
            case '(':
                token->type = TK_L_PAREN;
                tokens.push_back(token);
                break;
            case ')':
                token->type = TK_R_PAREN;
                tokens.push_back(token);
                break;
            case '[':
                token->type = TK_L_BRACK;
                tokens.push_back(token);
                break;
            case ']':
                token->type = TK_R_BRACK;
                tokens.push_back(token);
                break;
            case '{':
                token->type = TK_L_CURLY;
                tokens.push_back(token);
                break;
            case '}':
                token->type = TK_R_CURLY;
                tokens.push_back(token);
                break;
            case ' ':
            case '\n':
            case '\t':
            case '\r':
                break;
            default:
                err("line: %d, col: %d, illegal token: \"%c\"\n", file->cur_ln(), file->cur_col(), tk_char);
                break;
        }

    nextchar:
        tk_char = file->nextchar();
    }
}
