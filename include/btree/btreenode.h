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

    //needed for reading
    BTreeNode()
        : maxKeysCount(0)
    {};

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

    void updateKey(Key oldKey, Key newKey)
    {
        FileLocation loc;
        if (this->search(oldKey, loc))
        {
            this->remove(oldKey, loc);
            this->insert(newKey, loc);
        }
    }

    void remove(Key key, FileLocation& loc)
    {
        auto i = this->index.find(key);
        if (i != this->index.end())
        {
            loc = i->second;
            this->index.erase(i);
        }
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

    template<class TKey>
    friend void BTreeNodePack(const BTreeNode<TKey>& node, IOStream& stream);
    

    template<class TKey>
    friend void BTreeNodeUnPack(BTreeNode<TKey>& node, IOStream& stream);
    

private:
    BTreeNode(const BTreeNode&);
    BTreeNode& operator= (const BTreeNode&);

private:
    size_t maxKeysCount;
    typedef std::map<Key, FileLocation> MapIndex;
    MapIndex index;
    std::unique_ptr<FileLocation> fileLocation;
};

template<class TKey>
inline void BTreeNodePack(const BTreeNode<TKey>& node, IOStream& stream)
{
    stream.pack(node.maxKeysCount);

    stream.pack(node.index.size());

    std::for_each(node.index.begin(), node.index.end(),
        [&](const typename BTreeNode<TKey>::MapIndex::value_type& value)
    {
        stream.pack(value.first);
        stream.pack(value.second);
    });
}

template<class TKey>
inline void BTreeNodeUnPack(BTreeNode<TKey>& node, IOStream& stream)
{
    stream.unpack(node.maxKeysCount);

    size_t indexCount = 0;
    stream.unpack(indexCount);

    TKey key;
    FileLocation loc;

    node.index.clear();
    
    for (size_t i = 0; i < indexCount; ++i)
    {
        stream.unpack(key);
        stream.unpack(loc);
        node.index.insert(std::make_pair(key, loc));
    }
}


}

#endif
