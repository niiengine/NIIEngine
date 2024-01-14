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
#include "NiiRenderPattern.h"
#include "NiiSpaceManager.h"
#include "NiiFusionManager.h"
#include "NiiFrameFusion.h"
#include "NiiRenderQueue.h"
#include "NiiShadowRenderQueue.h"
#include "NiiSysSyncParam.h"
#include "NiiMaterial.h"
#include "NiiMurmurHash.h"

	/** 
	@version NIIEngine 6.0.0
	*/
	virtual void _loadJson(const rapidjson::Value & jsonValue, const HlmsJson::NamedBlocks & blocks,
		ShaderChMaterial * mat, GroupID gid, const String & additionalTextureExtension) const {}

	/** 
	@version NIIEngine 6.0.0
	*/
	virtual void _saveJson(const ShaderChMaterial * mat, String & outString, const String & additionalTextureExtension) const {}

    /// These are "default" or "Base" properties common to many implementations and thus defined here.
    /// Most of them start with the suffix hlms_
    struct _EngineAPI N_PropertyRender
    {
        static const IdString Macroblock 	        = IdString("PsoMacroblock");
        static const IdString Blendblock 	        = IdString("PsoBlendblock");
        static const IdString InputLayoutId         = IdString("InputLayoutId");
        static const IdString AlphaTestCmpFunc 	    = IdString( "alpha_test_cmp_func" );
        
        static const IdString Skeleton 			    = IdString( "hlms_skeleton" );
        static const IdString BonesPerVertex 	    = IdString( "hlms_bones_per_vertex" );
        static const IdString Pose 				    = IdString( "hlms_pose" );
        static const IdString PoseHalfPrecision     = IdString( "hlms_pose_half" );
        static const IdString PoseNormals 		    = IdString( "hlms_pose_normals" );

        static const IdString Normal 			    = IdString( "hlms_normal" );
        static const IdString QTangent 			    = IdString( "hlms_qtangent" );
        static const IdString Tangent 			    = IdString( "hlms_tangent" );
		//Change per mesh (hash can be cached on the obj)
        static const IdString Colour			    = IdString( "hlms_colour" );

        static const IdString IdentityWorld		    = IdString( "hlms_identity_world" );
        static const IdString IdentityViewProj 	    = IdString( "hlms_identity_viewproj" );
        /// When this is set, the value of IdentityViewProj is meaningless.
        static const IdString IdentityViewProjDynamic = IdString( "hlms_identity_viewproj_dynamic" );

        static const IdString UvCount			    = IdString( "hlms_uv_count" );
        static const IdString UvCount0			    = IdString( "hlms_uv_count0" );
        static const IdString UvCount1			    = IdString( "hlms_uv_count1" );
        static const IdString UvCount2			    = IdString( "hlms_uv_count2" );
        static const IdString UvCount3			    = IdString( "hlms_uv_count3" );
        static const IdString UvCount4 			    = IdString( "hlms_uv_count4" );
        static const IdString UvCount5			    = IdString( "hlms_uv_count5" );
        static const IdString UvCount6			    = IdString( "hlms_uv_count6" );
        static const IdString UvCount7			    = IdString( "hlms_uv_count7" );

        //Change per frame (grouped together with scene pass)
        static const IdString LightsDirectional		= IdString( "hlms_lights_directional" );
        static const IdString LightsDirNonCaster	= IdString( "hlms_lights_directional_non_caster" );
        static const IdString LightsPoint			= IdString( "hlms_lights_point" );
        static const IdString LightsSpot			= IdString( "hlms_lights_spot" );
        static const IdString LightsAreaApprox		= IdString( "hlms_lights_area_approx" );
        static const IdString LightsAreaLtc			= IdString( "hlms_lights_area_ltc" );
        static const IdString LightsAreaTexMask		= IdString( "hlms_lights_area_tex_mask" );
        static const IdString LightsAttenuation		= IdString( "hlms_lights_attenuation" );
        static const IdString LightsSpotParams		= IdString( "hlms_lights_spotparams" );
        static const IdString LightsAreaTexColour	= IdString( "hlms_lights_area_tex_colour" );
		
        //Change per scene pass
        static const IdString PsoClipDistances 		= IdString( "hlms_pso_clip_distances" );
        static const IdString GlobalClipPlanes		= IdString( "hlms_global_clip_planes" );
        static const IdString DualParaboloidMapping = IdString( "hlms_dual_paraboloid_mapping" );
        static const IdString InstancedStereo		= IdString( "hlms_instanced_stereo" );
        static const IdString StaticBranchLights	= IdString( "hlms_static_branch_lights" );
        static const IdString NumShadowMapLights	= IdString( "hlms_num_shadow_map_lights" );
        static const IdString NumShadowMapTextures	= IdString("hlms_num_shadow_map_textures" );
        static const IdString PssmSplits			= IdString( "hlms_pssm_splits" );
        static const IdString PssmBlend				= IdString( "hlms_pssm_blend" );
        static const IdString PssmFade				= IdString( "hlms_pssm_fade" );
        static const IdString ShadowCaster			= IdString( "hlms_shadowcaster" );
		
        static const IdString ShadowCasterDirectional = IdString( "hlms_shadowcaster_directional" );
        static const IdString ShadowCasterPoint		= IdString( "hlms_shadowcaster_point" );
        static const IdString ShadowUsesDepthTexture = IdString( "hlms_shadow_uses_depth_texture" );
        static const IdString RenderDepthOnly		= IdString( "hlms_render_depth_only" );
        static const IdString FineLightMask			= IdString( "hlms_fine_light_mask" );
        static const IdString UseUvBaking			= IdString( "hlms_use_uv_baking" );
        static const IdString UvBaking				= IdString( "hlms_uv_baking" );
        static const IdString BakeLightingOnly		= IdString( "hlms_bake_lighting_only" );
        static const IdString GenNormalsGBuf		= IdString( "hlms_gen_normals_gbuffer" );
        static const IdString PrePass				= IdString( "hlms_prepass" );
        static const IdString UsePrePass			= IdString( "hlms_use_prepass" );
        static const IdString UsePrePassMsaa		= IdString( "hlms_use_prepass_msaa" );
        static const IdString UseSsr				= IdString( "hlms_use_ssr" );
		
        // Per pass. Related with ScreenSpaceRefractions
        static const IdString SsRefractionsAvailable = IdString( "hlms_ss_refractions_available" );
        static const IdString EnableVpls			= IdString( "hlms_enable_vpls" );
        static const IdString ForwardPlus			= IdString( "hlms_forwardplus" );
        static const IdString ForwardPlusFlipY		= IdString( "hlms_forwardplus_flipY" );
        static const IdString ForwardPlusDebug		= IdString( "hlms_forwardplus_debug" );
        static const IdString ForwardPlusFadeAttenRange = IdString( "hlms_forward_fade_attenuation_range" );
        static const IdString ForwardPlusFineLightMask = IdString( "hlms_forwardplus_fine_light_mask" );
        static const IdString ForwardPlusCoversEntireTarget = IdString("hlms_forwardplus_covers_entire_target");
        static const IdString Forward3DNumSlices 	= IdString( "forward3d_num_slices" );
        static const IdString FwdClusteredWidthxHeight = IdString( "fwd_clustered_width_x_height" );
        static const IdString FwdClusteredWidth 	= IdString( "fwd_clustered_width" );
        static const IdString FwdClusteredLightsPerCell = IdString( "fwd_clustered_lights_per_cell" );
        static const IdString EnableDecals			= IdString( "hlms_enable_decals" );
        static const IdString FwdPlusDecalsSlotOffset = IdString( "hlms_forwardplus_decals_slot_offset" );
        static const IdString DecalsDiffuse			= IdString( "hlms_decals_diffuse" );
        static const IdString DecalsNormals			= IdString( "hlms_decals_normals" );
        static const IdString DecalsEmissive		= IdString( "hlms_decals_emissive" );
        static const IdString FwdPlusCubemapSlotOffset = IdString( "hlms_forwardplus_cubemap_slot_offset" );

        static const IdString Forward3D				= IdString( "forward3d" );
        static const IdString ForwardClustered		= IdString( "forward_clustered" );
        static const IdString VPos					= IdString( "hlms_vpos" );
        static const IdString ScreenPosInt			= IdString( "hlms_screen_pos_int" );
        static const IdString ScreenPosUv			= IdString( "hlms_screen_pos_uv" );
        static const IdString VertexId				= IdString( "hlms_vertex_id" );

        //Change per material (hash can be cached on the renderable)
        static const IdString AlphaTest 			= IdString( "alpha_test" );
        static const IdString AlphaTestShadowCasterOnly = IdString( "alpha_test_shadow_caster_only" );
        static const IdString AlphaBlend 			= IdString( "hlms_alphablend" );
        // Per material. Related with SsRefractionsAvailable
        static const IdString ScreenSpaceRefractions = IdString( "hlms_screen_space_refractions" );

        //Standard depth range is being used instead of reverse Z.
        static const IdString NoReverseDepth 		= IdString( "hlms_no_reverse_depth" );

        static const IdString Syntax  				= IdString( "syntax" );
        static const IdString Hlsl 					= IdString( "hlsl" );
        static const IdString Glsl					= IdString( "glsl" );
        static const IdString Glsles				= IdString( "glsles" );
        static const IdString Metal					= IdString( "metal" );
        static const IdString GL3Plus				= IdString( "GL3+" );
        static const IdString GLES					= IdString( "GLES" );
        static const IdString iOS					= IdString( "iOS" );
        static const IdString macOS					= IdString( "macOS" );
        static const IdString GLVersion;
        static const IdString HighQuality			= IdString( "hlms_high_quality" );
        static const IdString FastShaderBuildHack 	= IdString( "fast_shader_build_hack" );
        static const IdString TexGather				= IdString( "hlms_tex_gather" );
        static const IdString DisableStage			= IdString( "hlms_disable_stage" );

        //Useful GL Extensions
        static const IdString GlAmdTrinaryMinMax 	= IdString( "hlms_amd_trinary_minmax" );

        static const IdString * UvCountPtrs[8];
        
        static const int HlmsTypeBits 				= 3;
        static const int RenderableBits 			= 21;
        static const int PassBits 					= 8;

        static const int HlmsTypeShift 				= 32 - HlmsTypeBits;
        static const int RenderableShift 			= HlmsTypeShift - RenderableBits;
        static const int PassShift 					= RenderableShift - PassBits;
        static const int InputLayoutShift;

        static const int RendarebleHlmsTypeMask 	= (1 << (HlmsTypeBits + RenderableBits)) - 1;
        static const int HlmsTypeMask 				= (1 << HlmsTypeBits) - 1;
        static const int RenderableMask 			= (1 << RenderableBits) - 1;
        static const int PassMask 					= (1 << PassBits) - 1;
        
    };

