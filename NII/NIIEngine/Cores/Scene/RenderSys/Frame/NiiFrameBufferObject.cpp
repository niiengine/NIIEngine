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

#include "NiiPreProcess.h"

#include "NiiFrameBufferObject.h"
#include "NiiTexture.h"
#include "NiiPixelFormat.h"
#include "NiiStrConv.h"
#include "NiiException.h"

namespace Ogre
{
    //-----------------------------------------------------------------------------------
    bool FboKeyEqual(const FrameTarget & o1, const FrameTarget & o2) const
    {
        return o1.mTexture == o2.mTexture &&
                o1.mTarget == o2.mTarget &&
                o1.mMip == o2.mMip &&
                o1.mTargetMip == o2.mTargetMip &&
                o1.mArrayIndex == o2.mArrayIndex &&
                o1.mTargetArrayIndex == o2.mTargetArrayIndex;
    }
    //-----------------------------------------------------------------------------------
    bool FboKeyNonEqual(const FrameTarget & o1, const FrameTarget & o2) const
    {
        return o1.mTexture != o2.mTexture ||
                o1.mTarget != o2.mTarget ||
                o1.mMip != o2.mMip ||
                o1.mTargetMip != o2.mTargetMip ||
                o1.mArrayIndex != o2.mArrayIndex ||
                o1.mTargetArrayIndex != o2.mTargetArrayIndex;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // FrameTarget
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    FrameTarget::FrameTarget()
    {
        memset(this, 0, sizeof(*this));
    }
    //-----------------------------------------------------------------------------------
    bool FrameTarget::operator != (const FrameTarget & o) const
    {
        return mTexture != o.mTexture ||
                mTarget != o.mTarget ||
                mMip != o.mMip ||
                mTargetMip != o.mTargetMip ||
                mArrayIndex != o.mArrayIndex ||
                mTargetArrayIndex != o.mTargetArrayIndex ||
                mInitType != o.mInitType ||
                mStoreType != o.mStoreType;
    }
    //-----------------------------------------------------------------------------------
    bool FrameTarget::operator == (const FrameTarget & o) const
    {
        return mTexture == o.mTexture &&
                mTarget == o.mTarget &&
                mMip == o.mMip &&
                mTargetMip == o.mTargetMip &&
                mArrayIndex == o.mArrayIndex &&
                mTargetArrayIndex == o.mTargetArrayIndex &&
                mInitType == o.mInitType &&
                mStoreType == o.mStoreType;
    }
    //-----------------------------------------------------------------------------------
    bool FrameTarget::operator < (const FrameTarget & o) const
    {
        if(mTexture != o.mTexture)
            return mTexture < o.mTexture;
        if(mTarget != o.mTarget)
            return mTarget < o.mTarget;
        if(mMip != o.mMip)
            return mMip < o.mMip;
        if(mTargetMip != o.mTargetMip)
            return mTargetMip < o.mTargetMip;
        if(mArrayIndex != o.mArrayIndex)
            return mArrayIndex < o.mArrayIndex;
        if(mTargetArrayIndex != o.mTargetArrayIndex)
            return mTargetArrayIndex < o.mTargetArrayIndex;
        if(mInitType != o.mInitType)
            return mInitType < o.mInitType;
        if(mStoreType != o.mStoreType)
            return mStoreType < o.mStoreType;

        return false;
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // FrameBufferObject
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    FrameBufferObject::FrameBufferObject() :
        mColourCount(0),
        mClearDepth(1.0f),
        mClearStencil(0),
        clearColour(Colour::Black),
        mAllArray(false),
        mTextureFlipping(false),
        mStepping(false)
        mDepthReadOnly(false),
        mStencilReadOnly(false),
    {
    }
    //-----------------------------------------------------------------------------------
    FrameBufferObject::~FrameBufferObject()
    {
    }
    //-----------------------------------------------------------------------------------
    /*void FrameBufferObject::checkWarnIfRtvWasFlushed(Nmark type)
    {
        bool mustWarn = false;

        for( size_t i = 0; i < mColourCount && !mustWarn; ++i )
        {
            if(mColour[i].mInitType == FrameTarget::IT_Texture && (type & (FrameBufferObject::TT_Colour0 << i)))
            {
                mustWarn = true;
            }
        }

        if(mColour[NII_DepthTargetIndex].mInitType == FrameTarget::IT_Texture && mColour[NII_DepthTargetIndex].mTexture &&
            (type & FrameBufferObject::TT_Depth))
        {
            mustWarn = true;
        }

        if(mColour[NII_StencilTargetIndex].mInitType == FrameTarget::IT_Texture && mColour[NII_StencilTargetIndex].mTexture &&
            (type & FrameBufferObject::TT_Stencil))
        {
            mustWarn = true;
        }

        if( mustWarn )
        {
            N_EXCEPT(Internal,
                         "FrameObj is getting flushed sooner than expected. "
                         "This is a performance and/or correctness warning and "
                         "the developer explicitly told us to warn you. Disable "
                         "warn_if_rtv_was_flushed / FusionMatedate::Op::mWarnIfRtvWasFlushed "
                         "to ignore this; use a FrameTarget::IT_Clear or don't break the two "
                         "passes with something in the middle that causes the flush.");
        }
    }*/
    //-----------------------------------------------------------------------------------
    void FrameBufferObject::syncType(Nmark type)
    {
        if(type & FrameBufferObject::TT_Colour)
        {
            mColourCount = 0;

            mTextureFlipping = false;

            while(mColourCount < NII_MAX_RenderTarget && mColour[mColourCount].mTexture)
            {
                const FrameTarget & clrFrame = mColour[mColourCount];

                if( clrFrame.mTexture->isWindow() )
                {
                    FrameTarget & clr = mColour[mColourCount];
                    if(!clrFrame.mTexture->isMultiSample() && clrFrame.mTarget)
                    {
                        clr.mTarget = 0;
                        clr.mStoreType = FrameTarget::ST_Store;
                    }
                    else if(clrFrame.mTexture->isMultiSample() && !clrFrame.mTarget && !clrFrame.mTexture->isManualMSAA())
                    {
                        clr.mTarget = clr.mTexture;
                        clr.mStoreType = FrameTarget::ST_Multisample;
                    }
                }

                if(clrFrame.mStoreType == FrameTarget::ST_Multisample && !clrFrame.mTarget)
                {
                    N_EXCEPT(Internal, "When mStoreType == FrameTarget::ST_Multisample, there MUST be a resolve texture set.");
                }

                if(clrFrame.mTarget)
                {
                    if(!clrFrame.mTexture->isMultiSample())
                    {
                        N_EXCEPT(Internal, "Resolve Texture '" + clrFrame.mTarget->getName() +
                            "' specified, but texture to render to '" + clrFrame.mTexture->getName() + "' is not MSAA");
                    }
                    if(clrFrame.mTarget == clrFrame.mTexture && (clrFrame.mMip != 0 || clrFrame.mArrayIndex != 0))
                    {
                        N_EXCEPT(Internal, "MSAA textures can only render to mip 0 and slice 0 "
                            "unless using explicit resolves. Texture: " + clrFrame.mTexture->getName());
                    }
                }

                if(clrFrame.mAllArray && clrFrame.mArrayIndex != 0)
                {
                    N_EXCEPT(Internal, "Layered Rendering (i.e. binding 2D array or cubemap) "
                        "only supported if slice = 0. Texture: " + clrFrame.mTexture->getName());
                }

                ++mColourCount;
            }

            /*mTextureFlipping = false;

            for(size_t i = 0; i < mColourCount && !mTextureFlipping; ++i)
            {
                Texture * texture = mColour[i].mTexture;
                mTextureFlipping = texture->isFlipping();
            }

            if( !mTextureFlipping && mColour[NII_DepthTargetIndex].mTexture)
                mTextureFlipping = mColour[NII_DepthTargetIndex].mTexture->isFlipping();

            if( !mTextureFlipping && mColour[NII_StencilTargetIndex].mTexture)
                mTextureFlipping = mColour[NII_StencilTargetIndex].mTexture->isFlipping();*/
        }

        if( mColourCount == 0 && !mColour[NII_DepthTargetIndex].mTexture && !mColour[NII_StencilTargetIndex].mTexture )
        {
            N_EXCEPT(Internal, "FrameBufferObject has no colour, depth nor stencil attachments!" );
        }

        if(mColour[NII_DepthTargetIndex].mTexture && !PixelUtil::isDepth(mColour[NII_DepthTargetIndex].mTexture->getFormat()))
        {
            N_EXCEPT(Internal, "FrameBufferObject depth attachment '" + mColour[NII_DepthTargetIndex].mTexture->getName() + "' is not a depth format!");
        }

        if(mColour[NII_StencilTargetIndex].mTexture && !PixelUtil::isStencil(mColour[NII_StencilTargetIndex].mTexture->getFormat()))
        {
            N_EXCEPT(Internal, "FrameBufferObject stencil attachment '" + mColour[NII_StencilTargetIndex].mTexture->getName() + "' is not a stencil format!" );
        }

        if(mColour[NII_DepthTargetIndex].mTexture)
        {
            for(size_t i = 0; i < mColourCount; ++i)
            {
                if(!mColour[NII_DepthTargetIndex].mTexture->isDepthStencilSupport(mColour[i].mTexture))
                {
                    N_EXCEPT(Internal,
                                 "Manually specified depth buffer '" +
                                 mColour[NII_DepthTargetIndex].mTexture->getName() + "' is incompatible with colour RTT #" +
                                 StringConverter::toString( i ) + "'" + mColour[i].mTexture->getName()
                                 + "\nColour: " + mColour[i].mTexture->getSettingsDesc()
                                 + "\nDepth: " + mColour[NII_DepthTargetIndex].mTexture->getSettingsDesc());
                }
            }
            
        }

        if(mColour[NII_StencilTargetIndex].mTexture && mColour[NII_StencilTargetIndex].mTexture != mColour[NII_DepthTargetIndex].mTexture)
        {
            for(size_t i = 0; i < mColourCount; ++i)
            {
                if(!mColour[NII_StencilTargetIndex].mTexture->isDepthStencilSupport(mColour[i].mTexture))
                {
                    N_EXCEPT(Internal,
                                 "Manually specified stencil buffer '" +
                                 mColour[NII_StencilTargetIndex].mTexture->getName() + "' is incompatible with colour RTT #" +
                                 StringConverter::toString( i ) + "'" + mColour[i].mTexture->getName()
                                 + "'\nColour: " + mColour[i].mTexture->getSettingsDesc()
                                 + "\nStencil: " + mColour[NII_StencilTargetIndex].mTexture->getSettingsDesc());
                }
            }
            
        }

        mTextureFlipping = false;

        for(size_t i = 0; i < mColourCount && !mTextureFlipping; ++i)
        {
            mTextureFlipping = mColour[i].mTexture->isFlipping();
        }

        if(!mTextureFlipping && mColour[NII_DepthTargetIndex].mTexture)
            mTextureFlipping = mColour[NII_DepthTargetIndex].mTexture->isFlipping();

        if(!mTextureFlipping && mColour[NII_StencilTargetIndex].mTexture)
            mTextureFlipping = mColour[NII_StencilTargetIndex].mTexture->isFlipping();
    }
    //-----------------------------------------------------------------------------------
    void FrameBufferObject::setClearColour(Nidx idx, const Colour & clr)
    {
        assert(idx < NII_MAX_RenderTarget);
        mColour[idx].clearColour = clr;
    }
    //-----------------------------------------------------------------------------------
    void FrameBufferObject::setClearColour(const Colour & clr)
    {
        for(uint8 i = 0; i < mColourCount; ++i)
            mColour[i].clearColour = clr;
    }
    //-----------------------------------------------------------------------------------
    void FrameBufferObject::setClearDepth(NIIf depth)
    {
        mClearDepth = depth;
    }
    //-----------------------------------------------------------------------------------
    void FrameBufferObject::setClearStencil(Nui32 stencil)
    {
        mClearStencil = stencil;
    }
    //-----------------------------------------------------------------------------------
    bool FrameBufferObject::isTargetEqual(const FrameBufferObject * o) const
    {
        if(!o)
            return false;

        if(mColourCount != o->mColourCount)
            return false;

        for(size_t i = 0; i < mColourCount; ++i)
        {
            if(mColour[i].mTexture != o->mColour[i].mTexture ||
               mColour[i].mTarget != o->mColour[i].mTarget ||
               mColour[i].mMip != o->mColour[i].mMip ||
               mColour[i].mTargetMip != o->mColour[i].mTargetMip ||
               mColour[i].mArrayIndex != o->mColour[i].mArrayIndex ||
               mColour[i].mTargetArrayIndex != o->mColour[i].mTargetArrayIndex )
            {
                return false;
            }
        }

        if(mColour[NII_DepthTargetIndex].mTexture != o->mColour[NII_DepthTargetIndex].mTexture || 
            mColour[NII_StencilTargetIndex].mTexture != o->mColour[NII_StencilTargetIndex].mTexture )
        {
            return false;
        }

        return true;
    }
    //-----------------------------------------------------------------------------------
    bool FrameBufferObject::isTarget(const Texture * tex) const
    {
        for(size_t i = 0; i < mColourCount; ++i)
        {
            if(mColour[i].mTexture == tex)
                return true;
        }

        if(mColour[NII_DepthTargetIndex].mTexture == tex || mColour[NII_StencilTargetIndex].mTexture == tex)
            return true;

        return false;
    }
    //-----------------------------------------------------------------------------------
    bool FrameBufferObject::isStencilFeature() const
    {
        return mColour[NII_StencilTargetIndex].mTexture || (mColour[NII_DepthTargetIndex].mTexture && PixelUtil::isStencil(mColour[NII_DepthTargetIndex].mTexture->getFormat()));
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    // FboKey
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    struct _EngineAPI FboKey
    {
        FboKey()    { memset(this, 0, sizeof(*this)); }
        FboKey(const FrameBufferObject & o)
        {
            memset(this, 0, sizeof(*this));
            mColourCount = o.getColourCount();

            //Load & Store actions don't matter for generating different FBOs.

            for(size_t i = 0; i < mColourCount; ++i)
            {
                mColour[i] = o.mColour[i];
                mAllArray[i] = o.mColour[i].mAllArray;
            }

            mColour[NII_DepthTargetIndex] = o.mColour[NII_DepthTargetIndex];
            mColour[NII_StencilTargetIndex] = o.mColour[NII_StencilTargetIndex];
        }

        bool operator == (const FboKey & o) const
        {
            if(mColourCount != o.mColourCount)
                return false;

            for(size_t i = 0; i < mColourCount; ++i)
            {
                if(mAllArray[i] != o.mAllArray[i])
                    return false;
                if(FboKeyNonEqual(mColour[i], o.mColour[i]))
                    return false;
            }

            if(FboKeyNonEqual(mColour[NII_DepthTargetIndex], o.mColour[NII_DepthTargetIndex]))
                return false;
            if(FboKeyNonEqual(mColour[NII_StencilTargetIndex], o.mColour[NII_StencilTargetIndex]))
                return false;

            return true;
        }
        bool operator < (const FboKey & o) const
        {
            if(mColourCount != o.mColourCount)
                return mColourCount < o.mColourCount;

            for(size_t i = 0; i < mColourCount; ++i)
            {
                if(mAllArray[i] != o.mAllArray[i])
                    return mAllArray[i] < o.mAllArray[i];
                if(mColour[i] != o.mColour[i])
                    return mColour[i] < o.mColour[i];
            }

            if(FboKeyNonEqual(mColour[NII_DepthTargetIndex], o.mColour[NII_DepthTargetIndex]))
                return mColour[NII_DepthTargetIndex] < o.mColour[NII_DepthTargetIndex];
            if(FboKeyNonEqual(mColour[NII_StencilTargetIndex], o.mColour[NII_StencilTargetIndex]))
                return mColour[NII_StencilTargetIndex] < o.mColour[NII_StencilTargetIndex];

            return false;
        }

        FrameTarget mColour[NII_MAX_RenderTargetDS];
        Nui32 mColourCount;
        bool mAllArray[NII_MAX_RenderTarget];
    };
    //-----------------------------------------------------------------------------------
}
