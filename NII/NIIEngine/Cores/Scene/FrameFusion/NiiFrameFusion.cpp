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
#include "NiiFrameFusion.h"
#include "NiiFusionManager.h"
#include "NiiFusion.h"
#include "NiiGBufferManager.h"
#include "NiiMaterialManager.h"
#include "NiiTextureManager.h"
#include "NiiLogManager.h"
#include "NiiEngine.h"
#include "NiiStrConv.h"
#include "NiiGpuBuffer.h"
#include "NiiMultiTextureFrame.h"
#include "NiiTextureFrame.h"
#include "NiiViewport.h"
#include "NiiFrameObj.h"
#include "NiiSpaceManager.h"


#define MultiName(bname, idx) (bname + _T("/") + N_conv(idx))

namespace NII
{
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RSClearOperation
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    class RSClearOperation : public FusionRender
    {
    public:
        RSClearOperation(Nui32 buffers, Colour colour, NIIf depth, Nui16 stencil) :
            mBufferMark(buffers),
            mColour(colour),
            mDepth(depth),
            mStencil(stencil) {}

        virtual void execute(SpaceManager * sm, RenderSys * rsys)
        {
            rsys->clearBuffer(mBufferMark, mColour, mDepth, mStencil);
        }

        // Which buffers to clear (FaceType)
        Nui32 mBufferMark;
        // Colour to clear in case FBT_COLOUR is set
        Colour mColour;
        // Depth to set in case FBT_DEPTH is set
        NIIf mDepth;
        // Stencil value to set in case FBT_STENCIL is set
        Nui16 mStencil;
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RSStencilOperation
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    class RSStencilOperation : public FusionRender
    {
    public:
        RSStencilOperation(bool stencilCheck, CmpMode func, Nui32 refValue, Nui32 mask,
            StencilOpType stencilFailOp, StencilOpType depthFailOp, StencilOpType passOp,
            bool twoSidedOperation) :
            mEnable(stencilCheck), mStencil(twoSidedOperation, mask, mask, func, refValue, 
                stencilFailOp, depthFailOp, passOp){}

        virtual void execute(SpaceManager * sm, RenderSys *rsys)
        {
            rsys->setStencilTest(mEnable);
            rsys->setStencil(mStencil);
        }

        bool mEnable;
        ShaderChStencil mStencil;
        //CmpMode mFunc;
        //Nui32 mRefValue;
        //Nui32 mMask;
        //StencilOpType mStencilFailOp;
        //StencilOpType mDepthFailOp;
        //StencilOpType mPassOp;
        //bool mTwoSide;
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RectFusionRender
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    class RectFusionRender : public FusionRender
    {
    public:
        RectFusionRender(FrameFusion * fusion, Nui32 shaderopid, Material * mat) :
            mMaterial(mat),
            mFusion(fusion),
            mOpId(shaderopid),
            mQuadCorner(false),
            mQuadFarCorners(false),
            mQuadFarCornersViewSpace(false),
            mQuadLeft(-1),
            mQuadTop(1),
            mQuadRight(1),
            mQuadBottom(-1)
        {
            mMaterial->load();

            FrameFusion::Listeners::iterator i, iend = mFusion->mListeners.end();
            for (i = mFusion->mListeners.begin(); i != iend; ++i)
            {
                (*i)->notifyMaterialSetup(shaderopid, mMaterial);
            }
            mShaderFusion = mMaterial->get(0);
            N_assert(mShaderFusion, "error");
        }

        void setQuadCorners(NIIf left, NIIf top, NIIf right, NIIf bottom)
        {
            mQuadLeft = left;
            mQuadTop = top;
            mQuadRight = right;
            mQuadBottom = bottom;
            mQuadCorner = true;
        }

        void setQuadFarCorners(bool farCorners, bool farCornersViewSpace)
        {
            mQuadFarCorners = farCorners;
            mQuadFarCornersViewSpace = farCornersViewSpace;
        }

        virtual void execute(SpaceManager * sm, RenderSys * rsys)
        {
            FrameFusion::Listeners::iterator l, lend = mFusion->mListeners.end();
            for (l = mFusion->mListeners.begin(); l != lend; ++l)
            {
                (*l)->notifyMaterialRender(mOpId, mMaterial);
            }

            Viewport * vp = rsys->getViewports();
            RectGeo * rect = static_cast<RectGeo *>(N_Only(Fusion).getArea());

            if (mQuadCorner)
            {
                // insure positions are using peculiar render system offsets
                NIIf hOffset = rsys->getTexelXOffset() / (0.5f * vp->getPixelWidth());
                NIIf vOffset = rsys->getTexelYOffset() / (0.5f * vp->getPixelHeight());
                rect->setRect(mQuadLeft + hOffset, mQuadTop - vOffset, mQuadRight + hOffset, mQuadBottom - vOffset);
            }

            if (mQuadFarCorners)
            {
                const Vector3f * corners = vp->getCamera()->getClipPlaneIntersect();
                if (mQuadFarCornersViewSpace)
                {
                    const Matrix4f &viewMat = vp->getCamera()->getViewMatrix(false);
                    rect->setNormals(viewMat * corners[5], viewMat * corners[6], viewMat*corners[4], viewMat*corners[7]);
                }
                else
                {
                    rect->setNormals(corners[5], corners[6], corners[4], corners[7]);
                }
            }

            const ShaderCh * ch;
            ShaderChList::iterator i, iend = mShaderFusion->getShaderChList().end();
            for (i = mShaderFusion->getShaderChList().begin(); i != iend; ++i)
            {
                ch = sm->applyCh(*i, false, false);
                sm->render(rect, ch, LightList(), false);
            }
        }

        Material * mMaterial;
        ShaderFusion * mShaderFusion;
        FrameFusion * mFusion;
        Nui32 mOpId;

        bool mQuadCorner, mQuadFarCorners, mQuadFarCornersViewSpace;
        NIIf mQuadLeft;
        NIIf mQuadTop;
        NIIf mQuadRight;
        NIIf mQuadBottom;
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RSSetSchemeOperation
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    class RSSetSchemeOperation : public FusionRender
    {
    public:
        RSSetSchemeOperation(SchemeID sid) :
            mPreviousOpt(false),
            mSchemeID(sid) {}

        virtual void execute(SpaceManager * sm, RenderSys * rsys)
        {
            mPreviousScheme = N_Only(Material).getCurrent();
            N_Only(Material).setCurrent(mSchemeID);

            mPreviousOpt = sm->isOptMaterial();
            sm->setOptMaterial(true);
        }

        SchemeID getPreviousScheme() const { return mPreviousScheme; }
        bool getPreviousLateResolving() const { return mPreviousOpt; }

        SchemeID mSchemeID;
        SchemeID mPreviousScheme;
        bool mPreviousOpt;
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // RSRestoreSchemeOperation
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    class RSRestoreSchemeOperation : public FusionRender
    {
    public:
        RSRestoreSchemeOperation(const RSSetSchemeOperation * setOperation) :
            mSetOperation(setOperation) {}

        virtual void execute(SpaceManager * sm, RenderSys * rsys)
        {
            N_Only(Material).setCurrent(mSetOperation->getPreviousScheme());
            sm->setOptMaterial(mSetOperation->getPreviousLateResolving());
        }

        const RSSetSchemeOperation * mSetOperation;
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // FrameObjOperation
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    typedef vector<std::pair<NIIi, FusionRender *> >::type FrameFusionRenderList;
    class FrameObjOperation
    {
    public:
        FrameObjOperation() {}
        FrameObjOperation(FrameObj * fb) :
            mFrameObj(fb),
            mQueueGroup(0),
            mVisibleMask(0xFFFFFFFF),
            mLodBias(1.0f),
            mMaterialScheme(0),
            mOnce(false),
            mBeenRender(false),
            mAutoVisible(false),
            mShadowEnable(true) {}
        typedef std::bitset<RENDER_QUEUE_COUNT> RenderQueueBitSet;
    public:
        FrameObj * mFrameObj;
        NIIi mQueueGroup;
        FrameFusionRenderList RenderOpList;
        Nui32 mVisibleMask;
        NIIf mLodBias;
        RenderQueueBitSet mRenderQueueBit;
        SchemeID mMaterialScheme;
        bool mOnce;
        bool mBeenRender;
        bool mAutoVisible;
        bool mShadowEnable;
    };
    typedef vector<FrameObjOperation>::type FrameObjOperationList;
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ResultRenderListener
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    class _EngineInner ResultRenderListener : public RenderListener, public FrameAlloc
    {
    public:
        /// @copydetails RenderListener::renderBegin
        virtual bool renderBegin(GroupID render);

        /// @copydetails RenderListener::renderEnd
        virtual bool renderEnd(GroupID render);

        /// 设置当前操作和目标
        void setOperation(FrameObjOperation * op, SpaceManager * sm, RenderSys *rs);

        /// 通告当前目的视口
        void notifyViewport(Viewport * vp) { mViewport = vp; }

        /// 冲洗剩余的渲染系统操作
        void flushUpTo(Nui8 id);
    private:
        FrameObjOperation * mOperation;
        SpaceManager * mSceneManager;
        RenderSys * mRenderSys;
        Viewport * mViewport;
        FrameFusionRenderList::iterator mCurrent;
        FrameFusionRenderList::iterator mLast;
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // FrameResultRestore
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    class _EngineInner FrameResultRestore : public FrameAlloc
    {
    public:
        class FusionChain : public FrameAlloc
        {
        public:
            FusionChain(FrameFusion * fusion) :
                mFusion(fusion),
                mPreFusion(0){}
            ~FusionChain()
            {
                if (mPreFusion)
                {
                    delete mPreFusion;
                    mPreFusion = 0;
                }
            }

            inline FusionChain * next(FrameFusion * fusion)
            {
                N_assert(fusion, "error");
                FusionChain * re = new FusionChain(fusion);
                re->mPreFusion = this;
                return re;
            }

            inline FrameFusion * getFusion() const { return mFusion; }
            inline FusionChain * getPre() const { return mPreFusion; }
            inline FrameFusion * getPreFusion() const
            { 
                N_assert(mPreFusion, "error");
                return mPreFusion->mFusion;
            }
        public:
            FrameFusion * mFusion;
            FusionChain * mPreFusion;
        };
    public:
        FrameResultRestore(Viewport * vp, FusionInstance * res)
        {
            mResult = res;
            mRenderListener = N_new ResultRenderListener();
            mRestoreClearMark = vp->getClearMark();
        }
        ~FrameResultRestore()
        {
            N_delete mRenderListener;
        }

        void _compileTargetOperations(FusionChain * chain);
        void _compileOutputOperation(FrameObjOperation & op, FusionChain * chain);

        void collectPasses(FrameObjOperation & op, FusionShader * target, FrameFusion * fusion);

        void beginRender(FrameObjOperation & op, Viewport * vp, Camera * cam)
        {
            if (cam)
            {
                SpaceManager * sm = cam->getManager();
                mRenderListener->setOperation(&op, sm, sm->getRender());
                mRenderListener->notifyViewport(vp);

                // Register it
                sm->add(mRenderListener);
                // Set visiblity mask
                mRestoreViableMark = sm->getVisibleMask();
                sm->setVisibleMask(op.mVisibleMask);
                // Set whether we find visibles
                mRestoreAutoVisible = sm->isAutoVisible();
                sm->setAutoVisible(op.mAutoVisible);
                // Set LOD bias level
                mRestoreLodBias = cam->getLodBias();
                cam->setLodBias(cam->getLodBias() * op.mLodBias);
            }
            // Set material scheme
            mRestoreSchemeID = vp->getMaterial();
            vp->setMaterial(op.mMaterialScheme);
            // Set shadows enabled
            mRestoreShadowEnable = vp->isShadowEnable();
            vp->setShadowEnable(op.mShadowEnable);
            // XXX TODO
            //vp->setAutoClear(true);
            //vp->setOverlayEnable(false);
            //vp->setBgColour(op.clearColour);
        }

        void endRender(FrameObjOperation & op, Viewport * vp, Camera * cam)
        {
            if (cam)
            {
                SpaceManager * sm = cam->getManager();
                sm->remove(mRenderListener);
                /// Restore default scene and camera settings
                sm->setVisibleMask(mRestoreViableMark);
                sm->setAutoVisible(mRestoreAutoVisible);
                cam->setLodBias(mRestoreLodBias);
            }
            vp->setMaterial(mRestoreSchemeID);
            vp->setShadowEnable(mRestoreShadowEnable);
        }
    public:
        FrameObjOperationList mState;
        FrameObjOperation mOperation;
        FusionInstance * mResult;
        ResultRenderListener * mRenderListener;
        Nui32 mRestoreClearMark;
        Nui32 mRestoreViableMark;
        NIIf mRestoreLodBias;
        SchemeID mRestoreSchemeID;
        bool mRestoreShadowEnable;
        bool mRestoreAutoVisible;
    };
    //---------------------------------------------------------------------
    void FrameResultRestore::collectPasses(FrameObjOperation & finalState, FusionShader * target, FrameFusion * fusion)
    {
        ShaderCh * targetpass;
        ShaderFusion * srctech;
        Material * srcmat;

        FusionShader::OpList::const_iterator cp, cpend = target->getList().end();
        for (cp = target->getList().begin(); cp != cpend; ++cp)
        {
            FusionShaderOp * pass = *cp;
            switch (pass->getType())
            {
            case FusionShaderOp::OT_Clear:
            {
                FusionRender * op = N_new RSClearOperation(pass->getClearBuffers(),
                    pass->getClearColour(), pass->getClearDepth(), (Nui16)pass->getClearStencil());
                finalState.RenderOpList.push_back(Npair(finalState.mQueueGroup, op));
                mResult->setupRender(op);
            }
            break;
            case FusionShaderOp::OT_StencilTest:
            {
                FusionRender * op = N_new RSStencilOperation(pass->getStencilCheck(), 
                    pass->getStencilFunc(), pass->getStencilValue(), pass->getStencilMask(), 
                    pass->getStencilFailOp(), pass->getStencilDepthFailOp(),
                    pass->getStencilPassOp(), pass->getStencilTwoSided());
                finalState.RenderOpList.push_back(Npair(finalState.mQueueGroup, op));
                mResult->setupRender(op);
            }
            break;
            case FusionShaderOp::OT_RenderScene:
            {
                if (pass->getFirstRenderQueue() < finalState.mQueueGroup)
                {
                    // Mismatch -- warn user
                    // XXX We could support repeating the last queue, with some effort
                    N_Only(Log).log(_I18n("Warning in compilation of Fusion ") +
                        N_conv(fusion->getFusion()->getOriginID()) + _I18n(": Attempt to render queue ") +
                        N_conv(pass->getFirstRenderQueue()) + _I18n(" before ") +
                        N_conv(finalState.mQueueGroup));
                }

                RSSetSchemeOperation * setSchemeOperation = 0;
                if (pass->getMaterial() != 0)
                {
                    //Add the triggers that will set the scheme and restore it each frame
                    finalState.mQueueGroup = pass->getFirstRenderQueue();
                    setSchemeOperation = N_new RSSetSchemeOperation(pass->getMaterial());
                    finalState.RenderOpList.push_back(Npair(finalState.mQueueGroup, setSchemeOperation));
                    mResult->setupRender(setSchemeOperation);
                }

                // Add render queues
                for (NIIi x = pass->getFirstRenderQueue(); x <= pass->getLastRenderQueue(); ++x)
                {
                    N_assert(x >= 0, "error");
                    finalState.mRenderQueueBit.set(x);
                }
                finalState.mQueueGroup = pass->getLastRenderQueue() + 1;

                if (setSchemeOperation != 0)
                {
                    //Restoring the scheme after the queues have been rendered
                    FusionRender * op = N_new RSRestoreSchemeOperation(setSchemeOperation);
                    finalState.RenderOpList.push_back(Npair(finalState.mQueueGroup, op));
                    mResult->setupRender(op);
                }

                finalState.mAutoVisible = true;
            }
            break;
            case FusionShaderOp::OT_RenderQuad:
            {
                srcmat = pass->getMaterial();
                if (srcmat == 0)
                {
                    // No material -- warn user
                    N_Only(Log).log(_I18n("Warning in compilation of Fusion ") +
                        N_conv(fusion->getFusion()->getOriginID()) +
                        _I18n(": No material defined for composition pass"));
                    break;
                }
                srcmat->load();
                if (srcmat->getValidCount() == 0)
                {
                    // No supported techniques -- warn user
                    N_Only(Log).log(_I18n("Warning in compilation of Fusion ") +
                        N_conv(fusion->getFusion()->getOriginID()) + _I18n(": material ") +
                        N_conv(srcmat->getOriginID()) + _I18n(" has no supported techniques"));
                    break;
                }
                srctech = srcmat->getLOD(0);
                //Material * mat = N_Only(Material).create(
                //    "c" + StrConv::conv(dummyCounter) + "/" + srcmat->getOriginID(), GroupInner);
                Material * mat = static_cast<Material *>(N_Only(Material).create(0, GroupInner));
                mat->get(0)->removeAll();
                // Copy and adapt passes from source material
                ShaderChList::iterator i, iend = srctech->getShaderChList().end();
                for (i = srctech->getShaderChList().begin(); i != iend; ++i)
                {
                    ShaderCh * srcpass = *i;
                    // Create new target pass
                    targetpass = mat->get(0)->create();
                    (*targetpass) = (*srcpass);
                    // Set up inputs
                    for (Nidx x = 0; x < pass->getInputCount(); ++x)
                    {
                        const FusionShaderOp::OpUnit & inp = pass->getInput(x);
                        if (!inp.first.empty())
                        {
                            if (x < targetpass->getTexture().getCount())
                            {
                                ResourceIDList rlist;
                                rlist.push_back(fusion->getTextureID(inp.first, inp.second));
                                targetpass->getTexture().get(x)->setSrc(rlist, Texture::T_2D);
                            }
                            else
                            {
                                // Texture unit not there
                                N_Only(Log).log(_I18n("Warning in compilation of Fusion ") +
                                    N_conv(fusion->getFusion()->getOriginID()) + _I18n(": material ") +
                                    N_conv(srcmat->getOriginID()) + _I18n(" texture unit ") +
                                    N_conv(x) + _I18n(" out of bounds"));
                            }
                        }
                    }
                }

                RectFusionRender * rsQuadOperation = N_new RectFusionRender(fusion, pass->getID(), mat);
                NIIf left, top, right, bottom;

                if (pass->getQuadCorners(left, top, right, bottom))
                    rsQuadOperation->setQuadCorners(left, top, right, bottom);

                rsQuadOperation->setQuadFarCorners(pass->getQuadFarCorners(), pass->getQuadFarCornersViewSpace());
                finalState.RenderOpList.push_back(Npair(finalState.mQueueGroup, rsQuadOperation));
                mResult->setupRender(rsQuadOperation);
            }
            break;
            case FusionShaderOp::OT_RenderCustom:
            {
                FusionRender * cOP = N_Only(Fusion).getRender(pass->getCustomType())->createInstance(fusion, pass);
                finalState.RenderOpList.push_back(Npair(finalState.mQueueGroup, cOP));
                mResult->setupRender(cOP);
            }
            break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void FrameResultRestore::_compileTargetOperations(FusionChain * chain)
    {
        if (chain->getPre())
            _compileTargetOperations(chain->getPre());

        const FrameShader::TargetList & it = chain->getFusion()->getShader()->getTargetList();
        FrameShader::TargetList::const_iterator i, iend = it.end();
        for (i = it.begin(); i != iend; ++i)
        {
            FusionShader * target = *i;
            FrameObjOperation ts(chain->getFusion()->getFrameObj(target->getOutputName()));
            ts.mOnce = target->isOnce();
            ts.mVisibleMask = target->getVisibleMask();
            ts.mLodBias = target->getLodBias();
            ts.mShadowEnable = target->isShadowEnable();
            ts.mMaterialScheme = target->getMaterial();

            if (target->isUsePreResult())
            {
                // Collect target state for previous compositor
                // The FrameObjOperation for the final target is collected seperately as it is merged
                // with later operations
                _compileOutputOperation(ts, chain->getPre());
            }
            // Collect passes of our own target
            collectPasses(ts, target, chain->getFusion());
            mState.push_back(ts);
        }
    }
    //-----------------------------------------------------------------------
    void FrameResultRestore::_compileOutputOperation(FrameObjOperation & finalState, FusionChain * chain)
    {
        // Final target
        FusionShader * outch = chain->getFusion()->getShader()->getOutputCh();

        // Logical-and together the mVisibleMask, and multiply the mLodBias
        finalState.mVisibleMask &= outch->getVisibleMask();
        finalState.mLodBias *= outch->getLodBias();
        finalState.mMaterialScheme = outch->getMaterial();
        finalState.mShadowEnable = outch->isShadowEnable();

        if (outch->isUsePreResult())
        {
            // Collect target state for previous compositor
            // The FrameObjOperation for the final target is collected seperately as it is merged
            // with later operations
            _compileOutputOperation(finalState, chain->getPre());
        }

        collectPasses(finalState, outch, chain->getFusion());
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // FusionInstance
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    FusionInstance::FusionInstance(Viewport * vp):
        mViewport(vp),
        mOriginal(0),
        mResourceID(0),
        mRefesh(true),
        mEnable(false)
    {
        N_assert(vp, "error");
        mFrameResultRestore = N_new FrameResultRestore(vp, this);

        vp->add(this);
        createResource();
        vp->getBuffer()->add(this);
    }
    //-----------------------------------------------------------------------
    FusionInstance::~FusionInstance()
    {
        N_delete mFrameResultRestore;

        destroyRender();

        if (mViewport)
        {
            mViewport->getBuffer()->remove(this);
            mViewport->remove(this);
            destroyAll();
            destroyResource();
            mViewport = 0;
        }
    }
    //-----------------------------------------------------------------------
    void FusionInstance::createResource()
    {
        // Create "default" compositor
        /* Fusion that is used to implicitly represent the original
            render in the chain. This is an identity compositor with only an output pass:
            compositor Nii/Scene
            {
                technique
                {
                    target_output
                    {
                        pass clear
                        {
                            /// Clear frame
                        }
                        pass render_scene
                        {
                            visibility_mask FFFFFFFF
                            render_queues SKIES_EARLY SKIES_LATE
                        }
                    }
                }
            };
        */

        mSchemeID = mViewport->getMaterial();
        //String compName = "Nii/Scene/" + mSchemeID;
        Fusion * scene = 0;
        if(mResourceID)
            scene = static_cast<Fusion *>(N_Only(Fusion).get(mResourceID, GroupInner));
        if(scene == 0)
        {
            scene = static_cast<Fusion *>(N_Only(Fusion).create(0, GroupInner));
            mResourceID = scene->getOriginID();
            FrameShader * t = scene->create();
            t->setSchemeID(0);
            FusionShader * tp = t->getOutputCh();
            tp->setVisibleMask(0xFFFFFFFF);
            {
                FusionShaderOp * pass = tp->create();
                pass->setType(FusionShaderOp::OT_Clear);
            }
            {
                FusionShaderOp * pass = tp->create();
                pass->setType(FusionShaderOp::OT_RenderScene);
                /// Render everything, including skies
                pass->setFirstRenderQueue(RQG_Bg);
                pass->setLastRenderQueue(RQG_NearEnd);
            }

            // Create base "original scene" compositor
            scene = static_cast<Fusion *>(N_Only(Fusion).load(mResourceID, GroupInner));
        }
        mOriginal = N_new FrameFusion(scene->getValid(), this);
    }
    //-----------------------------------------------------------------------
    void FusionInstance::destroyResource()
    {
        if (mOriginal)
        {
            mResourceID = 0;
            N_delete mOriginal;
            mOriginal = 0;
        }
    }
    //-----------------------------------------------------------------------
    FrameFusion * FusionInstance::create(Fusion * filter, Nidx pos, SchemeID scheme)
    {
        filter->ref();
        FrameShader * tech = filter->getValid(scheme);
        if(!tech)
        {
            // Warn user
            N_Only(Log).log(_I18n("FusionInstance: Fusion ") + N_conv(filter->getOriginID()) +
                _I18n(" has no supported techniques."), LL_Fatal);
            return 0;
        }
        FrameFusion * re = N_new FrameFusion(tech, this);

        if(pos == -1)
            pos = mFusionList.size();
        else
            N_assert(pos <= mFusionList.size(), "Index out of bounds.");
        mFusionList.insert(mFusionList.begin() + pos, re);

        mRefesh = true;
        mEnable = true;
        return re;
    }
    //-----------------------------------------------------------------------
    void FusionInstance::destroy(Nidx pos)
    {
        N_assert (pos < mFusionList.size(), "Index out of bounds.");
        FusionList::iterator i = mFusionList.begin() + pos;
        N_delete *i;
        mFusionList.erase(i);

        mRefesh = true;
    }
    //-----------------------------------------------------------------------
    void FusionInstance::destroy(FrameFusion * i)
    {
        mFusionList.erase(std::find(mFusionList.begin(), mFusionList.end(), i));
        N_delete i;
        mRefesh = true;
    }
    //-----------------------------------------------------------------------
    void FusionInstance::destroyAll()
    {
        FusionList::iterator i, iend = mFusionList.end();
        for(i = mFusionList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mFusionList.clear();

        mRefesh = true;
    }
    //-----------------------------------------------------------------------
    void FusionInstance::setupRender(FusionRender * op)
    {
        mRenderList.push_back(op);
    }
    //-----------------------------------------------------------------------
    void FusionInstance::destroyRender()
    {
        RenderList::iterator i, iend = mRenderList.end();
        for (i = mRenderList.begin(); i != iend; ++i)
        {
            N_delete *i;
        }
        mRenderList.clear();

        mFrameResultRestore->mState.clear();
        mFrameResultRestore->mOperation = FrameObjOperation(0);
    }
    //-----------------------------------------------------------------------
    FrameFusion * FusionInstance::get(Nidx index)
    {
        N_assert(index < mFusionList.size(), "Index out of bounds.");
        return mFusionList[index];
    }
    //-----------------------------------------------------------------------
    FrameFusion * FusionInstance::get(ResourceID rid)
    {
        for(FusionList::iterator it = mFusionList.begin(); it != mFusionList.end(); ++it)
        {
            if((*it)->getFusion()->getOriginID() == rid)
            {
                return *it;
            }
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    void FusionInstance::setEnable(Nidx position, bool state)
    {
        FrameFusion * inst = get(position);
        if(!state && inst->isEnable())
        {
            // If we're disabling a 'middle' compositor in a chain, we have to be
            // careful about textures which might have been shared by non-adjacent
            // instances which have now become adjacent.
            FrameFusion * nextInstance = getNext(inst, true);
            if (nextInstance)
            {
                const FrameShader::TargetList & tpit = nextInstance->getShader()->getTargetList();
                FrameShader::TargetList::const_iterator i, iend = tpit.end();
                for(i = tpit.begin(); i != iend; ++i)
                {
                    FusionShader * tp = *i;
                    if (tp->isUsePreResult())
                    {
                        if (nextInstance->getShader()->getDefine(tp->getOutputName())->mPool)
                        {
                            // recreate
                            nextInstance->destroyTexture(false, true);
                            nextInstance->createTexture(false);
                        }
                    }

                }
            }
        }
        inst->setEnable(state);
    }
    //-----------------------------------------------------------------------
    void FusionInstance::onPreFlush(const FrameObjArgs * evt)
    {
        // Compile if state is dirty
        if(mRefesh)
            build();

        // Do nothing if no compositors enabled
        if (!mEnable)
        {
            return;
        }

        // Update dependent render targets; this is done in the preRenderTarget
        // and not the onPreViewFlush for a reason: at this time, the
        // target Rendertarget will not yet have been set as current.
        // ( RenderSys::setViewport(...) ) if it would have been, the rendering
        // order would be screwed up and problems would arise with copying rendertextures.
        Camera * cam = mViewport->getCamera();
        if(cam)
        {
            cam->getManager()->setFrameResult(this);
        }

        FrameObjOperationList::iterator i, iend = mFrameResultRestore->mState.end();
        for(i = mFrameResultRestore->mState.begin(); i != iend; ++i)
        {
            if(i->mOnce && i->mBeenRender)
                continue;
            i->mBeenRender = true;
            mFrameResultRestore->beginRender(*i, i->mFrameObj->getViewport(0), cam);
            i->mFrameObj->flush();
            mFrameResultRestore->endRender(*i, i->mFrameObj->getViewport(0), cam);
        }
    }
    //-----------------------------------------------------------------------
    void FusionInstance::onEndFlush(const FrameObjArgs * evt)
    {
        Camera * cam = mViewport->getCamera();
        if(cam)
        {
            cam->getManager()->setFrameResult(0);
        }
    }
    //-----------------------------------------------------------------------
    void FusionInstance::onPreViewFlush(const FrameViewArgs * evt)
    {
        // Only set up if there is at least one compositor enabled, and it's this viewport
        if(evt->mView != mViewport || !mEnable)
            return;

        // set original scene details from viewport
        FusionShaderOp * pass = mOriginal->getShader()->getOutputCh()->get(0);
        FusionShader * passParent = pass->getCh();
        if(pass->getClearBuffers() != mViewport->getClearMark() ||
            pass->getClearColour() != mViewport->getBgColour() ||
            pass->getClearDepth() != mViewport->getDepthClear() ||
            passParent->getVisibleMask() != mViewport->getVisableMark() ||
            passParent->getMaterial() != mViewport->getMaterial() ||
            passParent->isShadowEnable() != mViewport->isShadowEnable())
        {
            // recompile if viewport settings are different
            pass->setClearBuffers(mViewport->getClearMark());
            pass->setClearColour(mViewport->getBgColour());
            pass->setClearDepth(mViewport->getDepthClear());
            passParent->setVisibleMask(mViewport->getVisableMark());
            passParent->setMaterial(mViewport->getMaterial());
            passParent->setShadowEnable(mViewport->isShadowEnable());
            build();
        }

        Camera * cam = mViewport->getCamera();
        if(cam)
        {
            // Prepare for output operation
            mFrameResultRestore->beginRender(mFrameResultRestore->mOperation, mViewport, cam);
        }
    }
    //-----------------------------------------------------------------------
    void FusionInstance::onEndViewFlush(const FrameViewArgs * evt)
    {
        // Only tidy up if there is at least one compositor enabled, and it's this viewport
        if(evt->mView != mViewport || !mEnable)
            return;

        Camera * cam = mViewport->getCamera();
        if (cam)
        {
            mFrameResultRestore->endRender(mFrameResultRestore->mOperation, mViewport, cam);
        }
    }
    //-----------------------------------------------------------------------
    /*void FusionInstance::onCameraChange(Viewport * obj)
    {
        Camera * camera = obj->getCamera();
        NCount count = mFusionList.size();
        for(NCount i = 0; i < count; ++i)
        {
            mFusionList[i]->onCameraChange(camera);
        }
    }*/
    //-----------------------------------------------------------------------
    void FusionInstance::onSize(Viewport * obj)
    {
        NCount count = mFusionList.size();
        for(NCount i = 0; i < count; ++i)
        {
            mFusionList[i]->destroyTexture(true, true);
            mFusionList[i]->createTexture(true);
        }
    }
    //-----------------------------------------------------------------------
    void FusionInstance::onDestroy(Viewport * obj)
    {
        // this chain is now orphaned. tell compositor manager to delete it.
        N_Only(Fusion).removeInstance(obj);
    }
    //-----------------------------------------------------------------------
    void FusionInstance::build()
    {
        // remove original scene if it has the wrong material scheme
        if(mSchemeID != mViewport->getMaterial())
        {
            destroyResource();
            createResource();
        }
        destroyRender();

        bool fenable = false;

        // force default scheme so materials for compositor quads will determined correctly
        MaterialManager & matMgr = N_Only(Material);
        SchemeID prevMaterialScheme = matMgr.getCurrent();
        matMgr.setCurrent(0);

        // Set previous FrameFusion for each compositor in the list
        //mOriginal->mPreFusion = 0;
        FusionShaderOp * pass = mOriginal->getShader()->getOutputCh()->get(0);
        pass->setClearBuffers(mViewport->getClearMark());
        pass->setClearColour(mViewport->getBgColour());
        pass->setClearDepth(mViewport->getDepthClear());
        FrameResultRestore::FusionChain chain(mOriginal);
        FrameResultRestore::FusionChain * temp = &chain;
        //FrameFusion * lastComposition = mOriginal;
        for(FusionList::iterator i = mFusionList.begin(); i != mFusionList.end(); ++i)
        {
            if((*i)->isEnable())
            {
                fenable = true;
                temp = temp->next(*i);
                //(*i)->mPreFusion = lastComposition;
                //lastComposition = (*i);
            }
        }

        // Compile misc targets
        mFrameResultRestore->_compileTargetOperations(temp);

        // Final target viewport (0)
        mFrameResultRestore->mOperation.RenderOpList.clear();
        mFrameResultRestore->_compileOutputOperation(mFrameResultRestore->mOperation, temp);

        // Deal with viewport settings
        if(fenable != mEnable)
        {
            mEnable = fenable;
            if(mEnable)
            {
                // Save old viewport clearing options
                mFrameResultRestore->mRestoreClearMark = mViewport->getClearMark();
                // Don't clear anything every frame since we have our own clear ops
                mViewport->setAutoClear(false);
            }
            else
            {
                // Reset clearing options
                mViewport->setAutoClear(mFrameResultRestore->mRestoreClearMark > 0, mFrameResultRestore->mRestoreClearMark);
            }
        }

        // restore material scheme
        matMgr.setCurrent(prevMaterialScheme);

        mRefesh = false;
    }
    //-----------------------------------------------------------------------
    void FusionInstance::refresh()
    {
        mRefesh = true;
    }
    //-----------------------------------------------------------------------
    bool ResultRenderListener::renderBegin(GroupID render)
    {
        // Skip when not matching viewport
        // shadows update is nested within main viewport update
        if(mSceneManager->getViewport() != mViewport)
            return true;

        flushUpTo(render);
        // If no one wants to render this queue, skip it
        // Don't skip the OVERLAY queue because that's handled separately
        if(!mOperation->mRenderQueueBit.test(render) && render != RQG_Overlay)
        {
            return false;
        }
        return true;
    }
    //-----------------------------------------------------------------------
    bool ResultRenderListener::renderEnd(GroupID render)
    {
        return false;
    }
    //-----------------------------------------------------------------------
    void ResultRenderListener::setOperation(FrameObjOperation * op, SpaceManager * sm, RenderSys * rs)
    {
        mOperation = op;
        mSceneManager = sm;
        mRenderSys = rs;
        mCurrent = op->RenderOpList.begin();
        mLast = op->RenderOpList.end();
    }
    //-----------------------------------------------------------------------
    void ResultRenderListener::flushUpTo(Nui8 id)
    {
        // Process all RenderList up to and including render queue id.
        // Including, because the operations for RenderGroup x should be executed
        // at the beginning of the RenderGroup render for x.
        while(mCurrent != mLast && mCurrent->first <= id)
        {
            mCurrent->second->execute(mSceneManager, mRenderSys);
            ++mCurrent;
        }
    }
    //-----------------------------------------------------------------------
    FrameFusion * FusionInstance::getPre(FrameFusion * curr, bool activeOnly)
    {
        bool found = false;
        for(FusionList::reverse_iterator i = mFusionList.rbegin(); i != mFusionList.rend(); ++i)
        {
            if(found)
            {
                if((*i)->isEnable() || !activeOnly)
                    return *i;
            }
            else if(*i == curr)
            {
                found = true;
            }
        }

        return 0;
    }
    //---------------------------------------------------------------------
    FrameFusion* FusionInstance::getNext(FrameFusion * curr, bool activeOnly)
    {
        bool found = false;
        for(FusionList::iterator i = mFusionList.begin(); i != mFusionList.end(); ++i)
        {
            if(found)
            {
                if((*i)->isEnable() || !activeOnly)
                    return *i;
            }
            else if(*i == curr)
            {
                found = true;
            }
        }

        return 0;
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // FrameFusion
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    FrameFusion::FrameFusion(FrameShader * fs, FusionInstance * result) :
        mFusionShader(fs),
        mFusion(fs->getParent()),
        mResult(result),
        mEnabled(false),
        mAlive(false)
    {
        const String & logicName = mFusionShader->getName();
        if (!logicName.empty())
        {
            N_Only(Fusion).getListener(logicName)->onCreate(this);
        }
    }
    //-----------------------------------------------------------------------
    FrameFusion::~FrameFusion()
    {
        const String & logicName = mFusionShader->getName();
        if (!logicName.empty())
        {
            N_Only(Fusion).getListener(logicName)->onDestroy(this);
        }

        destroyTexture(false, true);
    }
    //-----------------------------------------------------------------------
    void FrameFusion::setEnable(bool set)
    {
        if (mEnabled != set)
        {
            mEnabled = set;

            if (mEnabled && !mAlive)
                setAlive(true);

            mResult->refresh();
        }
    }
    //-----------------------------------------------------------------------
    void FrameFusion::setAlive(bool set)
    {
        if (mAlive != set)
        {
            mAlive = set;
            if (set)
            {
                createTexture(false);
            }
            else
            {
                destroyTexture(false, true);
                setEnable(false);
            }

            mResult->refresh();
        }
    }
    //-----------------------------------------------------------------------
    void FrameFusion::setShader(FrameShader * shader, bool reuseTextures)
    {
        if (mFusionShader != shader)
        {
            if (reuseTextures)
            {
                const FrameShader::DefineList & it = mFusionShader->getDefineList();
                FrameShader::DefineList::const_iterator z, zend = it.end();
                for (z = it.begin(); z != zend; ++z)
                {
                    FrameShader::FrameDefine *def = *z;
                    if (def->mPool)
                    {
                        TextureList::iterator i = mTextureList.find(def->mName);
                        if (i != mTextureList.end())
                        {
                            mRefTextureList[def] = i->second;
                        }

                    }
                }
            }
            mFusionShader = shader;

            if(mAlive)
            {
                // free up resources, but keep reserves if reusing
                destroyTexture(false, !reuseTextures);
                createTexture(false);
                // Notify chain state needs recompile.
                mResult->refresh();
            }

        }
    }
    //---------------------------------------------------------------------
    void FrameFusion::setScheme(SchemeID sid, bool reuseTextures)
    {
        FrameShader * shader = mFusion->getValid(sid);
        if(shader)
        {
            setShader(shader, reuseTextures);
        }
    }
    //-----------------------------------------------------------------------
    SchemeID FrameFusion::getSchemeID() const 
    { 
        return mFusionShader ? mFusionShader->getSchemeID() : 0; 
    }
    //---------------------------------------------------------------------
    Texture * FrameFusion::getTexture(const String & name, NCount mrtIndex)
    {
        TextureList::iterator i = mTextureList.find(name);
        if(i != mTextureList.end())
        {
            return i->second;
        }
        i = mTextureList.find(MultiName(name, mrtIndex));
        if(i != mTextureList.end())
        {
            return i->second;
        }

        return 0;
    }
    //-----------------------------------------------------------------------
    void FrameFusion::createTexture(bool forResizeOnly)
    {
        static NCount dummyCounter = 0;
        // Create temporary textures
        // In principle, temporary textures could be shared between multiple viewports
        // (FrameResults). This will save a lot of memory in case more viewports
        // are composited.
        TextureSet assignedTextures;
        const FrameShader::DefineList & it = mFusionShader->getDefineList();
        FrameShader::DefineList::const_iterator z, zend = it.end();
        for (z = it.begin(); z != zend; ++z)
        {
            FrameShader::FrameDefine *def = *z;

            if (def->mFusionSource == 0)
            {
                //This is a reference, isn't created in this compositor
                continue;
            }

            FrameObj * rendTarget;
            if (def->mSourceType == FrameShader::ST_Pool)
            {
                //This is a global texture, just link the created resources from the parent
                Fusion * pFusion = mFusionShader->getParent();
                if (def->mFormatList.size() > 1)
                {
                    NCount atch = 0;
                    for (PixelFormatList::iterator p = def->mFormatList.begin();
                         p != def->mFormatList.end(); ++p, ++atch)
                    {
                        Texture * tex = pFusion->getTexture(def->mName, atch);
                        mTextureList[MultiName(def->mName, atch)] = tex;
                    }
                    MultiTextureFrame * mrt = static_cast<MultiTextureFrame *>(pFusion->getFrameObj(def->mName));
                    mMultiFrameObjList[def->mName] = mrt;

                    rendTarget = mrt;
                }
                else
                {
                    Texture * tex = pFusion->getTexture(def->mName, 0);
                    mTextureList[def->mName] = tex;

                    rendTarget = tex->getBuffer()->getBuffer();
                }

            }
            else
            {
                NCount width = def->mWidth;
                NCount height = def->mHeight;
                Nui32 fsaa = 0;
                String fsaaHint;
                bool hwGamma = false;

                // Skip this one if we're only (re)creating for a resize & it's not derived
                // from the target size
                if (forResizeOnly && width != 0 && height != 0)
                    continue;

                deriveOptions(def, hwGamma, fsaa, fsaaHint);

                if(width == 0)
                    width = (NCount)((NIIf)(mResult->getViewport()->getPixelWidth()) * def->mWidthFactor);
                if(height == 0)
                    height = (NCount)((NIIf)(mResult->getViewport()->getPixelHeight()) * def->mHeightFactor);

                // determine options as a combination of selected options and possible options
                if(!def->mFSAA)
                {
                    fsaa = 0;
                    fsaaHint = StrUtil::WBLANK;
                }
                hwGamma = hwGamma || def->mHWGamma;

                /// Make the tetxure
                if(def->mFormatList.size() > 1)
                {
                    String MRTbaseName = _T("c") + N_conv(dummyCounter++) +
                        _T("/") + def->mName + _T("/") + mResult->getViewport()->getBuffer()->getName();
                    MultiTextureFrame * mrt = N_Engine().getRender()->createMultiFrame(MRTbaseName);
                    mMultiFrameObjList[def->mName] = mrt;

                    // create and bind individual surfaces
                    NCount atch = 0;
                    for (PixelFormatList::iterator p = def->mFormatList.begin();
                         p != def->mFormatList.end(); ++p, ++atch)
                    {

                        //String texname = MRTbaseName + "/" + StrConv::conv(atch);
                        String mrtLocalName = MultiName(def->mName, atch);
                        Texture * tex;
                        if(def->mPool)
                        {
                            // get / create pooled texture
                            tex = N_Only(Fusion).touchFusionTexture(this, 0,
                                mrtLocalName, width, height, *p, fsaa, fsaaHint,
                                hwGamma && !PixelUtil::isFloat(*p), assignedTextures, def->mSourceType);
                        }
                        else
                        {
                            tex = N_Only(Texture).createTexture(0, GroupInner, Texture::T_2D,
                                *p, width, height, 1, 0, Texture::MM_FRAME_RGB, 0, 0,
                                hwGamma && !PixelUtil::isFloat(*p), fsaa, fsaaHint);
                        }

                        TextureFrame * rt = tex->getBuffer()->getBuffer();
                        rt->setAutoFlush(false);
                        mrt->attach(atch, rt);

                        // Also add to local textures so we can look up
                        mTextureList[mrtLocalName] = tex;

                    }

                    rendTarget = mrt;
                }
                else
                {
                    //String texName =  "c" + StrConv::conv(dummyCounter++) +
                    //    "/" + def->name + "/" + mResult->getViewport()->getBuffer()->getName();

                    // space in the name mixup the cegui in the compositor demo
                    // this is an auto generated name - so no spaces can't hart us.
                    //std::replace(texName.begin(), texName.end(), ' ', '_');

                    Texture * tex;
                    if (def->mPool)
                    {
                        // get / create pooled texture
                        tex = N_Only(Fusion).touchFusionTexture(this, 0,
                            def->mName, width, height, def->mFormatList[0], fsaa, fsaaHint,
                            hwGamma && !PixelUtil::isFloat(def->mFormatList[0]),
                            assignedTextures, def->mSourceType);
                    }
                    else
                    {
                        tex = N_Only(Texture).createTexture(0, GroupInner, Texture::T_2D,
                            def->mFormatList[0], width, height, 1, 0, Texture::MM_FRAME_RGB,
                            0, 0, hwGamma && !PixelUtil::isFloat(def->mFormatList[0]), fsaa, fsaaHint);
                    }

                    rendTarget = tex->getBuffer()->getBuffer();
                    mTextureList[def->mName] = tex;
                }
            }

            //Set ExtFrameBuffer pool for sharing
            rendTarget->setRasterGroup(def->mRasterGroup);

            // Set up viewport over entire texture
            rendTarget->setAutoFlush(false);

            // We may be sharing / reusing this texture, so test before adding viewport
            if(rendTarget->getViewportCount() == 0)
            {
                Viewport * v;
                Camera * camera = mResult->getViewport()->getCamera();
                if (!camera)
                {
                    v = rendTarget->createViewport(N_PRIMARY_Viewport, camera);
                }
                else
                {
                    // Save last viewport and current aspect ratio
                    Viewport * oldViewport = camera->getViewport();
                    NIIf aspectRatio = camera->getAspectRatio();

                    v = rendTarget->createViewport(N_PRIMARY_Viewport, camera);

                    // Should restore aspect ratio, in case of auto aspect ratio
                    // enabled, it'll changed when add new viewport.
                    camera->setAspectRatio(aspectRatio);
                    // Should restore last viewport, i.e. never disturb user code
                    // which might based on that.
                    camera->setViewport(oldViewport);
                }

                v->setAutoClear(false);
                v->setOverlayEnable(false);
                v->setBgColour(Colour(0, 0, 0, 0));
            }
        }
    }
    //---------------------------------------------------------------------
    void FrameFusion::deriveOptions(const FrameShader::FrameDefine * def,
        bool & sRGB, Nui32 & fsaa, String & fsaaHint)
    {
        // search for passes on this texture def that either include a render_scene
        // or use input previous
        bool renderingScene = false;

        const FrameShader::TargetList & it = mFusionShader->getTargetList();
        FrameShader::TargetList::const_iterator i, iend = it.end();
        for (i = it.begin(); i != iend; ++i)
        {
            FusionShader* tp = *i;
            if (tp->getOutputName() == def->mName)
            {
                if (tp->isUsePreResult())
                {
                    // this may be rendering the scene implicitly
                    // Can't check mPreFusion against mResult->_getOriginalSceneCompositor()
                    // at this time, so check the position
                    const FusionInstance::FusionList & instit = mResult->getList();
                    FusionInstance::FusionList::const_iterator cc, ccend = instit.end();
                    renderingScene = true;
                    for(cc = instit.begin(); cc != ccend; ++cc)
                    {
                        FrameFusion * inst = *cc;
                        if(inst == this)
                            break;
                        else if(inst->isEnable())
                        {
                            // nope, we have another compositor before us, this will
                            // be doing the AA
                            renderingScene = false;
                        }
                    }
                    if(renderingScene)
                        break;
                }
                else
                {
                    FusionShader::OpList::const_iterator i, iend = tp->getList().end();
                    for (i = tp->getList().begin(); i != iend; ++i)
                    {
                        if((*i)->getType() == FusionShaderOp::OT_RenderScene)
                        {
                            renderingScene = true;
                            break;
                        }
                    }
                }
            }
        }

        if(renderingScene)
        {
            FrameObj * target = mResult->getViewport()->getBuffer();
            sRGB = target->isHardwareSRGB();
            fsaa = target->getFSAA();
            fsaaHint = target->getFSAAExt();
        }
        else
        {
            sRGB = false;
            fsaa = 0;
            fsaaHint = StrUtil::WBLANK;
        }
    }
    //-----------------------------------------------------------------------
    void FrameFusion::destroyTexture(bool forResizeOnly, bool clearReserveTextures)
    {
        // Remove temporary textures
        // We only remove those that are not shared, shared textures are dealt with
        // based on their reference count.
        // We can also only free textures which are derived from the target size, if
        // required (saves some time & memory thrashing / fragmentation on resize)

        //TextureSet assignedTextures;
        const FrameShader::DefineList & it = mFusionShader->getDefineList();
        FrameShader::DefineList::const_iterator z, zend = it.end();
        for(z = it.begin(); z != zend; ++z)
        {
            FrameShader::FrameDefine *def = *z;

            if (def->mFusionSource == 0)
            {
                //This is a reference, isn't created here
                continue;
            }

            // potentially only remove this one if based on size
            if(!forResizeOnly || def->mWidth == 0 || def->mHeight == 0)
            {
                NCount subSurf = def->mFormatList.size();

                // Potentially many surfaces
                for(NCount s = 0; s < subSurf; ++s)
                {
                    String texName = subSurf > 1 ? MultiName(def->mName, s) : def->mName;

                    TextureList::iterator i = mTextureList.find(texName);
                    if(i != mTextureList.end())
                    {
                        if(!def->mPool && def->mSourceType != FrameShader::ST_Pool)
                        {
                            // remove myself from central only if not pooled and not global
                            N_Only(Texture).remove(i->second->getOriginID());
                        }

                        // remove from local
                        // reserves are potentially cleared later
                        mTextureList.erase(i);

                    }

                } // subSurf

                if(subSurf > 1)
                {
                    MultiFrameObjList::iterator mrti = mMultiFrameObjList.find(def->mName);
                    if(mrti != mMultiFrameObjList.end())
                    {
                        if(def->mSourceType != FrameShader::ST_Pool)
                        {
                            N_Engine().getRender()->destroyFrame(mrti->second);
                        }

                        mMultiFrameObjList.erase(mrti);
                    }

                }

            } // not for resize or width/height 0
        }

        if(clearReserveTextures)
        {
            if(forResizeOnly)
            {
                // just remove the ones which would be affected by a resize
                RefTextureList::iterator j, jend = mRefTextureList.end();
                for(j = mRefTextureList.begin(); j != jend;)
                {
                    if(j->first->mWidth == 0 || j->first->mHeight == 0)
                    {
                        mRefTextureList.erase(j++);
                    }
                    else
                    {
                        ++j;
                    }
                }
            }
            else
            {
                mRefTextureList.clear();
            }
        }

        // Now we tell the central list of textures to check if its unreferenced,
        // and to remove if necessary. Anything shared that was left in the reserve textures
        // will not be released here
        N_Only(Fusion).freeFusionTexture(false);
    }
    //-----------------------------------------------------------------------
    FrameObj * FrameFusion::getFrameObj(const String & name)
    {
        TextureList::iterator i = mTextureList.find(name);
        if(i != mTextureList.end())
            return i->second->getBuffer()->getBuffer();

        MultiFrameObjList::iterator mi = mMultiFrameObjList.find(name);
        if(mi != mMultiFrameObjList.end())
            return mi->second;

        //Find the instance and check if it is before us
        FrameShader::FrameDefine * texDef = mFusionShader->getDefine(name);
        if (texDef != 0 && texDef->mFusionSource == 0)
        {
            //This FrameDefine is reference.
            //Since referenced TD's have no info except name we have to find original TD
            FrameShader::FrameDefine * refTexDef = 0;

            //Try chain first
            if(mResult)
            {
                FrameFusion * refCompInst = mResult->get(texDef->mFusionSource);
                if(refCompInst)
                {
                    refTexDef = refCompInst->getFusion()->getValid(refCompInst->getSchemeID())->getDefine(name);
                }
                else
                {
                    N_EXCEPT(NotExist, _I18n("Referencing non-existent compositor"));
                }
            }
            if(refTexDef == 0)
            {
                //Still NULL. Try global search.
                const Fusion * refComp = static_cast<Fusion *>(N_Only(Fusion).get(texDef->mFusionSource));
                if(refComp)
                {
                    refTexDef = refComp->getValid()->getDefine(name);
                }
            }

            if(refTexDef == 0)
            {
                N_EXCEPT(NotExist, _I18n("Referencing non-existent compositor texture"));
            }
            switch(refTexDef->mSourceType)
            {
                case FrameShader::ST_Fusion:
                {
                    //Find the instance and check if it is before us
                    FrameFusion * refCompInst = 0;
                    const FusionInstance::FusionList & it = mResult->getList();
                    FusionInstance::FusionList::const_iterator cc, ccend = it.end();
                    bool beforeMe = true;
                    for (cc = it.begin(); cc != ccend; ++cc)
                    {
                       FrameFusion * nextCompInst = *cc;
                       if (nextCompInst->getFusion()->getOriginID() == texDef->mFusionSource)
                       {
                          refCompInst = nextCompInst;
                          break;
                       }
                       if (nextCompInst == this)
                       {
                          //We encountered ourselves while searching for the compositor -
                          //we are earlier in the chain.
                          beforeMe = false;
                       }
                    }

                    if (refCompInst == 0 || !refCompInst->isEnable())
                    {
                        N_EXCEPT(InvalidState, _I18n("Referencing inactive compositor texture"));
                    }
                    if (!beforeMe)
                    {
                        N_EXCEPT(InvalidState, _I18n("Referencing compositor that is later in the chain"));
                    }
                    return refCompInst->getFrameObj(texDef->mTextureSource);
                }
                case FrameShader::ST_Pool:
                {
                    //Chain and global case - the referenced compositor will know how to handle
                    const Fusion * refComp = static_cast<Fusion *>(N_Only(Fusion).get(texDef->mFusionSource));
                    if(refComp == 0)
                    {
                        N_EXCEPT(NotExist, _I18n("Referencing non-existent compositor"));
                    }
                    return refComp->getFrameObj(texDef->mTextureSource);
                }
            case FrameShader::ST_Temp:
            default:
                N_EXCEPT(InvalidParam, _I18n("Referencing local compositor texture"));
            }
        }

        N_EXCEPT(InvalidParam, _I18n("Non-existent local texture name"));
    }
    //-----------------------------------------------------------------------
    ResourceID FrameFusion::getTextureID(const String & name, NCount mrtIndex)
    {
        FrameShader::FrameDefine * texDef = mFusionShader->getDefine(name);
        if(texDef == 0)
        {
            N_EXCEPT(NotExist, _T("Referencing non-existent FrameDefine"));
        }

        //Check if texture definition is reference
        if(!texDef->mFusionSource)
        {
            //This FrameDefine is reference.
            //Since referenced TD's have no info except name we have to find original TD

            FrameShader::FrameDefine * refTexDef = 0;
            if(mResult)
            {
                FrameFusion * refCompInst = mResult->get(texDef->mFusionSource);
                if(refCompInst)
                {
                    refTexDef = refCompInst->getFusion()->getValid(refCompInst->getSchemeID())->getDefine(texDef->mTextureSource);
                }
                else
                {
                    N_EXCEPT(NotExist, _I18n("Referencing non-existent compositor"));
                }
            }
            if(refTexDef == 0)
            {
                const Fusion * refComp = static_cast<Fusion *>(N_Only(Fusion).get(texDef->mFusionSource));
                if(refComp)
                {
                    refTexDef = refComp->getValid()->getDefine(texDef->mTextureSource);
                }
            }

            if(refTexDef == 0)
            {
                N_EXCEPT(NotExist, _I18n("Referencing non-existent compositor texture"));
            }

            switch(refTexDef->mSourceType)
            {
                case FrameShader::ST_Fusion:
                {
                    //Find the instance and check if it is before us
                    FrameFusion * refCompInst = 0;
                    const FusionInstance::FusionList & it = mResult->getList();
                    FusionInstance::FusionList::const_iterator cc, ccend = it.end();
                    bool beforeMe = true;
                    for(cc = it.begin(); cc !=ccend; ++cc)
                    {
                        FrameFusion* nextCompInst = *cc;
                        if (nextCompInst->getFusion()->getOriginID() == texDef->mFusionSource)
                        {
                            refCompInst = nextCompInst;
                            break;
                        }
                        if (nextCompInst == this)
                        {
                            //We encountered ourselves while searching for the compositor -
                            //we are earlier in the chain.
                            beforeMe = false;
                        }
                    }

                    if (refCompInst == 0 || !refCompInst->isEnable())
                    {
                        N_EXCEPT(InvalidState, _I18n("Referencing inactive compositor texture"));
                    }
                    if (!beforeMe)
                    {
                        N_EXCEPT(InvalidState, _I18n("Referencing compositor that is later in the chain"));
                    }
                    return refCompInst->getTextureID(texDef->mTextureSource, mrtIndex);
                }
                case FrameShader::ST_Pool:
                {
                    //Chain and global case - the referenced compositor will know how to handle
                    const Fusion * refComp = static_cast<Fusion *>(N_Only(Fusion).get(texDef->mFusionSource));
                    if(refComp == 0)
                    {
                        N_EXCEPT(NotExist, _I18n("Referencing non-existent compositor"));
                    }
                    return refComp->getTexture(texDef->mTextureSource, mrtIndex)->getOriginID();
                }
                case FrameShader::ST_Temp:
                default:
                    N_EXCEPT(InvalidParam, _I18n("Referencing local compositor texture"));
            }

        } // End of handling texture references

        if (texDef->mFormatList.size() == 1)
        {
            //This is a simple texture
            TextureList::iterator i = mTextureList.find(name);
            if(i != mTextureList.end())
            {
                return i->second->getOriginID();
            }
        }
        else
        {
            // try MRTs - texture (rather than target)
            TextureList::iterator i = mTextureList.find(MultiName(name, mrtIndex));
            if (i != mTextureList.end())
            {
                return i->second->getOriginID();
            }
        }

        N_EXCEPT(InvalidParam, _I18n("Non-existent local texture name"));
    }
    //-----------------------------------------------------------------------
    void FrameFusion::add(Listener * l)
    {
        mListeners.push_back(l);
    }
    //-----------------------------------------------------------------------
    void FrameFusion::remove(Listener * l)
    {
        mListeners.erase(std::find(mListeners.begin(), mListeners.end(), l));
    }
    //-----------------------------------------------------------------------
    /*void FrameFusion::onCameraChange(Camera * camera)
    {
        // update local texture's viewports.
        TextureList::iterator localTexIter = mTextureList.begin();
        TextureList::iterator localTexIterEnd = mTextureList.end();
        while(localTexIter != localTexIterEnd)
        {
            TextureFrame * target = localTexIter->second->getBuffer()->getBuffer();
            // skip target that has no viewport (this means texture is under MRT)
            if(target->getViewportCount() == 1)
            {
                target->getViewport(0)->setCamera(camera);
            }
            ++localTexIter;
        }

        // update MRT's viewports.
        MultiFrameObjList::iterator localMRTIter = mMultiFrameObjList.begin();
        MultiFrameObjList::iterator localMRTIterEnd = mMultiFrameObjList.end();
        while(localMRTIter != localMRTIterEnd)
        {
            MultiTextureFrame * target = localMRTIter->second;
            target->getViewport(0)->setCamera(camera);
            ++localMRTIter;
        }
    }*/
    //-----------------------------------------------------------------------
    FusionRender::~FusionRender()
    {
    }
    //-----------------------------------------------------------------------
    FrameFusion::Listener::~Listener()
    {
    }
    //-----------------------------------------------------------------------
    void FrameFusion::Listener::notifyMaterialSetup(Nui32 pass_id, Material * mat)
    {
    }
    //-----------------------------------------------------------------------
    void FrameFusion::Listener::notifyMaterialRender(Nui32 pass_id, Material * mat)
    {
    }
    //-----------------------------------------------------------------------
    FusionRender * FusionRender::createInstance(FrameFusion * fusion, const FusionShaderOp * op)
    {
        return 0; 
    }
    //-----------------------------------------------------------------------
}