namespace NII
{
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    //
    //-----------------------------------------------------------------------------------
    //-----------------------------------------------------------------------------------
    bool OrderCacheByHash(const RenderStateCache * o1, const RenderStateCache * o2)
    {
        return o1->mHash < o2->mHash;
    }
	//-----------------------------------------------------------------------------------
    const IdString * N_PropertyRender::UvCountPtrs[8] =
    {
        &N_PropertyRender::UvCount0,
        &N_PropertyRender::UvCount1,
        &N_PropertyRender::UvCount2,
        &N_PropertyRender::UvCount3,
        &N_PropertyRender::UvCount4,
        &N_PropertyRender::UvCount5,
        &N_PropertyRender::UvCount6,
        &N_PropertyRender::UvCount7
    };
    //-----------------------------------------------------------------------------------
    HighLevelGpuProgram * compile(const String & code, const String & src, Nui32 finalHash, ShaderType type)
    {
        HighLevelGpuProgram * gp = N_Only(HighLevelGpuProgram).create(StrConv::conv(finalHash) + ShaderFiles[type],
            GroupInner, mShaderProfile, static_cast<GpuProgramType>(type));

        gp->setProgramCode(code) 
        gp->setProgramSrc(src);

        if( mShaderTargets[type].size() )
        {
            //D3D-specific
            gp->set( _T("target"), mShaderTargets[type] );
            gp->set( _T("entry_point"), "main" );
        }

        gp->setBuildParametersFromReflection(false);
        gp->setVertexMatrixFusion(get(N_PropertyRender::Skeleton) != 0);
        gp->setVertexInterpolation(false);
        gp->setVertexOffsetCount(get(N_PropertyRender::Pose) != 0);
        gp->setVpAndRtArrayIndexFromAnyShaderRequired(get(N_PropertyRender::InstancedStereo) != 0);
        gp->setVertexTextureFetch(false);

        gp->load();

        return gp;
    }

    /** 阴影测试器
    @version NIIEngine 3.0.0
    */
    class _EngineInner ShadowRenderTest : public RenderFilter
    {
    public:
        ShadowRenderTest(SpaceManager * gm, RenderPattern * sp):
            mScene(gm),
            mPattern(sp),
            mAlphaShadowCast(false)
        {
        }

        ///@copydetail RenderFilter::access
        void render(SpaceObj * sobj, GeometryObj * gobj, const ShaderCh * ch)
        {
            /* 如果我们使用透明投射阴影模式&在其他访问方法中它不需要实现这个,因为透明
                的从来没有分组,而总是排序,跳过这个
            */
            if(mAlphaShadowCast && !ch->getParent()->getParent()->isAlphaShadowCast())
                return;

            if(mPattern->testRender(ch, gobj))
            {
                mCh = mScene->applyCh(ch, false, true);
                if(mLightEnable)
                    mScene->render(gobj, mCh, mLightClip);
                else
                    mScene->render(gobj, mCh, *mLights, mLightClip);
            }
        }

        ///@copydetail RenderFilter::access
        void render(const RenderItemList & obj, bool reverse)
        {
			const ShaderCh * lch = 0;
			RenderItemList::const_iterator i, irendend = obj.end();
			for(i = obj.begin(); i != irendend; ++i)
			{
				lch = i->mCh;
                if(mPattern->testRender(lch))
                {
                    GeometryObj * re = i->mGeometryObj;
                    
                    mCh = mScene->applyCh(lch, false, true);
                    if(mPattern->testRender(mCh, re))
                    {
                        if(mLightEnable)
                            mScene->render(re, mCh, mLightClip);
                        else
                            mScene->render(re, mCh, *mLights, mLightClip);
                    }
                }
			}
        }

