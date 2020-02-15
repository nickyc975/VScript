#ifndef VS_ERROR_H
#define VS_ERROR_H

#define err(...)    __vs_report__(ERR, __VA_ARGS__)
#define warn(...)   __vs_report__(WARN, __VA_ARGS__)
#define note(...)   __vs_report__(NOTE, __VA_ARGS__)

#define u_op_err(op, type) \
    err("Can not apply operation \"%s\" on type \"%s\"\n", op, type)

#define u_op_err_lno(lno, op, type) \
    err("line: %ld, Can not apply operation \"%s\" on type \"%s\"\n", lno, op, type)

#define b_op_err(op, ltype, rtype) \
    err("Can not apply operation \"%s\" on type \"%s\" and type \"%s\"\n", op, ltype, rtype)

#define b_op_err_lno(lno, op, ltype, rtype) \
    err("line: %ld, Can not apply operation \"%s\" on type \"%s\" and type \"%s\"\n", lno, op, ltype, rtype)

typedef enum
{
    ERR,
    WARN,
    NOTE
} RE_TAG;

typedef enum
{
    TERM_ERROR,
    TERM_WARN,
    TERM_NORM
} TERM_STATUS;

void __vs_report__(RE_TAG tag, char *fmt, ...);

void terminate(TERM_STATUS status);

#endif