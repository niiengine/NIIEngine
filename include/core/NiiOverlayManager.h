/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

#ifndef _NII_OverlayManager_H_
#define _NII_OverlayManager_H_

#include "NiiPreInclude.h"
#include "NiiScriptParserSys.h"
#include "NiiOverlay.h"
#include "NiiSingleton.h"

namespace NII
{
    /** 表层面管理器
    @version NIIEngine 3.0.0
    */
    class _EngineAPI OverlayManager : public Singleton<OverlayManager>, public ScriptParserSys, public OverlayAlloc
    {
    public:
        typedef map<OverlayID, Overlay *>::type OverlayMap;
        typedef map<OverlayGeoID, OverlayGeo *>::type GeoList;
        typedef map<OverlayGeoType, OverlayGeoFactory *>::type FactoryMap;
        typedef vector<OverlayMap>::type OverlayMapList;
    public:
        OverlayManager();
        ~OverlayManager();

        /** 渲染表层面
        @version NIIEngine 3.0.0
        */
        void render(Viewport * vp, Camera * cam, RenderQueue * queue);

        /** 创建表层面
        @param[in] vpid
        @version NIIEngine 3.0.0
        */
        Overlay * create(Nid vpid, OverlayID id);

        /** 获取表层面
        @param[in] vpid
        @version NIIEngine 3.0.0
        */
        Overlay * get(Nid vpid, OverlayID id);

        /** 删除表层面
        @param[in] vpid
        @version NIIEngine 3.0.0
        */
        void destroy(Nid vpid, OverlayID id);

        /** 删除所有表层面
        @param[in] vpid
        @version NIIEngine 3.0.0
        */
        void destroyAll(Nid vpid);

        /** 删除所有表层面
        @version NIIEngine 3.0.0
        */
        void destroyAll();

        /** 获取表层面列表
        @param[in] vpid 视口ID
        @version NIIEngine 3.0.0 高级api
        */
        const OverlayMap & getList(Nid vpid);

        /** 获取表层面列表
        @version NIIEngine 3.0.0 高级api
        */
        void getList(OverlayMapList & maplist);

        /** 创建层几何
        @version NIIEngine 3.0.0
        */
        OverlayGeo * createGeo(OverlayGeoType tid, OverlayGeoID oid, bool templet = false);

        /** 创建层几何
        @version NIIEngine 3.0.0
        */
        OverlayGeo * createGeo(OverlayGeoType tid, bool templet = false);

        /** 创建层几何
        @version NIIEngine 3.0.0
        */
        OverlayGeo * createGeo(OverlayGeoID templateid, OverlayGeoID oid);

        /** 创建层几何
        @version NIIEngine 3.0.0
        */
        OverlayGeo * createGeo(OverlayGeoID templateid, OverlayGeoType type, OverlayGeoID oid, bool templet = false);

        /** 层几何是否存在
        @version NIIEngine 3.0.0
        */
        bool isGeoExist(OverlayGeoID tid, bool templet = false);

        /** 获取层几何
        @version NIIEngine 3.0.0
        */
        OverlayGeo * getGeo(OverlayGeoID tid, bool templet = false);

        /** 删除层几何
        @version NIIEngine 3.0.0
        */
        void destroyGeo(OverlayGeoID tid, bool templet = false);

        /** 删除所有层几何
        @version NIIEngine 3.0.0
        */
        void destroyAllGeo(bool templet = false);

        /** 是否不是模版层几何
        @version NIIEngine 3.0.0
        */
        bool isTemplate(OverlayGeoID id) const;

        /** 添加层几何工厂类
        @version NIIEngine 3.0.0
        */
        void add(OverlayGeoFactory * obj);

        /** 获取几何工厂类
        @version NIIEngine 3.0.0
        */
        const FactoryMap & getFactoryList() const;

		/** 生成一个有效ID
		@version NIIEngine 3.0.0
		*/
		OverlayID genValidID();

        /// @copydetails ScriptParserSys::parse
        void parse(DataStream * stream, GroupID gid, PtrList & out);

        /// @copydetails ScriptParserSys::getScriptLevel
        NIIf getScriptLevel() const;

        /// @copydetails Singleton::getOnly
        static OverlayManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static OverlayManager * getOnlyPtr();
    protected:
        /** 创建层
        @version NIIEngine 3.0.0
        */
        OverlayGeo * createGeoImpl(OverlayGeoType type, OverlayGeoID id);

        /** 从脚本流中解析新元素
        @version NIIEngine 3.0.0
        */
        void parseGeo(DataStream * ds, OverlayGeoType type, OverlayGeoID id,
            OverlayGeoID templateid, bool isContainer, Overlay * obj,
                OverlayView * view, bool templet);

        /** 分析子对象
        @version NIIEngine 3.0.0
        */
        bool parseGeoProperty(DataStream * ds, const String & line, Overlay * obj,
            OverlayView * view, bool templet);
    protected:
        typedef set<String>::type LoadedScripts;
        typedef map<ViewportID, OverlayMap>::type OverlayList;
    protected:
        FactoryMap mFactories;
        GeoList mInstances;
        GeoList mTemplates;
        LoadedScripts mLoadedScripts;
        OverlayList mOverlayList;
		OverlayID mValidID;
        N_mutex(mValidIDMutex)
    };
}
#endif