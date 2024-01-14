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
#include "OgreGLRenderSystemCommon.h"

#include "OgreEGLSupport.h"
#include "OgreEGLContext.h"

#include "OgreRoot.h"

#include "OgreLogManager.h"

extern "C" {
int gleswInit(void);
}

namespace Ogre {
    EGLContext::EGLContext(EGLDisplay eglDisplay,
                            const EGLSupport* glsupport,
                           ::EGLConfig glconfig,
                           ::EGLSurface drawable)
        : mGLSupport(glsupport),
          mContext(0)
    {
        N_assert(drawable);
        GLRenderSystemCommon* renderSystem = static_cast<GLRenderSystemCommon*>(Root::getSingleton().getRenderSystem());
        EGLContext* mainContext = static_cast<EGLContext*>(renderSystem->_getMainContext());
        ::EGLContext shareContext = (::EGLContext) 0;

        if (mainContext)
        {
            shareContext = mainContext->mContext;
        }

        _createInternalResources(eglDisplay, glconfig, drawable, shareContext);
    }

    EGLContext::~EGLContext()
    {
        GLRenderSystemCommon *rs =
            static_cast<GLRenderSystemCommon*>(Root::getSingleton().getRenderSystem());
        try {
            _destroyInternalResources();
        } catch(RenderingAPIException&) {
            // EGL_CHECK_ERROR might throw
            // just ignore as it will get logged anyway.
        }

        rs->_unregisterContext(this);
    }
    
    void EGLContext::_createInternalResources(EGLDisplay eglDisplay, ::EGLConfig glconfig, ::EGLSurface drawable, ::EGLContext shareContext)
    {
        mDrawable = drawable;
        mConfig = glconfig;
        mEglDisplay = eglDisplay;
        
        mContext = mGLSupport->createNewContext(mEglDisplay, mConfig, shareContext);
        
        if (!mContext)
        {
            N_EXCEPT(RenderingAPI,
                        "Unable to create a suitable EGLContext",
                        "EGLContext::EGLContext");
        }

        setCurrent();
    }
    
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
    void EGLContext::_updateInternalResources(EGLDisplay eglDisplay, ::EGLConfig glconfig, ::EGLSurface drawable)
    {
        mDrawable = drawable;
        mConfig = glconfig;
        mEglDisplay = eglDisplay;

        setCurrent();

        // Initialise GL3W
        if (gleswInit())
            LogManager::getSingleton().logMessage("Failed to initialize GL3W");
    }
#endif

    void EGLContext::_destroyInternalResources()
    {
        endCurrent();
        
        eglDestroyContext(mEglDisplay, mContext);
        EGL_CHECK_ERROR
        
        mContext = NULL;
    }

    void EGLContext::setCurrent()
    {
        EGLBoolean ret = eglMakeCurrent(mEglDisplay,
                                        mDrawable, mDrawable, mContext);
        EGL_CHECK_ERROR
        if (!ret)
        {
            N_EXCEPT(RenderingAPI, "Fail to make context current");
        }
    }

    void EGLContext::endCurrent()
    {
        eglMakeCurrent(mEglDisplay, 0, 0, 0);
        EGL_CHECK_ERROR
    }

    EGLSurface EGLContext::getDrawable() const
    {
        return mDrawable;
    }
}
