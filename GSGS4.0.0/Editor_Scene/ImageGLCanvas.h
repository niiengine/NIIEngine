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

#ifndef _IMAGE_EDITOR_CANVAS_H_
#define _IMAGE_EDITOR_CANVAS_H_

#include <wx/wx.h> 

#include "wx/glcanvas.h"
#include "ImageDocument.h"
#include "NiiUIManager.h"

using namespace NII;

namespace gsgs
{
    class ImageEditorView;

    /** Provides OpenGL output. This canvas is the actual visible part of our "view"
    * on the "document". The GUI system must be initialised when rendering, which is verified.
    */

    class ImageGLCanvas : public wxGLCanvas
    {
    public:
        /** Constructor.*/
        ImageGLCanvas( ImageEditorView* view, wxWindow* parent, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize );

        /** Destructor.*/
        ~ImageGLCanvas();

        /** Updates the openGL viewport setup.*/
        void changeSize( int newWidth, int newHeight );

        void setBackgroundColour( const wxColour& colour );
        void setImage( const wxString& name );

        // region manipulation
        void selectRegion( const wxString& name );
        void renameRegion( const wxString& currentname, const wxString& newname );
        void addRegion( const wxString& name, const wxRect& dim );
        void setRegionArea( const wxString& name, const wxRect& dim );
        void deleteRegion( const wxString& name );
        void deleteAllRegions();

        void computeZoomFactor( int MouseWheelDirection );

        void setView ( ImageEditorView* aView )
        {
            m_view = aView;
        }

        /** Forces a reset of cegui.*/
        bool Reset();

        /** Forces a re-draw.*/
        void Render();

        /** Returns whether the document is still valid or not.*/
        bool isDocumentValid() const;

    private:
        /** The view to which we are attached.*/
        ImageEditorView * m_view;

        /** Pointer to cegui singleton.*/
        UIManager * mUI;

        // GUI widgets
        Window * mImageWidget;

        /** True when big steps should be used when moving selection by key.*/
        bool m_useBigSteps;

        //! Used to track drag motion.
        wxPoint m_dragPoint;

        /** Current document zoom. float because it's as relative value to original size egal 1.*/
        float m_zoomFactor ;

        /** Current canvas width. Used alot so it's cached.*/
        float m_realWidth;

        /** Current canvas height. Used alot so it's cached.*/
        float m_realHeight;

        /** Last known mouseX position.*/
        long m_lastMouseX;

        /** Last known mouseY position.*/
        long m_lastMouseY;

        // Here we have scroll state variables; required because wxWidgets has
        // different behavior on different platforms for wxWindow scrollbars.
        // (wxGTK automated them without intervention, wxMSW did not).
        //! current horizontal scroll position
        int m_scrollPosX;
        //! horizontal size of the 'document' (zoomed image width)
        int m_scrollDocX;
        //! horizontal 'page' size (width of the canvas widow)
        int m_scrollPageX;
        //! current vertical scroll position
        int m_scrollPosY;
        //! vertical size of the 'document' (zoomed image height)
        int m_scrollDocY;
        //! vertical 'page' size (height of the canvas widow)
        int m_scrollPageY;

        /** Renders the current imageset.*/
        void renderImageset();

        /** Initialises the GUI core + renderer. We don't cleanup the frame when
        * new view is spawned, so this is a good place to put the GUI functionality.*/
        void initialiseGUI();

        bool reset();
        void createWindows();

        //! Update the scrollbars
        void updateScrollbars();
        //! update scroll position of the image based on scrollbar values.
        void updateImageScrollPosition();

        // handlers which we use to trigger cursor changes.
        void handleRegionNorthSouthCursor(const EventArgs* e);
        void handleRegionEastWestCursor(const EventArgs * e);
        void handleRegionNorthEastSouthWestCursor(const EventArgs* e);
        void handleRegionNorthWestSouthEastCursor(const EventArgs* e);
        void handleRegionMoveCursor(const EventArgs* e);

        //! handler used to auto activate image regions
        void handleRegionMouseEnter(const WidgetEventArgs* e);
        //! handler used to auto deactivate image regions
        void handleRegionMouseLeave(const WidgetEventArgs* e);
        //! handler used to react when a region is activated
        void handleRegionActivated( const WidgetEventArgs* e );
        //! handler used to react when a region is deactivated
        void handleRegionDeactivated( const WidgetEventArgs* e );
        //! handler used to update properties panel when region changes
        void handleRegionModified( const WidgetEventArgs* e );

        //
        // wxWidgets bits below
        //
        /** Handled wxSizeEvent. Propagate the new resolution to the gui renderer.*/
        void OnResize( wxSizeEvent& event );

        /** Event handler: notifies us that the canvas needs a repaint. We render the current imageset.*/
        void OnPaint( wxPaintEvent& event );

        /** Event handler: notifies us that the canvas needs a refresh of the background.
        * a good moment to draw the current background, if any.*/
        void OnErase( wxEraseEvent& event );

        /** Event handler: we handle mouse input here.*/
        void OnMouseMotion( wxMouseEvent& event );

        /** Event handler: we handle mouse input here.*/
        void OnLeftDown( wxMouseEvent& event );

        /** Event handler: we handle mouse input here.*/
        void OnLeftUp( wxMouseEvent& event );

        /** Event handler: we handle mouse input here.*/
        void OnRightDown( wxMouseEvent& event );

        /** Event handler: we handle mouse input here.*/
        void OnRightUp( wxMouseEvent& event );

        /** Event handler: we handle mouse input here.*/
        void OnMiddleDown( wxMouseEvent& event );

        /** Event handler: we handle mouse input here.*/
        //void OnMiddleUp( wxMouseEvent& event );

        /** Event handler: we handle mouse input here.*/
        void OnMouseWheel( wxMouseEvent& event );

        /** Event handler: handles key-down events. We use it to move the current selection.*/
        void OnKeyDown( wxKeyEvent& event );

        /** Event handler: stops moving the current selection.*/
        void OnKeyUp( wxKeyEvent& event );

        /** Event handler: scrollbar changes */
        void OnScrollWin( wxScrollWinEvent& event );

        DECLARE_EVENT_TABLE()
    };
}
#endif // _EDITOR_CANVAS_H_
