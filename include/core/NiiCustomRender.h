/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-2-27

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

#ifndef _NII_CUSTOM_RENDER_H_
#define _NII_CUSTOM_RENDER_H_

#include "NiiPreInclude.h"
#include "NiiCustomQueue.h"

namespace NII
{
    /** 自定义渲染
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CustomRender : public RenderAlloc
    {
    public:
        CustomRender(Nid id);
        virtual ~CustomRender();

        /** 添加自定义队列
        @note 参数内存将由这个类管理
        @version NIIEngine 3.0.0
        */
        void add(CustomQueue * obj);

        /** 添加自定义队列
        @version NIIEngine 3.0.0
        */
        CustomQueue * add(GroupID qid, const String & name);

        /** 获取自定义队列
        @version NIIEngine 3.0.0
        */
        CustomQueue * get(Nidx index);

        /** 移去自定义队列
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);

        /** 移去所有自定义队列
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** 获取数量
        @version NIIEngine 3.0.0
        */
        NCount getCount() const;

        /** 获取ID
        @version NIIEngine 3.0.0
        */
        Nid getID() const;

        /** 获取自定义队列
        @version NIIEngine 3.0.0 高级api
        */
        const CustomQueueList & getList();
    protected:
        Nid mID;
        CustomQueueList mList;
    };
}
#endif