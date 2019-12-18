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
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_RAY_H_
#define _NII_RAY_H_

#include "NiiPreInclude.h"
#include "NiiPlaneSpace.h"
#include "NiiVector3.h"

namespace NII
{
    /** 射线
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Ray
    {
    public:
        Ray();
        Ray(const Vector3f & origin, const Vector3f & direction);

        /// 算术运算符
        Vector3f operator*(NIIf t);

        /** 设置射线的原点
        @version NIIEngine 3.0.0
        */
        void setOrigin(const Vector3f & o);

        /** 获取射线的原点
        @version NIIEngine 3.0.0
        */
        const Vector3f & getOrigin() const;

        /** 设置射线的方法
        @version NIIEngine 3.0.0
        */
        void setDirection(const Vector3f & dir);

        /** 获取射线的方法
        @version NIIEngine 3.0.0
        */
        const Vector3f & getDirection() const;

        /** 获取沿射线的一个点单元的位置
        @version NIIEngine 3.0.0
        */
        Vector3f getPoint(NIIf t) const;

        /** 是否相交
        @param[out] d 相交距离
        @version NIIEngine 3.0.0
        */
        bool intersects(const Plane & p, NIIf & d) const;

        /** 是否相交
        @param[out] d 相交距离
        @version NIIEngine 3.0.0
        */
        bool intersects(const AABox & box, NIIf & d) const;

        /** 是否相交
        @param[in] d1 最近的相交距离
        @param[in] d2 最远的相交距离
        @version NIIEngine 3.0.0
        */
        bool intersects(const AABox & box, NIIf & d1, NIIf & d2) const;

        /** 是否相交
        @param[out] d 相交距离
        @param[in] insideD 射线在球的内部是否也需要求出距离
        @version NIIEngine 3.0.0
        */
        bool intersects(const Sphere & o, NIIf & d, bool insideD = false) const;

        /** 是否相交
        @param[out] d 相交距离
        @param[in] outdef 面外向定义
        @version NIIEngine 3.0.0
        */
        bool intersects(const PlaneList & o, NIIf & d, Plane::PositionType outdef) const;

        /** 是否相交
        @param[out] d 相交距离
        @param[out] positive 是否正面相交
        @version NIIEngine 3.0.0
        */
        bool intersects(const Vector3f & a, const Vector3f & b, const Vector3f & c, 
            const Vector3f & normal, NIIf & d, bool & positive) const;
    public:
        Vector3f mOrigin;
        Vector3f mDirection;
    };
}
#endif