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
        inline Vector3f operator*(NIIf t) { return Vector3f(mOrigin + (mDirection * t)); }

        /** 设置射线的原点
        @version NIIEngine 3.0.0
        */
        inline void setOrigin(const Vector3f & o) { mOrigin = o; }

        /** 获取射线的原点
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getOrigin() const { return mOrigin;  }

        /** 设置射线的方法
        @version NIIEngine 3.0.0
        */
        inline void setDirection(const Vector3f & dir) { mDirection = dir;  }

        /** 获取射线的方法
        @version NIIEngine 3.0.0
        */
        inline const Vector3f & getDirection() const { return mDirection;  }

        /** 获取沿射线的一个点单元的位置
        @version NIIEngine 3.0.0
        */
        inline Vector3f getPoint(NIIf t) const { return Vector3f(mOrigin + (mDirection * t)); }

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