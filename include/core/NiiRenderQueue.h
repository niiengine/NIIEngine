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

#ifndef _NII_RenderQueue_H_
#define _NII_RenderQueue_H_

#include "NiiPreInclude.h"
#include "NiiShaderCh.h"
#include "NiiRenderPattern.h"

namespace NII
{
    /** 
    @remark 所有需要排序的成分汇总成一个数值,只需排序这个数值就可以确认渲染次序
	@version NIIEngine 6.0.0
    */
    struct RenderItem
    {
        RenderItem() : 
			mGeometryObj(0), mSpaceObj(0), mSortNum(0) {}
        RenderItem(SpaceObj * sobj, GeometryObj * geo, Nui64 sn) :
            mGeometryObj(geo), mSpaceObj(sobj), mCh(0), mSortNum(sn){}
        RenderItem(SpaceObj * sobj, GeometryObj * geo, ShaderCh * ch) :
            mGeometryObj(geo), mSpaceObj(sobj), mCh(ch), mSortNum(ch->getIndex()){}

        bool operator < (const RenderItem & o) const
        {
            return mSortNum < o.mSortNum;
        }
    public:
        SpaceObj * mSpaceObj;
        GeometryObj * mGeometryObj;
        ShaderCh * mCh;         // 推导出ShaderChMaterial
        Nui64 mSortNum;         ///< 排序数值
    };

    typedef vector<RenderItem>::type RenderItemList;
	typedef vector<RenderItemList>::type RenderItemArrayList;
	typedef map<ShaderCh *, RenderItemList *, ShaderCh::IndexLess>::type RenderItemChList;

	/**
	@version NIIEngine 6.0.0
	*/
	enum RenderType
	{
        RT_Unknow,
		RT_Single,
		RT_Queue,
		RT_QueueArray
	};
    
    /** 渲染过滤器
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI RenderFilter : public RenderAlloc
    {
    public:
        RenderFilter() {}
        virtual ~RenderFilter() {}

        /** 执行渲染
        @version NIIEngine 3.0.0
        */
        virtual void render(SpaceObj * sobj, GeometryObj * gobj, const ShaderCh * ch) = 0;

