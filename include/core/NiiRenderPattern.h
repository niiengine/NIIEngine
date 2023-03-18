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

#ifndef _NII_RENDERPATTERN_H_
#define _NII_RENDERPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiPattern.h"
#include "NiiShaderCh.h"
#include "NiiPropertyObj.h"

namespace NII
{
    /***/
    enum RenderPatternType
    {
        RPT_Single = N_CodeGen_Render + 1,
        RPT_Normal = N_CodeGen_Render + 2,
        RPT_Overlay = N_CodeGen_Render + 3,
        RPT_Toon = N_CodeGen_Render + 4,
        RPT_Crayon = N_CodeGen_Render + 5,
        RPT_VanGogh = N_CodeGen_Render + 6,
        RPT_MidSumNight = N_CodeGen_Render + 7,
        
        RPT_Custom1 = N_CodeGen_Render + 8,
        RPT_Custom2 = N_CodeGen_Render + 9,
        RPT_Custom3 = N_CodeGen_Render + 10,
        RPT_Custom4 = N_CodeGen_Render + 11,
        RPT_Custom5 = N_CodeGen_Render + 12,
        RPT_Custom6 = N_CodeGen_Render + 13,
        RPT_Custom7 = N_CodeGen_Render + 14,
        RPT_Custom8 = N_CodeGen_Render + 15,
        RPT_Custom9 = N_CodeGen_Render + 16,
        
        RPT_Count = N_CodeGen_Render + 17
    };
    
    struct RenderStateCache
    {
        RenderStateCache() : mHash(0), mPattern(0) {}
        RenderStateCache(const RenderStateObject & rso, RenderPattern * pattern, Nui32 hash) : 
            mRSO(rso), mPattern(pattern), mHash(hash) {}
        
        RenderStateObject mRSO;
        PropertyValueList mPropertyList;
        RenderPattern * mPattern;
        Nui32 mHash;
    };

    typedef vector<RenderStateCache*>::type RenderStateCacheList;
    
    class ShadowRenderTest;

    /** ��Ӱ��������,�����Ժ��ϸ���Ż����޸�
    @remark
        ��Ӱ��������Ⱦ�����ǱȽϸ��ӵ�,�������ܶ�Ч������,�е�ʱ����ῴ��������Ϸ
        ��Ӱ�Ӷ����Ե���,�ƹ�����Խ���������ӰԽ��,Ч��Խ��,��Ӱʵ�ʾ��ǵƹ������
        �Ĺ���
    @note ����ʹ������Ĭ���ṩ����Ӱ������
    @par ���¼�������Ż���ӰЧ��
        (1)���һ����Ӱ��ȫ����һ������,ֱ�Ӱ���ӰͶ��ͨ�����뵽������Ⱦͨ����
        (2)����ƹ��Ǿ�̬,�������㹻Զ,�������ֻҪ����һ����Ӱ��
        (3)���������ڵƹ�(��̬,�㹻Զ)��,������Ӱ���ʹ�� PlanarShadow
        (4)����ƹ���,��������ȴ���32������ı�Ե,Ȼ��ͨ���ƹ�λ��ѡ����ӽ�����ı�Ե
        (5)�������Ͳ�Ҫ��Ӱ����
    @version NIIEngine 3.0.0 ����api
    */
    class _EngineAPI RenderPattern : public Pattern, public GpuProgramCodeGen
    {
        friend class SpaceManager;
    public:
        /** �ⲿ����
        @version NIIEngine 3.0.0
        */
        class Listener
        {
        public:
			enum ShadowType
			{
				ST_Normal,
				ST_Cast,
				ST_Receive
			};
		public:
            Listener();
            virtual ~Listener();

            /** Ѱ�ҿɼ�/��ӰͶ��/��Ӱ���ܼ���
            @version NIIEngine 6.0.0
            */
            virtual void onFindGeometry(SpaceManager * sm, Viewport * v, ShadowType type);
        };

        typedef vector<Listener *>::type Listeners;
		
        struct Material
        {
            Material() : mMaterial(0), mManager(false) {}
            Material(ShaderChMaterial * mat, bool mag, const String & src, GroupID gid) :
                mMaterial(mat), 
                mManager(mag), 
                mSrc(src), 
                mGroup(gid) {}

            ShaderChMaterial * mMaterial;
            String mSrc;
            GroupID mGroup;
            bool mManager;
        };

