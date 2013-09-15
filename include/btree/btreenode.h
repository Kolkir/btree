#ifndef _BTREE_NODE_H_
#define _BTREE_NODE_H_

#include <btree/importexport.h>
#include <btree/recordfile.h>

#include <iostream>
#include <map>

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
    }

    FileLocation search(Key key)
    {
        return this->index.at(key);
    }

    void setFileLocation(const FileLocation& loc)
    {
    }

    void pack(IOStream& stream)
    {
    }

    void unpack(IOStream& stream)
    {
    }

private:
    std::map<Key, FileLocation> index;
};

}

#endif
