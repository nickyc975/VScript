#include "objects/VSFileObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSNoneObject.hpp"

NEW_IDENTIFIER(__hash__);
NEW_IDENTIFIER(__eq__);
NEW_IDENTIFIER(__str__);
NEW_IDENTIFIER(__bytes__);
NEW_IDENTIFIER(read);
NEW_IDENTIFIER(hasmore);
NEW_IDENTIFIER(readable);
NEW_IDENTIFIER(readline);
NEW_IDENTIFIER(write);
NEW_IDENTIFIER(writable);
NEW_IDENTIFIER(writeline);
NEW_IDENTIFIER(flush);
NEW_IDENTIFIER(seek);
NEW_IDENTIFIER(seekable);
NEW_IDENTIFIER(close);
NEW_IDENTIFIER(closed);

VSObject *vs_open(VSObject *, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 2) {
        ERR_NARGS("open()", 2, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(args[0], T_STR, "as file name");
    ENSURE_TYPE(args[1], T_STR, "as open mode");

    std::string &name = STRING_TO_C_STRING(args[0]);
    std::string &mode = STRING_TO_C_STRING(args[1]);

    FILE *_file = fopen(name.c_str(), mode.c_str());
    if (_file == NULL) {
        perror("open()");
        terminate(TERM_ERROR);
    }

    INCREF_RET(new VSFileObject(_file, (VSStringObject *)args[0]));
}

VSObject *vs_file_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.__hash__()");

    cint_t hash = (cint_t)std::hash<std::string>{}(((VSFileObject *)self)->name->_value);
    INCREF_RET(C_INT_TO_INT(hash));
}

VSObject *vs_file_eq(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("file.__eq__()", 1, nargs);
        terminate(TERM_ERROR);
    }

    VSObject *that = args[0];
    if (that->type != self->type) {
        INCREF_RET(VS_FALSE);
    }

    ENSURE_TYPE(self, T_FILE, "file.__eq__()");

    cbool_t res = ((VSFileObject *)self)->name->_value == ((VSFileObject *)that)->name->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_file_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.__str__()");

    INCREF_RET(
        C_STRING_TO_STRING(
            "file object: " + ((VSFileObject *)self)->name->_value));
}

