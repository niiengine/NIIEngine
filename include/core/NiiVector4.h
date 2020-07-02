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
/*
Copyright (c) "2018-2019", Shenzhen Mindeng Technology Co., Ltd(www.niiengine.com),
        Mindeng Base Communication Application Framework
All rights reserved.
    Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
    Redistributions of source code must retain the above copyright notice, this list of
conditions and the following disclaimer.
    Redistributions in binary form must reproduce the above copyright notice, this list
of conditions and the following disclaimer in the documentation and/or other materials
provided with the distribution.
    Neither the name of the "ORGANIZATION" nor the names of its contributors may be used
to endorse or promote products derived from this software without specific prior written
permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _NII_Vector4_H_
#define _NII_Vector4_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"
#include "NiiMath.h"

namespace NII
{
    /** 四维齐次向量
    @version NIIEngine 3.0.0
    */
    template <typename T> class Vector4
    {
    public:
        Vector4();
        Vector4(const T & t);
        Vector4(const T & x, const T & y, const T & z);
        Vector4(const T & x, const T & y, const T & z, const T & w);

        Vector4<T> & operator =(const T & t);
        
        Vector4<T> & operator =(const Vector3<T> & o);
        
        Vector4<T> & operator =(const Vector4<T> & o);

        bool operator ==(const Vector4<T> & o) const;

        bool operator !=(const Vector4<T> & o) const;

        bool operator < (const Vector4<T> & o) const;
        
        bool operator > (const Vector4<T> & o) const;
        
        Vector4<T> operator +(const Vector4<T> & o) const;

        Vector4<T> operator -(const Vector4<T> & o) const;

        Vector4<T> operator *(const Vector4<T> & o) const;

        Vector4<T> operator /(const Vector4<T> & o) const;    
        
        Vector4<T> operator +(const T & t) const;
        
        Vector4<T> operator -(const T & t) const;
        
        Vector4<T> operator *(const T & t) const;
        
        Vector4<T> operator /(const T & t) const;

        Vector4<T> operator +() const;

        Vector4<T> operator -() const;
        
        friend Vector4<T> operator + (T t, const Vector4<T> & o)
        {
            return Vector4<T>(t + o.x, t + o.y, t + o.z, t + o.w);
        }
        
        friend Vector4<T> operator - (T t, const Vector4<T> & o)
        {
            return Vector4<T>(t - o.x, t - o.y, t - o.z, t - o.w);
        }
                
        friend Vector4<T> operator * (T t, const Vector4<T> & o)
        {
            return Vector4<T>(t * o.x, t * o.y, t * o.z, t * o.w);
        }

        friend Vector4<T> operator / (T t, const Vector4<T> & o)
        {
            return Vector4<T>(t / o.x, t / o.y, t / o.z, t / o.w);
        }

        Vector4<T> & operator +=(const Vector4<T> & o);

        Vector4<T> & operator -=(const Vector4<T> & o);

        Vector4<T> & operator *=(const Vector4<T> & o);

        Vector4<T> & operator /=(const Vector4<T> & o);  
        
        Vector4<T> & operator +=(const T & s);

        Vector4<T> & operator -=(const T & s);

        Vector4<T> & operator *=(const T & s);
        
        Vector4<T> & operator /=(const T & s);

        T & operator[](Nidx l);

        const T & operator[](Nidx l)const;

        /** 获取数据指针
        @version NIIEngine 3.0.0
        */
        const T * data() const;
        
        /** 单位化
        @remark 单位化,数据范围[0, 1]
        @return [0, 1]
        @version NIIEngine 3.0.0
        */
        T normalize();

        /** 单位化
        @remark normalize的快速算法,快但不准确
        @return [0, 1]
        @version NIIEngine 3.0.0
        */
        T normalizeFast();

        /** 点积
        @param 需要一个向量和本方法的发出向量
        @return 一个描述点积的浮点类型值
        @version NIIEngine 3.0.0
        */
        T dotProduct(const Vector4<T> & o) const;

        /** 计算面的法线
        @note 结果已经单位化
        @version NIIEngine 3.0.0
        */
        static Vector4<T> calcNormals(const Vector3<T> & v1, const Vector3<T> & v2, const Vector3<T> & v3);

        /// 指定的静态常量
        static const Vector4<T> ZERO;
        static const Vector4<T> UNIT;
        static const Vector4<T> X;
        static const Vector4<T> Y;
        static const Vector4<T> Z;
        static const Vector4<T> W;
        
        typedef Vector4<T>          value_type;
        typedef value_type &        reference;
        typedef const value_type &  const_reference;
        typedef value_type *        pointer;
        typedef const value_type *  const_pointer;
        typedef Nidx              index_type;
    public:
        T x, y, z, w;
    };

    // 内联
    //------------------------------------------------------------------------
    template <typename T>const Vector4<T> Vector4<T>::ZERO = Vector4<T>(0, 0, 0, 0);
    template <typename T>const Vector4<T> Vector4<T>::UNIT = Vector4<T>(1, 1, 1, 1);
    template <typename T>const Vector4<T> Vector4<T>::X = Vector4<T>(1, 0, 0, 0);
    template <typename T>const Vector4<T> Vector4<T>::Y = Vector4<T>(0, 1, 0, 0);
    template <typename T>const Vector4<T> Vector4<T>::Z = Vector4<T>(0, 0, 1, 0);
    template <typename T>const Vector4<T> Vector4<T>::W = Vector4<T>(0, 0, 0, 1);
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T>::Vector4()
    {
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T>::Vector4(const T & t) :
        x(t),
        y(t),
        z(t),
        w(t)
    {
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T>::Vector4(const T & _x, const T & _y, const T & _z) :
        x(_x),
        y(_y),
        z(_z)
    {
        w = T(1);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T>::Vector4(const T & _x, const T & _y, const T & _z, const T & _w) :
        x(_x),
        y(_y),
        z(_z),
        w(_w)
    {
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> & Vector4<T>::operator =(const T & t)
    {
        x = t;
        y = t;
        z = t;
        w = t;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> & Vector4<T>::operator =(const Vector3<T> & o)
    {
        x = o.x;
        y = o.y;
        z = o.z;
        w = T(1);
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> & Vector4<T>::operator =(const Vector4<T> & o)
    {
        x = o.x;
        y = o.y;
        z = o.z;
        w = o.w;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline bool Vector4<T>::operator ==(const Vector4<T> & o) const
    {
        return (x == o.x && y == o.y && z == o.z && w == o.w);
    }
    //------------------------------------------------------------------------
    template <typename T> inline bool Vector4<T>::operator !=(const Vector4<T> & o) const
    {
        return (x != o.x || y != o.y || z != o.z || w != o.w);
    }
    //------------------------------------------------------------------------
    template <typename T> inline bool Vector4<T>::operator < (const Vector4<T> & o) const
    {
        return (x < o.x && y < o.y && z < o.z && w < o.w);
    }
    //------------------------------------------------------------------------
    template <typename T> inline bool Vector4<T>::operator > (const Vector4<T> & o) const
    {
        return (x > o.x && y > o.y && z > o.z && w > o.w);;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> Vector4<T>::operator +(const Vector4<T> & o) const
    {
        return Vector4<T>(x + o.x, y + o.y, z + o.z, w + o.w);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> Vector4<T>::operator -(const Vector4<T> & o) const
    {
        return Vector4<T>(x - o.x, y - o.y, z - o.z, w - o.w);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> Vector4<T>::operator *(const Vector4<T> & o) const
    {
        return Vector4<T>(o.x * x, o.y * y, o.z * z, o.w * w);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> Vector4<T>::operator /(const Vector4<T> & o) const
    {
        return Vector4<T>(x / o.x, y / o.y, z / o.z, w / o.w);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> Vector4<T>::operator +(const T & t) const
    {
        return Vector4<T>(x + t, y + t, z + t, w + t);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> Vector4<T>::operator -(const T & t) const
    {
        return Vector4<T>(x - t, y - t, z - t, w - t);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> Vector4<T>::operator *(const T & s) const
    {
        return Vector4<T>(x * s, y * s, z * s, w * s);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> Vector4<T>::operator /(const T & s) const
    {
        NIIf inv = 1.0f / s;
        return Vector4<T>(x * inv, y * inv, z * inv, w * inv);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> Vector4<T>::operator +() const
    {
        return Vector4<T>(x, y, z, w);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> Vector4<T>::operator -() const
    {
        return Vector4<T>(-x, -y, -z, -w);
    }/*
    //------------------------------------------------------------------------
    template <typename T> Vector4<T> operator +(T t, const Vector4<T> & o)
    {
        return Vector4<T>(t + o.x, t + o.y, t + o.z, t + o.w);
    }
    //------------------------------------------------------------------------
    template <typename T> Vector4<T> operator -(T t, const Vector4<T> & o)
    {
        return Vector4<T>(t - o.x, t - o.y, t - o.z, t - o.w);
    }
    //------------------------------------------------------------------------
    template <typename T> Vector4<T> operator *(T t, const Vector4<T> & o)
    {
        return Vector4<T>(t * o.x, t * o.y, t * o.z, t * o.w);
    }
    //------------------------------------------------------------------------
    template <typename T> Vector4<T> operator /(T t, const Vector4<T> & o)
    {
        return Vector4<T>(t / o.x, t / o.y, t / o.z, t / o.w);
    }*/
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> & Vector4<T>::operator +=(const Vector4<T> & o)
    {
        x += o.x;
        y += o.y;
        z += o.z;
        w += o.w;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> & Vector4<T>::operator -=(const Vector4<T> & o)
    {
        x -= o.x;
        y -= o.y;
        z -= o.z;
        w -= o.w;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> & Vector4<T>::operator *=(const Vector4<T> & o)
    {
        x *= o.x;
        y *= o.y;
        z *= o.z;
        w *= o.w;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> & Vector4<T>::operator /=(const Vector4<T> & o)
    {
        x /= o.x;
        y /= o.y;
        z /= o.z;
        w /= o.w;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> & Vector4<T>::operator +=(const T & s)
    {
        x += s;
        y += s;
        z += s;
        w += s;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> & Vector4<T>::operator -=(const T & s)
    {
        x -= s;
        y -= s;
        z -= s;
        w -= s;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> & Vector4<T>::operator *=(const T & s)
    {
        x *= s;
        y *= s;
        z *= s;
        w *= s;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector4<T> & Vector4<T>::operator /=(const T & s)
    {
        NIIf inv = 1.0f / s;
        x *= inv;
        y *= inv;
        z *= inv;
        w *= inv;
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> T & Vector4<T>::operator[](Nidx l)
    {
        return *(&x + l);
    }
    //-------------------------------------------------------------------------
    template <typename T> const T & Vector4<T>::operator[](Nidx l) const
    {
        return *(&x + l);
    }
    //------------------------------------------------------------------------
    template <typename T> inline const T * Vector4<T>::data() const
    {
        return &x;
    }
    //------------------------------------------------------------------------
    template <typename T> T Vector4<T>::normalize()
    {
        NIIf sqrlen, invlen;

        sqrlen = x * x + y * y + z * z + w * w;
        invlen = Math::invSqrt(sqrlen);
        x *= invlen;
        y *= invlen;
        z *= invlen;
        w *= invlen;
        return invlen * sqrlen;
    }
    //------------------------------------------------------------------------
    template <typename T> T Vector4<T>::normalizeFast()
    {
        NIIf sqrlen, invlen;

        sqrlen = x * x + y * y + z * z + w * w;
        invlen = Math::RInvSqrt(sqrlen);
        x *= invlen;
        y *= invlen;
        z *= invlen;
        w *= invlen;
        return invlen * sqrlen;
    }
    //------------------------------------------------------------------------
    template <typename T> inline T Vector4<T>::dotProduct(const Vector4<T> & o) const
    {
        return x * o.x + y * o.y + z * o.z + w * o.w;
    }
    //------------------------------------------------------------------------
    template <typename T> Vector4<T> Vector4<T>::calcNormals(const Vector3<T> & v1,
        const Vector3<T> & v2, const Vector3<T> & v3)
    {
        Vector3<T> normal = (v2 - v1).crossProduct(v3 - v1);
        normal.normalise();
        // Now set up the w (distance of tri from origin
        return Vector4<T>(normal.x, normal.y, normal.z, -(normal.dotProduct(v1)));
    }
    //------------------------------------------------------------------------

    typedef Vector4<NIIi>   Vector4i;
    typedef Vector4<NIIf>   Vector4f;
    typedef Vector4<NIId>   Vector4d;
    typedef Vector4f        vector4;

    template<typename T> Nostream & operator << (Nostream & o, const Vector4<T> & v)
    {
        o << "Vector4(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
        return o;
    }
}
#endif