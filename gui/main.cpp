#include "gui.h"

#include <FL/Fl.H>
#include <FL/fl_ask.H>

#include <stdexcept>

int main(int argc, char **argv)
{
    try
    {
        fl_message_title_default("Btree Index visualization");

        BtreeGUI ui;
        ui.Show(argc, argv);

        bool done = false;
        while (!done)
        {
            try
            {
                if (!Fl::wait())
                {
                    done = true;
                }
            }
            catch (std::exception& err)
            {
                fl_alert(err.what());
            }
        }
    }
    catch (std::exception& err)
    {
        fl_alert(err.what());
    }
    catch (...)
    {
        fl_alert("Unhanded error!");
    }
    return 0;
}