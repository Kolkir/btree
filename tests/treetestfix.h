#ifndef _TREE_TEST_FIX_H_
#define _TREE_TEST_FIX_H_

#include <btree/recordfile.h>
#include <gtest/gtest.h>

#include <string>
#include <fstream>
#include <algorithm>
#include <string>
#include <memory>

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
        this->dataFile.close();
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

#endif

