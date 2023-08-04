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

#ifndef _NII_ENGINE_H_
#define _NII_ENGINE_H_

#include "NiiPreInclude.h"
#include "NiiSpaceTypeManager.h"
#include "NiiInstanceMesh.h"
#include "NiiResourceSchemeManager.h"
#include "NiiLodSchemeManager.h"

namespace NII
{
    /** NII系统的核心构成
    @remark
        统筹所有管理器类/工厂类
    @note
        一般的管理器都有 create/destroy 或 add/remove 级别的函数,这些函数管理类别
        是不相同的,前者管理器拥有关联对象和内存调配功能.后着只有关联对象功能,没有内
        存调配功能
    @par ID 0x01
    */
    class _EngineAPI Engine : public Singleton<Engine>, public EngineAlloc
    {
        friend class RenderSys;
    public:
        /** 使用模式
        @version NIIEngine 3.0.0
        */
        enum RunMode
        {
            RM_Debug,
            RM_Test,
            RM_Example,
            RM_APP,
            RM_Custom
        };
        typedef vector<Pattern *>::type ProcessList;
        typedef vector<Plugin *>::type PluginList;
        typedef vector<DynamicModule *>::type ModuleList;
        typedef vector<PosNode *>::type RootNodeList;
        typedef set<RenderFrameListener *>::type FrameLisList;
        typedef map<FactoryID, SpaceObjFactory *>::type SpaceObjFactoryList;
    public:
        /** 构造函数
        @param[in] game 默认的引擎游戏文件
        */
        Engine(const String & game = _T("niiengine"), const String & pluginPath = _T("plugins"), const String & appname = _T("Test"));

        ~Engine();

        /** 初始化系统
        @param[in] primaryWin 创建主窗体
        @version NIIEngine 3.0.0
        */
        ViewWindow * init(RunMode mode, bool primaryWin, PropertyData * extparam, const String & title = _T("NIIEngine"),
            const String & cfg = StrUtil::WBLANK);

        /** 是否初始化
        @version NIIEngine 3.0.0
        */
        inline bool isInit() const              { return mInit; }

        /** 渲染帧
        @version NIIEngine 3.0.0
        */
        bool renderFrame();

        /** 渲染帧
        @param[in] cost 上次调用与这次调用度过的时间
        @version NIIEngine 3.0.0
        */
        bool renderFrame(TimeDurMS cost);

        /** 开始渲染
        @version NIIEngine 3.0.0
        */
        void startRender();

        /** 结束渲染
        @version NIIEngine 3.0.0
        */
        void endRender();

        /** 添加渲染系统
        @version NIIEngine 3.0.0
        */
        void addRender(RenderSys * obj);

        /** 获取渲染系统
        @version NIIEngine 3.0.0
        */
        RenderSys * getRender(const String & name);

        /** 设置当前渲染系统
        @version NIIEngine 3.0.0 高级api
        */
        void setRender(RenderSys * obj);

        /** 获取当前渲染系统
        @version NIIEngine 3.0.0
        */
        inline RenderSys * getRender() const            { return mRenderSys; }

        /** 获取可用的渲染系统
        @version NIIEngine 3.0.0
        */
        inline const RenderSysList & getRenderList()    { return mRenderList; }

        /** 获取错误代码描述的实用函数
        @version NIIEngine 3.0.0
        */
        String getRenderError(NIIl num);

        /** 设置渲染系统特色
        @version NIIEngine 3.0.0
        */
        void setFeature(RenderFeature * rsf);

        /** 时间标准
        @version NIIEngine 3.0.0
        */
        inline Timer * getTimer()                       { return mTimer; }

        /** 获取剪切板
        @version NIIEngine 3.0.0
        */
        inline Clipboard * getClipboard() const         { return mClipboard; }

        /** 关闭系统.
        @version NIIEngine 3.0.0
        */
        void shutdown();

        /** 添加场景类型工厂类
        @version NIIEngine 3.0.0
        */
        inline void add(SpaceTypeFactory * fact)       { mSpaceTypeManager->add(fact); }

