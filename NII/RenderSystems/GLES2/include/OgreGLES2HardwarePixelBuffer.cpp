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

#include "OgreGLES2HardwareBufferManager.h"
#include "OgreGLES2HardwarePixelBuffer.h"

#include "OgreTextureManager.h"

#include "OgreGLES2PixelFormat.h"
#include "OgreGLES2FBORenderTexture.h"
#include "OgreGLUtil.h"
#include "OgreGLES2RenderSystem.h"
#include "OgreGLES2StateCacheManager.h"
#include "OgreRoot.h"
#include "OgreGLSLESProgramManager.h"
#include "OgreGLSLESLinkProgram.h"
#include "OgreGLSLESProgramPipeline.h"
#include "OgreBitwise.h"
#include "OgreGLNativeSupport.h"
#include "OgreGLES2HardwareBuffer.h"
#include "OgreGLES2Texture.h"

namespace Ogre {
    GLES2HardwarePixelBuffer::GLES2HardwarePixelBuffer(uint32 width, uint32 height,
                                                     uint32 depth, PixelFormat format,
                                                     Buffer::Usage usage)
        : GLHardwarePixelBufferCommon(width, height, depth, format, usage)
    {
    }

    void GLES2HardwarePixelBuffer::blitFromMemory(const PixelBox &src, const Box &dstBox)
    {
        if (!mBuffer.contains(dstBox))
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "Destination box out of range",
                        "GLES2HardwarePixelBuffer::blitFromMemory");
        }

        PixelBox scaled;

        if (src.getWidth() != dstBox.getWidth() ||
            src.getHeight() != dstBox.getHeight() ||
            src.getDepth() != dstBox.getDepth())
        {
            // Scale to destination size.
            // This also does pixel format conversion if needed
            allocateBuffer();
            scaled = mBuffer.getSubVolume(dstBox);
            Image::scale(src, scaled, Image::FILTER_BILINEAR);
        }
        else if (src.format != mFormat)
        {
            // Extents match, but format is not accepted as valid source format for GL
            // do conversion in temporary buffer
            allocateBuffer();
            scaled = mBuffer.getSubVolume(dstBox);
            PixelUtil::bulkPixelConversion(src, scaled);
        }
        else
        {
            // No scaling or conversion needed
            scaled = src;
        }

        upload(scaled, dstBox);
        freeBuffer();
    }

    void GLES2HardwarePixelBuffer::blitToMemory(const Box &srcBox, const PixelBox &dst)
    {
        if (!mBuffer.contains(srcBox))
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "source box out of range",
                        "GLES2HardwarePixelBuffer::blitToMemory");
        }

        if (srcBox.left == 0 && srcBox.right == getWidth() &&
            srcBox.top == 0 && srcBox.bottom == getHeight() &&
            srcBox.front == 0 && srcBox.back == getDepth() &&
            dst.getWidth() == getWidth() &&
            dst.getHeight() == getHeight() &&
            dst.getDepth() == getDepth() &&
            GLES2PixelUtil::getGLInternalFormat(dst.format) != 0)
        {
            // The direct case: the user wants the entire texture in a format supported by GL
            // so we don't need an intermediate buffer
            download(dst);
        }
        else
        {
            // Use buffer for intermediate copy
            allocateBuffer();
            // Download entire buffer
            download(mBuffer);
            if(srcBox.getWidth() != dst.getWidth() ||
                srcBox.getHeight() != dst.getHeight() ||
                srcBox.getDepth() != dst.getDepth())
            {
                // We need scaling
                Image::scale(mBuffer.getSubVolume(srcBox), dst, Image::FILTER_BILINEAR);
            }
            else
            {
                // Just copy the bit that we need
                PixelUtil::bulkPixelConversion(mBuffer.getSubVolume(srcBox), dst);
            }
            freeBuffer();
        }
    }
    
    // TextureBuffer
    GLES2TextureBuffer::GLES2TextureBuffer(GLES2Texture* parent, GLint face, GLint level,
                                           GLint width, GLint height, GLint depth)
        : GLES2HardwarePixelBuffer(width, height, depth, parent->getFormat(), (Usage)parent->getUsage()),
          mTarget(parent->getGLES2TextureTarget()), mTextureID(parent->getGLID()), mFace(face),
          mLevel(level)
    {
        // Get face identifier
        mFaceTarget = mTarget;
        if(mTarget == GL_TEXTURE_CUBE_MAP)
            mFaceTarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;

        mGLInternalFormat =
            GLES2PixelUtil::getGLInternalFormat(mFormat, parent->isHardwareGammaEnabled());

        mRowSize = mWidth;
        mSliceSize = mHeight*mWidth;
        mSizeInBytes = PixelUtil::getSize(mWidth, mHeight, mDepth, mFormat);

#if OGRE_DEBUG_MODE
        // Log a message
        std::stringstream str;
        str << "GLES2HardwarePixelBuffer constructed for texture " << parent->getName()
            << " id " << mTextureID << " face " << mFace << " level " << mLevel << ":"
            << " width=" << mWidth << " height="<< mHeight << " depth=" << mDepth
            << " format=" << PixelUtil::getFormatName(mFormat);
        LogManager::getSingleton().logMessage(LL_Info, str.str());
#endif

        // Set up a pixel box
        mBuffer = PixelBox(mWidth, mHeight, mDepth, mFormat);
        
        if (mWidth==0 || mHeight==0 || mDepth==0)
            // We are invalid, do not allocate a buffer
            return;

        // Is this a render target?
        if (mUsage & TU_RENDERTARGET)
        {
            // Create render target for each slice
            mSliceTRT.reserve(mDepth);
            for(uint32 zoffset=0; zoffset<mDepth; ++zoffset)
            {
                String name;
                name = "rtt/" + StrConv::conv((size_t)this) + "/" + parent->getName();
                GLSurfaceDesc surface;
                surface.buffer = this;
                surface.zoffset = zoffset;
                RenderTexture* trt = GLRTTManager::getSingleton().createRenderTexture(
                    name, surface, parent->isHardwareGammaEnabled(), parent->getFSAA());
                mSliceTRT.push_back(trt);
                Root::getSingleton().getRenderSystem()->attachRenderTarget(*mSliceTRT[zoffset]);
            }
        }
    }

    GLES2TextureBuffer::~GLES2TextureBuffer()
    {
        if (mUsage & TU_RENDERTARGET)
        {
            // Delete all render targets that are not yet deleted via destroyImpl because the rendertarget
            // was deleted by the user.
            for (SliceTRT::const_iterator it = mSliceTRT.begin(); it != mSliceTRT.end(); ++it)
            {
                Root::getSingleton().getRenderSystem()->destroyRenderTarget((*it)->getName());
            }
        }
    }

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_EMSCRIPTEN
    void GLES2TextureBuffer::updateTextureId(GLuint textureID)
    {
        mTextureID = textureID;
    }
