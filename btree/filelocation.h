#ifndef _FILE_LOCATION_H_
#define _FILE_LOCATION_H_

#include <cstddef>

namespace btree
{

typedef unsigned long long RecAddr;

class RecordFile;

class FileLocation
{
public:
    FileLocation()
        : maxSize(0)
        , addr(0)
    {
    }

    ~FileLocation(){};

    size_t getMaxSize() const {return this->maxSize;}
    RecAddr getAddr() const {return this->addr;}

private:
    friend class RecordFile;

    template <class T, class E>
    friend class Pack;

    template <class T, class E>
    friend class UnPack;

    FileLocation(RecAddr addr, size_t size) : maxSize(size), addr(addr) {}

    void setMaxSize(size_t size){ this->maxSize = size;}

    void setAddr(RecAddr addr) { this->addr = addr;}

    friend bool operator== (const FileLocation& a, const FileLocation& b)
    {
        return a.addr == b.addr && a.maxSize == b.maxSize;
    }

    friend bool operator!= (const FileLocation& a, const FileLocation& b)
    {
        return !(a == b);
    }
private:
    size_t maxSize;
    RecAddr addr;
};

}
#endif