        /** 移去场景类型工厂类
        @version NIIEngine 3.0.0
        */
        inline void remove(SpaceTypeFactory * fact)    { mSpaceTypeManager->remove(fact->getID()); }

        /** 创建空间管理
        @version NIIEngine 3.0.0
        */
        inline SpaceManager * createSpaceManager(FactoryID tid, const String & name = StrUtil::WBLANK)  { return mSpaceTypeManager->createSpace(tid, name); }

        /** 创建空间管理
        @param[in] tmark 尝试从类型结构匹配类型
        @version NIIEngine 3.0.0
        */
        inline SpaceManager * createSpaceManager(Nmark mark, const String & name = StrUtil::WBLANK)    { return mSpaceTypeManager->createSpace(mark, name); }

        /** 删除空间管理
        @version NIIEngine 3.0.0
        */
        inline void destroySpaceManager(SpaceManager * sm)                  { mSpaceTypeManager->destroy(sm); }

        /** 获取空间管理
        @version NIIEngine 3.0.0
        */
        inline SpaceManager * getSpaceManager(const String & name) const    { return mSpaceTypeManager->getSpace(name);}

        /** 空间管理是否存在
        @param[in] name 检索的实例名字
        */
        inline bool isSpaceManagerExist(const String & name) const          { return mSpaceTypeManager->isSpaceExist(name); }

        /** 创建复用网格
        @version NIIEngine 3.0.0
        */
        InstanceMesh * createInstanceMesh(SpaceManager * sm, InstanceMeshID imid, 
            ResourceID rid, GroupID gid, Nui16 subidx, NCount batch, InstanceMeshType type,
                Nmark mARK = 0);

        /** 获取复用网格
        @version NIIEngine 3.0.0
        */
        InstanceMesh * getInstanceMesh(InstanceMeshID imid) const;

        /** 是否存在复用网格
        @version NIIEngine 3.0.0
        */
        bool isInstanceMeshExist(InstanceMeshID imid) const;

        /** 删除复用网格
        @version NIIEngine 3.0.0
        */
        void destroyInstanceMesh(InstanceMeshID imid);

        /** 删除所有复用网格
        @version NIIEngine 3.0.0
        */
        void destroyAllInstanceMesh();

        /** 更新复用网格
        @version NIIEngine 3.0.0
        */
        void updateInstanceMesh(InstanceMesh * mesh);

        /** 创建复用几何体
        @version NIIEngine 3.0.0
        */
        InstanceGeo * createInstanceGeo(SpaceManager * sm, ResourceID material, InstanceMeshID imid);

        /** 删除复用几何体
        @version NIIEngine 3.0.0
        */
        void destroyInstanceGeo(InstanceGeo * geo);

        /** 开始处理空间
        @version NIIEngine 3.0.0
        */
        void processBegin(SpaceManager * sm);

        /** 结束处理空间
        @version NIIEngine 3.0.0
        */
        void processEnd(SpaceManager * sm);

        /** 获取当前处理空间
        @version NIIEngine 3.0.0 高级api
        */
        SpaceManager * getProcessSpace() const;

        /** 空间管理信息
        @version NIIEngine 3.0.0
        */
        inline const SpaceTypeVerbose * getSpaceVerbose(TypeID tid) const       { return mSpaceTypeManager->getVerbose(tid); }

        /** 获取空间类型列表
        @version NIIEngine 3.0.0 高级api
        */
        inline SpaceTypeManager::SpaceList & getSpaceList()                     { return mSpaceTypeManager->getSpaceList(); }

        /** 获取空间类型列表
        @version NIIEngine 3.0.0
        */
        const SpaceTypeManager::SpaceList & getSpaceList() const                { return mSpaceTypeManager->getSpaceList(); }

        /** 添加空间对象工厂类
        @version NIIEngine 3.0.0
        */
        void addFactory(SpaceObjFactory * fact, bool override = false);

        /** 移去空间对象工厂类
        @version NIIEngine 3.0.0
        */
        void removeFactory(FactoryID fid);

