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
#include "OgreRoot.h"
#include "OgreGLXContext.h"
#include "OgreGLXGLSupport.h"

namespace Ogre
{
    GLXContext::GLXContext(GLXGLSupport* glsupport, ::GLXFBConfig fbconfig, ::GLXDrawable drawable, ::GLXContext context) :
        mDrawable(drawable), mContext(0), mFBConfig(fbconfig), mGLSupport(glsupport), mExternalContext(false)
    {
        GLRenderSystemCommon *renderSystem = static_cast<GLRenderSystemCommon*>(Root::getSingleton().getRenderSystem());
        GLXContext* mainContext = static_cast<GLXContext*>(renderSystem->_getMainContext());
        ::GLXContext shareContext = 0;

        if (mainContext)
        {
            shareContext = mainContext->mContext;
        }

        if (context)
        {
            mContext = context;
            mExternalContext = true;
        }
        else
        {
            mContext = mGLSupport->createNewContext(mFBConfig, GLX_RGBA_TYPE, shareContext, GL_TRUE);
        }

        if (! mContext)
        {
            N_EXCEPT(RenderingAPI, "Unable to create a suitable GLXContext", "GLXContext::GLXContext");
        }
    }

    GLXContext::~GLXContext()
    {
        GLRenderSystemCommon *rs = static_cast<GLRenderSystemCommon*>(Root::getSingleton().getRenderSystem());

        if (!mExternalContext)
            glXDestroyContext(mGLSupport->getGLDisplay(), mContext);

        rs->_unregisterContext(this);
    }

    void GLXContext::setCurrent()
    {
        glXMakeCurrent(mGLSupport->getGLDisplay(), mDrawable, mContext);
    }

    void GLXContext::endCurrent()
    {
        glXMakeCurrent(mGLSupport->getGLDisplay(), None, None);
    }

    GLContext* GLXContext::clone() const
    {
        return new GLXContext(mGLSupport, mFBConfig, mDrawable);
    }
}
