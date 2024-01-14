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

#ifndef _NII_GTKGLSUPPORT_H_
#define _NII_GTKGLSUPPORT_H_

#include "NiiGLSupport.h"
#include <gtkmm/main.h>
#include <gtkglmm.h>

namespace NII 
{
    class RenderWidget;
    /**
     * GL support in a GTK window.
     *
     * I made this a Singleton, so that the main context can be queried by
     * GTKWindows.
     */
    class GTKGLSupport : public GLSupport, public Singleton<GTKGLSupport>
    {
    public:
        GTKGLSupport();

        /// @copydetails GLSupport::addConfig
        void addConfig();

        /// @copydetails GLSupport::setConfig
        void setConfig(const String & name, const String & value);

        /// @copydetails GLSupport::checkConfig
        String checkConfig();

        /// @copydetails GLSupport::createWindow
        ViewWindow * createWindow(bool win, GLRenderSystem * sys, const String & title);

        /// @copydetails GLSupport::newWindow
        ViewWindow * newWindow(const String & name, NCount width, NCount height, bool full, 
            const PropertyData * custom = 0);

        /// @copydetails GLSupport::start
        void start();

        /// @copydetails GLSupport::stop
        void stop();

        void begin_context(FrameObj *_target = 0);
        void end_context();

        /// @copydetails GLSupport::initialiseExtensions
        void initialiseExtensions();

        /// @copydetails GLSupport::checkMinGLVersion
        bool checkMinGLVersion(const String & v) const;

        /// @copydetails GLSupport::checkGpuApi
        bool checkGpuApi(const VString & ext) const;

        /// @copydetails GLSupport::getProcAddress
        void * getProcAddress(const VString& procname);

        Glib::RefPtr<const Gdk::GL::Context> getMainContext() const;

        /** Override standard Singleton retrieval.
        @remarks
        Why do we do this? Well, it's because the Singleton
        implementation is in a .h file, which means it gets compiled
        into anybody who includes it. This is needed for the
        Singleton template to work, but we actually only want it
        compiled into the implementation of the class based on the
        Singleton, not all of them. If we don't change this, we get
        link errors when trying to use the Singleton-based class from
        an outside dll.
        @par
        This method just delegates to the template version anyway,
        but the implementation stays in this single compilation unit,
        preventing link errors.
        */
        static GTKGLSupport& getOnly(void);
        /** Override standard Singleton retrieval.
        @remarks
        Why do we do this? Well, it's because the Singleton
        implementation is in a .h file, which means it gets compiled
        into anybody who includes it. This is needed for the
        Singleton template to work, but we actually only want it
        compiled into the implementation of the class based on the
        Singleton, not all of them. If we don't change this, we get
        link errors when trying to use the Singleton-based class from
        an outside dll.
        @par
        This method just delegates to the template version anyway,
        but the implementation stays in this single compilation unit,
        preventing link errors.
        */
        static GTKGLSupport* getOnlyPtr(void);
    private:
        int _context_ref;
        Gtk::Main _kit;
        Glib::RefPtr<Gdk::GL::Context> _main_context;
        Glib::RefPtr<Gdk::GL::Window> _main_window;
    };
};
#endif