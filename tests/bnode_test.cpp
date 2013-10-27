#include "treetestfix.h"

#include <btree/btreenode.h>
#include <btree/keypack.h>
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
    auto loc5 = this->file->append(a, btree::PackPODFileRecord<A>);
    std::unique_ptr<btree::FileLocation> nodeLoaction;
    {
        btree::RecordFile file(indexFile);
        btree::BTreeNode<std::string> node(bTreeOrder);
        ASSERT_FALSE(node.isOverflow());
        node.insert("abc", loc1);
        ASSERT_FALSE(node.isOverflow());
        node.insert("abcd", loc2);
        ASSERT_FALSE(node.isOverflow());
        node.insert("abcde", loc3);
        ASSERT_FALSE(node.isOverflow());
        node.insert("abcdef", loc4);
        ASSERT_FALSE(node.isOverflow());
        node.insert("abcdefg", loc5);

        ASSERT_TRUE(node.isOverflow());
        node.remove("abcdef", loc4);
        ASSERT_FALSE(node.isOverflow());

        nodeLoaction.reset(new btree::FileLocation(file.append(node, btree::BTreeNodePack<std::string, 100, btree::KeyPack<std::string> >)));
        indexFile.flush();
    }
    ASSERT_TRUE(nodeLoaction.get() != nullptr);
    btree::RecordFile file(indexFile);
    btree::BTreeNode<std::string> node;
    file.read(*nodeLoaction, node, btree::BTreeNodeUnPack<std::string, 100, btree::KeyUnPack<std::string> >);
    
    ASSERT_FALSE(node.isOverflow());
    ASSERT_TRUE(node.search("abc", loc2));
    ASSERT_EQ(loc1, loc2);
}