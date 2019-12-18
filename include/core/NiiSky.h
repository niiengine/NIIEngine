/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-3-1

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

#ifndef _NII_SKY_H_
#define _NII_SKY_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 天空
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Sky : public SpaceAlloc
    {
    public:
        enum PlaneType
        {
            PT_Front = 0,
            PT_Back = 1,
            PT_Left = 2,
            PT_Right = 3,
            PT_Up = 4,
            PT_Down = 5
        };
    public:
        Sky();
        Sky(const SpaceManager * sm);
        virtual ~Sky();

        /** 把天空渲染到设想机中
        @param[in] obj 摄象机
        @version NIIEngine 3.0.0
        */
        virtual void render(Camera * obj);

        /** 设置当前空间域
        @version NIIEngine 3.0.0
        */
        void setSpace(SpaceManager * sm);
        
        /** 设置渲染组
        @version NIIEngine 3.0.0
        */
        void setRenderGroup(GroupID gid);

        /** 获取渲染组
        @version NIIEngine 3.0.0
        */
        GroupID getRenderGroup()const;

        /** 设置资源组
        @version NIIEngine 3.0.0
        */
        void setResourceGroup(GroupID gid);

        /** 获取资源组
        @version NIIEngine 3.0.0
        */
        GroupID getResourceGroup() const;

        /** 设置是否启用
        @version NIIEngine 3.0.0
        */
        void setEnable(bool b);

        /** 获取是否启用
        @version NIIEngine 3.0.0
        */
        bool isEnable() const;
    protected:
        /** 渲染到队列中
        @version NIIEngine 3.0.0 高级api
        */
        virtual void renderImpl(RenderQueue * rq) = 0;
    protected:
        SpaceManager * mParent;
        GroupID mRenderGroup;
        GroupID mResourceGroup;
        SpaceNode * mNode;
        bool mEnable;
    };
}

#endif