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
#include "NiiGLDepthBuffer.h"
#include "NiiGLHardwarePixelBuffer.h"
#include "NiiGLRenderSystem.h"
#include "NiiGLFrameBufferObject.h"

namespace NII
{
    GLDepthBuffer::GLDepthBuffer(GroupID poolId, GLRenderSystem *rsys, GLContext *glcontext,
        GLRenderBuffer *depth, GLRenderBuffer *stencil, NCount width, NCount height, Nui32 fsaa, bool manual): 
        GLDepthBufferBase(poolId, rsys, glcontext, depth, stencil, width, height, fsaa, manual)
    {
        if( mDepthBuffer )
        {
            switch( mDepthBuffer->getGLFormat() )
            {
            case GL_DEPTH_COMPONENT16:
                mBitDepth = 16;
                break;
            case GL_DEPTH_COMPONENT24:
            case GL_DEPTH24_STENCIL8:  // Packed depth / stencil
                mBitDepth = 24;
                break;
            case GL_DEPTH_COMPONENT32:
            case GL_DEPTH_COMPONENT32F:
            case GL_DEPTH32F_STENCIL8:
                mBitDepth = 32;
                break;
            }
        }
    }
}
