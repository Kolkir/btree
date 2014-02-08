#ifndef _DEF_PACK_H_
#define _DEF_PACK_H_

#include <btree/filelocation.h>
#include <btree/stream.h>

#include <iterator>
#include <type_traits>
#include <vector>
#include <string>

namespace btree
{

template <class T, class Enable = void>
class Pack
{
};

template <class T, class Enable = void>
class UnPack
{
};

template <class T>
class Pack<T, typename std::enable_if<std::is_pod<T>::value>::type>
{
public:
    template <class Iter>
    void operator()(Iter iter, const T& value)
    {
        std::copy(reinterpret_cast<const char*>(&value), 
                  reinterpret_cast<const char*>(&value) + sizeof(T),
                  iter);
    }
};

template <class T>
class UnPack<T, typename std::enable_if<std::is_pod<T>::value>::type>
{
public:
    template <class Iter>
    Iter operator()(Iter start, T& value)
    {
        Iter end = start;
        std::advance(end, sizeof(T));
        std::copy(start, 
                  end,
                  reinterpret_cast<char*>(&value));

        return end;
    }
};

template <>
class Pack<std::string, void>
{
public:
    template <class Iter>
    void operator()(Iter iter, const std::string& value)
    {
        auto len = value.length();
        std::copy(reinterpret_cast<const char*>(&len), 
                  reinterpret_cast<const char*>(&len) + sizeof(len),
                  iter);
        std::copy(reinterpret_cast<const char*>(value.data()), 
                  reinterpret_cast<const char*>(value.data()) + len,
                  iter);
    }
};

template <>
class UnPack<std::string, void>
{
public:
    template <class Iter>
    Iter operator()(Iter start, std::string& value)
    {
        auto len = value.length();

        auto end = start;
        std::advance(end, sizeof(len));

        std::copy(start, 
                  end,
                  reinterpret_cast<char*>(&len));

        start = end;
        std::advance(end, len);
        std::vector<char> buf(len);

        std::copy(start, end, buf.data());
        value.assign(buf.begin(), buf.end());

        return end;
    }
};

template <>
class Pack<FileLocation, void>
{
public:
    template <class Iter>
    void operator()(Iter iter, const FileLocation& value)
    {
        auto addr = value.getAddr();
        std::copy(reinterpret_cast<const char*>(&addr), 
                  reinterpret_cast<const char*>(&addr) + sizeof(addr),
                  iter);
        
        auto size = value.getMaxSize();
        std::copy(reinterpret_cast<const char*>(&size), 
                  reinterpret_cast<const char*>(&size) + sizeof(size),
                  iter);
    }
};

template <>
class UnPack<FileLocation, void>
{
public:
    template <class Iter>
    Iter operator()(Iter start, FileLocation& value)
    {
        auto addr = value.getAddr();

        auto end = start;
        std::advance(end, sizeof(addr));
        
        std::copy(start, 
                  end,
                  reinterpret_cast<char*>(&addr));

        value.setAddr(addr);

        auto maxSize = value.getMaxSize();

        start = end;
        std::advance(end, sizeof(maxSize));

        std::copy(start, 
                  end,
                  reinterpret_cast<char*>(&maxSize));

        value.setMaxSize(maxSize);
        return end;
    }
};

}

#endif

