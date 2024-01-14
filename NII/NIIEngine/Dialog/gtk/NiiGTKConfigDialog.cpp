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

#include "NiiConfigDialog.h"
#include "NiiException.h"
#include "NiiLogManager.h"

#include <gtk/gtk.h>

namespace NII
{
/**
Backdrop image. This is an arbitrary PNG file encoded into a C array.

You can easily generate your own backdrop with the following python script:

#!/usr/bin/python
import sys
pngstring=open(sys.argv[2], "rb").read()
print "static Nui8 %s[%i]={%s};" % (sys.argv[1],len(pngstring), ",".join([str(ord(x)) for x in pngstring]))

Call this with
$ bintoheader.py GLX_backdrop_data GLX_backdrop.png > GLX_backdrop.h
*/
#include "GLX_backdrop.h"

/*
 * A GTK+2 dialog window, making it possible to configure NII
 * in a graphical way. It uses plain C gtk+ bindings since gtk-- is not
 * part of most standard distributions while gtk+ itself is present
 * in every Linux distribution I ever seen.
 */
    //------------------------------------------------------------
    bool _NiiPrivate __gtk_init_once ()
    {
        static bool gtk_already_initialized = false;
        if (gtk_already_initialized)
            return true;

        gtk_already_initialized = true;

        // Initialize gtk+
        NIIi argc = 0;
        NIIb **argv = NULL;
        // Avoid gtk calling setlocale() otherwise
        // scanf("%f") won't work on some locales etc.
        // Leave this on application developer's responsability.
        gtk_disable_setlocale ();
        return gtk_init_check (&argc, &argv);
    }
    //------------------------------------------------------------
    ConfigDialog::ConfigDialog (){}
    //------------------------------------------------------------
    void ConfigDialog::rendererChanged (GtkComboBox * widget, gpointer data)
    {
        ConfigDialog * This = static_cast<ConfigDialog *>(data);

        gchar * renderer = gtk_combo_box_get_active_text(widget);

        const RenderSysList &renderers = N_Engine().getRenderList ();
        for (RenderSysList::const_iterator r = renderers.begin(); r != renderers.end (); r++)
        {
            if (strcmp(renderer, (*r)->getName ().c_str ()) == 0)
            {
                This->mSelectedRenderSystem = *r;
                This->setupRendererParams ();
            }
        }
    }
    //------------------------------------------------------------
    gboolean ConfigDialog::refreshParams (gpointer data)
    {
        ConfigDialog *This = static_cast<ConfigDialog *> (data);

        This->setupRendererParams ();
        return FALSE;
    }
    //------------------------------------------------------------
    void ConfigDialog::optionChanged (GtkComboBox *widget, gpointer data)
    {
        ConfigDialog *This = static_cast<ConfigDialog *> (data);
        GtkWidget *ro_label = static_cast<GtkWidget *> (
            g_object_get_data (G_OBJECT (widget), "renderer-option"));

        This->mSelectedRenderSystem->setConfig (
            gtk_label_get_text (GTK_LABEL (ro_label)),
            gtk_combo_box_get_active_text (widget));

        g_idle_add (refreshParams, data);
    }
    //-----------------------------------------------------------
    static void remove_all_callback (GtkWidget *widget, gpointer data)
    {
        GtkWidget *container = static_cast<GtkWidget *> (data);
        gtk_container_remove(GTK_CONTAINER (container), widget);
    }
    //-----------------------------------------------------------
    void ConfigDialog::setupRendererParams ()
    {
        // Remove all existing child widgets
        gtk_container_forall(GTK_CONTAINER (mParamTable), remove_all_callback, mParamTable);

        SysOptionList options = mSelectedRenderSystem->getConfig ();

        // Resize the table to hold as many options as we have
        gtk_table_resize (GTK_TABLE (mParamTable), options.size (), 2);

        Nui32 row = 0;
        for (SysOptionList::iterator i = options.begin(); i != options.end (); i++, row++)
        {
            if (i->second.mReserve.empty())
            {
                continue;
            }

            GtkWidget *ro_label = gtk_label_new (i->second.name.c_str ());
            gtk_widget_show (ro_label);
            gtk_table_attach (GTK_TABLE (mParamTable), ro_label, 0, 1, row, row + 1,
                              GtkAttachOptions (GTK_EXPAND | GTK_FILL),
                              GtkAttachOptions (0), 5, 0);
            gtk_label_set_justify (GTK_LABEL (ro_label), GTK_JUSTIFY_RIGHT);
            gtk_misc_set_alignment (GTK_MISC (ro_label), 1, 0.5);

            GtkWidget *ro_cb = gtk_combo_box_new_text ();
            gtk_widget_show (ro_cb);
            gtk_table_attach (GTK_TABLE (mParamTable), ro_cb, 1, 2, row, row + 1,
                              GtkAttachOptions (GTK_EXPAND | GTK_FILL),
                              GtkAttachOptions (0), 5, 0);

            // Set up a link from the combobox to the label
            g_object_set_data (G_OBJECT (ro_cb), "renderer-option", ro_label);

            StringList::iterator opt_it;
            Nui32 idx = 0;
            for (opt_it = i->second.mReserve.begin();
                 opt_it != i->second.mReserve.end (); opt_it++, idx++)
            {
                gtk_combo_box_append_text (GTK_COMBO_BOX (ro_cb),
                                           (*opt_it).c_str ());
                if (strcmp (i->second.mValue.c_str (), (*opt_it).c_str ()) == 0)
                    gtk_combo_box_set_active (GTK_COMBO_BOX (ro_cb), idx);
            }

            g_signal_connect (G_OBJECT (ro_cb), "changed",
                              G_CALLBACK (optionChanged), this);
        }

        gtk_widget_grab_focus (GTK_WIDGET (mOKButton));
    }
    //------------------------------------------------------------------
    static void backdrop_destructor (guchar * pixels, gpointer data)
    {
        free (pixels);
    }
    //------------------------------------------------------------------
    bool ConfigDialog::createWindow ()
    {
        // Create the dialog window
        mDialog = gtk_dialog_new_with_buttons(_T("NII Engine Setup"),
            NULL, GTK_DIALOG_MODAL, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, NULL);

        mOKButton = gtk_dialog_add_button(GTK_DIALOG(mDialog), GTK_STOCK_OK, GTK_RESPONSE_OK);
        gtk_window_set_position(GTK_WINDOW(mDialog), GTK_WIN_POS_CENTER);
        gtk_window_set_resizable(GTK_WINDOW(mDialog), FALSE);
        gtk_widget_show(GTK_DIALOG(mDialog)->vbox);

        GtkWidget * vbox = gtk_vbox_new(FALSE, 5);
        gtk_widget_show(vbox);
        gtk_box_pack_start(GTK_BOX (GTK_DIALOG (mDialog)->vbox), vbox, TRUE, TRUE, 0);

        // Unpack the image and create a GtkImage object from it
        try
        {
            static String imgType ("png");
            Image img;
            MemStream * imgStream = N_new MemStream (GLX_backdrop_data, sizeof (GLX_backdrop_data), false);
            img.load (imgStream, imgType);
            N_delete imgStream;
            PixelBlock src = img.getPixel(0, 0);

            NCount width = img.getWidth();
            NCount height = img.getHeight();

            // Convert and copy image -- must be allocated with malloc
            Nui8 * data = (Nui8 *)malloc(width * height * 4);
            // Keep in mind that PixelBlock does not free the data - this is ok
            // as gtk takes pixel data ownership in gdk_pixbuf_new_from_data
            PixelBlock dst(src, PF_A8B8G8R8, data);

            PixelUtil::bulkPixelConversion(src, dst);

            GdkPixbuf * pixbuf = gdk_pixbuf_new_from_data (
                (const guchar *)dst.mData, GDK_COLORSPACE_RGB,
                    true, 8, width, height, width * 4, backdrop_destructor, NULL);

            GtkWidget * nii_logo = gtk_image_new_from_pixbuf (pixbuf);

            gdk_pixbuf_unref(pixbuf);

            gtk_widget_show(nii_logo);
            gtk_box_pack_start(GTK_BOX(vbox), nii_logo, FALSE, FALSE, 0);
        }
        catch (Exception & e)
        {
            // Could not decode image; never mind
            N_Only(Log).log(_T("WARNING: Failed to decode Nii logo image"));
        }

        GtkWidget * rs_hbox = gtk_hbox_new(FALSE, 0);
        gtk_box_pack_start (GTK_BOX(vbox), rs_hbox, FALSE, TRUE, 0);

        GtkWidget * rs_label = gtk_label_new(_T("Rendering subsystem:"));
        gtk_widget_show(rs_label);
        gtk_box_pack_start(GTK_BOX(rs_hbox), rs_label, TRUE, TRUE, 5);
        gtk_label_set_justify(GTK_LABEL (rs_label), GTK_JUSTIFY_RIGHT);
        gtk_misc_set_alignment(GTK_MISC (rs_label), 1, 0.5);

        GtkWidget *rs_cb = gtk_combo_box_new_text ();
        gtk_widget_show(rs_cb);
        gtk_box_pack_start(GTK_BOX (rs_hbox), rs_cb, TRUE, TRUE, 5);

        g_signal_connect(G_OBJECT (rs_cb), "changed", G_CALLBACK (rendererChanged), this);

        // Add all available renderers to the combo box
        const RenderSysList & renderers = N_Engine().getRenderList ();
        Nui32 idx = 0, sel_renderer_idx = 0;
        for (RenderSysList::const_iterator r = renderers.begin(); r != renderers.end (); r++, idx++)
        {
            gtk_combo_box_append_text (GTK_COMBO_BOX (rs_cb), (*r)->getName ().c_str ());
            if (mSelectedRenderSystem == *r)
                sel_renderer_idx = idx;
        }
        // Don't show the renderer choice combobox if there's just one renderer
        if (idx > 1)
            gtk_widget_show (rs_hbox);

        GtkWidget *ro_frame = gtk_frame_new (NULL);
        gtk_widget_show (ro_frame);
        gtk_box_pack_start (GTK_BOX (vbox), ro_frame, TRUE, TRUE, 0);

        GtkWidget *ro_label = gtk_label_new (_T("Renderer options:"));
        gtk_widget_show (ro_label);
        gtk_frame_set_label_widget (GTK_FRAME (ro_frame), ro_label);
        gtk_label_set_use_markup (GTK_LABEL (ro_label), TRUE);

        mParamTable = gtk_table_new (0, 0, FALSE);
        gtk_widget_show (mParamTable);
        gtk_container_add (GTK_CONTAINER (ro_frame), mParamTable);

        gtk_combo_box_set_active (GTK_COMBO_BOX (rs_cb), sel_renderer_idx);

        return true;
    }
    //-------------------------------------------------------------------
    bool ConfigDialog::display ()
    {
        if (!__gtk_init_once ())
            return false;

        /* Select previously selected rendersystem */
        N_Engine().get(mSelectedRenderSystem);

        /* Attempt to create the window */
        if (!createWindow())
            N_EXCEPT (InternalError, _T("Could not create configuration dialog"));

        // Modal loop
        gint result = gtk_dialog_run(GTK_DIALOG(mDialog));
        gtk_widget_destroy(mDialog);

        // Wait for all gtk events to be consumed ...
        while (gtk_events_pending())
            gtk_main_iteration_do(FALSE);

        if (result != GTK_RESPONSE_OK)
            return false;

        N_Engine().set(mSelectedRenderSystem);

        return true;
    }
    //-------------------------------------------------------------------
}