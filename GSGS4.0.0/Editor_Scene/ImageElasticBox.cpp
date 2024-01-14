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
#include "ImageElasticBox.h"
#include "NiiUIStyle.h"
#include <cmath>

using namespace NII;

namespace gsgs
{
    // Default size for the sizing border (in pixels)
    const float ElasticBox::DefaultSizingBorderSize = 3.0f;
    //-----------------------------------------------------------------------
    // Window factory name
    const FactoryID ElasticBox::WidgetTypeName = N_WIDGET_Custom + 1;//( "ImagesetEditor/ElasticBox" );
    // Event fired when the cursor is to be changed to normal
    N_EVENT(Window, ElasticBox, EventSetNormalCursor,   0);     //( "SetNormalCursor" );
    // Event fired when the cursor is to be changed for moving
    N_EVENT(Window, ElasticBox, EventSetMoveCursor,     1);     //( "SetMoveCursor" );
    // Event fired when the cursor is to be changed for east / west sizing
    N_EVENT(Window, ElasticBox, EventSetEastWestCursor, 2);     //( "SetEastWestCursor" );
    // Event fired when the cursor is to be changed for north / south sizing
    N_EVENT(Window, ElasticBox, EventSetNorthSouthCursor, 3);   //( "SetNorthSouthCursor" );
    // Event fired when the cursor is to be changed for north-east / south-west sizing
    N_EVENT(Window, ElasticBox, EventSetNorthEastSouthWestCursor, 4);  //( "SetNorthEastSouthWestCursor" );
    // Event fired when the cursor is to be changed for north-west / south-east sizing
    N_EVENT(Window, ElasticBox, EventSetNorthWestSouthEastCursor, 5);  //( "SetNorthWestSouthEastCursor" );
    //-----------------------------------------------------------------------
    const WidgetViewlID ElasticBoxView::TypeName = N_VIEW_Custom + 1; //( "ImagesetEditor/ElasticBoxRenderer" );
    //-----------------------------------------------------------------------
    N_PropertyCmdb(ElasticBox, Sizable, N_Property_Custom + 1, "Property to get/set the sizable state of the ElasticBox.  Value is either \"True\" or \"False\".", 
        setSizable, isSizable, "True", false, false, false);

    N_PropertyCmdb(ElasticBox, Movable, N_Property_Custom + 2, "Property to get/set the movable state of the ElasticBox.  Value is either \"True\" or \"False\".",
        setMovable, isMovable, "True", false, false, false);

    N_PropertyCmdb(ElasticBox, SizingBorderThickness, N_Property_Custom + 3, "Property to get/set the thickness of the sizing border on the ElasticBox.  Value is a float specifying the border thickness in pixels.",
        setSizingBorderThickness, getSizingBorderThickness, "3", false, false, false);

    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ElasticBoxView
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ElasticBoxView::ElasticBoxView(WidgetViewlID wsid) :
        WidgetView(wsid, TypeName)
    {
    }
    //-----------------------------------------------------------------------
    void ElasticBoxView::flush()
    {
        // get WidgetLookFeel for the assigned look.
        const Style* wlf = getStyle();
        // render imagery for current state
        wlf->getLayout(mSrc->isActive() ? ActiveState : NormalState).render(mSrc);
    }

