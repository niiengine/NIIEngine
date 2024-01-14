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
#include "NiiGLRenderSystemBase.h"
#include "NiiGLContext.h"
#include "NiiFrustum.h"
#include "NiiGLSupport.h"
#include "NiiGLRenderTexture.h"

#if N_PLAT == N_PLAT_ANDROID || N_PLAT == N_PLAT_EMSCRIPTEN
#include "NiiEGLWindow.h"
#endif

namespace NII 
{
    //---------------------------------------------------------------------
    static void removeDuplicates(StringList & c)
    {
        std::sort(c.begin(), c.end());
        auto p = std::unique(c.begin(), c.end());
        c.erase(p, c.end());
    }
    //---------------------------------------------------------------------
    void GLRenderSystemBase::setupConfig()
    {
        mGLSupport->setupConfig(mOptions, mOptionData);

        RenderSys::setupConfig();

        SysOption optDisplayFrequency;
        optDisplayFrequency.mID = N_PropertyWindow_Frequency;
        optDisplayFrequency.mName = _T("Display Frequency");
        optDisplayFrequency.mReserveEnable = true;
        mOptions[optDisplayFrequency.mID] = optDisplayFrequency;
        //mOptionData.add(optDisplayFrequency.mID, _T(""));

        SysOption optVideoMode;
        optVideoMode.mID = N_PropertyWindow_VideoMode;
        optVideoMode.mName = _T("Video Mode");
        optVideoMode.mReserveEnable = true;
        for (const auto & mode : mGLSupport->getVideoModes())
        {
            optVideoMode.mReserve.push_back(mode.getDescription());
        }
        removeDuplicates(optVideoMode.mReserve); // also sorts
        optVideoMode.mValue = optVideoMode.mReserve[0];
        mOptions[optVideoMode.mID] = optVideoMode;
        mOptionData.add(optVideoMode.mID, optVideoMode.mValue);

        SysOption optFSAA;
        optFSAA.mID = N_PropertyWindow_FSAA;
        optFSAA.mName = _T("FSAA");
        optFSAA.mReserveEnable = true;
        for (int sampleLevel : mGLSupport->getFSAALevels())
        {
            optFSAA.mReserve.push_back(N_conv(sampleLevel));
        }
        if (!optFSAA.mReserve.empty())
        {
            removeDuplicates(optFSAA.mReserve);
            optFSAA.mValue = optFSAA.mReserve[0];
        }
        mOptions[optFSAA.mID] = optFSAA;
        mOptionData.add(optFSAA.mID, optFSAA.mValue);

        // TODO remove this on next release
        SysOption optRTTMode;
        optRTTMode.mID = N_PropertyWindow_TBOMode;
        optRTTMode.mName = _T("RenderTarget Mode");
        optRTTMode.mReserve.push_back(_T("FBO"));
        optRTTMode.mValue = optRTTMode.mReserve[0];
        optRTTMode.mReserveEnable = true;
        mOptions[optRTTMode.mID] = optRTTMode;
        mOptionData.add(optRTTMode.mID, optRTTMode.mValue);
        refreshConfig();
    }
    //---------------------------------------------------------------------
    void GLRenderSystemBase::refreshConfig()
    {
        // set bpp and refresh rate as appropriate
        SysOptionList::iterator optVideoMode = mOptions.find(N_PropertyWindow_VideoMode);
        SysOptionList::iterator optDisplayFrequency = mOptions.find(N_PropertyWindow_Frequency);
        SysOptionList::iterator optFullScreen = mOptions.find(N_PropertyWindow_FullWindow);
        SysOptionList::iterator optColourDepth = mOptions.find(N_PropertyWindow_ColourDepth);

        // we can only set refresh rate in full screen mode
        bool isFullscreen = false;
        if (optFullScreen != mOptions.end())
            isFullscreen = optFullScreen->second.mValue == _T("Yes");

        if (optVideoMode == mOptions.end() || optDisplayFrequency == mOptions.end())
            return;

        optDisplayFrequency->second.mReserve.clear();
        if(isFullscreen)
        {
            for (const auto & mode : mGLSupport->getVideoModes())
            {
                if (mode.getDescription() == optVideoMode->second.mValue)
                {
                    optDisplayFrequency->second.mReserve.push_back(N_conv(mode.refreshRate));

                    if(optColourDepth != mOptions.end())
                        optColourDepth->second.mReserve.push_back(N_conv((Nui32)mode.bpp));
                }
            }

            removeDuplicates(optDisplayFrequency->second.mReserve);
            removeDuplicates(optColourDepth->second.mReserve);
        }
        else
        {
            optDisplayFrequency->second.mReserve.push_back(_T("N/A"));
            
            // coulour depth is optional
            if (optColourDepth != mOptions.end())
            {
                for (const auto & mode : mGLSupport->getVideoModes())
                {
                    if (mode.getDescription() == optVideoMode->second.mValue)
                    {
                        optColourDepth->second.mReserve.push_back(N_conv((Nui32)mode.bpp));
                    }
                }

                removeDuplicates(optColourDepth->second.mReserve);
            }
        }

        if (optDisplayFrequency->second.mReserve.empty())
        {
            optVideoMode->second.mValue = mGLSupport->getVideoModes()[0].getDescription();
            optDisplayFrequency->second.mValue = N_conv(mGLSupport->getVideoModes()[0].refreshRate);
        }
        else
        {
            optDisplayFrequency->second.mValue = optDisplayFrequency->second.mReserve[0];
        }
        mOptionData.add(optDisplayFrequency->second.mID, optDisplayFrequency->second.mValue);
    }
    //---------------------------------------------------------------------
    bool GLRenderSystemBase::setConfig(const String & name, const String & value)
    {
        SysOptionList::iterator option, oend = mOptions.end();
        for (option = mOptions.begin(); option != oend; ++option)
        {
            if (option->second.mName == name)
            {
                option->second.mValue = value;
                mOptionData.add(option->second.mID, value);
                if (name == _T("Video Mode"))
                {
                    StringStream mode(option->second.mValue);
                    String token;
                    Nui32 tempwidth;
                    Nui32 tempheight;
                    mode >> tempwidth;
                    mode >> token; // 'x' as seperator between width and height
                    mode >> tempheight;
                    mOptionData.add(N_PropertyWindow_Width, N_conv(tempwidth));
                    mOptionData.add(N_PropertyWindow_Height, N_conv(tempheight));
                    // backend specific options. Presence determined by getConfig
                    mode >> token; // '@' as seperator between bpp on D3D
                    if (!mode.eof())
                    {
                        Nui32 bpp;
                        mode >> bpp;
                        mOptionData.add(N_PropertyWindow_ColourDepth, N_conv(bpp));
                    }
                    refreshConfig();
                }
                else if (name == _T("Full Screen"))
                {
                    refreshConfig();
                }
                else if (name == _T("FSAA"))
                {
                    StringStream fsaaMode(option->second.mValue);
                    Nui32 fsaa = 0;
                    fsaaMode >> fsaa;
                    mOptionData.add(N_PropertyWindow_FSAA, N_conv(fsaa));

                    // D3D specific
                    String hint;
                    fsaaMode >> hint;
                    if (!fsaaMode.eof())
                        mOptionData.add(N_PropertyWindow_FSAAHint, hint);
                }
                break;
            }
        }
        if (option == oend)
            return false;

        return true;
    }
    //---------------------------------------------------------------------
    bool GLRenderSystemBase::checkGpuApi(const VString & ext) const
    {
        return mExtensionList.find(ext) != mExtensionList.end() || mGLSupport->checkGpuApi(ext);
    }
    //---------------------------------------------------------------------
    bool GLRenderSystemBase::hasMinGLVersion(int major, int minor) const
    {
        if (mVersion.major == major) {
            return mVersion.minor >= minor;
        }
        return mVersion.major >= major;
    }
    //---------------------------------------------------------------------
    void GLRenderSystemBase::_completeDeferredVaoFboDestruction()
    {
        if(GLContext* ctx = mCurrentContext)
        {
            std::vector<Nui32>& vaos = ctx->_getVaoDeferredForDestruction();
            while(!vaos.empty())
            {
                _destroyVao(ctx, vaos.back());
                vaos.pop_back();
            }
            
            std::vector<Nui32>& fbos = ctx->_getFboDeferredForDestruction();
            while(!fbos.empty())
            {
                _destroyFbo(ctx, fbos.back());
                fbos.pop_back();
            }

        }
    }
    //---------------------------------------------------------------------
    void GLRenderSystemBase::_getDepthStencilFormatFor(PixelFormat interpf, Nui32 * depth, Nui32 * stencil)
    {
        mRTTManager->getBestDepthStencil(interpf, depth, stencil);
    }
    //---------------------------------------------------------------------
}
