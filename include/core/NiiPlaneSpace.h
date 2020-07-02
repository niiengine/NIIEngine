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

#ifndef _NII_PLANESPACE_H_
#define _NII_PLANESPACE_H_

#include "NiiPreInclude.h"
#include "NiiPlane.h"
#include "NiiAABox.h"
#include "NiiSphere.h"

namespace NII
{
    /** 平面限制空间
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PlaneSpace
    {
    public:
        PlaneSpace();
        PlaneSpace(Plane::PositionType outside);

        /** 添加限制面
        @version NIIEngine 3.0.0
        */
        inline void add(const Plane & o){ mPlanes.push_back(o); }
        
        /** 移去限制面
        @version NIIEngine 3.0.0
        */
        void remove(const Plane & o);
        
        /** 移去所有限制面
        @version NIIEngine 3.0.0
        */
        inline void clear(){ mPlanes.clear(); }
        
        /** 是否相交
        @version NIIEngine 3.0.0
        */
        bool intersects(const AABox & box) const;
        
        /** 是否相交
        @version NIIEngine 3.0.0
        */
        bool intersects(const Sphere & sphere) const;
    public:
        PlaneList mPlanes;
        Plane::PositionType mOuterType;
    };
    typedef vector<PlaneSpace>::type PlaneSpaceList;
}
#endif