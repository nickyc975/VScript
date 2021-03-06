#include "compiler/VSTokenizer.hpp"

#include <errno.h>

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSCharObject.hpp"
#include "objects/VSFloatObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSNoneObject.hpp"
#include "objects/VSStringObject.hpp"

#define ERR_WITH_POS(ln, col, msg, ...) err("line: %ld, col: %d, " msg, ln, col, __VA_ARGS__)

VSToken::VSToken(
    TOKEN_TYPE tk_type, VSObject *tk_value, VSObject *literal, long long ln, long long col) : tk_type(tk_type), tk_value(tk_value), literal(literal), ln(ln), col(col) {
    this->refcnt = 1;
    INCREF(tk_value);
    INCREF(literal);
}

VSToken::~VSToken() {
    DECREF_EX(this->tk_value);
    DECREF_EX(this->literal);
}

VSTokenizer::VSTokenizer(FILE *file) {
    this->file = file;
    this->peek = NULL;
    this->ln = 1;
    this->col = 1;
    this->refcnt = 1;
    this->gettoken();
}

VSTokenizer::~VSTokenizer() {
    DECREF_EX(this->peek);
    fclose(this->file);
}

char VSTokenizer::getchar() {
    char c = fgetc(this->file);
    if (c == '\n') {
        this->ln++;
        this->col = 1;
    } else if (!feof(this->file)) {
        this->col++;
    }
    return c;
}

int VSTokenizer::ungetchar() {
    if (fseek(this->file, -1, SEEK_CUR)) {
        return -1;
    }
    if (this->peekchar() == '\n') {
        this->ln--;
        this->col = 1;
    } else if (col > 1) {
        this->col--;
    }
    return 0;
}

char VSTokenizer::peekchar() {
    char c = fgetc(this->file);
    if (!feof(this->file)) {
        fseek(this->file, -1, SEEK_CUR);
    }
    return c;
}

int VSTokenizer::seek(int steps) {
    int seeked = 0;
    if (steps > 0) {
        while (seeked < steps && !feof(this->file)) {
            this->getchar();
            seeked++;
        }
    } else if (steps < 0) {
        while (seeked > steps && !this->ungetchar()) {
            seeked--;
        }
    }
    return seeked;
}

