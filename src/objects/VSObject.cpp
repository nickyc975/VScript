#include "vs.hpp"
#include "error.hpp"

vs_id_t VSObject::id = 0;

VSObject::VSObject() : objid(++id)
{
    this->type = OBJ_DATA;
    this->value = VSValue::None();
}

VSObject::VSObject(VSValue *value) : type(OBJ_DATA), objid(++id), value(value)
{
}

VSObject::VSObject(VSCodeObject *codeblock) : type(OBJ_CODE), objid(++id), codeblock(codeblock)
{
}

VSObject::VSObject(VSObjectList *objlist) : type(OBJ_LIST), objid(++id), objlist(objlist)
{
}

VSObject &VSObject::operator=(const VSObject &that)
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
            this->objlist = that.objlist;
            break;
        default:
            err("unknown object type");
            break;
        }
    }
    return *this;
}

void VSObject::incref()
{
    if (this->type == OBJ_DATA)
    {
        if (this->value->type == NONE || this->value->type == BOOL)
        {
            return;
        }
        this->value->refcnt++;
    }
    else if (this->type == OBJ_LIST)
    {
        this->objlist->refcnt++;
    }
}

void VSObject::decref()
{
    if (this->type == OBJ_DATA)
    {
        if (this->value->type == NONE || this->value->type == BOOL)
        {
            return;
        }
        this->value->refcnt--;
        if (this->value->refcnt == 0)
        {
            delete this->value;
            this->value = VSValue::None();
        }
    }
    else if (this->type == OBJ_LIST)
    {
        this->objlist->refcnt--;
        if (this->objlist->refcnt == 0)
        {
            delete this->objlist;
            this->type = OBJ_DATA;
            this->value = VSValue::None();
        }
    }
}