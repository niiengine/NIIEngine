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
#include "NiiEngine.h"
#include "NiiViewWindow.h"
#include "NiiEquationManager.h"
#include "NiiLogManager.h"
#include "NiiDynamicModuleManager.h"
#include "NiiThreadManager.h"
#include "NiiDynamicModule.h"
#include "NiiConfigFile.h"
#include "NiiMaterialManager.h"
#include "NiiRenderSysManager.h"
#include "NiiMeshManager.h"
#include "NiiTextureManager.h"
#include "NiiParticleSpaceManager.h"
#include "NiiSkeletonManager.h"
#include "NiiOverlayGeoFactory.h"
#include "NiiOverlayManager.h"
#include "NiiAnimationManager.h"
#include "NiiProfiler.h"
#include "NiiErrorDialog.h"
#include "NiiConfigDialog.h"
#include "NiiStrConv.h"
#include "NiiVFSManager.h"
#include "NiiPlugin.h"
#include "NiiShadowVolumeExtrudeProgram.h"
#include "NiiResourceSchemeManager.h"
#include "NiiScriptCompilerManager.h"
#include "NiiGeometrySpace.h"
#include "NiiParticleCtrl.h"
#include "NiiPixelChain.h"
#include "NiiPixelOrbit.h"
#include "NiiLight.h"
#include "NiiScriptModule.h"
#include "NiiCustomSpaceObj.h"
#include "NiiConvexBody.h"
#include "NiiPattern.h"
#include "NiiClipboard.h"
#include "NiiUIManager.h"
#include "NiiInputManager.h"
#include "NiiGBufferManager.h"
#include "NiiScriptHelperManager.h"
#include "NiiSimdUtil.h"
#include "NiiPath.h"

#include "NiiInstanceGeo.h"
#include "NiiFontManager.h"
#include "NiiPixelManager.h"
#include "NiiGpuProgramManager.h"
#include "NiiFusionManager.h"
#include "NiiScriptCompiler.h"
#include "NiiSysOption.h"
#include "NiiDefaultStream.h"
#include "NiiTimer.h"
#include "NiiPlatInfo.h"

