#ifndef _POD_FILE_RECORD_H_
#define _POD_FILE_RECORD_H_

#include <btree/stream.h>

#include <type_traits>

namespace btree
{

template <class T>
void PackPODFileRecord(const T& value, IOStream& stream)
{
    stream.pack(value);
}

template <class T>
void UnPackPODFileRecord(T& value, IOStream& stream)
{
    stream.unpack(value);
}


}

#endif
