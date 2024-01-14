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

#ifndef _IMAGE_ELASTIC_BOX_H_
#define _IMAGE_ELASTIC_BOX_H_

#include "gsgsPreInclude.h"
#include "NiiPropertyObj.h"
#include "NiiUIWindow.h"
#include "NiiVector2.h"

using namespace NII::UI;
using namespace NII;
namespace gsgs
{
    /*!
    \brief
        Window renderer for ElasticBox windows.

        LookNFeel should provide the following:

        States:
            - Active       - rendering for active state.
            - Inactive     - rendering for inactive state.
    */
    class ElasticBoxView : public WidgetView
    {
    public:
        //! Type name for the WindowRenderer
        static const WidgetViewlID TypeName;

        ElasticBoxView(WidgetViewlID wsid);

        void flush();
    };

    /*! Factory for ElasticBox window renderer type
    */
    class ElasticBoxWRFactory : public WidgetViewFactory
    {
    public:
        FactoryID getID() const     { return ElasticBoxView::TypeName; }
        WidgetView* create( void );
        void destroy(WidgetView* wr );
    };

    ElasticBoxWRFactory& getElasticBoxWRFactory();

    /*! Class for a bounding box widget.

        The elastic box widget is a custom widget for the ImagesetEditor; it
        provides a widget that can be drag sized and drag moved.  The purpose of
        the widget is to provide a means to visually define some region.

        ElasticBox is based somewhat on FrameWindow, from the core GUI
        library.
    */
    class ElasticBox : public Window
    {
    public:
        static const FactoryID WidgetTypeName;
        //! Event fired when the cursor is to be changed to normal
        static const EventID EventSetNormalCursor;
        //! Event fired when the cursor is to be changed for moving
        static const EventID EventSetMoveCursor;
        //! Event fired when the cursor is to be changed for east / west sizing
        static const EventID EventSetEastWestCursor;
        //! Event fired when the cursor is to be changed for north / south sizing
        static const EventID EventSetNorthSouthCursor;
        //! Event fired when the cursor is to be changed for north-east / south-west sizing
        static const EventID EventSetNorthEastSouthWestCursor;
        //! Event fired when the cursor is to be changed for north-west / south-east sizing
        static const EventID EventSetNorthWestSouthEastCursor;
        //! Default size for the sizing border (in pixels)
        static const float DefaultSizingBorderSize;

        ElasticBox(WidgetID wid, FactoryID fid, Container * own, UISheet * sheet = 0,
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);

        virtual ~ElasticBox( void );

        /*! Return whether this window is sizable (by the user).

        \return
            - true if the window can be sized
            - false if the window can not be sized
        */
        bool isSizable( void ) const;

        /*! Return whether the window is movable (by the user).

        \return
            - true if the window can be moved around
            - false if the window is fixed in position
        */
        bool isMovable( void ) const;

        /*!
        \brief
            Return the thickness of the sizing border in pixels.

        \return
            float value describing the thickness of the sizing border in pixels.
        */
        float getSizingBorderThickness( void ) const;

        /*!
        \brief
            Returns the current scale 'snap' value.

            The scale snap value is used to limit sizing and moving operations to
            pixel multiples of the scale snap value (useful when operating upon a
            scaled image for example).  Defaults to 1.

        \return
            float value describing the current scale snap value.
        */
        float getScaleSnap( void ) const;

        /*!
        \brief
            Enables or disables sizing (by the user) for this window.

        \param setting
            - true to enable sizing
            - false to disable sizing.

        \return
            Nothing
        */
        void setSizable( bool setting );

        /*!
        \brief
            Enables or disables moving (by the user) for this window.

        \param setting
            - true to enable moving of the window
            - false to disable moving of the window

        \return
            Nothing.
        */
        void setMovable( bool setting );

        /*!
        \brief
            Set the size of the sizing border for this window in pixels.

        \param pixels
            float value specifying the thickness for the sizing border in pixels.

        \return
            Nothing.
        */
        void setSizingBorderThickness( float pixels );

        /*!
        \brief
            Sets the current scale 'snap' value.

            The scale snap value is used to limit sizing and moving operations to
            pixel multiples of the scale snap value (useful when operating upon a
            scaled image for example).  Defaults to 1.

        \param value
            float value describing the scale snap value to be used.

        \return
            Nothing
        */
        void setScaleSnap( float value );

        // Base class implementations and overrides
        virtual void initialiseComponents( void );

    protected:
        /*!
        \brief
            move the window's left edge by 'delta'.  The rest of the window does
            not move, thus this changes the size of the Window.

        \param delta
            float value that specifies the amount to move the window edge, and in
            which direction.  Positive values make window smaller.
        */
        void moveLeftEdge( float delta );

        /*!
        \brief
            move the window's right edge by 'delta'.  The rest of the window does
            not move, thus this changes the size of the Window.

        \param delta
            float value that specifies the amount to move the window edge, and in
            which direction.  Positive values make window larger.
        */
        void moveRightEdge( float delta );

        /*!
        \brief
            move the window's top edge by 'delta'.  The rest of the window does
            notmove, thus this changes the size of the Window.

        \param delta
            float value that specifies the amount to move the window edge, and in
            which direction.  Positive values make window smaller.
        */
        void moveTopEdge( float delta );

        /*!
        \brief
            move the window's bottom edge by 'delta'.  The rest of the window does
            not move, thus this changes the size of the Window.

        \param delta
            float value that specifies the amount to move the window edge, and in
            which direction.  Positive values make window larger.
        */
        void moveBottomEdge( float delta );

