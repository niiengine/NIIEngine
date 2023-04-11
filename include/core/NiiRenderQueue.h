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
            mGeometryObj(geo), mSpaceObj(sobj), mCh(ch), mSortNum(0){}

        bool operator < (const RenderItem & o) const
        {
            return mSortNum < o.mSortNum;
        }
    public:
        SpaceObj * mSpaceObj;
        GeometryObj * mGeometryObj;
        ShaderCh * mCh;
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
		RT_Single,
		RT_Queue,
		RT_ArrayQueue
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
        virtual void render(const RenderItemList & obj) = 0;
    };

    /** 渲染排序模式
    @ersion NIIEngine 3.0.0
    */
    enum RenderSortMode
    {
		RSM_None = 0,
        RSM_View = 0x01,            ///< 由视口距离排序
        RSM_Material = 0x02,        ///< 由渲染材质排序
        RSM_Material_View = 0x04,   ///< 由先渲染材质排序,后视口距离排序
        RSM_Stable_View = 0x08,     		///< std::stable_sort or std::sort
        RSM_Stable_Material_View = 0x10,    ///< std::stable_sort or std::sort
		RSM_Sort_View_OK = 0x100,    ///< 
        RSM_Sort_Material_OK = 0x200,///< 
        RSM_Sort_Material_View_OK = 0x400,///< 
        RSM_All = 0x1F
    };

    /** 渲染等级组
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderLevelGroup : public RenderAlloc
    {
    public:
		/** 排序类型
		@version NIIEngine 6.0.0
		*/
		enum SortType
		{
			ST_BasicCh,
            ST_BasicSortDescend,
            ST_BasicSortAscend,
			ST_AlphaCh,
			ST_AlphaSortDescend,
            ST_AlphaSortAscend
		};
	public:
        RenderLevelGroup(RenderGroup * parent);

        virtual ~RenderLevelGroup();
		
		/**
		@version NIIEngine 6.0.0
		*/
		void render(SortType type, RenderFilter * filter) const;

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
        virtual void add(SpaceObj * sobj, GeometryObj * obj, ShaderFusion * sf);

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
        inline const RenderItemList & getBasic() const 			{ return mBasic; }

        /** 获取透明渲染列
        @version NIIEngine 3.0.0
        */
        inline const RenderItemList & getAlpha() const 			{ return mAlpha; }
        
        /** 获取非透明渲染列
        @version NIIEngine 3.0.0
        */
        inline const RenderItemChList & getChBasic() const      { return mChBasic; }

        /** 获取透明渲染列
        @version NIIEngine 3.0.0
        */
        inline const RenderItemChList & getChAlpha() const      { return mChAlpha; }

        /** 合并可渲染物群组
        @version NIIEngine 3.0.0
        */
        virtual void merge(const RenderLevelGroup * o);
    protected:
        RenderGroup * mParent;
        DrawCallGroup * mDrawCallList;
        RenderItemList mBasic;
        RenderItemList mAlpha;
		RenderItemChList mChBasic;
        RenderItemChList mChAlpha;
        Nmark mBasicMark;
		Nmark mAlphaMark;
    };

    /** 渲染组
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderGroup : public RenderAlloc
    {
    public:
        typedef map<Nui16, RenderLevelGroup *, std::less<Nui16> >::type RenderList;
    public:
        RenderGroup(RenderQueue * parent);

        virtual ~RenderGroup();

        /** 设置是否启用阴影
        @version NIIEngine 3.0.0
        */
        inline void setShadowEnable(bool b) 				{ mShadowsEnable = b; }

        /** 获取是否启用阴影
        @version NIIEngine 3.0.0
        */
        inline bool isShadowEnable() const 					{ return mShadowsEnable; }

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
        void add(SpaceObj * sobj, GeometryObj * obj, ShaderFusion * sf, Nui16 level);

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
        inline const RenderList & getRenderList() const 	{ return mRenderList; }
    protected:
        /** 创建渲染等级组实现
        @version NIIEngine 3.0.0
        */
        virtual void createImpl(RenderLevelGroup *& out);
    protected:
        RenderQueue * mParent;
		RenderType mMode;
        RenderList mRenderList;
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
        RQG_Sky_First   = 2,
        RQG_Ext_1       = 3,
        RQG_Ext_2       = 4,
        RQG_Ext_3       = 5,
        RQG_Ext_4       = 6,
        RQG_Ext_5       = 7,
        RQG_Geo_1       = 10,
        RQG_Ext_6       = 11,
        RQG_Ext_7       = 12,
        RQG_Ext_8       = 13,
        RQG_Ext_9       = 14,
        RQG_Ext_10      = 15,
        RQG_User        = 20,
        RQG_Ext_11      = 21,
        RQG_Ext_12      = 22,
        RQG_Ext_13      = 23,
        RQG_Ext_14      = 24,
        RQG_Ext_15      = 25,
        RQG_Geo_2       = 30,
        RQG_Ext_16      = 31,
        RQG_Ext_17      = 32,
        RQG_Ext_18      = 33,
        RQG_Ext_19      = 34,
        RQG_Ext_20      = 35,
        RQG_Sky_Last    = 40,
        RQG_Surface     = 41,
        RQG_Count       = 42,
        RQG_Max         = 255
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
        @param[in] gid 渲染组
        @return 返回true代表渲染这个渲染组,否则不渲染
        @version NIIEngine 3.0.0
        */
        virtual bool renderBegin(GroupID gid)
        { 
            return true;
        }

        /** 渲染队列后触发
        @param[in] gid 渲染组
        @return 返回true代表重复渲染这个渲染组,否则不重复渲染
        @version NIIEngine 3.0.0
        */
        virtual bool renderEnd(GroupID gid)
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
        RenderQueue(DrawCallGroup * dcg);
        virtual ~RenderQueue();
		
		/**
		@version NIIEngine 6.0.0
		*/
        void prepareState(RenderPattern::ShadowType type);

        /** 设置默认组
        @version NIIEngine 3.0.0
        */
        inline void setDefaultGroup(GroupID grp)           { mDefaultGroup = grp; }

        /** 获取默认组
        @version NIIEngine 3.0.0
        */
        inline GroupID getDefaultGroup() const             { return mDefaultGroup; }

        /** 设置默认等级
        @version NIIEngine 3.0.0
        */
        inline void setDefaultLevel(Nui16 lv)              { mDefaultLevel = lv; }

        /** 获取默认等级
        @version NIIEngine 3.0.0
        */
        inline Nui16 getDefaultLevel() const               { return mDefaultLevel; }

        /** 清理队列时是否删除所有渲染组
        @versioin NIIEngine 3.0.0
        */
        inline void setDestroyAllOnClear(bool r)           { mRemoveGroupOnClear = r; }
        
        /** 清理队列时是否删除所有渲染组
        @version NIIEngine 3.0.0
        */
        inline bool isDestroyAllOnClear() const            { return mRemoveGroupOnClear; }
		
        /** 添加排序模式
        @see RenderSortMode
        */
        void addSort(RenderSortMode om);

        /** 添加排序模式
        @see RenderSortMode
        */
        void addSort(RenderGroupType glevel, RenderSortMode om);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderSortMode om);

        /** 移去排序模式
        @version NIIEngine 3.0.0
        */
        void removeSort(RenderGroupType glevel, RenderSortMode om);
		
		/** 重至排序模式
		@version NIIEngine 3.0.0
		*/
		void resetSort();
        
        /** 添加指定对象到队列
        @version NIIEngine 3.0.0
        */
        inline void add(SpaceObj * sobj, GeometryObj * obj)                 { add(obj, mDefaultGroup, mDefaultLevel); }

        /** 添加指定对象到队列
        @version NIIEngine 3.0.0
        */
        inline void add(SpaceObj * sobj, GeometryObj * obj, GroupID gid)    { add(obj, gid, mDefaultLevel); }

        /** 添加指定对象到队列
        @version NIIEngine 3.0.0
        */
        void add(SpaceObj * sobj, GeometryObj * obj, GroupID gid, Nui16 level);

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
        RenderGroup * getGroup(GroupID qid);
		
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
        const GroupList & getGroupList() const				{ return mGroups;}

        /** 合并队列
        @version NIIEngine 3.0.0
        */
        void merge(const RenderQueue * o);
    protected:
		/**
		@version NIIEngine 6.0.0
		*/
        IndirectBuffer * getIndirectBuffer(NCount cnt);
	
        /** 创建渲染组
        @version NIIEngine 3.0.0
        */
        void createImpl(RenderGroup *& out);

        /** 创建主组
        @version NIIEngine 3.0.0
        */
        void createMainGroup();
    protected:
		DrawCallGroup * mDrawCallList;
        IndirectBufferList mFreeBufferList;
        IndirectBufferList mUsedBufferList;
        GroupList mGroups;
        GroupID mDefaultGroup;
        RenderStateCacheList mStateList;
        Nui32 mRenderMark;
        RenderPattern::ShadowType mShadowType;
		VertexData * mVertexData;
		IndexData * mIndexData;
        uint32 mVaoId;
        Nui16 mDefaultLevel;
        bool mRemoveGroupOnClear;
    };

    /** 自定义队列
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderGroupFusion : public RenderAlloc
    {
    public:
        RenderGroupFusion(GroupID gid, const String & name = StrUtil::WBLANK);

        virtual ~RenderGroupFusion(){}

        /** 名字(辅助)
        @version NIIEngine 3.0.0
        */
        const String & getName() const 	{ return mName; }

        /** 获取渲染组
        @version NIIEngine 3.0.0
        */
        GroupID getRenderGroup() const 	{ return mRenderGroup; }
        
        /** 是否启动阴影
        @version NIIEngine 3.0.0
        */
        void setShadowEnable(bool b)    { mShadowEnable = b; }

        /** 是否启动阴影
        @version NIIEngine 3.0.0
        */
        bool isShadowEnable() const     { return mShadowEnable; }

        /** 设置渲染通道
        @version NIIEngine 3.0.0
        */
        void setShaderCh(ShaderCh * ch) { mShaderCh = ch; }

        /** 获取渲染通道
        @version NIIEngine 3.0.0
        */
        ShaderCh * getShaderCh() const { return mShaderCh; }

        /** 执行渲染
        @version NIIEngine 3.0.0
        */
        virtual void render(RenderGroup * group, RenderPattern * pattern, SpaceManager * dst);
    protected:
        /** 执行渲染实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void renderImpl(RenderGroup * group, RenderPattern * pattern);
    protected:
        String mName;
        GroupID mRenderGroup;
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
        Nid getID() const { return mID; }

        /** 添加自定义队列
        @note 参数内存将由这个类管理
        @version NIIEngine 3.0.0
        */
        void add(RenderGroupFusion * obj) { mList.push_back(obj); }

        /** 添加自定义队列
        @version NIIEngine 3.0.0
        */
        RenderGroupFusion * add(GroupID qid, const String & name);

        /** 获取自定义队列
        @version NIIEngine 3.0.0
        */
        RenderGroupFusion * get(Nidx index) 		{ N_assert(index < mList.size(), "error"); return mList[index]; }

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
        NCount getCount() const             		{ return mList.size(); }

        /** 获取自定义队列
        @version NIIEngine 3.0.0 高级api
        */
        const RenderGroupFusionList & getList()   	{ return mList; }
    protected:
        Nid mID;
        RenderGroupFusionList mList;
    };
}
#endif