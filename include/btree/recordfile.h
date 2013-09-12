#ifndef _RECORD_FILE_H_
#define _RECORD_FILE_H_

#include <btree/importexport.h>

#include <type_traits>
#include <iostream>

namespace btree
{

class BTREE_API FileRecord
{
public:
    ~FileRecord(){};
    virtual size_t getSize() const = 0;
    virtual void write(std::ostream& stream) const = 0;
    virtual void read(std::istream& stream) = 0;
};

template <class T, class Enable = void>
class PODFileRecord: public FileRecord
{
    //Undefined
};

template <class T>
class PODFileRecord<T, typename std::enable_if<std::is_pod<T>::value >::type>: public FileRecord
{
public:
    PODFileRecord(T& value) : value(value)
    {}
    virtual size_t getSize() const 
    {
        return sizeof(T);
    }
    virtual void write(std::ostream& stream) const
    {
        stream.write(reinterpret_cast<const char*>(&value), getSize());
    }
    virtual void read(std::istream& stream)
    {
        T a;
        stream.read(reinterpret_cast<char*>(&a), getSize());
        this->value = a;
    }
private:
    PODFileRecord(const PODFileRecord&);
    PODFileRecord& operator=(const PODFileRecord&);
private:
    T& value;
};

class BTREE_API RecordFile
{
public:
    typedef unsigned long long RecAddr;

    RecordFile(std::iostream& stream);

    ~RecordFile();

    std::pair<bool, RecAddr> append(const FileRecord& rec);

    bool write(RecAddr addr, const FileRecord& rec);

    bool read(RecAddr addr, FileRecord& rec, size_t& maxLen);

private:
    RecordFile(const RecordFile&);
    RecordFile& operator= (const RecordFile&);

private:
    std::iostream& stream;
    std::ios::iostate prevState;
};

}

#endif
