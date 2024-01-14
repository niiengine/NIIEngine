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

#include "NiiException.h"
#include "NiiLogManager.h"
#include "NiiStringConverter.h"
#include "NiiSDLGLSupport.h"
#include "NiiSDLWindow.h"

using namespace NII;
    //-----------------------------------------------------------------------
    SDLGLSupport::SDLGLSupport()
    {
        SDL_Init(SDL_INIT_VIDEO);
    }
    //-----------------------------------------------------------------------
    SDLGLSupport::~SDLGLSupport(){}
    //-----------------------------------------------------------------------
    void SDLGLSupport::addConfig(void)
    {
        mVideoModes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_OPENGL);
        
        if (mVideoModes == (SDL_Rect **)0)
        {
            NII_EXCEPT(RenderingAPI, _T("Unable to load video modes"));
        }

        SysOption optFullScreen;
        SysOption optVideoMode;
        SysOption optFSAA;
        SysOption optRTTMode;
#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
        SysOption optEnableFixedPipeline;
#endif        

        // FS setting possiblities
        optFullScreen.name = "Full Screen";
        optFullScreen.mReserve.push_back("Yes");
        optFullScreen.mReserve.push_back("No");
        optFullScreen.mValue = "Yes";
        optFullScreen.mReserveEnable = true;

        // Video mode possiblities
        optVideoMode.name = "Video Mode";
        optVideoMode.mReserveEnable = true;
        for (size_t i = 0; mVideoModes[i]; i++)
        {
            char szBuf[16];
            snprintf(szBuf, 16, "%d x %d", mVideoModes[i]->w, mVideoModes[i]->h);
            optVideoMode.mReserve.push_back(szBuf);
            // Make the first one default
            if (i == 0)
            {
                optVideoMode.mValue = szBuf;
            }
        }

        //FSAA possibilities
        optFSAA.name = "FSAA";
        optFSAA.mReserve.push_back("0");
        optFSAA.mReserve.push_back("2");
        optFSAA.mReserve.push_back("4");
        optFSAA.mReserve.push_back("6");
        optFSAA.mValue = "0";
        optFSAA.mReserveEnable = true;

        optRTTMode.name = "RenderTarget Mode";
        optRTTMode.mReserve.push_back("FBO");
        optRTTMode.mReserve.push_back("PBuffer");
        optRTTMode.mReserve.push_back("Copy");
        optRTTMode.mValue = "FBO";
        optRTTMode.mReserveEnable = true;
        
#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
        optEnableFixedPipeline.name = "Fixed Pipeline Enabled";
        optEnableFixedPipeline.mReserve.push_back( "Yes" );
        optEnableFixedPipeline.mReserve.push_back( "No" );
        optEnableFixedPipeline.mValue = "Yes";
        optEnableFixedPipeline.mReserveEnable = true;
+#endif

        mOptions[optFullScreen.name] = optFullScreen;
        mOptions[optVideoMode.name] = optVideoMode;
        mOptions[optFSAA.name] = optFSAA;
        mOptions[optRTTMode.name] = optRTTMode;
#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
        mOptions[optEnableFixedPipeline.name] = optEnableFixedPipeline;
#endif        
    }
    //-----------------------------------------------------------------------
    String SDLGLSupport::checkConfig()
    {
        return String("");
    }
    //-----------------------------------------------------------------------
    ViewWindow * SDLGLSupport::createWindow(bool autoCreateWindow, GLRenderSystem * renderSystem,
        const String & windowTitle)
    {
        if (autoCreateWindow)
        {
            SysOptionList::iterator opt = mOptions.find("Full Screen");
            if (opt == mOptions.end())
                NII_EXCEPT(RenderingAPI, _T("Can't find full screen options!"));
            bool fullscreen = (opt->second.mValue == "Yes");

            opt = mOptions.find("Video Mode");
            if (opt == mOptions.end())
                NII_EXCEPT(RenderingAPI, _T("Can't find video mode options!"));
            String val = opt->second.mValue;
            String::size_type pos = val.find('x');
            if (pos == String::npos)
                NII_EXCEPT(RenderingAPI, _T("Invalid Video Mode provided"));

            // Parse FSAA config
            PropertyData winOptions;
            winOptions["title"] = windowTitle;
            int fsaa_x_samples = 0;
            opt = mOptions.find("FSAA");
            if(opt != mOptions.end())
            {
                winOptions["FSAA"] = opt->second.mValue;
            }

#ifdef RTSHADER_SYSTEM_BUILD_CORE_SHADERS
            opt = mOptions.find("Fixed Pipeline Enabled");
            if (opt == mOptions.end())
                NII_EXCEPT(Exception::ERR_INVALIDPARAMS, 
                    _T("Can't find Fixed Pipeline enabled options!"));
            bool enableFixedPipeline = (opt->second.mValue == "Yes");
            renderSystem->setFixedPipelineMode(enableFixedPipeline);
#endif            
            
            unsigned int w = StringConverter::parseUnsignedInt(val.substr(0, pos));
            unsigned int h = StringConverter::parseUnsignedInt(val.substr(pos + 1));

            const SDL_VideoInfo * videoInfo = SDL_GetVideoInfo();
            return renderSystem->createWindow(windowTitle, w, h, fullscreen, &winOptions);
        }
        else
        {
            // XXX What is the else?
            return NULL;
        }
    }
    //-----------------------------------------------------------------------
    ViewWindow * SDLGLSupport::newWindow(const String &name, NCount width, NCount height, 
        bool fullScreen, const PropertyData *miscParams)
    {
        SDLWindow * window = new SDLWindow();
        window->create(name, width, height, fullScreen, miscParams);
        return window;
    }
    //-----------------------------------------------------------------------
    void SDLGLSupport::start()
    {
        LogManager::getOnly().logMessage(
            "******************************\n"
            _T("*** Starting SDL Subsystem ***\n")
            "******************************");

        SDL_Init(SDL_INIT_VIDEO);
    }
    //-----------------------------------------------------------------------
    void SDLGLSupport::stop()
    {
        LogManager::getOnly().logMessage(
            "******************************\n"
            _T("*** Stopping SDL Subsystem ***\n")
            "******************************");

        SDL_Quit();
    }
    //-----------------------------------------------------------------------
    void* SDLGLSupport::getProcAddress(const String& procname)
    {
        return SDL_GL_GetProcAddress(procname.c_str());
    }
    //-----------------------------------------------------------------------