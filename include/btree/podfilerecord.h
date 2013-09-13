#ifndef _POD_FILE_RECORD_H_
#define _POD_FILE_RECORD_H_

#include <btree/stream.h>

#include <type_traits>

namespace btree
{

template <class T, class Enable = void>
class PODFileRecord
{
    //Undefined
};

template <class T>
class PODFileRecord<T, typename std::enable_if<std::is_pod<T>::value >::type>
{
public:
    PODFileRecord(T& value) : value(value)
    {}

    void pack(IOStream& stream) const
    {
        stream.pack(value);
    }

    void unpack(IOStream& stream)
    {
        stream.unpack(value);
    }

private:
    PODFileRecord(const PODFileRecord&);
    PODFileRecord& operator=(const PODFileRecord&);

private:
    T& value;
};

}

#endif
