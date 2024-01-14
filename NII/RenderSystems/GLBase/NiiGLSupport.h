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

#ifndef _NII_GLSUPPORT_H_
#define _NII_GLSUPPORT_H_

#include "NiiGLPreInclude.h"
#include "NiiGLRenderSystemBase.h"
#include "NiiGLPBuffer.h"

namespace NII
{
    class _EngineGLAPI GLSupport
    {
    public:
        struct VideoMode {
            Nui32 width;
            Nui32 height;
            Nui16 refreshRate;
            Nui16 bpp;

            String getDescription() const
            {
                Ntchar buff[64];
                NIIi n = Nsntprintf(buff, sizeof(buff), _T("%4d x %4d"), width, height);
                return String(buff, n);
            }
        };
        typedef vector<VideoMode>::type VideoModes;
        typedef std::set<VString> ExtensionList;
    public:
        GLSupport() {}
        virtual ~GLSupport() {}

        /** 添加基础选项
        @version NIIEngine 3.0.0
        */
        virtual void setupConfig(SysOptionList & out, PropertyData & out2) = 0;

        /// @copydoc RenderSys::createWindow
        virtual ViewWindow * newWindow(const String & name, NCount width, NCount height, bool full, const PropertyData * custom = 0) = 0;

        /** 创建离屏缓存
        @version NIIEngine 3.0.0
        */
        virtual GLPBuffer * createPBuffer(PixelDataType format, NCount w, NCount h) { return 0; }

        /// Start anything special
        virtual void start() = 0;

        /// Stop anything special
        virtual void stop() = 0;

        /// Check if an extension is available
        virtual bool checkGpuApi(const VString & ext) const { return extensionList.find(ext) != extensionList.end(); }
        
        virtual void initialiseExtensions()                 {}

        /// Get the address of a function
        virtual void * getProcAddress(const VString & name) = 0;

        /// @copydoc RenderSys::getDisplayMonitorCount
        virtual NCount getDisplayMonitorCount() const       { return 1; }
        const std::vector<int> & getFSAALevels() const      { return mFSAALevels; }
        const VideoModes& getVideoModes() const             { return mVideoModes; }
    protected:
        ExtensionList extensionList;
        VideoModes mVideoModes;
        std::vector<int> mFSAALevels;
    };
    
    _EngineGLAPI GLSupport * getGLSupport(int profile = 0);
}
#endif
