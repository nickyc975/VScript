#include <cstdio>
#include <cstdarg>

#include "error.hpp"

char *TAG_STR[] = {"ERROR", "WARNING", "NOTE"};

void __vs_report__(RE_TAG tag, char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    switch (tag)
    {
    case ERR:
        fprintf(stderr, "%s:\t", TAG_STR[tag]);
        vfprintf(stderr, fmt, args);
        break;
    default:
        fprintf(stdout, "%s:\t", TAG_STR[tag]);
        vfprintf(stdout, fmt, args);
        break;
    }
    va_end(args);
}