        typedef map<Nid, Material>::type MaterialList;
        typedef vector<std::pair<VString, Nui32> >::type TextureBindList;
        typedef vector<TextureBindList>::type TextureBindGroupList;
    public :
        RenderPattern(SpaceManager * sm, RenderSys * rsys, SysSyncParam * param);
        virtual ~RenderPattern();
		
        /**
        @version NIIEngine 6.0.0
        */
        RenderPatternType getType() const                   { return mType; }

        /**
        @version NIIEngine 6.0.0
        */
        Nid getID() const                                   { return mID; }

        /**
        @version NIIEngine 6.0.0
        */
        void setName(const String & str)                    { mName = str;}

        /**
        @version NIIEngine 6.0.0
        */
        const String & getName() const                      { return mName; }

        /** ��ʼ��
        @version NIIEngine 3.0.0
        */
        virtual void init();

        /** ��ǰϵͳ��/Ӳ��֧���Ƿ�֧�������Ӱ����
        @version NIIEngine 3.0.0
        */
        virtual bool isValid();

        /** ����ר�ö���
        @version NIIEngine 3.0.0
        */
        virtual RenderQueue * createQueue() const;

        /** �ָ���Ⱦ���еĽṹ
        @param[in] queue ��Ⱦ����
        @version NIIEngine 3.0.0
        */
        virtual void prepare(RenderQueue * queue) = 0;

        /** �ָ���Ⱦ���еĽṹ
        @param[in] group ��Ⱦ��
        @version NIIEngine 3.0.0
        */
        virtual void prepare(RenderGroup * group) = 0;

        /** ��Ⱦ����Ⱥ
        @param[in] grp ��Ҫ��Ⱦ�ļ�����
        @version NIIEngine 3.0.0
        */
        virtual void render(RenderGroup * grp);

        /** ��Ⱦ��Ӱ
        @remark ׼����Ⱦ��Ӱ
        @note ������Ӱ����Ҫ,�ɰ���Ӱ���ҪԤ�����ɻ�ժѡ��ԵҲ��Ҫ�������
        @version NIIEngine 3.0.0
        */
        virtual void renderShadow(Camera * cam, const Viewport * vp, const LightList & lights);

        /** Ӧ������
        @param[in] ch ʹ�õ�ͨ·
        @version NIIEngine 3.0.0
        */
        virtual void applyTexture(const ShaderCh * ch);

        /** Ӧ�õƹ�
        @remark ����Ӧ��
        @param[in] ch ������Ⱦͨ��
        @param[in] src Դ�ƹ�
        @param[in] oft Դ�ƹ⿪ʼ����,ָ src �����Ŀ�ʼ
        @param[in] app Ŀ��ƹ�(ʵ��Ӧ��)
        @note �����޸� src �������,�����Ҫ�޸� dst ���鷵�س�Ա����(src����)
        @version NIIEngine 3.0.0
        */
        virtual void applyLight(const ShaderCh * ch, const LightList & src, NCount & oft, LightList & app);
		
        /// @copydetails GpuProgramCodeGen::setup
        virtual void setup(const VFSList & vfsl);

        /// @copydetails GpuProgramCodeGen::setQuality
        virtual void setQuality(QualityType set);

        /// @copydetails GpuProgramCodeGen::setRender
        virtual void setRender(RenderSys * rsys);

        /**
        @version NIIEngine 6.0.0
        */
        void setDirectionalLight(NCounts max)               { mLightCnt = max; }

        /**
        @version NIIEngine 6.0.0
        */
        NCounts getDirectionalLight() const                 { return mLightCnt; }

        /** 
        @version NIIEngine 6.0.0
        */
        void setAreaLightForward(NCounts max, NCounts ltcMax);

        /** 
        @version NIIEngine 6.0.0
        */
        NCounts getAreaLightApprox() const		            { return mAreaLightApproxCnt; }

        /** 
        @version NIIEngine 6.0.0
        */
        NCounts getAreaLightsLtc() const			        { return mAreaLightsLtcCnt; }
        
        /** 
        @version NIIEngine 6.0.0
        */
        virtual void addObject(GeometryObj * obj);
		
