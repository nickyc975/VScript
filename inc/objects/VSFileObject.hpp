#ifndef VS_FILE_H
#define VS_FILE_H

#include "VSObject.hpp"
#include "VSStringObject.hpp"

VSObject *vs_open(VSObject *, VSObject *const *args, vs_size_t nargs);

class VSFileObject : public VSObject {
public:
    static const vs_size_t BUFFER_SIZE = 4096;
    static const str_func_map vs_file_methods;

    FILE *_file;

    int _r_buff_pos;
    char _read_buffer[BUFFER_SIZE + 1];

    int _w_buff_pos;
    char _write_buffer[BUFFER_SIZE + 1];

    VSStringObject *name;

    VSFileObject(FILE *file, VSStringObject *name);
    ~VSFileObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
};

#endif