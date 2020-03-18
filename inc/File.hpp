#ifndef VS_FILE_H
#define VS_FILE_H

#include <cstdio>
#include <cstdlib>
#include <string>

#define is_number(c) ((c) >= '0' && (c) <= '9')

#define is_alphabet(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

#define is_word(c) ((is_alphabet(c)) || (is_number(c)) || (c) == '_')

#define is_quote(c) ((c) == '\'' || (c) == '\"')

class File
{
private:
    FILE *file;
    long long ln, col;

public:
    File(FILE *file);
    ~File();

    long long cur_ln();
    long long cur_col();
    char getchar();
    int ungetchar();
    char nextchar();
    int seek(int steps);
    int getstr(std::string &str, int len);
    int getline(std::string &str);
    int getword(std::string &str);
    int getnum(std::string &str);
};

#endif