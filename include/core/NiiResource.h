/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-4

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_RESOURCE_H_
#define _NII_RESOURCE_H_

#include "NiiPreInclude.h"
#include "NiiPropertyCmdObj.h"
#include "NiiResourceListener.h"
#include "NiiLoadingState.h"
#include "NiiResourceType.h"
#include "NiiSharedPtr.h"

namespace NII
{
    /** 资源类
    @note 资源在同一个时刻仅会被一条线程访问
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Resource : public PropertyCmdObj, public ResourceAlloc
    {
        friend class ResourceManager;
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
            ResourceLoadScheme * ls = 0, ResourceResultScheme * rs = 0, LangID lid = N_PrimaryLang);

        virtual ~Resource();

        /** 加入监听
        @remark 用于嵌入控制
        @param[in] rl 监听
        @version NIIEngine 3.0.0 高级api
        */
        virtual void add(ResourceListener * rl);

        /** 移去监听
        @remark 用于嵌入控制
        @param[in] rl 监听
        @version NIIEngine 3.0.0 高级api
        */
        virtual void remove(ResourceListener * rl);

        /** 设置加载策略
        @remark 多线程专用,未调用plan前都可以改变
        @param[in] ls 加载策略
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setLoadScheme(ResourceLoadScheme * rls);

        /** 获取加载策略
        @remark 多线程专用,未调用plan前都可以改变
        @param[out] ls 加载策略
        @version NIIEngine 3.0.0 高级api
        */
        virtual ResourceLoadScheme * getLoadScheme() const;

        /** 设置转换策略
        @remark (pad/phone/pc)平台数据转换
        @param[in] rs 转换策略
        @version NIIEngine 3.0.0 高级api
        */
        virtual void setResultScheme(ResourceResultScheme * rrs);

        /** 获取转换策略
        @remark (pad/phone/pc)平台数据转换
        @param[out] rs 转换策略
        @version NIIEngine 3.0.0 高级api
        */
        virtual ResourceResultScheme * getResultScheme() const;

        /** 获取资源的来源
        @param[in] src 协议/文件名
        @version NIIEngine 3.0.0
        */
        virtual void setSrc(const String & src);

        /** 获取资源的来源.
        @return 协议/文件名
        @version NIIEngine 3.0.0
        */
        virtual const String & getSrc() const;

        /** 设置资源所属资源群组.
        @remark
            资源的重要成份,资源组告诉引擎资源将会在什么时候加载,而且不同的加载策略,
            资源组的加载方式有很大的不同
        @par 属于加载策略范畴
        @param[in] g 资源组ID
        @version NIIEngine 3.0.0
        */
        virtual void setGroup(GroupID g);

        /** 获取资源所属资源群组
        @remark
            资源的重要成份,资源组告诉引擎资源将会在什么时候加载,而且不同的加载策略,
            资源组的加载方式有很大的不同
        @par 属于加载策略范畴
        @return 资源组ID
        @version NIIEngine 3.0.0
        */
        virtual GroupID getGroup() const;

        /** 获取处理ID
        @remark 这个看起来和资源ID差不多,但在运行过程中数字上更连续
        @version NIIEngine 3.0.0
        */
        virtual PrcID getPrc() const;

        /** 获取资源ID
        @remark 资源需要定义才能正常映射路径/协议和资源ID的关系
        @version NIIEngine 3.0.0
        */
        virtual ResourceID getOriginID() const;

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
        NCount getRefCount() const;

        /** 返回资源当前是否加载中.
        @version NIIEngine 3.0.0
        */
        virtual bool isLoading() const;

        /** 返回资源是否可以重新加载的.
        @version NIIEngine 3.0.0
        */
        virtual bool isReloadable() const;

        /** 返回当前加载状态.
        @version NIIEngine 3.0.0
        */
        virtual LoadingState getState() const;

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

        /** 获取生成这个资源的管理器
        @version NIIEngine 3.0.0
        */
        virtual ResourceManager * getCreator();

        /** 获取资源变化次数
        @note 资源状态发生变化,表示可能要重新加载
        @version NIIEngine 3.0.0
        */
        virtual NCount getNotifyCount() const;

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onPlanEnd();

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onLoadEnd();

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onUnloadEnd();
    protected:
        Resource(ScriptTypeID stid, LangID lid = N_PrimaryLang);

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
        virtual NCount calcSize() const { return 0; }

        /** 获取工作量因子
        @remark 这个参数和具体的计算机硬件系统性能有关,用于配合多线程协调加载
        @version NIIEngine 3.0.0 高级api
        */
        virtual NCount getWorkLoad() const { return 0; }
	private:
		Resource() {}
    protected:
        ResourceManager * mCreator;
        Listeners mListenerList;
        ResourceID mOriginID;   ///< 资源ID(在同个应用程序体系中是唯一的)
        PrcID mPrc;             ///< 处理句柄(处理机制中的是唯一的)
        GroupID mGroup;
        NCount mSize;           ///< 资源大小单位:字节
        NCount mOriginSize;     ///< 在序列中的大小(包含 脚本/协议辅助, 不包含头)
        NCounts mNotifyCount;
        NCounts mRefCount;
        String mOrigin;			///< 资源对应的来源地址
        LoadingState mState;
        ResourceLoadScheme * mLoader;
        ResourceResultScheme * mResulter;
        bool mAutoDestroy;      ///< 是否自动删除
    };
    typedef SharedPtr<Resource> ResourcePtr;
    typedef vector<ResourceID>::type ResourceIDList;
}
#endif