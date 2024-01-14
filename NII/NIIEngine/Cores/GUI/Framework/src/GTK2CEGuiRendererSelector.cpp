/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/
#include "GTK2CEGuiRendererSelector.h"

const std::string GTK2CEGuiRendererSelector::OgreRendererName("NiiEngine");

GTK2CEGuiRendererSelector::GTK2CEGuiRendererSelector()
{
    // fake args for gtk_init
    int argc = 1;
    char* argv = "SampleApp";
    char** pargv = &argv;

    gtk_init (&argc, &pargv);

    createDialog();
}

GTK2CEGuiRendererSelector::~GTK2CEGuiRendererSelector()
{
}

bool GTK2CEGuiRendererSelector::invokeDialog()
{
    CEGuiRendererType renderer_types[3];
    int idx = 0;

    // Put items in the combobox for enabled renderers.
    if (d_rendererAvailability[OgreGuiRendererType])
    {
        gtk_combo_box_append_text(GTK_COMBO_BOX(d_combobox1), OgreRendererName.c_str());
        renderer_types[idx++] = OgreGuiRendererType;
    }

    gtk_widget_show(d_dialog);
    gtk_main ();

    if (!d_cancelled)
    {
        gint active = gtk_combo_box_get_active(GTK_COMBO_BOX(d_combobox1));

        if (active != -1)
        {
            d_lastSelected = renderer_types[active];
        }
        else
        {
            d_cancelled = true;
        }

    }

    return !d_cancelled;
}

void GTK2CEGuiRendererSelector::createDialog()
{
    d_dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(d_dialog, 320, 100);
    gtk_window_set_title(GTK_WINDOW(d_dialog), "CEGui - Engine Selection");
    gtk_window_set_position(GTK_WINDOW(d_dialog), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(d_dialog), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(d_dialog), GDK_WINDOW_TYPE_HINT_DIALOG);

    d_vbox1 = gtk_vbox_new(FALSE, 10);
    gtk_widget_show(d_vbox1);
    gtk_container_add(GTK_CONTAINER(d_dialog), d_vbox1);

    d_label1 = gtk_label_new("Select Engine:");
    gtk_widget_show(d_label1);
    gtk_box_pack_start(GTK_BOX(d_vbox1), d_label1, FALSE, FALSE, 0);
    gtk_misc_set_alignment(GTK_MISC(d_label1), 0, 0.5);

    d_hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(d_hbox1);
    gtk_box_pack_end(GTK_BOX(d_vbox1), d_hbox1, FALSE, TRUE, 5);

    d_cancelButton = gtk_button_new_from_stock("gtk-cancel");
    gtk_widget_show(d_cancelButton);
    gtk_box_pack_start(GTK_BOX(d_hbox1), d_cancelButton, TRUE, FALSE, 0);

    d_okButton = gtk_button_new_from_stock("gtk-ok");
    gtk_widget_show(d_okButton);
    gtk_box_pack_start(GTK_BOX(d_hbox1), d_okButton, TRUE, FALSE, 0);

    d_combobox1 = gtk_combo_box_new_text();
    gtk_widget_show(d_combobox1);
    gtk_box_pack_start(GTK_BOX(d_vbox1), d_combobox1, TRUE, FALSE, 0);

    g_signal_connect((gpointer) d_cancelButton, "clicked",
                       G_CALLBACK(GTK2CEGuiRendererSelector::on_cancelButton_clicked),
                       this);
    g_signal_connect((gpointer) d_okButton, "clicked",
                       G_CALLBACK(GTK2CEGuiRendererSelector::on_okButton_clicked),
                       this);
}

void GTK2CEGuiRendererSelector::on_cancelButton_clicked(GtkButton* button, gpointer user_data)
{
    static_cast<GTK2CEGuiRendererSelector*>(user_data)->d_cancelled = true;
    gtk_widget_hide(static_cast<GTK2CEGuiRendererSelector*>(user_data)->d_dialog);
    gtk_main_quit();
}

void GTK2CEGuiRendererSelector::on_okButton_clicked(GtkButton* button, gpointer user_data)
{
    static_cast<GTK2CEGuiRendererSelector*>(user_data)->d_cancelled = false;
    gtk_widget_hide(static_cast<GTK2CEGuiRendererSelector*>(user_data)->d_dialog);
    gtk_main_quit();
}
