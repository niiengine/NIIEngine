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
#include "ImageGLCanvas.h"
#include "ImagePropertiesPanel.h"
#include "ImageEditorView.h"
#include "ImageEditorFrame.h"
#include "StrHelper.h"
#include "ImageElasticBox.h"
#include "NiiPixelBufferManager.h"
#include "NiiUIWidgetManager.h"
#include "NiiUISchemeManager.h"
#include "NiiEventManager.h"
#include "NiiHash.h"
#include "NiiEngine.h"
#include "app.h"

// wx includes
#include <wx/stdpaths.h>

#define S_(X) #X
#define STRINGIZE(X) S_(X)

using namespace NII;

namespace gsgs
{
    //-----------------------------------------------------------------------
    BEGIN_EVENT_TABLE( ImageGLCanvas, wxGLCanvas )
        EVT_SIZE ( ImageGLCanvas::OnResize )
        EVT_PAINT   ( ImageGLCanvas::OnPaint )
        EVT_ERASE_BACKGROUND( ImageGLCanvas::OnErase )
        EVT_MOTION  ( ImageGLCanvas::OnMouseMotion )
        EVT_LEFT_DOWN   ( ImageGLCanvas::OnLeftDown )
        EVT_LEFT_UP ( ImageGLCanvas::OnLeftUp )
        EVT_RIGHT_DOWN  ( ImageGLCanvas::OnRightDown )
        EVT_RIGHT_UP    ( ImageGLCanvas::OnRightUp )
        EVT_MIDDLE_DOWN  ( ImageGLCanvas::OnMiddleDown )
        EVT_MOUSEWHEEL ( ImageGLCanvas::OnMouseWheel )
        EVT_KEY_DOWN    ( ImageGLCanvas::OnKeyDown )
        EVT_KEY_UP  ( ImageGLCanvas::OnKeyUp )
        EVT_SCROLLWIN( ImageGLCanvas::OnScrollWin )
    END_EVENT_TABLE()
    //-----------------------------------------------------------------------
    ImageGLCanvas::ImageGLCanvas( ImageEditorView* v, wxWindow* parent, const wxPoint& pos, const wxSize& size ) :
        wxGLCanvas( parent, static_cast<const wxGLContext *>( 0 ), -1, pos, size , wxSUNKEN_BORDER | wxVSCROLL | wxHSCROLL ),
        m_view( v ),
        mUI( 0 ),
        mImageWidget( 0 ),
        m_useBigSteps( true ),
        m_zoomFactor( 1.0f ),
        m_realWidth( 0 ),
        m_realHeight( 0 ),
        m_scrollPosX( 0 ),
        m_scrollDocX( 0 ),
        m_scrollPageX( 0 ),
        m_scrollPosY( 0 ),
        m_scrollDocY( 0 ),
        m_scrollPageY( 0 )
    {
        // Init one-time-only stuff
        SetCurrent();

        // we will use a cross-hair cursor
        SetCursor( wxCURSOR_CROSS );

        // Setup the GUI system
        initialiseGUI();
    }
    //-----------------------------------------------------------------------
    ImageGLCanvas::~ImageGLCanvas()
    {
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::setImage( const wxString & name )
    {
        // This could take some time, enter 'busy' state
        wxWindowDisabler wd; wxBusyCursor bc;

        // delete old imageset used for the main image file display
        Nui32 pgid = NII::FastHash(_T("__auto_editor_imageset__"));
        N_Only(PixelBuffer).destroy(pgid);

        // reset all image information
        m_realWidth = 0;
        m_realHeight = 0;
        mImageWidget->setPropertyValue(N_PropertyUIWidget_Image, _T(""));

        // load new image file as an imageset
        if ( !name.IsEmpty() )
        {
            try
            {
                PixelBufferGroup * imageset = N_Only(PixelBuffer).create(pgid, StrHelper::ToNIIString( name ) );

                // set image to our main 'StaticImage' widget (so it is displayed)
                assert ( imageset->isExist ( 0 ) ) ;
                mImageWidget->setPropertyValue(N_PropertyUIWidget_Image, _T("set:__auto_editor_imageset__ image:full_image"));

                // update details we hold about the current source image
                const PixelBuffer * img = imageset->get( 0 ) ;
                PlaneSizef sizeImage = img->getSize();
                m_realWidth = sizeImage.mWidth;
                m_realHeight = sizeImage.mHeight;

                // a nice OpenGL renderer abuse hack to change the filtering on
                // the texture for the image.  Man... it feels good.  Ahhh! :-D
                N_Engine().getRender()->_bindTexture(0, imageset->getTexture(), true);
                //glBindTexture(GL_TEXTURE_2D, tex);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            }
            catch ( ...)
            {
                wxLogError ( wxT( "ImageGLCanvas::setImage - Error"));
            }
        }

        // mute global events so that document is not changed if the regions get
        // resized or moved due to a new image.
        N_Only(Event).setGlobalEnable(false);
        // set main image size.
        mImageWidget->setSize(RelPlaneSizef(RelPosf( 0, m_realWidth ), RelPosf( 0, m_realHeight ) ) );

        // do we even have a doc?
        if ( isDocumentValid() )
        {
            ImageDocument* doc = (ImageDocument*)m_view->GetDocument();

            // for each region window, re-sync it's size with what the doc
            // holds for it.
            for (size_t i = 0; i < mImageWidget->getCount(); ++i)
            {
                Widget * wnd = mImageWidget->get( i );

                if (wnd->getType() == ElasticBox::WidgetTypeName)
                {
                    wxString name( StrHelper::ToWXString( wnd->getName() ) );
                    // extract the correct size from the document
                    wxRect area( doc->getRegionArea( name ) );
                    // set this area for the region
                    setRegionArea( name, area );
                }
            }
        }

        // unmute global events again.
        N_Only(Event).setGlobalEnable(true);

        // update zoom
        computeZoomFactor( 0 );

        Render();
    }

    //-----------------------------------------------------------------------
    void ImageGLCanvas::changeSize( int newWidth, int newHeight )
    {
        // update the OpenGL viewport area
        glViewport(0, 0, static_cast<GLsizei>( newWidth ), static_cast<GLsizei>( newHeight ) );

        // Inform GUI renderer of change
        mUI->setViewSize(N_Engine().getView(), PlaneSizei( newWidth , newHeight ) );
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnResize( wxSizeEvent& event )
    {
        // We need the client size
        int clientWidth, clientHeight;
        GetClientSize( &clientWidth, &clientHeight );

        changeSize( clientWidth, clientHeight );

        updateScrollbars();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::initialiseGUI()
    {
        // Initialise OpenGL renderer and the GUI system
        mUI = N_OnlyPtr(UI);

        // Add support to GUI for our custom 'ElasticBox' widget
        N_Only(Widget).addFactory( &getElasticBoxFactory() );
        N_Only(Widget).addViewFactory( &getElasticBoxWRFactory() );

        // build string where our required files can be found.
        String dataDir(StrHelper::ToNIIString(wxStandardPaths::Get().GetResourcesDir()));
        if ( !dataDir.empty() && ( dataDir[dataDir.length() - 1] != _T('/') ) )
                dataDir += _T('/');

        // on Windows, the datafiles here will be in a 'data' subdir
        #if defined(__WIN32__) || defined (_WIN32)
            dataDir += _T("data/");
        #endif

        // set the data path
        GroupID ggid = N_Only(ResourceScheme).create(0, dataDir);

        // Load GUI scheme used in the editor
        N_Only(UI::Scheme).create( _T("ImagesetEditor.scheme"), ggid);

        // Setup some subscriptions on the global event set
        CommandObj * tempc = N_Only(Event).getCommand(0);
        tempc->bind(ElasticBox::EventSetNorthSouthCursor, this, &ImageGLCanvas::handleRegionNorthSouthCursor);
        tempc->bind(ElasticBox::EventSetEastWestCursor, this, &ImageGLCanvas::handleRegionEastWestCursor);
        tempc->bind(ElasticBox::EventSetNorthEastSouthWestCursor, this, &ImageGLCanvas::handleRegionNorthEastSouthWestCursor);
        tempc->bind(ElasticBox::EventSetNorthWestSouthEastCursor, this, &ImageGLCanvas::handleRegionNorthWestSouthEastCursor);
        tempc->bind(ElasticBox::CursorMoveEvent, this, &ImageGLCanvas::handleRegionMoveCursor);

        // we subscribe these globally since it allows us to quickly 'mute' these
        // events for all regions when we need to.
        tempc->bind(Widget::MoveEvent, this, &ImageGLCanvas::handleRegionModified);
        tempc->bind(Widget::SizeEvent, this, &ImageGLCanvas::handleRegionModified);

        createWindows();

        // set GUI to initial state required by the editor
        reset();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::createWindows()
    {
        // Get window manager which we wil use for a few jobs here.
        WidgetManager & winMgr = N_Only(Widget);

        try
        {
            mImageWidget = static_cast<Window *>(winMgr.create(100001, 0 ));
            mImageWidget->setMaxSize(RelPlaneSizef(RelPosf( 0.0f, 20480.0f ), RelPosf( 0.0f, 20480.0f ) ) );
            mImageWidget->setPos( RelVector2f(RelPosf( 0.0f, 0.0f ), RelPosf( 0.0f, 0.0f ) ) );
            mImageWidget->setSize(RelPlaneSizef(RelPosf( 0, m_realWidth ), RelPosf( 0, m_realHeight ) ) );
            mUI->getSheet(N_Engine().getView())->setActive( mImageWidget );
        }
        catch ( NII::Exception & e )
        {
            wxLogError ( wxT( "%s" ), StrHelper::ToWXString( e.getVerbose() ).c_str() );
        }
    }
    //-----------------------------------------------------------------------
    bool ImageGLCanvas::reset()
    {
        // Cleanup regions
        deleteAllRegions();

        // reset image size to 0
        m_realWidth = 0;
        m_realHeight = 0;
        mImageWidget->setSize(RelPlaneSizef(RelPosf( 0, m_realWidth ), RelPosf( 0, m_realHeight ) ) );

        return true;
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::selectRegion( const wxString& name )
    {
        if ( mImageWidget->isExist( StrHelper::ToNIIString( name ) ) )
        {
            Widget* sel = mImageWidget->get( StrHelper::ToNIIString( name ) );

            if ( !sel->isActive() )
            {
                sel->activate();
                Render();
            }
        }
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::renameRegion( const wxString& currentname, const wxString& newname )
    {
        if ( mImageWidget->isExist( StrHelper::ToNIIString( currentname ) ) )
        {
            Widget* toolbox = mImageWidget->get( StrHelper::ToNIIString( currentname ) );
            toolbox->setName( StrHelper::ToNIIString( newname ) );
            selectRegion( newname );
        }
    }

    //-----------------------------------------------------------------------
    void ImageGLCanvas::addRegion( const wxString& name, const wxRect& dim )
    {
        ElasticBox* toolbox = static_cast<ElasticBox*>(N_Only(Widget).create(100002, 0 ) );
        toolbox->setName(StrHelper::ToNIIString(name));
        float posx = 0, posy = 0, width = 0, height = 0;

        if  ( ( m_realWidth != 0 ) && ( m_realHeight != 0) )
        {
            posx = static_cast<float>( dim.GetX() ) * m_zoomFactor;
            posy = static_cast<float>( dim.GetY() ) * m_zoomFactor;
            width = static_cast<float>( dim.GetWidth() ) * m_zoomFactor;
            height = static_cast<float>( dim.GetHeight() ) * m_zoomFactor;
        }
        // set size and position (all relative co-ords)
        toolbox->setPos( RelVector2f(RelPosf( 0.0f, posx ), RelPosf( 0.0f, posy ) ) );
        toolbox->setSize(RelPlaneSizef(RelPosf( 0.0f, width ), RelPosf( 0.0f, height ) ) );
        // set standard options we use for these ElasticBox widgets
        toolbox->setSizingBorderThickness( 3.0f );
        toolbox->setScaleSnap( m_zoomFactor );
        // Add this new box to the main image window.
        mImageWidget->add( toolbox );

        // subscibe some events...
        toolbox->bind(ElasticBox::ActivateEvent, this, &ImageGLCanvas::handleRegionActivated );

        toolbox->bind(ElasticBox::DeactivateEvent, this,&ImageGLCanvas::handleRegionDeactivated);

        toolbox->bind(ElasticBox::CursorEnterEvent, this, &ImageGLCanvas::handleRegionMouseEnter);

        toolbox->bind(ElasticBox::CursorLeaveEvent, this, &ImageGLCanvas::handleRegionMouseLeave);

        // do this last, so that events handlers are called
        toolbox->activate();

        // Update display.
        Render();
    }

    //-----------------------------------------------------------------------
    void ImageGLCanvas::setRegionArea( const wxString& name, const wxRect& dim )
    {
        if ( mImageWidget->isExist( StrHelper::ToNIIString( name ) ) )
        {
            Widget * win = mImageWidget->get( StrHelper::ToNIIString( name ) );

            float posx = static_cast<float>( dim.GetX() ) * m_zoomFactor;
            float posy = static_cast<float>( dim.GetY() ) * m_zoomFactor;
            float width = static_cast<float>( dim.GetWidth() ) * m_zoomFactor;
            float height = static_cast<float>( dim.GetHeight() ) * m_zoomFactor;

            RelRectf temp(RelPosf(0.0f, posx),
                RelPosf(0.0f, posy),
                RelPosf(0.0f, posx + width),
                RelPosf(0.0f, posy + height));
            win->setRelArea(temp);
        }
    }

    //-----------------------------------------------------------------------
    void ImageGLCanvas::deleteRegion( const wxString& name )
    {
        if ( mImageWidget->isExist( StrHelper::ToNIIString( name ) ) )
        {
            mImageWidget->get( StrHelper::ToNIIString( name ) )->destroy();
            Render();
        }
    }

    //-----------------------------------------------------------------------
    void ImageGLCanvas::deleteAllRegions()
    {
        typedef std::vector<Widget*> DeleteList;
        DeleteList deletables;

        // collect together all the ElasticBoxes attached:
        for (size_t i = 0; i < mImageWidget->getCount(); ++i)
        {
            Widget* wnd = mImageWidget->get( i );

            if (wnd->getType() == ElasticBox::WidgetTypeName)
                deletables.push_back(wnd);
        }

        // delete all the ElasticBoxes
        for (DeleteList::iterator it = deletables.begin(); it != deletables.end(); ++it)
            // Yes, yes, breaking my own rules, I know :-p
            (*it)->destroy();

        Render();
    }

    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnMouseMotion( wxMouseEvent& event )
    {
        this->SetFocus();

        // see if we want to drag the editor image view around
        if ( event.Dragging() && event.MiddleIsDown() )
        {
            // extract mouse location
            wxPoint pt( event.GetPosition() );
            // calculate movement
            wxPoint change = m_dragPoint - pt;
            // update the drag point
            m_dragPoint = pt;

            // set new scrollbar positions
            m_scrollPosX += change.x;
            m_scrollPosY += change.y;
            updateScrollbars();
            // make sure the image position is in sync
            updateImageScrollPosition();
        }
        // not dragging, so do a normal GUI input injection.
        else
        {
            // tell GUI about the event
            N_Only(UI).getSheet(N_Engine().getView())->onCursorMove(event.GetX(), event.GetY());

            // Check this, because we might get this event during document closing
            if ( isDocumentValid() && ( event.GetX() != m_lastMouseX || event.GetY() != m_lastMouseY ) )
            {
                m_lastMouseX = event.GetX();
                m_lastMouseY = event.GetY();

                // updating statusbar with the mouse position on the imageset ( computing offset and zoom)
                float mousePosX = mImageWidget->getActualPosX();
                float mousePosY = mImageWidget->getActualPosY();
                mousePosX = ( m_lastMouseX - mousePosX ) / m_zoomFactor;
                mousePosY = ( m_lastMouseY - mousePosY ) / m_zoomFactor;
                m_view->updateMouseStatusBar(
                    static_cast<int>( mousePosX ),
                    static_cast<int>( mousePosY ) );

                // updating statusbar with the and zoom and the imageset size
                m_view->updateDocStatusBar( m_zoomFactor, m_realWidth, m_realHeight );
            }
        }
        Render();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnMouseWheel( wxMouseEvent& event )
    {
        // Check this, because we might get this event during document closing
        if ( isDocumentValid() )
        {
            int MouseWheelDirection = event.GetWheelRotation();
            computeZoomFactor( MouseWheelDirection );
            m_view->updateDocStatusBar( m_zoomFactor, m_realWidth, m_realHeight );
            Render();
        }
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnLeftDown( wxMouseEvent & event )
    {
        // Check this, because we might get this event during document closing
        if ( isDocumentValid() )
        {
            // tell GUI about the event
            N_Only(UI).getSheet(N_Engine().getView())->onButtonDown( NII_MEDIA::MB_Left );
            Render();
        }
        event.Skip();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnLeftUp( wxMouseEvent & event )
    {
        // Check this, because we might get this event during document closing
        if ( isDocumentValid() )
        {
            // tell GUI about the event
            N_Only(UI).getSheet(N_Engine().getView())->onButtonUp( NII_MEDIA::MB_Left );
            Render();
        }
        event.Skip();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnRightDown( wxMouseEvent& event )
    {
        // Check this, because we might get this event during document closing
        if ( isDocumentValid() )
        {
            // tell GUI about the event
            N_Only(UI).getSheet(N_Engine().getView())->onButtonDown( NII_MEDIA::MB_Right );
            Render();
        }
        event.Skip();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnRightUp( wxMouseEvent & event )
    {
        // Check this, because we might get this event during document closing
        if ( isDocumentValid() )
        {
            // tell GUI about the event
            N_Only(UI).getSheet(N_Engine().getView())->onButtonUp( NII_MEDIA::MB_Right );
            Render();
        }
        event.Skip();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnMiddleDown( wxMouseEvent & event )
    {
        // initialise point where dragging started
        m_dragPoint = event.GetPosition();
        event.Skip();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::computeZoomFactor( int MouseWheelDirection )
    {
        // Logger::getSingleton().logEvent("imagesetSize '" + AppliConfigFile +"'.", Standard);
        if ( MouseWheelDirection > 0 )
            m_zoomFactor += 0.125;
        else if ( MouseWheelDirection < 0 )
            m_zoomFactor -= 0.125;

        // extrem down limit verif
        if ( m_zoomFactor < 0.25 )
            m_zoomFactor = 0.25;

        // extrem up limit verif
        if ( m_zoomFactor > 10.0 )
            m_zoomFactor = 10.0;

        float width = m_zoomFactor * m_realWidth;
        float height = m_zoomFactor * m_realHeight;

        // mute events to stop us from generating a lot of unrequired noise
        N_Only(Event).setGlobalEnable(false);
        // update the size of the main image
        mImageWidget->setSize(RelPlaneSizef( RelPosf(0, width ), RelPosf(0, height ) ) );

        // set snap scale for all defined image regions
        for (size_t i = 0; i < mImageWidget->getCount(); ++i)
        {
            Widget * wnd = mImageWidget->get( i );

            if (wnd->getType() == ElasticBox::WidgetTypeName)
            {
                float old_zoom = static_cast<ElasticBox*>(wnd)->getScaleSnap();
                static_cast<ElasticBox*>(wnd)->setScaleSnap( m_zoomFactor );

                RelRectf area( wnd->getRelArea() );
                // cancel out old zoom scaling
                area.mLeft.mOffset /= old_zoom;
                area.mTop.mOffset /= old_zoom;
                area.mRight.mOffset /= old_zoom;
                area.mBottom.mOffset /= old_zoom;
                // apply new zoom scaling
                area.mLeft.mOffset *= m_zoomFactor;
                area.mTop.mOffset *= m_zoomFactor;
                area.mRight.mOffset *= m_zoomFactor;
                area.mBottom.mOffset *= m_zoomFactor;
                // update window rect for new zoom factor
                wnd->setRelArea(area);
            }
        }

        // allow events to fire again.
        N_Only(Event).setGlobalEnable(true);

        updateScrollbars();
        // Here we invoke our nasty hack to work around a wxMac refresh issue.
        wxGetApp().GetImageEditorFrame()->Refresh();
    }

    //-----------------------------------------------------------------------
    bool ImageGLCanvas::Reset()
    {
        return reset();
    }

    //-----------------------------------------------------------------------
    void ImageGLCanvas::Render()
    {
        SetCurrent();

        // clear colour buffer
        glClear( GL_COLOR_BUFFER_BIT );

        // GUI rendering
        renderImageset();

        glFlush();
        SwapBuffers();
    }
    //-----------------------------------------------------------------------
    bool ImageGLCanvas::isDocumentValid() const
    {
        return (m_view && m_view->GetDocument());
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnPaint( wxPaintEvent& event )
    {
        // We need to do this to avoid a flood of paint events
        wxPaintDC paintDC( this );

        Render();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::renderImageset()
    {
        N_Only(UI).render();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnErase( wxEraseEvent& event )
    {
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnKeyDown( wxKeyEvent& event )
    {
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnKeyUp( wxKeyEvent& event )
    {
        // Control key is used to change selection by keys to small steps (when you're near your target location.)
        if ( event.ControlDown() )
        {
            m_useBigSteps = true;
        }
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::OnScrollWin( wxScrollWinEvent& event )
    {
        int orientation = event.GetOrientation();
        int adjustment, page, range, position;

        if ( orientation == wxVERTICAL )
        {
            page = m_scrollPageY;
            range = m_scrollDocY;
            position = m_scrollPosY;
        }
        else
        {
            page = m_scrollPageX;
            range = m_scrollDocX;
            position = m_scrollPosX;
        }

        wxEventType evtype = event.GetEventType();

        if ( evtype == wxEVT_SCROLLWIN_TOP )
            adjustment = -position;
        else if ( evtype == wxEVT_SCROLLWIN_BOTTOM )
            adjustment = range - page - position;
        else if ( evtype == wxEVT_SCROLLWIN_LINEDOWN )
            adjustment = 1;
        else if ( evtype == wxEVT_SCROLLWIN_LINEUP )
            adjustment = -1;
        else if ( evtype == wxEVT_SCROLLWIN_PAGEDOWN )
            adjustment = page;
        else if ( evtype == wxEVT_SCROLLWIN_PAGEUP )
            adjustment = -page;
        else if ( evtype == wxEVT_SCROLLWIN_THUMBTRACK )
            adjustment = event.GetPosition() - position;
        else if ( evtype == wxEVT_SCROLLWIN_THUMBRELEASE )
            adjustment = event.GetPosition() - position;
        else
            adjustment = 0;

        position += adjustment;

        if ( orientation == wxVERTICAL )
            m_scrollPosY = position;
        else
            m_scrollPosX = position;

        updateScrollbars();
        updateImageScrollPosition();
        event.Skip();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::setBackgroundColour( const wxColour& colour )
    {
        glClearColor(
            colour.Red() / 255.0f,
            colour.Green() / 255.0f,
            colour.Blue() / 255.0f,
            1.0f
        );
        Render();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::updateScrollbars()
    {
        // update internal tracking of this information.
        GetClientSize( &m_scrollPageX, &m_scrollPageY );
        m_scrollDocX = static_cast<int>( m_realWidth * m_zoomFactor );
        m_scrollDocY = static_cast<int>( m_realHeight * m_zoomFactor );

        // update scrolly bars
        SetScrollbar( wxVERTICAL, m_scrollPosY, m_scrollPageY, m_scrollDocY );
        SetScrollbar( wxHORIZONTAL, m_scrollPosX, m_scrollPageX, m_scrollDocX );

        // finally, update our local copies (saves doing our own constraints)
        m_scrollPosX = GetScrollPos( wxHORIZONTAL );
        m_scrollPosY = GetScrollPos( wxVERTICAL );
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::updateImageScrollPosition()
    {
        mImageWidget->setPos( RelVector2f(
            RelPosf( 0, 0 - m_scrollPosX ), RelPosf( 0, 0 - m_scrollPosY ) ) );

        Render();
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::handleRegionActivated( const WidgetEventArgs * e )
    {
        const ActivationEventArgs * args = static_cast<const ActivationEventArgs *>(e);
        ElasticBox* win = static_cast<ElasticBox*>( args->mWidget );

        if ( win )
        {
            win->setAlpha( 0.75f );

            // Check this, because we might get this event during document closing
            if ( isDocumentValid() )
            {
                wxString name = StrHelper::ToWXString( win->getName() );
                m_view->getPropsPanel()->selectRegion( name );
            }
        }
    }

    //-----------------------------------------------------------------------
    void ImageGLCanvas::handleRegionDeactivated( const WidgetEventArgs * e )
    {
        const ActivationEventArgs* args = static_cast<const ActivationEventArgs* >(e);
        ElasticBox* win = static_cast<ElasticBox*>( args->mWidget);

        if ( win )
        {
            win->setAlpha( 0.3f );
        }
    }

    //-----------------------------------------------------------------------
    void ImageGLCanvas::handleRegionMouseEnter( const WidgetEventArgs * e )
    {
        const WidgetEventArgs * wea = static_cast<const WidgetEventArgs*>(e);
        ElasticBox* win = static_cast<ElasticBox*>( wea->mWidget);

        if ( win )
            win->activate();
    }

    //-----------------------------------------------------------------------
    void ImageGLCanvas::handleRegionMouseLeave( const WidgetEventArgs * e )
    {
        const WidgetEventArgs* wea = static_cast<const WidgetEventArgs*>(e);
        ElasticBox* win = static_cast<ElasticBox*>( wea->mWidget);

        if ( win )
            win->deactivate();

        // do this so we do not get stuck with a 'sizing' type cursor
        SetCursor( wxCURSOR_CROSS );
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::handleRegionModified( const WidgetEventArgs * e )
    {
        // Check this, because we might get this event during document closing
        if ( isDocumentValid() )
        {
            Window* wnd = static_cast<Window*>(e->mWidget);

            if ( wnd && wnd->getType() == ElasticBox::WidgetTypeName)
            {
                wxString name = StrHelper::ToWXString( wnd->getName() );
                float posx    = wnd->getPos().x.mOffset / m_zoomFactor;
                float posy    = wnd->getPos().y.mOffset / m_zoomFactor;
                float width   = wnd->getWidth().mOffset / m_zoomFactor;
                float height  = wnd->getHeight().mOffset / m_zoomFactor;

                ImageDocument* document = static_cast<ImageDocument*>( m_view->GetDocument() );
                document->setRegionArea(name, wxRect(
                    static_cast<int>( posx ),
                    static_cast<int>( posy ),
                    static_cast<int>( width ),
                    static_cast<int>( height ) ), true );
            }
        }
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::handleRegionNorthSouthCursor(const EventArgs * e)
    {
        SetCursor( wxCURSOR_SIZENS );
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::handleRegionEastWestCursor(const EventArgs * e)
    {
        SetCursor( wxCURSOR_SIZEWE );
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::handleRegionNorthEastSouthWestCursor(const EventArgs * e)
    {
        SetCursor( wxCURSOR_SIZENESW );
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::handleRegionNorthWestSouthEastCursor(const EventArgs * e)
    {
        SetCursor( wxCURSOR_SIZENWSE );
    }
    //-----------------------------------------------------------------------
    void ImageGLCanvas::handleRegionMoveCursor(const EventArgs * e)
    {
        SetCursor( wxCURSOR_CROSS );
    }
    //-----------------------------------------------------------------------
}