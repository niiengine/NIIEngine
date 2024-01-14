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

#include "NiiwxWidgetGLSupport.h"
#include "NiiwxWidgetWindow.h"
#include "NiiLogManager.h"
#include "NiiException.h"
#include "NiiStrConv.h"

namespace NII
{
    GLSupport * getGLSupport(int profile)
    {
        return new wxWidgetGLSupport();
    }
    //-----------------------------------------------------------------------
    template<> wxWidgetGLSupport * Singleton<wxWidgetGLSupport>::mOnly = 0;
    wxWidgetGLSupport * wxWidgetGLSupport::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    wxWidgetGLSupport & wxWidgetGLSupport::getOnly()
    {
        N_assert(mOnly);
        return (*mOnly);
    }
    //-----------------------------------------------------------------------
    wxWidgetGLSupport::wxWidgetGLSupport()
    {
    }
    //-----------------------------------------------------------------------
    void wxWidgetGLSupport::setupConfig(SysOptionList & out, PropertyData & out2)
    {
        SysOption optColourDepth;

        // Video mode possiblities
        DEVMODE DevMode;
        DevMode.dmSize = sizeof(DEVMODE);
        for (DWORD i = 0; EnumDisplaySettings(NULL, i, &DevMode); ++i)
        {
            if (DevMode.dmBitsPerPel < 16 || DevMode.dmPelsHeight < 480)
                continue;
            mVideoModes.push_back({ DevMode.dmPelsWidth, DevMode.dmPelsHeight,
                Nui16(DevMode.dmDisplayFrequency), Nui16(DevMode.dmBitsPerPel) });
        }

        optColourDepth.mID = N_PropertyWindow_ColourDepth;
        optColourDepth.mName = _T("Colour Depth");
        optColourDepth.mReserveEnable = true;

        out[optColourDepth.mID] = optColourDepth;
    }
    //-----------------------------------------------------------------------
    ViewWindow * wxWidgetGLSupport::newWindow(const String & name, NCount width, NCount height,
        bool full, const PropertyData * custom)
    {
        //ViewWindow * parentWindowHandle;
        wxWidgetWindow * window = new wxWidgetWindow();
        window->setup(name, width, height, full, custom);

        return window;
    }
    //-----------------------------------------------------------------------
    void wxWidgetGLSupport::start()
    {
        LogManager::getOnly().log(
            "-------------------------------\n"
            _T("--- Starting wxWidget Subsystem Begin---\n")
            "-------------------------------");

    }
    //-----------------------------------------------------------------------     
    void wxWidgetGLSupport::stop()
    {
        LogManager::getOnly().log(
            "-------------------------------\n"
            _T("--- Stopping wxWidget Subsystem End---\n")
            "-------------------------------");
    }
    //-----------------------------------------------------------------------
    void* wxWidgetGLSupport::getProcAddress(const VString & procname)
    {
        return (void *)wglGetProcAddress(procname.c_str());
    }
    //-----------------------------------------------------------------------
}