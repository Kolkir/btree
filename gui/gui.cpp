#include "gui.h"

#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

#include <sstream>
#include <string>
#include <vector>

namespace
{
    int rect_margin = 6;

    template <class Callable>
    void handleException(Fl_Window* window, Callable&& func)
    {
        auto processError = [&](BtreeGUI* ui, const char* msg)
        {
            if (ui != nullptr)
            {
                ui->errMsg = msg;
                int res = fl_choice(msg, "Terminate", "ReThrow", "Ignore");
                if (res == 0)
                {
                    std::terminate();
                }
                else if (res == 1)
                {
                    throw;
                }
                else
                {
                    ui->errMsg.clear();
                }
            }
            else
            {
                throw;
            }
        };

        BtreeGUI* ui = nullptr;
        if (window != nullptr)
        {
            ui = reinterpret_cast<BtreeGUI*>(window->user_data());
        }

        try
        {
            if (ui == nullptr ||
                ui->errMsg.empty())
            {
                func(ui);
            }
        }
        catch (std::exception& ex)
        {
            processError(ui, ex.what());
        }
        catch (...)
        {
            processError(ui, "Unknown error");
        }
    }
}

Canvas::Canvas(int x, int y, int w, int h)
    : Fl_Widget(x, y, w, h)
    , nodeSpace(20)

{
}

void Canvas::drawNode(const Application::TreeType::KeyNodePtr& node, int xpos, int ypos, bool isLast, bool isRoot)
{
    std::stringstream buf;
    std::for_each(node->children.begin(), node->children.end(),
        [&buf](decltype(*node->children.begin())& node)
    {
        buf << node.first << "; ";
    });
    auto str = buf.str();
    if (!str.empty())
    {
        str.resize(str.size() - 1);
    }

    int dx = 0;
    int dy = 0;
    int tw = 0;
    int th = 0;

    fl_text_extents(str.c_str(), dx, dy, tw, th);

    int tx = x() - dx + xpos + rect_margin + (nodeWidth - tw) / 2;
    int ty = y() - dy + ypos + rect_margin;

    fl_draw(str.c_str(), tx, ty);

    fl_rect(x() + xpos,
            y() + ypos,
            nodeWidth,
            nodeHeight);

    if (!isRoot)
    {
        fl_line(x() + xpos + nodeWidth / 2,
                y() + ypos,
                x() + xpos + nodeWidth / 2,
                y() + ypos - nodeSpace / 2);
    }

    if (!isLast)
    {
        fl_line(x() + xpos + nodeWidth / 2,
                y() + ypos + nodeHeight,
                x() + xpos + nodeWidth / 2,
                y() + ypos + nodeHeight + nodeSpace / 2);

    }
}

ChildrenPoints Canvas::drawNodeRec(size_t treeHeight, const Application::TreeType::KeyNodePtr& node, size_t level, int rightShift)
{
    if (level < treeHeight)
    {
        size_t childrenStart = std::numeric_limits<size_t>::max();
        size_t childrenEnd = 0;
        size_t internalRightShift = rightShift;

        std::for_each(node->children.begin(), node->children.end(),
            [&](decltype(*node->children.begin())& cnode)
        {
            auto points = drawNodeRec(treeHeight, cnode.second, level + 1, internalRightShift);
            internalRightShift = points.shift;
            childrenStart = std::min(childrenStart, points.start);
            childrenEnd = std::max(childrenEnd, points.end);
        });

        size_t nodeStart = childrenStart + (childrenEnd - childrenStart) / 2 - nodeWidth / 2;
        size_t ypos = nodeSpace + (level - 1) * (nodeHeight + nodeSpace);
        drawNode(node,
                 nodeStart,
                 ypos,
                 false,
                 level == 1);

        fl_line(x() + nodeStart - nodeSpace / 2,
                y() + ypos + nodeHeight + nodeSpace / 2,
                x() + nodeStart + nodeWidth + nodeSpace / 2,
                y() + ypos + nodeHeight + nodeSpace / 2);

        return ChildrenPoints(nodeStart, nodeStart + nodeWidth, internalRightShift + nodeSpace * 2);
    }
    else
    {
        drawNode(node,
                 rightShift,
                 nodeSpace + (level - 1) * (nodeHeight + nodeSpace),
                 true,
                 level == 1);
        return ChildrenPoints(rightShift, rightShift + nodeWidth, rightShift + nodeWidth + nodeSpace);
    }

}

void Canvas::draw()
{
    handleException(this->window(), [&](BtreeGUI* ui)
    {
        if (ui != nullptr)
        {
            auto root = ui->app.getTreeStructure();
            if (!root)
            {
                return;
            }

            fl_color(0);
            fl_font(FL_HELVETICA, 16);

            int dx = 0;
            int dy = 0;
            nodeWidth = 0;
            nodeHeight = 0;

            fl_text_extents("9999; 9999; 9999; 9999", dx, dy, nodeWidth, nodeHeight);

            nodeWidth += rect_margin * 2;
            nodeHeight += rect_margin * 2;

            drawNodeRec(ui->app.getTreeHeight(), root, 1, nodeSpace);
        }
    });
}

MainWindow::MainWindow(int width, int height, const char* name)
    : Fl_Double_Window(width, height, name)
{
}

int MainWindow::handle(int event)
{
    return Fl_Double_Window::handle(event);
}

//event handlers decalrations
namespace
{
    void OnFileNewMenu(Fl_Menu_*, void*);
    void OnFileOpenMenu(Fl_Menu_*, void*);
    void OnQuit(Fl_Menu_*, void*);
    void OnAddItem(Fl_Button*, void*);
    void OnDelItem(Fl_Button*, void*);
    void OnClear(Fl_Button*, void*);
}

