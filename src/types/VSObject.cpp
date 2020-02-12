#include "vs.hpp"
#include "error.hpp"

vs_id_t VSObject::id = 0;

VSObject::VSObject(): obj_id(++id)
{
}

VSObject::VSObject(OBJECT_TYPE type): type(type), obj_id(++id)
{
    
}

VSObject::VSObject(VSValue *value): type(OBJ_DATA), obj_id(++id), value(value)
{

}

VSObject::VSObject(VSCodeObject *codeblock): type(OBJ_CODE), obj_id(++id), codeblock(codeblock)
{

}

VSObject::VSObject(std::vector<VSObject> *obj_list): type(OBJ_LIST), obj_id(++id),obj_list(obj_list)
{

}

VSObject& VSObject::operator=(const VSObject &that)
{
    if (this != &that)
    {
        this->type = that.type;
        switch (that.type)
        {
        case OBJ_DATA:
            this->value = that.value;
            break;
        case OBJ_CODE:
            this->codeblock = that.codeblock;
            break;
        case OBJ_LIST:
            this->obj_list = that.obj_list;
            break;
        default:
            err("unknown object type");
            break;
        }
    }
    return *this;
}