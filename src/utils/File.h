#include <stdio.h>
#include <stdlib.h>

#ifndef File_H

#define File_H
#define READ_BUFFER_SIZE 1024

#define is_number(c) ((c) >= '0' && (c) <= '9')

#define is_alphabet(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))

#define is_word(c) ((is_alphabet(c)) || (is_number(c)) || (c) == '_')

#define is_quote(c) ((c) == '\'' || (c) == '\"')

class File
{
private:
    FILE *file;
    long long cursor;
    char buffer[READ_BUFFER_SIZE];

public:
    long long ln, col;

    File(FILE *file);
    ~File();

    char getchar();
    char nextchar();
    int seek(int step);
    int getstr(char *str_buffer, int len);
    int getline(char *line_buffer, int len);
    int getword(char *word_buffer, int len);
    int getnum(char *num_buffer, int len);
};

#endif