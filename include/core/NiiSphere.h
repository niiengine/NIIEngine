/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-2-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_SPHERE_H_
#define _NII_SPHERE_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"

namespace NII
{
    /** 早期多数用来做边界检测的 球
    @remark x^2 + y^2 + z^2 = r^2
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Sphere
    {
    public:
        Sphere();
        Sphere(const vector3 & center, NIIf radius);

        /** 设置球的半径.
        @version NIIEngine 3.0.0
        */
        void setRadius(NIIf radius);

        /** 返回球的半径.
        @version NIIEngine 3.0.0
        */
        NIIf getRadius() const;

        /** 设置球的中心点.
        @version NIIEngine 3.0.0
        */
        void setCenter(const Vector3f & center);

        /** 返回球的中心点.
        @version NIIEngine 3.0.0
        */
        const Vector3f & getCenter() const;

        /** 与另一个球体合并
        @version NIIEngine 3.0.0
        */
        void merge(const Sphere & o);

        /** 是否相交
        @version NIIEngine 3.0.0
        */
        bool intersects(const Vector3f & o) const;

        /** 是否相交
        @version NIIEngine 3.0.0
        */
        bool intersects(const Sphere & o) const;

        /** 是否相交
        @version NIIEngine 3.0.0
        */
        bool intersects(const Plane & o) const;

        /** 是否相交
        @version NIIEngine 3.0.0
        */
        bool intersects(const AABox & o) const;
    public:
        Vector3f mCenter;
        NIIf mRadius;
    };
}
#endif