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

#ifndef _NII_SpaceTypeManager_H_
#define _NII_SpaceTypeManager_H_

#include "NiiPreInclude.h"
#include "NiiSpaceManager.h"
#include "NiiSingleton.h"

namespace NII
{
    /** 空间类型辅助信息
    @version NIIEngine 3.0.0
    */
    class SpaceTypeVerbose
    {
    public:
        /** 空间类型
        @version NIIEngine 3.0.0
        */
        enum SpaceType
        {
            ST_Normal       = 0x01,
            ST_Intersect    = 0x02,
            ST_Auto         = 0x04,
            ST_World        = 0x08,
            ST_All          = 0x0f
        };
    public:
        FactoryID mTypeID;
        String mVerbose;
        Nmark mMark;
    };

    /** 场景工厂类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceTypeFactory : public SceneAlloc
    {
        friend class SpaceTypeManager;
    public:
        SpaceTypeFactory();
        virtual ~SpaceTypeFactory();

        /** 工厂ID
        @version NIIEngine 3.0.0
        */
        virtual FactoryID getID() = 0;

        /** 创建
        @version NIIEngine 3.0.0
        */
        virtual SpaceManager * create(const String & name) = 0;

        /** 删除
        @version NIIEngine 3.0.0
        */
        virtual void destroy(SpaceManager * obj) = 0;

        /** 获取描述
        @version NIIEngine 3.0.0
        */
        const SpaceTypeVerbose & getVerbose() const;
    protected:
        virtual const SpaceTypeVerbose & getVerboseImpl() const = 0;
    protected:
        SpaceTypeVerbose mVerbose;
        bool mVerboseValid;
    };
    typedef vector<const SpaceTypeVerbose *>::type SpaceTypeList;

    /** 空间类型管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SpaceTypeManager : public Singleton<SpaceTypeManager>, public SceneAlloc
    {
    public:
        typedef map<String, SpaceManager *>::type SpaceList;
    public:
        SpaceTypeManager();
        ~SpaceTypeManager();

        /** 设置渲染系统
        @version NIIEngine 3.0.0
        */
        void setRender(RenderSys * rs);

        /** 添加空间管理工厂类
        @version NIIEngine 3.0.0
        */
        void add(SpaceTypeFactory * obj);

        /** 移去空间管理工厂类
        @version NIIEngine 3.0.0
        */
        void remove(FactoryID tid);

        /** 获取
        @verseion NIIEngine 3.0.0
        */
        const SpaceTypeVerbose * getVerbose(FactoryID tid) const;

        /** 创建对象
        @version NIIEngine 3.0.0
        */
        SpaceManager * createSpace(FactoryID tid, const String & name = N_StrBlank);

        /** 创建对象
        @version NIIEngine 3.0.0
        */
        SpaceManager * createSpace(Nmark mark, const String & name = N_StrBlank);

        /** 获取对象
        version NIIEngine 3.0.0
        */
        SpaceManager * getSpace(const String & name) const;

        /** 是否存在对象
        @version NIIEngine 3.0.0
        */
        bool isSpaceExist(const String & name) const;

        /** 删除对象
        @version NIIEngine 3.0.0
        */
        void destroy(SpaceManager * sm);

        /** 删除所有对象
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 获取列表
        @version NIIEngine 3.0.0
        */
        SpaceList & getSpaceList();

        /** 获取列表
        @version NIIEngine 3.0.0 高级api
        */
        const SpaceList & getSpaceList() const;
    private:
        typedef vector<SpaceTypeFactory *>::type FactoryList;
    private:
        RenderSys * mGpuSys;
        FactoryList mFactoryList;
        SpaceTypeList mSpaceTypeList;
        SpaceList mSpaceList;
        NCount mCount;
    };
}
#endif