#endif

    void GLES2TextureBuffer::upload(const PixelBox &data, const Box &dest)
    {
        GLES2RenderSystem* rs = getGLES2RenderSystem();

        rs->getStateManager()->bindGLTexture(mTarget, mTextureID);

        bool hasGLES30 = rs->hasMinGLVersion(3, 0);
        // PBO handling is broken
#if 0// OGRE_NO_GLES3_SUPPORT == 0
        // Calculate size for all mip levels of the texture
        size_t dataSize = 0;
        if(mTarget == GL_TEXTURE_2D_ARRAY)
        {
            dataSize = PixelUtil::getSize(dest.getWidth(), dest.getHeight(), dest.getDepth(), data.format);
        }
        else
        {
            dataSize = PixelUtil::getSize(data.getWidth(), data.getHeight(), mDepth, data.format);
        }

        // Upload data to PBO
        GLES2HardwareBuffer buffer(GL_PIXEL_UNPACK_BUFFER, dataSize, mUsage);
        buffer.writeData(0, dataSize, data.data, false);

        void* pdata = NULL;
#if OGRE_DEBUG_MODE
        std::stringstream str;
        str << "GLES2TextureBuffer::upload: " << mTextureID
        << " pixel buffer: " << buffer.getGLBufferId()
        << " bytes: " << mSizeInBytes
        << " dest depth: " << dest.getDepth()
        << " dest front: " << dest.front
        << " datasize: " << dataSize
        << " face: " << mFace << " level: " << mLevel
        << " width: " << mWidth << " height: "<< mHeight << " depth: " << mDepth
        << " format: " << PixelUtil::getFormatName(mFormat)
        << " data format: " << PixelUtil::getFormatName(data.format);
        LogManager::getSingleton().logMessage(LL_Info, str.str());
#endif
#else
        void* pdata = data.getData();
#if OGRE_DEBUG_MODE
        LogManager::getSingleton().logMessage("GLES2TextureBuffer::upload - ID: " + StrConv::conv(mTextureID) +
                                              " Target: " + StrConv::conv(mTarget) +
                                              " Format: " + PixelUtil::getFormatName(data.format) +
                                              " Origin format: " + StrConv::conv(GLES2PixelUtil::getGLOriginFormat(data.format), 0, ' ', std::ios::hex) +
                                              " Data type: " + StrConv::conv(GLES2PixelUtil::getGLOriginDataType(data.format), 0, ' ', std::ios::hex));
#endif
#endif

        if (PixelUtil::isCompress(data.format))
        {
            if(data.format != mFormat || !data.isWholeSlice())
                N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                            "Compressed images must be consecutive, in the source format",
                            "GLES2TextureBuffer::upload");

            GLenum format = GLES2PixelUtil::getGLInternalFormat(mFormat);
            // Data must be consecutive and at beginning of buffer as PixelStorei not allowed
            // for compressed formats
            switch(mTarget) {
                case GL_TEXTURE_2D:
                case GL_TEXTURE_CUBE_MAP:
                        GLCheck(glCompressedTexSubImage2D(mFaceTarget, mLevel,
                                                  dest.left, dest.top,
                                                  dest.getWidth(), dest.getHeight(),
                                                  format, data.getWholeSliceSize(),
                                                  pdata));
                    break;
                case GL_TEXTURE_2D_ARRAY:
                    if(!hasGLES30)
                        break;
                    OGRE_FALLTHROUGH;
                case GL_TEXTURE_3D_OES:
                    GLCheck(glCompressedTexSubImage3DOES(mTarget, mLevel,
                                              dest.left, dest.top, dest.front,
                                              dest.getWidth(), dest.getHeight(), dest.getDepth(),
                                              format, data.getWholeSliceSize(),
                                              pdata));
                    break;
            }
        }
        else
        {
            if (data.getWidth() != data.rowPitch)
            {
                if(!hasGLES30)
                    N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                                "Unsupported texture format",
                                "GLES2TextureBuffer::upload");

                GLCheck(glPixelStorei(GL_UNPACK_ROW_LENGTH, data.rowPitch))
            }

            if (data.getHeight() * data.getWidth() != data.slicePitch)
            {
                if(!hasGLES30)
                    N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                                "Unsupported texture format",
                                "GLES2TextureBuffer::upload");
                GLCheck(glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, (data.slicePitch/data.getWidth())));
            }

            if((data.getWidth()*PixelUtil::getNumElemBytes(data.format)) & 3) {
                // Standard alignment of 4 is not right
                GLCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
            }

            switch(mTarget) {
                case GL_TEXTURE_2D:
                case GL_TEXTURE_CUBE_MAP:
                    GLCheck(glTexSubImage2D(mFaceTarget, mLevel,
                                    dest.left, dest.top,
                                    dest.getWidth(), dest.getHeight(),
                                    GLES2PixelUtil::getGLOriginFormat(data.format), GLES2PixelUtil::getGLOriginDataType(data.format),
                                    pdata));
                    break;
                case GL_TEXTURE_2D_ARRAY:
                    if(!hasGLES30)
                        break;
                    OGRE_FALLTHROUGH;
                case GL_TEXTURE_3D_OES:
                    GLCheck(glTexSubImage3DOES(
                                    mTarget, mLevel,
                                    dest.left, dest.top, dest.front,
                                    dest.getWidth(), dest.getHeight(), dest.getDepth(),
                                    GLES2PixelUtil::getGLOriginFormat(data.format), GLES2PixelUtil::getGLOriginDataType(data.format),
                                    pdata));
                    break;
            }

            // TU_AUTOMIPMAP is only enabled when there are no custom mips
            // so we do not have to care about overwriting
            if ((mUsage & TU_AUTOMIPMAP) && (mLevel == 0)
                    && (hasGLES30 || mTarget == GL_TEXTURE_2D || mTarget == GL_TEXTURE_CUBE_MAP))
            {
                GLCheck(glGenerateMipmap(mTarget));
            }
        }

        // Restore defaults
        if(hasGLES30) {
            GLCheck(glPixelStorei(GL_UNPACK_ROW_LENGTH, 0));
            GLCheck(glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0));
        }

        GLCheck(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
    }

    //-----------------------------------------------------------------------------  
    void GLES2TextureBuffer::download(const PixelBox &data)
    {
        if(data.getWidth() != getWidth() ||
           data.getHeight() != getHeight() ||
           data.getDepth() != getDepth())
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "only download of entire buffer is supported by GL ES",
                        "GLES2TextureBuffer::download");

        if(PixelUtil::isCompress(data.format))
        {
            N_EXCEPT(Exception::ERR_INVALIDPARAMS,
                        "Compressed images cannot be downloaded by GL ES",
                        "GLES2TextureBuffer::download");
        }

        if((data.getWidth()*PixelUtil::getNumElemBytes(data.format)) & 3) {
            // Standard alignment of 4 is not right
            GLCheck(glPixelStorei(GL_PACK_ALIGNMENT, 1));
        }

        GLint currentFBO = 0;
        GLuint tempFBO = 0;
        GLCheck(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFBO));
        GLCheck(glGenFramebuffers(1, &tempFBO));
        GLCheck(glBindFramebuffer(GL_FRAMEBUFFER, tempFBO));

        // Construct a temp PixelBox that is RGBA because GL_RGBA/GL_UNSIGNED_BYTE is the only combination that is
        // guaranteed to work on all platforms.
        size_t sizeInBytes = PixelUtil::getSize(data.getWidth(), data.getHeight(), data.getDepth(), PF_A8B8G8R8);
        PixelBox tempBox = PixelBox(data.getWidth(), data.getHeight(), data.getDepth(), PF_A8B8G8R8);
        tempBox.data = new uint8[sizeInBytes];

        switch (mTarget)
        {
            case GL_TEXTURE_2D:
            case GL_TEXTURE_CUBE_MAP:
                GLCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTextureID, 0));
                GLCheck(glCheckFramebufferStatus(GL_FRAMEBUFFER));
                GLCheck(glReadPixels(0, 0, data.getWidth(), data.getHeight(),
                                                 GL_RGBA,
                                                 GL_UNSIGNED_BYTE,
                                                 tempBox.data));
                break;
        }

        PixelUtil::bulkPixelConversion(tempBox, data);

        delete[] tempBox.data;
        tempBox.data = 0;

        // Restore defaults
        GLCheck(glPixelStorei(GL_PACK_ALIGNMENT, 4));
        GLCheck(glBindFramebuffer(GL_FRAMEBUFFER, currentFBO));
        GLCheck(glDeleteFramebuffers(1, &tempFBO));
    }

    //-----------------------------------------------------------------------------  
    void GLES2TextureBuffer::bindToFramebuffer(uint32 attachment, uint32 zoffset)
    {
        N_assert(zoffset < mDepth);
        GLCheck(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                                                   mFaceTarget, mTextureID, mLevel));
    }
    
    void GLES2TextureBuffer::copyFromFramebuffer(size_t zoffset)
    {
        getGLES2RenderSystem()->getStateManager()->bindGLTexture(mTarget, mTextureID);
        GLCheck(glCopyTexSubImage2D(mFaceTarget, mLevel, 0, 0, 0, 0, mWidth, mHeight));
    }

    //-----------------------------------------------------------------------------  
    void GLES2TextureBuffer::blit(const HardwarePixelBufferSharedPtr &src, const Box &srcBox, const Box &dstBox)
    {
        GLES2TextureBuffer *srct = static_cast<GLES2TextureBuffer *>(src.get());
        // TODO: blitFromTexture currently broken
        // Destination texture must be 2D or Cube
        // Source texture must be 2D
        if (false) //(((src->getUsage() & TU_RENDERTARGET) == 0 && (srct->mTarget == GL_TEXTURE_2D)) ||
            //((srct->mTarget == GL_TEXTURE_3D_OES) && (mTarget != GL_TEXTURE_2D_ARRAY)))
        {
            blitFromTexture(srct, srcBox, dstBox);
        }
        else
        {
            GLES2HardwarePixelBuffer::blit(src, srcBox, dstBox);
        }
    }
    
    //-----------------------------------------------------------------------------  
    // Very fast texture-to-texture blitter and hardware bi/trilinear scaling implementation using FBO
    // Destination texture must be 1D, 2D, 3D, or Cube
    // Source texture must be 1D, 2D or 3D
    // Supports compressed formats as both source and destination format, it will use the hardware DXT compressor
    // if available.
    // @author W.J. van der Laan
    void GLES2TextureBuffer::blitFromTexture(GLES2TextureBuffer *src, const Box &srcBox, const Box &dstBox)
    {
        N_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Not implemented",
                    "GLES2TextureBuffer::blitFromTexture");
        // todo - add a shader attach...
