#include "vs.hpp"

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