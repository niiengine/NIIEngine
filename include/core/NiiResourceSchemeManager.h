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

#ifndef _NII_RESOURCE_Scheme_MANAGER_H_
#define _NII_RESOURCE_Scheme_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiResourceType.h"
#include "NiiVFS.h"
#include "NiiDataStream.h"
#include "NiiCommon.h"
#include "NiiJob.h"
#include "NiiThread.h"
#include <ctime>

namespace NII
{
    typedef RequestID BgPrcID;
	struct ResourceRequest;
    struct LoadGroup;
    /** 资源加载管理器
    @remark
        资源组在引擎中是个非常重要的组成,确定资源是怎么被加载,协调硬件系统控制其加载
        过程,可以配合 TriggerManager 去决定资源什么时候被加载,被卸载.还负责管理资源
        类型和资源管理器.如果不使用这个管理器加载资源,过程是实时,并且堵塞.
    @note
        TriggerManager 是另外一个复杂的控制管理器,处于顶层应用设计中的,所以并非潜在
        于引擎机制中的,而且要配合 EventObj 和 CommandObj 一起使用
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ResourceSchemeManager : public JobPrc, public ThreadMain,
        public Singleton<ResourceSchemeManager>
    {
        friend class Engine;
    public:
        typedef map<ResourceType, ResourceManager *>::type ResMagList;
    public:
    
        /** 设置处理类型
        @note 在产生线程ThreadMain前调用
        @version NIIEngine 3.0.0
        */
        inline void setPrcType(Nui32 type){ mPrcType = type; }
        
        /** 获取处理类型
        @note 在产生线程ThreadMain前调用
        @version NIIEngine 3.0.0
        */
        inline Nui32 getPrcType() const { return mPrcType; }
        
        /** 请求加载/卸载
        @param[in] gid 需要加载的组
        @param[in] count [0, 100]
            如果调用的是100在这个函数返回后,组里的资源已经完全加载,如果调用的是0在这
            个函数返回后,组里的资源已经完全卸载(不删除)
        @version NIIEngine 3.0.0
        */
        void request(GroupID gid, NCount count);

        /** 创建一个资源组
        @remark
            资源组对于资源来说是非常重要的,资源组控制资源在什么时候加载,和什么时候释
            放,通过不同加载策略和结果策略来组织,让场景做到无缝连接
        @note
            虽然资源本身可以不属于任何一个组,但如果想更好的搭配加载过程,就要懂得如何
            使用资源组
        @param[in] gid 资源组ID,在同个应用程序中应该是唯一的
        @param[in] global 全局组
        @version NIIEngine 3.0.0
        */
        void create(GroupID gid, bool global = true);

        /** 删除资源组
        @version NIIEngine 3.0.0
        */
        void destroy(GroupID gid);

        /** 清除资源组
        @version NIIEngine 3.0.0
        */
        void clear(GroupID gid);

        /*** 是否存在资源组
        @version NIIEngine 3.0.0
        */
        bool isExist(GroupID gid);

        /** 是否是全局资源组
        @version NIIEngine 3.0.0
        */
        bool isGlobal(GroupID gid);

        /** 添加资源类型管理器
        @param[in] type 资源类型
        @param[in] mag 管理器实例
        @version NIIEngine 3.0.0
        */
        void add(ResourceType type, ResourceManager * mag);

        /** 移去资源类型管理器
        @param[in] type 资源类型
        @version NIIEngine 3.0.0
        */
        void remove(ResourceType type);

        /** 获取指定资源类型的资源管理器
        @param[in] type 资源类型
        @note 只能是 1:1 映射关系
        @version NIIEngine 3.0.0
        */
        ResourceManager * get(ResourceType type);

        /** 获取一个遍历注册资源管理的一个迭代
        @version NIIEngine 3.0.0 高级api
        */
        ResMagList & getManagerList();

        /** 设置引擎资源使用的资源组
        @version NIIEngine 3.0.0
        */
        void setEngineGroup(GroupID gid);

        /** 获取引擎资源使用的资源组
        @version NIIEngine 3.0.0
        */
        GroupID getEngineGroup() const;

        /** 添加脚本解析器
        @version NIIEngine 3.0.0
        */
        void add(ScriptParserSys * obj);

