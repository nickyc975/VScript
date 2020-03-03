#include "error.hpp"
#include "objects/VSBoolObject.hpp"
#include "objects/VSObject.hpp"

VSObject *VSObject::getlen(VSObject *obj)
{
    
}

vs_size_t VSObject::c_getlen(VSObject *obj)
{

}

VSObject *VSObject::getitem_at(VSObject *container, VSObject *key)
{

}

void VSObject::setitem_at(VSObject *container, VSObject *key, VSObject *value)
{

}

VSObject *VSObject::hasitem_at(VSObject *container, VSObject *key)
{

}

void VSObject::removeitem_at(VSObject *container, VSObject *key)
{

}

void VSObject::appenditem(VSObject *container, VSObject *item)
{

}

VSObject *VSObject::hasitem(VSObject *container, VSObject *item)
{

}

void VSObject::removeitem(VSObject *container, VSObject *item)
{

}

VSObject *vs_hash_not_implemented(const VSObject *obj)
{
    err("%s.__hash__() not implemented.", vs_typeof(obj)->__name__.c_str());
    terminate(TERM_ERROR);
    return NULL;
}

VSObject *vs_default_eq(const VSObject *a, const VSObject *b)
{
    return vs_bool_from_cbool(a == b);
}