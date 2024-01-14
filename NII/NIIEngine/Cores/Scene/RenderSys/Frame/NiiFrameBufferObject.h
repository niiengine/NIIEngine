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
#include "NiiCommon.h"

namespace NII
{
    /**
    @version NIIEngine 6.0.0
    */
    struct _EngineAPI FrameTarget
    {
        /**
        @version NIIEngine 6.0.0
        */
        enum InitType
        {
            IT_None,
            IT_Clear,
            IT_SubClear,
            IT_Texture
        };
        
        /**
        @version NIIEngine 6.0.0
        */
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
        
        Texture * mTexture;
        Texture * mTarget;
        InitType mInitType;
        StoreType mStoreType;

        Nui16 mMip;
        Nui16 mTargetMip;

        Nui16 mArrayIndex;
        Nui16 mTargetArrayIndex;
    };

    /**
    @version NIIEngine 6.0.0
    */
    class _EngineAPI FrameBufferObject : public RenderSysData, public RenderSysAlloc
    {
    public:
        /**
        @version NIIEngine 6.0.0
        */
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
            TT_All = TT_Colour | TT_Depth | TT_Stencil
        };
    public:
        FrameBufferObject();
        virtual ~FrameBufferObject();

        /**
        @version NIIEngine 6.0.0
        */
        virtual bool isTargetEqual(const FrameBufferObject * o) const;

        /**
        @version NIIEngine 6.0.0
        */
        bool isTarget(const Texture * tex) const;
        
        /**
        @version NIIEngine 6.0.0
        */
        virtual void syncType(Nmark type);
        
        /**
        @version NIIEngine 6.0.0
        */
        virtual void setClearColour(const Colour & clr);
        
        /**
        @version NIIEngine 6.0.0
        */
        virtual void setClearColour(Nidx idx, const Colour & clr);
        
        /**
        @version NIIEngine 6.0.0
        */
        virtual void setClearDepth(NIIf depth);
        
        /**
        @version NIIEngine 6.0.0
        */
        virtual void setClearStencil(Nui32 stencil);

        /**
        @version NIIEngine 6.0.0
        */
        inline Nui32 getColourCount() const            { return mColourCount; }

        /**
        @version NIIEngine 6.0.0
        */
        inline bool isFlipping() const                 { return mTextureFlipping; }
        
        /**
        @version NIIEngine 6.0.0
        */
        bool isStencilFeature() const;
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
        bool mStepping;
    };
}
#endif