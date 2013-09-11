#ifndef _RECORD_FILE_H_
#define _RECORD_FILE_H_

#include <btree/importexport.h>

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

class BTREE_API RecordFile
{
public:
    typedef unsigned long long RecAddr;

    RecordFile(std::iostream& stream);

    ~RecordFile();

    std::pair<bool, RecAddr> append(const FileRecord& rec);

    bool write(RecAddr addr, const FileRecord& rec);

    bool read(RecAddr addr, FileRecord& rec);

private:
    RecordFile(const RecordFile&);
    RecordFile& operator= (const RecordFile&);

private:
    std::iostream& stream;
    std::ios::iostate prevState;
};

}

#endif
