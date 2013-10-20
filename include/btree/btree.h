#ifndef _BTREE_H_
#define _BTREE_H_

#include <btree/importexport.h>
#include <btree/btreenode.h>
#include <btree/recordfile.h>
#include <btree/keypack.h>

#include <iostream>
#include <memory>
#include <vector>

namespace btree
{

template <class Key,
          size_t maxKeySize = sizeof(Key),
          class KeyPackFunc = KeyPack<Key>,
          class KeyUnPackFunc = KeyUnPack<Key>>
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
        this->root.reset(new Node(this->order));
        FileLocation firstLocation;
        this->file->read(firstLocation, *this->root, BTreeNodeUnPack<Key, maxKeySize, KeyUnPackFunc>);
        this->root->setFileLocation(firstLocation);
        this->nodes.push_back(root);
        this->calcHeight();
    }

    void create(std::iostream& stream)
    {
        this->nodes.clear();
        this->file.reset(new RecordFile(stream));
        this->root.reset(new Node(this->order));
        auto loc = this->file->append(*this->root, BTreeNodePack<Key, maxKeySize, KeyPackFunc>);
        this->root->setFileLocation(loc);
        this->height = 1;
        this->nodes.push_back(this->root);
    }

    void insert(Key key, const FileLocation& value)
    {
        auto thisNode = this->findLeafNode(key);
        
        bool newLargest = false;
        Key prevKey;

        // test for special case of new largest key in tree
        if (key > thisNode->largestKey())
        {
            newLargest = true; 
            prevKey = thisNode->largestKey();
        }

        bool overflow = !thisNode->canInsert();

        if (!overflow)
        {
            thisNode->insert(key, value);
        }
        
        // handle special case of new largest key in tree
        if (newLargest)
        {
            for (size_t i = 0; i < this->height; ++i) 
            {
                this->nodes[i]->updateKey(prevKey, key);
                if (i > 0)
                {
                    this->store(*this->nodes[i]);
                }
            }
        }

        int level = this->height - 1; 
        while (overflow)
        {
  /*          //remember the largest key
            largestKey=thisNode->LargestKey();
            // split the node
            newNode = NewNode();
            thisNode->Split(newNode);
            Store(thisNode); Store(newNode);
            level--; // go up to parent level
            if (level < 0) break;
            // insert newNode into parent of thisNode
            parentNode = Nodes[level];
            result = parentNode->UpdateKey
            (largestKey,thisNode->LargestKey());
            result = parentNode->Insert
            (newNode->LargestKey(),newNode->RecAddr);
            thisNode=parentNode;*/
        }
        this->store(*thisNode);
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

    NodePtr findLeafNode(Key key)
    {
        this->nodes.clear();
        this->nodes.push_back(this->root);

        // start from second level to handle empty first level situation
        for (size_t level = 1; level < this->height; ++level)
        {
            FileLocation recLocation;
            this->nodes.back()->searchInexact(key, recLocation);
            this->nodes.push_back(this->fetch(recLocation));
        }
        return this->nodes.back();
    }

    NodePtr fetch(const FileLocation& loc)
    {
        NodePtr newNode(new Node(this->order));
        this->file->read(loc, *newNode, BTreeNodeUnPack<Key, maxKeySize, KeyUnPackFunc>);
        newNode->setFileLocation(loc);
        return newNode;
    }

    void store(const BTreeNode<Key>& node)
    {
        assert(node.getFileLocation() != nullptr);
        auto newLoc = this->file->write(*node.getFileLocation(), node, BTreeNodePack<Key, maxKeySize, KeyPackFunc>);
        if (newLoc != *node.getFileLocation())
        {
            throw IndexLocationChanged("Can't store a node");
        }
    }

private:
    size_t order;
    size_t height;
    NodePtr root;
    std::vector<NodePtr> nodes;
    std::unique_ptr<RecordFile> file;
};

}

#endif
