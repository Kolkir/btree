#include "app.h"

static const size_t BTREE_ORDER = 4;

Application::Application()
{
}

void Application::close()
{
    this->dataFile.reset();
    this->tree.reset();
    this->dataFileStream.close();
    this->indexFileStream.close();
}

const std::string& Application::getWorkDir() const
{
    return this->workDir;
}

void Application::setWorkDir(const std::string& dir)
{
    this->workDir = dir;
}

void Application::openTree(const std::string& fname)
{
    this->close();
    this->open(fname, false);
}

void Application::makeNewTree(const std::string& fname)
{
    close();
    this->open(fname, true);
}

void Application::open(const std::string& fname, bool create)
{
    try
    {
        if (create)
        {
            std::ofstream data(fname, std::ios::binary);
            std::ofstream index(fname + ".idx", std::ios::binary);
        }

        this->dataFileStream.open(fname, std::ios::in | std::ios::out | std::ios::binary);
        if (this->dataFileStream)
        {
            this->dataFile = std::make_unique<btree::RecordFile>(this->dataFileStream);
        }
        else
        {
            throw std::logic_error("Can't open file" + fname);
        }

        this->indexFileStream.open(fname + ".idx", std::ios::in | std::ios::out | std::ios::binary);
        if (this->indexFileStream)
        {
            this->tree = std::make_unique<TreeType>(BTREE_ORDER);
            if (create)
            {
                this->tree->create(this->indexFileStream);
            }
            else
            {
                this->tree->open(this->indexFileStream);
            }
        }
        else
        {
            throw std::logic_error("Can't open file" + fname + ".idx");
        }
    }
    catch (...)
    {
        close();
        throw;
    }
}

void Application::addItem(unsigned int val)
{
    if (this->dataFile && this->tree)
    {
        auto i = this->dataFile->append(val, btree::PackPODFileRecord<decltype(val)>);
        tree->insert(val, i);
    }
}

void Application::delItem(unsigned int val)
{
    if (this->dataFile && this->tree)
    {
        tree->remove(val);
    }
}

void Application::clearItems()
{
    if (this->dataFile && this->tree)
    {
        this->dataFileStream.seekp(0);
        this->dataFileStream.seekg(0);
        this->dataFile = std::make_unique<btree::RecordFile>(this->dataFileStream);

        this->indexFileStream.seekp(0);
        this->indexFileStream.seekg(0);
        this->tree->create(this->indexFileStream);
    }
}

Application::TreeType::KeyNodePtr Application::getTreeStructure()
{
    if (this->tree)
    {
        return this->tree->getTreeStructure();
    }
    else
    {
        return TreeType::KeyNodePtr();
    }
}

size_t Application::getTreeHeight() const
{
    if (this->tree)
    {
        return tree->getHeight();
    }
    return 0;
}

