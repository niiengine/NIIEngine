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

#ifndef _NII_GTKWINDOW_H_
#define _NII_GTKWINDOW_H_

#include "NiiViewWindow.h"

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkglmm.h>

#include <GL/gl.h>

namespace NII
{
    class RenderWidget : public Gtk::GL::DrawingArea
    {
    public:
        RenderWidget(bool depthBuffer);
    };

    class GTKWindow : public ViewWindow, public SigC::Object
    {
    public:
        GTKWindow();
        ~GTKWindow();

        /**
        * Pump the main loop to actually generate events
        * @returns false when there are no events left to pump
        */
        bool pump_events();

        /**
        * Get the actual widget that is housing OGRE's GL view.
        */
        RenderWidget * getRenderWidget();

        /// @copydetails ViewWindow::setup
        void setup(const String & name, NCount wpixel, NCount hpixel, bool full,
            const PropertyData * params);

        /// @copydetails ViewWindow::setFullMode
        void setFullMode(bool fullScreen, NCount width, NCount height);

        /// @copydetails ViewWindow::destroy
        void destroy(void);

        /// @copydetails ViewWindow::isActive
        bool isActive() const;

        /// @copydetails ViewWindow::isClosed
        bool isClosed() const;

        /// @copydetails ViewWindow::
        void pos(NIIi left, NIIi top);

        /// @copydetails ViewWindow::resize
        void resize(NCount width, NCount height);

        /// @copydetails FrameObj::swap
        void swap(bool waitForVSync);

        /// @copydetails FrameObj::fill
        void fill(FrameObj::FaceType buffer, const Box & src, PixelBlock & dst);

        /** @copydetails FrameObj::getPlatformData
         * GTKMMWINDOW		The Gtk::Window instance (Rendering window)
         * GTKGLMMWIDGET	The Gtk::GL::DrawingArea instance (Ogre widget)
         */
        bool getPlatformData(const String & name, void * pData) const;
    protected:
        bool on_delete_event(GdkEventAny * event);
        bool on_expose_event(GdkEventExpose * event);
    private:
        Gtk::Window * mGtkWindow;
        RenderWidget * mRenderWidget;
    };
};
#endif