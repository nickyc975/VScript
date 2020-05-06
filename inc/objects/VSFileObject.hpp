#ifndef VS_FILE_H
#define VS_FILE_H

#include "VSObject.hpp"
#include "VSStringObject.hpp"

VSObject *vs_open(VSObject *, VSObject *const *args, vs_size_t nargs);

#define FILE_READABLE  0x1
#define FILE_WRITABLE  0x2
#define FILE_SEEKABLE  0x4
#define FILE_CLOSED    0x8
#define FILE_CREATED   0x10
#define FILE_APPENDING 0x20

class VSFileObject : public VSObject {
private:
    static const str_func_map vs_file_methods;

public:
    int flags;
    FILE *_file;
    VSStringObject *name;

    VSFileObject(FILE *file, VSStringObject *name, int flags);
    ~VSFileObject();

    bool hasattr(std::string &attrname) override;
    VSObject *getattr(std::string &attrname) override;
    void setattr(std::string &attrname, VSObject *attrvalue) override;
};

#endif