#include <btree/recordfile.h>
#include <cassert>

namespace btree
{

RecordFile::RecordFile(std::iostream& stream)
    : stream(stream)
{
    this->prevState = this->stream.exceptions();
    this->stream.exceptions(std::ios::failbit | std::ios::badbit);
}

RecordFile::~RecordFile()
{
    this->stream.exceptions(this->prevState);
}

std::pair<bool, RecordFile::RecAddr> RecordFile::append(const FileRecord& rec)
{
    bool res = false;
    std::ios::pos_type pos(0);
    try
    {
        this->stream.seekp(0, std::ios::end);
        pos = this->stream.tellp();

        auto len = rec.getSize();
        this->stream.write(reinterpret_cast<const char*>(&len), sizeof(len));//max length
        rec.write(this->stream);

        res = true;
    }
    catch (std::ios::failure&)
    {
        res = false;
    }
    return std::make_pair(res, pos);
}

bool RecordFile::write(RecAddr addr, const FileRecord& rec)
{
    try
    {
        this->stream.seekp(addr + sizeof(size_t), std::ios::beg);
        rec.write(this->stream);
        return true;
    }
    catch (std::ios::failure&)
    {
        return false;
    }
}

bool RecordFile::read(RecAddr addr, FileRecord& rec, size_t& maxLen)
{
    try
    {
        this->stream.seekg(addr, std::ios::beg);
        maxLen = 0;
        this->stream.read(reinterpret_cast<char*>(&maxLen), sizeof(maxLen));
        rec.read(this->stream);
        auto pos = this->stream.tellg();
        auto neededPos = static_cast<decltype(pos)>(addr + sizeof(maxLen) + maxLen);
        assert(pos <= neededPos);
    }
    catch (std::ios::failure&)
    {
        return false;
    }
    return true;
}

}