#if N_PLAT == N_PLAT_IOS
#include "macUtils.h"
#endif

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    enum FrameEventTimeType
    {
        FETT_STARTED = 0,
        FETT_QUEUED = 1,
        FETT_ENDED = 2,
        FETT_COUNT = 3
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // Engine
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    template<> Engine * Singleton<Engine>::mOnly = 0;
    Engine * Engine::getOnlyPtr()
    {
        return mOnly;
    }
    //-----------------------------------------------------------------------
    Engine & Engine::getOnly()
    {
        N_assert(mOnly, "error logic");
        return (*mOnly);
    }
    //-----------------------------------------------------------------------
    typedef void (*DLL_START_PLUGIN)(void);
    typedef void (*DLL_STOP_PLUGIN)(void);
    //-----------------------------------------------------------------------
    Engine::Engine(const String & game, const String & pluginPath, const String & appname) :
        mLogManager(0),
        mRenderSysManager(0),
        mInputManager(0),
        mEquationManager(0),
        mUIManager(0),
        mNextFrame(0),
        mXmlPrc(0),
        mTimeSmooth(0),
        mMinPixelSize(0),
        mRenderSys(0),
        mSpaceObjMark(1),
        mInit(false)
    {
        Path::setAppName(appname);
        Path::setPluginPath(pluginPath);
        PlatInfo::startup();
        SIMDUtil::startup();
        Path::startup();

        mFrameTimeList = N_new_array_t(TimeList, FETT_COUNT);
        mVersion = N_conv((Nui32)NII_VERSION_MAJOR) + _T(".") + N_conv((Nui32)NII_VERSION_MINOR) + _T(".") +
            N_conv((Nui32)NII_VERSION_PATCH) + _T("(") + NII_VERSION_NAME + _T(")");
        mConfFile = Path::getExecutablePath() + game + _T(".cfg");

        // Create log manager and default log file if there is no log manager yet
        if(LogManager::getOnlyPtr() == 0)
        {
            mLogManager = N_new LogManager();
            mLogManager->create(Path::getExecutablePath() + game + _T(".log"), true, true);
        }
        mScriptHelperManager = N_new ScriptHelperManager();
        mDynamicModuleManager = N_new DynamicModuleManager();
        mVFSManager = N_new VFSManager();
        mResSchemeManager = N_new ResourceSchemeManager();
        mClipboard = N_new Clipboard();
        // ThreadManager (note: users can replace this if they want)
        mThreadManager = N_new ThreadManager();

        // match threads to hardware
    #if NII_THREAD_SUPPORT
        unsigned threadCount = NII_THREAD_HARDWARE_CONCURRENCY;
        if(!threadCount)
            threadCount = 1;
        mThreadManager->setFireCount(threadCount);
    #endif
        // Create SpaceManager enumerator (note - will be managed by singleton)
        mSpaceTypeManager = N_new SpaceTypeManager();
        mRenderSysManager = N_new RenderSysManager();
        mMaterialManager = N_new MaterialManager();
        mMeshManager = N_new MeshManager();
        mSkeletonManager = N_new SkeletonManager();
        mParticleManager = N_new ParticleSpaceManager();

        mTimer = N_new Timer();

        mOverlayManager = N_new OverlayManager();

        mPreDefineOverlayGeoFactory[0] = N_new ContainerOverlayGeoFactory();
        mOverlayManager->add(mPreDefineOverlayGeoFactory[0]);
        mPreDefineOverlayGeoFactory[1] = N_new FrameOverlayGeoFactory();
        mOverlayManager->add(mPreDefineOverlayGeoFactory[1]);
        mPreDefineOverlayGeoFactory[2] = N_new TextOverlayGeoFactory();
        mOverlayManager->add(mPreDefineOverlayGeoFactory[2]);

        mFontManager = N_new FontManager();
        mPixelBufferManager = N_new PixelManager();
        mAnimationManager = N_new AnimationManager();
        mLodSchemeManager = N_new LodSchemeManager();

    #if NII_PROFILING
        // Profiler
        mProfiler = N_new Profiler();
        Profiler::getOnly().setTimer(mTimer);
    #endif
        mVFSFactory = N_new FileSystemArchiveFactory();
        N_Only(VFS).add(mVFSFactory);

        mHLGPManager = N_new HighLevelGpuProgramManager();
        mFusionManager = N_new FusionManager();
        mCompilerManager = N_new ScriptCompilerManager();

        // instantiate and register base movable factories
        mPreDefineSpaceFactory[0] = N_new GeometrySpaceFactory();
        addFactory(mPreDefineSpaceFactory[0]);
        mPreDefineSpaceFactory[1] = N_new LightFactory();
        addFactory(mPreDefineSpaceFactory[1]);
        mPreDefineSpaceFactory[2] = N_new ParticleCtrlFactory();
        addFactory(mPreDefineSpaceFactory[2]);
        mPreDefineSpaceFactory[3] = N_new CustomSpaceObjFactory();
        addFactory(mPreDefineSpaceFactory[3]);
        mPreDefineSpaceFactory[4] = N_new PixelChainFactory();
        addFactory(mPreDefineSpaceFactory[4]);
        mPreDefineSpaceFactory[5] = N_new PixelOrbitFactory();
        addFactory(mPreDefineSpaceFactory[5]);

        // Load plugins
        loadPlugins(Path::getPluginPath() + game + _T(".plugin"));

        N_Only(Log).log(_I18n("⊙⊙⊙⊙⊙⊙ NIIEngine大型多媒体框架 ⊙⊙⊙⊙⊙⊙"));
        N_Only(Log).log(String(_I18n("版本: ") + mVersion));

        mPrimaryWinInit = false;

        ScriptModuleList::iterator i, iend = mScriptModuleList.end();
        for(i = mScriptModuleList.begin(); i != iend; ++i)
        {
            (*i)->load();
        }
    }
    //-----------------------------------------------------------------------
    Engine::~Engine()
    {
        PlatInfo::shutdown();
        SIMDUtil::shutdown();
        Path::removeTempDir();
        shutdown();
        N_Only(Material).recover();
        destroyAllInstanceMesh();
        N_delete_array_t(mFrameTimeList, TimeList, FETT_COUNT);

        SpaceManager::destroyAllQueueFusion();

        N_delete mSpaceTypeManager;
        N_delete mRenderSysManager;

        N_delete mFusionManager;

    #if NII_PROFILING
        N_delete mProfiler;
    #endif
        N_delete mOverlayManager;
        N_delete mFontManager;
        N_delete mPixelBufferManager;
        N_delete mAnimationManager;
        N_delete mLodSchemeManager;
        
        N_delete mVFSManager;
        N_delete mVFSFactory;
        N_delete mSkeletonManager;
        N_delete mMeshManager;
        N_delete mParticleManager;

        if(mEquationManager)
            N_delete mEquationManager;
        N_delete mHLGPManager;
        if(mUIManager)
            N_delete mUIManager;
        N_delete mPreDefineOverlayGeoFactory[2];
        N_delete mPreDefineOverlayGeoFactory[1];
        N_delete mPreDefineOverlayGeoFactory[0];

        unloadPlugins();
        N_delete mMaterialManager;
        N_delete mResSchemeManager;

        N_delete mPreDefineSpaceFactory[0];
        N_delete mPreDefineSpaceFactory[1];
        N_delete mPreDefineSpaceFactory[2];
        N_delete mPreDefineSpaceFactory[3];
        N_delete mPreDefineSpaceFactory[4];
        N_delete mPreDefineSpaceFactory[5];

        N_delete mThreadManager;

        N_delete mTimer;
        N_delete mScriptHelperManager;
        N_delete mDynamicModuleManager;
        N_delete mLogManager;

        N_delete mCompilerManager;

        N_delete mClipboard;

        mPrimaryWinInit = false;

        PropertyCmdObj::cleanAll();

        ScriptModuleList::const_iterator z, zend = mScriptModuleList.end();
        for(z = mScriptModuleList.begin(); z != zend; ++z)
        {
            (*z)->unload();
        }
    }
    //-----------------------------------------------------------------------
    void Engine::saveConfig()
    {
        if (mConfFile.empty())
            return;

    #if N_PLAT == N_PLAT_IOS
        // Check the Documents directory within the application sandbox
        String outBaseName, path;
        StrUtil::splitFilename(mConfFile, outBaseName, path);
        String configFileName = macBundlePath() + "/../Documents/" + outBaseName;
        Nofstream of1(configFileName.c_str());
        if (of1.is_open())
        {
            mConfFile = configFileName;
            /*if (mRenderSys)
            {
                of1 << "Default Render System=" << mRenderSys->getName() << std::endl;
            }
            else
            {
                of1 << "Default Render System=" << std::endl;
            }*/

            RenderSysList::const_iterator it, itend = getRenderList().end();
            for (it = getRenderList().begin(); it != itend; ++it)
            {
                RenderSys * rs = *it;
                of1 << std::endl;
                of1 << "[" << rs->getName() << "]" << std::endl;
                const SysOptionList & opts = rs->getConfig();
                SysOptionList::const_iterator z, zend = opts.end();
                for (z = opts.begin(); z != zend; ++z)
                {
                    of1 << z->second.mName << "=" << z->second.mValue << std::endl;
                }
                if (mRenderSys && mRenderSys->getName() == rs->getName())
                {
                    of1 << "Default = True" << std::endl;
                }
                else
                {
                    of1 << "Default = False" << std::endl;
                }
            }
            of1.close();
            return;
        }
        of1.close();
    #endif
        Nofstream of(mConfFile.c_str());

        if(!of)
            N_EXCEPT(IO, _I18n("Cannot create settings file."));

        /*if(mRenderSys)
        {
            of << "Default Render System=" << mRenderSys->getName() << std::endl;
        }
        else
        {
            of << "Default Render System=" << std::endl;
        }*/

        RenderSysList::const_iterator it, itend = getRenderList().end();
        for (it = getRenderList().begin(); it != itend; ++it)
        {
            RenderSys * rs = *it;
            of << std::endl;
            of << "[" << rs->getName() << "]" << std::endl;
            const SysOptionList & opts = rs->getConfig();
            SysOptionList::const_iterator z, zend = opts.end();
            for (z = opts.begin(); z != zend; ++z)
            {
                of << z->second.mName << "=" << z->second.mValue << std::endl;
            }
            if (mRenderSys && mRenderSys->getName() == rs->getName())
            {
                of << "Default=True" << std::endl;
            }
            else
            {
                of << "Default=False" << std::endl;
            }
        }

        of.close();
    }
    //-----------------------------------------------------------------------
    bool Engine::restoreConfig(void)
    {
        if (mConfFile.empty())
            return true;
    #if N_PLAT == N_PLAT_IOS
        // Read the config from Documents first(user config) if it exists on iPhone.
        // If it doesn't exist or is invalid then use mConfFile

        String outBaseName, extension;
        StrUtil::splitFilename(mConfFile, outBaseName, extension);

        String configFileName = macBundlePath() + "/../Documents/" + outBaseName;
        std::ifstream fp;
        fp.open(configFileName.c_str(), std::ios::in);
        if(fp.is_open())
        {
            // A config file exists in the users Documents dir, we'll use it
            mConfFile = configFileName;
        }
        else
        {
            std::ifstream configFp;

            // This might be the first run because there is no config file in the
            // Documents directory.  It could also mean that a config file isn't being used at all

            // Try the path passed into init
            configFp.open(mConfFile.c_str(), std::ios::in);

            // If we can't open this file then we have no default config file to work with
            // Use the documents dir then.
            if(!configFp.is_open())
            {
                // Check to see if one was included in the app bundle
                mConfFile = macBundlePath() + "/nii.cfg";

                configFp.open(mConfFile.c_str(), std::ios::in);

                // If we can't open this file then we have no default config file to work with
                // Use the Documents dir then.
                if(!configFp.is_open())
                    mConfFile = configFileName;
            }
            configFp.close();
        }
        fp.close();
    #endif
        // Restores configuration from saved state
        // Returns true if a valid saved configuration is
        //   available, and false if no saved config is
        //   stored, or if there has been a problem
        ConfigFile cfg;
        try
        {
            // Don't trim whitespace
            cfg.load(mConfFile, _T("\t:="), false);
        }
        catch(Exception & e)
        {
            if (typeid(e) == typeid(InvalidFileException))
            {
                return false;
            }
            else
            {
                throw;
            }
        }
        RenderSys * defaultsys = 0;
        const ConfigFile::SectionSetList & iSection = cfg.getSectionList();
        ConfigFile::SectionSetList::const_iterator si, siend = iSection.end();
        for (si = iSection.begin(); si != siend; ++si)
        {
            RenderSys * rs = getRender(si->first);
            if (rs)
            {
                const ConfigFile::SetList & settings = *si->second;
                ConfigFile::SetList::const_iterator i, iend = settings.end();
                for (i = settings.begin(); i != iend; ++i)
                {
                    if (!rs->setConfig(i->first, i->second))
                    {
                        if (i->first == _T("Default") && i->second == _T("True"))
                        {
                            defaultsys = rs;
                        }
                    }
                }
            }
        }

        //RenderSys * rs = getRender(cfg.getValue(_T("Default Render System")));
        if(defaultsys)
        {
            if(defaultsys->checkConfig().length() > 0)
                return false;

            setRender(defaultsys);
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    bool Engine::config(bool useRestore)
    {
        bool oldc = restoreConfig();
        if (oldc && useRestore)
            return true;
        ConfigDialog * cfgdlg = N_new ConfigDialog();
        bool ok = cfgdlg->display();
        if (ok)
        {
            saveConfig();
        }
        N_delete cfgdlg;
        return ok;
    }
    //-----------------------------------------------------------------------
    RenderSys * Engine::getRender(const String & name)
    {
        if(name.empty())
        {
            return 0;
        }

        RenderSysList::const_iterator i, iend = getRenderList().end();
        for(i = getRenderList().begin(); i != iend; ++i)
        {
            RenderSys * rs = *i;
            if(rs->getName() == name)
                return rs;
        }

        return 0;
    }
    //-----------------------------------------------------------------------
    void Engine::addRender(RenderSys * obj)
    {
        mRenderList.push_back(obj);
    }
    //-----------------------------------------------------------------------
    SpaceManager * Engine::getProcessSpace() const
    {
        if(mSpaceQueue.empty())
            return 0;
        else
            return mSpaceQueue.back();
    }
    //-----------------------------------------------------------------------
    void Engine::processBegin(SpaceManager * sm)
    {
        mSpaceQueue.push_back(sm);
    }
    //-----------------------------------------------------------------------
    void Engine::processEnd(SpaceManager * sm)
    {
        N_assert(getProcessSpace() == sm, "Mismatched push/pop of SpaceManager");

        mSpaceQueue.pop_back();
    }
    //-----------------------------------------------------------------------
    void Engine::setRender(RenderSys * obj)
    {
        // Sets the active rendering system
        // Can be called direct or will be called by
        //   standard config dialog

        // Is there already an active renderer?
        // If so, disable it and init the new one
        if(mRenderSys && mRenderSys != obj)
        {
            mRenderSys->shutdown();
        }
        mRenderSys = obj;
        SpaceTypeManager::getOnly().setRender(obj);
    }
    //-----------------------------------------------------------------------
    ViewWindow * Engine::init(RunMode mode, bool primaryWin, PropertyData * extparam,
        const String & title, const String & cfgfile)
    {
        if(!mRenderSys)
            N_EXCEPT(InvalidState, _I18n("Cannot init - no render system has been selected."));

        if(!mEquationManager)
            mEquationManager = N_new EquationManager();

        ConvexBody::setupPool();

        RenderSysManager & rscManager = N_Only(RenderSys);
        // caller wants to load custom RenderFeature form a config file
        if(cfgfile != StrUtil::WBLANK)
        {
            ConfigFile cfg;
            cfg.load(cfgfile, _T("\t:="), false);

            // GpuFeature Database setting must be in the same format as
            // resources.cfg in Nii examples.
            const ConfigFile::SetList & base = cfg.getValueList(_T("GpuFeature Database"));
            ConfigFile::SetList::const_iterator ii, iiend = base.end();
            for(ii = base.begin(); ii != iiend; ++ii)
            {
                Nui32 archType;
                StrConv::conv(ii->first, archType);
                String filename = ii->second;

                rscManager.load(filename, (VFSType)archType, true);
            }

            String capsName = cfg.getValue(_T("Custom GpuFeature"));

            RenderFeature * rsc = rscManager.getFeature(capsName);
            if(rsc == 0)
            {
                N_EXCEPT(NotExist, String(_I18n("Cannot load a RenderSystemCapability named ")) + capsName);
            }
            setFeature(rsc);
        }
        PlatInfo::log(N_Only(Log).get());
        mRenderSys->init();

        mTimer->reset();
        mInit = true;
        extparam->add(*mRenderSys->getConfigData());
        ViewWindow * win = mRenderSys->createWindow(title, 0, 0, false, extparam);
 
        if(primaryWin && !mPrimaryWinInit)
        {
            // 后台加载器
			mResSchemeManager->init();
            // 启用后台资源线程
            mThreadManager->startup(0, 4, mResSchemeManager);
            // 初始化材质管理器
            mMaterialManager->init();
            // 初始网格管理器
            mMeshManager->init();
            // 合成器
            mFusionManager->init();
            // 初始粒子系统管理器
            mParticleManager->init();

            mUIManager = N_new UIManager(mRenderSys, win);
            // 初始插件 - 在窗体创建后，因此资源系统资源不可用
            PluginList::iterator i, end = mPluginList.end();
            for (i = mPluginList.begin(); i != end; ++i)
            {
                (*i)->init();
            }
            mPrimaryWinInit = true;
            win->setFirstWindow(true);
        }

        mWins.insert(Npair(win->getID(), win));
        return win;
    }
    //-----------------------------------------------------------------------
    void Engine::setFeature(RenderFeature * rsf)
    {
        mRenderSys->setCustomFeature(rsf);
    }
    //-----------------------------------------------------------------------
    String Engine::getRenderError(NIIl num)
    {
        if(mRenderSys)
            return mRenderSys->getErrorString(num);

        return _T("");
    }
    //-----------------------------------------------------------------------
    TextureManager * Engine::getTexture() const
    {
        return N_OnlyPtr(Texture);
    }
    //-----------------------------------------------------------------------
    MeshManager * Engine::getMesh() const
    {
        return N_OnlyPtr(Mesh);
    }
    //-----------------------------------------------------------------------
    void Engine::add(RenderFrameListener * lis)
    {
        FrameLisList::iterator i = mFrameLisList.find(lis);

        if(i != mFrameLisList.end())
        {
            lis->mEnable = true;
        }
        else
        {
            lis->mEnable = true;
            mFrameLisList.insert(lis);
        }
    }
    //-----------------------------------------------------------------------
    void Engine::remove(RenderFrameListener * lis)
    {
        // Remove, 1 only (set), and only when this listener was added before.
        if(mFrameLisList.find(lis) != mFrameLisList.end())
            lis->mEnable = false;
    }
    //---------------------------------------------------------------------
    TimeDurMS Engine::calcFrameAvg(Nidx type)
    {
        Nul now = mTimer->getMS();

        TimeList & times = mFrameTimeList[type];
        times.push_back(now);

        if(times.size() == 1)
        {
            return times[0];
        }

        TimeList::iterator it = times.begin(), end = times.end() - 2; // 至少需要2个时间
        NCount c = times.size();
        while(it != end)
        {
            if(now - *it > mTimeSmooth || c > 6)
            {
                ++it;
                --c;
            }
            else
                break;
        }
        times.erase(times.begin(), it);

        return (times.back() - times.front()) / (times.size()-1);
    }
    //-----------------------------------------------------------------------
    void Engine::beginRender()
    {
        N_assert(mRenderSys != 0, "error logic");

        //mRenderSys->_initAllFrame();
        resetFrameTime();

        mRenderEnd = false;

        while(!mRenderEnd)
        {
            WindowUtil::messageProcess();

            if(!renderFrame())
                break;
        }
    }
    //-----------------------------------------------------------------------
    void Engine::endRender()
    {
        mRenderEnd = true;
    }
    //-----------------------------------------------------------------------
    bool Engine::renderFrame()
    {
        RenderFrameArgs evt;
        ProcessList::iterator i, end = mProcesses.end();
        for(i = mProcesses.begin(); i != end; ++i)
        {
            (*i)->_update();
        }
        N_Only(Animation).update(mTimer->getMS());

        NiiProfileBeginGroup("Frame", NIIPROF_GENERAL);

        FrameLisList::iterator i, temp, iend = mFrameLisList.end();
        for(i = mFrameLisList.begin(); i != iend;)
        {
            if((*i)->mEnable == false)
            {
                temp = i++;
                mFrameLisList.erase(temp);
            }
        }
        
        evt.mCurrent = calcFrameAvg(FETT_STARTED);
        
        FrameLisList::iterator i, temp, iend = mFrameLisList.end();
        for(i = mFrameLisList.begin(); i != iend;)
        {
            if(!(*i)->onPreRender(&evt))
                return false;
            else
                ++i;
        }
                
        evt.mCurrent = calcFrameAvg(FETT_QUEUED);

        updateInstanceMeshImpl();

        //mRenderSys->_updateAllFrame(false);
                
        ++mNextFrame;
        FrameLisList::iterator i, temp, iend = mFrameLisList.end();
        for(i = mFrameLisList.begin(); i != iend;)
        {
            if(!(*i)->onRenderCore(evt))
                return false;
            else
                ++i;
        }

        if(mUIManager->mEnable)
            mUIManager->render();

        //mRenderSys->_swapAllFrame(mRenderSys->isVSync());

        for(SpaceTypeManager::SpaceList::iterator it = getSpaceList().begin(); it != getSpaceList().end(); ++it)
            it->second->prcLodEvent();
                
        evt.mCurrent = calcFrameAvg(FETT_ENDED);
              
        FrameLisList::iterator i, temp, iend = mFrameLisList.end();
        for(i = mFrameLisList.begin(); i != iend;)
        {
            if(!(*i)->onEndRender(&evt))
                return false;
            else
                ++i;
        }

        if(GBufferManager::getOnlyPtr())
            N_Only(GBuffer).recoverTemp(evt->mCurrent);

        mThreadManager->update();

        NiiProfileEndGroup("Frame", NIIPROF_GENERAL);

        return true;
    }
    //---------------------------------------------------------------------
    bool Engine::renderFrame(TimeDurMS time)
    {
        RenderFrameArgs evt;
        evt.mCurrent = time;
        
        ProcessList::iterator i, end = mProcesses.end();
        for(i = mProcesses.begin(); i != end; ++i)
        {
            (*i)->_update();
        }
        N_Only(Animation).update(mTimer->getMS());

        NiiProfileBeginGroup("Frame", NIIPROF_GENERAL);

        FrameLisList::iterator i, temp, iend = mFrameLisList.end();
        for(i = mFrameLisList.begin(); i != iend;)
        {
            if((*i)->mEnable == false)
            {
                temp = i++;
                mFrameLisList.erase(temp);
            }
        }
        
        FrameLisList::iterator i, temp, iend = mFrameLisList.end();
        for(i = mFrameLisList.begin(); i != iend;)
        {
            if(!(*i)->onPreRender(&evt))
                return false;
            else
                ++i;
        }
        
        updateInstanceMeshImpl();

        //mRenderSys->_updateAllFrame(false);

        ++mNextFrame;

        FrameLisList::iterator i, temp, end = mFrameLisList.end();
        for(i= mFrameLisList.begin(); i != end;)
        {
            if(!(*i)->onRenderCore(evt))
                return false;
            else
                ++i;
        }
        
        if(mUIManager->mEnable)
            mUIManager->render();

        //mRenderSys->_swapAllFrame(mRenderSys->isVSync());

        for(SpaceTypeManager::SpaceList::iterator it = getSpaceList().begin(); it != getSpaceList().end(); ++it)
            it->second->prcLodEvent();
        
        FrameLisList::iterator i, temp, iend = mFrameLisList.end();
        for(i= mFrameLisList.begin(); i != end;)
        {
            if(!(*i)->onEndRender(&evt))
                return false;
            else
                ++i;
        }

        if(GBufferManager::getOnlyPtr())
            N_Only(GBuffer).recoverTemp(evt.mCurrent);

        mThreadManager->update();

        NiiProfileEndGroup("Frame", NIIPROF_GENERAL);

        return true;
    }
    //-----------------------------------------------------------------------
    void Engine::shutdown()
    {
		mResSchemeManager->shutdown();
        mThreadManager->shutdown(0);

        SpaceTypeManager::getOnly().destroyAll();

        PluginList::reverse_iterator i, end = mPluginList.rend();
        for (i = mPluginList.rbegin(); i != end; ++i)
        {
            (*i)->shutdown();
        }

        ShadowVolumeExtrudeProgram::shutdown();

        ConvexBody::shutdownPool();

        mInit = false;

        N_Only(Log).log(_I18n("*-*-* NIIEngine Shutdown"));
    }
    //-----------------------------------------------------------------------
    void Engine::loadPlugins(const String & pluginsfile)
    {
        ConfigFile cfg;

        try
        {
            cfg.load(pluginsfile);
        }
        catch (Exception)
        {
            N_Only(Log).log(pluginsfile + _I18n(" Error! plugin loading disabled."));
            return;
        }

        String pluginDir = cfg.getValue(_T("PluginFolder")); // 在Mac OS X忽略, 使用资源/目录
        StringList pluginList = cfg.getValues(_T("Plugin"));

        if (!pluginDir.empty() && *pluginDir.rbegin() != N_DIR_SEP_C)
        {
            pluginDir += N_DIR_SEP_S;
        }
        else
        {
            pluginDir = Path::getPluginPath();
        }

        StringList::iterator it, itend = pluginList.end();
        for(it = pluginList.begin(); it != itend; ++it )
        {
            loadPlugin(pluginDir + (*it));
        }
    }
    //-----------------------------------------------------------------------
    void Engine::unloadPlugins()
    {
        ModuleList::reverse_iterator i, end = mModuleList.rend();
        for(i = mModuleList.rbegin(); i != end; ++i)
        {
            DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*i)->getSymbol("NiiDynModuleEnd");
            pFunc();
            N_Only(DynamicModule).unload(*i);
        }
        mModuleList.clear();

        PluginList::reverse_iterator l, lend = mPluginList.rend();
        for(l = mPluginList.rbegin(); l != lend; ++l)
        {
            (*l)->uninstall();
        }
        mPluginList.clear();
    }
    //---------------------------------------------------------------------
    DataStream * Engine::createFile(const String & file, GroupID gid, bool overwrite, const String & pattern)
    {
        // 这个文件包含指定路径?
        String path, basename;
        StrUtil::splitFilename(file, basename, path);

        // 没有路径元素，先尝试资源系统
        DataStream * stream = 0;
        if(path.empty())
        {
            try
            {
                stream = N_Only(ResourceScheme).createSrc(file, gid, overwrite, pattern);
            }
            catch (...) {}
        }

        if(stream == 0)
        {
            std::fstream * fs = N_new_t(std::fstream);
            fs->open(file.c_str(), std::ios::out | std::ios::binary);
            if(!*fs)
            {
                N_delete_t(fs, basic_fstream);
                N_EXCEPT(IO, _I18n("不能打开写入: ") + file);
            }
            stream = N_new FileDataStream(fs, file);
        }
        return stream;
    }
    //---------------------------------------------------------------------
    DataStream * Engine::openFile(const String & file, GroupID gid, const String & pattern)
    {
        DataStream * stream;
        if(N_Only(ResourceScheme).isExist(gid, file))
        {
            stream = N_Only(ResourceScheme).open(file, gid);
        }
        else
        {
            std::ifstream * ifs = N_new_t(std::ifstream);
            ifs->open(file.c_str(), std::ios::in | std::ios::binary);
            if(!*ifs)
            {
                N_delete_t(ifs, basic_ifstream);
                N_EXCEPT(InvalidFile, _T("'") + file + _I18n("' file not found!"));
            }
            stream = N_new FileDataStream(ifs, file);
        }
        return stream;
    }
    //-----------------------------------------------------------------------
    Nui32 Engine::getColourValue(const Colour & src, DataType dst)
    {
        switch(dst)
        {
#if N_PLAT == N_PLAT_WIN32
        default:
#endif
        case DT_Colour_ARGB:
            return src.getARGB();
#if N_PLAT != N_PLAT_WIN32
        default:
#endif
        case DT_Colour_ABGR:
            return src.getABGR();
        };
    }
    //-----------------------------------------------------------------------
    ViewWindow * Engine::getView(Nid id)
    {
        if (id == 0)
            return mWins.size() ? mWins.begin()->second : NULL;
        WinList::iterator i = mWins.find(id);
        if(i != mWins.end())
            return i->second;

        return 0;
    }
    //-----------------------------------------------------------------------
    ViewWindow * Engine::createWindow(const String & name, NCount w, NCount h, bool fullwin, const PropertyData * misc)
    {
        if(!mRenderSys)
        {
            N_EXCEPT(InvalidState, _I18n("Cannot create window - no render system has been selected."));
        }
        ViewWindow * re = mRenderSys->createWindow(name, w, h, fullwin, misc);

        if(!mPrimaryWinInit)
        {
            // 后台加载器
			mResSchemeManager->init();
            // 启用后台资源线程
            mThreadManager->startup(0, 4, mResSchemeManager);
            // 初始化材质管理器
            mMaterialManager->init();
            // 初始网格管理器
            mMeshManager->init();
            // 合成器
            mFusionManager->init();
            // 初始粒子系统管理器
            mParticleManager->init();
            // 初始插件 - 在窗体创建后，因此资源系统资源不可用
            PluginList::iterator i, end = mPluginList.end();
            for (i = mPluginList.begin(); i != end; ++i)
            {
                (*i)->init();
            }
            mPrimaryWinInit = true;
            re->setFirstWindow(true);
        }
        return re;
    }
    //-----------------------------------------------------------------------
    bool Engine::createWindow(const WindowDefineList & dlist, WindowList & out)
    {
        if(!mRenderSys)
        {
            N_EXCEPT(InvalidState, _I18n("Cannot create render windows - no render system has been selected."));
        }

        bool success;
        success = mRenderSys->createWindow(dlist, out);
        if(success && !mPrimaryWinInit)
        {
            // 后台加载器
			mResSchemeManager->init();
            // 启用后台资源线程
            mThreadManager->startup(0, 4, mResSchemeManager);
            // 初始化材质管理器
            mMaterialManager->init();
            // 初始网格管理器
            mMeshManager->init();
            // 合成器
            mFusionManager->init();
            // 初始粒子系统管理器
            mParticleManager->init();
            // 初始插件 - 在窗体创建后，因此资源系统资源不可用
            PluginList::iterator i, end = mPluginList.end();
            for (i = mPluginList.begin(); i != end; ++i)
            {
                (*i)->init();
            }
            mPrimaryWinInit = true;

            out[0]->setFirstWindow(true);
        }

        return success;
    }
    //-----------------------------------------------------------------------
    void Engine::destroy(ViewWindow * win)
    {
        if(mRenderSys)
        {
            mRenderSys->destroyFrame(win);
        }
    }
    //---------------------------------------------------------------------
    void Engine::install(Plugin * plugin)
    {
        N_Only(Log).log(_I18n("Installing plugin: ") + plugin->getName());
        mPluginList.push_back(plugin);
        plugin->install();
        if (mInit)
        {
            plugin->init();
            plugin->log();
        }
        N_Only(Log).log(_I18n("Plugin successfully installed"));
    }
    //---------------------------------------------------------------------
    void Engine::uninstall(Plugin * plugin)
    {
        N_Only(Log).log(_I18n("Uninstalling plugin: ") + plugin->getName());
        PluginList::iterator i = std::find(mPluginList.begin(), mPluginList.end(), plugin);
        if (i != mPluginList.end())
        {
            if (mInit)
                plugin->shutdown();
            plugin->uninstall();
            mPluginList.erase(i);
        }
        N_Only(Log).log(_I18n("Plugin successfully uninstalled"));
    }
    //-----------------------------------------------------------------------
    void Engine::add(Pattern * prc)
    {
        N_Only(Log).log(_I18n("Try touching Pattern: ") + prc->getPID());

        ProcessList::iterator i, end = mProcesses.end();
        Nui32 pid = prc->getPID();
        for (i = mProcesses.begin(); i != end; ++i)
        {
            if((*i)->getPID() == pid)
                return;
        }

        mProcesses.push_back(prc);
        try
        {
            prc->_init();
        }
        catch(...)
        {
            remove(prc);
            N_Only(Log).log(_I18n("处理器: ") + N_conv(prc->getPID()) + _I18n("加入错误 !"));
            return;
        }

        N_Only(Log).log(_I18n("处理器: ") + N_conv(prc->getPID()) + _I18n("加入成功 !"));
    }
    //-----------------------------------------------------------------------
    void Engine::remove(Pattern * prc)
    {
        ProcessList::iterator i = std::find(mProcesses.begin(), mProcesses.end(), prc);
        if(i != mProcesses.end())
        {
            N_Only(Log).log(_I18n("移去处理器: ") + N_conv(prc->getPID()));
            mProcesses.erase(i);
            N_Only(Log).log(_I18n("移去处理器: ")+ N_conv(prc->getPID()) + _I18n("移去成功 !"));
        }
    }
    //------------------------------------------------------------------------
    void Engine::addScriptModule(ScriptModule * obj)
    {
        ScriptModuleList::const_iterator i, iend = mScriptModuleList.end();
        for(i = mScriptModuleList.begin(); i != iend; ++i)
        {
            if((*i)->getID() == obj->getID())
                return;
        }

        if(obj)
        {
            mScriptModuleList.push_back(obj);
            obj->load();
        }
    }
    //------------------------------------------------------------------------
    ScriptModule * Engine::getScriptModule(ScriptModuleID smid) const
    {
        ScriptModuleList::const_iterator i, iend = mScriptModuleList.end();
        for(i = mScriptModuleList.begin(); i != iend; ++i)
        {
            if((*i)->getID() == smid)
                return *i;
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void Engine::executeScript(ScriptModuleID smid, const String & filename, GroupID gid) const
    {
        ScriptModuleList::const_iterator i, iend = mScriptModuleList.end();
        for(i = mScriptModuleList.begin(); i != iend; ++i)
        {
            if((*i)->getID() == smid)
            {
                try
                {
                    (*i)->executeFile(filename, gid);
                }
                // Forward script exceptions with line number and file info
                catch(ScriptException & e)
                {
                    throw(e);
                }
                catch(...)
                {
                    N_EXCEPT(Runtime, _I18n("An exception was thrown during the execution of the script file."));
                }
            }
            else
            {
                N_Only(Log).log(_I18n("The script named '") + filename +
                    _I18n("' could not be executed as no ScriptModule is available."));
            }
        }
    }
    //------------------------------------------------------------------------
    NIIi Engine::executeScriptFunc(ScriptModuleID smid, const String & func) const
    {
        ScriptModuleList::const_iterator i, iend = mScriptModuleList.end();
        for(i = mScriptModuleList.begin(); i != iend; ++i)
        {
            if((*i)->getID() == smid)
            {
                try
                {
                    return (*i)->executeFunc(func);
                }
                // Forward script exceptions with line number and file info
                catch(ScriptException & e)
                {
                    throw(e);
                }
                catch(...)
                {
                    N_EXCEPT(Runtime, _I18n("An exception was thrown during execution of the scripted function."));
                }

            }
            else
            {
                N_Only(Log).log(_I18n("The global script function named '") + func +
                    _I18n("' could not be executed as no ScriptModule is available."));
            }
        }
        return 0;
    }
    //------------------------------------------------------------------------
    void Engine::executeScriptStr(ScriptModuleID smid, const String & str) const
    {
        ScriptModuleList::const_iterator i, iend = mScriptModuleList.end();
        for(i = mScriptModuleList.begin(); i != iend; ++i)
        {
            if((*i)->getID() == smid)
            {
                try
                {
                    (*i)->executeScript(str);
                }
                // Forward script exceptions with line number and file info
                catch(ScriptException & e)
                {
                    throw(e);
                }
                catch(...)
                {
                    N_EXCEPT(Runtime, _I18n("An exception was thrown during execution of the script code."));
                }
            }
            else
            {
                N_Only(Log).log(_I18n("The script code could not be executed as no ScriptModule is available."));
            }
        }
    }
    //-----------------------------------------------------------------------
    void Engine::loadPlugin(const String & plugin)
    {
        DynamicModule * lib = N_Only(DynamicModule).load(plugin);
        // 为以后卸载存储
        // 检查是否存在，因为调用2+次的，DynamicModuleManager会返回存在项
        if(std::find(mModuleList.begin(), mModuleList.end(), lib) == mModuleList.end())
        {
            mModuleList.push_back(lib);

            DLL_START_PLUGIN pFunc = (DLL_START_PLUGIN)lib->getSymbol("NiiDynModuleBegin");

            if(!pFunc)
                N_EXCEPT(NotExist, _I18n("Cannot find symbol NiiDynModuleBegin in library ") + plugin);

            pFunc();
        }
    }
    //-----------------------------------------------------------------------
    void Engine::unloadPlugin(const String & plugin)
    {
        ModuleList::iterator i, end = mModuleList.end();
        for(i = mModuleList.begin(); i != end; ++i)
        {
            if((*i)->getName() == plugin)
            {
                DLL_STOP_PLUGIN pFunc = (DLL_STOP_PLUGIN)(*i)->getSymbol("NiiDynModuleEnd");
                pFunc();
                N_Only(DynamicModule).unload(*i);
                mModuleList.erase(i);
                break;
            }
        }
    }
    //-----------------------------------------------------------------------
    void Engine::resetFrameTime()
    {
        for(NIIi i = 0; i < FETT_COUNT; ++i)
            mFrameTimeList[i].clear();
    }
    //-----------------------------------------------------------------------
    InstanceMesh * Engine::createInstanceMesh(SpaceManager * sm, InstanceMeshID imid,
        ResourceID rid, GroupID gid, Nui16 subMeshIdx, NCount numInstancesPerBatch,
            InstanceMeshType technique, Nmark flags)
    {
        if(mInstanceSpaceTypeList.find(imid) != mInstanceSpaceTypeList.end())
        {
            N_EXCEPT(UniqueRepeat, _I18n("InstancedManager with name '") + 
                N_conv(imid) + _I18n("' already exists!"));
        }

        InstanceMesh * re = N_new InstanceMesh(imid, sm, rid, gid,
            technique, flags, numInstancesPerBatch, subMeshIdx);

        mInstanceSpaceTypeList[imid] = re;
        return re;
    }
    //---------------------------------------------------------------------
    InstanceMesh * Engine::getInstanceMesh(InstanceMeshID imid) const
    {
        InstanceSpaceTypeList::const_iterator itor = mInstanceSpaceTypeList.find(imid);

        if(itor == mInstanceSpaceTypeList.end())
        {
            N_EXCEPT(NotExist, _T("InstancedManager with name '") + N_conv(imid) + _T("' not found"));
        }

        return itor->second;
    }
    //---------------------------------------------------------------------
    bool Engine::isInstanceMeshExist(InstanceMeshID imid) const
    {
        InstanceSpaceTypeList::const_iterator itor = mInstanceSpaceTypeList.find(imid);
        return itor != mInstanceSpaceTypeList.end();
    }
    //---------------------------------------------------------------------
    void Engine::destroyInstanceMesh(InstanceMeshID imid)
    {
        //The manager we're trying to destroy might have been scheduled for updating
        //while we haven't yet rendered a frame. Update now to avoid a dangling ptr
        updateInstanceMeshImpl();

        InstanceSpaceTypeList::iterator i = mInstanceSpaceTypeList.find(imid);
        if(i != mInstanceSpaceTypeList.end())
        {
            N_delete i->second;
            mInstanceSpaceTypeList.erase(i);
        }
    }
    //---------------------------------------------------------------------
    void Engine::destroyAllInstanceMesh()
    {
        InstanceSpaceTypeList::iterator itor = mInstanceSpaceTypeList.begin();
        InstanceSpaceTypeList::iterator end  = mInstanceSpaceTypeList.end();

        while(itor != end)
        {
            N_delete itor->second;
            ++itor;
        }

        mInstanceSpaceTypeList.clear();
        mUpdateInstanceMeshs.clear();
    }
    //---------------------------------------------------------------------
    InstanceGeo * Engine::createInstanceGeo(SpaceManager * sm, ResourceID material, InstanceMeshID imid)
    {
        InstanceSpaceTypeList::const_iterator itor = mInstanceSpaceTypeList.find(imid);

        if(itor == mInstanceSpaceTypeList.end())
        {
            N_EXCEPT(NotExist, _I18n("InstancedManager with name '") + N_conv(imid) + _I18n("' not found"));
        }

        return itor->second->createInstanceGeo(sm, material);
    }
    //---------------------------------------------------------------------
    void Engine::destroyInstanceGeo(InstanceGeo * geo)
    {
        geo->_getOwner()->removeInstanceGeo(geo);
    }
    //---------------------------------------------------------------------
    void Engine::updateInstanceMesh(InstanceMesh * mesh)
    {
        mUpdateInstanceMeshs.push_back(mesh);
    }
    //---------------------------------------------------------------------
    void Engine::updateInstanceMeshImpl()
    {
        mDirtyInstanceMeshs.insert(mDirtyInstanceMeshs.end(),
            mUpdateInstanceMeshs.begin(), mUpdateInstanceMeshs.end());

        mUpdateInstanceMeshs.clear();

        while(!mDirtyInstanceMeshs.empty())
        {
            InstanceSpaceList::const_iterator itor = mDirtyInstanceMeshs.begin();
            InstanceSpaceList::const_iterator end  = mDirtyInstanceMeshs.end();

            while(itor != end)
            {
                (*itor)->_updateDirtyBatches();
                ++itor;
            }

            //Clear temp buffer
            mDirtyInstanceMeshs.clear();

            //Do it again?
            mDirtyInstanceMeshs.insert(mDirtyInstanceMeshs.end(),
                mUpdateInstanceMeshs.begin(), mUpdateInstanceMeshs.end());
            mUpdateInstanceMeshs.clear();
        }
    }
    //---------------------------------------------------------------------
    void Engine::addFactory(SpaceObjFactory * fact, bool override)
    {
        SpaceObjFactoryList::iterator facti = mSpaceObjFactoryList.find(fact->getID());
        if (override || facti == mSpaceObjFactoryList.end())
        {
            if (fact->getTypeMark() != 0xFFFFFFFF)
            {
                if (facti != mSpaceObjFactoryList.end() && facti->second->getTypeMark() != 0xFFFFFFFF)
                {
                    fact->setTypeMark(facti->second->getTypeMark());
                }
                else
                {
                    if (mSpaceObjMark == SpaceManager::CustomSpaceMark)
                    {
                        N_EXCEPT(UniqueRepeat, _I18n("Cannot allocate a type flag since all the available flags have been used."));
                    }
                    Nui32 ret = mSpaceObjMark;
                    mSpaceObjMark <<= 1;
                    fact->setTypeMark(ret);
                }
            }

            mSpaceObjFactoryList[fact->getID()] = fact;

            N_Only(Log).log(_I18n("SpaceObjFactory for type '") + N_conv(fact->getID()) + _I18n("' registered."));
        }
    }
    //---------------------------------------------------------------------
    bool Engine::isFactoryExist(FactoryID fid) const
    {
        return (mSpaceObjFactoryList.find(fid) != mSpaceObjFactoryList.end());
    }
    //---------------------------------------------------------------------
    SpaceObjFactory * Engine::getFactory(FactoryID fid)
    {
        SpaceObjFactoryList::iterator i = mSpaceObjFactoryList.find(fid);
        if(i == mSpaceObjFactoryList.end())
        {
            return i->second;
        }
        return 0;
    }
    //---------------------------------------------------------------------
    void Engine::removeFactory(FactoryID fid)
    {
        SpaceObjFactoryList::iterator i = mSpaceObjFactoryList.find(fid);
        if (i != mSpaceObjFactoryList.end())
        {
            mSpaceObjFactoryList.erase(i);
        }
    }
    //---------------------------------------------------------------------
    DataType Engine::getColourFormat()
    {
        if(mRenderSys)
        {
            return mRenderSys->getColourType();
        }
        else
        {
#if N_PLAT == N_PLAT_WIN32
            return DT_Colour_ARGB; // windows平台使用 D3D 格式
#else
            return DT_Colour_ABGR; // 其他地方使用 GL 格式
#endif
        }
    }
    //-----------------------------------------------------------------------
    void Engine::addUpdate(PosNode * n)
    {
        if(!n->mRootUpdate)
        {
            n->mRootUpdate = true;
            mRootNodeUpdate.push_back(n);
        }
    }
    //-----------------------------------------------------------------------
    void Engine::removeUpdate(PosNode * n)
    {
        // Erase from queued updates
        RootNodeList::iterator it = std::find(mRootNodeUpdate.begin(), mRootNodeUpdate.end(), n);
        N_assert(it != mRootNodeUpdate.end(), "erro logic");
        if(it != mRootNodeUpdate.end())
        {
            // Optimised algorithm to erase an element from unordered vector.
            *it = mRootNodeUpdate.back();
            mRootNodeUpdate.pop_back();
        }
    }
    //-----------------------------------------------------------------------
    void Engine::updateNode()
    {
        RootNodeList::iterator i, end = mRootNodeUpdate.end();
        for(i = mRootNodeUpdate.begin(); i != end; ++i)
        {
            // Update, and force parent update since chances are we've ended
            // up with some mixed state in there due to re-entrancy
            PosNode * n = *i;
            n->mRootUpdate = false;
            n->notify(true);
        }
        mRootNodeUpdate.clear();
    }
    //---------------------------------------------------------------------
    void Engine::setThread(ThreadManager * obj)
    {
        if(mThreadManager != obj)
        {
            N_delete mThreadManager;
            mThreadManager = obj;
            if (mInit)
            {
                mThreadManager->startup(0, 4, mResSchemeManager);
            }
        }
    }
    //---------------------------------------------------------------------
}

// Matrix3 注意(x,y,z) 坐标系统被假定为右手坐标系.
// 坐标轴旋转矩阵的形式
//   RX =    1       0       0
//           0     cos(t) -sin(t)
//           0     sin(t)  cos(t)
// 当 t > 0 表明在yz平面逆时针旋转
//   RY =  cos(t)    0     sin(t)
//           0       1       0
//        -sin(t)    0     cos(t)
// 当  t > 0 表明在zx平面逆时针旋转
//   RZ =  cos(t) -sin(t)    0
//         sin(t)  cos(t)    0
//           0       0       1
// 当 t > 0 表明在xy平面逆时针旋转