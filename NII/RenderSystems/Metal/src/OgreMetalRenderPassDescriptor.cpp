/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2017 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "OgreMetalRenderPassDescriptor.h"

#include "OgreMetalTextureGpu.h"
#include "OgreMetalTextureGpuWindow.h"
#include "OgreMetalRenderSystem.h"

#include "OgreHlmsDatablock.h"
#include "OgrePixelFormatGpuUtils.h"

#include <execinfo.h> //backtrace

namespace Ogre
{
    MetalRenderPassDescriptor::MetalRenderPassDescriptor( MetalDevice *device,
                                                          MetalRenderSystem *renderSystem ) :
        mDepthAttachment( 0 ),
        mStencilAttachment( 0 ),
        mRequiresManualResolve( false ),
        mSharedFboItor( renderSystem->_getFrameBufferDescMap().end() ),
        mDevice( device ),
        mRenderSys( renderSystem )
  #if OGRE_DEBUG_MODE
      , mNumCallstackEntries( 0 )
  #endif
    {
#if OGRE_DEBUG_MODE
        memset( mCallstackBacktrace, 0, sizeof( mCallstackBacktrace ) );
#endif
    }
    //-----------------------------------------------------------------------------------
    MetalRenderPassDescriptor::~MetalRenderPassDescriptor()
    {
        for( size_t i=0u; i<mColourCount; ++i )
        {
            mColourAttachment[i] = 0;
            mResolveColourAttachm[i] = 0;
        }
        mDepthAttachment = 0;
        mStencilAttachment = 0;

        MetalFrameBufferDescMap &frameBufferDescMap = mRenderSys->_getFrameBufferDescMap();
        if( mSharedFboItor != frameBufferDescMap.end() )
        {
            --mSharedFboItor->second.refCount;
            if( !mSharedFboItor->second.refCount )
                frameBufferDescMap.erase( mSharedFboItor );
            mSharedFboItor = frameBufferDescMap.end();
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::checkRenderWindowStatus(void)
    {
        if( (mColourCount > 0 && mColour[0].texture->isWindow()) ||
            (mDepth.texture && mDepth.texture->isWindow()) ||
            (mStencil.texture && mStencil.texture->isWindow()) )
        {
            if( mColourCount > 1u )
            {
                N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                             "Cannot use RenderWindow as MRT with other colour textures",
                             "MetalRenderPassDescriptor::colourEntriesModified" );
            }

            if( (mColourCount > 0 && !mColour[0].texture->isWindow()) ||
                (mDepth.texture && !mDepth.texture->isWindow()) ||
                (mStencil.texture && !mStencil.texture->isWindow()) )
            {
                N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                             "Cannot mix RenderWindow colour texture with depth or stencil buffer "
                             "that aren't for RenderWindows, or viceversa",
                             "MetalRenderPassDescriptor::checkRenderWindowStatus" );
            }
        }

        calculateSharedKey();
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::calculateSharedKey(void)
    {
        FboKey key( *this );
        MetalFrameBufferDescMap &frameBufferDescMap = mRenderSys->_getFrameBufferDescMap();
        MetalFrameBufferDescMap::iterator newItor = frameBufferDescMap.find( key );

        if( newItor == frameBufferDescMap.end() )
        {
            MetalFrameBufferDescValue value;
            value.refCount = 0;
            frameBufferDescMap[key] = value;
            newItor = frameBufferDescMap.find( key );
        }

        ++newItor->second.refCount;

        if( mSharedFboItor != frameBufferDescMap.end() )
        {
            --mSharedFboItor->second.refCount;
            if( !mSharedFboItor->second.refCount )
                frameBufferDescMap.erase( mSharedFboItor );
        }

        mSharedFboItor = newItor;
    }
    //-----------------------------------------------------------------------------------
    MTLLoadAction MetalRenderPassDescriptor::get( InitType::InitType action )
    {
        switch( action )
        {
        case FrameTarget::IT_None:
            return MTLLoadActionDontCare;
        case FrameTarget::IT_Clear:
            return MTLLoadActionClear;
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        case FrameTarget::IT_SubClear:
            return MTLLoadActionClear;
#else
        case FrameTarget::IT_SubClear:
            return MTLLoadActionLoad;
#endif
        case FrameTarget::IT_Texture:
            return MTLLoadActionLoad;
        }

        return MTLLoadActionLoad;
    }
    //-----------------------------------------------------------------------------------
    MTLStoreAction MetalRenderPassDescriptor::get( StoreType::StoreType action )
    {
        switch( action )
        {
        case FrameTarget::ST_None:
            return MTLStoreActionDontCare;
        case FrameTarget::ST_Store:
            return MTLStoreActionStore;
        case FrameTarget::ST_Multisample:
            return MTLStoreActionMultisampleResolve;
        case FrameTarget::ST_MultisampleStore:
            return MTLStoreActionStoreAndMultisampleResolve;
        case FrameTarget::ST_MultisampleOrStore:
            N_assert( false && "StoreOrResolve is invalid. "
                    "Compositor should've set one or the other already!" );
            return MTLStoreActionStore;
        }

        return MTLStoreActionStore;
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::sanitizeMsaaResolve( size_t colourIdx )
    {
        const size_t i = colourIdx;

        //iOS_GPUFamily3_v2, OSX_GPUFamily1_v2
        if( mColour[i].mStoreType == FrameTarget::ST_MultisampleStore &&
            !mRenderSys->hasStoreAndMultisampleResolve() &&
            (mColour[i].texture->isMultiSample() && mColour[i].mTarget) )
        {
            //Must emulate the behavior (slower)
            mColourAttachment[i].storeAction = MTLStoreActionStore;
            mColourAttachment[i].resolveTexture = nil;
            mResolveColourAttachm[i] = [mColourAttachment[i] copy];
            mResolveColourAttachm[i].loadAction = MTLLoadActionLoad;
            mResolveColourAttachm[i].storeAction = MTLStoreActionMultisampleResolve;

            mRequiresManualResolve = true;
        }
        else if( mColour[i].mStoreType == FrameTarget::ST_None ||
                 mColour[i].mStoreType == FrameTarget::ST_Store )
        {
            mColourAttachment[i].resolveTexture = nil;
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::updateColourRtv( uint8 lastNumColourEntries )
    {
        mRequiresManualResolve = false;
        if( mColourCount < lastNumColourEntries )
        {
            for( size_t i=mColourCount; i<lastNumColourEntries; ++i )
            {
                mColourAttachment[i] = 0;
                mResolveColourAttachm[i] = 0;
            }
        }

        bool hasRenderWindow = false;

        for( size_t i=0; i<mColourCount; ++i )
        {
            if( mColour[i].texture->getResidencyStatus() != StorageType::Resident )
            {
                N_EXCEPT( Exception::ERR_INVALIDPARAMS, "RenderTexture '" +
                             mColour[i].texture->getName() + "' must be resident!",
                             "MetalRenderPassDescriptor::updateColourRtv" );
            }
            if( i > 0 && hasRenderWindow != mColour[i].texture->isWindow() )
            {
                //This is a GL restriction actually, which we mimic for consistency
                N_EXCEPT( Exception::ERR_INVALIDPARAMS,
                             "Cannot use RenderWindow as MRT with other colour textures",
                             "MetalRenderPassDescriptor::updateColourRtv" );
            }

            hasRenderWindow |= mColour[i].texture->isWindow();

            mColourAttachment[i] = 0;

            if( mColour[i].texture->getFormat() == PFG_NULL )
                continue;

            mColourAttachment[i] = [MTLRenderPassColorAttachmentDescriptor alloc];

            if( !mColour[i].texture->isWindow() )
            {
                N_assert( dynamic_cast<MetalTextureGpu*>( mColour[i].texture ) );
                MetalTextureGpu *textureMetal = static_cast<MetalTextureGpu*>( mColour[i].texture );

                if( mColour[i].texture->isMultiSample() )
                {
                    MetalTextureGpu *resolveTexture = 0;
                    if( mColour[i].resolveTexture )
                    {
                        N_assert( dynamic_cast<MetalTextureGpu*>( mColour[i].mTarget ) );
                        resolveTexture = static_cast<MetalTextureGpu*>( mColour[i].mTarget );
                    }

                    if( !mColour[i].texture->isManualMSAA() )
                        mColourAttachment[i].texture = textureMetal->getMsaaFramebufferName();
                    else
                        mColourAttachment[i].texture = textureMetal->getFinalTextureName();

                    if( mColour[i].mTarget )
                        mColourAttachment[i].resolveTexture = mTarget->getFinalTextureName();
                }
                else
                {
                    mColourAttachment[i].texture = textureMetal->getFinalTextureName();
                }
            }

            mColourAttachment[i].clearColor = MTLClearColorMake( mColour[i].clearColour.r,
                                                                 mColour[i].clearColour.g,
                                                                 mColour[i].clearColour.b,
                                                                 mColour[i].clearColour.a );

            mColourAttachment[i].level          = mColour[i].mMip;
            mColourAttachment[i].resolveLevel   = mColour[i].mTargetMip;

            if( mColour[i].texture->getTextureType() == Texture::T_3D )
                mColourAttachment[i].depthPlane = mColour[i].mArrayIndex;
            else
                mColourAttachment[i].slice = mColour[i].mArrayIndex;

            if( mColour[i].mTarget )
            {
                if( mColour[i].mTarget->getTextureType() == Texture::T_3D )
                    mColourAttachment[i].resolveDepthPlane = mColour[i].mTargetArrayIndex;
                else
                    mColourAttachment[i].resolveSlice = mColour[i].mTargetArrayIndex;
            }

            mColourAttachment[i].loadAction = MetalRenderPassDescriptor::get( mColour[i].mInitType );
            mColourAttachment[i].storeAction = MetalRenderPassDescriptor::get( mColour[i].mStoreType );

            if( mColour[i].mStoreType == FrameTarget::ST_MultisampleStore &&
                (!mColour[i].texture->isMultiSample() || !mColour[i].mTarget) )
            {
                //Ogre allows non-MSAA textures to use this flag. Metal may complain.
                mColourAttachment[i].storeAction = MTLStoreActionStore;
            }

            //iOS_GPUFamily3_v2, OSX_GPUFamily1_v2
            if( mColour[i].mStoreType == FrameTarget::ST_MultisampleStore &&
                !mRenderSys->hasStoreAndMultisampleResolve() &&
                (mColour[i].texture->isMultiSample() && mColour[i].mTarget) )
            {
                //Must emulate the behavior (slower)
                mColourAttachment[i].storeAction = MTLStoreActionStore;
                mColourAttachment[i].resolveTexture = nil;
                mResolveColourAttachm[i] = [mColourAttachment[i] copy];
                mResolveColourAttachm[i].loadAction = MTLLoadActionLoad;
                mResolveColourAttachm[i].storeAction = MTLStoreActionMultisampleResolve;

                mRequiresManualResolve = true;
            }

            sanitizeMsaaResolve( i );
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::updateDepthRtv(void)
    {
        mDepthAttachment = 0;

        if( !mDepth.texture )
        {
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
            if( mStencil.texture )
            {
                N_EXCEPT( RenderingAPI,
                             "Stencil without depth (RenderTexture '" +
                             mStencil.texture->getName() + "'). This is not supported by macOS",
                             "MetalRenderPassDescriptor::updateDepthRtv" );
            }
#endif
            return;
        }

        if( mDepth.texture->getResidencyStatus() != StorageType::Resident )
        {
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "RenderTexture '" +
                         mDepth.texture->getName() + "' must be resident!",
                         "MetalRenderPassDescriptor::updateDepthRtv" );
        }

        N_assert( dynamic_cast<MetalTextureGpu*>( mDepth.texture ) );
        MetalTextureGpu *textureMetal = static_cast<MetalTextureGpu*>( mDepth.texture );

        mDepthAttachment = [MTLRenderPassDepthAttachmentDescriptor alloc];
        mDepthAttachment.texture = textureMetal->getFinalTextureName();

        if( !mRenderSys->isReverseDepth() )
            mDepthAttachment.clearDepth = mDepth.clearDepth;
        else
            mDepthAttachment.clearDepth = Real( 1.0f ) - mDepth.clearDepth;

        mDepthAttachment.loadAction = MetalRenderPassDescriptor::get( mDepth.mInitType );
        mDepthAttachment.storeAction = MetalRenderPassDescriptor::get( mDepth.mStoreType );

        if( mDepth.mStoreType == FrameTarget::ST_MultisampleStore &&
            (!mDepth.texture->isMultiSample() || !mDepth.mTarget) )
        {
            //Ogre allows non-MSAA textures to use this flag. Metal may complain.
            mDepthAttachment.storeAction = MTLStoreActionStore;
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::updateStencilRtv(void)
    {
        mStencilAttachment = 0;

        if( !mStencil.texture )
            return;

        if( mStencil.texture->getResidencyStatus() != StorageType::Resident )
        {
            N_EXCEPT( Exception::ERR_INVALIDPARAMS, "RenderTexture '" +
                         mStencil.texture->getName() + "' must be resident!",
                         "MetalRenderPassDescriptor::updateStencilRtv" );
        }

        N_assert( dynamic_cast<MetalTextureGpu*>( mStencil.texture ) );
        MetalTextureGpu *textureMetal = static_cast<MetalTextureGpu*>( mStencil.texture );

        mStencilAttachment = [MTLRenderPassStencilAttachmentDescriptor alloc];
        mStencilAttachment.texture = textureMetal->getFinalTextureName();
        mStencilAttachment.clearStencil = mStencil.clearStencil;

        mStencilAttachment.loadAction = MetalRenderPassDescriptor::get( mStencil.mInitType );
        mStencilAttachment.storeAction = MetalRenderPassDescriptor::get( mStencil.mStoreType );

        if( mStencil.mStoreType == FrameTarget::ST_MultisampleStore &&
            (!mStencil.texture->isMultiSample() || !mStencil.mTarget) )
        {
            //Ogre allows non-MSAA textures to use this flag. Metal may complain.
            mStencilAttachment.storeAction = MTLStoreActionStore;
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::syncType( uint32 entryTypes )
    {
        uint8 lastNumColourEntries = mColourCount;
        FrameBufferObject::syncType( entryTypes );

        checkRenderWindowStatus();

        if( entryTypes & FrameBufferObject::TT_Colour )
            updateColourRtv( lastNumColourEntries );

        if( entryTypes & FrameBufferObject::TT_Depth )
            updateDepthRtv();

        if( entryTypes & FrameBufferObject::TT_Stencil )
            updateStencilRtv();
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::setClearColour( uint8 idx, const Colour &clearColour )
    {
        FrameBufferObject::setClearColour( idx, clearColour );

        if( mColourAttachment[idx] )
        {
            mColourAttachment[idx] = [mColourAttachment[idx] copy];
            mColourAttachment[idx].clearColor = MTLClearColorMake( clearColour.r, clearColour.g,
                                                                   clearColour.b, clearColour.a );
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::setClearDepth( Real clearDepth )
    {
        FrameBufferObject::setClearDepth( clearDepth );

        if( mDepthAttachment )
        {
            mDepthAttachment = [mDepthAttachment copy];
            if( !mRenderSys->isReverseDepth() )
                mDepthAttachment.clearDepth = clearDepth;
            else
                mDepthAttachment.clearDepth = Real( 1.0f ) - clearDepth;
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::setClearStencil( uint32 clearStencil )
    {
        FrameBufferObject::setClearStencil( clearStencil );

        if( mStencilAttachment )
        {
            mStencilAttachment = [mStencilAttachment copy];
            mStencilAttachment.clearStencil = clearStencil;
        }
    }
    //-----------------------------------------------------------------------------------
    uint32 MetalRenderPassDescriptor::checkForClearActions( MetalRenderPassDescriptor *other ) const
    {
        uint32 entriesToFlush = 0;

        N_assert( this->mSharedFboItor == other->mSharedFboItor );
        N_assert( this->mColourCount == other->mColourCount );

        const RenderFeature *capabilities = mRenderSys->getCapabilities();
        const bool isTiler = capabilities->hasCapability( RSC_IS_TILER );

        for( size_t i=0; i<mColourCount; ++i )
        {
            //this->mColour[i].mAllArray doesn't need to be analyzed
            //because it requires a different FBO.
            if( other->mColour[i].mInitType == FrameTarget::IT_Clear ||
                (isTiler && mColour[i].mInitType == FrameTarget::IT_SubClear) )
            {
                entriesToFlush |= FrameBufferObject::TT_Colour0 << i;
            }
        }

        if( other->mDepth.mInitType == FrameTarget::IT_Clear ||
            (isTiler && mDepth.mInitType == FrameTarget::IT_SubClear) )
        {
            entriesToFlush |= FrameBufferObject::TT_Depth;
        }

        if( other->mStencil.mInitType == FrameTarget::IT_Clear ||
            (isTiler && mStencil.mInitType == FrameTarget::IT_SubClear) )
        {
            entriesToFlush |= FrameBufferObject::TT_Stencil;
        }

        return entriesToFlush;
    }
    //-----------------------------------------------------------------------------------
    uint32 MetalRenderPassDescriptor::willSwitchTo( MetalRenderPassDescriptor *newDesc,
                                                    bool warnIfRtvWasFlushed ) const
    {
        uint32 entriesToFlush = 0;

        if( !newDesc ||
            this->mSharedFboItor != newDesc->mSharedFboItor ||
            this->mStepping || newDesc->mStepping )
        {
            entriesToFlush = FrameBufferObject::TT_All;
        }
        else
            entriesToFlush |= checkForClearActions( newDesc );

        if( warnIfRtvWasFlushed )
            newDesc->checkWarnIfRtvWasFlushed( entriesToFlush );

        return entriesToFlush;
    }
    //-----------------------------------------------------------------------------------
    bool MetalRenderPassDescriptor::cannotInterruptRendering(void) const
    {
        bool cannotInterrupt = false;

        for( size_t i=0; i<mColourCount && !cannotInterrupt; ++i )
        {
            if( mColour[i].mStoreType != FrameTarget::ST_Store &&
                mColour[i].mStoreType != FrameTarget::ST_MultisampleStore )
            {
                cannotInterrupt = true;
            }
        }

        cannotInterrupt |= (mDepth.texture &&
                            mDepth.mStoreType != FrameTarget::ST_Store &&
                            mDepth.mStoreType != FrameTarget::ST_MultisampleStore) ||
                           (mStencil.texture &&
                            mStencil.mStoreType != FrameTarget::ST_Store &&
                            mStencil.mStoreType != FrameTarget::ST_MultisampleStore);

        return cannotInterrupt;
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::performLoadActions( MTLRenderPassDescriptor *passDesc,
                                                        bool renderingWasInterrupted )
    {
        if( mStepping )
            return;

        for( size_t i=0; i<mColourCount; ++i )
            passDesc.colorAttachments[i] = mColourAttachment[i];

        if( mColourCount > 0 && mColour[0].texture->isWindow() )
        {
            //The RenderWindow's MetalDrawable changes every frame. We need a
            //hard copy since the previous descriptor may still be in flight.
            //Also ensure we do not retain current drawable's texture beyond the frame.
            passDesc.colorAttachments[0] = [mColourAttachment[0] copy];
            MetalTextureGpuWindow *textureMetal = static_cast<MetalTextureGpuWindow*>( mColour[0].texture );
            textureMetal->nextDrawable();
            if( textureMetal->isMultiSample() )
            {
                passDesc.colorAttachments[0].texture = textureMetal->getMsaaFramebufferName();
                passDesc.colorAttachments[0].resolveTexture = textureMetal->getFinalTextureName();
                if( mColour[0].mStoreType == FrameTarget::ST_None ||
                    mColour[0].mStoreType == FrameTarget::ST_Store ||
                    (mColour[0].mStoreType == FrameTarget::ST_MultisampleStore &&
                     !mRenderSys->hasStoreAndMultisampleResolve()) )
                {
                    passDesc.colorAttachments[0].resolveTexture = nil;
                }
            }
            else
            {
                passDesc.colorAttachments[0].texture = textureMetal->getFinalTextureName();
            }
        }

        passDesc.depthAttachment = mDepthAttachment;
        passDesc.stencilAttachment = mStencilAttachment;

        if( renderingWasInterrupted )
        {
            for( size_t i=0; i<mColourCount; ++i )
            {
                passDesc.colorAttachments[i] = [passDesc.colorAttachments[i] copy];
                passDesc.colorAttachments[i].loadAction = MTLLoadActionLoad;
            }

            if( passDesc.depthAttachment )
            {
                passDesc.depthAttachment = [passDesc.depthAttachment copy];
                passDesc.depthAttachment.loadAction = MTLLoadActionLoad;
            }

            if( passDesc.stencilAttachment )
            {
                passDesc.stencilAttachment = [passDesc.stencilAttachment copy];
                passDesc.stencilAttachment.loadAction = MTLLoadActionLoad;
            }

            const bool cannotInterrupt = cannotInterruptRendering();

            static bool warnedOnce = false;
            if( !warnedOnce || cannotInterrupt )
            {
                LogManager::getSingleton().logMessage(
                            "WARNING: Rendering was interrupted. Likely because a StagingBuffer "
                            "was used while inside HlmsPbs::fillBuffersFor; but could be caused "
                            "by other reasons such as mipmaps being generated in a listener, "
                            "buffer transfer/copies, manually dispatching a compute shader, etc."
                            " Performance will be degraded. This message will only appear once.",
                            LML_CRITICAL );

#if OGRE_DEBUG_MODE
                LogManager::getSingleton().logMessage(
                            "Dumping callstack at the time rendering was interrupted: ", LML_CRITICAL );

                char **translatedCS = backtrace_symbols( mCallstackBacktrace, mNumCallstackEntries );

                for( size_t i=0; i<mNumCallstackEntries; ++i )
                    LogManager::getSingleton().logMessage( translatedCS[i], LML_CRITICAL );

                memset( mCallstackBacktrace, 0, sizeof(mCallstackBacktrace) );
                mNumCallstackEntries = 0;
#endif
                warnedOnce = true;
            }

            if( cannotInterrupt )
            {
                N_EXCEPT( Exception::ERR_INVALID_STATE,
                             "ERROR: We cannot resume rendering because the compositor pass specified "
                             "'Keep' or 'MultisampleResolve' as StoreType for colour, depth "
                             "or stencil, therefore rendering will be incorrect!!!\n"
                             "Either remove the buffer manipulation from the rendering hot loop "
                             "that is causing the interruption, or set the store actions to Store, or"
                             "StoreAndMultisampleResolve");
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void MetalRenderPassDescriptor::performStoreActions( uint32 entriesToFlush,
                                                         bool isInterruptingRendering )
    {
        if( mStepping )
            return;

        if( isInterruptingRendering )
        {
#if OGRE_DEBUG_MODE
            //Save the backtrace to report it later
            const bool cannotInterrupt = cannotInterruptRendering();
            static bool warnedOnce = false;
            if( !warnedOnce || cannotInterrupt )
            {
                mNumCallstackEntries = backtrace( mCallstackBacktrace, 32 );
                warnedOnce = true;
            }
#endif
            return;
        }

        //End (if exists) the render command encoder tied to this RenderPassDesc.
        //Another encoder will have to be created, and don't let ours linger
        //since mActiveFrame2 probably doesn't even point to 'this'
        mDevice->endAllEncoders( false );

        if( !(entriesToFlush & Colour) )
            return;

        if( !mRequiresManualResolve )
            return;

        if( mRenderSys->hasStoreAndMultisampleResolve() )
            return;

        MTLRenderPassDescriptor *passDesc = [MTLRenderPassDescriptor renderPassDescriptor];
        size_t currentColourIdx = 0;

        for( size_t i=0; i<mColourCount; ++i )
        {
            if( mColour[i].mTarget &&
                mColour[i].mStoreType == FrameTarget::ST_MultisampleStore )
            {
                passDesc.colorAttachments[currentColourIdx] = mResolveColourAttachm[i];
                ++currentColourIdx;
            }
        }

        if( mColourCount > 0 && mColour[0].texture->isWindow() )
        {
            //RenderWindows are a special case. Do not retain.
            passDesc.colorAttachments[0] = [passDesc copy];
            MetalTextureGpuWindow *textureMetal =
                    static_cast<MetalTextureGpuWindow*>( mColour[0].texture );
            passDesc.colorAttachments[0].texture = textureMetal->getMsaaFramebufferName();
            passDesc.colorAttachments[0].resolveTexture = textureMetal->getFinalTextureName();
        }

        mDevice->mRenderEncoder =
                [mDevice->mCurrentCommandBuffer renderCommandEncoderWithDescriptor:passDesc];
        mDevice->endRenderEncoder( false );
    }
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    MetalFrameBufferDescValue::MetalFrameBufferDescValue() : refCount( 0 ) {}
}
