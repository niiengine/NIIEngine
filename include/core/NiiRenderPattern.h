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

    /** 阴影处理器类,便于以后的细节优化和修改
    @remark
        阴影处理在渲染中算是比较复杂的,将带来很多效率问题,有的时候你会看到网络游戏
        把影子都忽略掉了,灯光数量越多产生的阴影越多,效率越慢,阴影实际就是灯光和物体
        的故事
    @note 建议使用引擎默认提供的阴影处理器
    @par 以下几点可以优化阴影效率
        (1)如果一个阴影完全覆盖一个物体,直接把阴影投射通道加入到物体渲染通道中
        (2)如果灯光是静态,而且是足够远,物体仅仅只要创建一次阴影体
        (3)大型物体在灯光(静态,足够远)下,它的阴影最好使用 PlanarShadow
        (4)方向灯光下,物体可以先创建32个方向的边缘,然后通过灯光位置选择最接近结果的边缘
        (5)觉得慢就不要阴影算了
    @version NIIEngine 3.0.0 顶级api
    */
    class _EngineAPI RenderPattern : public Pattern, public GpuProgramCodeGen
    {
        friend class SpaceManager;
    public:
        /** 外部控制
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

            /** 寻找可见/阴影投射/阴影接受几何
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

        /** 初始化
        @version NIIEngine 3.0.0
        */
        virtual void init();

        /** 当前系统软/硬件支持是否支持这个阴影处理
        @version NIIEngine 3.0.0
        */
        virtual bool isValid();

        /** 创建专用队列
        @version NIIEngine 3.0.0
        */
        virtual RenderQueue * createQueue() const;

        /** 分割渲染队列的结构
        @param[in] queue 渲染队列
        @version NIIEngine 3.0.0
        */
        virtual void prepare(RenderQueue * queue) = 0;

        /** 分割渲染队列的结构
        @param[in] group 渲染组
        @version NIIEngine 3.0.0
        */
        virtual void prepare(RenderGroup * group) = 0;

        /** 渲染队列群
        @param[in] grp 需要渲染的几何组
        @version NIIEngine 3.0.0
        */
        virtual void render(RenderGroup * grp);

        /** 渲染阴影
        @remark 准备渲染阴影
        @note 纹理阴影特需要,蒙板阴影如果要预先生成或摘选边缘也需要这个函数
        @version NIIEngine 3.0.0
        */
        virtual void renderShadow(Camera * cam, const Viewport * vp, const LightList & lights);

        /** 应用纹理
        @param[in] ch 使用的通路
        @version NIIEngine 3.0.0
        */
        virtual void applyTexture(const ShaderCh * ch);

        /** 应用灯光
        @remark 用于应用
        @param[in] ch 传入渲染通道
        @param[in] src 源灯光
        @param[in] oft 源灯光开始索引,指 src 参数的开始
        @param[in] app 目标灯光(实际应用)
        @note 不能修改 src 里的设置,如果需要修改 dst 建议返回成员变量(src副本)
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

        /** 设置三角形面序模式
        @param[in] ch 使用的通路
        @version NIIEngine 3.0.0
        */
        virtual CullingMode setCullingMode(CullingMode mode);

        /** 获取三角形面序模式
        @version NIIEngine 3.0.0
        */
        virtual CullingMode getCullingMode() const;

        /** 排序渲染灯光
        @remark 建立阴影时,先对灯光排序
        @param[in][out] dst 有效灯光
        @version NIIEngine 3.0.0
        */
        virtual void sortLight(LightList & dst);

        /** 确立有效灯光
        @param[in][out] dest 灯光
        @version NIIEngine 3.0.0
        */
        virtual void findLight(LightList & dst);

        /** 推导阴影渲染通路
        @remark 在渲染各个阶段转换通路
        @param[in] dst 目标通路
        @param[in] src 源通路
        @version NIIEngine 3.0.0
        */
        virtual void deriveShadow(const ShaderCh *& dst, const ShaderCh * src);

        /** 是否通过渲染测试
        @param[in] ch 当前应用的通道
        @param[in] obj 当前渲染的对象
        @version NIIEngine 3.0.0
        */
        virtual bool testRender(const ShaderCh * ch, const GeometryObj * obj);

        /** 是否通过渲染测试
        @param[in] ch 当前应用的通道
        */
        virtual bool testRender(const ShaderCh * ch);

        /** 是否处于渲染阴影阶段
        @version NIIEngine 3.0.0
        */
        virtual bool isRenderShadow() const;

        /** 是否主流纹理阴影
        @version NIIEngine 3.0.0
        */
        virtual bool isTextureShadow() const;

        /** 是否主流模版阴影
        @version NIIEngine 3.0.0
        */
        virtual bool isStencilShadow() const;

        /** 设置阴影颜色.
        @remark 如果是使用附加型去绘制阴影,越描越黑,所以不合适,仅合适调制型绘制阴影
        @version NIIEngine 3.0.0
        */
        virtual void setShadowColour(const Colour & c);

        /** 获取阴影颜色.
        @remark 如果是使用附加型去绘制阴影,越描越黑,所以不合适,仅合适调制型绘制阴影
        @version NIIEngine 3.0.0
        */
        virtual const Colour & getShadowColour() const;

        /** 设置阴影投射是否使用后表面,而不是正表面.
        @version NIIEngine 3.0.0
        */
        void setBackFaceInCast(bool b)          { mBackFaceInCast = b; }

        /** 获得阴影投射是否使用后表面,而不是正表面.
        @version nIIEngine 3.0.0
        */
        bool isBackFaceInCast() const           { return mBackFaceInCast;}

        /** 是否无限远裁剪面
        @remark 蒙板阴影专用
        @version NIIEngine 3.0.0
        */
        void setInfiniteFar(bool b)             { mInfiniteFar = b; }

        /** 是否无限远裁剪面
        @remark 蒙板阴影专用
        @version NIIEngine 3.0.0
        */
        bool isInfiniteFar()const               { return mInfiniteFar; }

        /** 设置是否进行灯光面裁剪.
        @version NIIEngine 3.0.0
        */
        void setLightClipPlane(bool b)          { mLightClipPlane = b; }

        /** 获取是否进行灯光面裁剪.
        @version NIIEngine 3.0.0
        */
        bool isLightClipPlane() const           { return mLightClipPlane; }

        /** 设置是否禁止阴影
        @version NIIEngine 3.0.0
        */
        void setShadowEnable(bool b)            { mShadowEnable = b; }

        /** 获取是否禁止阴影
        @version NIIEngine 3.0.0
        */
        bool isShadowEnable() const             { return mShadowEnable; }

        /** 设置阴影的延伸长度
        @param[in] ext 延伸长度
        @version NIIEngine 3.0.0
        */
        void setShadowExtent(NIIf ext)          { mShadowExtent = dist; }

        /** 获取阴影的延伸长度
        @version NIIEngine 3.0.0
        */
        NIIf getShadowExtent() const            { return mShadowExtent; }

        /** 设置阴影最大延伸长度
        @param[in] ext 最大延伸长度
        @version NIIEngine 3.0.0
        */
        void setShadowExtentMax(NIIf ext)       { mShadowExtentMax = ext; }

        /** 获取阴影最大延伸长度
        @version NIIEngine 3.0.0
        */
        NIIf getShadowExtentMax() const         { return mShadowExtentMax; }

        /** 添加监听
        @version NIIEngine 3.0.0
        */
        void add(Listener * s);

        /** 移去监听
        @version NIIEngine 3.0.0
        */
        void remove(Listener * s);
		
        /** 移去监听
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
        /** 应用渲染系统
        @version NIIEngine 3.0.0
        */
        void applyRender(RenderSys * sys);

        /** 阴影通路
        @version NIIEngine 顶级api
        */
        virtual void renderImpl(RenderGroup * rg) = 0;

        /** 基础通路
        @version NIIEngine 顶级api
        */
        virtual void renderBasic(RenderGroup * rg);

        /** 查找可视物体时触发
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
        String mName;						///< 辅助
        RenderPatternType mType;
        SpaceManager * mParent;             ///< 当前场景
        RenderSys * mRenderSys;             ///< 当前渲染系统
        RenderQueue * mRenderQueue;         ///< 当前渲染组
        Camera * mCamera;                   ///< 当前摄象机
        const Viewport * mViewPort;         ///< 当前视口
        CullingMode mCullingMode;           ///< 当前拣选模式
        SysSyncParam * mSyncParams;         ///< 渲染自动参数
        ShadowRenderTest * mTest;           ///< 阴影测试器
        ShaderCh * mCast;
        ShaderCh * mReceive;
        ResourceID mCastRID;
        ResourceID mReceiveRID;
        Colour mShadowColour;
        NIIf mShadowExtent;                 ///< 阴影长度
        NIIf mShadowExtentMax;
        Nui32 mLightHash;                   ///< 当前灯光哈希
        Listeners mListeners;               ///< 监听列表
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
        bool mInfiniteFar;                  ///< 无限远的截面
        bool mLightClipPlane;               ///< 灯光面裁剪
        static LightList mNullLight;
    };
}
#endif