        /** 空间对象工厂类是否存在
        @version NIIEngine 3.0.0
        */
        bool isFactoryExist(FactoryID fid) const;

        /** 获取工厂类对象
        @version NIIEngine 3.0.0
        */
        SpaceObjFactory * getFactory(FactoryID fid);

        /** 获取工厂类型列表
        @version NIIEngine 3.0.0
        */
        inline const SpaceObjFactoryList & getSpaceObjFactoryList() const   { return mSpaceObjFactoryList; }

        /** 设置线程管理器
        @version NIIEngine 3.0.0
        */
        void setThread(ThreadManager * obj);

        /** 获取线程管理器
        @version NIIEngine 3.0.0
        */
        inline ThreadManager * getThread() const                    { return mThreadManager; }

        /** 设置输入控制管理器
        @version NIIEngine 3.0.0
        */
        inline void setInput(InputManager * obj)           { mControlPatternManager = obj; }

        /** 获取输入控制管理器
        @version NIIEngine 3.0.0
        */
        inline InputManager * getInput() const             { return mControlPatternManager; }

        /** 获取输入控制项管理器
        @version NIIEngine 3.0.0
        */
        InputManager * getInputItem() const;

        /** 获取文理管理器
        @version NIIEngine 3.0.0
        */
        TextureManager * getTexture() const;

        /** 获取网格管理器
        @version NIIEngine 3.0.0
        */
        MeshManager * getMesh() const;

        /** 添加帧监听
        @version NIIEngine 3.0.0
        */
        void add(RenderFrameListener * lis);

        /** 移去帧监听
        @version NIIEngine 3.0.0
        */
        void remove(RenderFrameListener * lis);

        /** 保存配置
        @remark 存储配置以后可恢复
        @version NIIEngine 3.0.0
        */
        void saveConfig();

        /** 读取配置
        @version NIIEngine 3.0.0
        */
        bool restoreConfig();

        /** 配置选项
        @version NIIEngine 3.0.0
        */
        bool config(bool useRestore = false);

        /** 创建可写入文件流.
        @param[in] overwrite 如果已经存在是否覆盖
        @param[in] protocol 非普通本地文件/或非本文件使用
        */
        DataStream * createFile(const String & file, GroupID gid = GroupDefault,
            bool overwrite = false, const String & protocol = StrUtil::WBLANK);

        /** 打开文件流
        @param[in] protocol 非普通本地文件/或非本文件使用
        @version NIIEngine 3.0.0
        */
        DataStream * openFile(const String & file, GroupID = GroupDefault,
            const String & protocol = StrUtil::WBLANK);

        /** 把颜色转换32位
        @param[in] src 资源颜色
        @param[in] dst 目标类型,DT_Colour_ARGB/DT_Colour_ABGR
        @version NIIEngine 3.0.0
        */
        static Nui32 getColourValue(const Colour & src, DataType dst);

        /** 首要颜色格式
        @remark DT_Colour_ARGB/DT_Colour_ABGR 其中一种
        @note 和系统/渲染系统有关
        @version NIIEngine 3.0.0
        */
        DataType getColourFormat();

        /** 创建窗体
        @version NIIEngine 3.0.0
        */
        ViewWindow * createWindow(const String & name, NCount width, NCount height,
            bool fullwin, const PropertyData * custom = 0);

        /** 创建多个窗体
        @version NIIEngine 3.0.0
        */
        bool createWindow(const WindowDefineList & dlist, WindowList & out);

        /** 获取窗体
        @version NIIEngine 3.0.0
        */
        ViewWindow * getView(Nid id = 0);

        /** 删除窗体
        @version NIIEngine 3.0.0
        */
        void destroy(ViewWindow * win);

        /** 加载插件
        @remark  确保插件内容不再被引用
        @param[in] name 需要加载的插件名字
        */
        void loadPlugin(const String & name);

        /** 卸载插件.
        @remark 确保插件内容不再被引用
        @param[in] name 需要卸载的插件名字
        */
        void unloadPlugin(const String & name);

