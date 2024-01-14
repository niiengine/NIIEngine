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
#include "NiiShaderCh.h"
#include "NiiEngine.h"
#include "NiiException.h"
#include "NiiMaterialManager.h"
#include "NiiStrConv.h"

namespace NII
{
    /*enum FeatureType
    {
        SB_StencilCheckDisable      = 1u << 0u,
        SB_BothSideStencil          = 1u << 1u,
        SB_DepthCheckDisable        = 1u << 2u,
        SB_DepthWriteDisable        = 1u << 3u,
        SB_InvertVertexWinding      = 1u << 4u
    };*/
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ShaderChBase
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ShaderChBase::ShaderChBase(ShaderChType type, Nmark bindmark) :
        mType(type),
        mBindMark(bindmark)
    {
    }
    //-----------------------------------------------------------------------
    void ShaderChBase::read(GpuProgramParam *, NCount) const
    {
    }
    //-----------------------------------------------------------------------
    void ShaderChBase::write(const GpuProgramParam *, NCount)
    {
        
    }
    //-----------------------------------------------------------------------
    void ShaderChBase::bind()
    {
        
    }
    //-----------------------------------------------------------------------
    void ShaderChBase::sync()
    {
        
    }
    //-----------------------------------------------------------------------
    ShaderChBase * ShaderChBase::clone() const
    {
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ShaderChColour
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ShaderChColour::ShaderChColour(ShaderChType type) :
        ShaderChBase(type),
        mAmbient(Colour::White),
        mDiffuse(Colour::White),
        mSpecular(Colour::Black),
        mEmissive(Colour::Black),
        mShininess(0.0f),
        mRejectMode(CPM_ALWAYS_PASS),
        mRefValue(0.0f),
        mMark(CTM_T_NONE)
    {
    }
    //-----------------------------------------------------------------------
    ShaderChColour::ShaderChColour(const ShaderChColour & o)
    {
        mAmbient = o.mAmbient;
        mDiffuse = o.mDiffuse;
        mSpecular = o.mSpecular;
        mEmissive = o.mEmissive;
        mShininess = o.mShininess;
        mRejectMode = o.mRejectMode;
        mRefValue = o.mRefValue;
        mMark = o.mMark;
    }
    //------------------------------------------------------------------------
    ShaderChColour::ShaderChColour(CmpMode func, Nui8 value, bool alphaCoverage) :
        mRejectMode(func)
    {
        if(alphaCoverage)
        {
            mMark = value | ShaderChAlpha_AlphaCoverage;
        }
        else
        {
            mMark = 0;
        }
    }
    //-----------------------------------------------------------------------
    ShaderChColour::ShaderChColour(const Colour & ambient, const Colour & diffuse,
        const Colour & specular, NIIf shinin) :
            mAmbient(ambient),
            mDiffuse(diffuse),
            mSpecular(specular),
            mShininess(shinin)
    {
        mMark = CTM_M_AMBIENT | CTM_M_DIFFUSE | CTM_M_SPECULAR;
    }
    //-----------------------------------------------------------------------
    ShaderChColour::ShaderChColour(const Colour & ambient, const Colour & diffuse,
        const Colour & specular, const Colour & emissive, NIIf shinin, Nmark mark) :
            mAmbient(ambient),
            mDiffuse(diffuse),
            mSpecular(specular),
            mEmissive(emissive),
            mShininess(shinin),
            mMark(mark)
    {
    }
    //-----------------------------------------------------------------------
    bool ShaderChColour::operator ==(const ShaderChColour & o) const
    {
        if(mAmbient == o.mAmbient &&
            mDiffuse == o.mDiffuse &&
            mSpecular == o.mSpecular &&
            mEmissive == o.mEmissive &&
            mShininess == o.mShininess &&
            mRejectMode == o.mRejectMode &&
            mRefValue == o.mRefValue &&
            mMark == o.mMark)
            return true;
        return false;
    }
    //-----------------------------------------------------------------------
    bool ShaderChColour::operator <(const ShaderChColour & o) const
    {
        if (mAmbient + mDiffuse + mSpecular + mEmissive + mShininess + mRefValue + mRejectMode + mMark < 
            o.mAmbient + o.mDiffuse + o.mSpecular + o.mEmissive + o.mShininess + o.mRefValue + o.mRejectMode + o.mMark)
            return true;
        return false;
    }
    //-----------------------------------------------------------------------
    bool ShaderChColour::isReflexColour() const
    {
        if(mDiffuse == Colour::Black && mSpecular == Colour::Black)
            return false;
        return true;
    }
    //-----------------------------------------------------------------------
    void ShaderChColour::setColourTracking(Nmark colour)
    {
        mMark &= ~CTM_T_NONE;
        mMark |= colour;
    }
    //-----------------------------------------------------------------------
    void ShaderChColour::read(GpuProgramParam * param, NCount memoft) const
    {
        param->_write(memoft, mAmbient.ptr(), 4);
        param->_write(memoft + 4, mDiffuse.ptr(), 4);
        param->_write(memoft + 8, mSpecular.ptr(), 4);
        param->_write(memoft + 12, mEmissive.ptr(), 4);
        param->_write(memoft + 16, &mShininess);
        param->_write(memoft + 17, &mRefValue);
        param->_write(memoft + 18, &mRejectMode);
        param->_write(memoft + 19, &mMark);
    }
    //-----------------------------------------------------------------------
    void ShaderChColour::write(const GpuProgramParam * param, NCount memoft)
    {
        param->_read(memoft, mAmbient.ptr(), 4);
        param->_read(memoft + 4, mDiffuse.ptr(), 4);
        param->_read(memoft + 8, mSpecular.ptr(), 4);
        param->_read(memoft + 12, mEmissive.ptr(), 4);
        param->_read(memoft + 16, &mShininess);
        param->_read(memoft + 17, &mRefValue);
        param->_read(memoft + 18, &mRejectMode);
        param->_read(memoft + 19, &mMark);
    }
    //------------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ShaderChBlend
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    static const Nmark __ShaderChBlendMark[FBM_Count + 1][3] =
    {
        { CF_ONE, CF_ONE, CBM_ADD },                    //FBM_ADD
        { CF_ONE, CF_ZERO, CBM_MIN },                   //FBM_MIN
        { CF_ONE, CF_ZERO, CBM_MAX },                   //FBM_MAX
        { CF_ONE, CF_ONE, CBM_SUBTRACT},                //FBM_SUBTRACT
        { CF_DST, CF_ZERO, CBM_ADD },                   //FBM_MODULATE
        { CF_ONE, CF_ZERO, CBM_ADD },                   //FBM_REPLACE
        { CF_SRC_ALPHA, CF_INV_SRC_ALPHA, CBM_ADD },    //FBM_INV_ALPHA
        { CF_SRC, CF_INV_SRC, CBM_ADD },                //FBM_INV_COLOUR
        { CF_ONE, CF_ZERO, CBM_ADD }                    //FBM_Count
    };
    //-----------------------------------------------------------------------
    ShaderChBlend::ShaderChBlend() :
        ShaderChBase(SCT_Blend),
        mSrcFactor(CF_ONE),
        mDstFactor(CF_ZERO),
        mSrcAlphaFactor(CF_ONE),
        mDstAlphaFactor(CF_ZERO),
        mAlphaMode(CBM_ADD),
        mMode(CBM_ADD),
        mMark(CM_RGBA)
    {
    }
    //-----------------------------------------------------------------------
    ShaderChBlend::ShaderChBlend(const ShaderChBlend & o)
    {
        mSrcFactor = o.mSrcFactor;
        mDstFactor = o.mDstFactor;
        mSrcAlphaFactor = o.mSrcAlphaFactor;
        mDstAlphaFactor = o.mDstAlphaFactor;
        mMode = o.mMode;
        mAlphaMode = o.mAlphaMode;
        mMark = o.mMark;
    }
    //-----------------------------------------------------------------------
    ShaderChBlend::ShaderChBlend(ColourFactor src, ColourFactor dst, ColourBlendMode mode)
    {
        setBlend(src, dst);
        setBlendMode(mode);
    }
    //-----------------------------------------------------------------------
    ShaderChBlend::ShaderChBlend(ColourFactor src, ColourFactor dst, ColourFactor asrc,
        ColourFactor adst, ColourBlendMode mode)
    {
        setBlend(src, dst, asrc, adst);
        setBlendMode(mode);
    }
    //-----------------------------------------------------------------------
    ShaderChBlend::ShaderChBlend(ColourFactor src, ColourFactor dst, ColourFactor asrc,
        ColourFactor adst, ColourBlendMode mode, ColourBlendMode amode)
    {
        setBlend(src, dst, asrc, adst);
        setBlendMode(mode, amode);
    }
    //-----------------------------------------------------------------------
    bool ShaderChBlend::operator ==(const ShaderChBlend & o) const
    {
        // Default blending (overwrite)
        if(mSrcFactor == o.mSrcFactor &&
            mDstFactor == o.mDstFactor &&
            mSrcAlphaFactor == o.mSrcAlphaFactor &&
            mDstAlphaFactor == o.mDstAlphaFactor &&
            mMode == o.mMode &&
            mAlphaMode == o.mAlphaMode &&
            mMark == o.mMark)
            return true;
        return false;
    }
    //-----------------------------------------------------------------------
    bool ShaderChBlend::operator <(const ShaderChBlend & o) const
    {
        if (mSrcFactor + mSrcAlphaFactor + mMode + mAlphaMode + mMark <
            o.mSrcFactor + o.mSrcAlphaFactor + o.mMode + o.mAlphaMode + o.mMark ||
            mDstFactor + mDstAlphaFactor + mMode + mAlphaMode + mMark < 
            o.mDstFactor + o.mDstAlphaFactor + o.mMode + o.mAlphaMode + o.mMark)
            return true;
        return false;
    }
    //-----------------------------------------------------------------------
    void ShaderChBlend::setBlend(FrameBlendMode fbm)
    {
        ColourFactor src, dst;
        if(fbm < FBM_Count)
        {
            src = (ColourFactor)__ShaderChBlendMark[fbm][0];
            dst = (ColourFactor)__ShaderChBlendMark[fbm][1];
            mMode = (ColourBlendMode)__ShaderChBlendMark[fbm][2];
        }
        else
        {
            src = (ColourFactor)__ShaderChBlendMark[FBM_Count][0];
            dst = (ColourFactor)__ShaderChBlendMark[FBM_Count][1];
            mMode = (ColourBlendMode)__ShaderChBlendMark[FBM_Count][2];
        }

        setBlend(src, dst);
    }
    //-----------------------------------------------------------------------
    void ShaderChBlend::setBlend(FrameBlendMode fbm, FrameBlendMode afbm)
    {
        // Convert types into blend factors

        ColourFactor src;
        ColourFactor dst;
        ColourFactor srca;
        ColourFactor dsta;
        if(fbm < FBM_Count)
        {
            src = (ColourFactor)__ShaderChBlendMark[fbm][0];
            dst = (ColourFactor)__ShaderChBlendMark[fbm][1];
            mMode = (ColourBlendMode)__ShaderChBlendMark[fbm][2];
        }
        else
        {
            src = (ColourFactor)__ShaderChBlendMark[FBM_Count][0];
            dst = (ColourFactor)__ShaderChBlendMark[FBM_Count][1];
            mMode = (ColourBlendMode)__ShaderChBlendMark[FBM_Count][2];
        }
        if(afbm < FBM_Count)
        {
            srca = (ColourFactor)__ShaderChBlendMark[afbm][0];
            dsta = (ColourFactor)__ShaderChBlendMark[afbm][1];
            mAlphaMode = (ColourBlendMode)__ShaderChBlendMark[afbm][2];
        }
        else
        {
            srca = (ColourFactor)__ShaderChBlendMark[FBM_Count][0];
            dsta = (ColourFactor)__ShaderChBlendMark[FBM_Count][1];
            mAlphaMode = (ColourBlendMode)__ShaderChBlendMark[FBM_Count][2];
        }

        setBlend(src, dst, srca, dsta);
    }
    //-----------------------------------------------------------------------
    void ShaderChBlend::setBlend(ColourFactor src, ColourFactor dst)
    {
        mSrcFactor = src;
        mDstFactor = dst;
        if(mSrcFactor == CF_ONE)
            mMark |= ShaderChBlendSrcOne;
        else
            mMark &= ~ShaderChBlendSrcOne;

        if(mDstFactor == CF_ZERO)
            mMark |= ShaderChBlendDstZERO;
        else
            mMark &= ~ShaderChBlendDstZERO;

        mMark &= ~ShaderChBlendSeparate;
    }
    //-----------------------------------------------------------------------
    void ShaderChBlend::setBlend(ColourFactor src, ColourFactor dst,
        ColourFactor asrc, ColourFactor adst)
    {
        mSrcFactor = src;
        mDstFactor = dst;
        if(mSrcFactor == CF_ONE)
            mMark |= ShaderChBlendSrcOne;
        else
            mMark &= ~ShaderChBlendSrcOne;
        if(mDstFactor == CF_ZERO)
            mMark |= ShaderChBlendDstZERO;
        else
            mMark &= ~ShaderChBlendDstZERO;

        mSrcAlphaFactor = asrc;
        mDstAlphaFactor = adst;
        if(mSrcAlphaFactor == CF_ONE)
            mMark |= ShaderChBlendSrcAlphaOne;
        else
            mMark &= ~ShaderChBlendSrcAlphaOne;
        if(mDstAlphaFactor == CF_ZERO)
            mMark |= ShaderChBlendDstAlphaZERO;
        else
            mMark &= ~ShaderChBlendDstAlphaZERO;

        mMark |= ShaderChBlendSeparate;
    }
    //-----------------------------------------------------------------------
    void ShaderChBlend::setBlendMode(ColourBlendMode cbm)
    {
        mMode = cbm;
        mMark &= ~ShaderChBlendSeparateMode;
    }
    //-----------------------------------------------------------------------
    void ShaderChBlend::setBlendMode(ColourBlendMode cbm, ColourBlendMode acbm)
    {
        mMode = cbm;
        mAlphaMode = acbm;
        mMark |= ShaderChBlendSeparateMode;
    }
    //-----------------------------------------------------------------------
    void ShaderChBlend::read(GpuProgramParam * param, NCount memoft) const
    {
        param->_write(memoft, &mSrcFactor);
        param->_write(memoft+1, &mDstFactor);
        param->_write(memoft+2, &mSrcAlphaFactor);
        param->_write(memoft+3, &mDstAlphaFactor);
        param->_write(memoft+4, &mMode);
        param->_write(memoft+5, &mAlphaMode);
        param->_write(memoft+6, &mMark);
    }
    //-----------------------------------------------------------------------
    void ShaderChBlend::write(const GpuProgramParam * param, NCount memoft)
    {
        param->_read(memoft, &mSrcFactor);
        param->_read(memoft+1, &mDstFactor);
        param->_read(memoft+2, &mSrcAlphaFactor);
        param->_read(memoft+3, &mDstAlphaFactor);
        param->_read(memoft+4, &mMode);
        param->_read(memoft+5, &mAlphaMode);
        param->_read(memoft+6, &mMark);
    }
    //-----------------------------------------------------------------------
    bool ShaderChBlend::isTransparent() const
    {
        // Transparent if any of the destination colour is taken into account
        if(mDstFactor == CF_ZERO &&
            mSrcFactor != CF_DST && mSrcFactor != CF_INV_DST &&
            mSrcFactor != CF_DST_ALPHA && mSrcFactor != CF_INV_DST_ALPHA)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    //------------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ShaderChStencil
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ShaderChStencil::ShaderChStencil(ShaderChType type):
        ShaderChBase(type),
        mFrontTestMark(0xFFFFFFFF), 
        mFrontCmpMark(0xFFFFFFFF),
        mFrontFunc(CPM_ALWAYS_PASS),
        mFrontCmpValue(0), 
        mFrontStencilFailOp(SOT_KEEP),
        mFrontDepthFailOp(SOT_KEEP), 
        mFrontTestPassOp(SOT_KEEP),
        mBackTestMark(0xFFFFFFFF), 
        mBackCmpMark(0xFFFFFFFF),
        mBackFunc(CPM_ALWAYS_PASS),
        mBackCmpValue(0), 
        mBackStencilFailOp(SOT_KEEP),
        mBackDepthFailOp(SOT_KEEP), 
        mBackTestPassOp(SOT_KEEP),
        mDepthFunc(CPM_LESS_EQUAL),
        mPolygonMode(PM_SOLID),
        mCullMode(CM_Clockwise),
        mBias(0.0f),
        mBiasFactor(0.0f),
        mSlopeScaleBias(0.0f)
    {
        mMark = ShaderCh_DepthCheck | ShaderCh_DepthWrite;
    }
    //-----------------------------------------------------------------------
    ShaderChStencil::ShaderChStencil(const ShaderChStencil & o):
        mFrontTestMark(o.mFrontTestMark),
        mFrontCmpMark(o.mFrontCmpMark),
        mFrontFunc(o.mFrontFunc),
        mFrontCmpValue(o.mFrontCmpValue),
        mFrontStencilFailOp(o.mFrontStencilFailOp),
        mFrontDepthFailOp(o.mFrontDepthFailOp),
        mFrontTestPassOp(o.mFrontTestPassOp),
        mBackTestMark(o.mBackTestMark),
        mBackCmpMark(o.mBackCmpMark),
        mBackFunc(o.mBackFunc),
        mBackCmpValue(o.mBackCmpValue),
        mBackStencilFailOp(o.mBackStencilFailOp),
        mBackDepthFailOp(o.mBackDepthFailOp),
        mBackTestPassOp(o.mBackTestPassOp),
        mDepthFunc(o.mDepthFunc),
        mBias(o.mBias),
        mSlopeScaleBias(o.mSlopeScaleBias),
        mBiasFactor(o.mBiasFactor),
        mPolygonMode(o.mPolygonMode),
        mCullMode(o.mCullMode),
        mMark(o.mMark)
    {

    }
    //-----------------------------------------------------------------------
    ShaderChStencil::ShaderChStencil(bool check, CmpMode mode, bool write, NIIf bias, NIIf scaleslopebias) :
        mDepthFunc(mode),
        mBias(bias),
        mSlopeScaleBias(scaleslopebias)
    {
        mMark = 0;
        if(check)
            mMark |= ShaderCh_DepthCheck;

        if(write)
            mMark |= ShaderCh_DepthWrite;
    }
    //-----------------------------------------------------------------------
    ShaderChStencil::ShaderChStencil(bool twoside, Nui32 testmask, Nui32 cmpmask,
        CmpMode func, Nui32 value, StencilOpType stencilfail, StencilOpType depthfail,
        StencilOpType testpass):
        mFrontTestMark(testmask),
        mFrontCmpMark(cmpmask),
        mFrontFunc(func),
        mFrontCmpValue(value),
        mFrontStencilFailOp(stencilfail),
        mFrontDepthFailOp(depthfail),
        mFrontTestPassOp(testpass),
        mBackTestMark(testmask),
        mBackCmpMark(cmpmask),
        mBackFunc(func),
        mBackCmpValue(value),
        mBackStencilFailOp(stencilfail),
        mBackDepthFailOp(depthfail),
        mBackTestPassOp(testpass)
    {
        mMark = ShaderCh_StencilCheck;
        if(twoside)
            mMark |= ShaderCh_StencilBothSide;
    }
    //-----------------------------------------------------------------------
    bool ShaderChStencil::operator ==(const ShaderChStencil & o) const
    {
        if(mMark && o.mMark &&
            mFrontTestMark == o.mFrontTestMark &&
            mFrontCmpMark == o.mFrontCmpMark &&
            mFrontFunc == o.mFrontFunc &&
            mFrontCmpValue == o.mFrontCmpValue &&
            mFrontStencilFailOp == o.mFrontStencilFailOp &&
            mFrontDepthFailOp == o.mFrontDepthFailOp &&
            mFrontTestPassOp == o.mFrontTestPassOp &&
            mBackTestMark == o.mBackTestMark &&
            mBackCmpMark == o.mBackCmpMark &&
            mBackFunc == o.mBackFunc &&
            mBackCmpValue == o.mBackCmpValue &&
            mBackStencilFailOp == o.mBackStencilFailOp &&
            mBackDepthFailOp == o.mBackDepthFailOp &&
            mBackTestPassOp == o.mBackTestPassOp && 
            mDepthFunc == o.mDepthFunc &&
            mBias == o.mBias &&
            mSlopeScaleBias == o.mSlopeScaleBias &&
            mBiasFactor == o.mBiasFactor)
            return true;
        else if (mMark == o.mMark &&
            mFrontTestMark == o.mFrontTestMark &&
            mFrontCmpMark == o.mFrontCmpMark &&
            mFrontFunc == o.mFrontFunc &&
            mFrontCmpValue == o.mFrontCmpValue &&
            mFrontStencilFailOp == o.mFrontStencilFailOp &&
            mFrontDepthFailOp == o.mFrontDepthFailOp &&
            mFrontTestPassOp == o.mFrontTestPassOp && 
            mDepthFunc == o.mDepthFunc &&
            mBias == o.mBias &&
            mSlopeScaleBias == o.mSlopeScaleBias &&
            mBiasFactor == o.mBiasFactor)
            return true;

        return false;
    }
    //-----------------------------------------------------------------------
    bool ShaderChStencil::operator <(const ShaderChStencil & o) const
    {
        if (mMark < o.mMark ||
            mFrontTestMark + mFrontCmpMark + mFrontFunc + mFrontCmpValue <
                o.mFrontTestMark + o.mFrontCmpMark + o.mFrontFunc + o.mFrontCmpValue ||
            mFrontTestPassOp + mFrontStencilFailOp + mFrontDepthFailOp < 
                o.mFrontTestPassOp + o.mFrontStencilFailOp + o.mFrontDepthFailOp ||
            mBackTestMark + mBackCmpMark + mBackFunc + mBackCmpValue < 
                o.mBackTestMark + o.mBackCmpMark + o.mBackFunc + o.mBackCmpValue ||
            mBackStencilFailOp + mBackDepthFailOp + mBackTestPassOp < 
                o.mBackStencilFailOp + o.mBackDepthFailOp + o.mBackTestPassOp ||
            mDepthFunc + mBias + mSlopeScaleBias + mBiasFactor < 
                o.mDepthFunc + o.mBias + o.mSlopeScaleBias + o.mBiasFactor)
            return true;
        else if (mMark == o.mMark &&(
            mFrontTestMark + mFrontCmpMark + mFrontFunc < 
                o.mFrontTestMark + o.mFrontCmpMark + o.mFrontFunc ||
            mFrontCmpValue + mFrontStencilFailOp + mFrontDepthFailOp + mFrontTestPassOp < 
                o.mFrontCmpValue + o.mFrontStencilFailOp + o.mFrontDepthFailOp + o.mFrontTestPassOp ||
            mDepthFunc + mBias + mSlopeScaleBias + mBiasFactor < 
                o.mDepthFunc + o.mBias + o.mSlopeScaleBias + o.mBiasFactor))
            return true;

        return false;
    }
    //-----------------------------------------------------------------------
    void ShaderChStencil::flipStencil()
    {
        CmpMode tempCM = mFrontFunc;
        mFrontFunc = mBackFunc;
        mBackFunc = tempCM;
        
        StencilOpType tempSOT = mFrontStencilFailOp;
        mFrontStencilFailOp = mBackStencilFailOp;
        mBackStencilFailOp = tempSOT;

        tempSOT = mFrontTestPassOp;
        mFrontTestPassOp = mBackTestPassOp;
        mBackTestPassOp = tempSOT;
        
        tempSOT = mFrontDepthFailOp;
        mFrontDepthFailOp = mBackDepthFailOp;
        mBackDepthFailOp = mFrontDepthFailOp;
    }
    //-----------------------------------------------------------------------
    void ShaderChStencil::read(GpuProgramParam * param, NCount memoft) const
    {
        param->_write(memoft, &mFrontTestMark);
        param->_write(memoft + 2, &mBackTestMark);
        param->_write(memoft + 3, &mFrontCmpMark);
        param->_write(memoft + 4, &mBackCmpMark);
        param->_write(memoft + 5, &mFrontCmpValue);
        param->_write(memoft + 6, &mBackCmpValue);
        param->_write(memoft + 7, &mFrontFunc);
        param->_write(memoft + 8, &mBackFunc);
        param->_write(memoft + 9, &mFrontStencilFailOp);
        param->_write(memoft + 10, &mFrontDepthFailOp);
        param->_write(memoft + 11, &mFrontTestPassOp);
        param->_write(memoft + 12, &mBackStencilFailOp);
        param->_write(memoft + 13, &mBackDepthFailOp);
        param->_write(memoft + 14, &mBackTestPassOp);
        param->_write(memoft + 15, &mBias);
        param->_write(memoft + 16, &mBiasFactor);
        param->_write(memoft + 17, &mSlopeScaleBias);
        param->_write(memoft + 18, &mDepthFunc);
        param->_write(memoft + 19, &mMark);
    }
    //-----------------------------------------------------------------------
    void ShaderChStencil::write(const GpuProgramParam * param, NCount memoft)
    {
        param->_read(memoft, &mFrontTestMark);
        param->_read(memoft + 2, &mBackTestMark);
        param->_read(memoft + 3, &mFrontCmpMark);
        param->_read(memoft + 4, &mBackCmpMark);
        param->_read(memoft + 5, &mFrontCmpValue);
        param->_read(memoft + 6, &mBackCmpValue);
        param->_read(memoft + 7, &mFrontFunc);
        param->_read(memoft + 8, &mBackFunc);
        param->_read(memoft + 9, &mFrontStencilFailOp);
        param->_read(memoft + 10, &mFrontDepthFailOp);
        param->_read(memoft + 11, &mFrontTestPassOp);
        param->_read(memoft + 12, &mBackStencilFailOp);
        param->_read(memoft + 13, &mBackDepthFailOp);
        param->_read(memoft + 14, &mBackTestPassOp);
        param->_read(memoft + 15, &mBias);
        param->_read(memoft + 16, &mBiasFactor);
        param->_read(memoft + 17, &mSlopeScaleBias);
        param->_read(memoft + 18, &mDepthFunc);
        param->_read(memoft + 19, &mMark);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ShaderChFog
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    const ShaderChFog ShaderChFog::White = ShaderChFog();
    const ShaderChFog ShaderChFog::Red = ShaderChFog();
    const ShaderChFog ShaderChFog::Green = ShaderChFog();
    const ShaderChFog ShaderChFog::Yellow = ShaderChFog();
    const ShaderChFog ShaderChFog::Blue = ShaderChFog();
    const ShaderChFog ShaderChFog::None = ShaderChFog();
    //-----------------------------------------------------------------------
    ShaderChFog::ShaderChFog() :
        ShaderChBase(SCT_Fog),
        mMode(FM_NONE),
        mColour(Colour::White),
        mBegin(0.0f),
        mEnd(1.0f),
        mDensity(0.001f)
    {
    }
    //-----------------------------------------------------------------------
    ShaderChFog::ShaderChFog(const ShaderChFog & o)
    {
        mMode = o.mMode;
        mColour = o.mColour;
        mBegin = o.mBegin;
        mEnd = o.mEnd;
        mDensity = o.mDensity;
    }
    //-----------------------------------------------------------------------
    ShaderChFog::ShaderChFog(FogMode mode, const Colour & colour, NIIf expDensity,
        NIIf linearStart, NIIf linearEnd):
            mMode(mode),
            mColour(colour),
            mBegin(linearStart),
            mEnd(linearEnd),
            mDensity(expDensity)
    {
    }
    //-----------------------------------------------------------------------
    bool ShaderChFog::operator ==(const ShaderChFog & o) const
    {
        if(mMode == o.mMode && mColour == o.mColour &&
            mBegin == o.mBegin && mEnd == o.mEnd && mDensity == o.mDensity)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    bool ShaderChFog::operator < (const ShaderChFog & o) const
    {
        if (mColour * ((mEnd - mBegin) * mDensity) < o.mColour * ((o.mEnd - o.mBegin) * o.mDensity))
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    void ShaderChFog::read(GpuProgramParam * param, NCount memoft) const
    {
        param->_write(memoft, mColour.ptr(), 4);
        param->_write(memoft + 4, &mMode);
        param->_write(memoft + 5, &mBegin);
        param->_write(memoft + 6, &mEnd);
        param->_write(memoft + 7, &mDensity);
    }
    //------------------------------------------------------------------------
    void ShaderChFog::write(const GpuProgramParam * param, NCount memoft)
    {
        param->_read(memoft, mColour.ptr(), 4);
        param->_read(memoft + 4, &mMode);
        param->_read(memoft + 5, &mBegin);
        param->_read(memoft + 6, &mEnd);
        param->_read(memoft + 7, &mDensity);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ShaderChPoint
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    const ShaderChPoint ShaderChPoint::Default;
    //------------------------------------------------------------------------
    ShaderChPoint::ShaderChPoint() :
        ShaderChBase(SCT_Point),
        mSize(1.0f),
        mLineWidth(1.0f),
        mMinSize(0.0f),
        mMaxSize(0.0f),
        mConstant(1.0f),
        mLinear(1.0f),
        mQuadratic(0.0f),
        mMark(0)
    {
    }
    //------------------------------------------------------------------------
    ShaderChPoint::ShaderChPoint(const ShaderChPoint & o)
    {
        mSize = o.mSize;
        mLineWidth = o.mLineWidth;
        mMinSize = o.mMinSize;
        mMaxSize = o.mMaxSize;
        mConstant = o.mConstant;
        mLinear = o.mLinear;
        mQuadratic = o.mQuadratic;
        mMark = o.mMark;
    }
    //------------------------------------------------------------------------
    ShaderChPoint::ShaderChPoint(NIIf size, NIIf minsize, NIIf maxsize) :
        mSize(size),
        mMinSize(minsize),
        mMaxSize(maxsize),
        mConstant(1.0f),
        mLinear(1.0f),
        mQuadratic(0.0f),
        mMark(0)
    {
    }
    //------------------------------------------------------------------------
    bool ShaderChPoint::operator == (const ShaderChPoint & o) const
    {
        if(mSize == o.mSize &&
            mMinSize == o.mMinSize &&
            mMaxSize == o.mMaxSize &&
            mConstant == o.mConstant &&
            mLinear == o.mLinear &&
            mQuadratic == o.mQuadratic &&
            mMark == o.mMark)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    bool ShaderChPoint::operator < (const ShaderChPoint & o) const
    {
        if (mSize + mMinSize + mMaxSize < o.mSize + o.mMinSize + o.mMaxSize ||
            mConstant + mLinear + mQuadratic + mMark < o.mConstant + o.mLinear + o.mQuadratic + o.mMark)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    void ShaderChPoint::read(GpuProgramParam * param, NCount memoft) const
    {
        param->_write(memoft, &mSize);
        param->_write(memoft + 1, &mMinSize);
        param->_write(memoft + 2, &mMaxSize);
        param->_write(memoft + 3, &mLineWidth);
        param->_write(memoft + 4, &mConstant);
        param->_write(memoft + 5, &mLinear);
        param->_write(memoft + 6, &mQuadratic);
        param->_write(memoft + 7, &mMark);
    }
    //------------------------------------------------------------------------
    void ShaderChPoint::write(const GpuProgramParam * param, NCount memoft)
    {
        param->_read(memoft, &mSize, 1);
        param->_read(memoft + 1, &mMinSize, 1);
        param->_read(memoft + 2, &mMaxSize, 1);
        param->_read(memoft + 3, &mLineWidth, 1);
        param->_read(memoft + 4, &mConstant, 1);
        param->_read(memoft + 5, &mLinear, 1);
        param->_read(memoft + 6, &mQuadratic, 1);
        param->_read(memoft + 7, &mMark, 1);
    }
    //------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // ShaderCh
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    ShaderCh::ShaderCh(ShaderFusion * sf, Nidx index, bool fusion):
        mParent(sf),
        mIndex(index),
        mSysCullMode(SCM_BACK),
        mMaxLightCount(NII_MAX_LIGHT),
        mLightCountLimit(1),
        mRenderCount(1),
        mLightBegin(0),
        mOnlyLightType(LT_NONE),
        mLightMask(0xFFFFFFFF),
        mShaderMode(SM_GOURAUD),
        mShaderStep(SS_Unknow),
        mColour(const_cast<ShaderChColour *>(MaterialManager::InvalidColour)),
        mPoint(const_cast<ShaderChPoint *>(MaterialManager::InvalidPoint)),
        mFog(const_cast<ShaderChFog *>(MaterialManager::InvalidFog)),
        mBlend(const_cast<ShaderChBlend *>(MaterialManager::InvalidBlend)),
        mStencil(const_cast<ShaderChStencil *>(MaterialManager::InvalidStencil)),
        mTexture(const_cast<ShaderChTexture *>(MaterialManager::InvalidTexture)),
        mProgram(const_cast<ShaderChProgram *>(MaterialManager::InvalidProgram)),
        mEachLight(false),
        mDepthSort(true),
        mAutoReduce(true),
        mForceDepthSort(false)
    {
        if (fusion)
            mMark = ColourMark::CM_RGBA | ShaderBase::SB_Light | ShaderBase::SB_Texture | SB_Fusion;
        else
            mMark = ColourMark::CM_RGBA | ShaderBase::SB_Light | ShaderBase::SB_Texture;
        mTexture = N_new ShaderChTexture(this);

        StrConv::conv(mIndex, mName);
   }
    //-----------------------------------------------------------------------------
    ShaderCh::ShaderCh(ShaderFusion * sf, Nidx index, const ShaderCh & o, bool fusion) :
        mParent(sf),
        mIndex(index)
    {
        *this = o;
        if(fusion)
            mMark |= SB_Fusion;
        else
            mMark &= ~SB_Fusion;
    }
    //-----------------------------------------------------------------------------
    ShaderCh::~ShaderCh()
    {
        if(mMark & SB_Colour)
        {
            N_delete mColour;
            mColour = 0;
        }
        if(mMark & SB_Point)
        {
            N_delete mPoint;
            mPoint = 0;
        }
        if(mMark & SB_Fog)
        {
            N_delete mFog;
            mFog = 0;
        }
        if(mMark & SB_Blend)
        {
            N_delete mBlend;
            mBlend = 0;
        }
        if(mMark & SB_DepthStencil)
        {
            N_delete mStencil;
            mStencil = 0;
        }
        if(mMark & SB_Texture)
        {
            N_delete mTexture;
            mTexture = 0;
        }
        if(mMark & SB_Program)
        {
            N_delete mProgram;
            mProgram = 0;
        }
    }
    //-----------------------------------------------------------------------------
    ShaderCh & ShaderCh::operator=(const ShaderCh & o)
    {
        mName = o.mName;
        if(o.mMark & SB_Colour)
        {
            if(mMark & SB_Colour)
                *mColour = *o.mColour;
            else
                mColour = N_new ShaderChColour(*o.mColour);
        }
        else if (o.mMark & SB_ColourRef)
        {
            if (mMark & SB_Colour)
            {
                N_delete mColour;
                mMark &= ~SB_Colour;
            }
            mColour = o.mColour;
            mMark |= SB_ColourRef;
        }

        if(o.mMark & SB_Point)
        {
            if(mMark & SB_Point)
                *mPoint = *o.mPoint;
            else
                mPoint = N_new ShaderChPoint(*o.mPoint);
        }
        else if (o.mMark & SB_PointRef)
        {
            if (mMark & SB_Point)
            {
                N_delete mPoint;
                mMark &= ~SB_Point;
            }
            mPoint = o.mPoint;
            mMark |= SB_PointRef;
        }

        if(o.mMark & SB_Fog)
        {
            if(mMark & SB_Fog)
                *mFog = *o.mFog;
            else
                mFog = N_new ShaderChFog(*o.mFog);
        }
        else if (o.mMark & SB_FogRef)
        {
            if (mMark & SB_Fog)
            {
                N_delete mFog;
                mMark &= ~SB_Fog;
            }
            mFog = o.mFog;
            mMark |= SB_FogRef;
        }

        if(o.mMark & SB_Blend)
        {
            if(mMark & SB_Blend)
                *mBlend = *o.mBlend;
            else
                mBlend = N_new ShaderChBlend(*o.mBlend);
        }
        else if (o.mMark & SB_BlendRef)
        {
            if (mMark & SB_Blend)
            {
                N_delete mBlend;
                mMark &= ~SB_Blend;
            }
            mBlend = o.mBlend;
            mMark |= SB_BlendRef;
        }

        if(o.mMark & SB_DepthStencil)
        {
            if(mMark & SB_DepthStencil)
                *mStencil = *o.mStencil;
            else
                mStencil = N_new ShaderChStencil(*o.mStencil);
        }
        else if (o.mMark & SB_DepthStencilRef)
        {
            if (mMark & SB_DepthStencil)
            {
                N_delete mStencil;
                mMark &= ~SB_DepthStencil;
            }
            mStencil = o.mStencil;
            mMark |= SB_DepthStencilRef;
        }

        if(o.mMark & SB_Texture)
        {
            if(mMark & SB_Texture)
                *mTexture = *o.mTexture;
            else
                mTexture = N_new ShaderChTexture(*o.mTexture);
        }
        else if (o.mMark & SB_TextureRef)
        {
            if (mMark & SB_Texture)
            {
                N_delete mTexture;
                mMark &= ~SB_Texture;
            }
            mTexture = o.mTexture;
            mMark |= SB_TextureRef;
        }

        if(o.mMark & SB_Program)
        {
            if(mMark & SB_Program)
                *mProgram = *o.mProgram;
            else
                mProgram = N_new ShaderChProgram(*o.mProgram);
        }
        else if (o.mMark & SB_ProgramRef)
        {
            if (mMark & SB_Program)
            {
                N_delete mProgram;
                mMark &= ~SB_Program;
            }
            mProgram = o.mProgram;
            mMark |= SB_ProgramRef;
        }

        mMark = o.mMark;
        mDepthSort = o.mDepthSort;
        mForceDepthSort = o.mForceDepthSort;
        mSysCullMode = o.mSysCullMode;
        mMaxLightCount = o.mMaxLightCount;
        mEachLight = o.mEachLight;
        mLightCountLimit = o.mLightCountLimit;
        mOnlyLightType = o.mOnlyLightType;
        mShaderMode = o.mShaderMode;
        mAutoReduce = o.mAutoReduce;
        mRenderCount = o.mRenderCount;

        //mLightClip = o.mLightClip;
        //mLightClipPlane = o.mLightClipPlane;
        mShaderStep = o.mShaderStep;
        mLightMask = o.mLightMask;

        return *this;
    }
    //-----------------------------------------------------------------------
    void ShaderCh::addRef(ShaderFusion * fusion)
    {
        
    }
    //-----------------------------------------------------------------------
    void ShaderCh::removeRef(ShaderFusion * fusion)
    {
        
    }
    //-----------------------------------------------------------------------
    bool ShaderCh::IndexLess::operator()(const ShaderCh * a, const ShaderCh * b) const
    {
        Nui32 indexa = a->mIndex;
        Nui32 indexb = b->mIndex;
        if(indexa == indexb)
        {
            return a < b;
        }
        else
        {
            return indexa < indexb;
        }
    }
    //-----------------------------------------------------------------------
    ShaderChColour * ShaderCh::setColour(SetOperatorType type, const ShaderChColour * refIn)
    {
        if(type == SOT_Ref)
        {
            if(mMark & SB_ColourAlpha)
            {
                N_delete mColour;
                mMark &= ~SB_ColourAlpha;
            }
            mColour = const_cast<ShaderChColour *>(refIn);
            mMark |= SB_ColourAlphaRef;
        }
        else if(type == SOT_Set)
        {
            if(!(mMark & SB_ColourAlpha))
            {
                mColour = N_new ShaderChColour(*refIn);
                mMark |= SB_ColourAlpha;
                mMark &= ~SB_ColourAlphaRef;
            }
            else
            {
                *mColour = *refIn;
            }
        }
        return mColour;
    }
    //-----------------------------------------------------------------------
    ShaderChPoint * ShaderCh::setPoint(SetOperatorType type, const ShaderChPoint * refIn)
    {
        if(type == SOT_Ref)
        {
            if(mMark & SB_Point)
            {
                N_delete mPoint;
                mMark &= ~SB_Point;
            }
            mPoint = const_cast<ShaderChPoint *>(refIn);
            mMark |= SB_PointRef;
        }
        else if(type == SOT_Set)
        {
            if(!(mMark & SB_Point))
            {
                mPoint = N_new ShaderChPoint(*refIn);
                mMark |= SB_Point;
                mMark &= ~SB_PointRef;
            }
            else
            {
                *mPoint = *refIn;
            }
        }
        return mPoint;
    }
    //-----------------------------------------------------------------------
    ShaderChFog * ShaderCh::setFog(SetOperatorType type, const ShaderChFog * refIn)
    {
        if(type == SOT_Ref)
        {
            if(mMark & SB_Fog)
            {
                N_delete mFog;
                mMark &= ~SB_Fog;
            }
            mMark |= SB_FogRef;
            mFog = const_cast<ShaderChFog *>(refIn);
        }
        else if(type == SOT_Set)
        {
            if(!(mMark & SB_Fog))
            {
                mFog = N_new ShaderChFog(*refIn);
                mMark |= SB_Fog;
                mMark &= ~SB_FogRef;
            }
            else
            {
                *mFog = *refIn;
            }
        }
        return mFog;
    }
    //-----------------------------------------------------------------------
    ShaderChBlend * ShaderCh::setBlend(SetOperatorType type, const ShaderChBlend * refIn)
    {
        if(type == SOT_Ref)
        {
            if(mMark & SB_Blend)
            {
                N_delete mBlend;
                mMark &= ~SB_Blend;
            }
            mMark |= SB_BlendRef;
            mBlend = const_cast<ShaderChBlend *>(refIn);
        }
        else if(type == SOT_Set)
        {
            if(!(mMark & SB_Blend))
            {
                mBlend = N_new ShaderChBlend(*refIn);
                mMark |= SB_Blend;
                mMark &= ~SB_BlendRef;
            }
            else
            {
                *mBlend = *refIn;
            }
        }
        return mBlend;
    }
    //-----------------------------------------------------------------------
    ShaderChStencil * ShaderCh::setStencil(SetOperatorType type, const ShaderChStencil * refIn)
    {
        if(type == SOT_Ref)
        {
            if(mMark & SB_DepthStencil)
            {
                N_delete mStencil;
                mMark &= ~SB_DepthStencil;
            }
            mMark |= SB_DepthStencilRef;
            mStencil = const_cast<ShaderChStencil *>(refIn);
        }
        else if(type == SOT_Set)
        {
            if(!(mMark & SB_DepthStencil))
            {
                mStencil = N_new ShaderChStencil(*refIn);
                mMark |= SB_DepthStencil;
                mMark &= ~SB_DepthStencilRef;
            }
            else
            {
                *mStencil = *refIn;
            }
        }
        return mStencil;
    }
    //-----------------------------------------------------------------------
    ShaderChTexture * ShaderCh::setTexture(SetOperatorType type, const ShaderChTexture * refIn)
    {
        if(type == SOT_Ref)
        {
            if(mMark & SB_Texture)
            {
                N_delete mTexture;
                mMark &= ~SB_Texture;
            }
            mMark |= SB_TextureRef;
            mTexture = const_cast<ShaderChTexture *>(refIn);
        }
        else if(type == SOT_Set)
        {
            if(!(mMark & SB_Texture))
            {
                mTexture = N_new ShaderChTexture(this, *refIn);
                mMark |= SB_Texture;
                mMark &= ~SB_TextureRef;
            }
            else
            {
                *mTexture = *refIn;
            }
        }

        return mTexture;
    }
    //-----------------------------------------------------------------------
    ShaderChProgram * ShaderCh::setProgram(SetOperatorType type, const ShaderChProgram * refIn)
    {
        if(type == SOT_Ref)
        {
            if(mMark & SB_Program)
            {
                N_delete mProgram;
                mMark &= ~SB_Program;
            }
            mMark |= SB_ProgramRef;
            mProgram = const_cast<ShaderChProgram *>(refIn);
        }
        else if(type == SOT_Set)
        {
            if(!(mMark & SB_Program))
            {
                mProgram = N_new ShaderChProgram(this, *refIn);
                mMark |= SB_Program;
                mMark &= ~SB_ProgramRef;
            }
            else
            {
                *mProgram = *refIn;
            }
        }

        return mProgram;
    }
    //-----------------------------------------------------------------------
    void ShaderCh::plan()
    {
        mTexture->plan();
        mProgram->plan();
    }
    //-----------------------------------------------------------------------
    void ShaderCh::unplan()
    {
        mTexture->unplan();
        mProgram->unplan();
    }
    //-----------------------------------------------------------------------
    void ShaderCh::load()
    {
        mTexture->load();
        mProgram->load();
    }
    //-----------------------------------------------------------------------
    void ShaderCh::unload()
    {
        mTexture->unload();
        mProgram->unload();
    }
    //-----------------------------------------------------------------------
    GroupID ShaderCh::getResourceGroup() const                
    { 
        return mParent->getResourceGroup(); 
    }
    //-----------------------------------------------------------------------
    bool ShaderCh::isLoad() const                             
    { 
        return mParent->isLoad(); 
    }
    //-----------------------------------------------------------------------
    void ShaderCh::enable(ShaderBase mark)
    {
        if (mark > SB_Program && mark < SB_Frame_Depth)
            return;
        {
            switch (mark)
            {
            case SB_Fog:
                if (!(mMark & SB_Fog & SB_FogRef))
                {
                    mFog = N_new ShaderChFog();
                    mMark |= SB_Fog;
                }
                break;
            case SB_Point:
                if (!(mMark & SB_Point & SB_PointRef))
                {
                    mPoint = N_new ShaderChPoint();
                    mMark |= SB_Point;
                }
                break;
            case SB_Colour:
            case SB_Alpha:
            case SB_ColourAlpha:
                if (!(mMark & SB_ColourAlpha & SB_ColourAlphaRef))
                {
                    mColour = N_new ShaderChColour();
                    mMark |= SB_ColourAlpha;
                }
                break;
            case SB_Blend:
                if (!(mMark & SB_Blend & SB_BlendRef))
                {
                    mBlend = N_new ShaderChBlend();
                    mMark |= SB_Blend;
                }
                break;
            case SB_Depth:
            case SB_Stencil:
            case SB_DepthStencil:
                if (!(mMark & SB_DepthStencil & SB_DepthStencilRef))
                {
                    mStencil = N_new ShaderChStencil();
                    mMark |= SB_DepthStencil;
                }
                break;
            case SB_Texture:
                if (!(mMark & SB_Texture & SB_TextureRef))
                {
                    mTexture = N_new ShaderChTexture(this);
                    mMark |= SB_Texture;
                }
                break;
            case SB_Program:
                if (!(mMark & SB_Program & SB_ProgramRef))
                {
                    mProgram = N_new ShaderChProgram(this);
                    mMark |= SB_Program;
                }
                break;
            default:
                mMark |= mark;
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderCh::disable(ShaderBase mark)
    {
        if (mark > SB_Program && mark < SB_Frame_Depth)
            return;
        {
            switch (mark)
            {
            case SB_Fog:
                if (mMark & SB_Fog)
                {
                    N_delete mFog;
                    mFog = (ShaderChFog *)MaterialManager::InvalidFog;
                    mMark &= ~SB_Fog;
                }
                else if (mMark & SB_FogRef)
                {
                    mFog = (ShaderChFog *)MaterialManager::InvalidFog;
                    mMark &= ~SB_FogRef;
                }
                break;
            case SB_Point:
                if (mMark & SB_Point)
                {
                    N_delete mPoint;
                    mPoint = (ShaderChPoint *)MaterialManager::InvalidPoint;
                    mMark &= ~SB_Point;
                }
                else if (mMark & SB_PointRef)
                {
                    mPoint = (ShaderChPoint *)MaterialManager::InvalidPoint;
                    mMark &= ~SB_PointRef;
                }
                break;
            case SB_Alpha:
            case SB_Colour:
            case SB_ColourAlpha:
                if (mMark & SB_ColourAlpha)
                {
                    N_delete mColour;
                    mColour = (ShaderChColour *)MaterialManager::InvalidColour;
                    mMark &= ~SB_ColourAlpha;
                }
                else if (mMark & SB_ColourAlphaRef)
                {
                    mColour = (ShaderChColour *)MaterialManager::InvalidColour;
                    mMark &= ~SB_ColourAlphaRef;
                }
                break;
            case SB_Blend:
                if (mMark & SB_Blend)
                {
                    N_delete mBlend;
                    mBlend = (ShaderChBlend *)MaterialManager::InvalidBlend;
                    mMark &= ~SB_Blend;
                }
                else if (mMark & SB_BlendRef)
                {
                    mBlend = (ShaderChBlend *)MaterialManager::InvalidBlend;
                    mMark &= ~SB_BlendRef;
                }
                break;
            case SB_Depth:
            case SB_Stencil:
            case SB_DepthStencil:
                if (mMark & SB_DepthStencil)
                {
                    N_delete mStencil;
                    mStencil = (ShaderChStencil *)MaterialManager::InvalidStencil;
                    mMark &= ~SB_DepthStencil;
                }
                else if (mMark & SB_DepthStencilRef)
                {
                    mStencil = (ShaderChStencil *)MaterialManager::InvalidStencil;
                    mMark &= ~SB_DepthStencilRef;
                }
                break;
            case SB_Texture:
                if (mMark & SB_Texture)
                {
                    N_delete mTexture;
                    mTexture = (ShaderChTexture *)MaterialManager::InvalidTexture;
                    mMark &= ~SB_Texture;
                }
                else if (mMark & SB_TextureRef)
                {
                    mTexture = (ShaderChTexture *)MaterialManager::InvalidTexture;
                    mMark &= ~SB_TextureRef;
                }
                break;
            case SB_Program:
                if (mMark & SB_Program)
                {
                    N_delete mProgram;
                    mProgram = (ShaderChProgram *)MaterialManager::InvalidProgram;
                    mMark &= ~SB_Program;
                }
                else if (mMark & SB_ProgramRef)
                {
                    mProgram = (ShaderChProgram *)MaterialManager::InvalidProgram;
                    mMark &= ~SB_ProgramRef;
                }
                break;
            default:
                mMark &= ~mark;
            }
        }
    }
    //-----------------------------------------------------------------------
    void ShaderCh::notify()
    {
        mParent->notify();
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ShaderChMaterial
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    ShaderChMaterial::ShaderChMaterial(Nid name, RenderPattern * rp, 
        ShaderChStencil * stencil, ShaderChBlend * blend, const PropertyData & params) :
        mParent(rp),
        mOrigin(0),
        mID(name),
        mType(rp->getType()),
        mAutoLoadTexture(true),
        mAlphaTestOnlyInCast(false),
        mAlphaTestCmp(CMPF_ALWAYS_PASS),
        mAlphaTestValue(0.5f),
        mShadowConstantBias(0.01f)
    {
        mHash = 0;
        mDepth[0] = mDepth[1] = 0;
        mBlend[0] = mBlend[1] = 0;
        setStencil(stencil, false);
        setBlend(blend, false);

        String value;
        if(params.get(N_PropertyRender::AlphaTest, value))
        {
            mAlphaTestCmp = CMPF_LESS;

            if(value.size())
            {
                StringList vec;
                StrUtil::split(value, vec);
                
                StringList::const_iterator itor = vec.begin();
                StringList::const_iterator end  = vec.end();
                while(itor != end)
                {
                    if( *itor == _T("less") )
                        mAlphaTestCmp = CMPF_LESS;
                    else if( *itor == _T("less_equal") )
                        mAlphaTestCmp = CMPF_LESS_EQUAL;
                    else if( *itor == _T("equal") )
                        mAlphaTestCmp = CMPF_EQUAL;
                    else if( *itor == _T("greater") )
                        mAlphaTestCmp = CMPF_GREATER;
                    else if( *itor == _T("greater_equal") )
                        mAlphaTestCmp = CMPF_GREATER_EQUAL;
                    else if( *itor == _T("not_equal") )
                        mAlphaTestCmp = CMPF_NOT_EQUAL;
                    else
                    {
                        StrConv::conv(*itor, mAlphaTestValue, 0.5f);
                        StrConv::conv(*itor, mAlphaTestOnlyInCast, false);
                    }

                    ++itor;
                }
            }
        }
    }
    //-----------------------------------------------------------------------------------
    ShaderChMaterial::~ShaderChMaterial()
    {
        N_assert(mRefList.empty() && "This Datablock is still being used by some Renderables."
            " Change their Datablocks before destroying this.");

        RenderSys * rsys = mParent->getRender();
        if(rsys)
        {
            rsys->destroyStencil(mDepth[0]);
            rsys->destroyBlend(mBlend[0]);
            rsys->destroyStencil(mDepth[1]);
            rsys->destroyBlend(mBlend[1]);
            rsys->destroyStencil(mDepth[2]);
            rsys->destroyBlend(mBlend[2]);
        }
    }
    //-----------------------------------------------------------------------------------
    ShaderChMaterial* ShaderChMaterial::clone(const String & name) const
    {
        //OgreProfileExhaustive( "ShaderChMaterial::clone" );

        ShaderChMaterial * mat = mParent->createMaterial(name, name, ShaderChStencil(), ShaderChBlend(), PropertyData());

        mat->setStencil(mDepth[0], ShadowType::ST_Normal);
        mat->setStencil(mDepth[1], ShadowType::ST_Cast);
        mat->setStencil(mDepth[2], ShadowType::ST_Receive);

        mat->setBlend(mBlend[0], ShadowType::ST_Normal);
        mat->setBlend(mBlend[1], ShadowType::ST_Cast);
        mat->setBlend(mBlend[2], ShadowType::ST_Receive);

        mat->mAlphaTestCmp = mAlphaTestCmp;
        mat->mAlphaTestOnlyInCast = mAlphaTestOnlyInCast;
        mat->mAlphaTestValue = mAlphaTestValue;

        mat->mShadowConstantBias = mShadowConstantBias;

        cloneImpl(mat);

        mat->init();

        return mat;
    }
    //-----------------------------------------------------------------------------------
    void ShaderChMaterial::setStencil(ShaderChStencil * stencil, ShadowType type)
    {
        //OgreProfileExhaustive( "ShaderChMaterial::setMacroblockPtr" );

        RenderSys * rsys = mParent->getRender();

        rsys->ref(stencil);
        if(mDepth[type])
            rsys->destroyStencil(mDepth[type]);
        mDepth[type] = stencil;

        uint16 macroId = 0;
        uint16 blendId = 0;

        if(mDepth[type])
            macroId = mDepth[type]->mId;
        if(mBlend[type])
            blendId = mBlend[type]->mId;
        N_64_16(mHash, type, ((macroId & 0x1F) << 5) | (blendId & 0x1F));

        if(type != ST_Cast)
        {
            if(mParent->getRender()->isBackFaceInCast() && stencil->mCullMode != CM_None)
            {
                ShaderChStencil * casterSCS = static_cast<ShaderChStencil *>(stencil->clone());
                casterSCS->mCullMode = stencil->mCullMode == CM_Clockwise ? CM_Anticlockwise : CM_Clockwise;
                rsys->sync(casterSCS);
                rsys->ref(casterSCS);
                if(mDepth[ST_Cast])
                    rsys->destroyStencil(mDepth[ST_Cast]);
                mDepth[ST_Cast] = casterSCS;

                if(mDepth[ST_Cast])
                    macroId = mDepth[ST_Cast]->mId;
                else
                    macroId = 0;
                if(mBlend[ST_Cast])
                    blendId = mBlend[ST_Cast]->mId;
                else
                    blendId = 0;
                N_64_16(mHash, ST_Cast, ((macroId & 0x1F) << 5) | (blendId & 0x1F));
        
            }
            else
            {
                rsys->ref(mDepth[0]);
                if(mDepth[ST_Cast])
                    rsys->destroyStencil(mDepth[ST_Cast]);
                mDepth[ST_Cast] = mDepth[0];

                if(mDepth[ST_Cast])
                    macroId = mDepth[ST_Cast]->mId;
                else
                    macroId = 0;
                if(mBlend[ST_Cast])
                    blendId = mBlend[ST_Cast]->mId;
                else
                    blendId = 0;
                N_64_16(mHash, ST_Cast, ((macroId & 0x1F) << 5) | (blendId & 0x1F));
            }
        }

        queueRef();
    }
    //-----------------------------------------------------------------------------------
    void ShaderChMaterial::setBlend(ShaderChBlend * blend, ShadowType type)
    {
        //OgreProfileExhaustive( "ShaderChMaterial::setBlendblockPtr" );

        RenderSys * rsys = mParent->getRender();

        rsys->ref(blend);
        if(mBlend[type])
            rsys->destroyBlend(mBlend[type]);
        mBlend[type] = blend;

        uint16 macroId = 0;
        uint16 blendId = 0;

        if(mDepth[type])
            macroId = mDepth[type]->mId;
        if(mBlend[type])
            blendId = mBlend[type]->mId;
        N_64_16(mHash, type, ((macroId & 0x1F) << 5) | (blendId & 0x1F));

        if(type != ST_Cast)
        {
            rsys->ref(mBlend[0]);
            if(mBlend[ST_Cast])
                rsys->destroyBlend(mBlend[ST_Cast]);
            mBlend[ST_Cast] = mBlend[0];

            if(mDepth[ST_Cast])
                macroId = mDepth[ST_Cast]->mId;
            else
                macroId = 0;
            if(mBlend[ST_Cast])
                blendId = mBlend[ST_Cast]->mId;
            else
                blendId = 0;
            N_64_16(mHash, ST_Cast, ((macroId & 0x1F) << 5) | (blendId & 0x1F));
        }
        
        queueRef();
    }
    //-----------------------------------------------------------------------------------
    void ShaderChMaterial::setAlphaTest(CmpMode mode, bool cast)
    {
        if(mAlphaTestCmp != mode || mAlphaTestOnlyInCast != cast)
        {
            mAlphaTestCmp = mode;
            mAlphaTestOnlyInCast = cast;
            queueRef();
        }
    }
    //-----------------------------------------------------------------------------------
    void ShaderChMaterial::setAlphaTestValue(float value)
    {
        mAlphaTestValue = value;
    }
    //-----------------------------------------------------------------------------------
    void ShaderChMaterial::addRef(GeometryObj * obj)
    {
        N_assert1(obj->mRefIndex == (uint32)~0 && "GeometryObj must be unlinked before being linked again!");

        obj->mRefIndex = mRefList.size();
        mRefList.push_back(obj);
    }
    //-----------------------------------------------------------------------------------
    void ShaderChMaterial::removeRef(GeometryObj * obj)
    {
        if(obj->mRefIndex >= mRefList.size() || obj != *(mRefList.begin() + obj->mRefIndex))
        {
            N_EXCEPT(Internal, "A GeometryObj had it's mRefIndex out of date!!! (or the GeometryObj wasn't being tracked by this mat)");
        }

        GeometryObjList::type::iterator itor = mRefList.begin() + obj->mRefIndex;
        itor = remove(mRefList, itor);

        //The GeometryObj that was at the end got swapped and has now a different index
        if(itor != mRefList.end())
            (*itor)->mRefIndex = itor - mRefList.begin();

        obj->mRefIndex = ~0;
    }
    //-----------------------------------------------------------------------------------
    void ShaderChMaterial::queueRef(bool onlyUnRef)
    {
        //OgreProfileExhaustiveAggr( "ShaderChMaterial::queueRef" );

        GeometryObjList::type::const_iterator itor = mRefList.begin();
        GeometryObjList::type::const_iterator end  = mRefList.end();

        while(itor != end)
        {
            try
            {
                if(!onlyUnRef || !(*itor)->getNormalHash() || !(*itor)->getCasterHash())
                {
                    mParent->addObject(*itor);
                }
                ++itor;
            }
            catch(Exception & e)
            {
                size_t currentIdx = itor - mRefList.begin();
                N_log( e.getFullDescription() );
                N_log( "Couldn't apply change to mat '" + mID.getFriendlyText() + "' for this renderable. "
                    "Using default one. Check previous log messages to see if there's more information.", LML_CRITICAL);


                if( mType == RPT_Single )
                {
                    (*itor)->setMaterial(mParent->getRender()->getMaterial());
                }
                else
                {
                    //Try to use the default mat from the same
                    //HLMS as the one the user wanted us to apply
                    (*itor)->setMaterial(mParent->getMaterial());
                }

                //The container was changed with setMaterial change,
                //the iterators may have been invalidated.
                itor = mRefList.begin() + currentIdx;
                end  = mRefList.end();
            }
        }
    }
    //-----------------------------------------------------------------------------------
    void ShaderChMaterial::init()
    {
        
    }
    //-----------------------------------------------------------------------------------
    void ShaderChMaterial::cloneImpl(ShaderChMaterial * o) const
    {
    }
    //-----------------------------------------------------------------------------------
    bool ShaderChMaterial::isBackFaceInCast() const
    {
        CullingMode ocm = mDepth[0]->mCullMode;
        CullingMode tcm = mDepth[1]->mCullMode;

        if(mParent->getRender()->isBackFaceInCast() && ocm != CM_None && tcm != CM_None)
        {
            tcm = tcm == CM_Clockwise ? CM_Anticlockwise : CM_Clockwise;
        }
        return tcm != ocm;
    }
    //-----------------------------------------------------------------------------------
    const Colour & ShaderChMaterial::getDiffuseColour() const
    {
        return Colour::White;
    }
    //-----------------------------------------------------------------------------------
    const Colour & ShaderChMaterial::getEmissiveColour() const
    {
        return Colour::Black;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RenderStateObject
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void RenderStateObject::init()
    {
        memset(&mType, 0, (const Nui8 *)this + sizeof(RenderStateObject) - (const Nui8*)&mType);
        mType = OT_POINT_LIST;
    }
    //-----------------------------------------------------------------------
    RenderStateObject & RenderStateObject::operator = (const RenderStateObject & o)
    {
        mVS = o.mVS;
        mGS = o.mGS;
        mTS = o.mTS;
        mDS = o.mDS;
        mFS = o.mFS;
        mUnitsList = o.mUnitsList;
        mSampleObject = o.mSampleObject;
        memcpy(&mType, &o.mType, (const Nui8 *)this + sizeof(RenderStateObject) - (const Nui8*)&mType);
        return *this;
    }
    //-----------------------------------------------------------------------
    bool RenderStateObject::equalModel(const RenderStateObject & o) const
    {
        return mVS == o.mVS && mGS == o.mGS && mTS == o.mTS &&
            mDS == o.mDS && mFS == o.mFS && mUnitsList == o.mUnitsList;
    }
    //-----------------------------------------------------------------------
    int RenderStateObject::lesslModel(const RenderStateObject & o) const
    {
        if(mVS < o.mVS) return -1;
        if(mVS != o.mVS) return 1;
        if(mGS < o.mGS) return -1;
        if(mGS != o.mGS) return 1;
        if(mTS < o.mTS) return -1;
        if(mTS != o.mTS) return 1;
        if(mDS < o.mDS) return -1;
        if(mDS != o.mDS) return 1;
        if(mFS < o.mFS) return -1;
        if(mFS != o.mFS) return 1;
        if(mUnitsList < o.mUnitsList) return -1;
        if(mUnitsList != o.mUnitsList) return 1;

        return 0;
    }
    //-----------------------------------------------------------------------
    bool RenderStateObject::equalRender(const RenderStateObject & o) const
    {
        return equalModel(o) && mSampleObject == o.mSampleObject && 
            memcmp(&mType, &o.mType, (const Nui8*)&mRenderMark - (const Nui8*)&mType) == 0;
    }
    //-----------------------------------------------------------------------
    bool RenderStateObject::lessRender(const RenderStateObject & o) const
    {
        int re = lesslModel(o);
        if(re != 0)
            return re < 0;

        return mSampleObject < o.mSampleObject || memcmp(&mType, &o.mType, (const Nui8*)&mRenderMark - (const Nui8*)&mType) < 0;
    }
    //-----------------------------------------------------------------------
    void RenderStateObject::initStencil()
    {
        if(!mStencil->mDepthCheck)
        {
            //Depth check is already off, we don't need to hold a strong reference.
            mRenderMark &= ~SB_DepthCheckDisable;
        }
        if(!mStencil->mDepthWrite)
        {
            //Depth writes is already off, we don't need to hold a strong reference.
            mRenderMark &= ~SB_DepthWriteDisable;
        }
        if(mStencil->mCullMode == CM_None)
        {
            //Without culling there's nothing to invert, we don't need to hold a strong reference.
            mRenderMark &= ~SB_InvertVertexWinding;
        }

        if(mRenderMark)
        {
            ShaderChStencil * nscs = mStencil->clone();

            //This mRSO has no depth buffer, disable check and keep a hard copy (strong ref.)
            if(mRenderMark & SB_DepthCheckDisable)
                nscs->mDepthCheck = false;
            //This is a depth prepass, disable depth writes and keep a hard copy (strong ref.)
            if(mRenderMark & SB_DepthWriteDisable)
                nscs->mDepthWrite = false;
            //We need to invert culling mode.
            if(mRenderMark & SB_InvertVertexWinding)
            {
                nscs->mCullMode = nscs->mCullMode == CM_Clockwise ? CM_Anticlockwise : CM_Clockwise;
            }
            mRenderSys->sync(nscs);
            mStencil = nscs;
            //mStencil = mRenderSys->getStencil(nscs);
        }
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ComputeStateObject
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    void ComputeStateObject::init()
    {
        memset(mCeilCount, 0, sizeof(mCeilCount));
        memset(mGroupCount, 0, sizeof(mGroupCount));
    }
    //-----------------------------------------------------------------------
}