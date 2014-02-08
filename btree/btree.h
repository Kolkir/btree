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

template <class KeyDef>
class BTree
{
public:
    typedef typename KeyDef::Key KeyType;
    typedef BTreeNode<KeyDef> Node;
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

        this->file->read(header.rootNodeLocation, *this->root, BTreeNodeUnPack<KeyDef>);
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

        auto loc = this->file->append(*this->root, BTreeNodePack<KeyDef>);
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

    void insert(const KeyType& key, const FileLocation& value)
    {
        auto thisNode = this->findLeafNode(key);
        assert(thisNode.get() != nullptr);

        bool newLargest = false;
        KeyType prevLargestKey(thisNode->largestKey());

        // test for special case of new largest key in tree
        if (KeyDef::Less()(thisNode->largestKey(), key))
        {
            newLargest = true; 
        }

        thisNode->insert(key, value);

        // handle special case of new largest key in tree
        if (newLargest)
        {
            this->updateLargestKey(prevLargestKey, key);
        }

        decltype(this->root) newNode;
        int level = this->height - 1;

        bool overflow = thisNode->isOverflow();
        while (overflow)
        {
            //remember the largest key
            KeyType largestKey(thisNode->largestKey());
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
        auto loc = this->file->append(*this->root, BTreeNodePack<KeyDef>); // put previous root into file

        KeyType maxSplitKey(thisNode->largestKey());
        // insert 2 keys in new root node
        this->root->clear();
        this->root->insert(maxSplitKey, loc);
        this->root->insert(newNode->largestKey(), *newNode->getFileLocation());

        ++this->height;
    }

    bool remove(const KeyType& key)
    {
        auto thisNode = this->findLeafNode(key);
        assert(thisNode.get() != nullptr);

        FileLocation loc;
        if (thisNode->search(key, loc))
        {
            KeyType prevLargestKey(thisNode->largestKey());

            NodePtr sibling;
            if (this->height > 1)
            {
                sibling = this->getSibling(prevLargestKey, this->height - 1);
            }
            thisNode->remove(key, loc);

            bool underflow = thisNode->isUnderflow();

            if (!underflow)
            {
                // if largest key in thisNode was changed update upper nodes
                KeyType newLargestKey(thisNode->largestKey());

                bool largestChanged = KeyDef::Less()(prevLargestKey, newLargestKey);

                if (largestChanged)
                {
                    this->updateLargestKey(prevLargestKey, newLargestKey);
                }

                this->store(*thisNode);
            }
            else
            {
                int level = this->height - 1;
                while (underflow && level > 0)
                {
                    assert(sibling);

                    KeyType siblingLargestKey(sibling->largestKey());

                    //redistribute
                    if (thisNode->takeFrom(*sibling))
                    {
                        // if largest key in thisNode was changed update upper nodes
                        KeyType newLargestKey(thisNode->largestKey());

                        bool largestChanged = KeyDef::Less()(prevLargestKey, newLargestKey);

                        if (largestChanged)
                        {
                            this->updateLargestKey(prevLargestKey, newLargestKey);
                        }

                        KeyType newSiblingLargestKey(sibling->largestKey());
                        
                        largestChanged = KeyDef::Less()(siblingLargestKey, newSiblingLargestKey);

                        if (largestChanged)
                        {
                            this->updateLargestKey(siblingLargestKey, newSiblingLargestKey);
                        }

                        // thisNode will be stored later
                        this->store(*sibling);
                        break; //we don't need propagte changes to parent nodes
                    }
                    else if (sibling->mergeWith(*thisNode))
                    {
                        KeyType newSiblingLargestKey(sibling->largestKey());

                        bool largestChanged = KeyDef::Less()(siblingLargestKey, newSiblingLargestKey);

                        if (largestChanged)
                        {
                            this->updateLargestKey(siblingLargestKey, newSiblingLargestKey);
                        }
                        this->store(*sibling);

                        --level; // go up to parent level
                        assert(level >= 0);
                        thisNode = this->nodes[level];

                        if (level > 0)
                        {
                            sibling = this->getSibling(thisNode->largestKey(), level);
                        }
                        else
                        {
                            sibling.reset();
                        }

                        FileLocation loc;
                        thisNode->remove(key, loc);
                        underflow = thisNode->isUnderflow();
                    }
                    else
                    {
                        assert(false);
                        break;
                    }
                }
                this->store(*thisNode);

                if (level > 0)
                {
                    return true;// remove complete
                }

                //if root have less than one child remove it
                if (root->size() == 1 &&
                    this->height > 1)
                {
                    root = this->fetch(root->begin()->second);
                    --this->height;
                }
            }
            return true;
        }
        return false;
    }

    bool get(const KeyType& key, FileLocation& loc)
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
        std::map<KeyType, KeyNodePtr> children;
    };

    //only for debug and testing
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
        auto loc = this->file->append(*newNode, BTreeNodePack<KeyDef>);
        newNode->setFileLocation(loc);
        return newNode;
    }

    NodePtr findLeafNode(const KeyType& key)
    {
        bool reloaded = false;
        // start from second level to handle empty first level situation
        for (size_t level = 1; level < this->height; ++level)
        {
            FileLocation recLocation;
            this->nodes.at(level - 1)->searchInexact(key, recLocation);

            if (!reloaded)
            {
                if (level >= this->nodes.size() || //  check node at this level loaded
                    !this->nodes.at(level)->equal(key, recLocation)) //prevent unnessary loading 
                {
                    reloaded = true;
                    this->nodes.resize(level); //remove loaded nodes from the other branch
                }
            }
            if (reloaded)
            {
                this->nodes.push_back(this->fetch(recLocation));
            }
        }
        return this->nodes.back();
    }

    NodePtr getSibling(const KeyType& key, int level)
    {
        assert(level >= 1);
        if (level >= 1 && 
            static_cast<size_t>(level) < this->nodes.size())
        {
            auto parent = this->nodes[level - 1];
            FileLocation loc;
            bool getSib = parent->getLeft(key, loc);
            if (!getSib)
            {
                getSib = parent->getRight(key, loc);
            }
            if (getSib)
            {
                return this->fetch(loc);
            }
            assert(false); // there is can't be nodes without siblings except root
        }
        return NodePtr();
    }

    NodePtr fetch(const FileLocation& loc)
    {
        NodePtr newNode(new Node(this->order));
        this->file->read(loc, *newNode, BTreeNodeUnPack<KeyDef>);
        newNode->setFileLocation(loc);
        return newNode;
    }

    void store(const Node& node)
    {
        assert(node.getFileLocation() != nullptr);
        auto newLoc = this->file->write(*node.getFileLocation(), node, BTreeNodePack<KeyDef>);
        if (newLoc != *node.getFileLocation())
        {
            throw IndexLocationChanged("Can't store a node");
        }
    }

    void updateLargestKey(const KeyType& oldKey, const KeyType& newKey)
    {
        for (size_t i = 0; i < this->height - 1; ++i)
        {
            this->nodes[i]->updateKey(oldKey, newKey);
            if (i > 0)
            {
                this->store(*this->nodes[i]);
            }
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
