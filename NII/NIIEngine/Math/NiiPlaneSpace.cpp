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

#include "NiiPreProcess.h"
#include "NiiPlaneSpace.h"
#include "NiiMath.h"

namespace NII
{
    //------------------------------------------------------------------------
    PlaneSpace::PlaneSpace() :
        mOuterType(Plane::PT_Negative)
    {
    }
    //------------------------------------------------------------------------
    PlaneSpace::PlaneSpace(Plane::PositionType outer) :
        mOuterType(outer)
    {
    }
    //------------------------------------------------------------------------
    void PlaneSpace::remove(const Plane & o)
    {
        PlaneList::iterator i, iend = mPlanes.end();
        for(i = mPlanes.begin(); i != iend; ++i)
        {
            if(*i == o)
            {
                mPlanes.erase(i);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    bool PlaneSpace::intersects(const AABox & box) const
    {
        PlaneList::const_iterator i, iend = mPlanes.end();
        for(i = mPlanes.begin(); i != iend; ++i)
        {
            const Plane & plane = *i;

            Plane::PositionType side = plane.getSide(box);
            if(side == mOuterType)
            {
                // 找到一个分裂平面,为此返回不相交
                return false;
            }
        }

        // 找不到一个分裂平面,假设相交
        return true;
    }
    //------------------------------------------------------------------------
    bool PlaneSpace::intersects(const Sphere & sphere) const
    {
        PlaneList::const_iterator i, iend = mPlanes.end();
        for(i = mPlanes.begin(); i != iend; ++i)
        {
            const Plane & plane = *i;

            // 测试球在平面哪个面
            NIIf d = plane.distance(sphere.getCenter());
            // 否认d 如果平面点向内
            if(mOuterType == Plane::PT_Negative)
                d = -d;

            if((d - sphere.getRadius()) > 0)
                return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
}