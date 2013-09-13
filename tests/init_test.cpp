#include <btree/btree.h>
#include <gtest/gtest.h>

#include <string>
#include <fstream>
#include <algorithm>
#include <string>

class TreeTest : public ::testing::Test 
{
protected:
    virtual void SetUp() 
    {
        this->fileName = "test.dat";
        this->bTreeOrder = 4;

        {
            std::ofstream f(this->fileName);
        }
        this->file = std::fstream(this->fileName, std::ios::in | std::ios::out | std::ios::binary);
        ASSERT_TRUE(file.good());
    }

    virtual void TearDown() 
    {
        this->file.close();
        std::remove(this->fileName.c_str());
    }


    std::string fileName;
    int bTreeOrder;
    std::fstream file;
};

TEST_F(TreeTest, TestInsert)
{
    /*btree::BTree<std::string> tree(bTreeOrder);
    tree.create(file);
    tree.insert("Hello", 0);
    int val = -1;
    ASSERT_FALSE(tree.get("Hello", val));
    ASSERT_EQ(0, val);*/
};


