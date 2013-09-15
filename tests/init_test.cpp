#include <btree/btree.h>
#include <btree/podfilerecord.h>
#include "treetestfix.h"


TEST_F(TreeTest, BTreeTest)
{
    A a;
    a.x = 123;
    a.y = 56.78;
    auto loc1 = this->file->append(btree::PODFileRecord<A>(a));

    btree::BTree<std::string> tree(bTreeOrder);
    tree.create(indexFile);
    tree.insert("Rec1", loc1);
    auto val = tree.get("Hello");
    ASSERT_TRUE(val != nullptr);
    ASSERT_EQ(loc1, *val);
};


