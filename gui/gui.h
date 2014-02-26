#ifndef _GUI_H_
#define _GUI_H_

#include "app.h"

#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Tile.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Value_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Text_Display.H>

#include <string>
#include <memory>

struct ChildrenPoints
{
    ChildrenPoints(size_t s, size_t e, size_t rs)
        : start(s)
        , end(e)
        , shift(rs)
    {}
    size_t start;
    size_t end;
    size_t shift;
};

class Canvas : public Fl_Widget
{
public:
    Canvas(int x, int y, int w, int h);
    virtual void draw();
private:
    ChildrenPoints drawNodeRec(size_t treeHeight, const Application::TreeType::KeyNodePtr& node, size_t level, int rightShift);
    void drawNode(const Application::TreeType::KeyNodePtr& node, int xpos, int ypos, bool isLast, bool isRoot);
private:
    int nodeWidth;
    int nodeHeight;
    int nodeSpace;

};

class MainWindow : public Fl_Double_Window
{
public:
    MainWindow(int width, int height, const char* name);
    virtual int handle(int event);
};

class BtreeGUI
{
public:
    BtreeGUI();
    void Show(int argc, char** argv);

public:
    MainWindow *mainWindow;
    Fl_Menu_Bar* menuBar;
    static Fl_Menu_Item menu[];

    Fl_Value_Input *itemIn;

    Fl_Button *addItemBtn;
    Fl_Button *delItemBtn;
    Fl_Button *clearBtn;

    Fl_Text_Display* creationOrder;
    Fl_Text_Buffer * creationOrderBuffer;

    Fl_Text_Display* textTree;
    Fl_Text_Buffer * textTreeBuffer;

    Fl_Scroll *imageScroll;
    Canvas *imageBox;

    Application app;

    std::string errMsg;

private:
    BtreeGUI(const BtreeGUI&);
    BtreeGUI& operator=(const BtreeGUI&);
};

#endif
