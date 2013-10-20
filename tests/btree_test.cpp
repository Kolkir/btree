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

