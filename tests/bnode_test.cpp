#include "treetestfix.h"

#include <btree/btreenode.h>
#include <btree/podfilerecord.h>
#include <gtest/gtest.h>

#include <fstream>
#include <string>

TEST_F(TreeTest, NodeTest1)
{
    A a;
    a.x = 123;
    a.y = 56.78;
    auto loc1 = this->file->append(a, btree::PackPODFileRecord<A>);

    btree::RecordFile file(indexFile);
    btree::BTreeNode<std::string> node(2);
    node.insert("abc", loc1);

    btree::FileLocation loc2;
    ASSERT_TRUE(node.search("abc", loc2));
    ASSERT_EQ(loc1, loc2);
}

TEST_F(TreeTest, NodeTest2)
{
    A a;
    a.x = 123;
    a.y = 56.78;
    auto loc1 = this->file->append(a, btree::PackPODFileRecord<A>);
    auto loc2 = this->file->append(a, btree::PackPODFileRecord<A>);
    auto loc3 = this->file->append(a, btree::PackPODFileRecord<A>);
    auto loc4 = this->file->append(a, btree::PackPODFileRecord<A>);
    std::unique_ptr<btree::FileLocation> nodeLoaction;
    {
        btree::RecordFile file(indexFile);
        btree::BTreeNode<std::string> node(bTreeOrder);
        ASSERT_TRUE(node.canInsert());
        node.insert("abc", loc1);
        ASSERT_TRUE(node.canInsert());
        node.insert("abcd", loc2);
        ASSERT_TRUE(node.canInsert());
        node.insert("abcde", loc3);
        ASSERT_TRUE(node.canInsert());
        node.insert("abcdef", loc4);

        ASSERT_FALSE(node.canInsert());
        node.remove("abcdef", loc4);
        ASSERT_TRUE(node.canInsert());

        nodeLoaction.reset(new btree::FileLocation(file.append(node, btree::BTreeNodePack<std::string>)));
        indexFile.flush();
    }
    ASSERT_TRUE(nodeLoaction.get() != nullptr);
    btree::RecordFile file(indexFile);
    btree::BTreeNode<std::string> node;
    file.read(*nodeLoaction, node, btree::BTreeNodeUnPack<std::string>);
    
    ASSERT_TRUE(node.canInsert());
    ASSERT_TRUE(node.search("abc", loc2));
    ASSERT_EQ(loc1, loc2);
}