#include "gui.h"

#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/fl_ask.H>
#include <FL/fl_draw.H>

#include <string>

Canvas::Canvas(int x, int y, int w, int h)
    : Fl_Widget(x, y, w, h)
{
}

void Canvas::draw()
{

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
    this->mainWindow = new Fl_Double_Window(800, 600, "Btree Visualization");
    this->mainWindow->size_range(640, 480);

    this->mainWindow->user_data((void*) (this));
    {
        this->menuBar = new Fl_Menu_Bar(0, 0, 800, 20);
        this->menuBar->menu(this->menu);
    }

    {
        Fl_Group* o = new Fl_Group(0, 20, 800, 35);
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
        }
        o->end();//group
    }
    { 
        this->imageScroll = new Fl_Scroll(0, 50, 800, 550);
        this->imageScroll->box(FL_DOWN_BOX);
        { 
            this->imageBox = new Canvas(0, 50, 800, 550);
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
        this->workDir = path;
    }
    this->mainWindow->show(argc, argv);
}


//event handlers implementations
namespace
{
void OnFileNewMenu(Fl_Menu_*, void*)
{
}

void OnFileOpenMenu(Fl_Menu_* m, void*)
{
    BtreeGUI* ui = reinterpret_cast<BtreeGUI*>(m->window()->user_data());
    if (ui != 0)
    {
        Fl_File_Chooser chooser(ui->workDir.c_str(),
            "Tree Files (*.{itr})",
            Fl_File_Chooser::SINGLE,
            "Open image");

        chooser.show();

        while (chooser.shown())
        {
            Fl::wait();
        }

        if (chooser.count() >= 1)
        {
            std::string path = chooser.value();
            const char* fname = fl_filename_name(path.c_str());
            path.resize(fname - &path[0]);
            ui->workDir = path;

        }
    }
}

void OnQuit(Fl_Menu_* m, void*)
{
    m->window()->hide();
}

void OnAddItem(Fl_Button*, void*)
{
}

void OnDelItem(Fl_Button*, void*)
{
}

void OnClear(Fl_Button*, void*)
{
}
}