//        std::cerr << "GLES2TextureBuffer::blitFromTexture " <<
//        src->mTextureID << ":" << srcBox.left << "," << srcBox.top << "," << srcBox.right << "," << srcBox.bottom << " " <<
//        mTextureID << ":" << dstBox.left << "," << dstBox.top << "," << dstBox.right << "," << dstBox.bottom << std::endl;

        // Store reference to FBO manager
        GLES2FBOManager *fboMan = static_cast<GLES2FBOManager *>(GLRTTManager::getSingletonPtr());

        GLES2RenderSystem* rs = getGLES2RenderSystem();
        rs->_disableTextureUnitsFrom(0);
        glActiveTexture(GL_TEXTURE0);

        // Disable alpha, depth and scissor testing, disable blending,
        // and disable culling
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);
        glDisable(GL_BLEND);
        glDisable(GL_CULL_FACE);

        // Set up source texture
        rs->getStateManager()->bindGLTexture(src->mTarget, src->mTextureID);

        // Set filtering modes depending on the dimensions and source
        if(srcBox.getWidth() == dstBox.getWidth() &&
           srcBox.getHeight() == dstBox.getHeight() &&
           srcBox.getDepth() == dstBox.getDepth())
        {
            // Dimensions match -- use nearest filtering (fastest and pixel correct)
            if(src->mUsage & TU_AUTOMIPMAP)
            {
                GLCheck(glTexParameteri(src->mTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
            }
            else
            {
                GLCheck(glTexParameteri(src->mTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            }
            GLCheck(glTexParameteri(src->mTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        }
        else
        {
            // Dimensions don't match -- use bi or trilinear filtering depending on the
            // source texture.
            if(src->mUsage & TU_AUTOMIPMAP)
            {
                // Automatic mipmaps, we can safely use trilinear filter which
                // brings greatly improved quality for minimisation.
                GLCheck(glTexParameteri(src->mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
            }
            else
            {
                // Manual mipmaps, stay safe with bilinear filtering so that no
                // intermipmap leakage occurs.
                GLCheck(glTexParameteri(src->mTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
            }
            GLCheck(glTexParameteri(src->mTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        }
        // Clamp to edge (fastest)
        GLCheck(glTexParameteri(src->mTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GLCheck(glTexParameteri(src->mTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

        if(src->getDepth() > 1) {
            GLCheck(glTexParameteri(src->mTarget, GL_TEXTURE_WRAP_R_OES, GL_CLAMP_TO_EDGE));
        }

#if OGRE_NO_GLES3_SUPPORT == 0
        // Set origin base level mipmap to make sure we source from the right mip
        // level.
        GLCheck(glTexParameteri(src->mTarget, GL_TEXTURE_BASE_LEVEL, src->mLevel));
#endif
        // Store old binding so it can be restored later
        GLint oldfb;
        GLCheck(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &oldfb));

        // Set up temporary FBO
        GLCheck(glBindFramebuffer(GL_FRAMEBUFFER, fboMan->getTemporaryFBO()));

        TexturePtr tempTex;
        if(!fboMan->checkFormat(mFormat))
        {
            // If target format not directly supported, create intermediate texture
            tempTex = TextureManager::getSingleton().createManual(
                "GLBlitFromTextureTMP", GroupInner, TEX_TYPE_2D,
                dstBox.getWidth(), dstBox.getHeight(), dstBox.getDepth(), 0,
                fboMan->getSupportedAlternative(mFormat));

            GLCheck(
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                                       static_pointer_cast<GLES2Texture>(tempTex)->getGLID(), 0));
            // Set viewport to size of destination slice
            GLCheck(glViewport(0, 0, dstBox.getWidth(), dstBox.getHeight()));
        }
        else
        {
            // We are going to bind directly, so set viewport to size and position of destination slice
            GLCheck(glViewport(dstBox.left, dstBox.top, dstBox.getWidth(), dstBox.getHeight()));
        }

        // Select scratch VAO #0
        if(rs->getCapabilities()->hasCapability(RSC_VAO))
            rs->getStateManager()->bindGLVertexArray(0);

        // Process each destination slice
        for(size_t slice = dstBox.front; slice < dstBox.back; ++slice)
        {
            if(!tempTex)
            {
                // Bind directly
                bindToFramebuffer(GL_COLOR_ATTACHMENT0, slice);
            }

            // Calculate source texture coordinates
            float u1 = (float)srcBox.left / (float)src->mWidth;
            float v1 = (float)srcBox.top / (float)src->mHeight;
            float u2 = (float)srcBox.right / (float)src->mWidth;
            float v2 = (float)srcBox.bottom / (float)src->mHeight;
            // Calculate source slice for this destination slice
            float w = (float)(slice - dstBox.front) / (float)dstBox.getDepth();
            // Get slice # in source
            w = w * (float)srcBox.getDepth() + srcBox.front;
            // Normalise to texture coordinate in 0.0 .. 1.0
            w = (w+0.5f) / (float)src->mDepth;

            // Finally we're ready to rumble
            rs->getStateManager()->bindGLTexture(src->mTarget, src->mTextureID);
            GLCheck(glEnable(src->mTarget));

            GLfloat squareVertices[] = {
               -1.0f, -1.0f,
                1.0f, -1.0f,
               -1.0f,  1.0f,
                1.0f,  1.0f,
            };
            GLfloat texCoords[] = {
                u1, v1, w,
                u2, v1, w,
                u2, v2, w,
                u1, v2, w
            };

            GLuint posAttrIndex = GLSLProgramBase::getFixedAttributeIndex(VT_Pos, 0);
            GLuint texAttrIndex = GLSLProgramBase::getFixedAttributeIndex(VT_Tex_Coord, 0);

            // Draw the textured quad
            GLCheck(glVertexAttribPointer(posAttrIndex,
                                  2,
                                  GL_FLOAT,
                                  0,
                                  0,
                                  squareVertices));
            GLCheck(glEnableVertexAttribArray(posAttrIndex));
            GLCheck(glVertexAttribPointer(texAttrIndex,
                                  3,
                                  GL_FLOAT,
                                  0,
                                  0,
                                  texCoords));
            GLCheck(glEnableVertexAttribArray(texAttrIndex));

            GLCheck(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

            GLCheck(glDisable(src->mTarget));

            if(tempTex)
            {
                // Copy temporary texture
                rs->getStateManager()->bindGLTexture(mTarget, mTextureID);
                switch(mTarget)
                {
                    case GL_TEXTURE_2D:
                    case GL_TEXTURE_CUBE_MAP:
                        GLCheck(glCopyTexSubImage2D(mFaceTarget, mLevel,
                                            dstBox.left, dstBox.top,
                                            0, 0, dstBox.getWidth(), dstBox.getHeight()));
                        break;
                }
            }
        }
        // Finish up
        if(!tempTex)
        {
            // Generate mipmaps
            if(mUsage & TU_AUTOMIPMAP)
            {
                rs->getStateManager()->bindGLTexture(mTarget, mTextureID);
                GLCheck(glGenerateMipmap(mTarget));
            }
        }

        // Reset source texture to sane state
        rs->getStateManager()->bindGLTexture(src->mTarget, src->mTextureID);

#if OGRE_NO_GLES3_SUPPORT == 0
        GLCheck(glTexParameteri(src->mTarget, GL_TEXTURE_BASE_LEVEL, 0));
#endif
        // Detach texture from temporary framebuffer
        GLCheck(glFramebufferRenderbuffer(
            GL_FRAMEBUFFER, PixelUtil::isDepth(mFormat) ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0,
            GL_RENDERBUFFER, 0));

        // Restore old framebuffer
        GLCheck(glBindFramebuffer(GL_FRAMEBUFFER, oldfb));
        if(tempTex)
            TextureManager::getSingleton().remove(tempTex);
    }
    //-----------------------------------------------------------------------------  
    // blitFromMemory doing hardware trilinear scaling
    void GLES2TextureBuffer::blitFromMemory(const PixelBox &src, const Box &dstBox)
    {
        // Fall back to normal GLFrameBuffer::blitFromMemory in case 
        // the source dimensions match the destination ones, in which case no scaling is needed
        // FIXME: always uses software path, as blitFromTexture is not implemented
        if(true ||
           (src.getWidth() == dstBox.getWidth() &&
            src.getHeight() == dstBox.getHeight() &&
            src.getDepth() == dstBox.getDepth()))
        {
            GLES2HardwarePixelBuffer::blitFromMemory(src, dstBox);
            return;
        }
        if(!mBuffer.contains(dstBox))
            N_EXCEPT(Exception::ERR_INVALIDPARAMS, "Destination box out of range",
                        "GLES2TextureBuffer::blitFromMemory");

        TextureType type = (src.getDepth() != 1) ? TEX_TYPE_3D : TEX_TYPE_2D;

        // Set automatic mipmap generation; nice for minimisation
        TexturePtr tex = TextureManager::getSingleton().createManual(
            "GLBlitFromMemoryTMP", GroupInner, type,
            src.getWidth(), src.getHeight(), src.getDepth(), MIP_UNLIMITED, src.format);

        // Upload data to 0,0,0 in temporary texture
        Box tempTarget(0, 0, 0, tex->getWidth(), tex->getHeight(), tex->getDepth());
        tex->getBuffer()->blitFromMemory(src, tempTarget);

        // Blit from texture
        blit(tex->getBuffer(), tempTarget, dstBox);

        // Delete temp texture
        TextureManager::getSingleton().remove(tex);
    }
    
    RenderTexture *GLES2TextureBuffer::getRenderTarget(size_t zoffset)
    {
        N_assert(mUsage & TU_RENDERTARGET);
        N_assert(zoffset < mDepth);
        return mSliceTRT[zoffset];
    }
    
    //********* GLES2RenderBuffer
    //----------------------------------------------------------------------------- 
    GLES2RenderBuffer::GLES2RenderBuffer(GLenum format, uint32 width, uint32 height, GLsizei numSamples):
    GLES2HardwarePixelBuffer(width, height, 1, GLES2PixelUtil::getClosestOGREFormat(format), HBU_WRITE_ONLY)
    {
        GLES2RenderSystem* rs = getGLES2RenderSystem();

        mGLInternalFormat = format;
        mNumSamples = numSamples;
        
        // Generate renderbuffer
        GLCheck(glGenRenderbuffers(1, &mRenderbufferID));

        // Bind it to FBO
        GLCheck(glBindRenderbuffer(GL_RENDERBUFFER, mRenderbufferID));

        if(rs->getCapabilities()->hasCapability(RSC_DEBUG))
        {
            GLCheck(glLabelObjectEXT(GL_RENDERBUFFER, mRenderbufferID, 0, ("RB " + StrConv::conv(mRenderbufferID) + " MSAA: " + StrConv::conv(mNumSamples)).c_str()));
        }

        // Allocate storage for depth buffer
        if (mNumSamples > 0)
        {
            if(rs->hasMinGLVersion(3, 0) || rs->checkGpuApi("GL_APPLE_framebuffer_multisample"))
            {
                GLCheck(glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER,
                                                                          mNumSamples, mGLInternalFormat, mWidth, mHeight));
            }
        }
        else
        {
            GLCheck(glRenderbufferStorage(GL_RENDERBUFFER, mGLInternalFormat,
                                                      mWidth, mHeight));
        }
    }
    //----------------------------------------------------------------------------- 
    GLES2RenderBuffer::~GLES2RenderBuffer()
    {
        GLCheck(glDeleteRenderbuffers(1, &mRenderbufferID));
    }
    //-----------------------------------------------------------------------------  
    void GLES2RenderBuffer::bindToFramebuffer(uint32 attachment, uint32 zoffset)
    {
        N_assert(zoffset < mDepth);
        GLCheck(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment,
                                                      GL_RENDERBUFFER, mRenderbufferID));
    }
}
