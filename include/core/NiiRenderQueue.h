/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-7

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

#ifndef _NII_RenderQueue_H_
#define _NII_RenderQueue_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 渲染队列组
    @version NIIEngine 3.0.0
    */
    enum RenderQueueGroup
    {
        RQG_Unknow = 0,
        RQG_Bg = 1,
        RQG_Sky_First = 2,
        RQG_Ext_1 = 3,
        RQG_Ext_2 = 4,
        RQG_Ext_3 = 5,
        RQG_Ext_4 = 6,
        RQG_Ext_5 = 7,
        RQG_Geo_1 = 10,
        RQG_Ext_6 = 11,
        RQG_Ext_7 = 12,
        RQG_Ext_8 = 13,
        RQG_Ext_9 = 14,
        RQG_Ext_10 = 15,
        RQG_User = 20,
        RQG_Ext_11 = 21,
        RQG_Ext_12 = 22,
        RQG_Ext_13 = 23,
        RQG_Ext_14 = 24,
        RQG_Ext_15 = 25,
        RQG_Geo_2 = 30,
        RQG_Ext_16 = 31,
        RQG_Ext_17 = 32,
        RQG_Ext_18 = 33,
        RQG_Ext_19 = 34,
        RQG_Ext_20 = 35,
        RQG_Sky_Last = 40,
        RQG_Surface = 41,
        RQG_Count = 42,
        RQG_Max = 255
    };

    /** 渲染队列
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderQueue : public RenderAlloc
    {
    public:
        typedef vector<std::pair<RenderQueueGroup, RenderGroup *> >::type GroupList;
    public:
        RenderQueue();
        virtual ~RenderQueue();

        /** 设置默认组
        @version NIIEngine 3.0.0
        */
        void setDefaultGroup(GroupID grp);

        /** 获取默认组
        @version NIIEngine 3.0.0
        */
        GroupID getDefaultGroup() const;

        /** 设置默认等级
        @version NIIEngine 3.0.0
        */
        void setDefaultLevel(Nui16 lv);

        /** 获取默认等级
        @version NIIEngine 3.0.0
        */
        Nui16 getDefaultLevel() const;

        /** 清理队列时是否删除所有渲染组
        @versioin NIIEngine 3.0.0
        */
        void setDestroyAllOnClear(bool r);
        
        /** 清理队列时是否删除所有渲染组
        @version NIIEngine 3.0.0
        */
        bool isDestroyAllOnClear() const;
        
        /** 添加一个对象到队列
        @version NIIEngine 3.0.0
        */
        void add(GeometryObj * obj);

        /** 添加一个对象到队列
        @version NIIEngine 3.0.0
        */
        void add(GeometryObj * obj, GroupID gid);

        /** 添加一个对象到队列
        @version NIIEngine 3.0.0
        */
        void add(GeometryObj * obj, GroupID gid, Nui16 level);

        /** 清理队列
        @version NIIEngine 3.0.0
        */
        void clear();

        /** 获取一个渲染队列群组
        @version NIIEngine 3.0.0
        */
        RenderGroup * getQueueGroup(GroupID qid);

        /** 获取渲染列表
        @version NIIEngine 3.0.0 高级api
        */
        const GroupList & getRenderList() const;

        /** 合并队列
        @version NIIEngine 3.0.0
        */
        void merge(const RenderQueue * o);
    protected:
        /** 创建渲染组
        @version NIIEngine 3.0.0
        */
        void createImpl(RenderGroup *& out);

        /** 创建主组
        @version NIIEngine 3.0.0
        */
        void createMainGroup();
    protected:
        GroupList mGroups;
        GroupID mDefaultGroup;
        Nui16 mDefaultLevel;
        bool mRemoveGroupOnClear;
    };
}
#endif