        /** 
        @version NIIEngine 6.0.0
        */
        ShaderChMaterial * createMaterial(Nid id, const ShaderChStencil & stencil, const ShaderChBlend & blend, 
            const StringIdMap & strlist, bool autoDestroy = true, const String & filename = N_StrBlank, GroupID gid = GroupUnknow);

        /**
        @version NIIEngine 6.0.0
        */
        ShaderChMaterial * getMaterial(Nid id) const;

        /**
        @version NIIEngine 6.0.0
        */
        void destroyMaterial(Nid id);

        /**
        @version NIIEngine 6.0.0
        */
        void destroyAllMaterial();

        /**
        @version NIIEngine 6.0.0
        */
        const MaterialList & getMaterialList() const        { return mMaterialList; }

        /**
        @version NIIEngine 6.0.0
        */
        ShaderChMaterial * getMaterial() const              { return mMaterial; }

        /** 
        @version NIIEngine 6.0.0
        */
        const RenderStateCache * getCache(Nui32 stateHash, const RenderStateCache & inState, const RenderQueueItem & inItem, bool casterShadow);

        /** 
        @version NIIEngine 6.0.0
        */
        virtual RenderStateCache * createState(bool casterShadow);

        /** 
        @version NIIEngine 6.0.0
        */
        virtual NCount queue(DrawCallGroup * dcg, const RenderStateCache * cache, const RenderQueueItem & item, bool casterShadow, Nui32 objhash){}

        /**
        @version NIIEngine 6.0.0
        */
        virtual void frameBegin(){}

        /**
        @version NIIEngine 6.0.0
        */
        virtual void frameEnded() {}

        /**
        @version NIIEngine 6.0.0
        */
        void compileProgramCode(const Object & obj, const StringList & srclist);

        /**
        @version NIIEngine 6.0.0
        */
        void compile(ObjectShader & shader);

        /**
        @version NIIEngine 6.0.0
        */
        const ShaderList & getShaderList() const    		{ return mShaderList; }

        /** ��������������ģʽ
        @param[in] ch ʹ�õ�ͨ·
        @version NIIEngine 3.0.0
        */
        virtual CullingMode setCullingMode(CullingMode mode);

        /** ��ȡ����������ģʽ
        @version NIIEngine 3.0.0
        */
        virtual CullingMode getCullingMode() const;

        /** ������Ⱦ�ƹ�
        @remark ������Ӱʱ,�ȶԵƹ�����
        @param[in][out] dst ��Ч�ƹ�
        @version NIIEngine 3.0.0
        */
        virtual void sortLight(LightList & dst);

        /** ȷ����Ч�ƹ�
        @param[in][out] dest �ƹ�
        @version NIIEngine 3.0.0
        */
        virtual void findLight(LightList & dst);

        /** �Ƶ���Ӱ��Ⱦͨ·
        @remark ����Ⱦ�����׶�ת��ͨ·
        @param[in] dst Ŀ��ͨ·
        @param[in] src Դͨ·
        @version NIIEngine 3.0.0
        */
        virtual void deriveShadow(const ShaderCh *& dst, const ShaderCh * src);

        /** �Ƿ�ͨ����Ⱦ����
        @param[in] ch ��ǰӦ�õ�ͨ��
        @param[in] obj ��ǰ��Ⱦ�Ķ���
        @version NIIEngine 3.0.0
        */
        virtual bool testRender(const ShaderCh * ch, const GeometryObj * obj);

        /** �Ƿ�ͨ����Ⱦ����
        @param[in] ch ��ǰӦ�õ�ͨ��
        */
        virtual bool testRender(const ShaderCh * ch);

        /** �Ƿ�����Ⱦ��Ӱ�׶�
        @version NIIEngine 3.0.0
        */
        virtual bool isRenderShadow() const;

        /** �Ƿ�����������Ӱ
        @version NIIEngine 3.0.0
        */
        virtual bool isTextureShadow() const;

        /** �Ƿ�����ģ����Ӱ
        @version NIIEngine 3.0.0
        */
        virtual bool isStencilShadow() const;

        /** ������Ӱ��ɫ.
        @remark �����ʹ�ø�����ȥ������Ӱ,Խ��Խ��,���Բ�����,�����ʵ����ͻ�����Ӱ
        @version NIIEngine 3.0.0
        */
        virtual void setShadowColour(const Colour & c);

