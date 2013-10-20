#include <btree/btree.h>
#include <btree/podfilerecord.h>

#include "treetestfix.h"


TEST_F(TreeTest, BTreeTest)
{
    A a;
    a.x = 123;
    a.y = 56.78;
    auto loc1 = this->file->append(a, btree::PackPODFileRecord<A>);

    btree::BTree<std::string, 100> tree(bTreeOrder);
    tree.create(indexFile);
    tree.insert("Rec1", loc1);
    auto val = tree.get("Rec1");
    ASSERT_TRUE(val != nullptr);
    ASSERT_EQ(loc1, *val);
};