        /** 设置参数
        @param[in] alphacast 透明物也投射阴影
        @param[in] lightclip 剪裁灯光
        @param[in] chlight 使用通道灯光
        @param[in] lights 灯光列表
        @version NIIEngine 3.0.0
        */
        void setParam(bool alphacast, bool lightclip, bool light, const LightList * lights)
        {
            mAlphaShadowCast = alphacast;
            mLightClip = lightclip;
            mLightEnable = light;
            mLights = lights;
        }
    protected:
        SpaceManager * mScene;
        RenderPattern * mPattern;
        const LightList * mLights;
        const ShaderCh * mCh;
        bool mAlphaShadowCast;
        bool mLightEnable;
        bool mLightClip;
    };
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    LightList RenderPattern::mNullLight;
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // RenderPattern::Listener
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    RenderPattern::Listener::Listener()
    {
    }
    //------------------------------------------------------------------------
    RenderPattern::Listener::~Listener()
    {
    }
    //------------------------------------------------------------------------
    void RenderPattern::Listener::onFindGeometry(SpaceManager * sm, Viewport * v, ShadowType type)
    {
        (void)sm;
        (void)v;
		(void)type;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // RenderPattern
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    RenderPattern::RenderPattern(RenderPatternType type, Nid id, SpaceManager * sm, RenderSys * rsys, SysSyncParam * param) :
        mRenderingType(RT_Forward),
        mLightCnt(0u),
        mAreaLightApproxCnt(1u),
        mAreaLightsLtcCnt(1u),
        mAreaLightIndex(0u),
        mCurrentAreaApproxLight2Cnt(0u),
        mCurrentAreaApproxLightCnt(0u),
        mCurrentAreaLtcLightCnt(0u),
        mListener(&c_defaultListener),
        mMaterial(0),
        mType(type),
        mID(id),
        mCast(0),
        mReceive(0),
        mRenderQueue(0),
        mShadowExtent(10000.0f),
        mShadowExtentMax(0.0f),
        mShadowColour(Colour(0.25, 0.25, 0.25)),
        mParent(sm),
        mRenderSys(rsys),
        mSyncParams(param),
        mBackFaceInCast(true),
        mInfiniteFar(true),
        mLightClipPlane(false),
        mShadowEnable(true),
        mInit(false)
    {
        mCastRID = N_Only(Material).genValidID();
        mReceiveRID = N_Only(Material).genValidID();
        mTest = N_new ShadowRenderTest(sm, this);
		
		mTextureBindGroupList.resize(ST_PCnt);
    }
    //------------------------------------------------------------------------
    RenderPattern::~RenderPattern()
    {
        N_delete mTest;
		
        clearCache();

        MaterialList::const_iterator itor = mMaterialList.begin();
        MaterialList::const_iterator end  = mMaterialList.end();

        while( itor != end )
        {
            if(itor->second.mManager && mRenderSys)
                mRenderSys->removeMaterial(itor->first);

            N_delete itor->second.mMaterial;
            ++itor;
        }

        mMaterialList.clear();
        mMaterial = 0;

        if(mRenderSys && mType < RPT_Count)
        {
            mRenderSys->removeRenderPattern(mType);
            mRenderSys = 0;
        }
    }
    //------------------------------------------------------------------------
    bool RenderPattern::isValid()
    {
        return false;
    }
    //------------------------------------------------------------------------
    void RenderPattern::renderShadow(Camera * cam, const Viewport * view, const LightList &)
    {
        mCamera = cam;
        mViewPort = view;
    }
    //------------------------------------------------------------------------
    void RenderPattern::applyTexture(const ShaderCh * ch)
    {
        NCount unit = 0;
        ShaderChTexture::Textures::const_iterator i, iend = ch->getTexture().getTextures().end();
        for(i = ch->getTexture().getTextures().begin(); i != iend; ++i, ++unit)
        {
            ShaderChTextureUnit * tunit = *i;
            if(ch->getProgram().isExist(ShaderChProgram::ST_VS))
            {
                if(tunit->isProjTexture())
                {
                    mSyncParams->setTextureProjector(tunit->getProjFrustum(), unit);
                }
            }

            if(tunit->getContentType() == ShaderChTexture::T_FUSION)
            {
                FusionInstance * currentChain = mParent->getFrameResult();
                if(!currentChain)
                {
                    N_EXCEPT(InvalidState, _I18n("A ch that wishes to reference a compositor texutre ")
                        _I18n("attempted to render in a pipeline without a compositor"));
                }
                FrameFusion * ff = currentChain->get(tunit->getFrameFusion());
                if(ff == 0)
                {
                    N_EXCEPT(NotExist, _I18n("Invalid compositor content_type compositor name"));
                }
                Texture * ftex = ff->getTexture(tunit->getFusionTexture(), tunit->getMultiIndex());
                if(ftex == 0)
                {
                    N_EXCEPT(NotExist, _I18n("Invalid compositor content_type texture name"));
                }
                tunit->setTexture(ftex);
            }
            mRenderSys->_enableTexture(unit, tunit);
        }
        mRenderSys->_enableTexture(ch->getTexture().getCount());
    }
    //------------------------------------------------------------------------
    void RenderPattern::applyLight(const ShaderCh * ch, const LightList & src, NCount & start, LightList & app)
    {
        NCount i, iend = ch->getLightRenderLimit();
        for(i = 0; i < iend && start < src.size(); ++start)
        {
            Light * temp = src[start];

            if(ch->getLightTypeLimit() == LT_NONE ||
                ch->getLightTypeLimit() == temp->getType() ||
                (ch->getLightMark() & temp->getLightMark()) == 0)
            {
                continue;
            }
            app.push_back(temp);
        }
    }
	//------------------------------------------------------------------------
    void RenderPattern::setup(const VFSList & vfsl)
    {
        clearCache();
        GpuProgramCodeGen::setup(vfsl);
    }
	//------------------------------------------------------------------------
    void RenderPattern::setQuality(QualityType type)
    {
        clearCache();
        GpuProgramCodeGen::setQuality(type);
    }
    //------------------------------------------------------------------------
    void RenderPattern::setRender(RenderSys * rsys)
    {
        clearCache();
        GpuProgramCodeGen::setRender(rsys);
        if(mRenderSys)
        {
            if(!mMaterial)
			{
                mMaterial = createMaterial(0, ShaderChStencil(), ShaderChBlend(), StringIdMap(), false);
				mMaterial->setName("[Default]");
			}
        }
    }
    //------------------------------------------------------------------------
    void RenderPattern::setAreaLightForward(uint16 max, uint16 ltcMax)
    {
        mAreaLightApproxCnt = max;
        mAreaLightsLtcCnt = ltcMax;
    }
	//------------------------------------------------------------------------
    Nmark RenderPattern::addObject(GeometryObj * gobj, const PropertyValueList & plist, const SegmentRefGroupList & pieces)
    {
        assert(mObjectList.size() <= N_PropertyRender::RenderableMask);

        Object obj(gobj, plist, pieces);

        ObjectList::iterator it = std::find(mObjectList.begin(), mObjectList.end(), obj);
        if(it == mObjectList.end())
        {
            mObjectList.push_back(obj);
            it = mObjectList.end() - 1;
        }

        //3 bits for mType (see getMaterial)
        return (mType << N_PropertyRender::HlmsTypeShift) | ((it - mObjectList.begin()) << N_PropertyRender::RenderableShift);
    }
	//------------------------------------------------------------------------
    void RenderPattern::sortLight(LightList &)
    {
    }
    //------------------------------------------------------------------------
    void RenderPattern::setCullingMode(CullingMode mode)
    {
        mCullingMode = mode;
        mRenderSys->_setCullingMode(mode);
    }
    //------------------------------------------------------------------------
    bool RenderPattern::testRender(const ShaderCh * ch, const GeometryObj * obj)
    {
        return true;
    }
    //------------------------------------------------------------------------
    bool RenderPattern::testRender(const ShaderCh * ch)
    {
        if(mParent->isOptMaterial())
        {
            ShaderFusion * opt = ch->getParent()->getParent()->getLOD();
            if(opt->getCount() <= ch->getIndex())
            {
                return false;
            }
        }
        return true;
    }
    //------------------------------------------------------------------------
    bool RenderPattern::isRenderShadow() const
    {
        return false;
    }
    //------------------------------------------------------------------------
    bool RenderPattern::isTextureShadow() const
    {
        return false;
    }
    //------------------------------------------------------------------------
    bool RenderPattern::isStencilShadow() const
    {
        return false;
    }
    //------------------------------------------------------------------------
    void RenderPattern::deriveShadow(const ShaderCh *& out, const ShaderCh * src)
    {
        out = src;
    }
    //------------------------------------------------------------------------
    void RenderPattern::init()
    {
        if(!mCast)
        {
            Material * cast = static_cast<Material *>(N_Only(Material).get(mCastRID));
            if(cast == 0)
            {
                cast = static_cast<Material *>(N_Only(Material).create(mCastRID, GroupInner));
                mCast = cast->get(0)->get(0);
                ShaderChColour * scc = mCast->setColour(ShaderCh::SOT_Set);
                scc->setAmbient(Colour::White);
                scc->setDiffuse(Colour::Black);
                scc->setEmissive(Colour::Black);
                scc->setSpecular(Colour::Black);
                ShaderChFog tempfog(FM_NONE, Colour::White, 0.001f, 0.0, 1.0);
                mCast->setFog(ShaderCh::SOT_Set, &tempfog);

                cast->ref(false);
            }
            else
            {
                mCast = cast->get(0)->get(0);
            }
        }

        if(!mReceive)
        {
            Material * receive = static_cast<Material *>(N_Only(Material).get(mReceiveRID));
            if(receive == 0)
            {
                receive = static_cast<Material *>(N_Only(Material).create(mReceiveRID, GroupInner));
                mReceive = receive->get(0)->get(0);
                // 这里不设置灯光和混合模式,依赖于additive / modulative
                ShaderChTextureUnit * t = mReceive->setTexture(ShaderCh::SOT_Set)->create();
                t->getSample()->setMode(TAM_CLAMP, TAM_CLAMP, TAM_CLAMP);
                receive->ref(false);
            }
            else
            {
                mReceive = receive->get(0)->get(0);
            }
        }
    }
    //------------------------------------------------------------------------
    void RenderPattern::render(RenderGroup * grp)
    {
        if(mShadowEnable && mViewPort->isShadowEnable() && !mParent->isLockShaderCh())
        {
            renderImpl(grp);
        }
        else
        {
            renderBasic(grp);
        }
    }
    //------------------------------------------------------------------------
    void RenderPattern::renderBasic(RenderGroup * rg)
    {
        RenderGroup::LevelList::const_iterator z, zend = rg->getLevelList().end();
        for(z = rg->getLevelList().begin(); z != zend; ++z)
        {
            RenderLevelGroup * rlg = z->second;

            rlg->sort(mCamera);

            mTest->setParam(false, true, true, &mNullLight);
            rlg->render(ST_BasicMaterial, mTest);

            mTest->setParam(false, true, true, &mNullLight);
            rlg->render(ST_AlphaMaterial, mTest);

            mTest->setParam(false, true, true, &mNullLight);
            rlg->render(ST_AlphaViewDescend, mTest);
        }
    }
    //------------------------------------------------------------------------
    void RenderPattern::findLight(LightList & dest)
    {
        if(dest.getHash() != mLightHash)
        {
            mLightHash = dest.getHash();
            mParent->notifyLight();
        }
    }
    //------------------------------------------------------------------------
    void RenderPattern::add(Listener * obj)
    {
        mListeners.push_back(obj);
    }
    //------------------------------------------------------------------------
    void RenderPattern::remove(Listener * obj)
    {
        Listeners::iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            if(*i == obj)
            {
                 mListeners.erase(i);
                 break;
            }
        }
    }
    //------------------------------------------------------------------------
    void RenderPattern::setShadowColour(const Colour & colour)
    {
        mShadowColour = colour;
    }
    //------------------------------------------------------------------------
    RenderQueue * RenderPattern::createQueue() const
    {
        RenderQueue * re = N_new RenderQueue();
        re->init();
        return re;
    }
    //------------------------------------------------------------------------
    void RenderPattern::onFindGeometry(Viewport * v, ShadowType type)
    {
        Listeners::iterator i, iend = mListeners.end();
        for(i = mListeners.begin(); i != iend; ++i)
        {
            (*i)->onFindGeometry(mParent, v, type);
        }
    }
    //------------------------------------------------------------------------
    const RenderStateCache * RenderPattern::createCache(HashType objhash, const RenderStateCache & rsc,
        HashType finalHash, const RenderItem & item)
    {
        //OgreProfileExhaustive("RenderPattern::createCache");

        //Set the properties by merging the cache from the pass, with the cache from renderable
        mPropertys.clear();
        //If retVal is null, we did something wrong earlier
        //(the cache should've been generated by now)
        const Object * ccobj = getObject(objhash);
        mPropertys.reserve(rsc.mPropertyList.size() + ccobj->mPropertyList.size());
        mPropertys.insert(mPropertys.end(), ccobj->mPropertyList.begin(), ccobj->mPropertyList.end());
        {
            //Now copy the properties from the pass (one by one, since be must maintain the order)
            PropertyValueList::const_iterator itor = rsc.mPropertyList.begin();
            PropertyValueList::const_iterator end  = rsc.mPropertyList.end();

            while( itor != end )
            {
                add(itor->mKey, itor->mValue);
                ++itor;
            }
        }

        for(size_t i = 0; i < ST_PCnt; ++i)
            mTextureRegs[i].clear();

        onGenProperty();

        //Retrieve the shader code from the code cache
        ObjectShader shader( ccobj->mSegmentList );
        remove( N_PropertyRender::Macroblock );
        remove( N_PropertyRender::Blendblock );
        remove( N_PropertyRender::InputLayoutId );
        shader.mObject.mPropertyList.swap(mPropertys); ///???
        {
            ShaderList::iterator itCodeCache = std::find(mShaderList.begin(), mShaderList.end(), shader);
            if(itCodeCache == mShaderList.end())
                compile(shader);
            else
            {
                shader.mProgramList = itCodeCache->mProgramList;
                shader.mObject.mPropertyList.swap(mPropertys);
            }
        }

        RenderStateObject rso;
        rso.init();
        rso.mVS = shader.mProgramList[ST_VS];
        rso.mGS = shader.mProgramList[ST_GS];
        rso.mTS = shader.mProgramList[ST_TS];
        rso.mDS = shader.mProgramList[ST_DS];
        rso.mFS = shader.mProgramList[ST_FS];

        ShadowType stype = get(N_PropertyRender::ShadowCaster) != 0 ? ST_Cast : ST_Normal;

        const ShaderChMaterial * mat = item.mGeometryObj->getMaterial();
        rso.mStencil = mat->getStencil(stype);
        rso.mBlend = mat->getBlend(stype);
        rso.mSampleObject = rsc.mRSO.mSampleObject;

        rso.initStencil();

        const size_t numGlobalClipDistances = (size_t)get(N_PropertyRender::PsoClipDistances);
        rso.mRenderMark = (1u << numGlobalClipDistances) - 1u;

        //TODO: Configurable somehow (likely should be in mat).
        rso.mRenderMark = 0xffffffff;

        if( item.mGeometryObj )
        {
            const GeometryRawList & vaos = item.mGeometryObj->getVaos(stype);
            if(vaos.size())
            {
                //v2 object. TODO: LOD? Should we allow Vaos with different vertex formats on LODs?
                //(also the mat hash in the renderable would have to account for that)
                rso.mType = vaos.front()->getType();
                rso.mUnitsList = vaos.front()->getVertexDeclaration();
            }
            else
            {
                //v1 object.
                v1::GeometryRaw renderOp;
                item.mGeometryObj->getGeometry(renderOp, stype);
                rso.mType = renderOp.mType;
                rso.mUnitsList = renderOp.vertexData->vertexDeclaration->convertToV2();
            }

            rso.enablePrimitiveRestart = true;
        }

        mRenderSys->createRSO(&rso);

        RenderStateCache cache(finalHash, mType, rso);
        RenderStateCacheList::iterator it = std::lower_bound(mShaderCache.begin(), mShaderCache.end(), &cache, OrderCacheByHash);

        assert((it == mShaderCache.end() || (*it)->mHash != finalHash) &&
                "Can't add the same shader to the cache twice! (or a hash collision happened)");

        RenderStateCache * re = new RenderStateCache(cache);
        mShaderCache.insert(it, re);

        if(mShaderProfile == "hlsl" || mShaderProfile == "metal")
            return; //D3D embeds the texture slots in the shader.

        const RenderStateObject & trso = re->mRSO;
        GpuProgram * shaders[ST_PCnt];
        shaders[ST_VS] = trso.mVS;
        shaders[ST_FS] = trso.mFS;
        shaders[ST_GS] = trso.mGS;
        shaders[ST_TS] = trso.mTS;
        shaders[ST_DS] = trso.mDS;

        String paramName;
        for(size_t i = 0; i < ST_PCnt; ++i)
        {
            if(shaders[i])
            {
                GpuProgramParam * params = shaders[i]->getDefaultParam();

                TextureRegsVec::const_iterator itor = mTextureRegs[i].begin();
                TextureRegsVec::const_iterator end  = mTextureRegs[i].end();

                while(itor != end)
                {
                    const char * paramNameC = &mTextureNameStrings[itor->strNameIdxStart];
                    paramName = paramNameC;
                    params->set(paramName, static_cast<int>(itor->texUnit));
                    ++itor;
                }
            }
        }

        return re;
    }
	//------------------------------------------------------------------------
	ShaderChMaterial * RenderPattern::createMaterialImpl(Nid id, const ShaderChStencil * stencil, const ShaderChBlend * blend, const StringIdMap & idlist)
	{ 
		return 0;
	}
    //------------------------------------------------------------------------
    void RenderPattern::onGenProperty()
    {
    }
    //------------------------------------------------------------------------
    void RenderPattern::bindTexture(ShaderType type, const VString & name, Nui32 texUnit)
    {
        mTextureRegs[type].push_back(TextureRegs(startIdx, texUnit));
        add(name, texUnit);
    }
    //------------------------------------------------------------------------
    void RenderPattern::unbindTexture(ShaderType type, const VString & name, Nui32 texUnit)
    {
        
    }
    //------------------------------------------------------------------------
    void RenderPattern::addObject(GeometryObj * obj)
    {
        //OgreProfileExhaustive("RenderPattern::addObject");

        mPropertys.clear();

        add(N_PropertyRender::Skeleton, obj->hasSkeletonAnimation());

        add(N_PropertyRender::Pose, obj->getNumPoses());
        add(N_PropertyRender::PoseHalfPrecision, obj->getPoseHalfPrecision());
        add(N_PropertyRender::PoseNormals, obj->getPoseNormals());

        uint16 numTexCoords = 0;
        if(obj->getVaos(VpNormal).empty())
        {
            v1::GeometryRaw op;
            //The RenderPattern uses the pass scene data to know whether this is a caster pass.
            //We want to know all the details, so request for the non-caster RenderOp.
            obj->getGeometry(op, false);
            v1::VertexDeclaration * vertexDecl = op.vertexData->vertexDeclaration;
            const v1::VertexDeclaration::VertexElementList &elementList = vertexDecl->getElements();
            v1::VertexDeclaration::VertexElementList::const_iterator itor = elementList.begin();
            v1::VertexDeclaration::VertexElementList::const_iterator end  = elementList.end();

            while( itor != end )
            {
                const v1::VertexElement & vertexElem = *itor;
                switch(vertexElem.getSemantic())
                {
                case VES_NORMAL:
                    if( v1::VertexElement::getTypeCount( vertexElem.getType() ) < 4 )
                    {
                        add(N_PropertyRender::Normal, 1);
                    }
                    else
                    {
                        add(N_PropertyRender::QTangent, 1);
                    }
                    break;
                case VES_TANGENT:
                    add(N_PropertyRender::Tangent, 1);
                    break;
                case VES_DIFFUSE:
                    add(N_PropertyRender::Colour, 1);
                    break;
                case VES_TEXTURE_COORDINATES:
                    numTexCoords = std::max<uint>(numTexCoords, vertexElem.getIndex() + 1);
                    add(*N_PropertyRender::UvCountPtrs[vertexElem.getIndex()], v1::VertexElement::getTypeCount(vertexElem.getType()));
                    break;
                case VES_BLEND_WEIGHTS:
                    add(N_PropertyRender::BonesPerVertex, v1::VertexElement::getTypeCount(vertexElem.getType()));
                    break;
                default:
                    break;
                }
                ++itor;
            }

            //v1::VertexDeclaration doesn't hold opType information. We need to save it now.
            //This means we do not allow LODs with different operation types or vertex layouts
            uint16 inputLayoutId = vertexDecl->getVaoType( mRenderSys, op.mType );
            add( N_PropertyRender::InputLayoutId, inputLayoutId );
        }
        else
        {
            //TODO: Account LOD
            GeometryRaw * vao = obj->getVaos( VpNormal )[0];
            const VertexBufferList &vertexBuffers = vao->getVertexBuffers();

            uint16 semIndex[VES_COUNT];
            memset( semIndex, 0, sizeof( semIndex ) );
            VertexBufferList::const_iterator itor = vertexBuffers.begin();
            VertexBufferList::const_iterator end  = vertexBuffers.end();

            while( itor != end )
            {
                const VertexUnitList & units = (*itor)->getVertexElements();
                VertexUnitList::const_iterator itElements = units.begin();
                VertexUnitList::const_iterator enElements = units.end();

                while( itElements != enElements )
                {
                    switch( itElements->mSemantic )
                    {
                    case VES_NORMAL:
                        if( v1::VertexElement::getTypeCount( itElements->mType ) < 4 )
                        {
                            add( N_PropertyRender::Normal, 1 );
                        }
                        else
                        {
                            add( N_PropertyRender::QTangent, 1 );
                        }
                        break;
                    case VES_TANGENT:
                        add( N_PropertyRender::Tangent, 1 );
                        break;
                    case VES_DIFFUSE:
                        add( N_PropertyRender::Colour, 1 );
                        break;
                    case VES_TEXTURE_COORDINATES:
                        numTexCoords = std::max<uint>( numTexCoords, semIndex[itElements->mSemantic-1]++ + 1 );
                        add( *N_PropertyRender::UvCountPtrs[semIndex[itElements->mSemantic-1]++], v1::VertexElement::getTypeCount( itElements->mType ) );
                        break;
                    case VES_BLEND_WEIGHTS:
                        add( N_PropertyRender::BonesPerVertex, v1::VertexElement::getTypeCount( itElements->mType ) );
                        break;
                    default:
                        break;
                    }
                    ++itElements;
                }

                ++itor;
            }

            //We do not allow LODs with different operation types or vertex layouts
            add(N_PropertyRender::InputLayoutId, vao->getVaoType());
        }

        add(N_PropertyRender::UvCount, numTexCoords);

        ShaderChMaterial * mat = obj->getMaterial();

        add(N_PropertyRender::AlphaTest, mat->getAlphaTest() != CMPF_ALWAYS_PASS);
        add(N_PropertyRender::AlphaTestShadowCasterOnly, mat->isAlphaTestOnlyInCast());
        add(N_PropertyRender::AlphaBlend, mat->getBlend(false)->isAutoTransparent());

        if(obj->getUseIdentityWorldMatrix())
            add(N_PropertyRender::IdentityWorld, 1);

        if(obj->getUseIdentityViewProjMatrixIsDynamic())
            add(N_PropertyRender::IdentityViewProjDynamic, 1);
        else if(obj->getUseIdentityProjection())
            add(N_PropertyRender::IdentityViewProj, 1);

        add( N_PropertyRender::Macroblock, obj->getMaterial()->getStencil(false)->mLifetimeId );
        add( N_PropertyRender::Blendblock, obj->getMaterial()->getBlend(false)->mLifetimeId );

        SegmentRefGroupList pieces;
        pieces.resize(ST_PCnt);
        if(mat->getAlphaTest() != CMPF_ALWAYS_PASS)
        {
            pieces[ST_FS][N_PropertyRender::AlphaTestCmpFunc] = GpuProgramManager::getCmpString( mat->getAlphaTest() );
        }
        onAddObject(obj, pieces, ShadowType::ST_Normal);

        Nui32 outHash = addObject(obj, mPropertys, pieces);

        //For shadow casters, turn normals off. UVs & diffuse also off unless there's alpha testing.
        add( N_PropertyRender::Normal, 0 );
        add( N_PropertyRender::QTangent, 0 );
        add( N_PropertyRender::AlphaBlend, mat->getBlend(true)->isAutoTransparent() );
        SegmentRefGroupList piecesCaster;
        piecesCaster.resize(ST_PCnt);
        if( mat->getAlphaTest() != CMPF_ALWAYS_PASS )
        {
            piecesCaster[ST_FS][N_PropertyRender::AlphaTestCmpFunc] = pieces[ST_FS][N_PropertyRender::AlphaTestCmpFunc];
        }
        if(obj->getVaos(VpShadow).size())
        {
            //v2 objects can have an input layout that is different for shadow mapping
            GeometryRaw * vao = obj->getVaos(VpShadow)[0];
            add(N_PropertyRender::InputLayoutId, vao->getVaoType());
        }
        onAddObject(obj, piecesCaster, Listener::ST_Cast);
        add(N_PropertyRender::Macroblock, obj->getMaterial()->getStencil(true)->mLifetimeId);
        add(N_PropertyRender::Blendblock, obj->getMaterial()->getBlend(true)->mLifetimeId);
        Nui32 outCasterHash = addObject(obj, mPropertys, piecesCaster);

        obj->setRenderHash(outHash, outCasterHash);
    }
    //-----------------------------------------------------------------------------------
    ShaderChMaterial * RenderPattern::createMaterial(Nid id, const ShaderChStencil & stencil, const ShaderChBlend & blend,
        const StringIdMap & strmap, bool autoDestroy, const String & filename, GroupID gid)
    {
        if(mMaterialList.find(id) != mMaterialList.end())
        {
            N_EXCEPT(Exception::ERR_DUPLICATE_ITEM, "A material with name '" + id.getFriendlyText() + "' already exists.");
        }

        ShaderChStencil * stencil = mRenderSys->getStencil(stencil);
        ShaderChBlend * blend = mRenderSys->getBlend(blend);

        ShaderChMaterial * re = createMaterialImpl(id, stencil, blend, strmap);

        mMaterialList[id] = Material(re, autoDestroy, filename, gid);

        re->init();

        if(autoDestroy)
            mRenderSys->addMaterial(re);

        return re;
    }
    //-----------------------------------------------------------------------------------
    ShaderChMaterial * RenderPattern::getMaterial(Nid id) const
    {
        MaterialList::const_iterator itor = mMaterialList.find(id);
        if(itor != mMaterialList.end())
            return itor->second.mMaterial;

        return 0;
    }
    //-----------------------------------------------------------------------------------
    void RenderPattern::destroyMaterial(Nid id)
    {
        MaterialList::iterator itor = mMaterialList.find(id);
        if(itor == mMaterialList.end())
        {
            N_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Can't find material with name '" + id.getFriendlyText() + "'");
        }

        if(itor->second.mManager)
            mRenderSys->removeMaterial(id);

        N_delete itor->second.mMaterial;
        mMaterialList.erase(itor);
    }
    //-----------------------------------------------------------------------------------
    void RenderPattern::destroyAllMaterial()
    {
        MaterialList::const_iterator itor = mMaterialList.begin();
        MaterialList::const_iterator end  = mMaterialList.end();

        while(itor != end)
        {
            if(itor->second.mManager && mRenderSys)
                mRenderSys->removeMaterial(itor->first);

            N_delete itor->second.mMaterial;
            ++itor;
        }

        mMaterialList.clear();
        mMaterial = createMaterial(0, ShaderChStencil(), ShaderChBlend(), StringIdMap(), false);
		mMaterial->setName("[Default]");
    }
    //-----------------------------------------------------------------------------------
    const RenderStateCache * RenderPattern::getCache(HashType statehash, const RenderStateCache & rsc,
        const RenderItem & item, ShadowType stype)
    {
        HashType finalHash;
        uint32 hash[2];
        hash[0] = stype == ST_Cast ? item.mGeometryObj->getCasterHash() : item.mGeometryObj->getNormalHash();
        hash[1] = rsc.mHash;

        //MurmurHash3_x86_32( hash, sizeof( hash ), IdString::Seed, &finalHash );

        //If this assert triggers, we've created too many shader variations (or bug)
        assert( (hash[0] >> N_PropertyRender::RenderableShift) <= N_PropertyRender::RendarebleHlmsTypeMask && "Too many material / meshes variations" );
        assert( (hash[1] >> N_PropertyRender::PassShift) <= N_PropertyRender::PassMask && "Should never happen (we assert in createState)" );

        finalHash = hash[0] | hash[1];

		RenderStateCache const * lastRe;
		
        if(statehash != finalHash)
        {
            lastRe = getCache(finalHash);

            if(!lastRe)
            {
                lastRe = createCache(hash[0], rsc, finalHash, item);
            }
        }
		else
		{
			lastRe = getCache(finalHash);
		}

        return lastRe;
    }
	//-----------------------------------------------------------------------------------
    RenderStateCache * RenderPattern::createState(ShadowType stype)
    {
		const ShadowFusionNode * node = mParent->getCurrentShadowNode();

        if(!mRenderSys->isReverseDepth())
            add(N_PropertyRender::NoReverseDepth, 1);

        if(stype != ST_Cast)
        {
            if( node )
            {
                int32 numPssmSplits = 0;
                const vector<Real>::type * pssmSplits = node->getPssmSplits( 0 );
                if( pssmSplits )
                    numPssmSplits = static_cast<int32>( pssmSplits->size() - 1 );
                add( N_PropertyRender::PssmSplits, numPssmSplits );

                bool isPssmBlend = false;
                const vector<Real>::type * pssmBlends = node->getPssmBlends( 0 );
                if( pssmBlends )
                    isPssmBlend = pssmBlends->size() > 0;
                add( N_PropertyRender::PssmBlend, isPssmBlend );

                bool isPssmFade = false;
                const Real * pssmFade = node->getPssmFade( 0 );
                if( pssmFade )
                    isPssmFade = * pssmFade != 0.0f;
                add( N_PropertyRender::PssmFade, isPssmFade );

                const TextureList & contiguousShadowMapTex = node->getContiguousShadowMapTex();

                size_t numShadowMapLights = node->getNumActiveShadowCastingLights();
                if( numPssmSplits )
                    numShadowMapLights += numPssmSplits - 1;
                add( N_PropertyRender::NumShadowMapLights, numShadowMapLights );
                add( N_PropertyRender::NumShadowMapTextures, contiguousShadowMapTex.size() );

                {
                    const Ogre::ShadowMetadata * shadowNodeDef = node->getDefinition();

                    char tmpBuffer[64];
                    LwString propName( LwString::FromEmptyPointer( tmpBuffer, sizeof(tmpBuffer) ) );

                    propName = "hlms_shadowmap";
                    const size_t basePropNameSize = propName.size();

                    size_t shadowMapTexIdx = 0;

                    for( size_t i = 0; i < numShadowMapLights; ++i )
                    {
                        //Skip inactive lights (e.g. no directional lights are available
                        //and there's a shadow map that only accepts dir lights)
                        while( !node->isShadowMapIdxActive( shadowMapTexIdx ) )
                            ++shadowMapTexIdx;

                        const Ogre::ShadowTextureDef * shadowTexDef = shadowNodeDef->getShadowTextureDefinition( shadowMapTexIdx );

                        propName.resize( basePropNameSize );
                        propName.a( (uint32)i ); //hlms_shadowmap0

                        const size_t basePropSize = propName.size();

                        add( propName.c_str(), node->getIndexToContiguousShadowMapTex( shadowMapTexIdx ) );

                        if( shadowTexDef->uvOffset != Vector2::ZERO || shadowTexDef->uvLength != Vector2::UNIT_SCALE )
                        {
                            propName.resize( basePropSize );
                            propName.a( "_uvs_fulltex" );
                            add( propName.c_str(), 1 );
                        }

                        float intPart, fractPart;

                        fractPart = modff( (float)shadowTexDef->uvOffset.x, &intPart );
                        propName.resize( basePropSize );
                        propName.a( "_uv_min_x_int" );
                        add( propName.c_str(), (int32)intPart );
                        propName.resize( basePropSize );
                        propName.a( "_uv_min_x_fract" );
                        add( propName.c_str(), (int32)(fractPart * 100000.0f) );

                        fractPart = modff( (float)shadowTexDef->uvOffset.y, &intPart );
                        propName.resize( basePropSize );
                        propName.a( "_uv_min_y_int" );
                        add( propName.c_str(), (int32)intPart );
                        propName.resize( basePropSize );
                        propName.a( "_uv_min_y_fract" );
                        add( propName.c_str(), (int32)(fractPart * 100000.0f) );

                        Vector2 uvMax = shadowTexDef->uvOffset + shadowTexDef->uvLength;
                        fractPart = modff( (float)uvMax.x, &intPart );
                        propName.resize( basePropSize );
                        propName.a( "_uv_max_x_int" );
                        add( propName.c_str(), (int32)intPart );
                        propName.resize( basePropSize );
                        propName.a( "_uv_max_x_fract" );
                        add( propName.c_str(), (int32)(fractPart * 100000.0f) );

                        fractPart = modff( (float)uvMax.y, &intPart );
                        propName.resize( basePropSize );
                        propName.a( "_uv_max_y_int" );
                        add( propName.c_str(), (int32)intPart );
                        propName.resize( basePropSize );
                        propName.a( "_uv_max_y_fract" );
                        add( propName.c_str(), (int32)(fractPart * 100000.0f) );

                        propName.resize( basePropSize );
                        propName.a( "_array_idx" );
                        add( propName.c_str(), shadowTexDef->arrayIdx );

                        const Light *light = node->getLightAssociatedWith( shadowMapTexIdx );
                        if( light->getType() == Light::LT_DIRECTIONAL )
                        {
                            propName.resize( basePropSize );
                            propName.a( "_is_directional_light" );
                            add( propName.c_str(), 1 );
                        }
                        else if( light->getType() == Light::LT_POINT )
                        {
                            propName.resize( basePropSize );
                            propName.a( "_is_point_light" );
                            add( propName.c_str(), 1 );

                            fractPart = modff( (float)shadowTexDef->uvLength.x, &intPart );
                            propName.resize( basePropSize );
                            propName.a( "_uv_length_x_int" );
                            add( propName.c_str(), (int32)intPart );
                            propName.resize( basePropSize );
                            propName.a( "_uv_length_x_fract" );
                            add( propName.c_str(), (int32)(fractPart * 100000.0f) );

                            fractPart = modff( (float)shadowTexDef->uvLength.y, &intPart );
                            propName.resize( basePropSize );
                            propName.a( "_uv_length_y_int" );
                            add( propName.c_str(), (int32)intPart );
                            propName.resize( basePropSize );
                            propName.a( "_uv_length_y_fract" );
                            add( propName.c_str(), (int32)(fractPart * 100000.0f) );
                        }

                        ++shadowMapTexIdx;
                    }
                }

                int usesDepthTextures = -1;

                const size_t numShadowMapTextures = contiguousShadowMapTex.size();
                for( size_t i = 0; i < numShadowMapTextures; ++i )
                {
                    bool missmatch = false;

                    if( PixelUtil::isDepth( contiguousShadowMapTex[i]->getFormat() ) )
                    {
                        missmatch = usesDepthTextures == 0;
                        usesDepthTextures = 1;
                    }
                    else
                    {
                        missmatch = usesDepthTextures == 1;
                        usesDepthTextures = 0;
                    }

                    if( missmatch )
                    {
                        N_EXCEPT( Exception::ERR_NOT_IMPLEMENTED, "Mixing depth textures with non-depth textures for "
                        "shadow mapping is not supported. Either all of " "them are depth textures, or none of them are.\n"
                        "Shadow Node: '" + node->getName().getFriendlyText() + "'");
                    }
                }

                if( usesDepthTextures == -1 )
                    usesDepthTextures = 0;

                add( N_PropertyRender::ShadowUsesDepthTexture, usesDepthTextures );
            }

            const FusionOp *pass = mParent->getCurrentCompositorPass();

            if( pass && pass->getType() == FOT_Space )
            {
                OGRE_ASSERT_HIGH( dynamic_cast<const SpaceFusionMetadata*>( pass->getDefinition() ) );
                const SpaceFusionMetadata * passSceneDef = static_cast<const SpaceFusionMetadata*>( pass->getDefinition() );
                if( passSceneDef->mUvBakingSet != 0xFF )
                {
                    add( N_PropertyRender::UseUvBaking, 1 );
                    add( N_PropertyRender::UvBaking, passSceneDef->mUvBakingSet );
                    if( passSceneDef->mBakeLightingOnly )
                        add( N_PropertyRender::BakeLightingOnly, 1 );
                }

                if( passSceneDef->mInstancedStereo )
                    add( N_PropertyRender::InstancedStereo, 1 );

                if( passSceneDef->mGenNormalsGBuf )
                    add( N_PropertyRender::GenNormalsGBuf, 1 );
            }

            ForwardPlusBase * forwardPlus = mParent->_getActivePassForwardPlus();
            if(forwardPlus)
                forwardPlus->setHlmsPassProperties( this );

            if(mShaderFileExt == _T(".glsl"))
            {
                //Actually the problem is not texture flipping, but origin. In D3D11,
                //we need to always flip because origin is different, but it's consistent
                //between texture and render window. In GL, RenderWindows don't need
                //to flip, but textures do.
                const FrameBufferObject *fbo = mRenderSys->getFBO();
                add( N_PropertyRender::ForwardPlusFlipY, fbo->isFlipping() );
            }

            uint numLightsPerType[Light::NUM_LIGHT_TYPES];
            int32 numAreaApproxLightsWithMask = 0;
            memset( numLightsPerType, 0, sizeof( numLightsPerType ) );

            uint shadowCasterDirectional = 0;

            if( mRenderingType == RT_TiledForward )
            {
                if( node )
                {
                    //Gather shadow casting lights, regardless of their type.
                    const LightClosestArray & lights = node->getShadowCastingLights();
                    LightClosestArray::const_iterator itor = lights.begin();
                    LightClosestArray::const_iterator end  = lights.end();
                    while( itor != end )
                    {
                        if( itor->light )
                        {
                            if( itor->light->getType() == Light::LT_DIRECTIONAL )
                                ++shadowCasterDirectional;
                            ++numLightsPerType[itor->light->getType()];
                        }
                        ++itor;
                    }
                }

                //Always gather directional & area lights.
                numLightsPerType[Light::LT_DIRECTIONAL] = 0;
                numLightsPerType[Light::LT_AREA_APPROX] = 0;
                numLightsPerType[Light::LT_AREA_LTC] = 0;
                {
                    mAreaLightIndex = std::numeric_limits<uint32>::max();
                    const LightListInfo & globalLightList = mParent->getGlobalLightList();
                    LightArray::const_iterator begin= globalLightList.lights.begin();
                    LightArray::const_iterator itor = begin;
                    LightArray::const_iterator end  = globalLightList.lights.end();

                    while( itor != end )
                    {
                        const Light::LightTypes lightType = (*itor)->getType();
                        if( lightType == Light::LT_DIRECTIONAL )
                            ++numLightsPerType[Light::LT_DIRECTIONAL];
                        else if( lightType == Light::LT_AREA_APPROX )
                        {
                            mAreaLightIndex = std::min<uint32>( mAreaLightIndex, itor - begin );
                            ++numLightsPerType[Light::LT_AREA_APPROX];
                            if( (*itor)->mTextureLightMaskIdx != std::numeric_limits<uint16>::max() )
                                ++numAreaApproxLightsWithMask;
                        }
                        else if( lightType == Light::LT_AREA_LTC )
                        {
                            mAreaLightIndex = std::min<uint32>( mAreaLightIndex, itor - begin );
                            ++numLightsPerType[Light::LT_AREA_LTC];
                        }
                        ++itor;
                    }
                }

                mCurrentDirectionalLightCnt = numLightsPerType[Light::LT_DIRECTIONAL];

                if( mLightCnt > 0 && numLightsPerType[Light::LT_DIRECTIONAL] > shadowCasterDirectional )
                {
                    numLightsPerType[Light::LT_DIRECTIONAL] = shadowCasterDirectional + mLightCnt;

                    add( N_PropertyRender::StaticBranchLights, 1 );
                }
            }
            else if( mRenderingType == RT_Forward )
            {
                if( node )
                {
                    //Gather shadow casting *directional* lights.
                    const LightClosestArray & lights = node->getShadowCastingLights();
                    LightClosestArray::const_iterator itor = lights.begin();
                    LightClosestArray::const_iterator end  = lights.end();
                    while( itor != end )
                    {
                        if( itor->light && itor->light->getType() == Light::LT_DIRECTIONAL )
                            ++shadowCasterDirectional;
                        ++itor;
                    }
                }

                //Gather all lights.
                const LightListInfo &globalLightList = mParent->getGlobalLightList();
                LightArray::const_iterator itor = globalLightList.lights.begin();
                LightArray::const_iterator end  = globalLightList.lights.end();

                size_t numTotalLights = 0;

                while( itor != end && numTotalLights < mLightCnt )
                {
                    ++numLightsPerType[(*itor)->getType()];
                    ++numTotalLights;
                    ++itor;
                }
            }

            add( N_PropertyRender::LightsAttenuation, numLightsPerType[Light::LT_POINT] + numLightsPerType[Light::LT_SPOTLIGHT] );
            add( N_PropertyRender::LightsSpotParams,  numLightsPerType[Light::LT_SPOTLIGHT] );


            numLightsPerType[Light::LT_POINT]       += numLightsPerType[Light::LT_DIRECTIONAL];
            numLightsPerType[Light::LT_SPOTLIGHT]   += numLightsPerType[Light::LT_POINT];

            //We need to limit the number of area lights before and after rounding
            {
                //Approx area lights
                numLightsPerType[Light::LT_AREA_APPROX] = std::min<uint16>( numLightsPerType[Light::LT_AREA_APPROX], mAreaLightApproxCnt );
                mCurrentAreaApproxLightCnt = numLightsPerType[Light::LT_AREA_APPROX];
                mCurrentAreaApproxLight2Cnt = numAreaApproxLightsWithMask;
                numLightsPerType[Light::LT_AREA_APPROX] = std::min<uint16>( numLightsPerType[Light::LT_AREA_APPROX], mAreaLightApproxCnt );
            }
            {
                //LTC area lights
                numLightsPerType[Light::LT_AREA_LTC] = std::min<uint16>( numLightsPerType[Light::LT_AREA_LTC], mAreaLightsLtcCnt );
                mCurrentAreaLtcLightCnt = numLightsPerType[Light::LT_AREA_LTC];
                numLightsPerType[Light::LT_AREA_LTC] = std::min<uint16>( numLightsPerType[Light::LT_AREA_LTC], mAreaLightsLtcCnt );
            }

            //The value is cummulative for each type (order: Directional, point, spot)
            add( N_PropertyRender::LightsDirectional, shadowCasterDirectional );
            add( N_PropertyRender::LightsDirNonCaster,numLightsPerType[Light::LT_DIRECTIONAL] );
            add( N_PropertyRender::LightsPoint,       numLightsPerType[Light::LT_POINT] );
            add( N_PropertyRender::LightsSpot,        numLightsPerType[Light::LT_SPOTLIGHT] );
            if( numLightsPerType[Light::LT_AREA_APPROX] > 0 )
                add( N_PropertyRender::LightsAreaApprox, mAreaLightApproxCnt );
            if( numLightsPerType[Light::LT_AREA_LTC] > 0 )
                add( N_PropertyRender::LightsAreaLtc, mAreaLightsLtcCnt );
            if( numAreaApproxLightsWithMask > 0 )
                add( N_PropertyRender::LightsAreaTexMask, 1 );
        }
        else
        {
            add( N_PropertyRender::ShadowCaster, 1 );

            const FusionOp * pass = mParent->getCurrentCompositorPass();
            if( pass )
            {
                const uint8 shadowMapIdx = pass->getDefinition()->mShadowMapIdx;
                const Light *light = node->getLightAssociatedWith( shadowMapIdx );
                if( light->getType() == Light::LT_DIRECTIONAL )
                    add( N_PropertyRender::ShadowCasterDirectional, 1 );
                else if( light->getType() == Light::LT_POINT )
                    add( N_PropertyRender::ShadowCasterPoint, 1 );
            }

            //add( N_PropertyRender::DualParaboloidMapping,   dualParaboloid );
            add( N_PropertyRender::Forward3D,               0 );
            add( N_PropertyRender::NumShadowMapLights,      0 );
            add( N_PropertyRender::NumShadowMapTextures,    0 );
            add( N_PropertyRender::PssmSplits,              0 );
            add( N_PropertyRender::LightsAttenuation,       0 );
            add( N_PropertyRender::LightsSpotParams,        0 );
            add( N_PropertyRender::LightsDirectional,       0 );
            add( N_PropertyRender::LightsDirNonCaster,      0 );
            add( N_PropertyRender::LightsPoint,             0 );
            add( N_PropertyRender::LightsSpot,              0 );
            add( N_PropertyRender::LightsAreaApprox,        0 );

            const FrameBufferObject * fbo = mRenderSys->getFBO();

            add(N_PropertyRender::ShadowUsesDepthTexture, (fbo->getColourCount() > 0) ? 0 : 1);
        }

        const Camera * camera = mParent->getCamerasInProgress().renderingCamera;
        if( camera && camera->isReflected() )
        {
            add( N_PropertyRender::PsoClipDistances, 1 );
            add( N_PropertyRender::GlobalClipPlanes, 1 );
        }

        const FrameBufferObject * fbo = mRenderSys->getFBO();
        add( N_PropertyRender::RenderDepthOnly, (fbo->getColourCount() > 0) ? 0 : 1 );

        if( mParent->getCurrentPrePassMode() == PrePassCreate )
        {
            add( N_PropertyRender::PrePass,         1 );
            add( N_PropertyRender::GenNormalsGBuf,  1 );
        }
        else if( mParent->getCurrentPrePassMode() == PrePassUse )
        {
            add( N_PropertyRender::UsePrePass,      1 );
            add( N_PropertyRender::VPos,            1 );

            add( N_PropertyRender::ScreenPosInt,    1 );

            {
                const TextureList & prePassTextures = mParent->getCurrentPrePassTextures();
                assert( !prePassTextures.empty() );
                if( prePassTextures[0]->isMultiSample() )
                {
                    add( N_PropertyRender::UsePrePassMsaa, prePassTextures[0]->getOriginSample().getMultiSampling() );
                }
            }

            if( mParent->getCurrentSsrTexture() != 0 )
                add( N_PropertyRender::UseSsr, 1 );
        }

        if( mParent->getCurrentRefractionsTexture() != 0 )
        {
            add( N_PropertyRender::VPos,                    1 );
            add( N_PropertyRender::ScreenPosInt,            1 );
            add( N_PropertyRender::SsRefractionsAvailable,  1 );
        }

        mListener->onCreateState(mParent, this, stype);

        Sample passCache;
        passCache.mSampleObject = mRenderSys->getSampleObject(mParent->getCurrentPrePassMode() == PrePassUse ? SB_DepthWriteDisable : 0);
        passCache.mPropertyList = mPropertys;

        assert( mSampleList.size() <= N_PropertyRender::PassMask && "Too many passes combinations, we'll overflow the bits assigned in the hash!" );

        SampleList::iterator it = std::find(mSampleList.begin(), mSampleList.end(), passCache);
        if(it == mSampleList.end())
        {
            mSampleList.push_back(passCache);
            it = mSampleList.end() - 1;
        }

        const uint32 hash = (it - mSampleList.begin()) << N_PropertyRender::PassShift;

        RenderStateCache re(hash, mType, RenderStateObject());
        re.mPropertyList = mPropertys;
        re.mRSO.mSampleObject = passCache.mSampleObject;

        return re;
    }
	//-----------------------------------------------------------------------------------
    void RenderPattern::compileProgramCode(const Object & obj, const StringList & srclist)
    {
        //OgreProfileExhaustive("RenderPattern::compileProgramCode");

        const uint32 finalHash = mType * 100000000u + static_cast<uint32>(mShaderList.size());

        ObjectShader shader(obj.mSegmentList);
        shader.mObject.mPropertyList = obj.mPropertyList;

        StringList::iterator i, iend = srclist.end();
        for(i = srclist.begin(); i != iend; ++i)
        {
            if(i->size())
            {
                String debugFilenameOutput;
                std::ofstream debugDumpFile;
                if( mDebug )
                {
                    debugFilenameOutput = mOutputPath + "./" + StrConv::conv( finalHash ) + ShaderFiles[c] + mShaderFileExt;
                    debugDumpFile.open( debugFilenameOutput.c_str(), std::ios::out | std::ios::binary );

                    if( mDebugProperty )
                    {
                        mPropertys.swap(shader.mObject.mPropertyList);
                        debugOutput(debugDumpFile);
                        mPropertys.swap(shader.mObject.mPropertyList);
                    }
                }

                shader.mProgramList.push_back(compile(*i, "", finalHash, static_cast<ShaderType>(c)));
            }
        }

        mShaderList.push_back(shader);
    }
    //-----------------------------------------------------------------------------------
    void RenderPattern::compile(ObjectShader & shader)
    {
        //OgreProfileExhaustive("RenderPattern::compile");

        //Give the shaders friendly base-10 names
        const uint32 finalHash = mType * 100000000u + static_cast<uint32>(mShaderList.size());

        mPropertys = shader.mObject.mPropertyList;

        {
            //Add RenderSys-specific properties
            IDList::const_iterator itor = mFeatureList.begin();
            IDList::const_iterator end  = mFeatureList.end();

            while( itor != end )
                add( *itor++, 1 );
        }

        //Generate the shaders
        for(size_t i = 0; i < ST_PCnt; ++i)
        {
            //Collect mSegmentList
            mSegmentList[i] = shader.mObject.mSegmentList[i];

            const String filename = ShaderFiles[i] + mShaderFileExt;
            if( mVFS->exists( filename ) )
            {
                if( mShaderProfile == "glsl" ) //TODO: String comparision
                {
                    add( N_PropertyRender::GL3Plus, mRenderSys->getShaderVersion() );
                }
                else if( mShaderProfile == "glsles" ) //TODO: String comparision
                {
                    add( N_PropertyRender::GLES, mRenderSys->getShaderVersion() );
                }

                add( N_PropertyRender::Syntax,  mShaderSyntax.mHash );
                add( N_PropertyRender::Hlsl,    N_PropertyRender::Hlsl.mHash );
                add( N_PropertyRender::Glsl,    N_PropertyRender::Glsl.mHash );
                add( N_PropertyRender::Glsles,  N_PropertyRender::Glsles.mHash );
                add( N_PropertyRender::Metal,   N_PropertyRender::Metal.mHash );

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                add( N_PropertyRender::iOS, 1 );
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
                add( N_PropertyRender::macOS, 1 );
#endif
                add( N_PropertyRender::HighQuality, mQualityType );

                //if( mFastShaderBuildHack )
                //    add( N_PropertyRender::FastShaderBuildHack, 1 );

                String debugFilenameOutput;
                std::ofstream debugDumpFile;
                if( mDebug )
                {
                    debugFilenameOutput = mOutputPath + "./" + StrConv::conv( finalHash ) + ShaderFiles[i] + mShaderFileExt;
                    debugDumpFile.open( debugFilenameOutput.c_str(), std::ios::out | std::ios::binary );

                    //We need to dump the properties before processing the files, as these
                    //may be overwritten or polluted by the files, thus hiding why we
                    //got this permutation.
                    if( mDebugProperty )
                        debugOutput( debugDumpFile );
                }

                //Library piece files first
                LibraryList::const_iterator itor = mLibrary.begin();
                LibraryList::const_iterator end  = mLibrary.end();

                while(itor != end)
                {
                    genSegment(itor->mVFS, itor->mSegment[i]);
                    ++itor;
                }

                //Main piece files
                genSegment(mVFS, mSegment[i]);

                //Generate the shader file.
                ScopeDataStream inFile(mVFS->open(filename));

                String inString;
                String outString;

                inString.resize( inFile->size() );
                inFile->read( &inString[0], inFile->size() );

                bool syntaxError = false;

                syntaxError |= genMath( inString, outString );
                while( !syntaxError && outString.find( "@foreach" ) != String::npos )
                {
                    syntaxError |= genFor( outString, inString );
                    inString.swap( outString );
                }
                syntaxError |= genProperty( outString, inString );
                syntaxError |= genUndef( inString, outString );
                while( !syntaxError  && (outString.find( "@piece" ) != String::npos || outString.find( "@insertpiece" ) != String::npos) )
                {
                    syntaxError |= genDefine( outString, inString );
                    syntaxError |= genRef( inString, outString );
                }
                syntaxError |= genPropertyMath( outString, inString );

                outString.swap( inString );

                if( syntaxError )
                {
                    N_log( "There were HLMS syntax errors while parsing " + StrConv::conv( finalHash ) + ShaderFiles[i] );
                }

                //Now dump the processed file.
                if( mDebug )
                    debugDumpFile.write( &outString[0], outString.size() );

                //Don't create and compile if template requested not to
                if(!get(N_PropertyRender::DisableStage))
                {
                    shader.mProgramList.push_back(compile(outString, debugFilenameOutput, finalHash, static_cast<ShaderType>(i)));
                }

                //Reset the disable flag.
                add(N_PropertyRender::DisableStage, 0);
            }
        }

        mShaderList.push_back(shader);
    }
    //-----------------------------------------------------------------------------------
    const RenderPattern::Object * RenderPattern::getObject(HashType objHash) const
    {
        return &mObjectList[(objHash >> N_PropertyRender::RenderableShift) & N_PropertyRender::RenderableMask];
    }
    //-----------------------------------------------------------------------------------
    const RenderStateCache * RenderPattern::getCache(HashType stateHash) const
    {
        RenderStateCache cache(stateHash, mType, RenderStateObject());
        RenderStateCacheList::const_iterator it = std::lower_bound(mShaderCache.begin(), mShaderCache.end(), &cache, OrderCacheByHash);

        if(it != mShaderCache.end() && (*it)->mHash == stateHash)
            return *it;

        return 0;
    }
    //------------------------------------------------------------------------
    void RenderPattern::clearCache()
    {
        mSampleList.clear();

        //Empty mShaderCache so that mRenderSys->destroyStencil would
        //be harmless even if _notifyMacroblockDestroyed gets called.
        RenderStateCacheList cache;
        cache.swap(mShaderCache);
        RenderStateCacheList::const_iterator itor = cache.begin();
        RenderStateCacheList::const_iterator end  = cache.end();

        while(itor != end)
        {
            mRenderSys->destroyRSO(&(*itor)->mRSO);
            if((*itor)->mRSO.mRenderMark)
                mRenderSys->destroyStencil((*itor)->mRSO.mStencil);

            delete *itor;
            ++itor;
        }

        //cache.clear();

        mShaderList.clear();
    }
	//------------------------------------------------------------------------
    /*void RenderPattern::generateFor()
    {
        uint16 numWorldTransforms = 1;
        bool castShadows          = true;

        *//*std::ifstream inFile( "E:/Projects/RenderPattern/bin/RenderPattern/PBS/GLSL/VertexShader_vs.glsl",
                              std::ios::in | std::ios::binary );
        std::ofstream outFile( "E:/Projects/RenderPattern/bin/RenderPattern/PBS/GLSL/Output_vs.glsl",
                               std::ios::out | std::ios::binary );*//*
        std::ifstream inFile( "E:/Projects/RenderPattern/bin/RenderPattern/PBS/GLSL/PixelShader_ps.glsl",
                                      std::ios::in | std::ios::binary );
        std::ofstream outFile( "E:/Projects/RenderPattern/bin/RenderPattern/PBS/GLSL/Output_ps.glsl",
                               std::ios::out | std::ios::binary );

        String inString;
        String outString;

        inFile.seekg( 0, std::ios::end );
        inString.resize( inFile.tellg() );
        inFile.seekg( 0, std::ios::beg );

        inFile.read( &inString[0], inString.size() );

        uint16 numWorldTransforms = 2;
        //bool castShadows          = true;

        add( N_PropertyRender::Skeleton, numWorldTransforms > 1 );
        add( N_PropertyRender::UvCount, 2 );
        add( "true", 1 );
        add( "false", 0 );

        add( N_PropertyRender::DualParaboloidMapping, 0 );

        add( N_PropertyRender::Normal, 1 );

        add( N_PropertyRender::UvCount0, 2 );
        add( N_PropertyRender::UvCount1, 4 );
        add( N_PropertyRender::BonesPerVertex, 4 );

        add( N_PropertyRender::PssmSplits, 3 );
        add( N_PropertyRender::PssmBlend, 1 );
        add( N_PropertyRender::PssmFade, 1 );
        add( N_PropertyRender::ShadowCaster, 0 );

        add( N_PropertyRender::LightsDirectional, 1 );
        add( N_PropertyRender::LightsDirNonCaster, 1 );
        add( N_PropertyRender::LightsPoint, 2 );
        add( N_PropertyRender::LightsSpot, 3 );

        add( N_PropertyRender::Pose, 0 );
        add( N_PropertyRender::PoseHalfPrecision, 0 );
        add( N_PropertyRender::PoseNormals, 0 );
        
        //parse( inString, outString );
        genFor( inString, outString );
        genProperty( outString, inString );
        genDefine( inString, outString );
        genRef( outString, inString );
        genPropertyMath( inString, outString );

        outFile.write( &outString[0], outString.size() );
    }*/
	//------------------------------------------------------------------------
}