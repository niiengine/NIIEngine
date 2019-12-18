/*
-----------------------------------------------------------------------------
大型多媒体框架
时间: 2018-5-7
文本编码: utf-8
所属公司: 深圳闽登科技有限公司
命名风格: 概论命名法
编程风格: 统筹式
管理模式: 分布式
内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)
主要成分: c++(80%) c(20%)
用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件
偏向用途: 立体游戏软件
主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com
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
#include "NiiMath.h"
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
        return Vector4<T>(x * inv, y * inv,z * inv, w * inv);
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
