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

#include "NiiGTKWindow.h"
#include "NiiGTKGLSupport.h"
#include "NiiRenderSys.h"
#include "NiiEngine.h"
#include "NiiLogManager.h"

namespace NII
{
    //-----------------------------------------------------------------------
    RenderWidget::RenderWidget(bool useDepthBuffer) : Gtk::GL::DrawingArea()
    {
        Glib::RefPtr<Gdk::GL::Config> glconfig;

        Gdk::GL::ConfigMode mode = Gdk::GL::MODE_RGBA | Gdk::GL::MODE_DOUBLE;
        if (useDepthBuffer)
            mode |= Gdk::GL::MODE_DEPTH;

        glconfig = Gdk::GL::Config::create(mode);
        if (glconfig.is_null())
        {
            LogManager::getOnly().logMessage(_T("[gtk] GLCONFIG BLOWUP"));
        }

        // Inherit GL context from Nii main context
        set_gl_capability(glconfig, GTKGLSupport::getOnly().getMainContext());

        add_events(Gdk::POINTER_MOTION_MASK | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK);
    }
    //-----------------------------------------------------------------------
    // RenderWidget TODO:
    // - resize events et al
    // - Change aspect ratio

    GTKWindow::GTKWindow() : mGtkWindow(0)
    {
        // Should  this move to GTKGLSupport?
            // Gtk::GL::init(0, NULL);
        // Already done in GTKGLSupport

        mWidth = 0;
        mHeight = 0;
    }
    //-----------------------------------------------------------------------
    GTKWindow::~GTKWindow() {}
    //-----------------------------------------------------------------------
    bool GTKWindow::pump_events()
    {
        Gtk::Main *kit = Gtk::Main::instance();
        if (kit->events_pending())
        {
            kit->iteration(false);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    RenderWidget * GTKWindow::getRenderWidget()
    {
        return mRenderWidget;
    }
    //-----------------------------------------------------------------------
    void GTKWindow::setup(const String & name, NCount width, NCount height, bool full,
        const PropertyData * params)
    {
        mName = name;
        mWidth = width;
        mHeight = height;

        int left;
        int top;
        int depthBuffer;

        PropertyData::PropertyList::const_iterator opt, optend = params->getList().end();
        for (opt = params->getList().begin(); opt != optend; ++opt)
        {
            if (opt->first == N_Window_Title)
                title = opt->second;
            else if (opt->first == N_Window_Left)
                StrConv::conv(opt->second, left);
            else if (opt->first == N_Window_Top)
                StrConv::conv(opt->second, top);
            else if (opt->first == N_Window_DepthBuffer)
                StrConv::conv(opt->second, depthBuffer);
        }
        if (!params)
        {
            mGtkWindow = new Gtk::Window();
            mGtkWindow->set_title(mName);

            if (full)
            {
                mFullMode = true;
                mGtkWindow->set_decorated(false);
                mGtkWindow->fullscreen();
            }
            else
            {
                mFullMode = false;
                mGtkWindow->set_default_size(mWidth, mHeight);
                mGtkWindow->move(left, top);
            }
        }
        else
        {
            // If miscParam is not 0, a parent widget has been passed in,
            // we will handle this later on after the widget has been created.
        }

        mRenderWidget = Gtk::manage(new RenderWidget(depthBuffer));
        mRenderWidget->set_size_request(width, height);

        mRenderWidget->signal_delete_event().connect(SigC::slot(*this, &GTKWindow::on_delete_event));
        mRenderWidget->signal_expose_event().connect(SigC::slot(*this, &GTKWindow::on_expose_event));

        if (mGtkWindow)
        {
            mGtkWindow->add(*mRenderWidget);
            mGtkWindow->show_all();
        }
        if (params)
        {
            // Attach it!
            // Note that the parent widget *must* be visible already at this point,
            // or the widget won't get realized in time for the GLinit that follows
            // this call. This is usually the case for Glade generated windows, anyway.
            void * tempptr = 0;
            params->getVoid(N_Window_GTKContainer, tempptr);
            reinterpret_cast<Gtk::Container*>(tempptr)->add(*mRenderWidget);
            mRenderWidget->show();
        }
        //mRenderWidget->realize();
    }
    //-----------------------------------------------------------------------
    void GTKWindow::destroy()
    {
        Engine::getOnly().getRender()->remove(this);
        // We could detach the widget from its parent and destroy it here too,
        // but then again, it is managed so we rely on GTK to destroy it.
        delete mGtkWindow;
        mGtkWindow = 0;
    }
    //-----------------------------------------------------------------------
    void GTKWindow::setFullMode(bool fullScreen, NCount width, NCount height)
    {
        if ((fullScreen) && (!mFullMode))
        {
            mFullMode = true;
            mGtkWindow->fullscreen();
            mRenderWidget->set_size_request(width, height);
        }
        else if ((!fullScreen) && (mFullMode))
        {
            mFullMode = false;
            mGtkWindow->unfullscreen();
            mRenderWidget->set_size_request(width, height);
        }
    }
    //-----------------------------------------------------------------------
    bool GTKWindow::isActive() const
    {
        return mRenderWidget->is_realized();
    }
    //-----------------------------------------------------------------------
    bool GTKWindow::isClosed() const
    {
        return mRenderWidget->is_visible();
    }
    //-----------------------------------------------------------------------
    void GTKWindow::pos(int left, int top)
    {
        if (mGtkWindow)
            mGtkWindow->move(left, top);
    }
    //-----------------------------------------------------------------------
    void GTKWindow::resize(unsigned int width, unsigned int height)
    {
        if (mGtkWindow)
            mGtkWindow->resize(width, height);
    }
    //-----------------------------------------------------------------------
    void GTKWindow::swap(bool waitForVSync)
    {
        Glib::RefPtr<Gdk::GL::Window> glwindow = mRenderWidget->get_gl_window();
        glwindow->swap_buffers();
    }
    //-----------------------------------------------------------------------
    void GTKWindow::fill(FrameObj::FaceType buffer, const Box & src, PixelBlock & dst)
    {
        if ((dst.left < 0) || (dst.right > mWidth) || (dst.top < 0) || (dst.bottom > mHeight) ||
            (dst.front != 0) || (dst.back != 1))
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, _T("Invalid box."));
        }

        if (buffer == FT_Unknow)
        {
            buffer = mFullMode ? FT_Front : FT_Back;
        }

        GLenum format = Nii::GLPixelUtil::getGLOriginFormat(dst.format);
        GLenum type = Nii::GLPixelUtil::getGLOriginDataType(dst.format);

        if ((format == GL_NONE) || (type == 0))
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, _T("Unsupported format."));
        }

        // Switch context if different from current one
        RenderSys * rsys = Engine::getOnly().getRender();
        rsys->_setViewport(this->getViewport(0));

        // Must change the packing to ensure no overruns!
        glPixelStorei(GL_PACK_ALIGNMENT, 1);

        glReadBuffer((buffer == FT_Front) ? GL_FRONT : GL_BACK);
        glReadPixels((GLint)dst.left, (GLint)dst.top,
            (GLsizei)dst.getWidth(), (GLsizei)dst.getHeight(),
            format, type, dst.data);

        // restore default alignment
        glPixelStorei(GL_PACK_ALIGNMENT, 4);
        //vertical flip
        {
            size_t rowSpan = dst.getWidth() * PixelUtil::getNumElemBytes(dst.format);
            size_t height = dst.getHeight();
            uchar *tmpData = new uchar[rowSpan * height];
            uchar *srcRow = (uchar *)dst.data, *tmpRow = tmpData + (height - 1) * rowSpan;

            while (tmpRow >= tmpData)
            {
                memcpy(tmpRow, srcRow, rowSpan);
                srcRow += rowSpan;
                tmpRow -= rowSpan;
            }
            memcpy(dst.data, tmpData, rowSpan * height);

            delete[] tmpData;
        }
    }
    //-----------------------------------------------------------------------
    bool GTKWindow::getPlatformData(const String & name, void * pData)
    {
        if (name == "GTKMMWINDOW")
        {
            Gtk::Window ** win = static_cast<Gtk::Window **>(pData);
            // Oh, the burdens of multiple inheritance
            *win = mGtkWindow;
            return true;
        }
        else if (name == "GTKGLMMWIDGET")
        {
            Gtk::GL::DrawingArea ** widget = static_cast<Gtk::GL::DrawingArea **>(pData);
            *widget = mRenderWidget;
            return true;
        }
        else if (name == "isTexture")
        {
            bool *b = reinterpret_cast<bool *>(pData);
            *b = false;
            return true;
        }
        return ViewWindow::getPlatformData(name, pData);
    }
    //-----------------------------------------------------------------------
    bool GTKWindow::on_delete_event(GdkEventAny * e)
    {
        Engine::getOnly().getRender()->remove(this);
        return false;
    }
    //-----------------------------------------------------------------------
    bool GTKWindow::on_expose_event(GdkEventExpose* e)
    {
        // Window exposed, update interior
        //std::cout << "Window exposed, update interior" << std::endl;
        // TODO: time between events, as expose events can be sent crazily fast
        flush();
        return false;
    }
    //-----------------------------------------------------------------------
}