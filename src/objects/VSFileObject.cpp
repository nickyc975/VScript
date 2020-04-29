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

VSObject *vs_file_read(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.read()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.read()");

    VSFileObject *file = (VSFileObject *)self;

    char c = fgetc(file->_file), end = feof(file->_file);
    while (file->_r_buff_pos < file->BUFFER_SIZE && c != end)
    {
        file->_read_buffer[file->_r_buff_pos] = c;
        c = fgetc(file->_file);
        file->_r_buff_pos++;
    }
    file->_read_buffer[file->_r_buff_pos] = 0;
    file->_r_buff_pos = 0;
    if (c != end) {
        fseek(file->_file, -1, SEEK_CUR);
    }

    INCREF_RET(C_STRING_TO_STRING(std::string(file->_read_buffer)));
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

    char c = fgetc(file->_file), end = feof(file->_file);
    while (file->_r_buff_pos < file->BUFFER_SIZE && c != end && c != '\n')
    {
        file->_read_buffer[file->_r_buff_pos] = c;
        c = fgetc(file->_file);
        file->_r_buff_pos++;
    }
    file->_read_buffer[file->_r_buff_pos] = 0;
    file->_r_buff_pos = 0;
    if (c != end) {
        fseek(file->_file, -1, SEEK_CUR);
    }

    INCREF_RET(C_STRING_TO_STRING(std::string(file->_read_buffer)));
}

VSObject *vs_file_write(VSObject *self, VSObject *const *args, vs_size_t nargs) {

}

VSObject *vs_file_writable(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.__hash__()");
}

VSObject *vs_file_writeline(VSObject *self, VSObject *const *args, vs_size_t nargs) {

}

VSObject *vs_file_flush(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.__hash__()");
}

VSObject *vs_file_seek(VSObject *self, VSObject *const *args, vs_size_t nargs) {

}

VSObject *vs_file_seekable(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.__hash__()");
}

VSObject *vs_file_close(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.__hash__()");
}

VSObject *vs_file_closed(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    ENSURE_TYPE(self, T_FILE, "file.__hash__()");
}

const str_func_map VSFileObject::vs_file_methods = {
    {ID___hash__, vs_file_hash},
    {ID___eq__, vs_file_eq},
    {ID___str__, vs_file_str},
    {ID___bytes__, vs_file_bytes},
    {ID_read, vs_file_read},
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
    this->_r_buff_pos = 0;
    this->_w_buff_pos = 0;

    this->name = name;
    INCREF(name);
}

VSFileObject::~VSFileObject() {
    fclose(this->_file);
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