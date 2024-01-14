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

#include "OgreAndroidEGLSupport.h"
#include "OgreAndroidEGLWindow.h"
#include "OgreGLUtil.h"

namespace Ogre {

    GLNativeSupport* getGLSupport(int)
    {
        return new AndroidEGLSupport();
    }

    AndroidEGLSupport::AndroidEGLSupport() : EGLSupport(CONTEXT_ES)
    {        
        mNativeDisplay = EGL_DEFAULT_DISPLAY;
        mGLDisplay = getGLDisplay();
        
        mCurrentMode.width = 1280;
        mCurrentMode.height = 800;
        mCurrentMode.refreshRate = 0;
        mOriginalMode = mCurrentMode;
        mVideoModes.push_back(mCurrentMode);
    }

    ConfigOptionMap AndroidEGLSupport::getConfig()
    {
        ConfigOptionMap mOptions = EGLSupport::getConfig();
        ConfigOption optOrientation;
        optOrientation.name = "Orientation";
        optOrientation.immutable = false;
        optOrientation.possibleValues.push_back("Landscape");
        optOrientation.possibleValues.push_back("Portrait");
        optOrientation.currentValue = optOrientation.possibleValues[0];
        
        mOptions[optOrientation.name] = optOrientation;
        return mOptions;
    }

    AndroidEGLSupport::~AndroidEGLSupport()
    {
        
    }
    
    void AndroidEGLSupport::switchMode( uint& width, uint& height, short& frequency )
    {
    }
    
    RenderWindow* AndroidEGLSupport::newWindow( const String &name, unsigned int width, unsigned int height, bool fullScreen, const PropertyData *miscParams)
    {
        AndroidEGLWindow* window = new AndroidEGLWindow(this);
        window->create(name, width, height, fullScreen, miscParams);
        
        return window;
    }
}
