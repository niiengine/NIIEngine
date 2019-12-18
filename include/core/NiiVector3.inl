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

	//内联
    //------------------------------------------------------------------------
    template <typename T> const Vector3<T> Vector3<T>::ZERO = Vector3<T>(0, 0, 0);
    template <typename T> const Vector3<T> Vector3<T>::UNIT = Vector3<T>(1, 1, 1);
    template <typename T> const Vector3<T> Vector3<T>::X = Vector3<T>(1, 0, 0);
    template <typename T> const Vector3<T> Vector3<T>::Y = Vector3<T>(0, 1, 0);
    template <typename T> const Vector3<T> Vector3<T>::Z = Vector3<T>(0, 0, 1);
    template <typename T> const Vector3<T> Vector3<T>::INV_X = Vector3<T>(-1, 0, 0);
    template <typename T> const Vector3<T> Vector3<T>::INV_Y = Vector3<T>(0, -1, 0);
    template <typename T> const Vector3<T> Vector3<T>::INV_Z = Vector3<T>(0, 0, -1);
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T>::Vector3()
    {
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T>::Vector3(const T & t) :
        x(t),
        y(t),
        z(t)
    {
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T>::Vector3(const T & _x, const T & _y, const T & _z) :
        x(_x),
        y(_y),
        z(_z)
    {
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T>::Vector3(const Vector3 & o) :
        x(o.x),
        y(o.y),
        z(o.z)
    {
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> & Vector3<T>::operator =(const T & s)
    {
        x = s;
        y = s;
        z = s;
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> & Vector3<T>::operator =(const Vector3<T> & o)
    {
        x = o.x;
        y = o.y;
        z = o.z;
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline bool Vector3<T>::operator ==(const Vector3<T> & o) const
    {
        return (x == o.x && y == o.y && z == o.z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline bool Vector3<T>::operator !=(const Vector3<T> & o) const
    {
        return (x != o.x || y != o.y || z != o.z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::operator +(const T & s) const
    {
        return Vector3<T>(x + s, y + s, z + s);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::operator -(const T & s) const
    {
        return Vector3<T>(x - s, y - s, y - s);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::operator *(const T & s) const
    {
        return Vector3<T>(x * s, y * s, z * s);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::operator /(const T & s) const
    {
        NIIf inv = 1.0f / s;

        return Vector3<T>(x * inv, y * inv, z * inv);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::operator +(const Vector3<T> & o) const
    {
        return Vector3<T>(x + o.x, y + o.y, z + o.z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::operator -(const Vector3<T> & o) const
    {
        return Vector3<T>(x - o.x, y - o.y, z - o.z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::operator *(const Vector3<T> & o) const
    {
        return Vector3<T>(x * o.x, y * o.y, z * o.z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::operator /(const Vector3<T> & o) const
    {
        return Vector3<T>(x / o.x, y / o.y, z / o.z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::operator +() const
    {
        return Vector3<T>(x, y, z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::operator -() const
    {
        return Vector3<T>(-x, -y, -z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> & Vector3<T>::operator +=(const Vector3<T> & o)
    {
        x += o.x;
        y += o.y;
        z += o.z;
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> & Vector3<T>::operator -=(const Vector3<T> & o)
    {
        x -= o.x;
        y -= o.y;
        z -= o.z;
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> & Vector3<T>::operator *=(const Vector3<T> & o)
    {
        x *= o.x;
        y *= o.y;
        z *= o.z;
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> & Vector3<T>::operator /=(const Vector3<T> & o)
    {
        x /= o.x;
        y /= o.y;
        z /= o.z;
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> & Vector3<T>::operator +=(const T & s)
    {
        x += s;
        y += s;
        z += s;
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> & Vector3<T>::operator -=(const T & s)
    {
        x -= s;
        y -= s;
        z -= s;
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> & Vector3<T>::operator *=(const T & s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> & Vector3<T>::operator /=(const T & s)
    {
        NIIf inv = 1.0f / s;
        x *= inv;
        y *= inv;
        z *= inv;
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> T & Vector3<T>::operator[](Nidx l)
    {
        return *(&x + l);
    }
    //-------------------------------------------------------------------------
    template <typename T> const T & Vector3<T>::operator[](Nidx l) const
    {
        return *(&x + l);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline T * Vector3<T>::data() const
    {
        return &x;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline T Vector3<T>::length() const
    {
        return Math::sqrt(x * x + y * y + z * z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline T Vector3<T>::lengthFast()
    {
        NIIf lenS;
        lenS = x * x + y * y + z * z;
        return lenS * Math::RInvSqrt(lenS);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline T Vector3<T>::lengthSquared() const
    {
        return x * x + y * y + z * z;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline T Vector3<T>::distance(const Vector3<T> & o) const
    {
        return (*this - o).length();
    }
    //-------------------------------------------------------------------------
    template <typename T> inline T Vector3<T>::distanceSquared(const Vector3<T> & o) const
    {
        return (*this - o).lengthSquared();
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::crossProduct(const Vector3<T> & o) const
    {
        return Vector3<T>(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline T Vector3<T>::dotProduct(const Vector3<T> & o) const
    {
        return x * o.x + y * o.y + z * o.z;
    }
    //-----------------------------------------------------------------
    template <typename T> bool Vector3<T>::isInTriangle(const Vector3<T> & a,
        const Vector3<T> & b, const Vector3<T> & c, const Vector3<T> & normal)
    {
        Vector3<T> v1 = b - a;
        Vector3<T> v2 = this - a;

        bool zeroDot[3];
        NIIf dot[3];

        dot[0] = v1.crossProduct(v2).dotProduct(normal);
        zeroDot[0] = Math::isEqual(dot[0], 0.0f, 1e-3);

        v1 = c - b;
        v2 = this - b;

        dot[1] = v1.crossProduct(v2).dotProduct(normal);
        zeroDot[1] = Math::isEqual(dot[1], 0.0f, 1e-3);

        if(!zeroDot[0] && !zeroDot[1] && Math::Sign(dot[0]) != Math::Sign(dot[1]))
        {
            return false;
        }

        v1 = a - c;
        v2 = this - c;

        dot[2] = v1.crossProduct(v2).dotProduct(normal);
        zeroDot[2] = Math::isEqual(dot[2], 0.0f, 1e-3);

        if((!zeroDot[0] && !zeroDot[2] && Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
            (!zeroDot[1] && !zeroDot[2] && Math::Sign(dot[1]) != Math::Sign(dot[2])))
        {
            return false;
        }
        return true;
    }
    //-------------------------------------------------------------------------
    template <typename T> T Vector3<T>::normalise()
    {
        NIIf lenS, lenInv;
        lenS = x * x + y * y + z * z;
        lenInv = Math::invSqrt(lenS);
        x *= lenInv;
        y *= lenInv;
        z *= lenInv;
        return lenInv * lenS;
    }
    //-------------------------------------------------------------------------
    template <typename T> T Vector3<T>::normaliseFast()
    {
        T lenS, lenInv;
        lenS = x * x + y * y + z * z;
        lenInv = Math::RInvSqrt(lenS);
        x *= lenInv;
        y *= lenInv;
        z *= lenInv;
        return lenInv * lenS;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline void Vector3<T>::normalise(Vector3<T> & out) const
    {
        out = *this;
        out.normalise();
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::middle(const Vector3<T> & vec) const
    {
        return Vector3<T>((x + vec.x) * 0.5f, (y + vec.y) * 0.5f, (z + vec.z) * 0.5f);
    }
    //-------------------------------------------------------------------------
    template <typename T> Vector3<T> & Vector3<T>::scale(T scale)
    {
        NIIf lenS, lenInv;
        if(!scale)
        {
            x = 0;
            y = 0;
            z = 0;
        }
        else
        {
            lenS = lengthSquared();
            if(lenS > scale * scale)
            {
                lenInv = scale * Math::invSqrt(lenS);
                x *= lenInv;
                y *= lenInv;
                z *= lenInv;
            }
        }
        return *this;
    }
    //-------------------------------------------------------------------------
    template <typename T> Vector3<T> Vector3<T>::floor(const Vector3<T> & o) const
    {
        Vector3<T> temp;
        if(o.x < x)
            temp.x = o.x;
        else
            temp.x = x;

        if(o.y < y)
            temp.y = o.y;
        else
            temp.y = y;

        if(o.z < z)
            temp.z = o.z;
        else
            temp.z = z;

        return temp;
    }
    //-------------------------------------------------------------------------
    template <typename T> Vector3<T> Vector3<T>::ceil(const Vector3<T> & o) const
    {
        Vector3<T> temp;
        if(o.x > x)
            temp.x = o.x;
        else
            temp.x = x;

        if(o.y > y)
            temp.y = o.y;
        else
            temp.y = y;

        if(o.z > z)
            temp.z = o.z;
        else
            temp.z = z;

        return temp;
    }
    //-------------------------------------------------------------------------
    template <typename T> Vector3<T> Vector3<T>::vertical() const
    {
        Vector3<T> first = this->crossProduct(Vector3<T>::X);

        if(first.lengthSquared() < 0.000000000001)
        {
            first = this->crossProduct(Vector3<T>::Y);
        }
        first.normalise();
        return first;
    }
    //-------------------------------------------------------------------------
    template <typename T> void Vector3<T>::NormalVectors(Vector3<T> & left, Vector3<T> & down)
    {
        NIIf d;
        d = x * x + y * y;
        if(!d)
        {
            left[0] = 1;
            left[1] = 0;
            left[2] = 0;
        }
        else
        {
            d = Math::invSqrt(d);
            left[0] = -y * d;
            left[1] = x * d;
            left[2] = 0;
        }
        down = left.crossProduct(*this);
    }
    //-------------------------------------------------------------------------
    template <typename T> void Vector3<T>::OrthogonalBasis(Vector3<T> & left, Vector3<T> & up)
    {
        NIIf l, s;

        if(Math::Fabs(z) > 0.7f)
        {
            l = y * y + z * z;
            s = Math::invSqrt(l);
            up[0] = 0;
            up[1] = z * s;
            up[2] = -y * s;
            left[0] = l * s;
            left[1] = -x * up[2];
            left[2] = x * up[1];
        }
        else
        {
            l = x * x + y * y;
            s = Math::invSqrt(l);
            left[0] = -y * s;
            left[1] = x * s;
            left[2] = 0;
            up[0] = -z * left[1];
            up[1] = z * left[0];
            up[2] = l * s;
        }
    }
    //-------------------------------------------------------------------------
    template <typename T> void Vector3<T>::ProjectOntoPlane(const Vector3<T> & normal, NIIf c)
    {
        NIIf backoff;
        backoff = dotProduct(normal);

        if(c != 1.0)
        {
            if(backoff < 0)
            {
                backoff *= c;
            }
            else
            {
                backoff /= c;
            }
        }
        *this -= backoff * normal;
    }
    //-------------------------------------------------------------------------
    template <typename T> bool Vector3<T>::ProjectAlongPlane(
        const Vector3<T> & normal, NIIf c, NIIf epsilon)
    {
        Vector3<T> cross;
        NIIf len;

        cross = crossProduct(normal).crossProduct(*this);
        cross.normalise();
        len = dotProduct(normal , cross);

        if(Math::Fabs(len) < epsilon)
        {
            return false;
        }
        cross *= c * (normal * (*this)) / len;
        (*this) -= cross;
        return true;
    }
    //-------------------------------------------------------------------------
    template <typename T> inline bool Vector3<T>::operator < (const Vector3<T> & o) const
    {
        return (x < o.x && y < o.y && z < o.z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline bool Vector3<T>::operator > (const Vector3<T> & o) const
    {
        return (x > o.x && y > o.y && z > o.z);
    }
	//-------------------------------------------------------------------------
	template <typename T> Vector3<T> Vector3<T>::random(const Radian & angle) const
	{
		Vector3<T> up = vertical();

		Radian fHalfAngle(0.5 * Radian(Math::randomf() * Math::PI2X));
		NIIf fSin = Math::Sin(fHalfAngle);
		T w1 = Math::Cos(fHalfAngle);
		T x1 = fSin * x;
		T y1 = fSin * y;
		T z1 = fSin * z;

		Vector3<T> uv, uuv;
		Vector3<T> qvec(x1, y1, z1);
		uv = qvec.crossProduct(up);
		uuv = qvec.crossProduct(uv);
		uv *= (2.0f * w1);
		uuv *= 2.0f;

		up = up + uv + uuv;

		Radian fHalfAngle2(0.5 * angle);
		fSin = Math::Sin(fHalfAngle2);
		w1 = Math::Cos(fHalfAngle2);
		x1 = fSin * up.x;
		y1 = fSin * up.y;
		z1 = fSin * up.z;

		Vector3<T> qvec1(x1, y1, z1);
		uv = qvec1.crossProduct(*this);
		uuv = qvec1.crossProduct(uv);
		uv *= (2.0f * w1);
		uuv *= 2.0f;

		return *this + uv + uuv;
	}
    //-------------------------------------------------------------------------
    template <typename T> Radian Vector3<T>::angle(const Vector3<T> & o) const
    {
        NIIf lenProduct = length() * o.length();
        NIIf f = dotProduct(o) / lenProduct;
        f = Clamp<NIIf>(f, -1.0, 1.0);
        return Math::ACos(f);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T> Vector3<T>::reflect(const Vector3<T> & normal) const
    {
        return Vector3<T>(*this - (2 * this->dotProduct(normal) * normal));
    }
    //-------------------------------------------------------------------------
    template <typename T> inline bool Vector3<T>::equal(const Vector3<T> & o, NIIf deviation) const
    {
        if(fabs(o.x - x + o.y - y + z - o.z) < deviation)
            return true;
        return false;
    }
    //-------------------------------------------------------------------------