char VSTokenizer::escape(char c) {
    switch (c) {
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

#define APPEND_CHAR(str, c, i) \
    do {                       \
        str.push_back(c);      \
        this->getchar();       \
        c = this->peekchar();  \
        i++;                   \
    } while (0);

int VSTokenizer::getnum(std::string &str) {
    int i = 0;
    char c = this->peekchar();

    // integer part of the number
    while (IS_NUMBER(c)) {
        APPEND_CHAR(str, c, i);
    }

    // if the number has decimal part
    if (c == '.') {
        APPEND_CHAR(str, c, i);
        while (IS_NUMBER(c)) {
            APPEND_CHAR(str, c, i);
        }
    }

    return i;
}

int VSTokenizer::getword(std::string &str) {
    int i = 0;
    char c = this->peekchar();

    while (!IS_WORD_CHAR(c) && !feof(this->file)) {
        this->getchar();
        c = this->peekchar();
    }

    while (IS_WORD_CHAR(c)) {
        APPEND_CHAR(str, c, i);
    }

    return i;
}

int VSTokenizer::getquoted(std::string &str) {
    int i = 0;
    char c = this->peekchar();
    if (feof(this->file) || !IS_QUOTE(c)) {
        return i;
    }

    APPEND_CHAR(str, c, i);
    while (!feof(this->file) && !IS_QUOTE(c)) {
        if (c == '\\') {
            this->getchar();
            c = this->escape(this->peekchar());
        }
        APPEND_CHAR(str, c, i);
    }

    if (!feof(this->file)) {
        APPEND_CHAR(str, c, i);
    }
    return i;
}

int VSTokenizer::getstr(std::string &str, int len) {
    int i = 0;
    char c = this->peekchar();
    while (i < len - 1 && !feof(this->file)) {
        APPEND_CHAR(str, c, i);
    }
    return i;
}

#undef APPEND_CHAR

VSToken *VSTokenizer::reco_none(std::string &literal) {
    if (literal.length() == 4 && literal == "none") {
        INCREF(VS_NONE);
        return new VSToken(TK_CONSTANT, VS_NONE, C_STRING_TO_STRING(literal), this->ln, this->col);
    }
    return NULL;
}

VSToken *VSTokenizer::reco_bool(std::string &literal) {
    VSToken *token = NULL;
    int len = literal.length();
    if (len == 4 && literal == "true") {
        INCREF(VS_TRUE);
        token = new VSToken(TK_CONSTANT, VS_TRUE, C_STRING_TO_STRING(literal), this->ln, this->col);
    } else if (len == 5 && literal == "false") {
        INCREF(VS_FALSE);
        token = new VSToken(TK_CONSTANT, VS_FALSE, C_STRING_TO_STRING(literal), this->ln, this->col);
    }
    return token;
}

VSToken *VSTokenizer::reco_char(std::string &literal) {
    int len = literal.length();
    if (len > 4 || literal.front() != '\'' || literal.back() != '\'' || (len == 4 && literal[1] != '\\')) {
        ERR_WITH_POS(this->ln, this->col, "invalid char literal: \"%s\"", literal.c_str());
        return NULL;
    }

    VSToken *token = NULL;
    if (literal[1] == '\\') {
        token = new VSToken(
            TK_CONSTANT, C_CHAR_TO_CHAR(escape(literal[2])), C_STRING_TO_STRING(literal), this->ln, this->col);
    } else {
        token = new VSToken(
            TK_CONSTANT, C_CHAR_TO_CHAR(literal[1]), C_STRING_TO_STRING(literal), this->ln, this->col);
    }
    return token;
}

VSToken *VSTokenizer::reco_num(std::string &literal) {
    if (literal.find(".") != literal.npos) {
        char *end = NULL;
        cfloat_t float_val = std::strtold(literal.c_str(), &end);
        if (errno == ERANGE) {
            errno = 0;
            ERR_WITH_POS(this->ln, this->col, "literal out of range of float: \"%s\"", literal.c_str());
            return NULL;
        }

        if (end - 1 != &(literal.back())) {
            ERR_WITH_POS(this->ln, this->col, "invalid float literal: \"%s\"", literal.c_str());
            return NULL;
        }
        return new VSToken(TK_CONSTANT, C_FLOAT_TO_FLOAT(float_val), C_STRING_TO_STRING(literal), this->ln, this->col);
    } else {
        char *end = NULL;
        cint_t int_val = std::strtoll(literal.c_str(), &end, 0);
        if (errno == ERANGE) {
            errno = 0;
            ERR_WITH_POS(this->ln, this->col, "literal out of range of int: \"%s\"", literal.c_str());
            return NULL;
        }

        if (end - 1 != &(literal.back())) {
            ERR_WITH_POS(this->ln, this->col, "invalid int literal: \"%s\"", literal.c_str());
            return NULL;
        }
        return new VSToken(TK_CONSTANT, C_INT_TO_INT(int_val), C_STRING_TO_STRING(literal), this->ln, this->col);
    }
}

VSToken *VSTokenizer::reco_str(std::string &literal) {
    if (literal.front() != '\"' || literal.back() != '\"') {
        ERR_WITH_POS(this->ln, this->col, "invalid string literal: \"%s\"", literal.c_str());
        return NULL;
    }
    VSObject *strobj = C_STRING_TO_STRING(literal.substr(1, literal.length() - 2));
    return new VSToken(TK_CONSTANT, strobj, strobj, this->ln, this->col);
}

VSToken *VSTokenizer::reco_kwd(std::string &literal) {
#define NEW_KWD_TOKEN(tp) new VSToken(tp, NULL, C_STRING_TO_STRING(literal), this->ln, this->col)

    VSToken *token = NULL;
    switch (literal[0]) {
        case 'b':
            if (literal == "break") {
                token = NEW_KWD_TOKEN(TK_BREAK);
            }
            break;
        case 'c':
            if (literal == "continue") {
                token = NEW_KWD_TOKEN(TK_CONTINUE);
            }
            break;
        case 'e':
            if (literal == "else") {
                token = NEW_KWD_TOKEN(TK_ELSE);
            } else if (literal == "elif") {
                token = NEW_KWD_TOKEN(TK_ELIF);
            }
            break;
        case 'f':
            if (literal == "for") {
                token = NEW_KWD_TOKEN(TK_FOR);
            } else if (literal == "func") {
                token = NEW_KWD_TOKEN(TK_FUNC);
            }
            break;
        case 'i':
            if (literal == "if") {
                token = NEW_KWD_TOKEN(TK_IF);
            }
            break;
        case 'l':
            if (literal == "lambda") {
                token = NEW_KWD_TOKEN(TK_LAMBDA);
            }
            break;
        case 'r':
            if (literal == "return") {
                token = NEW_KWD_TOKEN(TK_RETURN);
            }
            break;
        case 'v':
            if (literal == "val") {
                token = NEW_KWD_TOKEN(TK_VAL);
            } else if (literal == "var") {
                token = NEW_KWD_TOKEN(TK_VAR);
            }
            break;
        case 'w':
            if (literal == "while") {
                token = NEW_KWD_TOKEN(TK_WHILE);
            }
            break;
        default:
            break;
    }
    return token;

#undef NEW_KWD_TOKEN
}

bool VSTokenizer::hastoken() {
    return this->peektoken() != NULL || !feof(this->file);
}

VSToken *VSTokenizer::gettoken() {
    VSToken *old = this->peek;
    auto literal = std::string();

begain:
    char tk_char = this->peekchar();

    if (IS_NUMBER(tk_char)) {
        this->getnum(literal);
        this->peek = this->reco_num(literal);
        if (this->peek == NULL) {
            INCREF(VS_NONE);
            this->peek = new VSToken(TK_CONSTANT, VS_NONE, C_STRING_TO_STRING(literal), this->ln, this->col);
        }
    } else if (IS_WORD_CHAR(tk_char)) {
        this->getword(literal);

        // true or false
        if (tk_char == 't' || tk_char == 'f') {
            this->peek = this->reco_bool(literal);
            if (this->peek != NULL) {
                goto done;
            }
        }

        // none
        if (tk_char == 'n') {
            this->peek = this->reco_none(literal);
            if (this->peek != NULL) {
                goto done;
            }
        }

        // keyword
        this->peek = this->reco_kwd(literal);
        if (this->peek != NULL) {
            goto done;
        }

        // identifier
        this->peek = new VSToken(TK_IDENTIFIER, NULL, C_STRING_TO_STRING(literal), this->ln, this->col);
    } else if (IS_QUOTE(tk_char)) {
        this->getquoted(literal);

        if (tk_char == '\'') {
            this->peek = this->reco_char(literal);
            if (this->peek == NULL) {
                INCREF(VS_NONE);
                this->peek = new VSToken(TK_CONSTANT, VS_NONE, C_STRING_TO_STRING(literal), this->ln, this->col);
            }
        } else {
            this->peek = this->reco_str(literal);
            if (this->peek == NULL) {
                INCREF(VS_NONE);
                this->peek = new VSToken(TK_CONSTANT, VS_NONE, C_STRING_TO_STRING(literal), this->ln, this->col);
            }
        }

    } else {
#define NEW_SYM_TOKEN(tp, literal) new VSToken(tp, NULL, C_STRING_TO_STRING(literal), this->ln, this->col)

        tk_char = this->getchar();
        switch (tk_char) {
            case '=':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_EQ, "==");
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_ASSIGN, "=");
                }
                break;
            case '+':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_ADD_ASSIGN, "+=");
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_ADD, "+");
                }
                break;
            case '-':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_SUB_ASSIGN, "-=");
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_SUB, "-");
                }
                break;
            case '*':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_MUL_ASSIGN, "*=");
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_MUL, "*");
                }
                break;
            case '/':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_DIV_ASSIGN, "/=");
                } else if (this->peekchar() == '/') {
                    while (!feof(this->file) && this->getchar() != '\n')
                        ;
                    goto begain;
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_DIV, "/");
                }
                break;
            case '%':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_MOD_ASSIGN, "%=");
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_MOD, "%");
                }
                break;
            case '&':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_AND_ASSIGN, "&=");
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_AND, "&");
                }
                break;
            case '|':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_OR_ASSIGN, "|=");
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_OR, "|");
                }
                break;
            case '^':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_XOR_ASSIGN, "^=");
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_XOR, "^");
                }
                break;
            case '!':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_NEQ, "!=");
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_NOT, "!");
                }
                break;
            case '<':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_LE, "<=");
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_LT, "<");
                }
                break;
            case '>':
                if (this->peekchar() == '=') {
                    this->getchar();
                    this->peek = NEW_SYM_TOKEN(TK_GE, ">=");
                } else {
                    this->peek = NEW_SYM_TOKEN(TK_GT, ">");
                }
                break;
            case ',':
                this->peek = NEW_SYM_TOKEN(TK_COMMA, ",");
                break;
            case '.':
                this->peek = NEW_SYM_TOKEN(TK_DOT, ".");
                break;
            case ';':
                this->peek = NEW_SYM_TOKEN(TK_SEMICOLON, ";");
                break;
            case ':':
                this->peek = NEW_SYM_TOKEN(TK_COLON, ":");
                break;
            case '(':
                this->peek = NEW_SYM_TOKEN(TK_L_PAREN, "(");
                break;
            case ')':
                this->peek = NEW_SYM_TOKEN(TK_R_PAREN, ")");
                break;
            case '[':
                this->peek = NEW_SYM_TOKEN(TK_L_BRACK, "[");
                break;
            case ']':
                this->peek = NEW_SYM_TOKEN(TK_R_BRACK, "]");
                break;
            case '{':
                this->peek = NEW_SYM_TOKEN(TK_L_CURLY, "{");
                break;
            case '}':
                this->peek = NEW_SYM_TOKEN(TK_R_CURLY, "}");
                break;
            case ' ':
            case '\n':
            case '\t':
            case '\r':
                goto begain;
            default:
                this->peek = NULL;
                if (!feof(this->file)) {
                    ERR_WITH_POS(this->ln, this->col, "illegal token: \"%c\"\n", tk_char);
                }
                break;
        }

#undef NEW_SYM_TOKEN
    }

done:
    // if (this->peek == NULL) {
    //     note("peek token is null");
    // } else {
    //     note("got token: %s, %s", TOKEN_STR[this->peek->tk_type], STRING_TO_C_STRING(this->peek->literal).c_str());
    //     if (this->peek->tk_type == TK_CONSTANT) {
    //         note("constant type: %s", TYPE_STR[this->peek->tk_value->type]);
    //     }
    // }
    return old;
}

VSToken *VSTokenizer::peektoken() {
    return this->peek;
}
