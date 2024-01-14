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
#include "OgreGLES2DepthBuffer.h"
#include "OgreGLES2HardwarePixelBuffer.h"
#include "OgreGLES2RenderSystem.h"
#include "OgreGLES2FrameBufferObject.h"

namespace Ogre
{
    GLES2DepthBuffer::GLES2DepthBuffer( uint16 poolId, GLES2RenderSystem *renderSystem, GLContext *creatorContext,
                                    GLES2RenderBuffer *depth, GLES2RenderBuffer *stencil,
                                    uint32 width, uint32 height, uint32 fsaa,
                                    bool manual )
    : GLDepthBufferBase( poolId, renderSystem, creatorContext, depth, stencil, width, height, fsaa,  manual)
    {
        if( mDepthBuffer )
        {
            switch( mDepthBuffer->getGLFormat() )
            {
            case GL_DEPTH_COMPONENT16:
                mBitDepth = 16;
                break;
            case GL_DEPTH_COMPONENT24_OES:
            case GL_DEPTH_COMPONENT32_OES:
            case GL_DEPTH24_STENCIL8_OES:  // Packed depth / stencil
                mBitDepth = 32;
                break;
            }
        }
    }
}