    //-----------------------------------------------------------------------
    WidgetView* ElasticBoxWRFactory::create(void)
    {
        return new ElasticBoxView(ElasticBoxView::TypeName);
    }
    //-----------------------------------------------------------------------
    void ElasticBoxWRFactory::destroy(WidgetView* wr)
    {
        delete wr;
    }
    //-----------------------------------------------------------------------
    ElasticBoxWRFactory& getElasticBoxWRFactory()
    {
        static ElasticBoxWRFactory s_factory;
        return s_factory;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ElasticBox
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ElasticBox::ElasticBox(WidgetID wid, FactoryID fid, Container * own, UISheet * sheet,
        const String & name, LangID lid) :
            Window( wid, fid, own, sheet, name, lid ),
            d_sizable( true ),
            d_movable( true ),
            d_beingSized( false ),
            d_beingMoved( false ),
            d_borderThickness( DefaultSizingBorderSize ),
            d_scaleSnap( 1 ),
            d_xSnapPixelsMoved( 0 ),
            d_ySnapPixelsMoved( 0 ),
            d_lastSizingLocation( FrameWindow::RT_None)
    {
        setupCmd(N_CmdObj_Custom + 1);
    }
    //-----------------------------------------------------------------------
    ElasticBox::~ElasticBox( void ){}
    //-----------------------------------------------------------------------
    bool ElasticBox::isSizable( void ) const
    {
        return d_sizable;
    }
    //-----------------------------------------------------------------------
    bool ElasticBox::isMovable( void ) const
    {
        return d_movable;
    }
    //-----------------------------------------------------------------------
    float ElasticBox::getSizingBorderThickness( void ) const
    {
        return d_borderThickness;
    }
    //-----------------------------------------------------------------------
    float ElasticBox::getScaleSnap( void ) const
    {
        return d_scaleSnap;
    }
    //-----------------------------------------------------------------------
    void ElasticBox::setSizable( bool setting )
    {
        d_sizable = setting;

        if ( d_beingSized )
            lost();
    }
    //-----------------------------------------------------------------------
    void ElasticBox::setMovable( bool setting )
    {
        d_movable = setting;

        if ( d_beingMoved )
            lost();
    }
    //-----------------------------------------------------------------------
    void ElasticBox::setSizingBorderThickness( float pixels )
    {
        d_borderThickness = pixels;
    }
    //-----------------------------------------------------------------------
    void ElasticBox::initialiseComponents( void )
    {
        // Nothing to do here at the moment; we have no components.
    }
    //-----------------------------------------------------------------------
    void ElasticBox::setScaleSnap( float value )
    {
        d_scaleSnap = value;
        d_xSnapPixelsMoved = 0.0f;
        d_ySnapPixelsMoved = 0.0f;
    }
    //-----------------------------------------------------------------------
    void ElasticBox::moveLeftEdge( float delta )
    {
        // TODO: We do not currently deal with alignments other than HA_LEFT

        RelRectf area( mArea );
        area.mLeft.mOffset += delta;
        setRelArea( area );
    }
    //-----------------------------------------------------------------------
    void ElasticBox::moveRightEdge( float delta )
    {
        // TODO: We do not currently deal with alignments other than HA_LEFT

        float orgWidth = mPixelSize.mWidth;

        RelRectf area( mArea );
        area.mRight.mOffset += delta;
        setRelArea( area);

        // move the dragging point so mouse remains attached to edge of window
        d_dragPoint.x += mPixelSize.mWidth - orgWidth;
    }
    //-----------------------------------------------------------------------
    void ElasticBox::moveTopEdge( float delta )
    {
        // TODO: We do not currently deal with alignments other than VA_TOP

        RelRectf area( mArea );
        area.mTop.mOffset += delta;
        setRelArea( area );
    }
    //-----------------------------------------------------------------------
    void ElasticBox::moveBottomEdge( float delta )
    {
        // TODO: We do not currently deal with alignments other than VA_TOP

        float orgHeight = mPixelSize.mHeight;

        RelRectf area( mArea );
        area.mBottom.mOffset += delta;
        setRelArea( area);

        // move the dragging point so mouse remains attached to edge of window
        d_dragPoint.y += mPixelSize.mHeight - orgHeight;
    }
    //-----------------------------------------------------------------------
    FrameWindow::ResizeType ElasticBox::getSizingBorderAtPoint( const Vector2f& pt ) const
    {
        Rectf frame( getSizingRect() );

        // we can only size if the frame is enabled and sizing is on
        if ( isSizable() )
        {
            // point must be inside the outer edge
            if ( frame.intersects( pt ) )
            {
                // adjust rect to get inner edge
                frame.mLeft += d_borderThickness;
                frame.mTop += d_borderThickness;
                frame.mRight -= d_borderThickness;
                frame.mBottom -= d_borderThickness;

                // detect which edges we are on
                bool top = ( pt.y < frame.mTop );
                bool bottom = ( pt.y >= frame.mBottom );
                bool left = ( pt.x < frame.mLeft );
                bool right = ( pt.x >= frame.mRight );

                // return appropriate 'ResizeType' value
                if ( top && left )
                    return FrameWindow::RT_TopLeft;
                else if ( top && right )
                    return FrameWindow::RT_TopRight;
                else if ( bottom && left )
                    return FrameWindow::RT_BottomLeft;
                else if ( bottom && right )
                    return FrameWindow::RT_BottomRight;
                else if ( top )
                    return FrameWindow::RT_Top;
                else if ( bottom )
                    return FrameWindow::RT_Bottom;
                else if ( left )
                    return FrameWindow::RT_Left;
                else if ( right )
                    return FrameWindow::RT_Right;
            }
        }

        // deafult: None.
        return FrameWindow::RT_None;
    }
    //-----------------------------------------------------------------------
    bool ElasticBox::isLeftSizingLocation( FrameWindow::ResizeType loc ) const
    {
        return ( ( loc == FrameWindow::RT_Left ) ||
                 ( loc == FrameWindow::RT_TopLeft ) ||
                 ( loc == FrameWindow::RT_BottomLeft ) );
    }
    //-----------------------------------------------------------------------
    bool ElasticBox::isRightSizingLocation( FrameWindow::ResizeType loc ) const
    {
        return ( ( loc == FrameWindow::RT_Right ) ||
                 ( loc == FrameWindow::RT_TopRight ) ||
                 ( loc == FrameWindow::RT_BottomRight ) );
    }
    //-----------------------------------------------------------------------
    bool ElasticBox::isTopSizingLocation( FrameWindow::ResizeType loc ) const
    {
        return ( ( loc == FrameWindow::RT_Top ) ||
                 ( loc == FrameWindow::RT_TopLeft ) ||
                 ( loc == FrameWindow::RT_TopRight ) );
    }
    //-----------------------------------------------------------------------
    bool ElasticBox::isBottomSizingLocation( FrameWindow::ResizeType loc ) const
    {
        return ( ( loc == FrameWindow::RT_Bottom ) ||
                 ( loc == FrameWindow::RT_BottomLeft ) ||
                 ( loc == FrameWindow::RT_BottomRight ) );
    }
    //-----------------------------------------------------------------------
    Rectf ElasticBox::getSizingRect( void ) const
    {
        return Rectf( 0, 0, mPixelSize.mWidth, mPixelSize.mHeight);
    }
    //-----------------------------------------------------------------------
    void ElasticBox::doSizingUpdate( const Vector2f& mousePosition )
    {
        // calculate deltas to use for sizing
        float delta_x = calculateSnappedXDelta( mousePosition.x - d_dragPoint.x );
        float delta_y = calculateSnappedYDelta( mousePosition.y - d_dragPoint.y );

        // sizing for left or right edges
        if ( isLeftSizingLocation( d_lastSizingLocation ) )
            moveLeftEdge( delta_x );
        else if ( isRightSizingLocation( d_lastSizingLocation ) )
            moveRightEdge( delta_x );

        // sizing for top or bottom edges
        if ( isTopSizingLocation( d_lastSizingLocation ) )
            moveTopEdge( delta_y );
        else if ( isBottomSizingLocation( d_lastSizingLocation ) )
            moveBottomEdge( delta_y );
    }
    //-----------------------------------------------------------------------
    void ElasticBox::doMovingUpdate( const Vector2f& mousePosition )
    {
        // calculate amount that window has been moved in pixels
        float delta_x = calculateSnappedXDelta( mousePosition.x - d_dragPoint.x );
        float delta_y = calculateSnappedYDelta( mousePosition.y - d_dragPoint.y );

        // calculate offset
        RelVector2f offset(
            RelPosf(0, delta_x),
            RelPosf(0, delta_y) );

        RelRectf area(mArea);
        area.offset( offset );

        // update position
        setRelArea( area );
    }
    //-----------------------------------------------------------------------
    void ElasticBox::callCursorChangeHandler( const Vector2f& mousePosition )
    {
        WidgetEventArgs args( this );

        if ( isSizable() )
        {
            switch ( getSizingBorderAtPoint( mousePosition ) )
            {
            case FrameWindow::RT_Top:
            case FrameWindow::RT_Bottom:
                onSetNorthSouthCursor( &args );
                return ;

            case FrameWindow::RT_Left:
            case FrameWindow::RT_Right:
                onSetEastWestCursor(&args );
                return ;

            case FrameWindow::RT_TopLeft:
            case FrameWindow::RT_BottomRight:
                onSetNorthWestSouthEastCursor(&args );
                return ;

            case FrameWindow::RT_TopRight:
            case FrameWindow::RT_BottomLeft:
                onSetNorthEastSouthWestCursor(&args );
                return ;
            }
        }

        if ( isMovable() )
            onSetMoveCursor(&args );
        else
            onSetNormalCursor(&args );
    }
    //-----------------------------------------------------------------------
    float ElasticBox::calculateSnappedXDelta(float delta)
    {
        // update the drag point for part moves
        d_dragPoint.x += delta;
        // track the extent of part moves made so far.
        d_xSnapPixelsMoved += delta;

        // have we moved the equivalent of 1 whole pixel at this scale setting?
        if (std::fabs(d_xSnapPixelsMoved) > d_scaleSnap)
        {
            // this is the number of whole pixels moved if we were at scale level 1
            delta = (static_cast<int>(d_xSnapPixelsMoved) / static_cast<int>(d_scaleSnap)) * d_scaleSnap;
            // reset drag point now we are actually moving the window
            d_dragPoint.x -= delta;
            // retain any partial pixels moved
            d_xSnapPixelsMoved -= delta;
        }
        else
        {
            delta = 0.0f;
        }

        return delta;
    }
    //-----------------------------------------------------------------------
    float ElasticBox::calculateSnappedYDelta(float delta)
    {
        // update the drag point for part moves
        d_dragPoint.y += delta;
        // track the extent of part moves made so far.
        d_ySnapPixelsMoved += delta;

        // have we moved the equivalent of 1 whole pixel at this scale setting?
        if (std::fabs(d_ySnapPixelsMoved) > d_scaleSnap)
        {
            // this is the number of whole pixels moved if we were at scale level 1
            delta = (static_cast<int>(d_ySnapPixelsMoved) / static_cast<int>(d_scaleSnap)) * d_scaleSnap;
            // reset drag point now we are actually moving the window
            d_dragPoint.y -= delta;
            // retain any partial pixels moved
            d_ySnapPixelsMoved -= delta;
        }
        else
        {
            delta = 0.0f;
        }

        return delta;
    }
    //-----------------------------------------------------------------------
    void ElasticBox::onSetNormalCursor( const WidgetEventArgs * e )
    {
        signal( EventSetNormalCursor, e );
    }
    //-----------------------------------------------------------------------
    void ElasticBox::onSetMoveCursor( const WidgetEventArgs * e )
    {
        signal( EventSetMoveCursor, e );
    }
    //-----------------------------------------------------------------------
    void ElasticBox::onSetEastWestCursor(const WidgetEventArgs * e )
    {
        signal( EventSetEastWestCursor, e );
    }
    //-----------------------------------------------------------------------
    void ElasticBox::onSetNorthSouthCursor( const WidgetEventArgs * e )
    {
        signal( EventSetNorthSouthCursor, e );
    }
    //-----------------------------------------------------------------------
    void ElasticBox::onSetNorthEastSouthWestCursor( const WidgetEventArgs * e )
    {
        signal( EventSetNorthEastSouthWestCursor, e );
    }
    //-----------------------------------------------------------------------
    void ElasticBox::onSetNorthWestSouthEastCursor( const WidgetEventArgs * e )
    {
        signal( EventSetNorthWestSouthEastCursor, e );
    }
    //-----------------------------------------------------------------------
    void ElasticBox::onCursorMove(const CursorEventArgs * e )
    {
        // Do default processing and fire off event.
        Window::onCursorMove( e );

        // if we are not the window containing the mouse, do nothing more
        if (mSheet->getCursorOver() != this )
            return ;

        Vector2f localMousePos(e->mLocalX - getActualPosX(), e->mLocalY - getActualPosY());

        if ( d_beingSized )
            doSizingUpdate( localMousePos );
        else if ( d_beingMoved )
            doMovingUpdate( localMousePos );
        else
            callCursorChangeHandler( localMousePos );
    }
    //-----------------------------------------------------------------------
    void ElasticBox::onButtonDown(const CursorEventArgs * e )
    {
        // call default handler to fire event
        Window::onButtonDown( e );

        // we're only really interested in the left button
        if ( e->mMouseArg.mButton == MB_Left )
        {
            // get position of mouse as co-ordinates local to this window.
            Vector2f localPos(e->mLocalX - getActualPosX(), e->mLocalY - getActualPosY());

            // get sizing border location of the mouse
            d_lastSizingLocation = getSizingBorderAtPoint( localPos );

            if ( isSizable() && ( d_lastSizingLocation != FrameWindow::RT_None ) )
            {
                // ensure all inputs come to us for now
                if ( focus() )
                {
                    // setup the 'dragging' state variables
                    d_beingSized = true;
                    d_dragPoint = localPos;
                }
            }
            else if ( isMovable() && ( d_lastSizingLocation == FrameWindow::RT_None ) )
            {
                // ensure all inputs come to us for now
                if ( focus() )
                {
                    // setup the 'dragging' state variables
                    d_beingMoved = true;
                    d_dragPoint = localPos;
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ElasticBox::onButtonUp(const CursorEventArgs * e )
    {
        // call default handler to fire event
        Window::onButtonUp( e );

        if ( e->mMouseArg.mButton == MB_Left && isFocus() )
        {
            // release our capture on the input data
            // (eventually this causes drag state reset)
            lost();
        }

    }
    //-----------------------------------------------------------------------
    void ElasticBox::onLost( const WidgetEventArgs * e )
    {
        Window::onLost( e );

        // reset drag states
        d_beingSized = false;
        d_beingMoved = false;
        d_xSnapPixelsMoved = 0;
        d_ySnapPixelsMoved = 0;
    }
    //-----------------------------------------------------------------------
    bool ElasticBox::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new Sizable());
        dest->add(N_new Movable());
        dest->add(N_new SizingBorderThickness());
        return true;
    }
    //-----------------------------------------------------------------------
    ElasticBoxFactory& getElasticBoxFactory()
    {
        static ElasticBoxFactory s_factory;
        return s_factory;
    }
    //-----------------------------------------------------------------------
}