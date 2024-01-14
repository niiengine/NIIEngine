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
#include "NiiFusion.h"
#include "NiiEngine.h"
#include "NiiTextureManager.h"
#include "NiiFusionManager.h"
#include "NiiLogManager.h"
#include "NiiGpuBuffer.h"
#include "NiiTextureFrame.h"
#include "NiiMultiTextureFrame.h"
#include "NiiMaterialManager.h"
#include "NiiViewport.h"

#define MultiName(bname, idx) (bname + _T("/") + N_conv(idx))

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // FusionShaderOp
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    FusionShaderOp::FusionShaderOp(FusionShader * parent):
        mCh(parent),
        mType(OT_RenderQuad),
        mID(0),
        mFirstRenderQueue(RQG_Bg),
        mLastRenderQueue(RQG_NearEnd),
        mMaterialScheme(0),
        mClearBuffers(FBT_COLOUR|FBT_DEPTH),
        mClearColour(0.0,0.0,0.0,0.0),
        mClearDepth(1.0f),
        mClearStencil(0),
        mMaterial(0),
        mQuadLeft(-1),
        mQuadTop(1),
        mQuadRight(1),
        mQuadBottom(-1),
        mStencilFunc(CPM_ALWAYS_PASS),
        mStencilValue(0),
        mStencilMask(0xFFFFFFFF),
        mStencilFailOp(SOT_KEEP),
        mStencilDepthFailOp(SOT_KEEP),
        mStencilPassOp(SOT_KEEP),
        mQuadFarCorners(false),
        mQuadFarCornersViewSpace(false),
        mStencilTwoSided(false),
        mQuadCorner(false),
        mStencilCheck(false)
    {
        mInputs = N_alloc_t(OpUnit, NII_MAX_TextureLayer);
        for (NCount i = 0; i < NII_MAX_TextureLayer; ++i)
        {
            mInputs[i] = OpUnit(N_StrBlank, 0);
        }
    }
    //-----------------------------------------------------------------------
    FusionShaderOp::~FusionShaderOp()
    {
        if(mMaterial)
        {
            mMaterial->unref();
            mMaterial = 0;
        }
        free(mInputs);
    }
    //-----------------------------------------------------------------------
    void FusionShaderOp::setMaterial(Material * mat)
    {
        if(mMaterial != mat)
        {
            if(mMaterial != 0)
                mMaterial->unref();
            mMaterial = mat;
            mMaterial->ref(false);
        }
    }
    //-----------------------------------------------------------------------
    void FusionShaderOp::setMaterial(ResourceID rid)
    {
        if(mMaterial != 0)
        {
            if(mMaterial->getOriginID() != rid)
            {
                mMaterial->unref();
                mMaterial = static_cast<Material *>(N_Only(Material).get(rid));
                mMaterial->ref(false);
            }
        }
        else
        {
            mMaterial = static_cast<Material *>(N_Only(Material).get(rid));
            mMaterial->ref(false);
        }
    }
    //-----------------------------------------------------------------------
    void FusionShaderOp::setInput(Nidx idx, const String & input, Nidx secondidx)
    {
        N_assert(idx < NII_MAX_TextureLayer, "error");
        mInputs[idx] = OpUnit(input, secondidx);
    }
    //-----------------------------------------------------------------------
    const FusionShaderOp::OpUnit & FusionShaderOp::getInput(Nidx idx) const
    {
        N_assert(idx < NII_MAX_TextureLayer, "error");
        return mInputs[idx];
    }
    //-----------------------------------------------------------------------
    NCount FusionShaderOp::getInputCount() const
    {
        NCount count = 0;
        for(NCount x=0; x<NII_MAX_TextureLayer; ++x)
        {
            if(!mInputs[x].first.empty())
                count = x+1;
        }
        return count;
    }
    //-----------------------------------------------------------------------
    void FusionShaderOp::clearAllInput()
    {
        for(NCount x=0; x<NII_MAX_TextureLayer; ++x)
        {
            mInputs[x].first.clear();
        }
    }
    //-----------------------------------------------------------------------
    void FusionShaderOp::setQuadCorners(NIIf l, NIIf t, NIIf r, NIIf b)
    {
        mQuadCorner=true;
        mQuadLeft = l;
        mQuadTop = t;
        mQuadRight = r;
        mQuadBottom = b;
    }
    //-----------------------------------------------------------------------
    bool FusionShaderOp::getQuadCorners(NIIf & l, NIIf & t, NIIf & r, NIIf & b) const
    {
        l = mQuadLeft;
        t = mQuadTop;
        r = mQuadRight;
        b = mQuadBottom;
        return mQuadCorner;
    }
    //-----------------------------------------------------------------------
    void FusionShaderOp::setQuadFarCorners(bool farCorners, bool farCornersViewSpace)
    {
        mQuadFarCorners = farCorners;
        mQuadFarCornersViewSpace = farCornersViewSpace;
    }
    //-----------------------------------------------------------------------
    bool FusionShaderOp::isValid()
    {
        if(mType == OT_RenderQuad)
        {
            if(mMaterial == 0)
            {
                return false;
            }

            mMaterial->check();
            if(mMaterial->getValidCount() == 0)
            {
                return false;
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // FusionShader
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    FusionShader::FusionShader(FrameShader * fusion):
        mFusion(fusion),
        mVisibleMask(0xFFFFFFFF),
        mLodBias(1.0f),
        mMaterialScheme(0),
        mOnce(false),
        mInputMode(false),
        mShadowsEnable(true)
    {
        RenderSys * rsys = N_Engine().getRender();
        if(rsys)
        {
            mMaterialScheme = rsys->getMaterial();
        }
    }
    //-----------------------------------------------------------------------
    FusionShader::~FusionShader()
    {
        destroyAll();
    }
    //-----------------------------------------------------------------------
    FusionShaderOp * FusionShader::create()
    {
        FusionShaderOp * re = N_new FusionShaderOp(this);
        mPasses.push_back(re);
        return re;
    }
    //-----------------------------------------------------------------------
    void FusionShader::destroy(Nidx index)
    {
        N_assert (index < mPasses.size(), "Index out of bounds.");
        OpList::iterator i = mPasses.begin();
        std::advance(i, index);
        N_delete (*i);
        mPasses.erase(i);
    }
    //-----------------------------------------------------------------------
    FusionShaderOp * FusionShader::get(Nidx index)
    {
        N_assert (index < mPasses.size(), "Index out of bounds.");
        return mPasses[index];
    }
    //-----------------------------------------------------------------------
    void FusionShader::destroyAll()
    {
        OpList::iterator i, iend = mPasses.end();
        for (i = mPasses.begin(); i != iend; ++i)
        {
            N_delete (*i);
        }
        mPasses.clear();
    }
    //-----------------------------------------------------------------------
    bool FusionShader::isValid() const
    {
        OpList::const_iterator i, iend = mPasses.end();
        for(i = mPasses.begin(); i != iend; ++i)
        {
            if(!(*i)->isValid())
            {
                return false;
            }
        }
        return true;
    }
    //----------------------------------------------------------------------
    //----------------------------------------------------------------------
    // FrameShader
    //----------------------------------------------------------------------
    //----------------------------------------------------------------------
    FrameShader::FrameShader(Fusion * fusion) :
        mParent(fusion)
    {
        mOutputCh = N_new FusionShader(this);
    }
    //-----------------------------------------------------------------------
    FrameShader::~FrameShader()
    {
        removeAllDefine();
        removeAllCh();
        if (mOutputCh)
        {
            N_delete mOutputCh;
            mOutputCh = 0;
        }
    }
    //-----------------------------------------------------------------------
    FrameShader::FrameDefine * FrameShader::createDefine(const String & name)
    {
        FrameDefine * t = N_new FrameDefine();
        t->mName = name;
        mDefineList.push_back(t);
        return t;
    }
    //-----------------------------------------------------------------------
    void FrameShader::removeDefine(Nidx index)
    {
        N_assert (index < mDefineList.size(), "Index out of bounds.");
        DefineList::iterator i = mDefineList.begin() + index;
        N_delete (*i);
        mDefineList.erase(i);
    }
    //-----------------------------------------------------------------------
    FrameShader::FrameDefine * FrameShader::getDefine(Nidx index) const
    {
        N_assert (index < mDefineList.size(), "Index out of bounds.");
        return mDefineList[index];
    }
    //---------------------------------------------------------------------
    FrameShader::FrameDefine * FrameShader::getDefine(const String & name) const
    {
        DefineList::const_iterator i, iend = mDefineList.end();
        for(i = mDefineList.begin(); i != iend; ++i)
        {
            if((*i)->mName == name)
                return *i;
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    void FrameShader::removeAllDefine()
    {
        DefineList::iterator i, iend = mDefineList.end();
        for(i = mDefineList.begin(); i != iend; ++i)
        {
            N_delete (*i);
        }
        mDefineList.clear();
    }
    //-----------------------------------------------------------------------
    FusionShader * FrameShader::createCh()
    {
        FusionShader * t = N_new FusionShader(this);
        mTargetList.push_back(t);
        return t;
    }
    //-----------------------------------------------------------------------
    void FrameShader::removeCh(Nidx idx)
    {
        N_assert (idx < mTargetList.size(), "Index out of bounds.");
        TargetList::iterator i = mTargetList.begin() + idx;
        N_delete (*i);
        mTargetList.erase(i);
    }
    //-----------------------------------------------------------------------
    FusionShader * FrameShader::getCh(Nidx idx) const
    {
        N_assert (idx < mTargetList.size(), "Index out of bounds.");
        return mTargetList[idx];
    }
    //-----------------------------------------------------------------------
    void FrameShader::removeAllCh()
    {
        TargetList::iterator i, iend = mTargetList.end();
        for(i = mTargetList.begin(); i != iend; ++i)
        {
            N_delete (*i);
        }
        mTargetList.clear();
    }
    //-----------------------------------------------------------------------
    bool FrameShader::isSupport(bool degenerate) const
    {
        // A technique is supported if all materials referenced have a supported
        // technique, and the intermediate texture formats requested are supported
        // Material support is a cast-iron requirement, but if no texture formats
        // are directly supported we can let the rendersystem create the closest
        // match for the least demanding technique


        // Check output target pass is supported
        if (!mOutputCh->isValid())
        {
            return false;
        }

        // Check all target passes is supported
        TargetList::const_iterator pi, piend = mTargetList.end();
        for (pi = mTargetList.begin(); pi != piend; ++pi)
        {
            FusionShader * targetPass = *pi;
            if (!targetPass->isValid())
            {
                return false;
            }
        }

        DefineList::const_iterator i, iend = mDefineList.end();
        TextureManager& texMgr = N_Only(Texture);
        for (i = mDefineList.begin(); i != iend; ++i)
        {
            FrameDefine * td = *i;
            RenderSys * rsys = N_Engine().getRender();
            if(td->mFormatList.size() > rsys->getFeature()->getMaxMultiFrameObj())
            {
                return false;
            }

            for(PixelFormatList::iterator pfi = td->mFormatList.begin(); pfi != td->mFormatList.end(); ++pfi)
            {
                if(degenerate)
                {
                    // Don't care about exact format so NIIl as something is supported
                    if(texMgr.getFormat(Texture::T_2D, *pfi, Texture::MM_FRAME_RGB) == PF_UNKNOWN)
                    {
                        return false;
                    }
                }
                else
                {
                    PixelFormat supportpf = N_Only(Texture).getFormat(Texture::T_2D, *pfi, Texture::MM_FRAME_RGB);

                    // Need a format which is the same number of bits to pass
                    if(PixelUtil::getUnitSize(supportpf) < PixelUtil::getUnitSize(*pfi))
                    {
                        return false;
                    }
                }
            }

            //Check all render targets have same number of bits
            if(!rsys->getFeature()->isSupport(GF_MultiRenderTarget_EachDepthBit) && !td->mFormatList.empty())
            {
                PixelFormat nativeFormat = texMgr.getFormat(Texture::T_2D, td->mFormatList.front(), Texture::MM_FRAME_RGB);
                NCount nativeBits = PixelUtil::getUnitSize( nativeFormat ) * 8;
                for(PixelFormatList::iterator pfi = td->mFormatList.begin()+1;
                    pfi != td->mFormatList.end(); ++pfi)
                {
                    PixelFormat nativeTmp = texMgr.getFormat(Texture::T_2D, *pfi, Texture::MM_FRAME_RGB);
                    if((PixelUtil::getUnitSize(nativeTmp) *8) != nativeBits)
                    {
                        return false;
                    }
                }
            }
        }
        // Must be ok
        return true;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Fusion
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    Fusion::Fusion(ResourceID rid, GroupID gid,
        ResLoadScheme * ls, ResResultScheme * rs, LangID lid):
            Resource(N_CmdObj_Compositor, rid, gid, ls, rs, lid),
            mCompilationRequired(true)
    {
        mCreator = N_OnlyPtr(Fusion);
    }
    //-----------------------------------------------------------------------
    Fusion::~Fusion()
    {
        destroyAll();
        // have to call this here reather than in Resource destructor
        // since calling virtual methods in base destructors causes crash
        unload();
    }
    //-----------------------------------------------------------------------
    FrameShader * Fusion::create()
    {
        FrameShader * temp = N_new FrameShader(this);
        mFusions.push_back(temp);
        mCompilationRequired = true;
        return temp;
    }
    //-----------------------------------------------------------------------
    void Fusion::destroy(Nidx index)
    {
        N_assert(index < mFusions.size(), "Index out of bounds.");
        FrameList::iterator i = mFusions.begin();
        std::advance(i, index);
        N_delete(*i);
        mFusions.erase(i);
        mValidFusions.clear();
        mCompilationRequired = true;
    }
    //-----------------------------------------------------------------------
    FrameShader * Fusion::get(Nidx index) const
    {
        N_assert(index < mFusions.size(), "Index out of bounds.");
        return mFusions[index];
    }
    //-----------------------------------------------------------------------
    void Fusion::destroyAll()
    {
        FrameList::iterator i, iend = mFusions.end();
        for(i = mFusions.begin(); i != iend; ++i)
        {
            N_delete (*i);
        }
        mFusions.clear();
        mValidFusions.clear();
        mCompilationRequired = true;
    }
    //-----------------------------------------------------------------------
    FrameShader * Fusion::getValid(Nidx index) const
    {
        N_assert (index < mValidFusions.size(), "Index out of bounds.");
        return mValidFusions[index];
    }
    //-----------------------------------------------------------------------
    void Fusion::loadImpl()
    {
        if (mCompilationRequired)
        {
            mValidFusions.clear();
            FrameList::iterator i, iend = mFusions.end();
            for (i = mFusions.begin(); i != iend; ++i)
            {
                if ((*i)->isSupport(false))
                {
                    mValidFusions.push_back(*i);
                }
            }

            if (mValidFusions.empty())
            {
                for (i = mFusions.begin(); i != iend; ++i)
                {
                    if ((*i)->isSupport(true))
                    {
                        mValidFusions.push_back(*i);
                    }
                }
            }
            mCompilationRequired = false;
        }
        static NCount dummyCounter = 0;
        if (mValidFusions.empty())
            return;

        //To make sure that we are consistent, it is demanded that all composition
        //techniques define the same set of global textures.

        typedef std::set<String> StringSet;
        StringSet globalTextureNames;

        //Initialize global textures from first supported technique
        FrameShader * firstTechnique = mValidFusions[0];

        const FrameShader::DefineList & texDefIt = firstTechnique->getDefineList();
        FrameShader::DefineList::const_iterator j, jend = texDefIt.end();
        for (j = texDefIt.begin(); j != jend; ++j)
        {
            FrameShader::FrameDefine * def = *j;
            if (def->mSourceType == FrameShader::ST_Pool)
            {
                //Check that this is a legit global texture
                if (!def->mFusionSource)
                {
                    N_EXCEPT(InvalidState, _I18n("Global compositor texture definition can not be a reference"));
                }
                if (def->mWidth == 0 || def->mHeight == 0)
                {
                    N_EXCEPT(InvalidState, _I18n("Global compositor texture definition must have absolute size"));
                }
                if (def->mPool)
                {
                    N_Only(Log).log(_I18n("Pooling global compositor textures has no effect"));
                }
                globalTextureNames.insert(def->mName);

                //TODO GSOC : Heavy copy-pasting from FrameFusion. How to we solve it?

                // Make the tetxure
                FrameObj * rendTarget;
                if (def->mFormatList.size() > 1)
                {
                    String MRTbaseName = _T("c") + N_conv(dummyCounter++) +
                        _T("/") + N_conv(mOriginID) + _T("/") + def->mName;
                    MultiTextureFrame * mrt = N_Engine().getRender()->createMultiFrame(MRTbaseName);

                    mMultiTexFrameList[def->mName] = mrt;

                    // create and bind individual surfaces
                    NCount atch = 0;
                    for (PixelFormatList::iterator p = def->mFormatList.begin();
                        p != def->mFormatList.end(); ++p, ++atch)
                    {
                        //String texname = MRTbaseName + "/" + StrConv::conv(atch);
                        Texture * tex = N_Only(Texture).createTexture(0, GroupInner, Texture::T_2D, *p,
                            def->mWidth, def->mHeight, 1, 0, Texture::MM_FRAME_RGB, 0, 0,
                            def->mHWGamma && !PixelUtil::isFloat(*p), def->mFSAA);

                        TextureFrame * rt = tex->getBuffer()->getBuffer();
                        rt->setAutoFlush(false);
                        mrt->attach(atch, rt);

                        // Also add to local textures so we can look up
                        String mrtLocalName = MultiName(def->mName, atch);
                        mTextureList[mrtLocalName] = tex;
                    }
                    rendTarget = mrt;
                }
                else
                {
                    //String texName =  "c" + StrConv::conv(dummyCounter++) +
                   //     "/" + StrConv::conv(mOriginID) + "/" + def->name;

                    // space in the name mixup the cegui in the compositor demo
                    // this is an auto generated name - so no spaces can't hart us.
                    //std::replace(texName.begin(), texName.end(), ' ', '_' );

                    Texture * tex = N_Only(Texture).createTexture(0, GroupInner, Texture::T_2D,
                        def->mFormatList[0], def->mWidth, def->mHeight, 1, 0, Texture::MM_FRAME_RGB, 0, 0,
                        def->mHWGamma && !PixelUtil::isFloat(def->mFormatList[0]), def->mFSAA);

                    rendTarget = tex->getBuffer()->getBuffer();
                    mTextureList[def->mName] = tex;
                }
                //Set ExtFrameBuffer pool for sharing
                rendTarget->setRasterGroup(def->mRasterGroup);
            }
        }

        //Validate that all other supported techniques expose the same set of global textures.
        for (NCount i = 1; i < mValidFusions.size(); ++i)
        {
            FrameShader * technique = mValidFusions[i];
            bool isConsistent = true;
            NCount numGlobals = 0;
            const FrameShader::DefineList & texDefIt1 = technique->getDefineList();
            FrameShader::DefineList::const_iterator z, zend = texDefIt1.end();
            for (z = texDefIt1.begin(); z != zend; ++z)
            {
                FrameShader::FrameDefine * texDef = *z;
                if (texDef->mSourceType == FrameShader::ST_Pool)
                {
                    if (globalTextureNames.find(texDef->mName) == globalTextureNames.end())
                    {
                        isConsistent = false;
                        break;
                    }
                    ++numGlobals;
                }
            }
            if (numGlobals != globalTextureNames.size())
                isConsistent = false;

            if (!isConsistent)
            {
                N_EXCEPT(InvalidState, _I18n("Different composition techniques define different global textures"));
            }
        }
    }
    //-----------------------------------------------------------------------
    void Fusion::unloadImpl()
    {
        TextureList::iterator i = mTextureList.begin();
        while (i != mTextureList.end())
        {
            N_Only(Texture).remove(i->second->getOriginID());
            i++;
        }
        mTextureList.clear();

        MultiTexFrameList::iterator mrti = mMultiTexFrameList.begin();
        while (mrti != mMultiTexFrameList.end())
        {
            RenderSys * rsys = N_Engine().getRender();
            rsys->destroyFrame(mrti->second);
            ++mrti;
        }
        mMultiTexFrameList.clear();
    }
    //-----------------------------------------------------------------------
    NCount Fusion::calcSize() const
    {
        return 0;
    }
    //---------------------------------------------------------------------
    FrameShader * Fusion::getValid(SchemeID sid) const
    {
        FrameList::const_iterator i, iend = mValidFusions.end();
        for(i = mValidFusions.begin(); i != iend; ++i)
        {
            if((*i)->getSchemeID() == sid)
            {
                return *i;
            }
        }

        if (mValidFusions.size())
        {
            return *mValidFusions.begin();
        }

        return 0;
    }
    //-----------------------------------------------------------------------
    Texture * Fusion::getTexture(const String & name, Nidx multiIdx) const
    {
        TextureList::const_iterator i = mTextureList.find(name);
        if(i != mTextureList.end())
        {
            return i->second;
        }
        String mrtName = MultiName(name, multiIdx);
        i = mTextureList.find(mrtName);
        if(i != mTextureList.end())
        {
            return i->second;
        }

        N_EXCEPT(InvalidParam, _I18n("Non-existent global texture name"));
    }
    //---------------------------------------------------------------------
    FrameObj * Fusion::getFrameObj(const String & name) const
    {
        TextureList::const_iterator i = mTextureList.find(name);
        if(i != mTextureList.end())
            return i->second->getBuffer()->getBuffer();

        MultiTexFrameList::const_iterator mi = mMultiTexFrameList.find(name);
        if (mi != mMultiTexFrameList.end())
            return mi->second;
        //else
        //    N_EXCEPT(InvalidParam, _I18n("Non-existent global texture name"));

        return 0;
    }
    //---------------------------------------------------------------------
}