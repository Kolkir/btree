#ifndef _BTREE_NODE_H_
#define _BTREE_NODE_H_

#include <btree/importexport.h>
#include <btree/recordfile.h>

#include <iostream>
#include <map>
#include <algorithm>
#include <memory>

namespace btree
{

template<class Key>
class BTreeNode
{
public:

    BTreeNode(size_t maxKeysCount)
        : maxKeysCount(maxKeysCount)
    {};

    ~BTreeNode(){};

    bool canInsert() const
    {
        return this->index.size() < this->maxKeysCount;
    }

    void insert(Key key, const FileLocation& loc)
    {
        this->index.insert(std::make_pair(key, loc));
    }

    bool search(Key key, FileLocation& loc)
    {
        auto i = this->index.find(key);
        if (i != this->index.end())
        {
            loc = i->second;
            return true;
        }
        else
        {
            return false;
        }
    }

    Key largestKey() const
    {
        if (this->index.empty())
        {
            return Key();
        }
        else
        {
            return this->index.rbegin()->first;
        }
    }

    void setFileLocation(const FileLocation& loc)
    {
        this->fileLocation.reset( new FileLocation(loc));
    }

    const FileLocation* getFileLocation() const
    {
        return this->fileLocation.get();
    }

    template<class Key>
    friend void BTreeNodePack(const BTreeNode<Key>& node, IOStream& stream)
    {
        stream.pack(node.index.size());

        std::for_each(node.index.begin(), node.index.end(),
            [&](const MapIndex::value_type& value)
        {
            stream.pack(value.first);
            stream.pack(value.second);
        });
    }

    template<class Key>
    friend void BTreeNodeUnPack(BTreeNode<Key>& node, IOStream& stream)
    {
        size_t indexCount = 0;
        stream.unpack(indexCount);

        Key key;
        FileLocation loc;

        node.index.clear();
        
        for (size_t i = 0; i < indexCount; ++i)
        {
            stream.unpack(key);
            stream.unpack(loc);
            node.index.insert(std::make_pair(key, loc));
        }
    }

private:
    BTreeNode(const BTreeNode&);
    BTreeNode& operator= (const BTreeNode&);

private:
    size_t maxKeysCount;
    typedef std::map<Key, FileLocation> MapIndex;
    MapIndex index;
    std::unique_ptr<FileLocation> fileLocation;
};

}

#endif
