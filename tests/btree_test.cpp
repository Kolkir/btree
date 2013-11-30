#include <btree/btree.h>
#include <btree/podfilerecord.h>

#include "treetestfix.h"


TEST_F(TreeTest, InsertGet)
{
    A a;
    a.x = 123;
    a.y = 56.78;
    auto loc1 = this->file->append(a, btree::PackPODFileRecord<A>);

    btree::BTree<btree::AStringKey<100>> tree(bTreeOrder);
    tree.create(indexFile);
    tree.insert("Rec1", loc1);

    btree::FileLocation loc2;
    auto val = tree.get("Rec1", loc2);
    ASSERT_TRUE(val);
    ASSERT_EQ(loc1, loc2);
    
    tree.remove("Rec1");
};


TEST_F(TreeTest, CreateStructure)
{
    std::string keys = "CSDTAMPIBWNGURKEHOLJYQZFXV";
    int index = 0;

    auto i = keys.begin();
    auto e = keys.end();
    std::vector<std::pair<char, btree::FileLocation>> locations;

    for (; i != e; ++i, ++index)
    {
        auto loc = this->file->append(index, btree::PackPODFileRecord<int>);
        locations.push_back(std::make_pair(*i, loc));
    }

    {
        btree::BTree<btree::PodKey<char>> tree(bTreeOrder);
        tree.create(indexFile);

        auto i = locations.begin();
        auto e = locations.end();
        for (; i != e; ++i)
        {
            tree.insert(i->first, i->second);
        }
    }

    btree::BTree<btree::PodKey<char>> tree(bTreeOrder);
    tree.open(indexFile);

    ASSERT_EQ(3, tree.getHeight());

    auto rootNode = tree.getTreeStructure();

    ASSERT_EQ(3, rootNode->children.size());
    auto nodeI = rootNode->children.find('I');
    ASSERT_TRUE(nodeI != rootNode->children.end());
    auto nodeP = rootNode->children.find('P');
    ASSERT_TRUE(nodeP != rootNode->children.end());
    auto nodeZ = rootNode->children.find('Z');
    ASSERT_TRUE(nodeZ != rootNode->children.end());

    ASSERT_EQ(3, nodeI->second->children.size());
    auto nodeD = nodeI->second->children.find('D');
    ASSERT_TRUE(nodeD != nodeI->second->children.end());
    auto nodeG = nodeI->second->children.find('G');
    ASSERT_TRUE(nodeG != nodeI->second->children.end());
    auto nodeII = nodeI->second->children.find('I');
    ASSERT_TRUE(nodeII != nodeI->second->children.end());

    ASSERT_EQ(4, nodeD->second->children.size());
    auto nodeA = nodeD->second->children.find('A');
    ASSERT_TRUE(nodeA != nodeD->second->children.end());
    auto nodeB = nodeD->second->children.find('B');
    ASSERT_TRUE(nodeB!= nodeD->second->children.end());
    auto nodeC = nodeD->second->children.find('C');
    ASSERT_TRUE(nodeC != nodeD->second->children.end());
    auto nodeDD = nodeD->second->children.find('D');
    ASSERT_TRUE(nodeDD != nodeD->second->children.end());

    index = 0;

    auto li = locations.begin();
    auto le = locations.end();

    btree::FileLocation loc;
    int idx = 0;
    for (; li != le; ++li)
    {
        ASSERT_TRUE(tree.get(li->first, loc));
        this->file->read(loc, idx, btree::UnPackPODFileRecord<int>);
        ASSERT_EQ(index, idx);
        ++index;
    }

    tree.remove('A');
};
