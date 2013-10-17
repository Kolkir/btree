#ifndef _IOBUFFER_H_
#define _IOBUFFER_H_

#include <btree/importexport.h>
#include <btree/defpack.h>

#include <iterator>
#include <iostream>
#include <type_traits>
#include <vector>
#include <cassert>

namespace btree
{

class BTREE_API IOStream
{
public:
    IOStream(std::iostream& stream)
        : stream(stream)
    {
        this->prevState = this->stream.exceptions();
        this->stream.exceptions(std::ios::failbit | std::ios::badbit);
    }

    ~IOStream()
    {
        this->stream.exceptions(this->prevState);
    }

    template<class T>
    void write(const T& value)
    {
        static_assert(std::is_pod<T>::value, "only POD types are supported");
        this->stream.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

    void resetPack()
    {
        this->packBuffer.clear();
    }

    size_t getPackSize()
    {
        return this->packBuffer.size();
    }

    void flushPack()
    {
        this->stream.write(this->packBuffer.data(), this->packBuffer.size());
    }

    template<class T>
    void pack(const T& value)
    {
        Pack<T> pack;
        pack.pack(std::back_inserter(this->packBuffer), value);
    }

    template<class T>
    void pack(size_t offset, const T& value)
    {
        assert(offset < this->packBuffer.size());
        if (offset < this->packBuffer.size())
        {
            auto iter = this->packBuffer.begin();
            std::advance(iter, offset);
            Pack<T> pack;
            pack.pack(iter, value);
        }
    }


    void beginUnPack(size_t size)
    {
        this->packBuffer.resize(size);
        this->stream.read(this->packBuffer.data(), this->packBuffer.size());
        this->unPackIterator = this->packBuffer.begin();
    }

    size_t getUnPackSize()
    {
        return std::distance(this->packBuffer.begin(), this->unPackIterator);
    }

    template<class T>
    void unpack(T& value)
    {
        UnPack<T> unpack;
        this->unPackIterator = unpack.unpack(this->unPackIterator, value);
    }

    template<class T>
    void read(T& value)
    {
        static_assert(std::is_pod<T>::value, "only POD types are supported");
        stream.read(reinterpret_cast<char*>(&value), sizeof(T));
    }

    std::ios::pos_type tellp() const
    {
        return this->stream.tellp();
    }

    void seekp(std::ios::pos_type pos, std::ios::seekdir way)
    {
        this->stream.seekp(pos, way);
    }

    std::ios::pos_type tellg() const
    {
        return this->stream.tellg();
    }

    void seekg(std::ios::pos_type pos, std::ios::seekdir way)
    {
        this->stream.seekg(pos, way);
    }

private:
    IOStream(const IOStream&);
    IOStream& operator=(const IOStream&);
private:

    std::iostream& stream;
    std::ios::iostate prevState;

#ifdef WIN32
    #pragma warning( push )
    #pragma warning( disable : 4251 )
#endif
    
    std::vector<char> packBuffer;
    std::vector<char>::iterator unPackIterator;
    
#ifdef WIN32    
    #pragma warning( pop )
#endif    
};

}

#endif
