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
    btree::BTreeNode<std::string> node;
    node.insert("abc", loc1);

    auto loc2 = node.search("abc");
    ASSERT_EQ(loc1, loc2);
}

TEST_F(TreeTest, NodeTest2)
{
    A a;
    a.x = 123;
    a.y = 56.78;
    auto loc1 = this->file->append(a, btree::PackPODFileRecord<A>);
    std::unique_ptr<btree::FileLocation> nodeLoaction;
    {
        btree::RecordFile file(indexFile);
        btree::BTreeNode<std::string> node;
        node.insert("abc", loc1);
        nodeLoaction.reset(new btree::FileLocation(file.append(node, btree::BTreeNodePack<std::string>)));
        indexFile.flush();
    }
    ASSERT_TRUE(nodeLoaction);
    btree::RecordFile file(indexFile);
    btree::BTreeNode<std::string> node;
    file.read(*nodeLoaction, node, btree::BTreeNodeUnPack<std::string>);
    
    auto loc2 = node.search("abc");
    ASSERT_EQ(loc1, loc2);
}