#ifndef VS_ERROR_H
#define VS_ERROR_H

#define err(...)    __vs_report__(ERR, __VA_ARGS__)
#define warn(...)   __vs_report__(WARN, __VA_ARGS__)
#define note(...)   __vs_report__(NOTE, __VA_ARGS__)

typedef enum
{
    ERR,
    WARN,
    NOTE
} RE_TAG;

void __vs_report__(RE_TAG tag, char *fmt, ...);

#endif