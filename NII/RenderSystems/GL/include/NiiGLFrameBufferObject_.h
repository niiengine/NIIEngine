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
#ifndef __NII_GLFBO_H__
#define __NII_GLFBO_H__

#include "NiiGLRenderTexture.h"
#include "NiiGLContext.h"

namespace NII 
{
    
    /** Frame Buffer Object abstraction.
    */
    class _EngineGLAPI GLFrameBufferObject : public GLFrameBufferObjectBase
    {
    public:
        GLFrameBufferObject(GLFBOManager * mag, Nui32 fsaa);
        ~GLFrameBufferObject();

        /// @copydetails GLFrameBufferObjectBase::bind
        bool bind(bool recreateIfNeeded);

        /** This function acts very similar to @see GLFBORenderTexture::attachDepthBuffer
            The difference between D3D & OGL is that D3D setups the ExtFrameBuffer before rendering,
            while OGL setups the ExtFrameBuffer per FBO. So the ExtFrameBuffer (RenderBuffer) needs to
            be attached for OGL.
        */
        void attachExt(ExtFrameBuffer * extbuf);

        void detachExt();

        /** Swap buffers - only useful when using multisample buffers.
        */
        void swap();
        
        inline GLFBOManager * getManager() { return mManager; }
    protected:
        /// @copydetails GLFrameBufferObjectBase::initialise
        void initialise();
    private:
        GLFBOManager *mManager;
        GLSurfaceDesc mMultisampleColourBuffer;
    };

}

#endif
