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

#ifndef _NII_RESOURCEMANAGER_H_
#define _NII_RESOURCEMANAGER_H_

#include "NiiPreInclude.h"
#include "NiiResource.h"
#include "NiiResourceSchemeManager.h"
#include "NiiScriptParserSys.h"
#include "NiiSingleton.h"

namespace NII
{
    /** 资源管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ResourceManager : public ScriptParserSys, public ResourceAlloc
    {
        friend class Engine;
        friend class RenderSys;
    public:
        enum VerboseMode
        {
            VM_0 = 0,
            VM_1 = 1,
            VM_2 = 2,
            VM_3 = 3
        };
        typedef map<ResourceID, Resource *>::type Resources;
        typedef map<GroupID, Resources>::type Groups;
        typedef map<PrcID, Resource *>::type PrcIDMap;
        typedef std::pair<Resource *, bool> TouchResult; ///< (返回, true/新创:false/已存在)
    public:
        ResourceManager();
        ResourceManager(ResourceType type);
        virtual ~ResourceManager();

        /** 创建一个资源
        @param[in] ls 加载策略
        @param[in] rs 结果策略
        @param[in] params 额外参数
        @version NIIEngine 3.0.0
        */
        virtual Resource * create(ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, const PropertyData * params = 0);

        /** 获取资源
        @version NIIEngine 3.0.0
        */
        virtual Resource * get(ResourceID rid, GroupID gid = GroupUnknow);

        /** 获取资源
        @note PID 在运行时也是唯一的,但仅局限于当前的运行程序
        @version NIIEngine 3.0.0
        */
        virtual Resource * get(PrcID pid);

        /** 是否存在资源
        @version NIIEngine 3.0.0
        */
        virtual bool isExist(ResourceID rid);

        /** 是否存在资源
        @note PID 在运行时也是唯一的,但仅局限于当前的运行程序
        @version NIIEngine 3.0.0
        */
        virtual bool isExist(PrcID pid);

        /** 引用资源
        @note 一般对公共资源使用这个函数(引用值加一)
        @param[in] ls 加载策略,仅对新创建资源有效,对已经存在的资源无效
        @param[in] rs 结果策略,仅对新创建资源有效,对已经存在的资源无效
        @param[in] params 额外参数,仅对新创建资源有效,对已经存在的资源无效
        @version NIIEngine 3.0.0
        */
        virtual TouchResult touch(ResourceID rid, GroupID gid = GroupUnknow,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, const PropertyData * params = 0);

        /** 解除资源
        @note 一般对公共资源使用这个函数(引用值减一)
        @param[in] rid 资源ID
        @param[in] gid 资源组ID
        @version NIIEngine 3.0.0
        */
        virtual void untouch(ResourceID rid, GroupID gid);

        /** 计划资源
        @remark
            计划资源主要是为了检测是否有条件被加载或者使用备用方案,再或者是为了促进
            加载策略和结果策略
        @param[in] ls 加载策略
        @param[in] rs 结果策略
        @param[in] params 额外参数
        @version NIIEngine 3.0.0
        */
        virtual Resource * plan(ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, const PropertyData * params = 0);

        /** 加载资源
        @param[in] ls 加载策略
        @param[in] ls 结果策略
        @param[in] params 额外参数
        @version NIIEngine 3.0.0
        */
        virtual Resource * load(ResourceID rid, GroupID gid,
            ResLoadScheme * ls = 0, ResResultScheme * rs = 0, const PropertyData * params = 0);

        /** 卸载资源
        @version NIIEngine 3.0.0
        */
        virtual void unload(ResourceID rid);

        /** 卸载资源
        @version NIIEngine 3.0.0
        */
        virtual void unload(PrcID pid);

        /** 卸载所有未使用的资源
        @param[in] resume 是否仅卸载可以再次恢复的资源
        @version NIIEngine 3.0.0
        */
        virtual void unloadUnused(bool resume = true);

        /** 卸载所有资源
        @param[in] resume 是否仅卸载可以再次恢复的资源
        @version NIIEngine 3.0.0
        */
        virtual void unloadAll(bool resume = true);

        /** 重载所有未使用的资源
        @param[in] resume 是否仅重载可以再次恢复的资源
        @version NIIEngine 3.0.0
        */
        virtual void reloadUnused(bool resume = true);

        /** 重载所有资源
        @param[in] resume 是否仅重载可以再次恢复的资源
        @version NIIEngine 3.0.0
        */
        virtual void reloadAll(bool resume = true);

