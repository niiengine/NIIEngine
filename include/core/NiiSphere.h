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