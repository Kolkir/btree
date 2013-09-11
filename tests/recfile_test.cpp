#include <btree/btree.h>
#include <gtest/gtest.h>

#include <fstream>

struct A : public btree::FileRecord
{
    int x;
    char y;
    float z;
    
    virtual size_t getSize() const 
    {
        return sizeof(A);
    }
    
    virtual void write(std::ostream& stream) const
    {
        stream.write(reinterpret_cast<const char*>(this), getSize());
    }

    virtual void read(std::istream& stream)
    {
        A a;
        stream.read(reinterpret_cast<char*>(&a), getSize());
        *this = a;
    }

};

bool operator== (const A& a, const A& b)
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

    std::fstream outFileStream(fname, std::fstream::in | std::fstream::out | std::fstream::binary);
    ASSERT_TRUE(outFileStream.good());
    if (outFileStream)
    {
        btree::RecordFile file(outFileStream);

        auto addr1 = file.append(a);
        ASSERT_TRUE(addr1.first);
        auto addr2 = file.append(a);
        ASSERT_TRUE(addr2.first);

        ASSERT_TRUE(file.write(addr2.second, a));

        outFileStream.close();
        
        std::fstream inFileStream(fname, std::fstream::in | std::fstream::out | std::fstream::binary);
        ASSERT_TRUE(inFileStream.good());
        if (inFileStream)
        {
            btree::RecordFile inFile(inFileStream);

            A b;
            ASSERT_TRUE(inFile.read(addr2.second, b));
            ASSERT_EQ(a, b);
            ASSERT_TRUE(inFile.read(addr1.second, b));
            ASSERT_EQ(a, b);

            inFileStream.close();
        }
    }
}