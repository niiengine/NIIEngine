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

#include "NiiErrorDialog.h"

#include <iostream>
#include <gtk/gtk.h>

namespace NII 
{
    //---------------------------------------------------------------------------
    extern bool __gtk_init_once ();
    //---------------------------------------------------------------------------//
    ErrorDialog::ErrorDialog ()
    {
    }
    //---------------------------------------------------------------------------//
    void ErrorDialog::display (const String& errorMessage, String logName)
    {
        if (!__gtk_init_once ())
        {
            std::cerr << "*** ERROR: " << errorMessage << std::endl;
            return;
        }

        GtkWidget *dialog = gtk_message_dialog_new (
            NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK,
            errorMessage.c_str ());

        gtk_dialog_run (GTK_DIALOG (dialog));
        gtk_widget_destroy (dialog);

        // Wait for all gtk events to be consumed ...
        while (gtk_events_pending ())
            gtk_main_iteration_do (FALSE);
    }
    //---------------------------------------------------------------------------
}
