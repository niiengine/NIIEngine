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
/*
    For more information about Quaternions and the theory behind it, we recommend reading:
    http://www.ogre3d.org/tikiwiki/Quaternion+and+Rotation+Primer
    http://www.cprogramming.com/tutorial/3d/quaternions.html
    http://www.gamedev.net/page/resources/_/reference/programming/math-and-physics/
    quaternions/quaternion-powers-r1095
*/
// NOTE THAT THIS FILE IS BASED ON MATERIAL FROM:

// Geometric Tools, LLC
// Copyright (c) 1998-2010
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt

#include "NiiQuaternion.h"
#include "NiiMath.h"

#define FloatEpsilon 1e-03
namespace NII
{
    //-----------------------------------------------------------------------
    const Quaternion Quaternion::ZERO(0.0, 0.0, 0.0, 0.0);
    const Quaternion Quaternion::IDENTITY(1.0, 0.0, 0.0, 0.0);
    //-----------------------------------------------------------------------
    Quaternion::Quaternion() :
        w(1),
        x(0),
        y(0),
        z(0)
    {
    }
    //-----------------------------------------------------------------------
    Quaternion::Quaternion(const Quaternion & q) :
        w(q.w),
        x(q.x),
        y(q.y),
        z(q.z)
    {
    }
    //-----------------------------------------------------------------------
    Quaternion::Quaternion(NIIf _w, NIIf _x, NIIf _y, NIIf _z) :
        w(_w),
        x(_x),
        y(_y),
        z(_z)
    {
    }
    //-----------------------------------------------------------------------
    void Quaternion::from(const Matrix3f & rot)
    {
        // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
        // article "Quaternion Calculus and Fast Animation".

        NIIf fTrace = rot[0][0] + rot[1][1] + rot[2][2];
        NIIf fRoot;
        if (fTrace > 0.0)
        {
            // |w| > 1/2, may as well choose w > 1/2
            fRoot = Math::sqrt(fTrace + 1.0f);  // 2w
            w = 0.5f * fRoot;
            fRoot = 0.5f / fRoot;  // 1/(4w)
            x = (rot[2][1] - rot[1][2]) * fRoot;
            y = (rot[0][2] - rot[2][0]) * fRoot;
            z = (rot[1][0] - rot[0][1]) * fRoot;
        }
        else
        {
            // |w| <= 1/2
            static NCount s_iNext[3] = { 1, 2, 0 };
            NCount i = 0;
            if(rot[1][1] > rot[0][0])
                i = 1;
            if(rot[2][2] > rot[i][i])
                i = 2;
            NCount j = s_iNext[i];
            NCount k = s_iNext[j];

            fRoot = Math::sqrt(rot[i][i] - rot[j][j] - rot[k][k] + 1.0f);
            NIIf * apkQuat[3] = { &x, &y, &z };
            *apkQuat[i] = 0.5f * fRoot;
            fRoot = 0.5f / fRoot;
            w = (rot[k][j] - rot[j][k]) * fRoot;
            *apkQuat[j] = (rot[j][i] + rot[i][j]) * fRoot;
            *apkQuat[k] = (rot[k][i] + rot[i][k]) * fRoot;
        }
    }
    //-----------------------------------------------------------------------
    void Quaternion::to(Matrix3f & rot) const
    {
        NIIf fTx  = 2.0f * x;
        NIIf fTy  = 2.0f * y;
        NIIf fTz  = 2.0f * z;
        NIIf fTwx = fTx * w;
        NIIf fTwy = fTy * w;
        NIIf fTwz = fTz * w;
        NIIf fTxx = fTx * x;
        NIIf fTxy = fTy * x;
        NIIf fTxz = fTz * x;
        NIIf fTyy = fTy * y;
        NIIf fTyz = fTz * y;
        NIIf fTzz = fTz * z;

        rot[0][0] = 1.0f - (fTyy + fTzz);
        rot[0][1] = fTxy - fTwz;
        rot[0][2] = fTxz + fTwy;
        rot[1][0] = fTxy + fTwz;
        rot[1][1] = 1.0f - (fTxx + fTzz);
        rot[1][2] = fTyz - fTwx;
        rot[2][0] = fTxz - fTwy;
        rot[2][1] = fTyz + fTwx;
        rot[2][2] = 1.0f - (fTxx + fTyy);
    }
    //-----------------------------------------------------------------------
    void Quaternion::from(const Radian & angle, const Vector3f & axis)
    {
        // N_assert:  axis[] is unit length
        //
        // The quaternion representing the rotation is
        //   q = cos(A / 2) + sin(A / 2) * (x * i + y * j + z * k)

        Radian fHalfAngle(0.5 * angle);
        NIIf fSin = Math::Sin(fHalfAngle);
        w = Math::Cos(fHalfAngle);
        x = fSin * axis.x;
        y = fSin * axis.y;
        z = fSin * axis.z;
    }
    //-----------------------------------------------------------------------
    void Quaternion::to(Radian & angle, Vector3f & axis) const
    {
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)