        /*!
        \brief
            check local pixel co-ordinate point 'pt' and return one of the
            FrameWindow::ResizeType enumerated values depending where
            the point falls on the sizing border.

        \param pt
            Vector2f object describing, in pixels, the window relative offset to check.

        \return
            One of the FrameWindow::ResizeType enumerated values that
            describe which part of the sizing border that \a pt corresponded to,
            if any.
        */
        FrameWindow::ResizeType getSizingBorderAtPoint( const Vector2f& pt ) const;

        /*!
        \brief
            return true if given FrameWindow::ResizeType is on left edge.

        \param loc
            FrameWindow::ResizeType value to be checked.

        \return
            - true if \a loc is on the left edge.
            - false if \a loc is not on the left edge.
        */
        bool isLeftSizingLocation( FrameWindow::ResizeType loc ) const;

        /*!
        \brief
            return true if given FrameWindow::ResizeType is on right edge.

        \param loc
            FrameWindow::ResizeType value to be checked.

        \return
            - true if \a loc is on the right edge.
            - false if \a loc is not on the right edge.
        */
        bool isRightSizingLocation( FrameWindow::ResizeType loc ) const;

        /*!
        \brief
            return true if given FrameWindow::ResizeType is on top edge.

        \param loc
            FrameWindow::ResizeType value to be checked.

        \return
            - true if \a loc is on the top edge.
            - false if \a loc is not on the top edge.
        */
        bool isTopSizingLocation( FrameWindow::ResizeType loc ) const;

        /*!
        \brief
            return true if given FrameWindow::ResizeType is on bottom edge.

        \param loc
            FrameWindow::ResizeType value to be checked.

        \return
            - true if \a loc is on the bottom edge.
            - false if \a loc is not on the bottom edge.
        */
        bool isBottomSizingLocation( FrameWindow::ResizeType loc ) const;

        /*!
        \brief
            Return a Rect that describes, in window relative pixel co-ordinates,
            the outer edge of the sizing area for this window.
        */
        Rectf getSizingRect( void ) const;

        /*!
        \brief
            Handles the update logic for sizing operations.

        \param mousePosition
            Latest mouse position in window local pixel co-ordinates.
        */
        void doSizingUpdate( const Vector2f& mousePosition );

        /*!
        \brief
            Handles the update logic for moving operations.

        \param mousePosition
            Latest mouse position in window local pixel co-ordinates.
        */
        void doMovingUpdate( const Vector2f& mousePosition );

        /*!
        \brief
            Call appropriate handler to process cursor change requirements.

            The base class handlers provided here just fire events.
        */
        void callCursorChangeHandler( const Vector2f& mousePosition );

        /*!
        \brief
            Helper to calculate snapped deltas on the x axis.
        */
        float calculateSnappedXDelta(float delta);

        /*!
        \brief
            Helper to calculate snapped deltas on the y axis.
        */
        float calculateSnappedYDelta(float delta);

        /*!
        \brief
            Handler called when the cursor is to be changed back to normal.
        */
        virtual void onSetNormalCursor( const WidgetEventArgs * e );

        /*!
        \brief
            Handler called when the cursor is to be changed for movement.
        */
        virtual void onSetMoveCursor(const WidgetEventArgs * e );

        /*!
        \brief
            Handler called when the cursor is to be changed for east / west sizing.
        */
        virtual void onSetEastWestCursor(const WidgetEventArgs * e );

        /*!
        \brief
            Handler called when the cursor is to be changed for north / south sizing.
        */
        virtual void onSetNorthSouthCursor(const WidgetEventArgs * e );

        /*!
        \brief
            Handler called when the cursor is to be changed for north-east / south-west sizing.
        */
        virtual void onSetNorthEastSouthWestCursor(const WidgetEventArgs * e );

        /*!
        \brief
            Handler called when the cursor is to be changed for north-west / south-east sizing.
        */
        virtual void onSetNorthWestSouthEastCursor(const WidgetEventArgs * e );

        // Overridden event processing
        virtual void onCursorMove(const CursorEventArgs * e );
        virtual void onButtonDown(const CursorEventArgs * e );
        virtual void onButtonUp(const CursorEventArgs * e );
        virtual void onLost(const WidgetEventArgs* e );

        // Implementation Data
        //! true if window is user-sizable.
        bool d_sizable;
        //! true if the window is user-movable
        bool d_movable;
        //! true if window is currently being sized.
        bool d_beingSized;
        //! true if window is currently being moved.
        bool d_beingMoved;
        //! thickness of the sizing border around this window
        float d_borderThickness;
        //! current scale snap value
        float d_scaleSnap;
        //! tracks x pixels currently dragged - used for scale snap implementation.
        float d_xSnapPixelsMoved; 
        //! tracks y pixels currently dragged - used for scale snap implementation.
        float d_ySnapPixelsMoved; 
        //! point window is being dragged at.
        Vector2f d_dragPoint;
        /** Tracks last sizing location we knew about.  Required because at high
         * zoom - due to 'snap' - we're likely to move the mouse off the edge of the
         * window which will then not show up as a sizing location, causing very
         * rough response.
        */
        FrameWindow::ResizeType d_lastSizingLocation;

    private:
        bool initCmd(PropertyCmdSet * dest);
    };

    /* WindowFactory class for ElasticBox objects.
    */
    class ElasticBoxFactory : public WidgetFactory
    {
    public:
        FactoryID getID() const { return ElasticBox::WidgetTypeName; }

        Widget * create(WidgetID wid, Container * own)
        { return new ElasticBox(wid, ElasticBox::WidgetTypeName, own); }

        void destroy(Widget * obj)
        { delete obj; }
    };

    ElasticBoxFactory& getElasticBoxFactory();
}
#endif // _ELASTIC_BOX_WIDGET_H_
