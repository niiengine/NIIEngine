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

#include "OgreException.h"
#include "OgreLogManager.h"
#include "OgreStringConverter.h"
#include "OgreRoot.h"

#include "OgreX11EGLSupport.h"
#include "OgreX11EGLWindow.h"
#include "OgreX11EGLRenderTexture.h"

#include "OgreGLUtil.h"

#include <X11/extensions/Xrandr.h>

namespace Ogre {
    GLNativeSupport* getGLSupport(int profile)
    {
        return new X11EGLSupport(profile);
    }

    X11EGLSupport::X11EGLSupport(int profile) : EGLSupport(profile)
    {
        // A connection that might be shared with the application for GL rendering:
        mGLDisplay = getGLDisplay();

        // A connection that is NOT shared to enable independent event processing:
        mNativeDisplay = getNativeDisplay();

        int dummy;

        if (XQueryExtension(mNativeDisplay, "RANDR", &dummy, &dummy, &dummy))
        {
            XRRScreenConfiguration *screenConfig;

            screenConfig = XRRGetScreenInfo(mNativeDisplay, DefaultRootWindow(mNativeDisplay));

            if (screenConfig)
            {
                XRRScreenSize *screenSizes;
                int nSizes = 0;
                Rotation currentRotation;
                int currentSizeID = XRRConfigCurrentConfiguration(screenConfig, &currentRotation);

                screenSizes = XRRConfigSizes(screenConfig, &nSizes);

                mCurrentMode.width = screenSizes[currentSizeID].width;
                mCurrentMode.height = screenSizes[currentSizeID].height;
                mCurrentMode.refreshRate = XRRConfigCurrentRate(screenConfig);

                mOriginalMode = mCurrentMode;

                for (int sizeID = 0; sizeID < nSizes; sizeID++)
                {
                    short *rates;
                    int nRates = 0;

                    rates = XRRConfigRates(screenConfig, sizeID, &nRates);

                    for (int rate = 0; rate < nRates; rate++)
                    {
                        VideoMode mode;

                        mode.width = screenSizes[sizeID].width;
                        mode.height = screenSizes[sizeID].height;
                        mode.refreshRate = rates[rate];

                        mVideoModes.push_back(mode);
                    }
                }
                XRRFreeScreenConfigInfo(screenConfig);
            }
        } else
        {
            mCurrentMode.width = DisplayWidth(mNativeDisplay, DefaultScreen(mNativeDisplay));
            mCurrentMode.height = DisplayHeight(mNativeDisplay, DefaultScreen(mNativeDisplay));
            mCurrentMode.refreshRate = 0;

            mOriginalMode = mCurrentMode;

            mVideoModes.push_back(mCurrentMode);
        }

        EGLConfig *glConfigs;
        int config, nConfigs = 0;

        glConfigs = chooseGLConfig(NULL, &nConfigs);

        for (config = 0; config < nConfigs; config++)
        {
            int caveat, samples;

            getGLConfigAttrib(glConfigs[config], EGL_CONFIG_CAVEAT, &caveat);

            if (caveat != EGL_SLOW_CONFIG)
            {
                getGLConfigAttrib(glConfigs[config], EGL_SAMPLES, &samples);
                mFSAALevels.push_back(samples);
            }
        }

        free(glConfigs);
    }

    X11EGLSupport::~X11EGLSupport()
    {
        if (mNativeDisplay)
        {
            XCloseDisplay((Display*)mNativeDisplay);
        }

        if (mGLDisplay)
        {
            eglTerminate(mGLDisplay);
        }
    }

