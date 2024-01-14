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


#include "NiiGLCommon.h"
#include "NiiGLRenderSystemBase.h"
#include "NiiGLSLProgramBase.h"
#include "NiiGLSupport.h"
#include "NiiException.h"
#include "NiiEngine.h"
#include "NiiLogManager.h"

namespace NII 
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GLVertexArrayObject
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    GLVertexArrayObject::GLVertexArrayObject() : 
        mCreatorContext(0), 
        mVAO(0), 
        mNeedsUpdate(true), 
        mVertexStart(0) 
    {
    }
    //-----------------------------------------------------------------------
    GLVertexArrayObject::~GLVertexArrayObject()
    {
        if(mVAO != 0)
        {
            GLRenderSystemBase * rs = static_cast<GLRenderSystemBase *>(N_Engine().getRender());
            rs->_destroyVao(mCreatorContext, mVAO);
        }
    }
    //-----------------------------------------------------------------------
    void GLVertexArrayObject::bind(GLRenderSystemBase * rs)
    {
        if(mCreatorContext && mCreatorContext != rs->_getCurrentContext()) // VAO is unusable with current context, destroy it
        {
            if(mVAO != 0)
                rs->_destroyVao(mCreatorContext, mVAO);
            mCreatorContext = 0;
            mVAO = 0;
            mNeedsUpdate = true;
        }
        if(!mCreatorContext && rs->getFeature()->isSupport(GF_GL_VArrayObj)) // create VAO lazy or recreate after destruction
        {
            mCreatorContext = rs->_getCurrentContext();
            mVAO = rs->_createVao();
            mNeedsUpdate = true;
        }
        rs->_bindVao(mCreatorContext, mVAO);
        
        if(mNeedsUpdate)
            goto bindupadte;

        VertexUnitList::const_iterator elemIter, elemEnd = mVertex->mUnits.end();
        for (elemIter = mVertex->mUnits.begin(); elemIter != elemEnd; ++elemIter)
        {
            const VertexUnit & elem = *elemIter;

            Nui16 source = elem.mSource;

            if (mVertex->isAttach(source))
            {
                NIIi attrib = N_Only(GLSL::GLSLProgram)->getAttributeIndex(elem.mVType, elem.mIndex);

                VertexBuffer * vb = mVertex->getAttach(source);
                if (std::find(mAttribsBound.begin(), mAttribsBound.end(), std::make_pair(attrib, vb)) == mAttribsBound.end())
                    goto bindupadte;

                if (elem.mDivisor && std::find(mInstanceAttribsBound.begin(), mInstanceAttribsBound.end(), attrib) == mInstanceAttribsBound.end())
                    goto bindupadte;
            }
        }

        if(mVertex->mOffset != mVertexStart) 
        {
            goto bindupadte;
        }
        return;
        
    bindupadte:
        
        mAttribsBound.clear();
        mInstanceAttribsBound.clear();

        VertexUnitList::const_iterator elemIter, elemEnd = mVertex->mUnits.end();
        for (elemIter = mVertex->mUnits.begin(); elemIter != elemEnd; ++elemIter)
        {
            const VertexUnit & elem = *elemIter;

            Nui16 source = elem.mSource;

            VertexBuffer * vb = mVertex->getAttach(source);
            if (vb)
            {
                Nui32 attrib = N_Only(GLSL::GLSLProgram)->getAttributeIndex(elem.mVType, elem.mIndex);

                mAttribsBound.push_back(std::make_pair(attrib, vb));

                rs->bindVertexElementToGpu(elem, vb, mVertex->mOffset);

                if (elem.mDivisor)
                    mInstanceAttribsBound.push_back(attrib);
            }
        }

        mVertexStart = mVertex->mOffset;
        mNeedsUpdate = false;

    }
    //-----------------------------------------------------------------------
    /*bool GLVertexArrayObject::needsUpdate(VertexData * vd, size_t vertexOft)
    {
        if(mNeedsUpdate)
            return true;

        VertexUnitList::const_iterator elemIter, elemEnd = vd->mUnits.end();
        for (elemIter = vd->mUnits.begin(); elemIter != elemEnd; ++elemIter)
        {
            const VertexUnit & elem = *elemIter;

            Nui16 source = elem.mSource;

            if (vd->isAttach(source))
            {
                NIIi attrib = N_Only(GLSL::GLSLProgram)->getAttributeIndex(elem.mVType, elem.mIndex);

                VertexBuffer * vb = vd->getAttach(source);
                if (std::find(mAttribsBound.begin(), mAttribsBound.end(), std::make_pair(attrib, vb)) == mAttribsBound.end())
                    return true;

                if (elem.mDivisor && std::find(mInstanceAttribsBound.begin(), mInstanceAttribsBound.end(), attrib) == mInstanceAttribsBound.end())
                    return true;
            }
        }

        if(vertexOft != mVertexStart) 
        {
            return true;
        }

        return false;
    }
    //-----------------------------------------------------------------------
    void GLVertexArrayObject::bindToGpu(GLRenderSystemBase * rs, VertexData* vd, size_t vertexOft)
    {
        mAttribsBound.clear();
        mInstanceAttribsBound.clear();

        VertexUnitList::const_iterator elemIter, elemEnd = vd->mUnits.end();
        for (elemIter = vd->mUnits.begin(); elemIter != elemEnd; ++elemIter)
        {
            const VertexUnit & elem = *elemIter;

            Nui16 source = elem.mSource;

            VertexBuffer * vb = vd->getAttach(source);
            if (vb)
            {
                Nui32 attrib = N_Only(GLSL::GLSLProgram)->getAttributeIndex(elem.mVType, elem.mIndex);

                mAttribsBound.push_back(std::make_pair(attrib, vb));

                rs->bindVertexElementToGpu(elem, vb, vertexOft);

                if (elem.mDivisor)
                    mInstanceAttribsBound.push_back(attrib);
            }
        }

        mVertexStart = vertexOft;
        mNeedsUpdate = false;
    }*/
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // GLTextureBase
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    bool GLTextureBase::getPlatformData(const String & name, void * pData) const
    {
        if (name == _T("GLID"))
        {
            *static_cast<Nui32*>(pData) = mTextureID;
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    //---------------------------------------------------------------------
    // GLDepthBufferBase
    //---------------------------------------------------------------------
	//---------------------------------------------------------------------
	GLDepthBufferBase::GLDepthBufferBase(GroupID poolId, GLRenderSystemBase * rsys,
        GLContext * glcontext, GLFrameBufferBase * depth, GLFrameBufferBase * stencil,
		NCount width, NCount height, Nui32 fsaa, bool manual) :
            ExtFrameBuffer(poolId, width, height, 0, fsaa, _T(""), manual),
            mCreatorContext(glcontext),
            mDepthBuffer(depth),
            mStencilBuffer(stencil),
            mRenderSys(rsys)
	{

	}
	//---------------------------------------------------------------------
	GLDepthBufferBase::~GLDepthBufferBase()
	{
		if(mStencilBuffer && mStencilBuffer != mDepthBuffer)
		{
			N_delete mStencilBuffer;
			mStencilBuffer = 0;
		}

		if(mDepthBuffer)
		{
			N_delete mDepthBuffer;
			mDepthBuffer = 0;
		}
	}
	//---------------------------------------------------------------------
	bool GLDepthBufferBase::isSuitable(FrameObj * fo) const
	{
		bool retVal = false;

		//Check standard stuff first.
		if(mRenderSys->getFeature()->isSupport(GF_RenderTarget_LessDepth))
		{
			if(!ExtFrameBuffer::isSuitable(fo))
				return false;
		}
		else
		{
			if(getWidth() != fo->getWidth() || getHeight() != fo->getHeight() || getFsaa() != fo->getFSAA())
                return false;
		}

		//Now check this is the appropriate format
        GLFrameBufferObjectBase * fbo = dynamic_cast<GLRenderTarget *>(fo)->getFBO();

		if(!fbo)
		{
            GLContext * ctx = dynamic_cast<GLRenderTarget *>(fo)->getContext();

			//Non-FBO targets and FBO depth surfaces don't play along, only dummies which match the same
			//context
			if(!mDepthBuffer && !mStencilBuffer && (!ctx || mCreatorContext == ctx))
				retVal = true;
		}
		else
		{
			//Check this isn't a dummy non-FBO depth buffer with an FBO target, don't mix them.
			//If you don't want depth buffer, use a Null Depth Buffer, not a dummy one.
			if(mDepthBuffer || mStencilBuffer)
			{
                PixelFormat internalFormat = fbo->getFormat();
				Nui32 depthFormat, stencilFormat;
				mRenderSys->_getDepthStencilFormatFor(internalFormat, &depthFormat, &stencilFormat);

				bool bSameDepth = false;

				if(mDepthBuffer)
					bSameDepth |= mDepthBuffer->getGLFormat() == depthFormat;

				bool bSameStencil = false;

                if(!mStencilBuffer || mStencilBuffer == mDepthBuffer)
                    bSameStencil = stencilFormat == 0;
                else
                {
                    if(mStencilBuffer)
                        bSameStencil = stencilFormat == mStencilBuffer->getGLFormat();
                }
				retVal = PixelUtil::isDepth(internalFormat) ? bSameDepth : (bSameDepth && bSameStencil);
			}
		}
		return retVal;
	}
    //---------------------------------------------------------------------
    //---------------------------------------------------------------------
    // GLFrameBufferBase
    //---------------------------------------------------------------------
    //------------------------------------------------------------------
    GLFrameBufferBase::GLFrameBufferBase(NCount w, NCount h, NCount d, PixelFormat pf, Nmark mode):
            FrameBuffer(N_OnlyPtr(GLGBuffer), w, h, d, pf, mode & ~Buffer::M_HOST),
            mBuffer(w, h, d, pf),
            mGLInternalFormat(GL_NONE)
    {
        mCurrentLockOptions = (MuteMode)0;
    }
    //-----------------------------------------------------------------------------
    GLFrameBufferBase::~GLFrameBufferBase()
    {
        N_free(mBuffer.mData);
    }
    //-----------------------------------------------------------------------------
    void GLFrameBufferBase::allocateBuffer()
    {
        if(mBuffer.mData)
            // Already allocated
            return;
        mBuffer.mData = N_alloc_t(Nui8, mSize);
    }
    //-----------------------------------------------------------------------------
    void GLFrameBufferBase::freeBuffer()
    {
        // Free buffer if we're STATIC to save memory
        if(mMark & Buffer::M_DEV)
        {
            N_free(mBuffer.mData);
            mBuffer.mData = 0;
        }
    }
    //-----------------------------------------------------------------------------
    PixelBlock GLFrameBufferBase::lockImpl(Nmark m, const Box & lockBox)
    {
        allocateBuffer();
        if((mMark & Buffer::M_READ) || (m & MM_READ))
        {
            download(mBuffer);
        }
        mCurrentLockOptions = m;
        mMutexArea = lockBox;
        return mBuffer.getSub(lockBox);
    }
    //-----------------------------------------------------------------------------
    void GLFrameBufferBase::unlockImpl(void)
    {
        if(mCurrentLockOptions & Buffer::MM_WRITE)
        {
            upload(mMutexData, mMutexArea);
        }
        freeBuffer();
    }
	//-----------------------------------------------------------------------------
	Buffer * GLFrameBufferBase::clone(Nmark m) const
	{
		return 0;
	}
    //-----------------------------------------------------------------------------
    void GLFrameBufferBase::upload(const PixelBlock & data, const Box & dest)
    {
        N_EXCEPT(UnImpl, _I18n("not impl"));
    }
    //-----------------------------------------------------------------------------
    void GLFrameBufferBase::download(const PixelBlock & data)
    {
        N_EXCEPT(UnImpl, _I18n("not impl"));
    }
    //-----------------------------------------------------------------------------
    void GLFrameBufferBase::bindToFramebuffer(Nui32 attachment, NCount zoft)
    {
        N_EXCEPT(UnImpl, _I18n("not impl"));
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GLUniformCache
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    void GLUniformCache::clearCache()
    {
#ifdef N_ENABLE_STATE_CACHE
        mUniformValueMap.clear();
#endif
    }
    //-----------------------------------------------------------------------------
    bool GLUniformCache::updateUniform(NIIi location, const void *value, NCount length)
    {
#ifdef N_ENABLE_STATE_CACHE
        Nui32 current = mUniformValueMap[location];
        Nui32 hash = NII::FastHash((const char *)value, length);
        // First check if the uniform name is in the map. If not, this is new so insert it into the map.
        if (!current || (current != hash))
        {
            // Haven't cached this state yet or the value has changed
            mUniformValueMap[location] = hash;
            return true;
        }

        return false;
#else
        return true;
#endif
    }
    //-----------------------------------------------------------------------------
}