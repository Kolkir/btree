#ifndef _BTREE_NODE_H_
#define _BTREE_NODE_H_

#include <btree/importexport.h>
#include <btree/recordfile.h>

#include <iostream>
#include <map>
#include <algorithm>

namespace btree
{

template<class Key>
class BTreeNode
{
public:
    BTreeNode(){};
    ~BTreeNode(){};

    void insert(Key key, const FileLocation& loc)
    {
        this->index.insert(std::make_pair(key, loc));
    }

    FileLocation search(Key key)
    {
        return this->index.at(key);
    }

    void setFileLocation(const FileLocation& loc)
    {
    }

    void pack(IOStream& stream) const
    {
        std::for_each(this->index.begin(), this->index.end(),
            [&](const MapIndex::value_type& value)
        {
        });
    }

    void unpack(IOStream& stream)
    {
    }

private:
    typedef std::map<Key, FileLocation> MapIndex;
    MapIndex index;
};

}

#endif
