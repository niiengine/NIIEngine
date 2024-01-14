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
                // �ҵ�һ������ƽ��,Ϊ�˷��ز��ཻ
                return false;
            }
        }

        // �Ҳ���һ������ƽ��,�����ཻ
        return true;
    }
    //------------------------------------------------------------------------
    bool PlaneSpace::intersects(const Sphere & sphere) const
    {
        PlaneList::const_iterator i, iend = mPlanes.end();
        for(i = mPlanes.begin(); i != iend; ++i)
        {
            const Plane & plane = *i;

            // ��������ƽ���ĸ���
            NIIf d = plane.distance(sphere.getCenter());
            // ����d ���ƽ�������
            if(mOuterType == Plane::PT_Negative)
                d = -d;

            if((d - sphere.getRadius()) > 0)
                return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
}