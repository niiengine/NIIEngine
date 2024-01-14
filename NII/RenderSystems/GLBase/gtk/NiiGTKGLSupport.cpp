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

#include "NiiGTKGLSupport.h"
#include "NiiGTKWindow.h"
#include "NiiLogManager.h"
#include "NiiException.h"
#include "NiiStringConverter.h"

using namespace NII;
    //-----------------------------------------------------------------------
    template<> GTKGLSupport* Singleton<GTKGLSupport>::mOnly = 0;
    GTKGLSupport* GTKGLSupport::getOnlyPtr(void)
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    GTKGLSupport& GTKGLSupport::getOnly(void)
    {  
        N_assert(mOnly);  
        return (*mOnly);  
    }
    //-----------------------------------------------------------------------
    GTKGLSupport::GTKGLSupport() : 
        _kit(0, NULL),
        _context_ref(0)
    {
        Gtk::GL::init(0, NULL);
        _main_context = 0;
        _main_window = 0;
    }
    //-----------------------------------------------------------------------
    void GTKGLSupport::addConfig()
    {
        SysOption optFullScreen;
        SysOption optVideoMode;

         // FS setting possiblities
        optFullScreen.name = "Full Screen";
        optFullScreen.mReserve.push_back("Yes");
        optFullScreen.mReserve.push_back("No");
        optFullScreen.mValue = "No";
        optFullScreen.mReserveEnable = true;
     
        // Video mode possiblities
        // XXX Actually do this
        optVideoMode.name = "Video Mode";
        optVideoMode.mReserveEnable = true;
        optVideoMode.mReserve.push_back("640 x 480");
        optVideoMode.mReserve.push_back("800 x 600");
        optVideoMode.mReserve.push_back("1024 x 768");
        optVideoMode.mReserve.push_back("1280 x 1024");

        optVideoMode.mValue = "800 x 600";

        mOptions[optFullScreen.name] = optFullScreen;
        mOptions[optVideoMode.name] = optVideoMode;
    }
    //-----------------------------------------------------------------------        
    String GTKGLSupport::checkConfig()
    {
        return String("");
    }
    //-----------------------------------------------------------------------
    ViewWindow * GTKGLSupport::createWindow(bool autoCreateWindow, 
        GLRenderSystem * renderSystem, const String & windowTitle)
    {
        if(autoCreateWindow)
        {
            SysOptionList::iterator opt = mOptions.find("Full Screen");
            if (opt == mOptions.end())
                N_EXCEPT(RenderingAPI, _T("Can't find full screen options!"));
            bool fullscreen = (opt->second.mValue == "Yes");
     
            opt = mOptions.find("Video Mode");
            if (opt == mOptions.end())
                N_EXCEPT(RenderingAPI, _T("Can't find video mode options!"));
            String val = opt->second.mValue;
            String::size_type pos = val.find('x');
            if (pos == String::npos)
                N_EXCEPT(RenderingAPI, _T("Invalid Video Mode provided"));
     
            unsigned int w = StringConverter::parseUnsignedInt(val.substr(0, pos));
            unsigned int h = StringConverter::parseUnsignedInt(val.substr(pos + 1));
     
            return renderSystem->createWindow(windowTitle, w, h, 32, fullscreen);
        }
        else
        {
            // XXX What is the else?
            return NULL;
        }
    }
    //-----------------------------------------------------------------------
    ViewWindow * GTKGLSupport::newWindow(const String & name, NCount width, NCount height, 
        bool full, const PropertyData * custom)
    {
        //ViewWindow * parentWindowHandle;
        GTKWindow * window = new GTKWindow();
        window->setup(name, width, height, full, custom);

        // Copy some important information for future reference, for example
        // for when the context is needed
        if(!_main_context)
            _main_context = window->getRenderWidget()->get_gl_context();
        if(!_main_window)
            _main_window = window->getRenderWidget()->get_gl_window();

        return window;
    }
    //-----------------------------------------------------------------------
    void GTKGLSupport::start()
    {
        LogManager::getOnly().logMessage(
            "******************************\n"
            _T("*** Starting GTK Subsystem ***\n")
            "******************************");

    }
    //-----------------------------------------------------------------------     
    void GTKGLSupport::stop()
    {
        LogManager::getOnly().logMessage(
            "******************************\n"
            _T("*** Stopping GTK Subsystem ***\n")
            "******************************");
    }
    //-----------------------------------------------------------------------
    void GTKGLSupport::begin_context(FrameObj *_target)
    {
        // Support nested contexts, in which case.. nothing happens
        ++_context_ref;
        if (_context_ref == 1) 
        {
            if(_target) 
            {
                // Begin a specific context
                OGREWidget *_ogre_widget = static_cast<GTKWindow*>(_target)->getRenderWidget();

                _ogre_widget->get_gl_window()->gl_begin(_ogre_widget->get_gl_context());
            } 
            else 
            {
                // Begin a generic main context
                _main_window->gl_begin(_main_context);
            }
        }
    }
    //-----------------------------------------------------------------------
    void GTKGLSupport::end_context()
    {
        --_context_ref;
        if(_context_ref < 0)
            N_EXCEPT(RenderingAPI, _T("Too many contexts destroyed!"));
        if (_context_ref == 0)
        {
            // XX is this enough? (_main_window might not be the current window,
            // but we can never be sure the previous rendering window 
            // even still exists)
            _main_window->gl_end();
        }
    }
    //-----------------------------------------------------------------------     
    void GTKGLSupport::initialiseExtensions(void)
    {
        // XXX anythign to actually do here?
    }
    //-----------------------------------------------------------------------
    bool GTKGLSupport::checkMinGLVersion(const String& v) const
    {
        int major, minor;
        Gdk::GL::query_version(major, minor);

        std::string::size_type pos = v.find(".");
        int cmaj = atoi(v.substr(0, pos).c_str());
        int cmin = atoi(v.substr(pos + 1).c_str());

        return ( (major >= cmaj) && (minor >= cmin) );
    }
    //-----------------------------------------------------------------------
    bool GTKGLSupport::checkGpuApi(const VString& ext) const
    {
        // query_gl_extension needs an active context, doesn't matter which one
        if (_context_ref == 0)
            _main_window->gl_begin(_main_context);

        bool result = Gdk::GL::query_gl_extension(ext.c_str());

        if (_context_ref == 0)
            _main_window->gl_end();
    }
    //-----------------------------------------------------------------------
    void* GTKGLSupport::getProcAddress(const String& procname)
    {
        return (void*)Gdk::GL::get_proc_address(procname.c_str());
    }
    //-----------------------------------------------------------------------
    Glib::RefPtr<const Gdk::GL::Context> GTKGLSupport::getMainContext() const 
    {
        return _main_context;
    }
    //-----------------------------------------------------------------------