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

#ifndef _NII_RESOURCE_H_
#define _NII_RESOURCE_H_

#include "NiiPreInclude.h"
#include "NiiPropertyCmdObj.h"
#include "NiiPropertyData.h"
#include "NiiResourceType.h"
#include "NiiSharedPtr.h"

namespace NII
{
    /** 资源加载状态
    @version NIIEngine 3.0.0
    */
    enum LoadingState
    {
        LS_UNSETUP      = 101,      ///< 未配置
        LS_SETUPING     = 102,      ///< 配置中
        LS_SETUPED      = 103,      ///< 已策略

        LS_PLANNED      = 106,      ///< 完全准备好
        LS_PLANNING     = 107,      ///< 准备中
        LS_LOADING      = 108,      ///< 加载中
        LS_LOADED       = 109,      ///< 完全加载完
        LS_UNLOADING    = 104,      ///< 当前卸载中
        LS_UNLOADED     = 105,      ///< 卸载完成
    };

    /** 资源监听
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI ResourceListener
    {
    public:
        ResourceListener();
        virtual ~ResourceListener();

        /** 资源加载策略部署后触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onPlanEnd(Resource * r);

        /** 当加载完后触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onLoadEnd(Resource * r);

        /** 资源转换策略完成后触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onResultEnd(Resource * r);

        /** 当资源卸载完成后触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onUnloadEnd(Resource * r);

        /** 当加载策略启动时触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onLoadScheme(Resource * r, LoadScheme * s, bool enable);

        /** 当结果策略启动时触发
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onResultScheme(Resource * r, ResultScheme * s, bool enable);
    };

    /** 资源类
    @note 资源在同一个时刻仅会被一条线程访问
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Resource : public PropertyCmdObj, public ResourceAlloc
    {
        friend class ResourceManager;
		friend class ResourceSchemeManager;
    public:
        typedef vector<ResourceListener *>::type Listeners;
    public:
        /** 基本构造函数.
        @param[in] rid 对象的唯一ID
        @param[in] gid 群组
        @param[in] ls 加载处理策略
        @param[in] rs 结果处理策略
        @version NIIEngine 3.0.0
        */
        Resource(ScriptTypeID stid, ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, LangID lid = N_PrimaryLang);

        virtual ~Resource();

        /** 辅助名字
        @version NIIEngine 4.0.0
        */
        inline const String & getName() const       { return mName; }

		/** 获取资源ID
		@remark 资源需要定义才能正常映射路径/协议和资源ID的关系
		@version NIIEngine 3.0.0
		*/
		inline ResourceID getOriginID() const       { return mOriginID; }

		/** 获取处理ID
		@remark 这个看起来和资源ID差不多,但在运行过程中数字上更连续
		@version NIIEngine 3.0.0
		*/
		inline PrcID getPrc() const                 { return mPrc; }

		/** 返回当前加载状态.
		@version NIIEngine 3.0.0
		*/
		inline LoadingState getState() const        { return mState; }
        
        /** 设置属性
        @version NIIEngine 3.0.0
        */
        virtual void setProperty(PropertyData * param);

        /** 加入监听
        @remark 用于嵌入控制
        @version NIIEngine 3.0.0 高级api
        */
        virtual void add(ResourceListener * rl);

        /** 移去监听
        @remark 用于嵌入控制
        @version NIIEngine 3.0.0 高级api
        */
        virtual void remove(ResourceListener * rl);

        /** 设置加载策略
        @remark 多线程专用,未调用plan前都可以改变
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setLoadScheme(ResLoadScheme * rls);

        /** 获取加载策略
        @remark 多线程专用,未调用plan前都可以改变
        @version NIIEngine 3.0.0 高级api
        */
        virtual ResLoadScheme * getLoadScheme() const;

        /** 设置转换策略
        @remark (pad/phone/pc)平台数据转换
        @param[in] rs 转换策略
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setResultScheme(ResResultScheme * rrs);

        /** 获取转换策略
        @remark (pad/phone/pc)平台数据转换
        @version NIIEngine 3.0.0 高级api
        */
        virtual ResResultScheme * getResultScheme() const;

        /** 获取资源的来源
        @param[in] src 协议/文件名
        @version NIIEngine 3.0.0
        */
        inline void setSrc(const String & src)      { mSrc = src; }

        /** 获取资源的来源.
        @return 协议/文件名
        @version NIIEngine 3.0.0
        */
        inline const String & getSrc() const        { return mSrc; }

        /** 设置资源所属资源群组.
        @remark
            资源的重要成份,资源组告诉引擎资源将会在什么时候加载,而且不同的加载策略,
            资源组的加载方式有很大的不同
        @par 属于加载策略范畴
        @version NIIEngine 3.0.0
        */
        virtual void setGroup(GroupID gid);

