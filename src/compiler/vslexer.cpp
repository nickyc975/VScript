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
    token->identifier = (char *)malloc(len + 1);
    strcpy(token->identifier, buffer);
    token->identifier[len] = 0;
    token->kind = IDENTIFIER;
    return 0;
}

int recognize_none(File *file, Token *token)
{
    int len = file->getword(buffer, BUFFER_SIZE);
    if (len == 4 && !strcmp("none", buffer))
    {
        token->kind = CONSTANT;
        token->value = new Value();
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
        token->kind = CONSTANT;
        token->value = new Value(true);
    }
    else if (!strcmp("false", buffer))
    {
        token->kind = CONSTANT;
        token->value = new Value(false);
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
    token->kind = CONSTANT;
    file->getnum(buffer, BUFFER_SIZE);
    if (strchr(buffer, '.') != NULL)
    {
        token->value = new Value(atof(buffer));
    }
    else
    {
        token->value = new Value(atoll(buffer));
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
    
    token->kind = CONSTANT;
    if (buffer[1] == '\\') {
        token->value = new Value(escape(buffer[2]));
    }
    else
    {
        token->value = new Value(buffer[1]);
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
        token->kind = CONSTANT;
        token->value = new Value(str_buffer);
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
                token->kind = CONTINUE;
            }
            else
            {
                offset = len;
            }
            break;
        case 'i':
            if (!strcmp(buffer, "if"))
            {
                token->kind = IF;
            }
            else if (!strcmp(buffer, "input"))
            {
                token->kind = INPUT;
            }
            else
            {
                offset = len;
            }
            break;
        case 'e':
            if (!strcmp(buffer, "else"))
            {
                token->kind = ELSE;
            }
            else if (!strcmp(buffer, "elif"))
            {
                token->kind = ELIF;
            }
            else
            {
                offset = len;
            }
            break;
        case 'w':
            if (!strcmp(buffer, "while"))
            {
                token->kind = WHILE;
            }
            else
            {
                offset = len;
            }
            break;
        case 'f':
            if (!strcmp(buffer, "for"))
            {
                token->kind = FOR;
            }
            else if (!strcmp(buffer, "func"))
            {
                token->kind = FUNC;
            }
            else
            {
                offset = len;
            }
            break;
        case 'r':
            if (!strcmp(buffer, "return"))
            {
                token->kind = RETURN;
            }
            else
            {
                offset = len;
            }
            break;
        case 'b':
            if (!strcmp(buffer, "break"))
            {
                token->kind = BREAK;
            }
            else
            {
                offset = len;
            }
            break;
        case 'v':
            if (!strcmp(buffer, "val"))
            {
                token->kind = VAL;
            }
            else if (!strcmp(buffer, "var"))
            {
                token->kind = VAR;
            }
            else
            {
                offset = len;
            }
            break;
        case 'p':
            if (!strcmp(buffer, "print"))
            {
                token->kind = PRINT;
            }
            else
            {
                offset = len;
            }
            break;
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
                    token->kind = EQ;
                } else {
                    token->kind = ASSIGN;
                }
                tokens.push_back(token);
                break;
            case '+':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->kind = ADD_ASSIGN;
                } else {
                    token->kind = ADD;
                }
                tokens.push_back(token);
                break;
            case '-':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->kind = SUB_ASSIGN;
                } else {
                    token->kind = SUB;
                }
                tokens.push_back(token);
                break;
            case '*':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->kind = MUL_ASSIGN;
                } else {
                    token->kind = MUL;
                }
                tokens.push_back(token);
                break;
            case '/':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->kind = DIV_ASSIGN;
                } else if (file->nextchar() == '/') {
                    while (file->nextchar() != EOF && file->getchar() != '\n');
                    break;
                } else {
                    token->kind = DIV;
                }
                tokens.push_back(token);
                break;
            case '%':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->kind = MOD_ASSIGN;
                } else {
                    token->kind = MOD;
                }
                tokens.push_back(token);
                break;
            case '&':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->kind = AND_ASSIGN;
                } else {
                    token->kind = AND;
                }
                tokens.push_back(token);
                break;
            case '|':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->kind = OR_ASSIGN;
                } else {
                    token->kind = OR;
                }
                tokens.push_back(token);
                break;
            case '!':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->kind = NEQ;
                } else {
                    token->kind = NOT;
                }
                tokens.push_back(token);
                break;
            case '<':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->kind = LE;
                } else {
                    token->kind = LT;
                }
                tokens.push_back(token);
                break;
            case '>':
                if (file->nextchar() == '=') {
                    file->getchar();
                    token->kind = GE;
                } else {
                    token->kind = GT;
                }
                tokens.push_back(token);
                break;
            case ',':
                token->kind = COMMA;
                tokens.push_back(token);
                break;
            case ';':
                token->kind = SEMICOLON;
                tokens.push_back(token);
                break;
            case '(':
                token->kind = L_PAREN;
                tokens.push_back(token);
                break;
            case ')':
                token->kind = R_PAREN;
                tokens.push_back(token);
                break;
            case '[':
                token->kind = L_BRACK;
                tokens.push_back(token);
                break;
            case ']':
                token->kind = R_BRACK;
                tokens.push_back(token);
                break;
            case '{':
                token->kind = L_CURLY;
                tokens.push_back(token);
                break;
            case '}':
                token->kind = R_CURLY;
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
