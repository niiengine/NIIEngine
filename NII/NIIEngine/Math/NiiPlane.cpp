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
#include "NiiPlane.h"
#include "NiiMatrix3.h"
#include "NiiMatrix4.h"

namespace NII
{
    //-----------------------------------------------------------------------
    Plane::Plane() :
        mNormal(Vector3f::ZERO),
        mD(0.0f){}
    //-----------------------------------------------------------------------
    Plane::Plane (const Plane & o)
    {
        mNormal = o.mNormal;
        mD = o.mD;
    }
    //-----------------------------------------------------------------------
    Plane::Plane (const Vector3f & normal, NIIf constant)
    {
        mNormal = normal;
        mD = -constant;
    }
    //---------------------------------------------------------------------
    Plane::Plane (NIIf a, NIIf b, NIIf c, NIIf d) :
        mNormal(a, b, c),
        mD(d){}
    //-----------------------------------------------------------------------
    Plane::PositionType Plane::getSide(const Vector3f & o) const
    {
        NIIf fDistance = distance(o);

        if(fDistance < 0.0)
            return Plane::PT_Negative;

        if(fDistance > 0.0)
            return Plane::PT_Positive;

        return Plane::PT_None;
    }
    //-----------------------------------------------------------------------
    Plane::PositionType Plane::getSide(const AABox & o) const
    {
        const Vector3f & centre = o.getCenter();
        const Vector3f & halfSize = o.getSize() * 0.5;

        // Calculate the distance between box centre and the plane
        NIIf dist = distance(centre);

        // Calculate the maximise allows absolute distance for
        // the distance between box centre and plane
        NIIf maxAbsDist = Math::Abs(mNormal.x * halfSize.x) +
            Math::Abs(mNormal.y * halfSize.y) + Math::Abs(mNormal.z * halfSize.z);

        if(dist < -maxAbsDist)
            return Plane::PT_Negative;

        if(dist > +maxAbsDist)
            return Plane::PT_Positive;

        return Plane::PT_Merge;
    }
    //-----------------------------------------------------------------------
    void Plane::set(const Vector3f & p1, const Vector3f & p2, const Vector3f & p3)
    {
        Vector3f kEdge1 = p2 - p1;
        Vector3f kEdge2 = p3 - p1;
        mNormal = kEdge1.crossProduct(kEdge2);
        mNormal.normalise();
        mD = -mNormal.dotProduct(p1);
    }
    //-----------------------------------------------------------------------
    Vector3f Plane::project(const Vector3f & p) const
    {
        // We know plane normal is unit length, so use simple method
        Matrix3f xform;
        xform[0][0] = 1.0f - mNormal.x * mNormal.x;
        xform[0][1] = -mNormal.x * mNormal.y;
        xform[0][2] = -mNormal.x * mNormal.z;
        xform[1][0] = -mNormal.y * mNormal.x;
        xform[1][1] = 1.0f - mNormal.y * mNormal.y;
        xform[1][2] = -mNormal.y * mNormal.z;
        xform[2][0] = -mNormal.z * mNormal.x;
        xform[2][1] = -mNormal.z * mNormal.y;
        xform[2][2] = 1.0f - mNormal.z * mNormal.z;
        return xform * p;
    }
    //-----------------------------------------------------------------------
    NIIf Plane::normalise()
    {
        NIIf fLength = mNormal.length();

        if(fLength > 0.0f)
        {
            NIIf fInvLength = 1.0f / fLength;
            mNormal *= fInvLength;
            mD *= fInvLength;
        }

        return fLength;
    }
    //-----------------------------------------------------------------------
    void Plane::getReflect(Matrix4f * out) const
    {
        out->m[0][0] = -2 * mNormal.x * mNormal.x + 1;
        out->m[0][1] = -2 * mNormal.x * mNormal.y;
        out->m[0][2] = -2 * mNormal.x * mNormal.z;
        out->m[0][3] = -2 * mNormal.x * mD;

        out->m[1][0] = -2 * mNormal.y * mNormal.x;
        out->m[1][1] = -2 * mNormal.y * mNormal.y + 1;
        out->m[1][2] = -2 * mNormal.y * mNormal.z;
        out->m[1][3] = -2 * mNormal.y * mD;

        out->m[2][0] = -2 * mNormal.z * mNormal.x;
        out->m[2][1] = -2 * mNormal.z * mNormal.y;
        out->m[2][2] = -2 * mNormal.z * mNormal.z + 1;
        out->m[2][3] = -2 * mNormal.z * mD;

        out->m[3][0] = 0;
        out->m[3][1] = 0;
        out->m[3][2] = 0;
        out->m[3][3] = 1;
    }
    //-----------------------------------------------------------------------
    void Plane::transform(const Matrix4f & m)
    {
        Matrix4f invTrans = m.inverse().T();
        Vector4f v4(mNormal.x, mNormal.y, mNormal.z, mD);
        v4 = invTrans * v4;
        mNormal.x = v4.x;
        mNormal.y = v4.y;
        mNormal.z = v4.z;
        mD = v4.w / mNormal.normalise();
    }
    //----------------------------------------------------------------------------
    bool Plane::getPlane(NIIf a, NIIf b, NIIf c, NIIf d, Plane & dest)
    {
        NIIf slen = a * a + b * b + c * c;
        if (slen > 0.000001f)
        {
            NIIf invlen = Math::invSqrt(slen);
            dest = Plane(-a * invlen, -b * invlen, -c * invlen, d * invlen);
            return 1;
        }
        else
            return 0;
    }
    //-----------------------------------------------------------------------
}