        /** 移去脚本解析器
        @version NIIEngine 3.0.0
        */
        void remove(ScriptParserSys * obj);

        /** 获取脚本解析器
        @param[in] ext 文件后缀类型
        @version NIIEngine 3.0.0
        */
        void getParser(const String & ext, ScriptParserSys *& out);

        /** 创建一个资源
        @param[in] file 文件名字,包含格式扩展包含路径,对应 Reousrce::mSrc
        @version NIIEngine 3.0.0
        */
        void create(ResourceID rid, ResourceType type, const String & file,
            GroupID gid = GroupDefault, const PropertyData & params = PropertyData());

        /** 创建一个资源
        @param[in] file 文件名字,包含格式扩展包含路径,对应 Reousrce::mSrc
        @version NIIEngine 3.0.0
        */
        void create(ResourceID rid, ResourceType type, const String & file,
            GroupID gid, ResLoadScheme * ls, ResResultScheme * rs,
                const PropertyData & params = PropertyData());

        /** 删除一个资源
        @version NIIEngine 3.0.0
        */
        void destroy(ResourceID rid, GroupID gid);

        /** 添加虚拟文件系统
        @param[in] protocol 文件协议
        @param[in] type 协议类型
        @param[in] gid 资源组ID
        @param[in] r 是否把(子目录文件)也全部加到群组里
        @version NIIEngine 3.0.0
        */
        void add(const String & protocol, VFSType type, GroupID gid = GroupDefault,
            bool r = false);

        /** 移去虚拟文件系统
        @version NIIEngine 3.0.0
        */
        void remove(const String & protocol, GroupID gid = GroupDefault);

        /** 是否存在虚拟文件系统
        @version NIIEngine 3.0.0
        */
        bool isVFSExist(const String & protocol, GroupID gid = GroupDefault) const;

        /** 获取虚拟文件系统
        @param[in] protocol
        @param[in] gid
        @version NIIEngine 3.0.0
        */
        VFS * getVFS(const String & protocol, GroupID gid = GroupDefault) const;

        /** 是否存在一个文件
        @param[in] gid 资源组ID
        @param[in] file 文件协议
        */
        bool isExist(GroupID gid, const String & file);

        /** 是否存在一个文件
        @param[in] file 文件协议
        */
        bool isExist(const String & file);

        /** 找到文件所在的组
        @param[in] file 文件名字
        @return 资源组的ID
        */
        GroupID find(const String & file);
        
        /** 中止后台处理
        @version NIIEngine 3.0.0
        */
        void abort(BgPrcID pid);
        
        /** 返回任务是否处理完成
        @version NIIEngine 3.0.0
        */
        bool isComplete(BgPrcID pid);

        /** 安排指定资源组进入机制控制
        @remark
            创建好资源组需要使用这个函数安排资源组进入机制控制,这样资源组的策略才会有
            效果,但有点需要注意的是在调用这个函数前是不是先定义资源到资源组中.
        @param[in] gid 资源组ID
        @param[in] ls 资源加载策略ID,备用
        @param[in] rs 资源结果策略ID,备用
        @version NIIEngine 3.0.0
        */
        BgPrcID setup(GroupID gid, SchemeID ls, SchemeID rs);

        /** 安排所有资源组进入机制控制
        @remark
            创建好资源组需要使用这个函数安排资源组进入机制控制,这样资源组的策略才会有
            效果.
        @param[in] ls 资源加载策略ID,备用
        @param[in] rs 资源结果策略ID,备用
        @version NIIEngine 3.0.0
        */
        BgPrcID setupAll(SchemeID ls, SchemeID rs);

        /** 资源组是否处于机制控制中
        @param[in] 指定资源组是否已经被设立.
        @version NIIEngine 3.0.0
        */
        bool isSetup(GroupID gid);

        /** 计划资源组.
        @remark
            计划资源组主要是为了检测是否有条件被加载或者使用备用方案,再或者是为了促进
            加载策略和结果策略
        @param[in] gid 需要计划的资源组
        @param[in] res 是否计划资源
        @version NIIEngine 3.0.0
        */
        BgPrcID plan(GroupID gid, bool res = true);