        /** 安装插件
        @version NIIEngine 3.0.0
        */
        void install(Plugin * plugin);

        /** 卸载插件.
        @version NIIEngine 3.0.0
        */
        void uninstall(Plugin * plugin);

        /** 获取当前安装插件
        @version NIIEngine 3.0.0
        */
        inline const PluginList & getPluginList() const         { return mPluginList; }

        /** 添加一个处理器
        @remark
            这个方法会带来效率问题,而且同类型操作的处理器可能会冲突,如果是NII出产的
            插件,会自动屏蔽掉这些问题,否则严格依照处理机制检测是否会产生冲突
        @param[in] prc 处理器
        @version NIIEngine 3.0.0
        */
        void add(Pattern * prc);

        /** 移去一个处理器
        @remark 尽可能移去不需要的处理器类实例
        @param[in] prc 处理器
        @version NIIEngine 3.0.0
        */
        void remove(Pattern * prc);

        /** 添加运行脚本类型
        @param[in] obj 一般指的是执行类脚本,不是属性脚本
        @version NIIEngine 3.0.0
        */
        void addScriptModule(ScriptModule * obj);

        /** 获取运行脚本类型
        @version NIIEngine 3.0.0
        */
        ScriptModule * getScriptModule(ScriptModuleID smid) const;

        /** 执行脚本文件
        @version NIIEngine 3.0.0
        */
        void executeScript(ScriptModuleID smid, const String & file, GroupID gid = 0) const;

        /** 执行脚本函数
        @version NIIEngine 3.0.0
        */
        int executeScriptFunc(ScriptModuleID smid, const String & function) const;

        /** 执行脚本串
        @version NIIEngine 3.0.0
        */
        void executeScriptStr(ScriptModuleID smid, const String & str) const;

        /** 设置Xml处理器
        @version NIIEngine 3.0.0
        */
        inline void setXmlPattern(XmlPattern * pattern)     { mXmlPrc = pattern; }

        /** 获取Xml处理器
        @version NIIEngine 3.0.0
        */
        inline XmlPattern * getXmlPattern() const           { return mXmlPrc; }

        /** 获取当前使用帧
        @version NIIEngine 3.0.0
        */
        inline Nmark getCurrentFrame() const                { return mNextFrame; }

        /** 清理帧时间
        @version NIIEngine 3.0.0
        */
        void resetFrameTime();

        /** 设置时间平滑
        @version NIIEngine 3.0.0
        */
        inline void setTimeSmooth(TimeDurMS smooth)         { mTimeSmooth = smooth; }

        /** 获取时间平滑
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getTimeSmooth() const              { return mTimeSmooth; }

        /** 设置最小像素大小
        @version NIIEngine 3.0.0
        */
        inline void setMinPixelSize(NIIf size)              { mMinPixelSize = size; }

        /** 获取最小像素大小
        @version NIIEngine 3.0.0
        */
        inline NIIf getMinPixelSize()                       { return mMinPixelSize; }

        /** 获取显示器数量.
        @version NIIEngine 3.0.0
        */
        inline NCount getDisplayMonitorCount() const        { N_assert(mRenderSys, "error"); return mRenderSys->getDisplayMonitorCount(); }

        /** 添加更新位置节点
        @version NIIEngine 3.0.0
        */
        void addUpdate(PosNode * n);

        /** 移去更新位置节点
        @version NIIEngine 3.0.0
        */
        void removeUpdate(PosNode * n);

        /** 更新所有位置节点
        @version NIIEngine 3.0.0
        */
        void updateNode();

        /// @copydetails Singleton::getOnly
        static Engine & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static Engine * getOnlyPtr();
    protected:
        /** 加载插件实现
        @version NIIEngine 3.0.0
        */
        void loadPlugins(const String & file = _T("plugins.cfg"));

        /** 卸载插件实现
        @version NIIEngine 3.0.0
        */
        void unloadPlugins();

        /** 引发一个帧的开始事件的方法.
        @version NIIEngine 3.0.0
        */
        bool renderFrameBegin();

