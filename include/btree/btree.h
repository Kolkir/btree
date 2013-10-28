#ifndef _BTREE_H_
#define _BTREE_H_

#include <btree/btreenode.h>
#include <btree/recordfile.h>
#include <btree/keypack.h>

#include <iostream>
#include <memory>
#include <vector>

namespace btree
{

namespace detail
{
    struct BTreeFileHeader
    {
        BTreeFileHeader() : height(0){}
        size_t height;
        FileLocation rootNodeLocation;
    };

    inline void PackBTreeFileHeader(const BTreeFileHeader& header, IOStream& stream)
    {
        stream.pack(header.height);
        stream.pack(header.rootNodeLocation);
    }

    inline void UnPackBTreeFileHeader(BTreeFileHeader& header, IOStream& stream)
    {
        stream.unpack(header.height);
        stream.unpack(header.rootNodeLocation);
    }

}

template <class Key,
          size_t maxKeySize = sizeof(Key),
          class KeyPackFunc = KeyPack<Key>,
          class KeyUnPackFunc = KeyUnPack<Key>>
class BTree
{
public:
    typedef BTreeNode<Key> Node;
    typedef std::shared_ptr<Node> NodePtr;

    BTree(size_t order)
        : order(order)
        , height(1)
    {
    }

    ~BTree()
    {
        this->save();
    }

    void open(std::iostream& stream)
    {
        this->nodes.clear();
        this->file.reset(new RecordFile(stream));
        this->root.reset(new Node(this->order));

        detail::BTreeFileHeader header;
        this->headerLocation = this->file->read(header, detail::UnPackBTreeFileHeader);
        this->height = header.height;
        assert(this->height != 0);

        this->file->read(header.rootNodeLocation, *this->root, BTreeNodeUnPack<Key, maxKeySize, KeyUnPackFunc>);
        this->root->setFileLocation(header.rootNodeLocation);
        this->nodes.push_back(root);
    }

    void create(std::iostream& stream)
    {
        this->nodes.clear();
        this->file.reset(new RecordFile(stream));
        this->root.reset(new Node(this->order));

        detail::BTreeFileHeader header;
        this->headerLocation = this->file->append(header, detail::PackBTreeFileHeader);

        auto loc = this->file->append(*this->root, BTreeNodePack<Key, maxKeySize, KeyPackFunc>);
        this->root->setFileLocation(loc);
        this->height = 1;
        this->nodes.push_back(this->root);
    }

    void save()
    {
        detail::BTreeFileHeader header;
        header.height = this->height;
        header.rootNodeLocation = *this->root->getFileLocation();
        auto loc = this->file->write(this->headerLocation, header, detail::PackBTreeFileHeader);
        assert(loc == this->headerLocation);
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

        thisNode->insert(key, value);
        bool overflow = thisNode->isOverflow();

        // handle special case of new largest key in tree
        if (newLargest)
        {
            for (size_t i = 0; i < this->height - 1; ++i) 
            {
                this->nodes[i]->updateKey(prevKey, key);
                if (i > 0)
                {
                    this->store(*this->nodes[i]);
                }
            }
        }

        decltype(this->root) newNode;
        Key largestKey;
        int level = this->height - 1; 
        while (overflow)
        {
            //remember the largest key
            largestKey = thisNode->largestKey();
            // split the node
            newNode = this->newNode();
            thisNode->split(*newNode);
            this->store(*thisNode); 
            this->store(*newNode);
            --level; // go up to parent level
            if (level < 0)
            {
                break;
            }
            // insert newNode into parent of thisNode
            auto parentNode = this->nodes[level];
            parentNode->updateKey(largestKey, thisNode->largestKey());

            parentNode->insert(newNode->largestKey(), *newNode->getFileLocation());
            overflow = parentNode->isOverflow();

            thisNode = parentNode;
        }
        this->store(*thisNode);
        
        if (level >= 0)
        {
            return;// insert complete
        }

        // else we just split the root
        auto loc = this->file->append(*this->root, BTreeNodePack<Key, maxKeySize, KeyPackFunc>); // put previous root into file

        Key maxSplitKey = thisNode->largestKey();
        // insert 2 keys in new root node
        this->root->clear();
        this->root->insert(maxSplitKey, loc);
        this->root->insert(newNode->largestKey(), *newNode->getFileLocation());

        ++this->height;
    }

    bool get(Key key, FileLocation& loc)
    {
        NodePtr node = this->findLeafNode(key);
        assert(node.get() != nullptr);
        return node->search(key, loc);
    }

    size_t getHeight() const
    {
        return this->height;
    }

    struct KeyNode;
    typedef std::shared_ptr<KeyNode> KeyNodePtr;
    struct KeyNode
    {
        NodePtr node;
        std::map<Key, KeyNodePtr> children;
    };

    KeyNodePtr getTreeStructure()
    {
        using namespace std;

        std::vector<KeyNodePtr> treeLevel;
        std::vector<KeyNodePtr> nextLevel;

        KeyNodePtr rootNode(new KeyNode);
        rootNode->node = this->root;
        treeLevel.push_back(rootNode);

        size_t level = 0;

        while (!treeLevel.empty())
        {
            nextLevel.clear();

            std::for_each(treeLevel.begin(), treeLevel.end(),
                [&](KeyNodePtr curKeyNode)
            {
                if (curKeyNode->node)
                {
                    for_each(curKeyNode->node->begin(), curKeyNode->node->end(),
                        [&](const Node::MapIndex::value_type& val)
                    {
                        KeyNodePtr newNode(new KeyNode());
                        if (level < (this->height - 1))
                        {
                            newNode->node = this->fetch(val.second);
                        }
                        curKeyNode->children.insert(std::make_pair(val.first, newNode));
                        nextLevel.push_back(newNode);
                    });
                }
            });

            std::swap(treeLevel, nextLevel);
            ++level;
        }

        return rootNode;
    }

private:
    BTree(const BTree&);
    BTree& operator= (const BTree&);


private:

    NodePtr newNode() const
    {
        NodePtr newNode(new Node(this->order));
        auto loc = this->file->append(*newNode, BTreeNodePack<Key, maxKeySize, KeyPackFunc>);
        newNode->setFileLocation(loc);
        return newNode;
    }

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
    FileLocation headerLocation;
};

}

#endif
