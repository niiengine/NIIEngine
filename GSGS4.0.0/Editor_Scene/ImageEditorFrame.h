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

#ifndef _IMAGE_EDITOR_FRAME_H_
#define _IMAGE_EDITOR_FRAME_H_

#include "ImagePropertiesPanel.h"

#include "wx/wx.h"
#include "wx/docview.h"
#include "wx/splitter.h"

// We need a tiny addition to the retreived client rect
#define BORDER_SIZE 4

namespace gsgs
{
    // Forwarding
    class ImageDocument;
    class ImageGLCanvas;
    class ImageEditorView;
    class DialogResourceGroups;

    class ImageEditorFrame : public wxDocParentFrame
    {
        DECLARE_CLASS( ImageEditorFrame )
    public:
        /** Constructor. Attaches an OpenGL compatible canvas.*/
        ImageEditorFrame( wxDocManager* manager, wxFrame* frame, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, const long type );

        /** Destructor.*/
        ~ImageEditorFrame();

        /** Receives the document from the attached view for accessing by the menu-events.
        */
        void SetDocument( ImageDocument* document );

        /** Seperate method so we can call it after the entire frame has been initialised,
        * because for the canvas it must be visible, which doesn't look that nice when
        * creating the frame.*/
        ImageGLCanvas* AttachCanvas( ImageEditorView* view );

        ImageGLCanvas* getCanvas()                  { return m_glcanvasImageset; }

        ImagePropertiesPanel* getPropsPanel()       { return m_propsPanel; }

        /** We listen to the Edit events (including the Align part) of the menubar.*/
        DECLARE_EVENT_TABLE()

    private:
        ImageDocument* m_document;
        wxSplitterWindow* m_splitter;
        ImagePropertiesPanel* m_propsPanel;
        ImageGLCanvas* m_glcanvasImageset;
        DialogResourceGroups* m_resGrpsEditor;

        wxColour m_backgroundColour;

        /** We store the edit menu, to pass to the CommandProcessor.*/
        wxMenu* m_EditMenu;

        /** Attaches a menubar to the frame.*/
        void AttachMenubar();

        /** Attaches a toolbar to the frame.*/
        void AttachToolbar();

        void createContent();

        /** Overloaded so we can close the dialog as well.
        */
        //bool Close ( bool force = FALSE );

        /** Event handler: notifies us that the frame needs a repaint. */
        void OnPaint( wxPaintEvent& event );

        void OnChangeBackgroundColour( wxCommandEvent& event );
        void OnEditResourceGroups( wxCommandEvent& event );
    };
}
#endif // _EDITOR_FRAME_H_