        /** 引发一个帧的开始事件的方法
        @version NIIEngine 3.0.0
        */
        bool renderFrameBegin(RenderFrameArgs * arg);

        /** 更新所有物体
        @version NIIEngine 3.0.0
        */
        bool updateFrameImpl();

        /** 更新各个帧对象
        @version NIIEngine 3.0.0
        */
        bool updateFrameImpl(RenderFrameArgs * evt);

        /** 渲染核心物时触发
        @version NIIEngine 3.0.0
        */
        bool onRenderFrameCore();

        /** 渲染核心物时触发.
        @version NIIEngine 3.0.0
        */
        bool onRenderFrameCore(RenderFrameArgs * arg);

        /** 引发一个帧的结束事件的方法.
        @version NIIEngine 3.0.0
        */
        bool renderFrameEnd();

        /** 引发一个帧的结束事件的方法.
        @version NIIEngine 3.0.0
        */
        bool renderFrameEnd(RenderFrameArgs * arg);

        /** 更新复用网格状态
        @version NIIEngine 3.0.0
        */
        void updateInstanceMeshImpl();

        /** 事件平均帧率
        @version NIIEngine 3.0.0
        */
        TimeDurMS calcFrameAvg(Nidx type);
    protected:
        typedef deque<SpaceManager *>::type SpaceQueue;
        typedef map<Nid, ViewWindow *>::type WinList;
        typedef vector<InstanceMesh *>::type InstanceSpaceList;
        typedef map<InstanceMeshID, InstanceMesh *>::type InstanceSpaceTypeList;
        typedef vector<TimeDurMS>::type TimeList;
        typedef vector<ScriptModule *>::type ScriptModuleList;
    protected:
        RenderSys * mRenderSys;
        String mVersion;
        String mConfFile;
        SpaceQueue mSpaceQueue;
        RenderSysList mRenderList;
        FrameLisList mFrameLisList;
        ScriptModuleList mScriptModuleList;

        SpaceTypeManager * mSpaceTypeManager;
        InputManager * mControlPatternManager;
        ScriptHelperManager * mScriptHelperManager;
        DynamicModuleManager * mDynamicModuleManager;
        EquationManager * mEquationManager;
        ParticleSpaceManager * mParticleManager;
        MaterialManager * mMaterialManager;
        SkeletonManager * mSkeletonManager;
        AnimationManager * mAnimationManager;
        PixelManager * mPixelBufferManager;
        MeshManager * mMeshManager;
        FontManager * mFontManager;
        LogManager * mLogManager;
        UIManager * mUIManager;
        VFSFactory * mVFSFactory;
        VFSManager * mVFSManager;
        OverlayManager * mOverlayManager;
        OverlayGeoFactory * mPreDefineOverlayGeoFactory[3];
        HighLevelGpuProgramManager * mHLGPManager;
        ResourceSchemeManager * mResSchemeManager;
        FusionManager * mFusionManager;
        RenderSysManager * mRenderSysManager;
        ScriptCompilerManager * mCompilerManager;
        LodSchemeManager * mLodSchemeManager;

        Timer * mTimer;
        WinList mWins;
        TimeList * mFrameTimeList;
        Profiler * mProfiler;
        Clipboard * mClipboard;
        XmlPattern * mXmlPrc;
        TimeDurMS mTimeSmooth;
        Nmark mSpaceObjMark;
        Nmark mNextFrame;
        NIIf mMinPixelSize;

        ModuleList mModuleList;
        PluginList mPluginList;
        ProcessList mProcesses;
        SpaceObjFactory * mPreDefineSpaceFactory[7];
        SpaceObjFactoryList mSpaceObjFactoryList;
        ThreadManager * mThreadManager;
        RootNodeList mRootNodeUpdate;
        InstanceSpaceList mUpdateInstanceMeshs;
        InstanceSpaceList mDirtyInstanceMeshs;
        InstanceSpaceTypeList mInstanceSpaceTypeList;
        bool mInit;
        bool mRenderEnd;
        bool mPrimaryWinInit;
    };
}
#endif