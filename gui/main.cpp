#include "gui.h"

#include <FL/Fl.H>
#include <FL/fl_ask.H>

#include <stdexcept>

int main(int argc, char **argv)
{
    fl_message_title_default("Btree Index visualization");

    BtreeGUI ui;

    ui.Show(argc, argv);

    return Fl::run();
}