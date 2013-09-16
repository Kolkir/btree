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
        this->height = 1;
        this->nodes.push_back(this->root);
    }

    void insert(Key key, const FileLocation& value)
    {
        auto leafNode = this->findLeafNode(key);
    }

    FileLocation* get(Key key) const
    {
        return nullptr;
    }

private:
    BTree(const BTree&);
    BTree& operator= (const BTree&);


private:
    typedef BTreeNode<Key> Node;
    typedef std::shared_ptr<Node> NodePtr;

    NodePtr findLeafNode(Key key); 
    NodePtr fetch(const FileLocation& addr);

private:
    size_t order;
    size_t height;
    NodePtr root;
    std::vector<NodePtr> nodes;
    std::unique_ptr<RecordFile> file;
};

//-----------------------------------------------------------------------------

template <class Key>
typename BTree<Key>::NodePtr BTree<Key>::findLeafNode(Key key)
{
    size_t level = 0;
    for (; level < this->height; ++level)
    {
        auto recLocation = this->nodes[level]->search(key);
        this->nodes[level] = this->fetch(recLocation);
    }
    return this->nodes[level];
}

template <class Key>
typename BTree<Key>::NodePtr BTree<Key>::fetch(const FileLocation& loc)
{
    NodePtr newNode(new Node());
    this->file->read(loc, *newNode, BTreeNodeUnPack<Key>);
    newNode->setFileLocation(loc);
    return newNode;
}


}

#endif
