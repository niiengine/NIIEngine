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

#include "NiiGLRenderTexture.h"
#include "NiiGLHardwarePixelBufferBase.h"
#include "NiiGLRenderSystemBase.h"
#include "NiiEngine.h"

namespace NII 
{
    const String GLTextureFrame::CustomAttributeString_FBO = _T("FBO");
    const String GLTextureFrame::CustomAttributeString_TARGET = _T("TARGET");
    const String GLTextureFrame::CustomAttributeString_GLCONTEXT = _T("GLCONTEXT");
    //-----------------------------------------------------------------------------
    template<> GLRTTManager* Singleton<GLRTTManager>::mOnly = NULL;
    //-----------------------------------------------------------------------------
    GLFrameBufferObjectBase::GLFrameBufferObjectBase(Nui32 fsaa) : 
        mFB(0), 
        mMultisampleFB(0), 
        mNumSamples(fsaa)
    {
        mContext = static_cast<GLRenderSystemBase *>(N_Engine().getRender())->_getCurrentContext();

        // Initialise state
        mDepth.buffer = 0;
        mStencil.buffer = 0;
        for(size_t x = 0; x < NII_MAX_RenderTarget; ++x)
        {
            mColour[x].buffer=0;
        }
    }
    //-----------------------------------------------------------------------------
    void GLFrameBufferObjectBase::bindSurface(NCount attachment, const GLSurfaceDesc & target)
    {
        N_assert(attachment < NII_MAX_RenderTarget);
        mColour[attachment] = target;
        // Re-initialise
        if(mColour[0].buffer)
            initialise();
    }
    //-----------------------------------------------------------------------------
    void GLFrameBufferObjectBase::unbindSurface(NCount attachment)
    {
        N_assert(attachment < NII_MAX_RenderTarget);
        mColour[attachment].buffer = 0;
        // Re-initialise if buffer 0 still bound
        if(mColour[0].buffer)
            initialise();
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GLRTTManager
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GLRTTManager::GLRTTManager() {}
    //-----------------------------------------------------------------------------
    GLRTTManager::~GLRTTManager() {}
    //-----------------------------------------------------------------------------
    PixelFormat GLRTTManager::getSupportedAlternative(PixelFormat format)
    {
        if (checkFormat(format))
        {
            return format;
        }

        /// Find first alternative
        PixelFormatFeature pct = PixelUtil::getExentMark(format);

        switch (pct)
        {
        case PFF_Int16:
            format = PF_R16G16B16A16ui;
            break;
        //case PFF_Float16:   
            if(PixelUtil::isFloat16(format))
            format = PF_R16G16B16A16;
            break;
        //case PFF_Float:     
            if(PixelUtil::isFloat(format))
            format = PF_R32G32B32A32;
            break;
        default:
            format = PF_BYTE_RGBA; // native endian
            break;
        }

        if (checkFormat(format))
            return format;

        /// If none at all, return to default
        return PF_BYTE_RGBA; // native endian
    }
    //-----------------------------------------------------------------------------
    void GLRTTManager::releaseRenderBuffer(const GLSurfaceDesc & surface)
    {
        if(surface.buffer == 0)
            return;
        RBFormat key(surface.buffer->getGLFormat(), surface.buffer->getWidth(), surface.buffer->getHeight(), surface.numSamples);
        RenderBufferMap::iterator it = mRenderBufferMap.find(key);
        if(it != mRenderBufferMap.end())
        {
            // Decrease refcount
            --it->second.refcount;
            if(it->second.refcount==0)
            {
                // If refcount reaches zero, delete buffer and remove from map
                delete it->second.buffer;
                mRenderBufferMap.erase(it);
                //                              std::cerr << "Destroyed renderbuffer of format " << std::hex << key.format << std::dec
                //                                      << " of " << key.width << "x" << key.height << std::endl;
            }
        }
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GLTextureFrame
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GLTextureFrame::GLTextureFrame(const String & name, const GLSurfaceDesc & target, bool writeGamma, Nui32 fsaa) : 
        TextureFrame(target.buffer, target.zoffset)
    {
        mName = name;
        if (writeGamma)
            mMark |= M_SRGB;
        else
            mMark &= ~M_SRGB;
        mFSAA = fsaa;
    }
    //-----------------------------------------------------------------------------
}
