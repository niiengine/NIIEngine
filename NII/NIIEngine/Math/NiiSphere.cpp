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

#include"NiiPreProcess.h"
#include"NiiSphere.h"
#include"NiiVector3.h"
#include"NiiPlane.h"
#include"NiiMath.h"

namespace NII
{
    //------------------------------------------------------------------------
    Sphere::Sphere() :
        mRadius(1.0),
        mCenter(vector3::ZERO)
    {
    }
    //------------------------------------------------------------------------
    Sphere::Sphere(const vector3 & center, NIIf radius) :
        mRadius(radius),
        mCenter(center)
    {
    }
    //------------------------------------------------------------------------
    NIIf Sphere::getRadius() const
    {
        return mRadius;
    }
    //------------------------------------------------------------------------
    void Sphere::setRadius(NIIf radius)
    {
        mRadius = radius;
    }
    //------------------------------------------------------------------------
    void Sphere::setCenter(const Vector3f & center)
    {
        mCenter = center;
    }
    //------------------------------------------------------------------------
    const Vector3f & Sphere::getCenter() const
    {
        return mCenter;
    }
    //------------------------------------------------------------------------
    void Sphere::merge(const Sphere & o)
    {
        Vector3f diff =  o.mCenter - mCenter;
        NIIf lengthSq = diff.lengthSquared();
        NIIf radiusDiff = o.mRadius - mRadius;

        // Early-out
        if (Math::Sqr(radiusDiff) >= lengthSq)
        {
            // One fully contains the other
            if (radiusDiff <= 0.0f)
                return; // no change
            else
            {
                mCenter = o.mCenter;
                mRadius = o.mRadius;
                return;
            }
        }

        NIIf length = Math::sqrt(lengthSq);

        Vector3f newCenter;
        NIIf newRadius;
        if((length + o.mRadius) > mRadius)
        {
            NIIf t = (length + radiusDiff) / (2.0f * length);
            newCenter = mCenter + diff * t;
        }
        // otherwise, we keep our existing center

        newRadius = 0.5f * (length + mRadius + o.mRadius);

        mCenter = newCenter;
        mRadius = newRadius;
    }
    //------------------------------------------------------------------------
    bool Sphere::intersects(const Vector3f & v) const
    {
        return ((v - mCenter).lengthSquared() <= Math::Sqr(mRadius));
    }
    //------------------------------------------------------------------------
    bool Sphere::intersects(const Sphere & s) const
    {
        return (s.mCenter - mCenter).lengthSquared() <= Math::Sqr(s.mRadius + mRadius);
    }
    //------------------------------------------------------------------------
    bool Sphere::intersects(const AABox & box) const
    {
        // Use splitting planes
        const Vector3f & center = mCenter;
        NIIf radius = mRadius;
        const Vector3f & min = box.getMin();
        const Vector3f & max = box.getMax();

        // Arvo's algorithm
        NIIf s, d = 0;

        if(center.x < min.x)
        {
            s = center.x - min.x;
            d += s * s;
        }
        else if(center.x > max.x)
        {
            s = center.x - max.x;
            d += s * s;
        }

        if(center.y < min.y)
        {
            s = center.y - min.y;
            d += s * s;
        }
        else if(center.y > max.y)
        {
            s = center.y - max.y;
            d += s * s;
        }

        if(center.z < min.z)
        {
            s = center.z - min.z;
            d += s * s;
        }
        else if(center.z > max.z)
        {
            s = center.z - max.z;
            d += s * s;
        }

        return d <= radius * radius;
    }
    //-------------------------------------------------------------------
    bool Sphere::intersects(const Plane & plane) const
    {
        return (Math::Abs(plane.distance(mCenter)) <= mRadius);
    }
    //------------------------------------------------------------------------
}