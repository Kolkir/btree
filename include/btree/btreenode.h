#ifndef _BTREE_NODE_H_
#define _BTREE_NODE_H_

#include <btree/recordfile.h>

#include <iostream>
#include <map>
#include <algorithm>
#include <memory>

namespace btree
{

template<class KeyDef>
class BTreeNode
{
public:
    typedef typename KeyDef::Key KeyType;
    typedef std::map<typename KeyDef::Key, FileLocation, typename KeyDef::Less> MapIndex;

    //needed for reading
    BTreeNode()
        : maxKeysCount(0)
        , minKeysCount(0)
    {};

    BTreeNode(size_t maxKeysCount)
        : maxKeysCount(maxKeysCount + 1)
        , minKeysCount(maxKeysCount / 2)
    {};

    ~BTreeNode(){};

    void clear()
    {
        this->index.clear();
    }

    bool isOverflow() const
    {
        return this->index.size() >= this->maxKeysCount;
    }

    bool hasMinimumKeyCount() const
    {
        return this->index.size() <= this->minKeysCount;
    }

    void insert(const KeyType& key, const FileLocation& loc)
    {
        this->index.insert(std::make_pair(key, loc));
    }
    
    bool equal(const KeyType& key, const FileLocation& loc)
    {
        if (this->fileLocation && !this->index.empty())
        {
            const auto& lKey = this->index.rbegin()->first;
            return (!KeyDef::Less()(lKey , key) &&
                    !KeyDef::Less()(key , lKey) &&
                    loc == *this->fileLocation);
        }
        return false;
    }
    
    void split(BTreeNode& newNode)
    {
        auto midpt = (this->index.size() + 1) / 2;
        
        auto i = this->index.begin();
        std::advance(i, midpt);
        auto s = i;
        auto e = this->index.end();

        for (; i != e; ++i)
        {
            newNode.insert(i->first, i->second);
        }
        this->index.erase(s, e);
    }

    void updateKey(const KeyType& oldKey, const KeyType& newKey)
    {
        FileLocation loc;
        if (this->search(oldKey, loc))
        {
            this->remove(oldKey, loc);
            this->insert(newKey, loc);
        }
    }

    void remove(const KeyType& key, FileLocation& loc)
    {
        auto i = this->index.find(key);
        if (i != this->index.end())
        {
            loc = i->second;
            this->index.erase(i);
        }
    }

    bool search(const KeyType& key, FileLocation& loc)
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

    void searchInexact(const KeyType& key, FileLocation& loc)
    {
        assert(!this->index.empty());
        auto i = this->index.upper_bound(key);
        //check exact match
        if (i != this->index.begin() && std::prev(i)->first == key)
        {
            loc = std::prev(i)->second;
        }
        //check inexact
        else if (i != this->index.end())
        {
            loc = i->second;
        }
        else
        {
            loc = this->index.rbegin()->second;
        }
    }

    KeyType largestKey() const
    {
        if (this->index.empty())
        {
            return KeyType();
        }
        else
        {
            return this->index.rbegin()->first;
        }
    }

    void setFileLocation(const FileLocation& loc)
    {
        this->fileLocation.reset(new FileLocation(loc));
    }

    const FileLocation* getFileLocation() const
    {
        return this->fileLocation.get();
    }

    typename MapIndex::const_iterator begin() const
    {
        return this->index.begin();
    }

    typename MapIndex::const_iterator end() const
    {
        return this->index.end();
    }

    template <class TKeyDef>
    friend void BTreeNodePack(const BTreeNode<TKeyDef>& node, IOStream& stream);
    

    template<class TKeyDef>
    friend void BTreeNodeUnPack(BTreeNode<TKeyDef>& node, IOStream& stream);
    

private:
    BTreeNode(const BTreeNode&);
    BTreeNode& operator= (const BTreeNode&);

private:
    size_t maxKeysCount;
    size_t minKeysCount;
    MapIndex index;
    std::unique_ptr<FileLocation> fileLocation;
    KeyDef keyDef;
};

template<class TKeyDef>
inline void BTreeNodePack(const BTreeNode<TKeyDef>& node, IOStream& stream)
{
    stream.pack(node.maxKeysCount);

    stream.pack(node.index.size());

    std::for_each(node.index.begin(), node.index.end(),
        [&](const typename BTreeNode<TKeyDef>::MapIndex::value_type& value)
    {
        node.keyDef.pack(value.first, stream);
        stream.pack(value.second);
    });

    //store all posible set to reserve location
    auto leftCount = node.maxKeysCount - node.index.size();
    typename TKeyDef::Key key;
    FileLocation loc;

    for (size_t i = 0; i < leftCount; ++i)
    {
        node.keyDef.pack(key, stream);
        stream.pack(loc);
    }
}

template<class TKeyDef>
inline void BTreeNodeUnPack(BTreeNode<TKeyDef>& node, IOStream& stream)
{
    stream.unpack(node.maxKeysCount);

    size_t indexCount = 0;
    stream.unpack(indexCount);

    typename TKeyDef::Key key;
    FileLocation loc;

    node.index.clear();
    
    for (size_t i = 0; i < indexCount; ++i)
    {
        node.keyDef.unpack(key, stream);
        stream.unpack(loc);
        node.index.insert(std::make_pair(key, loc));
    }
}


}

#endif
