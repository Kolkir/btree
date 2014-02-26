#include "app.h"

#include <sstream>

static const size_t BTREE_ORDER = 4;

Application::Application()
{
}

Application::~Application()
{
    this->close();
}

void Application::close()
{
    if (this->tree)
    {
        this->tree->save();
    }

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
    this->fname = fname;
    this->close();
    this->open(fname, false);
}

void Application::makeNewTree(const std::string& fname)
{
    this->fname = fname;
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
            this->dataFile.reset(new btree::RecordFile(this->dataFileStream));
        }
        else
        {
            throw std::logic_error("Can't open file" + fname);
        }

        this->indexFileStream.open(fname + ".idx", std::ios::in | std::ios::out | std::ios::binary);
        if (this->indexFileStream)
        {
            this->tree.reset(new TreeType(BTREE_ORDER));
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
        std::stringstream buf;
        buf << ";" << val;
        creationOrderText += buf.str();
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
    if (!this->fname.empty())
    {
        this->close();
        this->open(this->fname, true);
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

size_t Application::getTreeOrder() const
{
    return BTREE_ORDER;
}

const std::string& Application::getCreationOrderText() const
{
    return  this->creationOrderText;
}
