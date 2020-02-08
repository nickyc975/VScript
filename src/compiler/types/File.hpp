#ifndef VSC_FILE_H
#define VSC_FILE_H

#include <cstdio>
#include <cstdlib>

#define READ_BUFFER_SIZE 1024

#define is_number(c) ((c) >= '0' && (c) <= '9')

#define is_alphabet(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

#define is_word(c) ((is_alphabet(c)) || (is_number(c)) || (c) == '_')

#define is_quote(c) ((c) == '\'' || (c) == '\"')

class File
{
private:
    FILE *file;
    long long ln, col;
    char buffer[READ_BUFFER_SIZE];

public:
    File(FILE *file);
    ~File();

    long long cur_ln();
    long long cur_col();
    char getchar();
    int ungetchar();
    char nextchar();
    int seek(int steps);
    int getstr(char *str_buffer, int len);
    int getline(char *line_buffer, int len);
    int getword(char *word_buffer, int len);
    int getnum(char *num_buffer, int len);
};

#endif