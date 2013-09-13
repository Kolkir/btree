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

template <class Key>
class BTree
{
public:
    BTree(size_t order)
        : order(order)
        , height(1)
    {
    }

    void open(std::iostream& stream)
    {
        this->nodes.clear();
        this->file.reset(new RecordFile(stream));
        this->root.reset(new Node());
        this->file->read_t(0, *this->root, maxLen);
        this->root.setMaxMemorySize(maxLen);
        this->nodes.push_back(root);
        this->calcHeight();
    }

    void create(std::iostream& stream)
    {
        this->nodes.clear();
        this->file.reset(new RecordFile(stream));
        this->root.reset(new Node());
    }

    void insert(Key key, RecAddr value)
    {
        auto leafNode = this->findLeafNode(key);
    }

    bool get(Key key, RecAddr& value) const
    {
        return false;
    }

private:
    BTree(const BTree&);
    BTree& operator= (const BTree&);


private:
    typedef BTreeNode<Key> Node;
    typedef std::shared_ptr<Node> NodePtr;

    NodePtr findLeafNode(Key key) const; 

private:
    size_t order;
    size_t height;
    NodePtr root;
    std::vector<NodePtr> nodes;
    std::unique_ptr<RecordFile> file;
};

//-----------------------------------------------------------------------------

template <class Key>
typename BTree<Key>::NodePtr BTree<Key>::findLeafNode(Key key) const
{
    return nullptr;
}

}

#endif
