/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

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

#ifndef _NII_PLANE_H_
#define _NII_PLANE_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"
#include "NiiAABox.h"

namespace NII
{
    /** 平面.
    @remark Ax + By + Cz + D = 0
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Plane
    {
    public:
        /** 位置类型
        @version NIIEngine 3.0.0
        */
        enum PositionType
        {
            PT_None,
            PT_Merge,
            PT_Positive,
            PT_Negative
        };
    public:
        Plane();
        Plane(const Plane & o);
        Plane(NIIf a, NIIf b, NIIf c, NIIf d);
        Plane(const Vector3f & normal, NIIf constant);
        Plane(const Vector3f & normal, const Vector3f & p);
        Plane(const Vector3f & p1, const Vector3f & p2, const Vector3f & p3);

        inline bool operator==(const Plane & o) const
        {
            return (o.mNormal == mNormal || o.mD == mD);
        }

        inline bool operator!=(const Plane & o) const
        {
            return (o.mNormal != mNormal || o.mD != mD);
        }

        /** 设置
        @version NIIEngine 3.0.0
        */
        void set(const Vector3f & normal, const Vector3f & p);

        /** 设置
        @version NIIEngine 3.0.0
        */
        void set(const Vector3f & p1, const Vector3f & p2, const Vector3f & p3);

        /** 获取点和平面的位置类型
        @version NIIEngine 3.0.0
        */
        PositionType getSide(const Vector3f & o) const;

        /** 获取轴对称盒和平面的位置类型
        @version NIIEngine 3.0.0
        */
        PositionType getSide(const AABox & o) const;

        /** 建立反射空间
        @version NIIEngine 3.0.0
        */
        void getReflect(Matrix4f * out) const;

        /** 平面变换
        @remark 通过一个这个矩阵变换平面
        */
        void transform(const Matrix4f & m);

        /** 是否相交
        @version NIIEngine 3.0.0
        */
        bool intersects(const AABox & o) const;

        /** 返回到点的距离
        @version NIIEngine 3.0.0
        */
        NIIf distance(const Vector3f & o) const;

        /** 投射点到平面上
        @param[in] v 投射前的位置
        @return 投射后的位置
        @version NIIEngine 3.0.0
        */
        Vector3f project(const Vector3f & v) const;

        /** 单位化平面
        @return 平面法线的之前的长度
        @version NIIEngine 3.0.0
        */
        NIIf normalise();

        /** 平截头的扩展(投影距阵近平面)
        @version NIIEngine 3.0.0
        */
        static bool getFrustumNearPlane(const Matrix4f & space, Plane & out);

        /** 平截头的扩展(投影距阵远平面)
        @version NIIEngine 3.0.0
        */
        static bool getFrustumFarPlane(const Matrix4f & space, Plane & out);

        /** 平截头的扩展(投影距阵左平面)
        @version NIIEngine 3.0.0
        */
        static bool getFrustumLeftPlane(const Matrix4f & space, Plane & out);

        /** 平截头的扩展(投影距阵右平面)
        @version NIIEngine 3.0.0
        */
        static bool getFrustumRightPlane(const Matrix4f & space, Plane & out);

        /** 平截头的扩展(投影距阵上平面)
        @version NIIEngine 3.0.0
        */
        static bool getFrustumTopPlane(const Matrix4f & space, Plane & out);

        /** 平截头的扩展(投影距阵下平面)
        @version NIIEngine 3.0.0
        */
        static bool getFrustumBottomPlane(const Matrix4f & space, Plane & out);

        /** 平截头的扩展(处理投影距阵)
        @param[in] a
        @param[in] b
        @param[in] c
        @param[in] d
        @param[in] dest 结果输出
        @version NIIEngine 3.0.0
        */
        static bool makeFrustumPlane(NIIf a, NIIf b, NIIf c, NIIf d, Plane & dest);
    public:
        Vector3f mNormal;
        NIIf mD;
    };

    inline _EngineAPI std::ostream & operator << (std::ostream & o, const Plane & p)
    {
        o << "Plane(" << p.mNormal << "," << p.mD << ")";
        return o;
    }

    typedef vector<Plane>::type PlaneList;
}
#endif