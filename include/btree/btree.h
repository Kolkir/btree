#ifndef _BTREE_H_
#define _BTREE_H_

#include <btree/importexport.h>
#include <btree/btreenode.h>
#include <btree/recordfile.h>

#include <iostream>
#include <memory>
#include <vector>

namespace btree
{

template <class Key, class Value>
class BTree
{
public:
    BTree(size_t order, std::iostream& stream)
        : height(1)
        , file(stream)
    {
    }

    void insert(Key key, Value value)
    {
        auto leafNode = this->findLeafNode(key);
    }

    bool get(Key key, Value& value) const
    {
        return false;
    }

private:
    BTree(const BTree&);
    BTree& operator= (const BTree&);


private:
    typedef BTreeNode<Key, Value> Node;
    typedef std::shared_ptr<Node> NodePtr;

    NodePtr findLeafNode(Key key) const; 

private:
    size_t height;
    Node root;
    std::vector<NodePtr> nodes;
    RecordFile file;
};

//-----------------------------------------------------------------------------

template <class Key, class Value>
typename BTree<Key, Value>::NodePtr BTree<Key, Value>::findLeafNode(Key key) const
{
    return nullptr;
}

void BTREE_API somefunc();
}

#endif
