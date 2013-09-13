#ifndef _FILE_LOCATION_H_
#define _FILE_LOCATION_H_

#include <btree/importexport.h>
#include <btree/stream.h>

namespace btree
{

typedef unsigned long long RecAddr;

class RecordFile;

class BTREE_API FileLocation
{
public:
    ~FileLocation(){};

private:
    friend class RecordFile;

    FileLocation(RecAddr addr, size_t size) : maxSize(size), addr(addr) {}

    size_t getMaxSize() const {return this->maxSize;}
    void setMaxSize(size_t size){ this->maxSize = size;}

    RecAddr getAddr() const {return this->addr;}
    void setAddr(RecAddr addr) { this->addr = addr;}

    friend bool operator== (const FileLocation& a, const FileLocation& b)
    {
        return a.addr == b.addr && a.maxSize == b.maxSize;
    }
private:
    size_t maxSize;
    RecAddr addr;
};

}
#endif