        NIIf slen = x * x + y * y + z * z;
        if(slen > 0.0)
        {
            angle = 2.0 * Math::ACos(w);
            NIIf fInvLength = Math::invSqrt(slen);
            axis.x = x * fInvLength;
            axis.y = y * fInvLength;
            axis.z = z * fInvLength;
        }
        else
        {
            // angle is 0 (mod 2 * pi), so any axis will do
            angle = Radian(0.0);
            axis.x = 1.0;
            axis.y = 0.0;
            axis.z = 0.0;
        }
    }
    //-----------------------------------------------------------------------
    void Quaternion::from(const Vector3f * axis)
    {
        Matrix3f rot;

        rot[0][0] = axis[0].x;
        rot[1][0] = axis[0].y;
        rot[2][0] = axis[0].z;

        rot[0][1] = axis[1].x;
        rot[1][1] = axis[1].y;
        rot[2][1] = axis[1].z;

        rot[0][2] = axis[2].x;
        rot[1][2] = axis[2].y;
        rot[2][2] = axis[2].z;

        from(rot);
    }
    //-----------------------------------------------------------------------
    void Quaternion::to(Vector3f * axis) const
    {
        Matrix3f rot;

        to(rot);

        axis[0].x = rot[0][0];
        axis[0].y = rot[1][0];
        axis[0].z = rot[2][0];

        axis[1].x = rot[0][1];
        axis[1].y = rot[1][1];
        axis[1].z = rot[2][1];

        axis[2].x = rot[0][2];
        axis[2].y = rot[1][2];
        axis[2].z = rot[2][2];
    }
    //-----------------------------------------------------------------------
    void Quaternion::from(const Vector3f & x, const Vector3f & y, const Vector3f & z)
    {
        Matrix3f rot;

        rot[0][0] = x.x;
        rot[1][0] = x.y;
        rot[2][0] = x.z;

        rot[0][1] = y.x;
        rot[1][1] = y.y;
        rot[2][1] = y.z;

        rot[0][2] = z.x;
        rot[1][2] = z.y;
        rot[2][2] = z.z;

        from(rot);
    }
    //-----------------------------------------------------------------------
    void Quaternion::to(Vector3f & x, Vector3f & y, Vector3f & z) const
    {
        Matrix3f rot;

        to(rot);

        x.x = rot[0][0];
        x.y = rot[1][0];
        x.z = rot[2][0];

        y.x = rot[0][1];
        y.y = rot[1][1];
        y.z = rot[2][1];

        z.x = rot[0][2];
        z.y = rot[1][2];
        z.z = rot[2][2];
    }
    //-----------------------------------------------------------------------
    void Quaternion::from(const Radian & ix, const Radian & iy, const Radian & iz)
    {
        // the order of rotation:
        // 1) around Z
        // 2) around Y
        // 3) around X
        // even though it's passed in reverse...

        NIIf tz = 0.5f * iz.getValue();
        NIIf ty = 0.5f * iy.getValue();
        NIIf tx = 0.5f * ix.getValue();

        NIIf sin_z_2 = sinf(tz);
        NIIf sin_y_2 = sinf(ty);
        NIIf sin_x_2 = sinf(tx);

        NIIf cos_z_2 = cosf(tz);
        NIIf cos_y_2 = cosf(ty);
        NIIf cos_x_2 = cosf(tx);

        w = cos_z_2 * cos_y_2 * cos_x_2 + sin_z_2 * sin_y_2 * sin_x_2;
        x = cos_z_2 * cos_y_2 * sin_x_2 - sin_z_2 * sin_y_2 * cos_x_2;
        y = cos_z_2 * sin_y_2 * cos_x_2 + sin_z_2 * cos_y_2 * sin_x_2;
        z = sin_z_2 * cos_y_2 * cos_x_2 - cos_z_2 * sin_y_2 * sin_x_2;
    }
    //-----------------------------------------------------------------------
    Vector3f Quaternion::getXAxis() const
    {
        //NIIf fTx  = 2.0 * x;
        NIIf fTy  = 2.0f * y;
        NIIf fTz  = 2.0f * z;
        NIIf fTwy = fTy * w;
        NIIf fTwz = fTz * w;
        NIIf fTxy = fTy * x;
        NIIf fTxz = fTz * x;
        NIIf fTyy = fTy * y;
        NIIf fTzz = fTz * z;

        return Vector3f(1.0f - (fTyy + fTzz), fTxy + fTwz, fTxz - fTwy);
    }
    //-----------------------------------------------------------------------
    Vector3f Quaternion::getYAxis() const
    {
        NIIf fTx  = 2.0f * x;
        NIIf fTy  = 2.0f * y;
        NIIf fTz  = 2.0f * z;
        NIIf fTwx = fTx * w;
        NIIf fTwz = fTz * w;
        NIIf fTxx = fTx * x;
        NIIf fTxy = fTy * x;
        NIIf fTyz = fTz * y;
        NIIf fTzz = fTz * z;

        return Vector3f(fTxy - fTwz, 1.0f - (fTxx + fTzz), fTyz + fTwx);
    }
    //-----------------------------------------------------------------------
    Vector3f Quaternion::getZAxis() const
    {
        NIIf fTx  = 2.0f * x;
        NIIf fTy  = 2.0f * y;
        NIIf fTz  = 2.0f * z;
        NIIf fTwx = fTx * w;
        NIIf fTwy = fTy * w;
        NIIf fTxx = fTx * x;
        NIIf fTxz = fTz * x;
        NIIf fTyy = fTy * y;
        NIIf fTyz = fTz * y;

        return Vector3f(fTxz + fTwy, fTyz - fTwx, 1.0f - (fTxx + fTyy));
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::operator* (const Quaternion & o) const
    {
        // NOTE:  Multiplication is not generally commutative, so in most
        // cases p*q != q*p.
        return Quaternion
        (
            w * o.w - x * o.x - y * o.y - z * o.z,
            w * o.x + x * o.w + y * o.z - z * o.y,
            w * o.y + y * o.w + z * o.x - x * o.z,
            w * o.z + z * o.w + x * o.y - y * o.x
        );
    }
    //-----------------------------------------------------------------------
    Quaternion operator* (NIIf factor, const Quaternion & q)
    {
        return Quaternion(factor * q.w, factor * q.x, factor * q.y, factor * q.z);
    }
    //-----------------------------------------------------------------------
    NIIf Quaternion::dot(const Quaternion & q) const
    {
        return w * q.w + x * q.x + y * q.y + z * q.z;
    }
    //-----------------------------------------------------------------------
    NIIf Quaternion::length() const
    {
        return Math::sqrt(w * w + x * x + y * y + z * z);
    }
    //-----------------------------------------------------------------------
    NIIf Quaternion::lengthSquared() const
    {
        return w * w + x * x + y * y + z * z;
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::inverse() const
    {
        NIIf fNorm = w * w + x * x + y * y + z * z;
        if(fNorm > 0.0)
        {
            NIIf fInvNorm = 1.0f / fNorm;
            return Quaternion(w * fInvNorm, -x * fInvNorm, -y * fInvNorm, -z * fInvNorm);
        }
        else
        {
            // return an invalid result to flag the error
            return ZERO;
        }
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::inverseUnit() const
    {
        // N_assert:  'this' is unit length
        return Quaternion(w, -x, -y, -z);
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::Exp() const
    {
        // If q = A*(x*i+y*j+z*k) where (x,y,z) is unit length, then
        // exp(q) = e^w(cos(A)+sin(A)*(x*i+y*j+z*k)).  If sin(A) is near zero,
        // use exp(q) = e^w(cos(A)+A*(x*i+y*j+z*k)) since A/sin(A) has limit 1.

        Radian fAngle (Math::sqrt(x * x + y * y + z * z));
        NIIf fSin = Math::Sin(fAngle);
        NIIf fExpW = Math::Exp(w);
        
        Quaternion re;
        re.w = fExpW*Math::Cos(fAngle);

        if(Math::Abs(fAngle.getValue()) >= FloatEpsilon)
        {
            NIIf fCoeff = fExpW*(fSin / (fAngle.getValue()));
            re.x = fCoeff * x;
            re.y = fCoeff * y;
            re.z = fCoeff * z;
        }
        else
        {
            re.x = fExpW*x;
            re.y = fExpW*y;
            re.z = fExpW*z;
        }

        return re;
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::Ln() const
    {
        // If q = cos(A)+sin(A)*(x*i+y*j+z*k) where (x,y,z) is unit length, then
        // log(q) = A*(x*i+y*j+z*k).  If sin(A) is near zero, use log(q) =
        // sin(A)*(x*i+y*j+z*k) since sin(A)/A has limit 1.

        Quaternion kResult;
        kResult.w = 0.0;

        if(Math::Abs(w) < 1.0)
        {
            NIIf fNormV = Math::sqrt(x*x + y*y + z*z);
            Radian fAngle(Math::ATan2(fNormV, w));
            
            NIIf fSin = Math::Sin(fAngle);
            if(Math::Abs(fSin) >= FloatEpsilon)
            {
                NIIf fCoeff = fAngle.getValue()/fSin;
                kResult.x = fCoeff * x;
                kResult.y = fCoeff * y;
                kResult.z = fCoeff * z;
                return kResult;
            }
        }

        kResult.x = x;
        kResult.y = y;
        kResult.z = z;

        return kResult;
    }
    //------------------------------------------------------------------------
    Vector3f Quaternion::operator* (const Vector3f & v) const
    {
        // nVidia SDK implementation
        Vector3f uv, uuv;
        Vector3f qvec(x, y, z);
        uv = qvec.crossProduct(v);
        uuv = qvec.crossProduct(uv);
        uv *= (2.0f * w);
        uuv *= 2.0f;

        return v + uv + uuv;
    }
    //-----------------------------------------------------------------------
    bool Quaternion::equals(const Quaternion & q, const Radian & tolerance) const
    {
        NIIf fCos = dot(q);
        Radian angle = Math::ACos(fCos);

        return (Math::Abs(angle.getValue()) <= tolerance.getValue())
            || Math::isEqual(angle.getValue(), Math::PI, tolerance.getValue());
    }
    //-----------------------------------------------------------------------
    bool Quaternion::isNaN() const
    {
        return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z) || Math::isNaN(w);
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::slerp(NIIf pos, const Quaternion & src, const Quaternion & dest, 
        bool nearest)
    {
        NIIf fCos = src.dot(dest);
        Quaternion rkT;

        // Do we need to invert rotation?
        if(fCos < 0.0f && nearest)
        {
            fCos = -fCos;
            rkT = -dest;
        }
        else
        {
            rkT = dest;
        }

        if (Math::Abs(fCos) < 1 - FloatEpsilon)
        {
            // Standard case (slerp)
            NIIf fSin = Math::sqrt(1 - Math::Sqr(fCos));
            Radian fAngle = Math::ATan2(fSin, fCos);
            NIIf fInvSin = 1.0f / fSin;
            NIIf fCoeff0 = Math::Sin((1.0f - pos) * fAngle) * fInvSin;
            NIIf fCoeff1 = Math::Sin(pos * fAngle) * fInvSin;
            return fCoeff0 * src + fCoeff1 * rkT;
        }
        else
        {
            // There are two situations:
            // 1. "src" and "dest" are very close (fCos ~= +1), so we can do a linear
            //    interpolation safely.
            // 2. "src" and "dest" are almost inverse of each other (fCos ~= -1), there
            //    are an infinite number of possibilities interpolation. but we haven't
            //    have method to fix this case, so just use linear interpolation here.
            Quaternion t = (1.0f - pos) * src + pos * rkT;
            // taking the complement requires renormalisation
            t.normalise();
            return t;
        }
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::squad(NIIf fT, const Quaternion & src, const Quaternion & a,
        const Quaternion & b, const Quaternion & dst, bool nearest)
    {
        NIIf fSlerpT = 2.0f * fT * (1.0f - fT);
        Quaternion kSlerpP = slerp(fT, src, dst, nearest);
        Quaternion kSlerpQ = slerp(fT, a, b);
        return slerp(fSlerpT, kSlerpP ,kSlerpQ);
    }
    //-------------------------------------------------------------------------
    Quaternion Quaternion::getRotation(const Vector3f & o1, const Vector3f & o2,
        const Vector3f & zeroAxis)
    {
        Quaternion q;

        Vector3f v0 = o1;
        Vector3f v1 = o2;
        v0.normalise();
        v1.normalise();

        NIIf d = v0.dotProduct(v1);

        if (d >= 1.0f)
        {
            return Quaternion::IDENTITY;
        }

        if (d < (1e-6f - 1.0f))
        {
            if (zeroAxis != Vector3f::ZERO)
            {
                // rotate 180 degrees about the fallback axis
                q.from(Radian(Math::PI), zeroAxis);
            }
            else
            {
                // Generate an axis
                Vector3f axis = Vector3f::X.crossProduct(o1);
                NIIf aqsl = axis.lengthSquared();
                if (aqsl < 0.000000000001) // pick another if colinear
                    axis = Vector3f::Y.crossProduct(o1);
                axis.normalise();
                q.from(Radian(Math::PI), axis);
            }
        }
        else
        {
            NIIf s = Math::sqrt((1 + d) * 2);
            NIIf invs = 1 / s;

            Vector3f c = v0.crossProduct(v1);

            q.x = c.x * invs;
            q.y = c.y * invs;
            q.z = c.z * invs;
            q.w = s * 0.5f;
            q.normalise();
        }
        return q;
    }
    //-----------------------------------------------------------------------
    NIIf Quaternion::normalise()
    {
        NIIf len = lengthSquared();
        NIIf factor = 1.0f / Math::sqrt(len);
        *this = *this * factor;
        return len;
    }
    //-----------------------------------------------------------------------
    Radian Quaternion::getRoll(bool prj) const
    {
        if(prj)
        {
            // roll = atan2(localx.y, localx.x)
            // pick parts of getXAxis() implementation that we need
//            NIIf fTx  = 2.0*x;
            NIIf fTy  = 2.0f * y;
            NIIf fTz  = 2.0f * z;
            NIIf fTwz = fTz * w;
            NIIf fTxy = fTy * x;
            NIIf fTyy = fTy * y;
            NIIf fTzz = fTz * z;

            // Vector3f(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);

            return Radian(Math::ATan2(fTxy + fTwz, 1.0f - (fTyy + fTzz)));
        }
        else
        {
            return Radian(Math::ATan2(2 * (x * y + w * z), w * w + x * x - y * y - z * z));
        }
    }
    //-----------------------------------------------------------------------
    Radian Quaternion::getPitch(bool prj) const
    {
        if(prj)
        {
            // pitch = atan2(localy.z, localy.y)
            // pick parts of getYAxis() implementation that we need
            NIIf fTx  = 2.0f * x;
//            NIIf fTy  = 2.0f*y;
            NIIf fTz  = 2.0f * z;
            NIIf fTwx = fTx * w;
            NIIf fTxx = fTx * x;
            NIIf fTyz = fTz * y;
            NIIf fTzz = fTz * z;

            // Vector3f(fTxy-fTwz, 1.0-(fTxx+fTzz), fTyz+fTwx);
            return Radian(Math::ATan2(fTyz + fTwx, 1.0f - (fTxx + fTzz)));
        }
        else
        {
            // internal version
            return Radian(Math::ATan2(2 * (y * z + w * x), w * w - x * x - y * y + z * z));
        }
    }
    //-----------------------------------------------------------------------
    Radian Quaternion::getYaw(bool prj) const
    {
        if(prj)
        {
            // yaw = atan2(localz.x, localz.z)
            // pick parts of getZAxis() implementation that we need
            NIIf fTx  = 2.0f * x;
            NIIf fTy  = 2.0f * y;
            NIIf fTz  = 2.0f * z;
            NIIf fTwy = fTy * w;
            NIIf fTxx = fTx * x;
            NIIf fTxz = fTz * x;
            NIIf fTyy = fTy * y;

            // Vector3f(fTxz+fTwy, fTyz-fTwx, 1.0-(fTxx+fTyy));

            return Radian(Math::ATan2(fTxz + fTwy, 1.0f - (fTxx + fTyy)));
        }
        else
        {
            // internal version
            return Radian(Math::ASin(-2 * (x * z - w * y)));
        }
    }
    //-----------------------------------------------------------------------
    Quaternion Quaternion::nlerp(NIIf pos, const Quaternion & src, const Quaternion & dest, bool nearest)
    {
        Quaternion result;
        NIIf fCos = src.dot(dest);
        if(fCos < 0.0f && nearest)
        {
            result = src + pos * ((-dest) - src);
        }
        else
        {
            result = src + pos * (dest - src);
        }
        result.normalise();
        return result;
    }
    //--------------------------------------------------------------------------------
}
