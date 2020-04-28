#include "objects/VSFileObject.hpp"

#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSFunctionObject.hpp"
#include "objects/VSIntObject.hpp"
#include "objects/VSNoneObject.hpp"

VSObject *vs_file_hash(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FILE, "file.__hash__()");

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

    VS_ENSURE_TYPE(self, T_FILE, "file.__eq__()");

    cbool_t res = ((VSFileObject *)self)->name->_value == ((VSFileObject *)that)->name->_value;
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_file_str(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__str__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FILE, "file.__str__()");

    INCREF_RET(
        C_STRING_TO_STRING(
            "file object: " + ((VSFileObject *)self)->name->_value));
}

VSObject *vs_file_bytes(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__bytes__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FILE, "file.__bytes__()");

    INCREF_RET(VS_NONE);
}

VSObject *vs_file_read(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.read()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FILE, "file.read()");

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

    VS_ENSURE_TYPE(self, T_FILE, "file.readable()");

    cbool_t res = !(((VSFileObject *)self)->_file->_flags & _IO_NO_READS);
    INCREF_RET(res ? VS_TRUE : VS_FALSE);
}

VSObject *vs_file_readline(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.readline()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FILE, "file.readline()");

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

    VS_ENSURE_TYPE(self, T_FILE, "file.__hash__()");
}

VSObject *vs_file_writeline(VSObject *self, VSObject *const *args, vs_size_t nargs) {

}

VSObject *vs_file_flush(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FILE, "file.__hash__()");
}

VSObject *vs_file_seek(VSObject *self, VSObject *const *args, vs_size_t nargs) {

}

VSObject *vs_file_seekable(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FILE, "file.__hash__()");
}

VSObject *vs_file_close(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FILE, "file.__hash__()");
}

VSObject *vs_file_closed(VSObject *self, VSObject *const *, vs_size_t nargs) {
    if (nargs != 0) {
        ERR_NARGS("file.__hash__()", 0, nargs);
        terminate(TERM_ERROR);
    }

    VS_ENSURE_TYPE(self, T_FILE, "file.__hash__()");
}

VSFileObject::VSFileObject(FILE *file, VSStringObject *name) {
    this->type = T_FILE;
    this->_file = file;
    this->_r_buff_pos = 0;
    this->_w_buff_pos = 0;

    this->name = name;
    INCREF(name);

    this->attrs["name"] = new AttributeDef(true, this->name);

    NEW_NATIVE_FUNC_ATTR(this, "__hash__", vs_file_hash);
    NEW_NATIVE_FUNC_ATTR(this, "__eq__", vs_file_eq);
    NEW_NATIVE_FUNC_ATTR(this, "__str__", vs_file_str);
    NEW_NATIVE_FUNC_ATTR(this, "__bytes__", vs_file_bytes);
    NEW_NATIVE_FUNC_ATTR(this, "read", vs_file_read);
    NEW_NATIVE_FUNC_ATTR(this, "readable", vs_file_readable);
    NEW_NATIVE_FUNC_ATTR(this, "readline", vs_file_readline);
    NEW_NATIVE_FUNC_ATTR(this, "write", vs_file_write);
    NEW_NATIVE_FUNC_ATTR(this, "writable", vs_file_writable);
    NEW_NATIVE_FUNC_ATTR(this, "writeline", vs_file_writeline);
    NEW_NATIVE_FUNC_ATTR(this, "flush", vs_file_flush);
    NEW_NATIVE_FUNC_ATTR(this, "seek", vs_file_seek);
    NEW_NATIVE_FUNC_ATTR(this, "seekable", vs_file_seekable);
    NEW_NATIVE_FUNC_ATTR(this, "close", vs_file_close);
    NEW_NATIVE_FUNC_ATTR(this, "closed", vs_file_closed);
}

VSFileObject::~VSFileObject() {
    fclose(this->_file);
    DECREF(this->name);
}