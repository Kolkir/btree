#include <btree/btree.h>
#include <gtest/gtest.h>

#include <fstream>

struct A
{
    int x;
    char y;
    float z;
};

bool operator== (const A& a, const A& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

struct B
{
    double x;
    double y;
    double z;
};

bool operator== (const B& a, const B& b)
{
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
//------------------------------------------------------------------------------

class FileRecordTest : public ::testing::Test 
{
protected:
    virtual void SetUp() 
    {
        fname = "test.dat";
        //create file at first
        std::ofstream f(fname);
    }

    virtual void TearDown() 
    {
        std::remove(fname.c_str());
    }
    std::string fname;
};

TEST_F(FileRecordTest, Test1)
{
    A a;
    a.x = 1;
    a.y = 2;
    a.z = 3.5;

    B b;
    b.x = 1.78;
    b.y = 2.56;
    b.z = 35.5;

    std::fstream outFileStream(fname, std::fstream::in | std::fstream::out | std::fstream::binary);
    ASSERT_TRUE(outFileStream.good());
    if (outFileStream)
    {
        btree::RecordFile file(outFileStream);

        auto addr1 = file.append(btree::PODFileRecord<A>(a));
        ASSERT_TRUE(addr1.first);
        auto addr2 = file.append(btree::PODFileRecord<B>(b));
        ASSERT_TRUE(addr2.first);
        auto addr3 = file.append(btree::PODFileRecord<A>(a));
        ASSERT_TRUE(addr3.first);

        ASSERT_TRUE(file.write(addr2.second, btree::PODFileRecord<A>(a)));

        outFileStream.close();
        
        std::fstream inFileStream(fname, std::fstream::in | std::fstream::out | std::fstream::binary);
        ASSERT_TRUE(inFileStream.good());
        if (inFileStream)
        {
            btree::RecordFile inFile(inFileStream);

            A a2;
            btree::PODFileRecord<A> aread(a2);
            size_t maxLen = 0;
            ASSERT_TRUE(inFile.read(addr2.second, aread, maxLen));
            ASSERT_EQ(a, a2);
            ASSERT_EQ(sizeof(b), maxLen);
            ASSERT_TRUE(inFile.read(addr1.second, aread, maxLen));
            ASSERT_EQ(a, a2);
            ASSERT_EQ(sizeof(a), maxLen);

            inFileStream.close();
        }
    }
}