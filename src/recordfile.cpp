#include <btree/recordfile.h>
#include <cassert>

namespace btree
{

RecordFile::RecordFile(std::iostream& stream)
    : stream(stream)
{
}

RecordFile::~RecordFile()
{
}

}

