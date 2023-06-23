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

#ifndef _NII_PLANE_H_
#define _NII_PLANE_H_

#include "NiiPreInclude.h"
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
        inline Plane(const Vector3f & normal, const Vector3f & p) { set(normal, p); }
        inline Plane(const Vector3f & p1, const Vector3f & p2, const Vector3f & p3) { set(p1, p2, p3);  }

        inline bool operator==(const Plane & o) const { return (o.mNormal == mNormal && o.mD == mD); }
        inline bool operator!=(const Plane & o) const { return (o.mNormal != mNormal || o.mD != mD); }

        /** 设置
        @version NIIEngine 3.0.0
        */
        inline void set(const Vector3f & normal, const Vector3f & p) { mNormal = normal; mD = -normal.dotProduct(p); }

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
        inline bool intersects(const AABox & o) const { return (getSide(o) == Plane::PT_Merge); }

        /** 返回到点的距离
        @version NIIEngine 3.0.0
        */
        inline NIIf distance(const Vector3f & o) const{ return mNormal.dotProduct(o) + mD; }

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
        inline static bool getFrustumNearPlane(const Matrix4f & space, Plane & out)
        {
            return getPlane(space[0][2], space[1][2], space[2][2], space[3][2], out);
        }

        /** 平截头的扩展(投影距阵远平面)
        @version NIIEngine 3.0.0
        */
        inline static bool getFrustumFarPlane(const Matrix4f & space, Plane & out)
        {
            return getPlane(space[0][3] - space[0][2], space[1][3] - space[1][2], 
                space[2][3] - space[2][2], space[3][3] - space[3][2], out);
        }

        /** 平截头的扩展(投影距阵左平面)
        @version NIIEngine 3.0.0
        */
        inline static bool getFrustumLeftPlane(const Matrix4f & space, Plane & out)
        {
            return getPlane(space[0][3] + space[0][0], space[1][3] + space[1][0], 
                space[2][3] + space[2][0], space[3][3] + space[3][0], out);
        }

        /** 平截头的扩展(投影距阵右平面)
        @version NIIEngine 3.0.0
        */
        static bool getFrustumRightPlane(const Matrix4f & space, Plane & out)
        {
            return getPlane(space[0][3] - space[0][0], space[1][3] - space[1][0], 
                space[2][3] - space[2][0], space[3][3] - space[3][0], out);
        }

        /** 平截头的扩展(投影距阵上平面)
        @version NIIEngine 3.0.0
        */
        static bool getFrustumTopPlane(const Matrix4f & space, Plane & out)
        {
            return getPlane(space[0][3] - space[0][1], space[1][3] - space[1][1], 
                space[2][3] - space[2][1], space[3][3] - space[3][1], out);
        }

        /** 平截头的扩展(投影距阵下平面)
        @version NIIEngine 3.0.0
        */
        static bool getFrustumBottomPlane(const Matrix4f & space, Plane & out)
        {
            return getPlane(space[0][3] + space[0][1], space[1][3] + space[1][1], 
                space[2][3] + space[2][1], space[3][3] + space[3][1], out);
        }

        /** 平截头的扩展(处理投影距阵)
        @param[in] a
        @param[in] b
        @param[in] c
        @param[in] d
        @param[in] out 结果输出
        @version NIIEngine 3.0.0
        */
        static bool getPlane(NIIf a, NIIf b, NIIf c, NIIf d, Plane & out);
    public:
        Vector3f mNormal;
        NIIf mD;
    };

    inline _EngineAPI Nostream & operator << (Nostream & o, const Plane & p)
    {
        o << "Plane(" << p.mNormal << "," << p.mD << ")";
        return o;
    }

    typedef vector<Plane>::type PlaneList;
}
#endif