        /** 执行渲染
        @version NIIEngine 3.0.0
        */
        virtual void render(const RenderItemList & obj, bool reverse = false) = 0;
    };

    /** 渲染排序模式
    @ersion NIIEngine 3.0.0
    */
    enum RenderSortMode
    {
		RSM_None = 0,
        RSM_View = 0x01,                    ///< 由视口距离排序
        RSM_Material = 0x02,                ///< 由渲染材质排序
        RSM_Material_View = 0x04,           ///< 由先渲染材质排序,后视口距离排序
        RSM_Stable_View = 0x09,     		///< std::stable_sort or std::sort
        RSM_Stable_Material_View = 0x12,    ///< std::stable_sort or std::sort
		RSM_Sort_View_OK = 0x100,           ///< 内部标记不使用
        RSM_Sort_Material_OK = 0x200,       ///< 内部标记不使用
        RSM_Sort_Material_View_OK = 0x400,  ///< 内部标记不使用
        RSM_All = 0x1F
    };

    /** 渲染等级组
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderLevelGroup : public RenderAlloc
    {
    public:
		/** 渲染排序类型
		@version NIIEngine 6.0.0
		*/
		enum RenderSortType
		{
			RST_BasicMaterial,
            RST_BasicMaterialDescend,
            RST_BasicMaterialAscend,
            RST_BasicView,
            RST_BasicViewDescend,
            RST_BasicViewAscend,
			RST_AlphaMaterial,
            RST_AlphaMaterialDescend,
            RST_AlphaMaterialAscend,
            RST_AlphaView,
			RST_AlphaViewDescend,
            RST_AlphaViewAscend
		};
        
        /** 排序组
        @version NIIEngine 3.0.0
        */
        class _EngineAPI Group : public RenderAlloc
        {
            friend class RenderLevelGroup;
        public:
            enum GroupSortType
            {
                GST_Material,
                GST_MaterialDescend,
                GST_MaterialAscend,
                GST_View,
                GST_ViewDescend,
                GST_ViewAscend
            };
        public:
            Group();
            ~Group();
            
            /** 执行渲染过滤器渲染
            @version NIIEngine 3.0.0
            */
            void render(GroupSortType rsm, RenderFilter * filter) const;

            /** 添加
            @version NIIEngine 3.0.0
            */
            void add(SpaceObj * sobj, GeometryObj * gobj, ShaderCh * sh, bool alpha = false);
            
            /** 添加
            @version NIIEngine 3.0.0
            */
            void add(SpaceObj * sobj, GeometryObj * gobj, ShaderFusion * sf);

            /** 移去
            @version NIIEngine 3.0.0
            */
            void remove(ShaderCh * ch);

            /** 添加排序模式
            @version NIIEngine 3.0.0
            */
            inline void addSort(RenderSortMode om)         { mSortMark |= om; }

            /** 移去排序模式
            @version NIIEngine 3.0.0
            */
            inline void removeSort(RenderSortMode om)      { mSortMark &= ~om; }

            /** 移去所有
            @version NIIEngine 3.0.0
            */
            void clear();

            /** 依照指定摄象机排序渲染对象
            @version NIIEngine 3.0.0
            */
            void sort(const Camera * cam);

            /** 合并渲染排序
            @version NIIEngine 3.0.0
            */
            void merge(const Group & o);

            /** 获取渲染列表
            @version NIIEngine 5.0.0
            */
            inline const RenderItemList & getRenderList() const     { return mRenderList; }

            /** 获取材质集渲染列表
            @version NIIEngine 5.0.0
            */
            inline const RenderItemChList & getChRenderList() const { return mChRenderList; }
        protected:
            RenderItemList mRenderList;
            RenderItemChList mChRenderList;
            Nmark mSortMark;
        };
	public:
        RenderLevelGroup(RenderGroup * parent, RenderCommandGroup * dcg);

        virtual ~RenderLevelGroup();

		/**
		@version NIIEngine 6.0.0
		*/
		void render(RenderSortType type, RenderFilter * filter) const;

        /** 添加排序模式
        @version NIIEngine 3.0.0
        */
        virtual void addSort(RenderSortMode rsm);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        virtual void removeSort(RenderSortMode rsm);

        /** 重至排序模式
        @see RenderSortMode
        */
        virtual void resetSort();

        /** 添加渲染
        @version NIIEngine 3.0.0
        */
        virtual void add(SpaceObj * sobj, GeometryObj * gobj, ShaderFusion * sf);

        /** 移去渲染
        @version NIIEngine 3.0.0
        */
        virtual void remove(ShaderCh * ch);

        /** 排序
        @version NIIEngine 3.0.0
        */
        virtual void sort(const Camera * cam);

        /** 清除这个可渲染物群组
        @version NIIEngine 3.0.0
        */
        virtual void clear();

        /** 获取非透明渲染列
        @version NIIEngine 3.0.0
        */
        inline const RenderItemList & getBasic() const 			{ return mBasic.mRenderList; }

        /** 获取透明渲染列
        @version NIIEngine 3.0.0
        */
        inline const RenderItemList & getAlpha() const 			{ return mAlpha.mRenderList; }
        
        /** 获取非透明渲染列
        @version NIIEngine 3.0.0
        */
        inline const RenderItemChList & getChBasic() const      { return mBasic.mChRenderList; }

        /** 获取透明渲染列
        @version NIIEngine 3.0.0
        */
        inline const RenderItemChList & getChAlpha() const      { return mAlpha.mChRenderList; }

        /** 合并可渲染物群组
        @version NIIEngine 3.0.0
        */
        virtual void merge(const RenderLevelGroup * o);
    protected:
        RenderGroup * mParent;
        RenderCommandGroup * mDrawCallList;
        Group mBasic;
        Group mAlpha;
    };

    /** 渲染组
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderGroup : public RenderAlloc
    {
    public:
        typedef map<Nui16, RenderLevelGroup *, std::less<Nui16> >::type LevelList;
    public:
        RenderGroup(RenderQueue * parent);

        virtual ~RenderGroup();

        /** 设置渲染类型
        @version NIIEngine 3.0.0
        */
        inline void setRenderType(RenderType type)          { mRenderType = type; }

        /** 设置渲染类型
        @version NIIEngine 3.0.0
        */
        inline RenderType getRenderType() const             { return mRenderType; }

        /** 设置是否启用阴影
        @version NIIEngine 3.0.0
        */
        inline void setShadowEnable(bool b) 				{ mShadowsEnable = b; }

        /** 获取是否启用阴影
        @version NIIEngine 3.0.0
        */
        inline bool isShadowEnable() const 					{ return mShadowsEnable; }
        
        /** 设置等级数量
        @version NIIEngine 6.0.0
        */
        void setLevel(NCount cnt);

        /** 添加排序模式
        @see RenderSortMode
        */
        void addSort(RenderSortMode om);

        /** 添加排序模式
        @see RenderSortMode
        */
        void addSort(Nui16 level, RenderSortMode om);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderSortMode om);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        void removeSort(Nui16 level, RenderSortMode om);

        /** 重至排序模式
        @version NIIEngine 3.0.0
        */
        void resetSort();

        /** 添加渲染
        @version NIIEngine 3.0.0
        */
        void add(SpaceObj * sobj, GeometryObj * gobj, ShaderFusion * sf, Nui16 level);

        /** 移去渲染
        @param[in] destroy
        @version NIIEngine 3.0.0
        */
        void clear(bool destroy = false);

        /** 合并渲染组
        @version NIIEngine 3.0.0
        */
        void merge(const RenderGroup * o);

        /** 获取渲染列表
        @version NIIEngine 3.0.0
        */
        inline const LevelList & getLevelList() const 	{ return mLevelList; }
    protected:
        /** 创建渲染等级组实现
        @version NIIEngine 3.0.0
        */
        virtual void createImpl(RenderLevelGroup *& out);
    protected:
        RenderQueue * mParent;
		RenderType mRenderType;
        LevelList mLevelList;
        Nmark mSortMark;
        bool mShadowsEnable;
    };

    /** 渲染队列组
    @version NIIEngine 3.0.0
    */
    enum RenderGroupType
    {
        RQG_Unknow      = 0,
        RQG_Bg          = 1,
        RQG_Sky         = 2,
        RQG_FarBegin    = 3,
        RQG_FarEnd      = 50,
        RQG_Geo_1       = 51,
        RQG_UserBegin   = 52,
        RQG_User        = 102,
        RQG_UserEnd     = 152,
        RQG_Geo_2       = 153,
        RQG_NearBegin   = 154,
        RQG_NearEnd     = 254,
        RQG_Overlay     = 255,
        RQG_Count       = 255,
        RQG_Max         = 256
    };
    
    /** 渲染监听
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderListener
    {
    public:
        virtual ~RenderListener() {}

        /** 渲染前触发
        @version NIIEngine 3.0.0
        */
        virtual void renderBegin() {}

        /** 渲染队列前触发
        @param[in] rgt 渲染组
        @return 返回true代表渲染这个渲染组,否则不渲染
        @version NIIEngine 3.0.0
        */
        virtual bool renderBegin(RenderGroupType rgt)
        { 
            return true;
        }

        /** 渲染队列后触发
        @param[in] rgt 渲染组
        @return 返回true代表重复渲染这个渲染组,否则不重复渲染
        @version NIIEngine 3.0.0
        */
        virtual bool renderEnd(RenderGroupType rgt)
        { 
            return false;
        }
        
        /** 渲染后触发
        @version NIIEngine 3.0.0
        */
        virtual void renderEnd() {}
    };

    /** 渲染队列
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderQueue : public RenderAlloc
    {
    public:
        typedef vector<std::pair<RenderGroupType, RenderGroup *> >::type GroupList;
    public:
        RenderQueue(RenderCommandGroup * dcg);
        virtual ~RenderQueue();
        
        /** 创建主组
        @version NIIEngine 3.0.0
        */
        virtual void init();

		/**
		@version NIIEngine 6.0.0
		*/
        void prepareState(ShadowType type);

        /** 设置渲染类型
        @version NIIEngine 3.0.0
        */
        void setRenderType(RenderGroupType rgt, RenderType rtype);

        /** 设置渲染类型
        @version NIIEngine 3.0.0
        */
        RenderType getRenderType(RenderGroupType rgt) const;

        /** 设置默认组
        @version NIIEngine 3.0.0
        */
        inline void setDefaultGroup(RenderGroupType rgt){ mDefaultGroup = rgt; }

        /** 获取默认组
        @version NIIEngine 3.0.0
        */
        inline RenderGroupType getDefaultGroup() const  { return mDefaultGroup; }

        /** 设置默认等级
        @version NIIEngine 3.0.0
        */
        inline void setDefaultLevel(Nui16 lv)           { mDefaultLevel = lv; }

        /** 获取默认等级
        @version NIIEngine 3.0.0
        */
        inline Nui16 getDefaultLevel() const            { return mDefaultLevel; }

        /** 清理队列时是否删除所有渲染组
        @versioin NIIEngine 3.0.0
        */
        inline void setDestroyAllOnClear(bool r)        { mRemoveGroupOnClear = r; }

        /** 清理队列时是否删除所有渲染组
        @version NIIEngine 3.0.0
        */
        inline bool isDestroyAllOnClear() const         { return mRemoveGroupOnClear; }

        /** 添加排序模式
        @see RenderSortMode
        */
        void addSort(RenderSortMode om);

        /** 添加排序模式
        @see RenderSortMode
        */
        void addSort(RenderGroupType rgt, RenderSortMode om);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderSortMode om);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderGroupType rgt, RenderSortMode om);

		/** 重至排序模式
		@version NIIEngine 3.0.0
		*/
		void resetSort();
        
        /** 添加指定对象到队列
        @version NIIEngine 3.0.0
        */
        inline void add(SpaceObj * sobj, GeometryObj * gobj, ShadowType stype) { add(sobj, gobj, mDefaultGroup, mDefaultLevel, stype); }

        /** 添加指定对象到队列
        @version NIIEngine 3.0.0
        */
        inline void add(SpaceObj * sobj, GeometryObj * gobj, RenderGroupType rgt, ShadowType stype){ add(sobj, gobj, rgt, mDefaultLevel, stype); }

        /** 添加指定对象到队列
        @version NIIEngine 3.0.0
        */
        void add(SpaceObj * sobj, GeometryObj * gobj, RenderGroupType rgt, Nui16 level, ShadowType stype);

        /** 清理队列
        @version NIIEngine 3.0.0
        */
        void clear();
		
		/** 
		@version NIIEngine 6.0.0
		*/
        void clearState();

        /** 获取指定渲染队列群组
        @version NIIEngine 3.0.0
        */
        RenderGroup * getGroup(RenderGroupType rgt);
		
		/**
		@version NIIEngine 6.0.0
		*/
        void frameBegin();

		/**
		@version NIIEngine 6.0.0
		*/
        void frameEnded();

        /** 获取渲染列表
        @version NIIEngine 3.0.0 高级api
        */
        inline const GroupList & getGroupList() const   { return mGroups;}

        /** 合并队列
        @version NIIEngine 3.0.0
        */
        void merge(const RenderQueue * o);
    protected:
        RenderQueue(){}

		/**
		@version NIIEngine 6.0.0
		*/
        IndirectBuffer * getIndirectBuffer(NCount cnt);

        /** 创建渲染组
        @version NIIEngine 3.0.0
        */
        void createImpl(RenderGroup *& out);
    protected:
		RenderCommandGroup * mDrawCallList;
        GroupList mGroups;
        IndirectBufferList mFreeBufferList;
        IndirectBufferList mUsedBufferList;
        RenderStateCacheList mStateList;
		VertexData * mVertexData;
		IndexData * mIndexData;
        ShadowType mShadowType;
        RenderGroupType mDefaultGroup;
        Nui32 mRenderMark;
        Nui32 mVaoId;
        Nui16 mDefaultLevel;
        bool mRemoveGroupOnClear;
    };

    /** 自定义队列
    @remark 用于混用阴影技术
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderGroupFusion : public RenderAlloc
    {
    public:
        RenderGroupFusion(RenderGroupType rgt, const String & name = StrUtil::WBLANK);

        virtual ~RenderGroupFusion(){}

        /** 名字(辅助)
        @version NIIEngine 3.0.0
        */
        inline const String & getName() const 	        { return mName; }

        /** 获取渲染组
        @version NIIEngine 3.0.0
        */
        inline RenderGroupType getRenderGroup() const   { return mRenderGroup; }
        
        /** 设置使用的阴影技术
        @param[in] sf SF_None则没有阴影
        @version NIIEngine 6.0.0
        */
        inline void setShadowFeture(ShadowFeture sf)    { mShadowFeture = sf; }
        
        /** 获取使用的阴影技术
        @version NIIEngine 6.0.0
        */
        inline ShadowFeture getShadowFeture() const     { return mShadowFeture; }

        /** 设置渲染通道
        @version NIIEngine 3.0.0
        */
        inline void setShaderCh(ShaderCh * ch)          { mShaderCh = ch; }

        /** 获取渲染通道
        @version NIIEngine 3.0.0
        */
        inline ShaderCh * getShaderCh() const           { return mShaderCh; }

        /** 执行渲染
        @version NIIEngine 3.0.0
        */
        virtual void render(RenderGroup * group, RenderPattern * pattern, SpaceManager * dst) const;
    protected:
        /** 执行渲染实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void renderImpl(RenderGroup * group, RenderPattern * pattern) const;
    protected:
        String mName;
        RenderGroupType mRenderGroup;
        ShadowFeture mShadowFeture;
        ShaderCh * mShaderCh;
    };
    typedef vector<RenderGroupFusion *>::type RenderGroupFusionList;

    /** 自定义渲染
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderQueueFusion : public RenderAlloc
    {
    public:
        RenderQueueFusion(Nid id);
        virtual ~RenderQueueFusion();

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        inline Nid getID() const                    { return mID; }

        /** 添加自定义队列
        @note 参数内存将由这个类管理
        @version NIIEngine 3.0.0
        */
        inline void add(RenderGroupFusion * obj)    { mList.push_back(obj); }

        /** 添加自定义队列
        @version NIIEngine 3.0.0
        */
        RenderGroupFusion * add(RenderGroupType rgt, const String & name);

        /** 获取自定义队列
        @version NIIEngine 3.0.0
        */
        inline RenderGroupFusion * get(Nidx index)  { N_assert(index < mList.size(), "error"); return mList[index]; }

        /** 移去自定义队列
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);

        /** 移去所有自定义队列
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 获取数量
        @version NIIEngine 3.0.0
        */
        inline NCount getCount() const              { return mList.size(); }

        /** 获取自定义队列
        @version NIIEngine 3.0.0 高级api
        */
        inline const RenderGroupFusionList & getList() const    { return mList; }
    protected:
        Nid mID;
        RenderGroupFusionList mList;
    };
}
#endif