        /** 加载资源组.
        @param[in] gid 需要加载的资源组
        @param[in] res 是否加载资源
        @version NIIEngine 3.0.0
        */
        BgPrcID load(GroupID gid, bool res = true);

        /** 卸载资源组的所有资源
        @param[in] gid 需要卸载的资源组的ID
        @param[in] resume 是否仅重载可以再次恢复的资源
        @version NIIEngine 3.0.0
        */
        BgPrcID unload(GroupID gid, bool resume = true);
        
        /** 卸载资源
        @version NIIEngine 3.0.0
        */
        BgPrcID unload(ResourceType resType, ResourceID rid);

        /** 卸载资源
        @version NIIEngine 3.0.0
        */
        BgPrcID unload(ResourceType resType, PrcID pid);

        /** 卸载资源组未使用的资源
        @param[in] gid 需要卸载的资源组的ID
        @param[in] resume 是否仅重载可以再次恢复的资源
        @version NIIEngine 3.0.0
        */
        void unloadUnused(GroupID gid, bool resume = true);

        /** 检测资源组
        @param[in] gid 需要检测的资源组的ID
        @version NIIEngine 3.0.0
        */
        bool isLoad(GroupID gid);

        /** 打开资源所定义的文件
        @param[in] rid 资源ID
        @param[in] allgroup 所有组中寻找
        @param[out] obj 输出
        @note 数据流使用完后要调用 N_delete 释放
        @version NIIEngine 3.0.0
        */
        DataStream * open(ResourceID rid, GroupID gid = GroupDefault);

        /** 打开文件
        @param[in] file 文件
        @param[in] allgroup 所有组中寻找
        @param[out] obj 输出
        @note 数据流使用完后要调用 N_delete 释放
        @version NIIEngine 3.0.0
        */
        DataStream * open(const String & file, GroupID gid = GroupDefault,
            bool allgroup = true, Resource * obj = 0);

        /** 打开多个文件
        @param[in] pattern 扩展符号/通配符字符'*'
        @note 数据流使用完后要调用 N_delete 释放
        @version NIIEngine 3.0.0
        */
        DataStreamList opens(const String & pattern, GroupID gid = GroupDefault);

        /** 创建资源文件
        @note 数据流使用完后要调用 N_delete 释放
        @version NIIEngine 3.0.0 高级api
        */
        DataStream * createSrc(const String & file, GroupID gid = GroupDefault,
            bool overwrite = false, const String & protocol = StrUtil::WBLANK);

        /** 删除资源文件
        @version NIIEngine 3.0.0 高级api
        */
        void deleteSrc(const String & file, GroupID gid = GroupDefault,
            const String & protocol = StrUtil::WBLANK);

        /** 删除所有匹配的文件
        @version NIIEngine 3.0.0
        */
        void deleteMatch(const String & pattern, GroupID gid = GroupDefault,
            const String & protocol = StrUtil::WBLANK);

        /** 列出资源组的所有文件或目录的名字
        @version NIIEngine 3.0.0
        */
        void list(GroupID gid, StringList & out, bool dirs = false);

        /** 列出资源组的所有文件或目录的附带信息
        @version NIIEngine 3.0.0
        */
        void list(GroupID gid, FileInfoList & out, bool dirs = false);

        /** 找出资源组的所有文件或目录的名字
        @version NIIEngine 3.0.0
        */
        void find(GroupID gid, StringList & out, const String & pattern, bool dirs = false);

        /** 找出资源组的所有文件或目录的附带信息
        @version NIIEngine 3.0.0
        */
        void find(GroupID gid, FileInfoList & out, const String & pattern, bool dirs = false);

        /** 获取文件的创建时间
        @version NIIEngine 3.0.0
        */
        time_t getCreateTime(GroupID gid, const String & file);

        /** 获取文件的修改时间
        @version NIIEngine 3.0.0
        */
        time_t getModifyTime(GroupID gid, const String & file);

        /** 获取文件的访问时间
        @version NIIEngine 3.0.0
        */
        time_t getAccessTime(GroupID gid, const String & file);

        /** 获取所有组
        @version NIIEngine 3.0.0
        */
        GroupIDList getGroups();

        /** 获取为指定组的资源
        @version NIIEngine 3.0.0
        */
        ResourceIDList getResources(GroupID gid);

