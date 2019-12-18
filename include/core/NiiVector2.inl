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
    //------------------------------------------------------------------------
    template <typename T>const Vector2<T> Vector2<T>::ZERO = Vector2<T>(0, 0);
    template <typename T>const Vector2<T> Vector2<T>::UNIT = Vector2<T>(1, 1);
    template <typename T>const Vector2<T> Vector2<T>::X = Vector2<T>(1,0);
    template <typename T>const Vector2<T> Vector2<T>::Y = Vector2<T>(0,1);
    template <typename T>const Vector2<T> Vector2<T>::INV_X = Vector2<T>(-1,0);
    template <typename T>const Vector2<T> Vector2<T>::INV_Y = Vector2<T>(0,-1);
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T>::Vector2()
    {
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T>::Vector2(const T & s) :
        x(s),
        y(s)
    {
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T>::Vector2(const T & _x, const T & _y) :
        x(_x),
        y(_y)
    {
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T>::Vector2(const Vector2<T> & o) :
        x(o.x),
        y(o.y)
    {
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> & Vector2<T>::operator =(const T & s)
    {
        x = s;
        y = s;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> & Vector2<T>::operator =(const Vector2<T> & o)
    {
        x = o.x;
        y = o.y;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline bool Vector2<T>::operator <(const Vector2<T> & o) const
    {
        if(x < o.x && y < o.y)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    template <typename T> inline bool Vector2<T>::operator >(const Vector2<T> & o) const
    {
        if(x > o.x && y > o.y)
            return true;
        return false;
    }
    //------------------------------------------------------------------------
    template <typename T> inline bool Vector2<T>::operator ==(const Vector2<T> & o) const
    {
        return (x == o.x && y == o.y);
    }
    //------------------------------------------------------------------------
    template <typename T> inline bool Vector2<T>::operator !=(const Vector2<T> & o) const
    {
        return (x != o.x || y != o.y);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::operator +(const Vector2<T> & o) const
    {
        return Vector2<T>(x + o.x, y + o.y);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::operator -(const Vector2<T> & o) const
    {
        return Vector2<T>(x - o.x, y - o.y);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::operator *(const Vector2<T> & o) const
    {
        return Vector2<T>(x * o.x, y * o.y);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::operator +(const T & s) const
    {
        return Vector2<T>(x + s, y + s);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::operator -(const T & s) const
    {
        return Vector2<T>(x - s, y - s);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::operator *(const T & s) const
    {
        return Vector2<T>(x * s, y * s);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::operator /(const T & s) const
    {
        NIIf inv = 1.0f / s;

        return Vector2<T>(x * inv, y * inv);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::operator /(const Vector2<T> & o) const
    {
        return Vector2<T>(x / o.x, y / o.y);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::operator +() const
    {
        return Vector2<T>(x, y);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::operator -() const
    {
        return Vector2<T>(-x, -y);
    }/*
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> operator *(const T & s, const Vector2<T> & o)
    {
        return Vector2<T>(s * o.x, s * o.y);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> operator /(const T & s, const Vector2<T> & o)
    {
        return Vector2<T>(s / o.x, s / o.y);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> operator +(const T & s, const Vector2<T> & o)
    {
        return Vector2<T>(s + o.x, s + o.y);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> operator -(const T & s, const Vector2<T> & o)
    {
        return Vector2<T>(s - o.x, s - o.y);
    }*/
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> & Vector2<T>::operator +=(const Vector2<T> & o)
    {
        x += o.x;
        y += o.y;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> & Vector2<T>::operator +=(const T & s)
    {
        x += s;
        y += s;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> & Vector2<T>::operator -=(const Vector2<T> & o)
    {
        x -= o.x;
        y -= o.y;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> & Vector2<T>::operator -=(const T & s)
    {
        x -= s;
        y -= s;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> & Vector2<T>::operator *=(const T & s)
    {
        x *= s;
        y *= s;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> & Vector2<T>::operator *=(const Vector2<T> & o)
    {
        x *= o.x;
        y *= o.y;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> & Vector2<T>::operator /=(const T & s)
    {
        NIIf inv = 1.0f / s;

        x *= inv;
        y *= inv;
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> & Vector2<T>::operator /=(const Vector2<T> & o)
    {
        x /= o.x;
        y /= o.y;
        return *this;
    }
	//------------------------------------------------------------------------
	template <typename T> inline T * Vector2<T>::data()
	{
		return &x;
	}
    //------------------------------------------------------------------------
    template <typename T> inline const T * Vector2<T>::data() const
    {
        return &x;
    }
    //------------------------------------------------------------------------
    template <typename T> inline T Vector2<T>::length() const
    {
        NIIf temp = x * x + y * y;
        return Math::Sqrt(temp);
    }
    //------------------------------------------------------------------------
    template <typename T> inline T Vector2<T>::lengthFast() const
    {
        NIIf sqrlen = x * x + y * y;
        return sqrlen * Math::RInvSqrt(sqrlen);
    }
    //------------------------------------------------------------------------
    template <typename T> inline T Vector2<T>::lengthSquared() const
    {
        return x * x + y * y;
    }
    //------------------------------------------------------------------------
    template <typename T> inline T Vector2<T>::distance(const Vector2<T> & o) const
    {
        return Vector2<T>(x - o.x, y - o.y).length();
    }
    //------------------------------------------------------------------------
    template <typename T> inline T Vector2<T>::distanceSquared(const Vector2<T> & o) const
    {
        return Vector2<T>(x - o.x, y - o.y).lengthSquared();
    }
    //------------------------------------------------------------------------
    template <typename T> inline T Vector2<T>::dotProduct(const Vector2<T> & o) const
    {
        return x * o.x + y * o.y;
    }
    //------------------------------------------------------------------------
    template <typename T> inline T Vector2<T>::crossProduct(const Vector2<T> & o) const
    {
        return x * o.y - y * o.x;
    }
    //-----------------------------------------------------------------
    template <typename T> bool Vector2<T>::isInTriangle(const Vector2<T> & a,
        const Vector2<T> & b, const Vector2<T> & c)
    {
        Vector2<T> v1, v2;
        NIIf dot[3];
        bool zeroDot[3];

        v1 = b - a;
        v2 = this - a;

        dot[0] = v1.crossProduct(v2);
        zeroDot[0] = Math::isEqual(dot[0], 0.0f, 1e-3);

        v1 = c - b;
        v2 = this - b;

        dot[1] = v1.crossProduct(v2);
        zeroDot[1] = Math::isEqual(dot[1], 0.0f, 1e-3);

        if(!zeroDot[0] && !zeroDot[1] && Math::Sign(dot[0]) != Math::Sign(dot[1]))
        {
            return false;
        }

        v1 = a - c;
        v2 = this - c;

        dot[2] = v1.crossProduct(v2);
        zeroDot[2] = Math::isEqual(dot[2], 0.0f, 1e-3);

        if((!zeroDot[0] && !zeroDot[2] && Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
            (!zeroDot[1] && !zeroDot[2] && Math::Sign(dot[1]) != Math::Sign(dot[2])))
        {
            return false;
        }
        return true;
    }
    //------------------------------------------------------------------------
    template <typename T> T Vector2<T>::normalise()
    {
        T sqrlen, invlen;

        sqrlen = x * x + y * y;
        invlen = Math::invSqrt(sqrlen);
        x *= invlen;
        y *= invlen;

        return invlen * sqrlen;
    }
    //------------------------------------------------------------------------
    template <typename T> T Vector2<T>::normaliseFast()
    {
        T sqrlen, invlen;

        sqrlen = x * x + y * y;
        invlen = Math::RInvSqrt(sqrlen);
        x *= invlen;
        y *= invlen;
        return invlen * sqrlen;
    }
    //------------------------------------------------------------------------
    template <typename T> inline void Vector2<T>::normalise(Vector2<T> & out) const
    {
        out = *this;
        out.normalise();
    }
    //------------------------------------------------------------------------
    template <typename T> Vector2<T> Vector2<T>::scale(NIIf scale)
    {
        T slen;
        T invlen;
        if(scale == 0)
        {
            return Vector2<T>(0, 0);
        }

        slen = lengthSquared();
        if(slen > scale * scale)
        {
            invlen = scale * Math::invSqrt(slen);
            x *= invlen;
            y *= invlen;
            return Vector2<T>(x * invlen, y * invlen);
        }
        return *this;
    }
    //------------------------------------------------------------------------
    template <typename T> inline T Vector2<T>::angle(const Vector2<T> & o)const
    {
        return Math::ACos((*this) * o / (length() * o.length()));
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::middle(const Vector2<T> & o) const
    {
        return Vector2<T>((x + o.x) * 0.5f, (y + o.y) * 0.5f);
    }
    //------------------------------------------------------------------------
    template <typename T> Vector2<T> Vector2<T>::floor(const Vector2<T> & o)
    {
        Vector2<T> temp;
        if(o.x < x)
            temp.x = o.x;
        else
            temp.x = x;
        if(o.y < y)
            temp.y = o.y;
        else
            temp.y = y;
        return temp;
    }
    //------------------------------------------------------------------------
    template <typename T> Vector2<T> Vector2<T>::ceil(const Vector2<T> & o)
    {
        Vector2<T> temp;
        if(o.x > x)
            temp.x = o.x;
        else
            temp.x = x;
        if(o.y > y)
            temp.y = o.y;
        else
            temp.y = y;
        return temp;
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::reflect(const Vector2<T> & normal) const
    {
        return Vector2<T>(*this - (2 * this->dotProduct(normal) * normal));
    }
    //------------------------------------------------------------------------
    template <typename T> Radian Vector2<T>::angle2(const Vector2<T> & o) const
    {
        NIIf len = length() * o.length();

        if(len < 0.000001)
            len = 0.000001;

        NIIf f = dotProduct(o) / len;

        f = Math::clamp(f, -1.0, 1.0);
        return Math::ACos(f);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::vertical() const
    {
        return Vector2<T>(-y, x);
    }
    //------------------------------------------------------------------------
    template <typename T> inline Vector2<T> Vector2<T>::random(T angle) const
    {
        angle *=  Math::randomf() * Math::PI2X;
        NIIf cosa = cos(angle);
        NIIf sina = sin(angle);
        return Vector2<T>(cosa * x - sina * y, sina * x + cosa * y);
    }
    //------------------------------------------------------------------------
