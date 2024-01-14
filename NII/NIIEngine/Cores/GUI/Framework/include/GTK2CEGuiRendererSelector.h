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
#ifndef _GTK2CEGuiRendererSelector_h_
#define _GTK2CEGuiRendererSelector_h_

#include "CEGuiRendererSelector.h"
#include <gtk/gtk.h>
#include <string>

class GTK2CEGuiRendererSelector : public CEGuiRendererSelector
{
public:
    GTK2CEGuiRendererSelector();
    ~GTK2CEGuiRendererSelector();

    bool invokeDialog();
private:
    // Renderer name strings
    static const std::string    OgreRendererName;
    static const std::string    OpenGLRendererName;
    static const std::string    IrrlichtRendererName;

    // Static member funcs
    void createDialog();
    static void on_cancelButton_clicked(GtkButton* button, gpointer user_data);
    static void on_okButton_clicked(GtkButton* button, gpointer user_data);
    static void on_combobox1_changed(GtkComboBox* combobox, gpointer user_data);

    // general data
    bool d_cancelled;

    // GTK2 widgets
    GtkWidget* d_dialog;
    GtkWidget* d_vbox1;
    GtkWidget* d_label1;
    GtkWidget* d_hbox1;
    GtkWidget* d_cancelButton;
    GtkWidget* d_okButton;
    GtkWidget* d_combobox1;
};

#endif  // end of guard _GTK2CEGuiRendererSelector_h_