    NativeDisplayType X11EGLSupport::getNativeDisplay()
    {
        if (!mNativeDisplay)
        {
        mNativeDisplay = (NativeDisplayType)XOpenDisplay(NULL);

            if (!mNativeDisplay)
            {
                N_EXCEPT(RenderingAPI,
                            "Couldn`t open X display",
                            "X11EGLSupport::getXDisplay");
            }

            mAtomDeleteWindow = XInternAtom((Display*)mNativeDisplay, "WM_DELETE_WINDOW", True);
            mAtomFullScreen = XInternAtom((Display*)mNativeDisplay, "_NET_WM_STATE_FULLSCREEN", True);
            mAtomState = XInternAtom((Display*)mNativeDisplay, "_NET_WM_STATE", True);
        }

        return mNativeDisplay;
    }

    String X11EGLSupport::getDisplayName(void)
    {
        return String((const char*)XDisplayName(DisplayString(mNativeDisplay)));
    }


    void X11EGLSupport::switchMode(uint& width, uint& height, short& frequency)
    {
        int size = 0;

        EGLVideoModes eglVideoModes(mVideoModes.begin(), mVideoModes.end());
        EGLVideoModes::iterator mode;
        EGLVideoModes::iterator end = eglVideoModes.end();
        EGLVideoMode *newMode = 0;

        for(mode = eglVideoModes.begin(); mode != end; size++)
        {
            if (mode->first.first >= width &&
                mode->first.second >= height)
            {
                if (!newMode ||
                    mode->first.first < newMode->first.first ||
                    mode->first.second < newMode->first.second)
                {
                    newMode = &(*mode);
                }
            }

            EGLVideoMode* lastMode = &(*mode);

            while (++mode != end && mode->first == lastMode->first)
            {
                if (lastMode == newMode && mode->second == frequency)
                {
                    newMode = &(*mode);
                }
            }
        }

        if (newMode && *newMode != mCurrentMode)
        {
            newMode->first.first = DisplayWidth(mNativeDisplay, 0);
            newMode->first.second = DisplayHeight(mNativeDisplay, 0);
            newMode->second = 0; // TODO: Hardcoding refresh rate for LCD's
            mCurrentMode = {newMode->first.first, newMode->first.second, newMode->second};
        }
    }

    XVisualInfo *X11EGLSupport::getVisualFromFBConfig(::EGLConfig glConfig)
    {
        XVisualInfo *vi, tmp;
        int vid, n;
        ::EGLDisplay glDisplay;

        glDisplay = getGLDisplay();
        mNativeDisplay = getNativeDisplay();

        if (eglGetConfigAttrib(glDisplay, glConfig, EGL_NATIVE_VISUAL_ID, &vid) == EGL_FALSE)
        {
            N_EXCEPT(RenderingAPI, "Fail to get VISUAL_ID from glConfig");
            return 0;
        }
        EGL_CHECK_ERROR

        if (vid == 0)
        {
            const int screen_number = DefaultScreen(mNativeDisplay);
            Visual *v = DefaultVisual((Display*)mNativeDisplay, screen_number);
            vid = XVisualIDFromVisual(v);
        }

        tmp.visualid = vid;
        vi = 0;
        vi = XGetVisualInfo((Display*)mNativeDisplay,
                            VisualIDMask,
                            &tmp, &n);
        if (vi == 0)
        {
            N_EXCEPT(RenderingAPI, "Fail to get X11 VISUAL");
            return 0;
        }

        return vi;
    }

    RenderWindow* X11EGLSupport::newWindow(const String &name,
                                        unsigned int width, unsigned int height,
                                        bool fullScreen,
                                        const PropertyData *miscParams)
    {
        EGLWindow* window = new X11EGLWindow(this);
        window->create(name, width, height, fullScreen, miscParams);

        return window;
    }

    //X11EGLSupport::getGLDisplay sets up the native variable
    //then calls EGLSupport::getGLDisplay
    EGLDisplay X11EGLSupport::getGLDisplay()
    {
        if (!mGLDisplay)
        {
            if(!mNativeDisplay)
                mNativeDisplay = getNativeDisplay();
            return EGLSupport::getGLDisplay();
        }
        return mGLDisplay;
    }

}

