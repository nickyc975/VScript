#include "vs.hpp"

VSObjectList::VSObjectList()
{
    this->data = std::vector<VSObject>();
    this->size = 0;
}

VSObjectList &VSObjectList::operator=(const VSObjectList &that)
{
    this->data = that.data;
    this->size = that.size;
    return *this;
}

VSObject &VSObjectList::operator[](vs_addr_t pos)
{
    return this->data[pos];
}

void VSObjectList::add(VSObject object)
{
    this->data.push_back(object);
    this->size++;
}

void VSObjectList::put(vs_addr_t pos, VSObject object)
{
    this->data[pos] = object;
    this->size++;
}

VSObject VSObjectList::get(vs_addr_t pos)
{
    return this->data[pos];
}

VSObject VSObjectList::remove()
{
    VSObject object = this->data.back();
    this->data.pop_back();
    this->size--;
    return object;
}

void VSObjectList::clear()
{
    this->data.clear();
    this->size = 0;
}

vs_size_t VSObjectList::length()
{
    return this->size;
}

vs_bool_t VSObjectList::empty()
{
    return this->data.empty();
}