        /** 分配有效的处理ID
        @remark 一般为连续的数值
        @version NIIEngine 3.0.0 高级api
        */
        PrcID genValidID();
    public:
        /** 资源创建后
        @version NIIEngine 3.0.0 高级api
        */
        void onCreate(Resource * res);

        /** 资源移去后
        @version NIIEngine 3.0.0 高级api
        */
        void onRemove(Resource * res);

        /** 资源组改变后
        @version NIIEngine 3.0.0 高级api
        */
        void onGroupChange(GroupID gid, Resource * res);

        /** 所有资源移去后
        @version NIIEngine 3.0.0
        */
        void onAllRemove(ResourceManager * mag);
    protected:
        ResourceSchemeManager();
        virtual ~ResourceSchemeManager();

        /// @copydetails ThreadMain::run
        void run(void * arg);

        /// @copydetails JobPrc::handle
        JobResult * handle(Job * jrq);

        /// @copydetails JobPrc::handle
        void handle(JobResult * jrs);

        /** 初始化后台队列系统
        @remark 一般由引擎内部调用
        @version NIIEngine 3.0.0 高级api
        */
        virtual void init();

        /** 关闭后台队列系统
        @remark 一般由引擎内部调用
        @version NIIEngine 3.0.0 高级api
        */
        virtual void shutdown();
        
        /** 准备资源
        @version NIIEngine 3.0.0
        */
        virtual BgPrcID plan(ResourceType resType, ResourceID rid,GroupID gid, 
			ResLoadScheme * ls = 0, ResResultScheme * rs = 0, const PropertyData * params = 0);

        /** 加载资源
        @version NIIEngine 3.0.0
        */
        virtual BgPrcID load(ResourceType type, ResourceID rid, GroupID gid, 
			ResLoadScheme * ls = 0, ResResultScheme * rs = 0, const PropertyData * params = 0);

        /**
        @version NIIEngine 3.0.0 高级api
        */
        BgPrcID addRequest(ResourceRequest * req);
    private:
        /** 构建资源组
        @version NIIEngine 3.0.0 内部函数
        */
        void setup(LoadGroup * grp);

        /** 添加组资源
        @version NIIEngine 3.0.0 内部函数
        */
        void add(LoadGroup * grp, Resource * res);

        /** 清理组资源
        @version NIIEngine 3.0.0 内部函数
        */
        void clear(LoadGroup * grp);

        /** 删除组
        @version NIIEngine 3.0.0 内部函数
        */
        void destroy(LoadGroup * grp);

        /** 解析脚本
        @version NIIEngine 3.0.0 内部函数
        */
        void parse(LoadGroup * grp);

        /** 获取资源组
        @version NIIEngine 3.0.0 内部函数
        */
        LoadGroup * get(GroupID gid) const;

        /** 是否存在文件
        @version NIIEngine 3.0.0 内部函数
        */
        bool isExist(LoadGroup * grp, const String & file);

        /** 获取文件创建时间
        @version NIIEngine 3.0.0 内部函数
        */
        time_t getCreateTime(LoadGroup * grp, const String & file);

        /** 获取文件修改时间
        @version NIIEngine 3.0.0 内部函数
        */
        time_t getModifyTime(LoadGroup * grp, const String & file);

        /** 获取文件访问时间
        @version NIIEngine 3.0.0 内部函数
        */
        time_t getAccessTime(LoadGroup * grp, const String & file);

        /** 找到文件所在的资源组
        @version NIIEngine 3.0.0 内部函数
        */
        LoadGroup * findImpl(const String & file);
    private:
		typedef set<BgPrcID>::type BgPrcList;
        typedef multimap<NIIf, ScriptParserSys *>::type ScriptSysList;
        typedef map<GroupID, LoadGroup *>::type GroupList;
    private:
        N_rw_mutex(mRWMutex);
        LoadGroup * mTemp;
        Nui32 mPrcType;
        BgPrcList mBgPrcList;
        TimeDurMS mTimeLimitMS;
        GroupID mEngineGroup;
        GroupList mGroups;
        ResMagList mResMagList;
        ScriptSysList mScriptSysList;
        static PrcID mValidPID;
        N_static_mutex(mValidPIDMutex)
    };
}
#endif