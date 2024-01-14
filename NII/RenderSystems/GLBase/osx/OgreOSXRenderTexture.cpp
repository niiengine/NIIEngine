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
#include "OgreRoot.h"
#include "OgreStringConverter.h"

#include "OgreGLRenderSystem.h"

#include "OgreOSXRenderTexture.h"
#include "OgreOSXCarbonContext.h"

#include <OpenGL/gl.h>
#define GL_EXT_texture_env_combine 1
#include <OpenGL/glext.h>
#include <AGL/agl.h>

namespace Ogre
{
    OSXPBuffer::OSXPBuffer( PixelComponentType format, uint32 width, uint32 height ) : GLPBuffer( format, width, height ), mContext( NULL )
    {
        LogManager::getSingleton().logMessage( "OSXPBuffer::OSXPBuffer()" );
        createPBuffer();
        // Create context
        //mContext = N_new OSXCarbonContext(mAGLContext);
    }
    
    OSXPBuffer::~OSXPBuffer()
    {
        LogManager::getSingleton().logMessage( "OSXPBuffer::~OSXPBuffer()" );
        N_delete mContext;
        destroyPBuffer();
    }

    GLContext* OSXPBuffer::getContext() const
    {
        LogManager::getSingleton().logMessage( "OSXPBuffer::getContext()" );
        return mContext;
    }
    
    void OSXPBuffer::createPBuffer()
    {
        LogManager::getSingleton().logMessage( "OSXPBuffer::createPBuffer()" );
        
        GLint attrib[] = { AGL_NO_RECOVERY, GL_TRUE, AGL_ACCELERATED, GL_TRUE, AGL_RGBA, AGL_NONE };
        AGLPixelFormat pf = aglChoosePixelFormat(NULL, 0, attrib);
        mAGLContext = aglCreateContext(pf, NULL);
        
        //mAGLContext = aglGetCurrentContext();
        aglCreatePBuffer( mWidth, mHeight, GL_TEXTURE_2D, GL_RGBA, 0, &mPBuffer );
        
        GLint vs = aglGetVirtualScreen( mAGLContext );
        aglSetPBuffer( mAGLContext, mPBuffer, 0, 0, vs );
        mContext = N_new OSXCocoaContext(mAGLContext, pf);
    }
    
    void OSXPBuffer::destroyPBuffer()
    {
        LogManager::getSingleton().logMessage( "OSXPBuffer::destroyPBuffer()" );
        aglDestroyPBuffer( mPBuffer );
    }
}

