#ifndef _APP_H_
#define _APP_H_

#include <btree/podfilerecord.h>
#include <btree/btree.h>

#include <string>
#include <fstream>

class Application
{
public:
    typedef btree::BTree <btree::PodKey<unsigned int> > TreeType;

    Application();
    ~Application();

    const std::string& getWorkDir() const;
    void setWorkDir(const std::string& dir);

    void openTree(const std::string& fname);
    void makeNewTree(const std::string& fname);

    void addItem(unsigned int val);
    void delItem(unsigned int val);
    void clearItems();

    TreeType::KeyNodePtr getTreeStructure();
    size_t getTreeHeight() const;
    size_t getTreeOrder() const;

private:
    Application(const Application&);
    Application& operator=(const Application&);

    void close();
    void open(const std::string& fname, bool create);

private:
    std::string workDir;
    std::fstream dataFileStream;
    std::unique_ptr<btree::RecordFile> dataFile;
    std::fstream indexFileStream;
    std::unique_ptr<TreeType> tree;
};

#endif
