#include "treetestfix.h"

#include <btree/btreenode.h>
#include <btree/podfilerecord.h>
#include <gtest/gtest.h>

#include <fstream>
#include <string>

TEST_F(TreeTest, NodeTest)
{
    A a;
    a.x = 123;
    a.y = 56.78;
    auto loc1 = this->file->append(btree::PODFileRecord<A>(a));

    btree::RecordFile file(indexFile);
    btree::BTreeNode<std::string> node;
    node.insert("abc", loc1);

    auto loc2 = node.search("abc");
    ASSERT_EQ(loc1, loc2);
}