        /** ��ȡ��Ӱ��ɫ.
        @remark �����ʹ�ø�����ȥ������Ӱ,Խ��Խ��,���Բ�����,�����ʵ����ͻ�����Ӱ
        @version NIIEngine 3.0.0
        */
        virtual const Colour & getShadowColour() const;

        /** ������ӰͶ���Ƿ�ʹ�ú����,������������.
        @version NIIEngine 3.0.0
        */
        void setBackFaceInCast(bool b)          { mBackFaceInCast = b; }

        /** �����ӰͶ���Ƿ�ʹ�ú����,������������.
        @version nIIEngine 3.0.0
        */
        bool isBackFaceInCast() const           { return mBackFaceInCast;}

        /** �Ƿ�����Զ�ü���
        @remark �ɰ���Ӱר��
        @version NIIEngine 3.0.0
        */
        void setInfiniteFar(bool b)             { mInfiniteFar = b; }

        /** �Ƿ�����Զ�ü���
        @remark �ɰ���Ӱר��
        @version NIIEngine 3.0.0
        */
        bool isInfiniteFar()const               { return mInfiniteFar; }

        /** �����Ƿ���еƹ���ü�.
        @version NIIEngine 3.0.0
        */
        void setLightClipPlane(bool b)          { mLightClipPlane = b; }

        /** ��ȡ�Ƿ���еƹ���ü�.
        @version NIIEngine 3.0.0
        */
        bool isLightClipPlane() const           { return mLightClipPlane; }

        /** �����Ƿ��ֹ��Ӱ
        @version NIIEngine 3.0.0
        */
        void setShadowEnable(bool b)            { mShadowEnable = b; }

        /** ��ȡ�Ƿ��ֹ��Ӱ
        @version NIIEngine 3.0.0
        */
        bool isShadowEnable() const             { return mShadowEnable; }

        /** ������Ӱ�����쳤��
        @param[in] ext ���쳤��
        @version NIIEngine 3.0.0
        */
        void setShadowExtent(NIIf ext)          { mShadowExtent = dist; }

        /** ��ȡ��Ӱ�����쳤��
        @version NIIEngine 3.0.0
        */
        NIIf getShadowExtent() const            { return mShadowExtent; }

        /** ������Ӱ������쳤��
        @param[in] ext ������쳤��
        @version NIIEngine 3.0.0
        */
        void setShadowExtentMax(NIIf ext)       { mShadowExtentMax = ext; }

        /** ��ȡ��Ӱ������쳤��
        @version NIIEngine 3.0.0
        */
        NIIf getShadowExtentMax() const         { return mShadowExtentMax; }

        /** ��Ӽ���
        @version NIIEngine 3.0.0
        */
        void add(Listener * s);

        /** ��ȥ����
        @version NIIEngine 3.0.0
        */
        void remove(Listener * s);
		
        /** ��ȥ����
        @version NIIEngine 3.0.0
        */
        const Listeners & getListeners() const	{ return mListeners; }
    protected:
        struct Object
        {
            GeometryObj * mObj;
            SegmentRefGroupList mSegmentGroupList;
            PropertyValueList mPropertyList;

            Object(GeometryObj * obj, const PropertyValueList & pvlist, const SegmentRefGroupList & slist) :
                mPropertyList(pvlist),
                mSegmentGroupList(slist),
                mObj(obj),
            {
            }

            bool operator == (const Object & o) const
            {
                NCount iend = mSegmentGroupList.size();
                if(iend != o.mSegmentGroupList[i].size())
                    return false;
                for(i = 0; i < iend; ++i)
                {
                    if(o.mSegmentGroupList[i] != mSegmentGroupList[i])
                        return false;
                }
                return mPropertyList == o.mPropertyList;
            }
        };
        typedef vector<Object>::type ObjectList;

        struct ObjectShader
        {
            Object mObject;
            GpuProgramList mProgramList;

            ObjectShader(const SegmentRefGroupList & slist) : 
                mObject(PropertyValueList(), slist){}

            bool operator == (const ObjectShader & o) const
            {
                return mObject == o.mObject;
            }
        };
        typedef vector<ObjectShader>::type ShaderList;

        struct Sample
        {
            SampleObject mSampleObject;
            PropertyValueList mPropertyList;

