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

#ifndef _NIIFrameBufferObject_H_
#define _NIIFrameBufferObject_H_

#include "NiiPreInclude.h"
#include "NiiColour.h"

namespace NII
{
    struct _EngineAPI FrameTarget
    {
        enum InitType
        {
            IT_None,
            IT_Clear,
            IT_SubClear,
            IT_Texture
        };
        
        enum StoreType
        {
            ST_None,
            ST_Store,
            ST_Multisample,
            ST_MultisampleStore,
            ST_MultisampleOrStore
        };
        
        FrameTarget();

        bool operator != (const FrameTarget & o) const;
        bool operator == (const FrameTarget & o) const;
        bool operator < (const FrameTarget & o) const;
        
        TextureGpu * mTexture;
        TextureGpu * mTarget;

        Nui16 mMip;
        Nui16 mTargetMip;

        Nui16 mArrayIndex;
        Nui16 mTargetArrayIndex;

        InitType mInitType;
        StoreType mStoreType;
    };

    class _EngineAPI FrameBufferObject : public RenderSysAlloc
    {
    public:
        enum TargetType
        {
            TT_Colour0 = 0x01,
            TT_Colour1 = 0x02,
            TT_Colour2 = 0x04,
            TT_Colour3 = 0x08,
            TT_Colour4 = 0x10,
            TT_Colour5 = 0x20,
            TT_Colour6 = 0x40,
            TT_Colour7 = 0x80,
            TT_Colour  = TT_Colour0|TT_Colour1|TT_Colour2|TT_Colour3|TT_Colour4|TT_Colour5|TT_Colour6|TT_Colour7,
            TT_Depth   = 1u << NII_DepthTargetIndex,
            TT_Stencil = 1u << NII_StencilTargetIndex,
            TT_All = TT_Colour|Depth|TT_Stencil
        };
    public:
        FrameBufferObject();
        virtual ~FrameBufferObject();

        virtual bool isTargetEqual(const FrameBufferObject * o) const;

        bool isTarget(const Texture * tex) const;
        
        virtual void syncType(Nmark type);

        virtual void setClearColour(const Colour & clr);
        virtual void setClearColour(Nidx idx, const Colour & clr);
        virtual void setClearDepth(NIIf depth);
        virtual void setClearStencil(Nui32 stencil);

        Nui32 getColourCount() const                { return mColourCount; }

        bool isTextureFlipping() const              { return mTextureFlipping; }
        
        bool isStencilFeature() const;

        virtual void getPlatformData(IdString name, void * pData, uint32 extraParam) {}
    public:
        FrameTarget mColour[NII_MAX_RenderTargetDS];
        Colour mClearColour[NII_MAX_RenderTarget];
        NIIf mClearDepth;
        Nui32 mClearStencil;
    protected:
        Nui32 mColourCount;
        bool mTextureFlipping;
    public: 
        bool mStencilReadOnly;
        bool mDeptReadOnly;
        bool mAllArray[NII_MAX_RenderTarget];
        bool mInformationOnly;
    };
}
#endif