VSObject *vs_file_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_file_read(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    vs_size_t len = 0;
    if (nargs == 0) {
        len = UINT64_MAX;
    } else if (nargs == 1) {
        VSObject *lenobj = args[0];
        ENSURE_TYPE(lenobj, T_INT, "as length");
        len = (vs_size_t)((VSIntObject *)lenobj)->_value;
    } else {
        ERR_NARGS("file.read()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.read()");

    VSFileObject *file = (VSFileObject *)self;

    if (file->_file->_flags & _IO_NO_READS) {
        err("file \"%s\" is not readable", file->name->_value.c_str());
        terminate(TERM_ERROR);
    }

    vs_size_t read = 0;
    char c = fgetc(file->_file);
    std::string str = std::string();
    while (!feof(file->_file) && read < len)
    {
        read++;
        str.push_back(c);
        c = fgetc(file->_file);
    }

    if (ferror(file->_file)) {
        perror("file.read()");
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_STRING_TO_STRING(str));
}

VSObject *vs_file_hasmore(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.hasmore()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.hasmore()");

    cbool_t res = !feof(((VSFileObject *)self)->_file);
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_file_readable(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.readable()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.readable()");

    cbool_t res = !(((VSFileObject *)self)->_file->_flags & _IO_NO_READS);
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_file_readline(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.readline()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.readline()");

    VSFileObject *file = (VSFileObject *)self;

    if (file->_file->_flags & _IO_NO_READS) {
        err("file \"%s\" is not readable", file->name->_value.c_str());
        terminate(TERM_ERROR);
    }

    char c = fgetc(file->_file);
    std::string str = std::string();
    while (!feof(file->_file) && c != '\n' && c != '\r') {
        str.push_back(c);
        c = fgetc(file->_file);
    }

    if (c == '\r') {
        c = fgetc(file->_file);
        if (c != '\n') {
            ungetc(c, file->_file);
        }
    }

    if (ferror(file->_file)) {
        perror("file.readline()");
        terminate(TERM_ERROR);
    }

    INCREF_RET(C_STRING_TO_STRING(str));
}

VSObject *vs_file_write(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("file.write()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.write()");
    ENSURE_TYPE(args[0], T_STR, "file.write()");

    VSFileObject *file = (VSFileObject *)self;
    std::string &str = STRING_TO_C_STRING(args[0]);

    if (file->_file->_flags & _IO_NO_WRITES) {
        err("file \"%s\" is not writable", file->name->_value.c_str());
        terminate(TERM_ERROR);
    }

    fputs(str.c_str(), file->_file);
    if (ferror(file->_file)) {
        perror("file.write()");
        terminate(TERM_ERROR);
    }
    INCREF_RET(VS_NONE);
}

VSObject *vs_file_writable(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.writable()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.writable()");

    cbool_t res = !(((VSFileObject *)self)->_file->_flags & _IO_NO_WRITES);
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_file_writeline(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    if (nargs != 1) {
        ERR_NARGS("file.writeline()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.writeline()");
    ENSURE_TYPE(args[0], T_STR, "file.writeline()");

    VSFileObject *file = (VSFileObject *)self;
    std::string &str = STRING_TO_C_STRING(args[0]);

    if (file->_file->_flags & _IO_NO_WRITES) {
        err("file \"%s\" is not writable", file->name->_value.c_str());
        terminate(TERM_ERROR);
    }

    fputs(str.c_str(), file->_file);
    fputc('\n', file->_file);
    if (ferror(file->_file)) {
        perror("file.writeline()");
        terminate(TERM_ERROR);
    }
    INCREF_RET(VS_NONE);
}

VSObject *vs_file_flush(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.flush()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.flush()");

    fflush(((VSFileObject *)self)->_file);
    if (ferror(((VSFileObject *)self)->_file)) {
        perror("file.flush()");
        terminate(TERM_ERROR);
    }
    INCREF_RET(VS_NONE);
}

VSObject *vs_file_seek(VSObject *self, VSObject *const *args, vs_size_t nargs) {
    long steps = 0, whence = SEEK_CUR;
    if (nargs == 1) {
        VSObject *stepsobj = args[0];
        ENSURE_TYPE(stepsobj, T_INT, "as file.seek() steps");
        steps = (int)((VSIntObject *)stepsobj)->_value;
    } else if (nargs == 2) {
        VSObject *stepsobj = args[0];
        VSObject *whenceobj = args[1];
        ENSURE_TYPE(stepsobj, T_INT, "as file.seek() steps");
        ENSURE_TYPE(whenceobj, T_INT, "as file.seek() whence");
        steps = (int)((VSIntObject *)stepsobj)->_value;
        whence = (int)((VSIntObject *)whenceobj)->_value;
    } else {
        ERR_NARGS("file.seek()", 1, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.seek()");
    if (whence < 0 || whence > 4) {
        err("invalid whence value: %ld for file.seek(), must be one of 0, 1, 2, 3, 4", whence);
        terminate(TERM_ERROR);
    }

    fseek(((VSFileObject *)self)->_file, steps, whence);
    if (ferror(((VSFileObject *)self)->_file)) {
        perror("file.seek()");
        terminate(TERM_ERROR);
    }
    INCREF_RET(VS_NONE);
}

VSObject *vs_file_seekable(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.seekable()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.seekable()");
    INCREF_RET(((VSFileObject *)self)->closed ? VS_TRUE : VS_FALSE);
}

VSObject *vs_file_close(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.close()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.close()");

    VSFileObject *file = (VSFileObject *)self;
    if (!file->closed) {
        fclose(file->_file);
        file->closed = true;
    }
    INCREF_RET(VS_NONE);
}

VSObject *vs_file_closed(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.closed()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.closed()");
    INCREF_RET(((VSFileObject *)self)->closed ? VS_TRUE : VS_FALSE);
}

const str_func_map VSFileObject::vs_file_methods = {
    {ID___hash__, vs_file_hash},
    {ID___eq__, vs_file_eq},
    {ID___str__, vs_file_str},
    {ID___bytes__, vs_file_bytes},
    {ID_read, vs_file_read},
    {ID_hasmore, vs_file_hasmore},
    {ID_readable, vs_file_readable},
    {ID_readline, vs_file_readline},
    {ID_write, vs_file_write},
    {ID_writable, vs_file_writable},
    {ID_writeline, vs_file_writeline},
    {ID_flush, vs_file_flush},
    {ID_seek, vs_file_seek},
    {ID_seekable, vs_file_seekable},
    {ID_close, vs_file_close},
    {ID_closed, vs_file_closed}
};

VSFileObject::VSFileObject(FILE *file, VSStringObject *name) {
    this->type = T_FILE;
    this->_file = file;
    this->closed = false;
    this->name = name;
    INCREF(name);
}

VSFileObject::~VSFileObject() {
    if (!this->closed) {
        fclose(this->_file);
        this->closed = true;
    }
    DECREF(this->name);
}

bool VSFileObject::hasattr(std::string &attrname) {
    return vs_file_methods.find(attrname) != vs_file_methods.end();
}

VSObject *VSFileObject::getattr(std::string &attrname) {
    auto iter = vs_file_methods.find(attrname);
    if (iter == vs_file_methods.end()) {
        ERR_NO_ATTR(this, attrname);
        terminate(TERM_ERROR);
    }

    VSFunctionObject *attr = new VSNativeFunctionObject(
        this, C_STRING_TO_STRING(attrname), vs_file_methods.at(attrname));
    INCREF_RET(attr);
}

void VSFileObject::setattr(std::string &attrname, VSObject *attrvalue) {
    err("Unable to apply setattr on native type: \"%s\"", TYPE_STR[this->type]);
    terminate(TERM_ERROR);
}