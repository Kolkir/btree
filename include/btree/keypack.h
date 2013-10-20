#ifndef _KEY_PACK_H_
#define _KEY_PACK_H_

#include <btree/stream.h>
#include <btree/exceptions.h>

#include <iterator>
#include <type_traits>
#include <vector>
#include <string>

namespace btree
{
    template <class T, class Enable = void>
    class KeyPack
    {
    };

    template <class T, class Enable = void>
    class KeyUnPack
    {
    };

    template <class T>
    class KeyPack<T, typename std::enable_if<std::is_pod<T>::value>::type>
    {
    public:
        void operator()(const T& value, IOStream& stream, size_t)
        {
            stream.pack(value);
        }
    };

    template <class T>
    class KeyUnPack<T, typename std::enable_if<std::is_pod<T>::value>::type>
    {
    public:
        void operator()(T& value, IOStream& stream, size_t)
        {
            stream.unpack(value);
        }
    };

    template<>
    class KeyPack<std::string, void>
    {
    public:
        void operator()(const std::string& value, IOStream& stream, size_t maxSize)
        {
            if (value.length() > maxSize)
            {
                throw InvalidKeyLength("Can't store a key");
            }
            stream.pack(value.length());
            std::string str(maxSize, ' ');
            std::copy(value.begin(), value.end(), str.begin());
            stream.pack(str);
        }
    };

    template <>
    class KeyUnPack<std::string, void>
    {
    public:
        void operator()(std::string& value, IOStream& stream, size_t maxSize)
        {
            size_t size = 0;
            stream.unpack(size);
            if (size > maxSize)
            {
                throw InvalidKeyLength("Can't load a key");
            }
            std::string str;
            stream.unpack(str);
            value = str;
            value.resize(size);
        }
    };

}

#endif

