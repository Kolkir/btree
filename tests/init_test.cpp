#include <btree/btree.h>
#include <gtest/gtest.h>

#include <string>
#include <fstream>
#include <algorithm>
/*
class TreeTest : public ::testing::Test 
{
protected:
    virtual void SetUp() 
    {
        this->fileName = "test.dat";
        this->keys = "CSDTAMPIBWNGURKEHOLJYQZFXV";
        this->bTreeOrder = 4;
        this->file = std::fstream(this->fileName, std::ios::in | std::ios::out | std::ios::binary);
        ASSERT_FALSE(file);
    }

    virtual void TearDown() 
    {
        this->file.close();
        std::remove(this->fileName.c_str());
    }


    std::string fileName;
    std::string keys;
    int bTreeOrder;
    std::fstream file;
};

TEST_F(TreeTest, TestInsert)
{
    btree::BTree<char, int> tree(bTreeOrder, file);
    tree.insert(keys[0], 0);
    int val = -1;
    ASSERT_FALSE(tree.get(keys[0], val));
    ASSERT_EQ(0, val);
};
*/

