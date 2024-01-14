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

#ifndef _ImageEDITOR_VIEW_H_
#define _ImageEDITOR_VIEW_H_

#include "wx/wx.h" 
#include "wx/docview.h" 

namespace gsgs
{
    class ImageEditorFrame;
    class ImageGLCanvas;
    class ImagePropertiesPanel;

    /** A "view" on a "document". Note that views are not created when the application
     * starts, but only when the user chooses "file->new" or "file->open".
     * Also note that the dialog is not a real view.
     */
    class ImageEditorView: public wxView
    {
        // Needed for the doc/view managerF
        DECLARE_DYNAMIC_CLASS( ImageEditorView )
    public:
        /** Constructor.*/
        ImageEditorView();

        /** Called by the document when the document's name changes, e.g. on Save or New.*/
        void onChangeFilename();

        // possible actions on the document
        void onChangeImageFile( const wxString& newfilename );

        /** Called by the document when the imageset name change.*/
        void onChangeImagesetName( const wxString& newname );

        /** Called by the document when the native resolution change.*/
        void onChangeNativeResolution( const wxPoint& newnativeres );

        /** Called by the document when the AutoScaled change.*/
        void onChangeAutoScaled( const bool newAutoScaled );

        /** Called by the document when a region is added.*/
        void onAddRegion( const wxString& name, const wxRect& dim );

        /** Called by the document when a region is deleted.*/
        void onDelRegion( const wxString& name );

        /** Called by the document when all region are deleted.*/
        void onDelAllRegion();

        /** Called by the document when a region is moved and/or scaled.*/
        void onSetRegionArea( const wxString& name, const wxRect& dim, const bool evt_src = false );

        /** Called by the document when all region are deleted.*/
        void onRenameRegion( const wxString& currentname, const wxString& newname );

        // Frame part update section
        /** Updates the mainframe's title bar according to the currently open document.*/
        void updateTitle();

        /** Updates the Mouse part of the status bar.*/
        void updateMouseStatusBar( const float posx, const float posy );

        /** Updates the View part of the status bar.*/
        void updateDocStatusBar( const float zoom, const float width, const float height );

        ImageGLCanvas* getCanvas()
        {
            return m_glcanvasImageset;
        }

        ImagePropertiesPanel* getPropsPanel()
        {
            return m_propsPanel;
        }
    private:
        //! ImageEditorFrame we are attached to.
        ImageEditorFrame* m_frame;
        //! ImagePropertiesPanel forming part of this ImageEditorView
        ImagePropertiesPanel* m_propsPanel;
        //! The ImageGLCanvas where we do our OpenGL and GUI based rendering.
        ImageGLCanvas* m_glcanvasImageset;

        /** Overwritten because the base class is a Window. We don't use it here.*/
        void OnDraw( wxDC* dc );

        /** Notifies us that a new document has been created, either through
         * File->New or File->Open.*/
        bool OnCreate( wxDocument* doc, long flags );

        /** Notifies that the user closes the current document. Either by an explicit
         * closing, or throuh File->New or File->Open.
        */
        bool OnClose( bool deleteWindow = true );

        /** Notification of the UpdateAllViews call in a document.
        */
        void OnUpdate( wxView* sender, wxObject* hint );
    };
}
#endif // _EDITOR_VIEW_H_
