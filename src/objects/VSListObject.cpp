#include "vs.hpp"

VSObjectList::VSListObject()
{
    this->data = std::vector<VSObject>();
    this->size = 0;
}

VSObjectList::~VSObjectList()
{
    for (auto obj : this->data)
    {
        obj.decref();
    }
}

VSObjectList &VSObjectList::operator=(const VSObjectList &that)
{
    this->data = that.data;
    for (auto obj : this->data)
    {
        obj.incref();
    }
    this->size = that.size;
    return *this;
}

void VSObjectList::push(VSObject object)
{
    this->data.push_back(object);
    object.incref();
    this->size++;
}

VSObject VSObjectList::back()
{
    return this->data.back();
}

void VSObjectList::pop()
{
    this->data.back().decref();
    this->data.pop_back();
    this->size--;
}

void VSObjectList::put(vs_addr_t pos, VSObject object)
{
    this->data[pos].decref();
    this->data[pos] = object;
    this->data[pos].incref();
    this->size++;
}

VSObject VSObjectList::get(vs_addr_t pos)
{
    return this->data[pos];
}

void VSObjectList::remove(vs_addr_t pos)
{
    this->data[pos].decref();
    this->data.erase(this->data.begin() + pos);
}

vs_size_t VSObjectList::length()
{
    return this->size;
}

vs_bool_t VSObjectList::empty()
{
    return this->data.empty();
}

void VSObjectList::clear()
{
    for (auto obj : this->data)
    {
        obj.decref();
    }
    this->data.clear();
    this->size = 0;
}