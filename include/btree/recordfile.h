#ifndef _RECORD_FILE_H_
#define _RECORD_FILE_H_

#include <btree/importexport.h>
#include <btree/filelocation.h>
#include <btree/stream.h>

#include <type_traits>
#include <iostream>
#include <cassert>

namespace btree
{

class BTREE_API RecordFile
{
public:
    RecordFile(std::iostream& stream);

    ~RecordFile();

    template<class T, class PackFunc>
    FileLocation append(const T& value, PackFunc packFunc)
    {
        this->stream.resetPack();
        size_t size = 0;
        this->stream.pack(size);
        packFunc(value, this->stream);
        size = this->stream.getPackSize();

        this->stream.seekp(0, std::ios::end);
        auto pos = this->stream.tellp();

        this->stream.pack(0, size);
        this->stream.flushPack();

        return FileLocation(pos, size);
    }

    template<class T, class PackFunc>
    FileLocation write(const FileLocation& loc, const T& value, PackFunc packFunc)
    {
        this->stream.resetPack();
        size_t size = 0;
        this->stream.pack(size); // wite fake value to calculate real size
        packFunc(value, this->stream);
        size = this->stream.getPackSize();
        
        if (size <= loc.getMaxSize())
        {
            this->stream.seekp(loc.getAddr(), std::ios::beg);
            size += sizeof(size);
            this->stream.pack(0, size); //owerwite with real record size
            this->stream.flushPack();
            return loc;
        }
        else
        {
            //append
            this->stream.seekp(0, std::ios::end);
            auto pos = this->stream.tellp();
            size += sizeof(size);
            this->stream.pack(0, size);  //owerwite with real record size
            this->stream.flushPack();

            return FileLocation(pos, size);
        }
    }

    template<class T, class UnpackFunc>
    void read(const FileLocation& loc, T& value, UnpackFunc unpackFunc)
    {
        this->stream.seekg(loc.getAddr(), std::ios::beg);
        
        this->stream.beginUnPack(loc.getMaxSize());
        
        size_t recSize = 0;
        this->stream.unpack(recSize);
        assert(recSize <= loc.getMaxSize());

        unpackFunc(value, this->stream);
        auto size = this->stream.getUnPackSize();
        if (size > 0)
        {
            assert(size <= loc.getMaxSize());
            auto pos = this->stream.tellg();
            auto maxPos = static_cast<decltype(pos)>(loc.getAddr() + sizeof(size_t) + loc.getMaxSize());
            assert(pos <= maxPos);
        }
    }

private:
    RecordFile(const RecordFile&);
    RecordFile& operator= (const RecordFile&);

private:
    IOStream stream;
    
};

}

#endif
