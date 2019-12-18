/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-23

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

#ifndef _NII_LOD_SCHEME_MANAGER_H_
#define _NII_LOD_SCHEME_MANAGER_H_

#include "NiiPreInclude.h"
#include "NiiLodScheme.h"
#include "NiiSingleton.h"
#include "NiiLodUnitList.h"

namespace NII
{
    /** LOD管理器
    @note src 0.0f 代表原始层,而且LodScheme需要一个原始层
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LodSchemeManager : public Singleton<LodSchemeManager>, public LodAlloc
    {
    public:
        typedef map<ClassID, LodScheme *>::type DefaultList;
        typedef map<SchemeID, LodScheme *>::type SchemeList;
        typedef map<ClassID, SchemeList>::type ClassSchemeList;
    public:
        LodSchemeManager();
        ~LodSchemeManager();

        /** 添加策略
        @version NIIEngine 3.0.0
        */
        void add(ClassID cid, LodScheme * obj);

        /** 移去策略
        @version NIIEngine 3.0.0
        */
        void remove(ClassID cid, SchemeID sid);

        /** 获取策略
        @version NIIEngine 3.0.0
        */
        LodScheme * get(ClassID cid, SchemeID sid);

        /** 创建策略
        @note 使用完后调用 N_delete
        @version NIIEngine 3.0.0
        */
        LodScheme * create(ClassID cid, SchemeID sid);

        /** 移去所有策略
        @version NIIEngine 3.0.0
        */
        void removeAll(ClassID cid);

        /** 移去所有策略
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 设置默认策略
        @version NIIEngine 3.0.0
        */
        void setDefault(ClassID cid, SchemeID sid);

        /** 创建默认策略
        @note 使用完后调用 N_delete
        @version NIIEngine 3.0.0
        */
        LodScheme * createDefault(ClassID cid);

        /** 获取列表
        @version NIIEngine 3.0.0
        */
        const SchemeList * getList(ClassID cid);

        /// @copydetails Singleton::getOnly
        static LodSchemeManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static LodSchemeManager * getOnlyPtr();
    private:
        DefaultList mDefaultList;
        ClassSchemeList mSchemeList;
    };
}
#endif