        /** 移去资源而不删除
        @note 一般这类是资源的总,确定是使用 remove 而不是使用 destroy
        @version NIIEngine 3.0.0 高级api
        */
        virtual void remove(ResourceID rid);

        /** 移去资源而不删除
        @note 一般这类是资源的总,一般直接使用 destroy 而不使用 remove
        @version NIIEngine 3.0.0 高级api
        */
        virtual void remove(PrcID pid);

        /** 移去资源而不删除
        @note 一般这类是资源的总,一般直接使用 destroy 而不使用 remove
        @version NIIEngine 3.0.0 高级api
        */
        //virtual void removeUnused();

        /** 移去所有资源而不删除
        @note 一般这类是资源的总,一般直接使用 destroy 而不使用 remove
        @version NIIEngine 3.0.0 高级api
        */
        //virtual void removeAll();

		/** 删除资源
		@note 和 remove 相比会自动调用 N_delete,更适合管理器使用
		@version NIIEngine 3.0.0
		*/

        /** 删除资源
        @note 和 remove 相比会自动调用 N_delete,更适合管理器使用
        @version NIIEngine 3.0.0
        */
        virtual void destroy(ResourceID rid);

        /** 删除资源
        @note 和 remove 相比会自动调用 N_delete,更适合管理器使用
        @version NIIEngine 3.0.0
        */
        virtual void destroy(PrcID pid);

        /** 删除未使用的资源
        @remark 使用另一个线程完成
        @version NIIEngine 3.0.0
        */
        virtual void destroyUnused();

        /** 删除所有资源
        @version NIIEngine 3.0.0
        */
        virtual void destroyAll();

        /** 设置内存使用极限
        @version NIIEngine 3.0.0
        */
        void setMemMax(NCount c);

        /** 获取内存使用极限
        @version NIIEngine 3.0.0
        */
        NCount getMemMax() const;

        /** 获取内存使用数量
        @version NIIEngine 3.0.0
        */
        NCount getMemUsage() const;

        /** 资源类型
        @version NIIEngine 3.0.0
        */
        ResourceType getType() const;

        /** 分配有效的资源ID
        @remark 设计模式中使用
        @version NIIEngine 3.0.0 高级api
        */
        ResourceID genValidID() const;

        /** 设置输出详细程度
        @version NIIEngine 3.0.0
        */
        void setVerbose(VerboseMode m);

        /** 获取输出详细程度
        @version NIIEngine 3.0.0
        */
        VerboseMode getVerbose();

        /** 获取处理资源列表
        @note 线程不安全的
        @version NIIEngine 3.0.0 高级api
        */
        const PrcIDMap & getPrcResList();
    public:
        /** 通知这个管理器,一个资源的被引用
        @version NIIEngine 3.0.0
        */
        virtual void onTouch(Resource * obj);

        /** 通知这个管理器,一个资源的被分离
        @version NIIEngine 3.0.0
        */
        virtual void onUnTouch(Resource * obj);

        /** 通知这个管理器,一个资源的管理已经加载
        @version NIIEngine 3.0.0
        */
        virtual void onLoad(Resource * obj);

        /** 通知这个管理器,一个资源的管理已经卸载
        @version NIIEngine 3.0.0
        */
        virtual void onUnload(Resource * obj);
    protected:
        /** 初始化管理器
        @remark 可以用于创建基本备用方案
        @version NIIEngine 3.0.0 高级api
        */
        virtual void init() = 0;

        /** 创建一个与这个管理器兼容的新资源实例(没有自定义参数可以在这里填充)
        @remark
        @param[in] ls 加载策略
        @param[in] rs 结果策略
        @param[in] params 参数
        @version NIIEngine 3.0.0 高级api
        */
        virtual Resource * createImpl(ResourceID rid, GroupID gid,
            ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params) = 0;

        /** 添加对象到管理器中
        @version NIIEngine 3.0.0 高级api
        */
        virtual void addImpl(Resource * obj);

        /** 把对象从管理器中移出
        @version NIIEngine 3.0.0 高级api
        */
        virtual void removeImpl(Resource * obj);
    protected:
        ResourceType mType;
        Groups mGroups;
        PrcIDMap mPrcIDs;
        VerboseMode mVerbose;
        NCount mMemMax;
        NCount mMemUsage;
        NCount mScriptLevel;
        mutable ResourceID mValidRID;
        N_mutex(mValidRIDMutex)
    };
}
#endif