#include <btree/btree.h>
#include <btree/podfilerecord.h>

#include "treetestfix.h"


TEST_F(TreeTest, InsertGet)
{
    A a;
    a.x = 123;
    a.y = 56.78;
    auto loc1 = this->file->append(a, btree::PackPODFileRecord<A>);

    btree::BTree<std::string, 100> tree(bTreeOrder);
    tree.create(indexFile);
    tree.insert("Rec1", loc1);

    btree::FileLocation loc2;
    auto val = tree.get("Rec1", loc2);
    ASSERT_TRUE(val);
    ASSERT_EQ(loc1, loc2);
};


TEST_F(TreeTest, CreateStructure)
{
    {
        btree::BTree<char> tree(bTreeOrder);
        tree.create(indexFile);

        std::string keys = "CSDTAMPIBWNGURKEHOLJYQZFXV";
        int index = 0;

        auto i = keys.begin();
        auto e = keys.end();
        for (; i != e; ++i, ++index)
        {
            auto loc = this->file->append(index, btree::PackPODFileRecord<int>);
            tree.insert(*i, loc);
        }
    }

    btree::BTree<char> tree(bTreeOrder);
    tree.open(indexFile);

    ASSERT_EQ(3, tree.getHeight());

    //ASSERT_EQ(1, tree.nodesCount(0));

    //std::vector<char> keys(bTreeOrder);
    //tree.getKeys(
};