Fl_Menu_Item BtreeGUI::menu[] =
{
    {"&File", 0, 0, 0, FL_SUBMENU},
      {"&New", 0, (Fl_Callback*) OnFileNewMenu },
      {"&Open", 0, (Fl_Callback*) OnFileOpenMenu},
      {"&Quit", 0, (Fl_Callback*) OnQuit},
      {0},
    {0}
};

BtreeGUI::BtreeGUI()
{
    this->mainWindow = new MainWindow(800, 600, "Btree Visualization");
    this->mainWindow->size_range(640, 480);

    this->mainWindow->user_data((void*) (this));
    {
        this->menuBar = new Fl_Menu_Bar(0, 0, 800, 20);
        this->menuBar->menu(this->menu);
    }

    {
        Fl_Group* o = new Fl_Group(0, 20, 800, 75);
        o->box(FL_DOWN_BOX);
        o->color((Fl_Color) 19);
        {
            int left = 60;
            this->itemIn = new Fl_Value_Input(left, 25, 60, 25, "Item :");
            this->itemIn->maximum(1000);
            this->itemIn->minimum(1);
            this->itemIn->step(1);
            this->itemIn->value(0);
            left += 60;//width
            left += 20;//space

            this->addItemBtn = new Fl_Button(left, 25, 60, 25, "Add");
            this->addItemBtn->callback((Fl_Callback*) OnAddItem);
            left += 60;//width
            left += 20;//space

            this->delItemBtn = new Fl_Button(left, 25, 60, 25, "Delete");
            this->delItemBtn->callback((Fl_Callback*) OnDelItem);
            left += 60;//width
            left += 20;//space

            this->clearBtn = new Fl_Button(left, 25, 60, 25, "Clear all");
            this->clearBtn->callback((Fl_Callback*) OnClear);

            this->creationOrder = new Fl_Text_Display(120, 55, 250, 25, "Creation Order");
            this->creationOrder->align(FL_ALIGN_LEFT);

            this->textTree = new Fl_Text_Display(450, 55, 250, 25, "Text Tree");
            this->textTree->align(FL_ALIGN_LEFT);
        }
        o->end();//group
    }
    {
        this->imageScroll = new Fl_Scroll(0, 85, 800, 515);
        this->imageScroll->box(FL_DOWN_BOX);
        {
            this->imageBox = new Canvas(0, 85, 800, 515);
        }
        this->imageScroll->end();
    } //imageScroll

    this->mainWindow->end();
    this->mainWindow->resizable(this->imageScroll);
}

void BtreeGUI::Show(int argc, char** argv)
{
    if (argc > 0)
    {
        std::string path = argv[0];
        const char* fname = fl_filename_name(path.c_str());
        path.resize(fname - &path[0]);
        this->app.setWorkDir(path);
    }
    this->mainWindow->show(argc, argv);
}


//event handlers implementations
namespace
{
void OnFileNewMenu(Fl_Menu_* m, void*)
{
    handleException(m->window(), [&](BtreeGUI* ui)
    {
        if (ui != nullptr)
        {
            Fl_File_Chooser chooser(ui->app.getWorkDir().c_str(),
                "Tree Files (*.{itr})",
                Fl_File_Chooser::CREATE,
                "New tree");

            chooser.show();

            while (chooser.shown())
            {
                Fl::wait();
            }

            if (chooser.count() >= 1)
            {
                std::string path = chooser.value();
                std::string fname = fl_filename_name(path.c_str());
                path.resize(path.size() - fname.size());
                ui->app.setWorkDir(path);
                ui->app.makeNewTree(chooser.value());
                ui->mainWindow->redraw();
            }
        }
    });
}

void OnFileOpenMenu(Fl_Menu_* m, void*)
{
    handleException(m->window(), [&](BtreeGUI* ui)
    {
        if (ui != nullptr)
        {
            Fl_File_Chooser chooser(ui->app.getWorkDir().c_str(),
                "Tree Files (*.{itr})",
                Fl_File_Chooser::SINGLE,
                "Open tree");

            chooser.show();

            while (chooser.shown())
            {
                Fl::wait();
            }

            if (chooser.count() >= 1)
            {
                std::string path = chooser.value();
                std::string fname = fl_filename_name(path.c_str());
                path.resize(path.size() - fname.size());
                ui->app.setWorkDir(path);
                ui->app.openTree(chooser.value());
                ui->mainWindow->redraw();
            }
        }
    });
}

void OnQuit(Fl_Menu_* m, void*)
{
    m->window()->hide();
}

void OnAddItem(Fl_Button* b, void*)
{
    handleException(b->window(), [&](BtreeGUI* ui)
    {
        if (ui != nullptr)
        {
            ui->app.addItem(static_cast<unsigned int>(ui->itemIn->value()));
            ui->mainWindow->redraw();
        }
    });
}

void OnDelItem(Fl_Button* b, void*)
{
    handleException(b->window(), [&](BtreeGUI* ui)
    {
        if (ui != nullptr)
        {
            ui->app.delItem(static_cast<unsigned int>(ui->itemIn->value()));
            ui->mainWindow->redraw();
        }
    });
}

void OnClear(Fl_Button* b, void*)
{
    handleException(b->window(), [&](BtreeGUI* ui)
    {
        if (ui != nullptr)
        {
            ui->app.clearItems();
            ui->mainWindow->redraw();
        }
    });
}
}
