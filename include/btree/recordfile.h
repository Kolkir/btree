#ifndef _RECORD_FILE_H_
#define _RECORD_FILE_H_

#include <btree/importexport.h>
#include <btree/stream.h>

#include <type_traits>
#include <iostream>
#include <cassert>

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

private:
    size_t maxSize;
    RecAddr addr;
};

class BTREE_API RecordFile
{
public:
    RecordFile(std::iostream& stream);

    ~RecordFile();

    template<class T>
    FileLocation append(const T& value)
    {
        this->stream.beginPack();
        value.pack(this->stream);
        auto size = this->stream.endPack();

        this->stream.seekp(0, std::ios::end);
        auto pos = this->stream.tellp();

        this->stream.flushPack();

        return FileLocation(pos, size);
    }

    template<class T>
    FileLocation write(const FileLocation& loc, const T& value)
    {
        this->stream.beginPack();
        value.pack(this->stream);
        auto size = this->stream.endPack();
        
        if (size <= loc.getMaxSize())
        {
            this->stream.seekp(loc.getAddr(), std::ios::beg);
            this->stream.flushPack();
            return loc;
        }
        else
        {
            return this->append(value);
        }
    }

    template<class T>
    void read(const FileLocation& loc, T& value)
    {
        this->stream.seekg(loc.getAddr(), std::ios::beg);

        this->stream.beginUnPack(maxLen);
        value.unpack(this->stream);
        auto size = this->stream.endUnPack();

        assert(size <= loc.getMaxSize());
        auto pos = this->stream.tellg();
        auto neededPos = static_cast<decltype(pos)>(loc.getAddr() + loc.getMaxSize());
        assert(pos <= neededPos);
    }

private:
    RecordFile(const RecordFile&);
    RecordFile& operator= (const RecordFile&);

private:
    IOStream stream;
    
};

}

#endif