        /** 获取资源所属资源群组
        @remark
            资源的重要成份,资源组告诉引擎资源将会在什么时候加载,而且不同的加载策略,
            资源组的加载方式有很大的不同
        @par 属于加载策略范畴
        @version NIIEngine 3.0.0
        */
        virtual GroupID getGroup() const;

        /** 检索资源大小.
        @version NIIEngine 3.0.0
        */
        virtual NCount getSize() const;

        /** 检索资源在序列中的大小.
        @version NIIEngine 3.0.0
        */
        virtual NCount getOriginSize() const;

        /** 资源状态/性质变化了
        @note 资源状态发生变化,表示可能要重新加载
        @verison NIIEngine 3.0.0
        */
        virtual void notify();

        /** 计划资源
        @remark
            计划资源主要是为了检测是否有条件被加载或者使用备用方案,再或者是为了促进
            加载策略和结果策略
        @version NIIEngine 3.0.0
        */
        virtual void plan();

        /** 加载资源
        @version NIIEngine 3.0.0
        */
        virtual void load();

        /** 重载资源
        @version NIIEngine 3.0.0
        */
        virtual void reload();

        /** 卸载资源
        @version NIIEngine 3.0.0
        */
        virtual void unload();

        /** 正式引用这个资源
        @param[in] load 是否现在加载
        @version NIIEngine 3.0.0 高级api
        */
        virtual void touch(bool load = true);

        /** 解除引用这个资源
        @version NIIEngine 3.0.0 高级api
        */
        virtual void untouch();

        /** 获取引用数量
        @note 用于自动释放资源
        @version NIIEngine 3.0.0
        */
        inline NCount getRefCount() const           { return mRefCount; }

        /** 返回资源当前是否加载中.
        @version NIIEngine 3.0.0
        */
        virtual bool isLoading() const;

        /** 返回资源是否可以重新加载的.
        @version NIIEngine 3.0.0
        */
        virtual bool isReloadable() const;

        /** 是否自动删除
        @note 用于自动释放资源
        @version NIIEngine 3.0.0
        */
        void setAutoDestroy(bool s);

        /** 是否自动删除
        @note 用于自动释放资源
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const;

        /** 获取生成资源的管理器
        @version NIIEngine 3.0.0
        */
        virtual ResourceManager * getCreator();

        /** 获取资源变化次数
        @note 资源状态发生变化,表示可能要重新加载
        @version NIIEngine 3.0.0
        */
        virtual NCount getNotifyCount() const;
        
        /** 从序列中加载
        @version NIIEngine 4.0.0
        */
        virtual void read(Serializer * out) const;
        
        /** 写入到序列中
        @version NIIEngine 4.0.0
        */
        virtual void write(const Serializer * in);
    protected:
        Resource(ScriptTypeID stid, LangID lid = N_PrimaryLang);
        
        /** 当计划完成后触发
        @version NIIEngine 3.0.0
        */
        virtual void onPlanEnd();

        /** 当加载完成后触发
        @version NIIEngine 3.0.0
        */
        virtual void onLoadEnd();

        /** 当卸载完成后触发
        @version NIIEngine 3.0.0
        */
        virtual void onUnloadEnd();

        /** 请求加载资源
        @version NIIEngine 3.0.0
        */
        virtual void requestImpl(LoadingState ls);

        /** 计划实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void planImpl();

        /** 撤消实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void unplanImpl();

        /** 加载实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void loadImpl() = 0;

        /** 加载完成实现
        @version NIIEngine 3.0.0
        */
        virtual void loadEndImpl();

        /** 卸载实现
        @version NIIEngine 3.0.0 高级api
        */
        virtual void unloadImpl() = 0;

        /** 卸载完成实现
        @note 在卸载线程中,已经发生互斥事件
        */
        virtual void unloadEndImpl();

        /** 计算资源大小
        @version NIIEngine 3.0.0 高级api
        */
        virtual NCount calcSize() const             { return 0; }

        /** 获取工作量化因子
        @remark 这个参数和具体的计算机硬件系统性能有关,用于配合多线程协调加载
        @version NIIEngine 3.0.0 高级api
        */
        virtual NCount getWorkFactor() const        { return 0; }
    private:
        Resource() {}
    protected:
        ResourceManager * mCreator;
        Listeners mListenerList;
        ResourceID mOriginID;   ///< 资源ID(在同个应用程序体系中是唯一的)
        PrcID mPrc;             ///< 处理句柄(处理机制中的是唯一的)
        String mSrc;            ///< 资源对应的来源地址
        String mName;
        GroupID mGroup;
        NCount mSize;
        NCount mOriginSize;     ///< 在序列中的大小(包含 脚本/协议辅助, 不包含头)
        NCounts mNotifyCount;
        NCounts mRefCount;
        LoadingState mState;
        ResLoadScheme * mLoader;
        ResResultScheme * mResulter;
        bool mAutoDestroy;
    };
    typedef SharedPtr<Resource> ResourcePtr;
    typedef vector<ResourceID>::type ResourceIDList;
}
#endif