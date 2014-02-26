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

    template<class T, class Enable = void>
    class PodKey final
    {
    };

    template <class T>
    class PodKey <T, typename std::enable_if<std::is_pod<T>::value>::type> final
    {
    public:
        typedef T Key;
        typedef std::less<Key> Less;

        void pack(const Key& value, IOStream& stream) const
        {
            stream.pack(value);
        }

        void unpack(Key& value, IOStream& stream) const
        {
            stream.unpack(value);
        }
    };


    template<size_t Len, class Elem, class Traits, class Allocator>
    class StringKey final
    {
    public:
        typedef std::basic_string<Elem, Traits, Allocator> Key;
        typedef std::less<Key> Less;

        void pack(const Key& value, IOStream& stream) const
        {
            if (value.length() > Len)
            {
                throw InvalidKeyLength("Can't store a key");
            }
            stream.pack(value.length());
            Key str(Len, typename Key::value_type(0));
            std::copy(value.begin(), value.end(), str.begin());
            stream.pack(str);
        }

        void unpack(Key& value, IOStream& stream) const
        {
            size_t size = 0;
            stream.unpack(size);
            if (size > Len)
            {
                throw InvalidKeyLength("Can't load a key");
            }
            Key str;
            stream.unpack(str);
            value = str;
            value.resize(size);
        }
    };

    template <size_t Len> using AStringKey = StringKey<Len, char, std::char_traits<char>, std::allocator<char>>;
    template <size_t Len> using WStringKey = StringKey<Len, wchar_t, std::char_traits<wchar_t>, std::allocator<wchar_t>>;
}

#endif