            bool operator == (const Sample & o) const
            {
                return mPropertyList == o.mPropertyList && mSampleObject == o.mSampleObject;
            }
        };
        typedef vector<Sample>::type SampleList;
    protected:
        /** Ӧ����Ⱦϵͳ
        @version NIIEngine 3.0.0
        */
        void applyRender(RenderSys * sys);

        /** ��Ӱͨ·
        @version NIIEngine ����api
        */
        virtual void renderImpl(RenderGroup * rg) = 0;

        /** ����ͨ·
        @version NIIEngine ����api
        */
        virtual void renderBasic(RenderGroup * rg);

        /** ���ҿ�������ʱ����
        @version NIIEngine 3.0.0
        */
        void onFindGeometry(Viewport * v, Listener::ShadowType type);
		
        /** 
        @version NIIEngine 6.0.0
        */
        virtual RenderStateCache * createCache(Nui32 objHash, const RenderStateCache & rsc, Nmark mark, const RenderQueueItem & item);

        /** 
        @version NIIEngine 6.0.0
        */
        virtual ShaderChMaterial * createMaterialImpl(Nid id, const ShaderChStencil * stencil = 0, const ShaderChBlend * blend = 0, const StringIdMap & idlist = StringIdMap()){ return 0;}
		
        /**
        @version NIIEngine 6.0.0
        */
        virtual void onGenProperty();

        /** 
        @version NIIEngine 6.0.0
        */
        virtual void onAddObject(GeometryObj * obj, const SegmentRefGroupList & in, Listener::ShadowType type) {}

        /**
        @version NIIEngine 6.0.0
        */
        void bindTexture(ShaderType type, const VString & name, int32 texUnit);
        
        /**
        @version NIIEngine 6.0.0
        */
        void unbindTexture(ShaderType type, const VString & name, int32 texUnit);

        /** 
        @version NIIEngine 6.0.0
        */
        Nmark addObject(GeometryObj * obj, const PropertyValueList & pvlist, const SegmentRefGroupList & slist);

        /**
        @version NIIEngine 6.0.0
        */
        const Object * getObject(Nui32 objHash) const;
        
        /**
        @version NIIEngine 6.0.0
        */
        const RenderStateCache * getCache(Nui32 stateHash) const;
        
        /** 
        @version NIIEngine 6.0.0
        */
        virtual void clearCache();
    protected:
        Nid mID;
        String mName;						///< ����
        RenderPatternType mType;
        SpaceManager * mParent;             ///< ��ǰ����
        RenderSys * mRenderSys;             ///< ��ǰ��Ⱦϵͳ
        RenderQueue * mRenderQueue;         ///< ��ǰ��Ⱦ��
        Camera * mCamera;                   ///< ��ǰ�����
        const Viewport * mViewPort;         ///< ��ǰ�ӿ�
        CullingMode mCullingMode;           ///< ��ǰ��ѡģʽ
        SysSyncParam * mSyncParams;         ///< ��Ⱦ�Զ�����
        ShadowRenderTest * mTest;           ///< ��Ӱ������
        ShaderCh * mCast;
        ShaderCh * mReceive;
        ResourceID mCastRID;
        ResourceID mReceiveRID;
        Colour mShadowColour;
        NIIf mShadowExtent;                 ///< ��Ӱ����
        NIIf mShadowExtentMax;
        Nui32 mLightHash;                   ///< ��ǰ�ƹ��ϣ
        Listeners mListeners;               ///< �����б�
        ShaderChMaterial * mMaterial;
        RenderStateCacheList mShaderCache;
        MaterialList mMaterialList;
        SampleList mSampleList;
        ObjectList mObjectList;
        ShaderList mShaderList;
        TextureBindGroupList mTextureBindGroupList;
        SegmentRefGroupListList mSegmentGroupList;
        NCounts mLightCnt;
        NCounts mAreaLightApproxCnt;
        NCounts mAreaLightsLtcCnt;
        NCounts mAreaLightIndex;
        NCounts mCurrentDirectionalLightCnt;
		NCounts mCurrentAreaApproxLightCnt;
        NCounts mCurrentAreaApproxLight2Cnt;
        NCounts mCurrentAreaLtcLightCnt;
        bool mInit;
        bool mShadowEnable;
        bool mBackFaceInCast;
        bool mInfiniteFar;                  ///< ����Զ�Ľ���
        bool mLightClipPlane;               ///< �ƹ���ü�
        static LightList mNullLight;
    };
}
#endif