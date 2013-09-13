#include <btree/btree.h>
#include <btree/podfilerecord.h>
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
        this->indexFileName = "test.idx";
        this->bTreeOrder = 4;

        {
            std::ofstream f(this->indexFileName);
            std::ofstream f2(this->fileName);
        }
        this->indexFile = std::fstream(this->indexFileName, std::ios::in | std::ios::out | std::ios::binary);
        ASSERT_TRUE(indexFile.good());
        this->dataFile = std::fstream(this->fileName, std::ios::in | std::ios::out | std::ios::binary);
        ASSERT_TRUE(dataFile.good());

        this->file.reset(new btree::RecordFile(this->dataFile));
    }

    virtual void TearDown() 
    {
        this->indexFile.close();
        std::remove(this->indexFileName.c_str());
        std::remove(this->fileName.c_str());
    }


    std::string fileName;
    std::string indexFileName;
    int bTreeOrder;
    std::fstream indexFile;
    std::fstream dataFile;
    std::unique_ptr<btree::RecordFile> file;
};

struct A
{
    int x;
    double y;
};


TEST_F(TreeTest, TestInsert)
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


