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
#include "NiiResourceSchemeManager.h"
#include "NiiResourceManager.h"
#include "NiiScriptSys.h"
#include "NiiSpaceManager.h"
#include "NiiLogManager.h"
#include "NiiDataStream.h"
#include "NiiVFSManager.h"
#include "NiiGroupLoadScheme.h"
#include "NiiGroupResultScheme.h"
#include "NiiDefaultStream.h"
#include "NiiException.h"
#include "NiiEngine.h"
#include "NiiThreadManager.h"
#include "NiiResource.h"
#include "NiiJob.h"

namespace NII
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	// ResourceRequest
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	enum RequestType
	{
		RT_INITIALISE_GROUP = 0,		///<
		RT_INITIALISE_ALL_GROUPS = 1,	///<
		RT_PREPARE_GROUP = 2,			///<
		RT_PREPARE_RESOURCE = 3,		///<
		RT_LOAD_GROUP = 4,				///<
		RT_LOAD_RESOURCE = 5,			///<
		RT_UNLOAD_GROUP = 6,			///<
		RT_UNLOAD_RESOURCE = 7			///<
	};

	/// 后台队列请求的结果
	struct BgProcessResult
	{
		bool error;     ///< 是否错误发生
		String message; ///< 任何从处理的信息
		BgProcessResult() : error(false) {}
	};

	struct ResourceRequest
	{
		RequestType mRequestType;
		ResourceID mID;
		PrcID mPID;
		ResourceType mType;
		GroupID mGID;
		ResLoadScheme * mLoad;
		ResResultScheme * mResult;
		PropertyData * mParams;
		BgProcessResult mOutResult;

		friend Nostream & operator<<(Nostream &, const ResourceRequest &)
		{
		}
	};
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	// ResourceResponse
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	class _EngineInner ResourceResponse : public ResourceAlloc
	{
	public:
		ResourceResponse(Resource * r, ResourceRequest * req) :
			mResource(r),
			mRequest(req) {}

		Resource * mResource;
		ResourceRequest * mRequest;

		friend Nostream & operator<<(Nostream &, const ResourceResponse &)
		{
		}
	};
	/** 资源任务
	@version NIIEngine 3.0.0
	*/
	class _EngineInner ResourceJob : public Job
	{
		friend class ResourceSchemeManager;
	public:
		ResourceJob(ResourceRequest * data, Nui8 retry = 0, RequestID id = 0) :
			Job(retry, id),
			mData(data){}

		virtual ~ResourceJob(){}

		/** 获取请求的类型
		@version NIIEngine 3.0.0
		*/
		inline ResourceRequest * getData() const { return mData; }
	protected:
		ResourceRequest * mData;
	};

	class _EngineInner ResourceJobResult : public JobResult
	{
		friend class ResourceSchemeManager;
	public:
		ResourceJobResult(Job * src, ResourceResponse * data, bool complete, const String & msg = StrUtil::WBLANK) :
			JobResult(src, complete, msg),
			mData(data) {}

		~ResourceJobResult()
		{
			if (mData)
			{
				N_delete mData;
				mData = 0;
			}
		}

		/** 获取请求的类型
		@version NIIEngine 3.0.0
		*/
		inline ResourceResponse * getData() const { return mData; }
	protected:
		ResourceResponse * mData;
	};
    //-----------------------------------------------------------------------
    PrcID ResourceSchemeManager::mValidPID = 0;
    GroupID ResourceSchemeManager::mValidGID = 10;
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ResMetadata
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    struct ResMetadata
    {
        ResourceID mID;
        ResourceType mType;
        String mFile;
        ResLoadScheme * mLoadScheme;
        ResResultScheme * mResultScheme;
        PropertyData mParams;
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // LoadGroup
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    struct LoadGroup
    {
        ~LoadGroup();
        void setup();
        void clear();
        void add(Resource * res)
        void add(const String & filename, VFS * vfs);
        void remove(const String & filename, VFS * vfs);
        void remove(VFS * vfs);
        bool isExist(const String & file) const;
        time_t getCreateTime(const String & file) const;
        time_t getModifyTime(const String & file) const;
        time_t getAccessTime(const String & file) const;

        typedef list<std::pair<bool, VFS *> >::type VFSList;
        typedef list<ResMetadata>::type MetadataList;
        typedef list<Resource *>::type Resources;
        typedef map<String, VFS *>::type FileList;
        typedef map<NIIf, Resources>::type LoadLevel;

        N_mutex1
        GroupID mID;
        LoadingState mState;
        FileList mCSFileList;
        FileList mFileList;
        VFSList mVFSList;
        MetadataList mDefineList;
        LoadLevel mLoadList;
        String mName;
        String mRelPath;
        GroupLoadScheme * mLScheme;
        GroupResultScheme * mRScheme;
        bool mGlobal;
    };
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    LoadGroup::~LoadGroup()
    {
        N_lock_mutex(N_mutex1_name)

        LoadGroup::LoadLevel::iterator j, jend = mLoadList.end();
        for(j = mLoadList.begin(); j != jend; ++j)
        {
            LoadGroup::Resources::iterator k, kend = j->second.end();
            for (k = j->second.begin(); k != kend; ++k)
            {
                (*k)->getCreator()->destroy((*k)->getPrc());
            }
            j->second.clear();
        }
        mLoadList.clear();
        mVFSList.clear();
    }
    //-----------------------------------------------------------------------
    void LoadGroup::add(const String & file, VFS * vfs)
    {
        mCSFileList[file] = vfs;
        if(!vfs->isMatchCase())
        {
            String lcase = file;
            StrUtil::toLower(lcase);
            mFileList[lcase] = vfs;
        }
    }
    //---------------------------------------------------------------------
    void LoadGroup::remove(const String & file, VFS * vfs)
    {
        LoadGroup::FileList::iterator i = mCSFileList.find(file);
        if(i != mCSFileList.end() && i->second == vfs)
        {
            mCSFileList.erase(i);
        }
        if(!vfs->isMatchCase())
        {
            String lcase = file;
            StrUtil::toLower(lcase);
            i = mFileList.find(file);
            if(i != mFileList.end() && i->second == vfs)
            {
                mFileList.erase(i);
            }
        }
    }
    //---------------------------------------------------------------------
    void LoadGroup::remove(VFS * vfs)
    {
        LoadGroup::FileList::iterator i, iend = mFileList.end();
        for(i = mFileList.begin(); i != iend;)
        {
            if(i->second == vfs)
            {
                LoadGroup::FileList::iterator del = i++;
                mFileList.erase(del);
            }
            else
            {
                ++i;
            }
        }
        iend = mCSFileList.end();
        for(i = mCSFileList.begin(); i != iend;)
        {
            if(i->second == vfs)
            {
                LoadGroup::FileList::iterator del = i++;
                mCSFileList.erase(del);
            }
            else
            {
                ++i;
            }
        }
    }
    //-----------------------------------------------------------------------
    bool LoadGroup::isExist(const String & file) const
    {
        N_lock_mutex(N_mutex1_name)

        LoadGroup::FileList::iterator i = mCSFileList.find(file);
        if(i == mCSFileList.end())
        {
            String lfile = file;
            StrUtil::toLower(lfile);
            i = mFileList.find(lfile);
            if(i == mFileList.end())
            {
                LoadGroup::VFSList::iterator li, liend = mVFSList.end();
                for(li = mVFSList.begin(); li != liend; ++li)
                {
                    if(li->second->isExist(file))
                    {
                        return true;
                    }
                }
            }
            else
            {
                return true;
            }
        }
        else
        {
            return true;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    time_t LoadGroup::getCreateTime(const String & file) const
    {
        N_lock_mutex(N_mutex1_name) // 锁定组互斥

        // 先尝试索引
        LoadGroup::FileList::iterator rit = mCSFileList.find(file);
        if (rit != mCSFileList.end())
        {
            return rit->second->getCreateTime(file);
        }
        else
        {
            // 尝试不区分大小写
            String lfile = file;
            StrUtil::toLower(lfile);
            rit = mFileList.find(lfile);
            if(rit != mFileList.end())
            {
                return rit->second->getCreateTime(file);
            }
            else
            {
                LoadGroup::VFSList::iterator li, liend = mVFSList.end();
                for (li = mVFSList.begin(); li != liend; ++li)
                {
                    time_t testTime = li->second->getCreateTime(file);
                    if(testTime > 0)
                    {
                        return testTime;
                    }
                }
            }
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    time_t LoadGroup::getModifyTime(const String & file) const
    {
        N_lock_mutex(N_mutex1_name) // 锁定组互斥

        // 先尝试索引
        LoadGroup::FileList::iterator rit = mCSFileList.find(file);
        if (rit != mCSFileList.end())
        {
            return rit->second->getModifiedTime(file);
        }
        else
        {
            // 尝试不区分大小写
            String lfile = file;
            StrUtil::toLower(lfile);
            rit = mFileList.find(lfile);
            if(rit != mFileList.end())
            {
                return rit->second->getModifiedTime(file);
            }
            else
            {
                LoadGroup::VFSList::iterator li, liend = mVFSList.end();
                for (li = mVFSList.begin(); li != liend; ++li)
                {
                    time_t testTime = li->second->getModifiedTime(file);
                    if(testTime > 0)
                    {
                        return testTime;
                    }
                }
            }
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    time_t LoadGroup::getAccessTime(const String & file) const
    {
        N_lock_mutex(N_mutex1_name) // 锁定组互斥

        // 先尝试索引
        LoadGroup::FileList::iterator rit = mCSFileList.find(file);
        if (rit != mCSFileList.end())
        {
            return rit->second->getAccessTime(file);
        }
        else
        {
            // 尝试不区分大小写
            String lfile = file;
            StrUtil::toLower(lfile);
            rit = mFileList.find(lfile);
            if(rit != mFileList.end())
            {
                return rit->second->getAccessTime(file);
            }
            else
            {
                LoadGroup::VFSList::iterator li, liend = mVFSList.end();
                for (li = mVFSList.begin(); li != liend; ++li)
                {
                    time_t testTime = li->second->getAccessTime(file);

                    if(testTime > 0)
                    {
                        return testTime;
                    }
                }
            }
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    void LoadGroup::setup()
    {
        LoadGroup::Resources * loadlist;
        LoadGroup::MetadataList::iterator i, iend = mDefineList.end();
        for(i = mDefineList.begin(); i != iend; ++i)
        {
            const ResMetadata & ResMetadata = *i;

            ResourceManager * mag = get(ResMetadata.mType);
            Resource * res = mag->create(ResMetadata.mID, mID, ResMetadata.mLoadScheme, ResMetadata.mResultScheme, &ResMetadata.mParams);

            LoadGroup::LoadLevel::iterator j = mLoadList.find(mag->getScriptLevel());
            if(j == mLoadList.end())
            {
                loadlist = &mLoadList.insert(Npair(mag->getScriptLevel(), LoadGroup::Resources())).first->second;
            }
            else
            {
                loadlist = &j->second;
            }
            loadlist->push_back(res);
        }
    }
    //-----------------------------------------------------------------------
    void LoadGroup::clear()
    {
        N_lock_mutex(N_mutex1_name)

        LoadGroup::LoadLevel::iterator j, jend = mLoadList.end();
        for(j = mLoadList.begin(); j != jend; ++j)
        {
            LoadGroup::Resources::iterator k, kend = j->second.end();
            for(k = j->second.begin(); k != kend; ++k)
            {
                (*k)->getCreator()->remove((*k)->getPrc());
            }
            j->second.clear();
        }
        mLoadList.clear();
    }
    //-----------------------------------------------------------------------
    void LoadGroup::add(Resource * res)
    {
        N_lock_mutex(N_mutex1_name)
        NIIf order = res->getCreator()->getScriptLevel();
        
        LoadGroup::Resources * loadList;
        LoadGroup::LoadLevel::iterator i = (mLoadList).find(order);
        if(i == (mLoadList).end())
        {
            loadList = &mLoadList.insert(Npair(order, LoadGroup::Resources())).first->second;
        }
        else
        {
            loadList = &i->second;
        }
        loadList->push_back(res);
    }
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    // ResourceSchemeManager
    //-----------------------------------------------------------------------
    //-----------------------------------------------------------------------
    template<> ResourceSchemeManager * Singleton<ResourceSchemeManager>::mOnly = 0;
    //-----------------------------------------------------------------------
    ResourceSchemeManager::ResourceSchemeManager() :
        mTemp(0),
        mTimeLimitMS(10)
    {
        create(GroupDefault, N_StrBlank);
        create(GroupInner, N_StrBlank);
        create(GroupUnknow, N_StrBlank);
        mEngineGroup = GroupDefault;
    }
    //-----------------------------------------------------------------------
    ResourceSchemeManager::~ResourceSchemeManager()
    {
        GroupList::iterator i, iend = mGroups.end();
        for(i = mGroups.begin(); i != iend; ++i)
        {
            N_delete_t(i->second, LoadGroup);
        }
        mGroups.clear();
        NII_LOCK_RW_MUTEX_WRITE(mRWMutex);
        shutdown();
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::request(GroupID gid, NCount count)
    {
    }
    //-----------------------------------------------------------------------
    GroupID ResourceSchemeManager::create(GroupID gid, const String & relpath, const String & name, bool global)
    {
        N_Only(Log).log(_I18n("创建资源组ID: ") + gid);
        if (gid == 0)
        {
            gid = genValidGID();
        }
        if(get(gid))
        {
            N_EXCEPT(UniqueRepeat, _I18n("资源组ID: '") + N_conv(gid) + _I18n("' 已存在!"));
        }

        LoadGroup * group = N_new_t(LoadGroup)();
        group->mState = LS_UNSETUP;
        group->mName = name;
        group->mRelPath = relpath;
        group->mID = gid;
        group->mGlobal = global;
        mGroups.insert(Npair(gid, group));
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::destroy(GroupID gid)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            N_Only(Log).log(_I18n("删除资源组ID: ") + gid);

            mTemp = group;

            unload(gid, false);
            N_delete_t(group, LoadGroup);
            mGroups.erase(mGroups.find(gid));

            mTemp = 0;
        }
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::clear(GroupID gid)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            N_Only(Log).log(_I18n("清理资源组ID: ") + gid);
            mTemp = group;

            group->clear();
            group->mState = LS_UNSETUP;

            mTemp = 0;
            N_Only(Log).log(_I18n("完成清理资源组ID: ") + gid);
        }
    }
    //-----------------------------------------------------------------------
    bool ResourceSchemeManager::isExist(GroupID gid)
    {
        return get(gid) != NULL;
    }
    //-----------------------------------------------------------------------
    bool ResourceSchemeManager::isGlobal(GroupID gid)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            return group->mGlobal;
        }
        return false;
    }
    //-----------------------------------------------------------------------
    const String & ResourceSchemeManager::getName(GroupID gid)
    {
        LoadGroup * group = get(gid);
        if (group)
        {
            return group->mName;
        }
        return N_StrBlank;
    }
    //-----------------------------------------------------------------------
    const String & ResourceSchemeManager::getRelPath(GroupID gid)
    {
        LoadGroup * group = get(gid);
        if (group)
        {
            return group->mRelPath;
        }
        return N_StrBlank;
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::add(ResourceType type, ResourceManager * rm)
    {
        N_Only(Log).log(_I18n("注册资源管理器类型: ") + type);
        mResMagList[type] = rm;
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::remove(ResourceType type)
    {
        N_Only(Log).log(_I18n("注销资源管理器类型: ") + type);
        ResMagList::iterator i = mResMagList.find(type);
        if(i != mResMagList.end())
        {
            mResMagList.erase(i);
        }
    }
    //-----------------------------------------------------------------------
    ResourceManager * ResourceSchemeManager::get(ResourceType type)
    {
        ResMagList::iterator i = mResMagList.find(type);
        if(i == mResMagList.end())
        {
            N_EXCEPT(NotExist, _I18n("不能找到资源管理器类型: '") + N_conv(type) + _T("'"));
        }
        return i->second;
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::add(PropertyScriptSys * spsys)
    {
        mScriptSysList.insert(ScriptSysList::value_type(spsys->getScriptLevel(), spsys));
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::remove(PropertyScriptSys * spsys)
    {
        NIIf order = spsys->getScriptLevel();
        ScriptSysList::iterator oi = mScriptSysList.find(order);
        while (oi != mScriptSysList.end() && oi->first == order)
        {
            if (oi->second == spsys)
            {
                // erase does not invalidate on multimap, except current
                ScriptSysList::iterator del = oi++;
                mScriptSysList.erase(del);
            }
            else
            {
                ++oi;
            }
        }
    }
    //-----------------------------------------------------------------------
    BgPrcID ResourceSchemeManager::setup(GroupID gid, SchemeID ls, SchemeID rs)
    {
#if NII_THREAD_SUPPORT
        ResourceRequest * req = N_new ResourceRequest();
        req.type = RT_INITIALISE_GROUP;
        req.mGID = gid;

        return addRequest(req);
#else
        N_Only(Log).log(_I18n("初始化资源组ID: ") + gid);
        LoadGroup * grp = get(gid);
        if(grp)
        {
            N_lock_mutex(grp->N_mutex1_name)

            if(grp->mState == LS_UNSETUP)
            {
                grp->mState = LS_SETUPING;
                parse(grp);

                mTemp = grp;
                N_Only(Log).log(_I18n("设立资源组ID: ") + gid);

                grp->setup();
                grp->mState = LS_SETUPED;

                N_Only(Log).log(_I18n("成功设立资源组ID: ") + gid);

                mTemp = 0;
            }
        }
        return 0;
#endif
    }
    //-----------------------------------------------------------------------
    BgPrcID ResourceSchemeManager::setupAll(SchemeID ls, SchemeID rs)
    {
#if NII_THREAD_SUPPORT
        ResourceRequest * req = N_new ResourceRequest();
        req.type = RT_INITIALISE_ALL_GROUPS;

        return addRequest(req);
#else
        GroupList::iterator i, iend = mGroups.end();
        for(i = mGroups.begin(); i != iend; ++i)
        {
            LoadGroup * grp = i->second;
            N_lock_mutex(grp->N_mutex1_name)
            if(grp->mState == LS_UNSETUP)
            {
                // 初始化处理
                grp->mState = LS_SETUPING;
                // 设置当前组
                mTemp = grp;
                // 解析属于grp组的脚本
                parse(grp);
                N_Only(Log).log(_I18n("设立资源组ID: ") + i->first);
                // 创建属于grp组的列表
                grp->setup();
                // 设置组状态为 LS_SETUPED
                grp->mState = LS_SETUPED;
                N_Only(Log).log(_I18n("成功设立资源组ID: ") + i->first);
                // 重置当前使用的组
                mTemp = 0;
            }
        }
        return 0;
#endif
    }
    //-----------------------------------------------------------------------
    bool ResourceSchemeManager::isSetup(GroupID gid)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            return (group->mState != LS_UNSETUP && group->mState != LS_SETUPING);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    BgPrcID ResourceSchemeManager::plan(GroupID gid, bool mainRes)
    {
#if NII_THREAD_SUPPORT
        ResourceRequest * req = N_new ResourceRequest();
        req.type = RT_PREPARE_GROUP;
        req.mGID = gid;

        return addRequest(req);
#else
        LoadGroup * group = get(gid);
        if(group)
        {
            N_Only(Log).stream() << _I18n("准备资源组ID: '") << gid
                << _I18n("' - 准备主要资源: ") << mainRes;

            N_lock_mutex(group->N_mutex1_name) // 锁定组的互斥

            mTemp = group;

            // 为开始事件计算出资源
            LoadGroup::LoadLevel::iterator oi, oiend = group->mLoadList.end();
            NCount resc = 0;
            if(mainRes)
            {
                for(oi = group->mLoadList.begin(); oi != oiend; ++oi)
                {
                    resc += oi->second.size();
                }
            }

            group->mLScheme->onPrePlan(gid, resc);

            // 现在真正加载
            if(mainRes)
            {
                oiend = group->mLoadList.end();
                for(oi = group->mLoadList.begin(); oi != oiend; ++oi)
                {
                    NCount n = 0;
                    //遍历所有加载命令列表里的资源
                    LoadGroup::Resources::iterator l = oi->second.begin();
                    while (l != oi->second.end())
                    {
                        Resource * res = *l;
                        group->mLScheme->onPrePlan(res);
                        res->plan();
                        group->mLScheme->onPlanEnd(res);

                        ++n;

                        //资源改变了组,如果是这样,迭代器将视为无效
                        if(res->getGroup() != gid)
                        {
                            l = oi->second.begin();
                            std::advance(l, n);
                        }
                        else
                        {
                            ++l;
                        }
                    }
                }
            }
            group->mLScheme->onPlanEnd(gid);

            mTemp = 0;

            N_Only(Log).log(_I18n("完成准备资源组ID: ") + gid);
        }
        return 0;
#endif
    }
    //-----------------------------------------------------------------------
    BgPrcID ResourceSchemeManager::load(GroupID gid, bool mainRes)
    {
#if NII_THREAD_SUPPORT
        ResourceRequest * req = N_new ResourceRequest();
        req.type = RT_LOAD_GROUP;
        req.mGID = gid;

        return addRequest(req);
#else
        LoadGroup * group = get(gid);
        if(group)
        {
            N_Only(Log).stream() << _I18n("加载资源组ID: '") << gid
                << _I18n("' - 加载主要资源: ") << mainRes;

            N_lock_mutex(group->N_mutex1_name) // 锁定组的互斥

            mTemp = group;

            NCount resc = 0;
            LoadGroup::LoadLevel::iterator oi, oiend = group->mLoadList.end();
            if(mainRes)
            {
                for(oi = group->mLoadList.begin(); oi != oiend; ++oi)
                {
                    resc += oi->second.size();
                }
            }

            group->mLScheme->onPreLoad(gid, resc);

            // 现在真正加载
            if(mainRes)
            {
                oiend = group->mLoadList.end();
                for(oi = group->mLoadList.begin(); oi != oiend; ++oi)
                {
                    NCount n = 0;
                    //遍历所有加载命令列表里的资源
                    LoadGroup::Resources::iterator l = oi->second.begin();
                    while (l != oi->second.end())
                    {
                        Resource * res = *l;
                        group->mLScheme->onPreLoad(res);
                        res->load();
                        group->mLScheme->onLoadEnd(res);

                        ++n;

                        //资源改变了组,如果是这样,迭代器将视为无效
                        if(res->getGroup() != gid)
                        {
                            l = oi->second.begin();
                            std::advance(l, n);
                        }
                        else
                        {
                            ++l;
                        }
                    }
                }
            }
            group->mLScheme->onLoadEnd(gid);
            group->mState = LS_LOADED;

            mTemp = 0;

            N_Only(Log).log(_I18n("完成加载资源组ID: ") + gid);
        }
        return 0;
#endif
    }
    //-----------------------------------------------------------------------
    BgPrcID ResourceSchemeManager::unload(GroupID gid, bool resume)
    {
#if NII_THREAD_SUPPORT
        ResourceRequest * req = N_new ResourceRequest();
        req.type = RT_UNLOAD_GROUP;
        req.mGID = gid;

        return addRequest(req);
#else
        LoadGroup * group = get(gid);
        if(group)
        {
            N_Only(Log).log(_I18n("卸载资源组ID: ") + gid);
            N_lock_mutex(group->N_mutex1_name) // 锁定组互斥
            mTemp = group;

            // 为开始事件计算出资源
            LoadGroup::LoadLevel::reverse_iterator oi, oiend = group->mLoadList.rend();
            for(oi = group->mLoadList.rbegin(); oi != oiend; ++oi)
            {
                LoadGroup::Resources::iterator l, lend = oi->second.end();
                for(l = oi->second.begin(); l != lend; ++l)
                {
                    Resource * res = *l;
                    if(!resume || res->isReloadable())
                    {
                        res->unload();
                    }
                }
            }
            group->mState = LS_SETUPED;

            mTemp = 0;
            N_Only(Log).log(_I18n("完成卸载资源组ID: ") + gid);
        }
        return 0;
#endif
    }
    //-----------------------------------------------------------------------
    BgPrcID ResourceSchemeManager::unload(ResourceType type, ResourceID rid)
    {
#if NII_THREAD_SUPPORT
        ResourceRequest * req = N_new ResourceRequest();
        req.type = RT_UNLOAD_RESOURCE;
        req.mType = type;
        req.mID = rid;

        return addRequest(req);
#else
		N_Only(ResourceScheme).get(type)->unload(rid);
        return 0;
#endif
    }
    //------------------------------------------------------------------------
    BgPrcID ResourceSchemeManager::unload(ResourceType type, PrcID pid)
    {
#if NII_THREAD_SUPPORT
        ResourceRequest * req = N_new ResourceRequest();
        req.type = RT_UNLOAD_RESOURCE;
        req.mType = type;
        req.mPID = pid;

        return addRequest(req);
#else
		N_Only(ResourceScheme).get(type)->unload(pid);
        return 0;
#endif
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::unloadUnused(GroupID gid, bool resume)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            N_Only(Log).log(_I18n("卸载未使用资源资源ID: ") + gid);
            mTemp = group;

            LoadGroup::LoadLevel::reverse_iterator oi, oiend = group->mLoadList.rend();
            for(oi = (group->mLoadList).rbegin(); oi != oiend; ++oi)
            {
                for(LoadGroup::Resources::iterator l = oi->second.begin();
                    l != oi->second.end(); ++l)
                {
                    if((*l)->getRefCount() == 0)
                    {
                        Resource * resource = *l;
                        if(!resume || resource->isReloadable())
                        {
                            resource->unload();
                        }
                    }
                }
            }
            group->mState = LS_SETUPED;

            mTemp = 0;
            N_Only(Log).log(_I18n("完成未用资源卸载资源组ID: ") + gid);
        }
    }
    //-----------------------------------------------------------------------
    bool ResourceSchemeManager::isLoad(GroupID gid)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            return group->mState == LS_LOADED;
        }

        return false;
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::create(ResourceID rid, ResourceType type, const String & file,
        GroupID gid, ResLoadScheme * ls, ResResultScheme * rs, const PropertyData & params)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            N_lock_mutex(group->N_mutex1_name)

            ResMetadata rdef;
            rdef.mID = rid;
            rdef.mType = type;
            rdef.mFile = file;
            rdef.mLoadScheme = ls;
            rdef.mResultScheme = rs;
            rdef.mParams = params;
            group->mDefineList.push_back(rdef);
        }
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::destroy(ResourceID rid, GroupID gid)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            N_lock_mutex(group->N_mutex1_name)

            LoadGroup::MetadataList::iterator i, iend = group->mDefineList.end();
            for(i = group->mDefineList.begin(); i != iend; ++i)
            {
                if(i->mID == rid)
                {
                    group->mDefineList.erase(i);
                    break;
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::add(const String & protocol, VFSType type, GroupID gid, bool r)
    {
        LoadGroup * group = get(gid);
        if(!group)
        {
            create(gid, N_StrBlank);
            group = get(gid);
        }

        N_lock_mutex(group->N_mutex1_name)

        VFS * vfs = N_Only(VFS).load(protocol, type);
        group->mVFSList.push_back(std::pair<bool, VFS *>(r, vfs));

        StringList namelist;
        vfs->find(namelist, _T("*"), r);
        StringList::iterator f, fend = namelist.end();
        for(f = namelist.begin(); f != fend; ++f)
        {
            group->add(*f, vfs);
        }
        StringStream msg;
        msg << _I18n("添加资源文件协议: '") << protocol << _I18n("' 类型: '") << type
            << _I18n("' 资源组: '") << gid << "'";
        if(r)
            msg << _I18n(" 启用遍历选项");
        N_Only(Log).log(msg.str());
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::remove(const String & protocol, GroupID gid)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            N_lock_mutex(group->N_mutex1_name)

            LoadGroup::VFSList::iterator i, iend = group->mVFSList.end();
            for(i = group->mVFSList.begin(); i != iend; ++i)
            {
                VFS * vfs = i->second;
                if(vfs->getName() == protocol)
                {
                    group->remove(vfs);
                    group->mVFSList.erase(i);

                    break;
                }
            }
            N_Only(Log).log(_I18n("移去资源文件协议: ") + protocol);
        }
    }
    //-----------------------------------------------------------------------
    bool ResourceSchemeManager::isVFSExist(const String & protocol, GroupID gid) const
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            N_lock_mutex(group->N_mutex1_name)

            LoadGroup::VFSList::iterator i, iend = group->mVFSList.end();
            for(i = group->mVFSList.begin(); i != iend; ++i)
            {
                VFS * vfs = i->second;
                if((vfs->getName() == protocol) || (vfs->getName().find(protocol) != String::npos))
                {
                    return true;
                }
            }
        }
        return false;
    }
    //-----------------------------------------------------------------------
    VFS * ResourceSchemeManager::getVFS(const String & protocol, GroupID gid) const
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            N_lock_mutex(group->N_mutex1_name)

            LoadGroup::VFSList::iterator i, iend = group->mVFSList.end();
            for(i = group->mVFSList.begin(); i != iend; ++i)
            {
                VFS * vfs = i->second;
                if((vfs->getName() == protocol) || (vfs->getName().find(protocol) != String::npos))
                {
                    return vfs;
                }
            }
        }
        return NULL;
    }
    //-----------------------------------------------------------------------
    VFS * ResourceSchemeManager::getVFS(ResourceID rid, GroupID gid = GroupDefault) const
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            N_lock_mutex(group->N_mutex1_name)
            LoadGroup::MetadataList::iterator i, iend = group->mDefineList.end();
            if(i->mID == rid)
            {
                const String & file = i->mFile;
                LoadGroup::FileList::iterator rit = group->mCSFileList.find(file);
                if(rit != group->mCSFileList.end())
                {
                    return rit->second;
                }
                else
                {
                    String lfile = file;
                    StrUtil::toLower(lfile);
                    rit = group->mFileList.find(lfile);
                    if(rit != group->mFileList.end())
                    {
                        return rit->second;
                    }
                    else
                    {
                        LoadGroup::VFSList::iterator li, liend = group->mVFSList.end();
                        for(li = group->mVFSList.begin(); li != liend; ++li)
                        {
                            VFS * vfs = li->second;
                            if(vfs->isExist(file))
                            {
                                return vfs;
                            }
                        }
                    }
                }
            }
        }
        return NULL;
    }
    //-----------------------------------------------------------------------
    DataStream * ResourceSchemeManager::open(ResourceID rid, GroupID gid)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            N_lock_mutex(group->N_mutex1_name)
            LoadGroup::MetadataList::iterator i, iend = group->mDefineList.end();
            if(i->mID == rid)
            {
                const String & file = i->mFile;
                LoadGroup::FileList::iterator rit = group->mCSFileList.find(file);
                if(rit != group->mCSFileList.end())
                {
                    return rit->second->open(file);
                }
                else
                {
                    String lfile = file;
                    StrUtil::toLower(lfile);
                    rit = group->mFileList.find(lfile);
                    if(rit != group->mFileList.end())
                    {
                        return rit->second->open(file);
                    }
                    else
                    {
                        LoadGroup::VFSList::iterator li, liend = group->mVFSList.end();
                        for(li = group->mVFSList.begin(); li != liend; ++li)
                        {
                            VFS * vfs = li->second;
                            if(vfs->isExist(file))
                            {
                                return vfs->open(file);
                            }
                        }
                    }
                }
            }
        }
        return NULL;
    }
    //-----------------------------------------------------------------------
    DataStream * ResourceSchemeManager::open(const String & file, GroupID gid, bool allgroup, Resource * obj)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            N_lock_mutex(group->N_mutex1_name)

            LoadGroup::FileList::iterator j = group->mCSFileList.find(file);
            if(j != group->mCSFileList.end())
            {
                return j->second->open(file);
            }
            else
            {
                String lfile = file;
                StrUtil::toLower(lfile);
                j = group->mFileList.find(lfile);
                if(j != group->mFileList.end())
                {
                    return j->second->open(file);
                }
                else
                {
                    LoadGroup::VFSList::iterator li, liend = group->mVFSList.end();
                    for(li = group->mVFSList.begin(); li != liend; ++li)
                    {
                        VFS * vfs = li->second;
                        if(vfs->isExist(file))
                        {
                            DataStream * stream = vfs->open(file);
                            return stream;
                        }
                    }
                }
            }
        }
        else if(allgroup)
        {
            group = findImpl(file);
            if(group)
            {
                if(obj)
                {
                    obj->setGroup(group->mID);
                }
                return open(file, group->mID, false);
            }
        }
        return NULL;
    }
    //-----------------------------------------------------------------------
    DataStreamList ResourceSchemeManager::opens(const String & pattern, GroupID gid)
    {
        DataStreamList re;

        LoadGroup * group = get(gid);
        if(group)
        {
            N_lock_mutex(group->N_mutex1_name)

            LoadGroup::VFSList::iterator i, iend = group->mVFSList.end();
            for(i = group->mVFSList.begin(); i != iend; ++i)
            {
                VFS * vfs = i->second;

                StringList names;
                vfs->find(names, pattern, i->first);
                StringList::iterator ni, niend = names.end();
                for(ni = names.begin(); ni != niend; ++ni)
                {
                    DataStream * ptr = vfs->open(*ni);
                    if(ptr != 0)
                    {
                        re.push_back(ptr);
                    }
                }
            }
        }
        return re;
    }
    //-----------------------------------------------------------------------
    DataStream * ResourceSchemeManager::createSrc(const String & filename,
        GroupID gid, bool overwrite, const String & protocol)
    {
        LoadGroup * group = get(gid);
        if(!group)
        {
            N_EXCEPT(NotExist, _I18n("不能找到资源组ID: ") + gid);
        }

        N_lock_mutex(group->N_mutex1_name) // 锁定组互斥

        LoadGroup::VFSList::iterator i, iend = group->mVFSList.end();
        for(i = group->mVFSList.begin(); i != iend; ++i)
        {
            VFS * vfs = i->second;

            if(!vfs->isReadOnly() && (protocol.empty() || StrUtil::match(vfs->getName(), protocol)))
            {
                if(!overwrite && vfs->isExist(filename))
                    N_EXCEPT(UniqueRepeat, _I18n("Cannot overwrite existing file ") + filename);

                DataStream * ret = vfs->create(filename);
                group->add(filename, vfs);
                return ret;
            }
        }
        N_EXCEPT(NotExist, _I18n("Cannot find a writable location in group ") + gid);
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::deleteSrc(const String & filename, GroupID gid, const String & protocol)
    {
        LoadGroup * grp = get(gid);
        if(grp)
        {
            N_lock_mutex(grp->N_mutex1_name)

            LoadGroup::VFSList::iterator li, liend = grp->mVFSList.end();
            for(li = grp->mVFSList.begin(); li != liend; ++li)   // 遍历组的所有位置
            {
                VFS * vfs = li->second;

                if(!vfs->isReadOnly() && (protocol.empty() || StrUtil::match(vfs->getName(), protocol)))
                {
                    if(vfs->isExist(filename))
                    {
                        vfs->remove(filename);
                        grp->remove(filename, vfs);
                        break;
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::deleteMatch(const String & filePattern, GroupID gid, const String & protocol)
    {
        LoadGroup * grp = get(gid);
        if(grp)
        {
            N_lock_mutex(grp->N_mutex1_name)

            LoadGroup::VFSList::iterator li, liend = grp->mVFSList.end();
            for(li = grp->mVFSList.begin(); li != liend; ++li)   // 遍历组的所有位置
            {
                VFS * vfs = li->second;

                if(!vfs->isReadOnly() && (protocol.empty() || StrUtil::match(vfs->getName(), protocol)))
                {
                    StringList matchfile;
                    vfs->find(matchfile, filePattern);
                    StringList::iterator f, fend = matchfile.end();
                    for(f = matchfile.begin(); f != fend; ++f)
                    {
                        vfs->remove(*f);
                        grp->remove(*f, vfs);

                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::getParser(const String & pattern, PropertyScriptSys *& out)
    {
        ScriptSysList::iterator oi, oiend = mScriptSysList.end();
        for(oi = mScriptSysList.begin(); oi != oiend; ++oi)
        {
            PropertyScriptSys * su = oi->second;
            const StringList & patterns = su->getScriptPattern();

            StringList::const_iterator p, pend = patterns.end();
            for(p = patterns.begin(); p != pend; ++p)
            {
                if(*p == pattern)
                {
                    out = su;
                    return;
                }
            }
        }
        out = 0;
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::parse(LoadGroup * grp)
    {
        N_Only(Log).log(_I18n("解析资源组脚本: ") + grp->mID);

        typedef NII::list<FileInfoList>::type FileListList;
        typedef SharedPtrST<FileListList> FileListListPtr;
        typedef std::pair<PropertyScriptSys *, FileListListPtr> LoaderFileListPair;
        typedef NII::list<LoaderFileListPair>::type ScriptLoaderFileList;
        ScriptLoaderFileList scriptLoaderFileList;
        NCount scriptCount = 0;
        // 遍历在加载命令的脚本和获得流
        ScriptSysList::iterator oi, oiend = mScriptSysList.end();
        for(oi = mScriptSysList.begin(); oi != oiend; ++oi)
        {
            PropertyScriptSys * su = oi->second;
            FileListListPtr fileListList(N_new_t(FileListList)(), FileListListPtr::MU_Delete_T);

            // 获取所有文件名模式和搜索它们
            const StringList & patterns = su->getScriptPattern();
            StringList::const_iterator p, pend = patterns.end();
            for(p = patterns.begin(); p != pend; ++p)
            {
                FileInfoList fileList;
                find(grp->mID, fileList, *p);
                scriptCount += fileList.size();
                fileListList->push_back(fileList);
            }
            // 存储起来,再处理
            scriptLoaderFileList.push_back(LoaderFileListPair(su, fileListList));
        }

        if(grp->mLScheme)
            grp->mLScheme->onPreScriptParse(grp->mID, scriptCount);

        // 遍历脚本并解析,注意服从原来的命令
        ScriptLoaderFileList::iterator slfli, slfliend = scriptLoaderFileList.end();
        for(slfli = scriptLoaderFileList.begin(); slfli != slfliend; ++slfli)
        {
            PropertyScriptSys * su = slfli->first;
            PtrList obj;
            // 遍历每个列表
            FileListList::iterator flli, flliend = slfli->second->end();
            for(flli = slfli->second->begin(); flli != flliend; ++flli)
            {
                // 遍历每个在列表的项目
                FileInfoList::iterator fii, fiiend = flli->end();
                for(fii = flli->begin(); fii != fiiend; ++fii)
                {
                    bool skipScript = grp->mLScheme->onPreScriptParse(fii->mFullName);

                    if(skipScript)
                    {
                        N_Only(Log).log(_I18n("Skipping script ") + fii->mFullName);
                    }
                    else
                    {
                        N_Only(Log).log(_I18n("Parsing script ") + fii->mFullName);
                        DataStream * stream = fii->mSrc->open(fii->mFullName);
                        if(stream != 0)
                        {
                            if(fii->mSrc->getType() == VFST_Local && stream->size() <= 1024 * 1024)
                            {
                                DataStream * cachedCopy = N_new MemStream(stream, stream->getName());
                                su->parse(cachedCopy, grp->mID, obj);
                                N_delete cachedCopy;
                            }
                            else
                                su->parse(stream, grp->mID, obj);

                            N_delete stream;
                        }
                    }
                    grp->mLScheme->onScriptParseEnd(fii->mFullName);
                }
            }
        }
        if(grp->mLScheme)
            grp->mLScheme->onScriptParseEnd(grp->mID);
        N_Only(Log).log(_I18n("完成解析资源组脚本: ") + grp->mID);
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::onCreate(Resource * res)
    {
        if(mTemp && res->getGroup() == mTemp->mID)
        {
            mTemp->add(res);
        }
        else
        {
            LoadGroup * grp = get(res->getGroup());
            if(grp)
            {
                grp->add(res);
            }
        }
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::onRemove(Resource * res)
    {
        if(mTemp)
        {
            // 什么都不做 - 这样成批卸载,列表被清除
        }
        else
        {
            LoadGroup * grp = get(res->getGroup());
            if(grp)
            {
                N_lock_mutex(grp->N_mutex1_name)
                LoadGroup::LoadLevel::iterator i = grp->mLoadList.find(res->getCreator()->getScriptLevel());

                if(i != grp->mLoadList.end())
                {
                    LoadGroup::Resources * resList = &i->second;
                    LoadGroup::Resources::iterator l, lend = resList->end();
                    for(l = resList->begin(); l != lend; ++l)
                    {
                        if(*l == res)
                        {
                            resList->erase(l);
                            break;
                        }
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::onGroupChange(GroupID gid, Resource * res)
    {
        Resource * resPtr = NULL;
        LoadGroup * grp = get(gid);

        if(grp)
        {
            N_lock_mutex(grp->N_mutex1_name)

            NIIf order = res->getCreator()->getScriptLevel();
            LoadGroup::LoadLevel::iterator i = grp->mLoadList.find(order);
            N_assert(i != grp->mLoadList.end(), "error");

            LoadGroup::Resources * loadList = &i->second;
            LoadGroup::Resources::iterator l, lend = loadList->end();

            for(l = loadList->begin(); l != lend; ++l)
            {
                if(*l == res)
                {
                    resPtr = *l;
                    loadList->erase(l);
                    break;
                }
            }
        }

        if(resPtr != NULL)
        {
            LoadGroup * newGrp = get(res->getGroup());
            newGrp->add(resPtr);
        }
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::onAllRemove(ResourceManager * manager)
    {
        GroupList::iterator grpi, grpiend = mGroups.end();
        for (grpi = mGroups.begin(); grpi != grpiend; ++grpi)
        {
            N_lock_mutex(grpi->second->N_mutex1_name)
            // 遍历所有加载命令
            LoadGroup::LoadLevel::iterator oi, oiend = (grpi->second->mLoadList).end();
            for(oi = (grpi->second->mLoadList).begin(); oi != oiend; ++oi)
            {
                LoadGroup::Resources::iterator l, lend = oi->second.end();
                for (l = oi->second.begin(); l != lend;)
                {
                    if ((*l)->getCreator() == manager)
                    {
                        LoadGroup::Resources::iterator del = l++;
                        oi->second.erase(del);
                    }
                    else
                    {
                        ++l;
                    }
                }
            }

        }
    }
    //-----------------------------------------------------------------------
    LoadGroup * ResourceSchemeManager::get(GroupID gid) const
    {
        GroupList::const_iterator i = mGroups.find(gid);
        if(i != mGroups.end())
        {
            return i->second;
        }
        return 0;
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::list(GroupID gid, StringList & out, bool dirs)
    {
        LoadGroup * group = get(gid);
        if(!group)
        {
            N_EXCEPT(NotExist, _I18n("不能找到资源组ID: ") + gid);
        }

        N_lock_mutex(group->N_mutex1_name)

        LoadGroup::VFSList::iterator i, iend = group->mVFSList.end();
        for(i = group->mVFSList.begin(); i != iend; ++i)
        {
            i->second->list(out, i->first, dirs);
        }
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::list(GroupID gid, FileInfoList & out, bool dirs)
    {
        LoadGroup * group = get(gid);
        if(!group)
        {
            N_EXCEPT(NotExist, _I18n("不能找到资源组ID: ") + gid);
        }

        N_lock_mutex(group->N_mutex1_name)

        LoadGroup::VFSList::iterator i, iend = group->mVFSList.end();
        for(i = group->mVFSList.begin(); i != iend; ++i)
        {
            i->second->list(out, i->first, dirs);
        }
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::find(GroupID gid, StringList & out, const String & pattern, bool dirs)
    {
        LoadGroup * group = get(gid);
        if(!group)
        {
            N_EXCEPT(NotExist, _I18n("不能找到资源组ID: ") + gid);
        }

        N_lock_mutex(group->N_mutex1_name)

        LoadGroup::VFSList::iterator i, iend = group->mVFSList.end();
        for(i = group->mVFSList.begin(); i != iend; ++i)
        {
            i->second->find(out, pattern, i->first, dirs);
        }
    }
    //-----------------------------------------------------------------------
    void ResourceSchemeManager::find(GroupID gid, FileInfoList & out, const String & pattern, bool dirs)
    {
        LoadGroup * group = get(gid);
        if(!group)
        {
            N_EXCEPT(NotExist, _I18n("不能找到资源组ID: ") + gid);
        }

        N_lock_mutex(group->N_mutex1_name)

        LoadGroup::VFSList::iterator i, iend = group->mVFSList.end();
        for(i = group->mVFSList.begin(); i != iend; ++i)
        {
            i->second->find(out, pattern, i->first, dirs);
        }
    }
    //-----------------------------------------------------------------------
    bool ResourceSchemeManager::isExist(GroupID gid, const String & file)
    {
        LoadGroup * group = get(gid);
        if(group)
        {
            return group->isExist(file);
        }
        return false;
    }
    //-----------------------------------------------------------------------
    time_t ResourceSchemeManager::getCreateTime(GroupID gid, const String & file) const
    {
        LoadGroup * grp = get(gid);
        if(grp)
        {
            return grp->getCreateTime(file);
        }
        return time_t(0);
    }
    //-----------------------------------------------------------------------
    time_t ResourceSchemeManager::getModifyTime(GroupID gid, const String & file) const
    {
        LoadGroup * grp = get(gid);
        if(grp)
        {
            return grp->getModifyTime(file);
        }
        return time_t(0);
    }
    //-----------------------------------------------------------------------
    time_t ResourceSchemeManager::getAccessTime(GroupID gid, const String & file) const
    {
        LoadGroup * grp = get(gid);
        if(grp)
        {
            return grp->getAccessTime(file);
        }
        return time_t(0);
    }
    //-----------------------------------------------------------------------
    bool ResourceSchemeManager::isExist(const String & file)
    {
        if(findImpl(file))
            return true;
        return false;
    }
    //-----------------------------------------------------------------------
    LoadGroup * ResourceSchemeManager::findImpl(const String & file)
    {
        GroupList::iterator i, iend = mGroups.end();
        for(i = mGroups.begin(); i != iend; ++i)
        {
            LoadGroup * grp = i->second;

            N_lock_mutex(grp->N_mutex1_name) // 锁定组互斥

            if(isExist(grp, file))
                return grp;
        }
        return NULL;
    }
    //-----------------------------------------------------------------------
    GroupID ResourceSchemeManager::find(const String & file)
    {
        LoadGroup * grp = findImpl(file);
        if(!grp)
        {
            N_EXCEPT(NotExist, _I18n("Unable to derive resource group for ") +
                file + _I18n(" automatically since the resource was not found."));
        }
        return grp->mID;
    }
    //------------------------------------------------------------------------
    bool ResourceSchemeManager::isComplete(BgPrcID pid)
    {
        return mBgPrcList.find(pid) == mBgPrcList.end();
    }
    //------------------------------------------------------------------------
    void ResourceSchemeManager::abort(BgPrcID ticket)
    {
        N_Only(Thread).abort(ticket);
    }
    //-----------------------------------------------------------------------
    GroupIDList ResourceSchemeManager::getGroups()
    {
        GroupIDList re;
        GroupList::iterator i, iend = mGroups.end();
        for(i = mGroups.begin(); i != iend; ++i)
        {
            re.push_back(i->second->mID);
        }
        return re;
    }
    //------------------------------------------------------------------------
    void ResourceSchemeManager::run(void * arg) 
    {
        
    }
    //------------------------------------------------------------------------
    JobResult * ResourceSchemeManager::handle(Job * jrq)
    {
        NII_LOCK_RW_MUTEX_READ(mRWMutex);
        ResourceRequest * resreq = static_cast<ResourceJob *>(jrq)->getData();

        if(jrq->isStop())
        {    // RT_PREPARE_RESOURCE 和 RT_LOAD_RESOURCE 是 new出 PropertyData 的
            if(resreq->mRequestType == RT_PREPARE_RESOURCE || resreq->mRequestType == RT_LOAD_RESOURCE)
            {
                N_delete resreq->mParams;
                resreq->mParams = 0;
            }
            resreq->mOutResult.error = false;
            return N_new ResourceJobResult(jrq, N_new ResourceResponse(0, resreq), true);
        }

        ResourceManager * rm = 0;
        Resource * mResource = 0;
        try
        {
            switch(resreq->mRequestType)
            {
            case RT_INITIALISE_GROUP:
                N_Only(ResourceScheme).setup(resreq->mGID, 0, 0);
                break;
            case RT_INITIALISE_ALL_GROUPS:
                N_Only(ResourceScheme).setupAll(0, 0);
                break;
            case RT_PREPARE_GROUP:
                N_Only(ResourceScheme).plan(resreq->mGID);
                break;
            case RT_LOAD_GROUP:
#if NII_THREAD_SUPPORT == 2
                N_Only(ResourceScheme).plan(resreq->mGID);
#else
                N_Only(ResourceScheme).load(resreq->mGID);
#endif
                break;
            case RT_UNLOAD_GROUP:
                N_Only(ResourceScheme).unload(resreq->mGID);
                break;
            case RT_PREPARE_RESOURCE:
                rm = N_Only(ResourceScheme).get(resreq->mType);
                mResource = rm->plan(resreq->mID, resreq->mGID, resreq->mLoad, 0, resreq->mParams);
                break;
            case RT_LOAD_RESOURCE:
                rm = N_Only(ResourceScheme).get(resreq->mType);
#if NII_THREAD_SUPPORT == 2
                mResource = rm->plan(resreq->mID, resreq->mGID, resreq->mLoad, resreq->mParams, true);
#else
                mResource = rm->load(resreq->mID, resreq->mGID, resreq->mLoad, 0, resreq->mParams);
#endif
                break;
            case RT_UNLOAD_RESOURCE:
                rm = N_Only(ResourceScheme).get(resreq->mType);
                if (resreq->mID == 0)
                    rm->unload(resreq->mPID);
                else
                    rm->unload(resreq->mID);
                break;
            };
        }
        catch(Exception & e)
        {    //RT_PREPARE_RESOURCE 和 RT_LOAD_RESOURCE 是 new出 PropertyData 的
            if(resreq->mRequestType == RT_PREPARE_RESOURCE || resreq->mRequestType == RT_LOAD_RESOURCE)
            {
                N_delete_t(resreq->mParams, PropertyData);
                resreq->mParams = 0;
            }
            resreq->mOutResult.error = true;
            resreq->mOutResult.message = e.getErrorVerbose();

            return N_new ResourceJobResult(jrq, N_new ResourceResponse(mResource, resreq), false, e.getErrorVerbose());
        }

        /*  1 成功了
            2 RT_PREPARE_RESOURCE 和 RT_LOAD_RESOURCE 是new出 PropertyData 的
        */
        if(resreq->mRequestType == RT_PREPARE_RESOURCE || resreq->mRequestType == RT_LOAD_RESOURCE)
        {
            N_delete_t(resreq->mParams, PropertyData);
            resreq->mParams = 0;
        }
        resreq->mOutResult.error = false;
 
        return N_new ResourceJobResult(jrq, N_new ResourceResponse(mResource, resreq), true);
    }
    //------------------------------------------------------------------------
    void ResourceSchemeManager::handle(JobResult * jrs)
    {
        if(jrs->getJob()->isStop())
        {
            mBgPrcList.erase(jrs->getJob()->getID());
            return ;
        }

        if(jrs->isComplete())
        {
            ResourceResponse * rep = static_cast<ResourceJobResult *>(jrs)->getData();
            const ResourceRequest * req = rep->mRequest;
#if NII_THREAD_SUPPORT == 2
            // NII_THREAD_SUPPORT的2选项在此时再加载,这个加载命令的prepare()为后台加载
            if(req->type == RT_LOAD_RESOURCE)
            {
                ResourceManager * rm = N_Only(ResourceScheme).get(req->mType);
                rm->load(req->mID, req->mGID, req->mLoad, req->mParams, true);
            }
            else if(req->type == RT_LOAD_GROUP)
            {
                N_Only(ResourceScheme).load(req->mGID);
            }
#endif
            mBgPrcList.erase(jrs->getJob()->getID());

            if(rep->mResource != 0)
            {
                if(req->mRequestType == RT_LOAD_RESOURCE)
                {
                    rep->mResource->onLoadEnd();
                }
                else
                {
                    rep->mResource->onPlanEnd();
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void ResourceSchemeManager::init()
    {

    }
    //------------------------------------------------------------------------
    void ResourceSchemeManager::shutdown()
    {
        N_Only(Thread).abort(0);
    }
    //-----------------------------------------------------------------------
    BgPrcID ResourceSchemeManager::plan(ResourceType type, ResourceID name, GroupID gid, 
		ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params)
    {
#if NII_THREAD_SUPPORT
        ResourceRequest * req = N_new ResourceRequest();
        req.type = RT_PREPARE_RESOURCE;
        req.mType = type;
        req.mID = name;
        req.mGID = gid;
        req.mLoad = ls;
        req.mParams = (params ? params->clone() : 0);

        return addRequest(req);
#else
        N_Only(ResourceScheme).get(type)->plan(name, gid, ls, rs, params);
        return 0;
#endif
    }
    //------------------------------------------------------------------------
    BgPrcID ResourceSchemeManager::load(ResourceType type, ResourceID rid, GroupID group,
        ResLoadScheme * ls, ResResultScheme * rs, const PropertyData * params)
    {
#if NII_THREAD_SUPPORT
        ResourceRequest * req = N_new ResourceRequest();
        req.type = RT_LOAD_RESOURCE;
        req.mType = type;
        req.mID = name;
        req.mGID = group;
        req.mLoad = ls;
        req.mParams = (params ? params->clone() : 0);

        return addRequest(req);
#else
		N_Only(ResourceScheme).get(type)->load(rid, group, ls, rs, params);
        return 0;
#endif
    }
    //-----------------------------------------------------------------------
    BgPrcID ResourceSchemeManager::addRequest(ResourceRequest * jrq)
    {
        ResourceJob * temp = N_new ResourceJob(jrq);
        RequestID requestID = N_Only(Thread).add(0, temp, this);
        mBgPrcList.insert(requestID);

        return requestID;
    }
    //-----------------------------------------------------------------------
    ResourceIDList ResourceSchemeManager::getResources(GroupID gid)
    {
        LoadGroup * group = get(gid);
        ResourceIDList re;
        if(group)
        {
            N_lock_mutex(group->N_mutex1_name)

            LoadGroup::MetadataList::iterator i, iend = group->mDefineList.end();
            for(i = group->mDefineList.begin(); i != iend; ++i)
            {
                re.push_back(i->mID);
            }
        }
        return re;
    }
    //-----------------------------------------------------------------------
    GroupID ResourceSchemeManager::genValidGID()
    {
        N_lock_mutex(mValidGIDMutex)
        GroupID temp = ++mValidGID;
        return temp;
    }
    //-----------------------------------------------------------------------
    PrcID ResourceSchemeManager::genValidPID()
    {
        N_lock_mutex(mValidPIDMutex)
        PrcID temp = ++mValidPID;
        return temp;
    }
    //---------------------------------------------------------------------
}