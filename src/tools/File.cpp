#include "File.hpp"

File::File(FILE *file)
{
    this->file = file;
    this->ln = 1, this->col = 1;
}

File::~File()
{
    fclose(this->file);
}

long long File::cur_ln()
{
    return this->ln;
}

long long File::cur_col()
{
    return this->col;
}

char File::getchar()
{
    char c = fgetc(this->file);
    if (c == '\n')
    {
        this->ln++;
        this->col = 1;
    }
    else if (c != EOF)
    {
        this->col++;
    }
    return c;
}

int File::ungetchar()
{
    if (fseek(this->file, -1, SEEK_CUR))
    {
        return -1;
    }
    if (this->nextchar() == '\n')
    {
        this->ln--;
        this->col = 1;
    }
    else if (col > 1)
    {
        this->col--;
    }
    return 0;
}

char File::nextchar()
{
    char c = fgetc(this->file);
    if (c != EOF)
        fseek(this->file, -1, SEEK_CUR);
    return c;
}

int File::seek(int steps)
{
    int seeked = 0;
    if (steps > 0)
    {
        while (seeked < steps && this->nextchar() != EOF)
        {
            this->getchar();
            seeked++;
        }
    }
    else if (steps < 0)
    {
        while (seeked > steps && !this->ungetchar())
        {
            seeked--;
        }
    }
    return seeked;
}

int File::getstr(char *str_buffer, int len)
{
    int i = 0;
    char c = this->nextchar();
    while (i < len - 1 && c != 0)
    {
        str_buffer[i] = this->getchar();
        c = this->nextchar();
        i++;
    }
    str_buffer[i] = 0;
    return i;
}

int File::getline(char *line_buffer, int len)
{
    int i = 0;
    char c = this->getchar();
    while (i < len - 1 && c != 0 && c != '\n')
    {
        line_buffer[i] = c;
        c = this->getchar();
        i++;
    }
    line_buffer[i] = 0;
    return i;
}

int File::getword(char *word_buffer, int len)
{
    int i = 0;
    char next_char = this->nextchar();

    while (!is_word(next_char) && next_char != 0)
    {
        this->getchar();
        next_char = this->nextchar();
    }

    while (i < len - 1 && is_word(next_char))
    {
        word_buffer[i] = this->getchar();
        next_char = this->nextchar();
        i++;
    }

    word_buffer[i] = 0;
    return i;
}

int File::getnum(char *num_buffer, int len)
{
    int i = 0;
    char c = this->getchar();

    while (!is_number(c) && c != 0)
    {
        c = this->getchar();
    }

    // number 0
    if (c == '0' && this->nextchar() != '.')
    {
        num_buffer[i++] = c;
    }
    else
    {
        // integer part of the number
        while (i < len - 1 && is_number(c))
        {
            num_buffer[i++] = c;
            c = this->getchar();
        }

        // if the number has decimal part
        if (i < len - 1 && c == '.')
        {
            num_buffer[i++] = c;
            c = this->getchar();
            while (i < len - 1 && is_number(c))
            {
                num_buffer[i++] = c;
                c = this->getchar();
            }
        }

        if (c != 0)
        {
            this->seek(-1);
        }
    }

    num_buffer[i] = 0;
    return i;
}