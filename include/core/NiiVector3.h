/*
Copyright (c) "2018-2019", NIIEngine Team Group
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

#ifndef _NII_VECTOR3_H_
#define _NII_VECTOR3_H_

#include "NiiPreInclude.h"
#include "NiiMath.h"

namespace NII
{
    /** 标准三维向量.
    @remark 一个关于通过三正交轴的三维坐标描述的向量,注意位置和方向和缩放功能的事物
        都可以通过这个类来描述,就要看你具体怎么操作数据
    @version NIIEngine 3.0.0
    */
    template <typename T> class Vector3
    {
    public:
        Vector3();

        Vector3(const T & t);

        Vector3(const T & x, const T & y, const T & z);

        Vector3(const Vector3 & o);

        Vector3<T> & operator =(const T & s);

        Vector3<T> & operator =(const Vector3<T> & o);

        bool operator ==(const Vector3<T> & o) const;

        bool operator !=(const Vector3<T> & o) const;

        bool operator < (const Vector3<T> & o) const;

        bool operator <= (const Vector3<T> & o) const;

        bool operator > (const Vector3<T> & o) const;

        bool operator >= (const Vector3<T> & o) const;

        Vector3<T> operator +(const T & s) const;

        Vector3<T> operator -(const T & s) const;

        Vector3<T> operator *(const T & s) const;

        Vector3<T> operator /(const T & s) const;

        Vector3<T> operator +(const Vector3<T> & o) const;

        Vector3<T> operator -(const Vector3<T> & o) const;

        Vector3<T> operator *(const Vector3<T> & o) const;

        Vector3<T> operator /(const Vector3<T> & o) const;

        Vector3<T> operator +() const;

        Vector3<T> operator -() const;

        friend Vector3<T> operator +(const T & s, const Vector3<T> & o)
        {
            return Vector3<T>(s + o.x, s + o.y, s + o.z);
        }

        friend Vector3<T> operator -(const T & s, const Vector3<T> & o)
        {
            return Vector3<T>(s - o.x, s - o.y, s - o.z);
        }

        friend Vector3<T> operator *(const T & s, const Vector3<T> & o)
        {
            return Vector3<T>(s * o.x, s * o.y, s * o.z);
        }

        friend Vector3<T> operator /(const T & s, const Vector3<T> & o)
        {
            return Vector3<T>(s / o.x, s / o.y, s / o.z);
        }

        Vector3<T> & operator +=(const Vector3<T> & o);

        Vector3<T> & operator -=(const Vector3<T> & o);

        Vector3<T> & operator *=(const Vector3<T> & o);

        Vector3<T> & operator /=(const Vector3<T> & o);

        Vector3<T> & operator +=(const T & s);

        Vector3<T> & operator -=(const T & s);

        Vector3<T> & operator *=(const T & s);

        Vector3<T> & operator /=(const T & s);

        T & operator[](Nidx l);

        const T & operator[](Nidx l)const;

        /** 获取数据指针
        @version NIIEngine 3.0.0
        */
        T * data() const;

        /** 返回向量长度大小
        @remark
            返回方根,浮点操作对线程有压力,如仅比较操作使用 lengthSquared()
        */
        T length() const;

        /** length的快速算法,快但不准确
        @version NIIEngine 3.0.0
        */
        T lengthFast();

        /** 返回向量长度大小的2次方
        @remark
            这个方法具体效果比 length () 方法差一步 但能换来效率的优势如果你只是
            想在它们中找到更大(长)或更小(短)你可以用这个方法代替
        */
        T lengthSquared() const;

        /** 返回与另一个向量之间的距离
        @note
            这个方法需要用到具体长度的方法,会带来CPU效率问题,如果你不需要具体的距
            离问题 可以使用squaredDistance()代替
        @version NIIEngine 3.0.0
        */
        T distance(const Vector3<T> & o) const;

        /** 返回与另一个向量的距离的2次方
        @remark
            这个方法具体效果比 distance () 方法差一步 但能换来效率的优势如果你只是
            想在它们中找到距离更大(长)或更小(短)你可以用这个方法代替
        @version NIIEngine 3.0.0
        */
        T distanceSquared(const Vector3<T> & o) const;

        /** 计算相交,即新向量是垂直.
        @remark 未单位化
        @param[in] o
        @version NIIEngine 3.0.0
        */
        Vector3<T> crossProduct(const Vector3<T> & o) const;

        /** 计算点积
        @param[in] o
        @version NIIEngine 3.0.0
        */
        T dotProduct(const Vector3<T> & o) const;

        /** 计算点积
        @param[in] o
        @version NIIEngine 3.0.0
        */
        T volume() const;

        /** 是否在三角形里
        @version NIIEngine 3.0.0
        */
        bool isInTriangle(const Vector3<T> & a, const Vector3<T> & b, 
            const Vector3<T> & c, const Vector3<T> & normal);

        /** 单位化向量
        @remark 单位化,数据范围[0,1]
        @version NIIEngine 3.0.0
        */
        T normalise();

        /** normalise()的快速算法
        @remark Lod 动态策略中有帮助
        @version NIIEngine 3.0.0
        */
        T normaliseFast();

        /** normalise 副本返回
        @version NIIEngine 3.0.0
        */
        void normalise(Vector3<T> & out) const;

        /** 返回两顶点距离的中位数
        @version NIIEngine 3.0.0
        */
        Vector3<T> middle(const Vector3<T> & o) const;

        /** 缩放向量
        @version NIIEngine 3.0.0
        */
        Vector3<T> & scale(T f);

        /** 取之间元素的最小
        @version NIIEngine 3.0.0
        */
        Vector3<T> floor(const Vector3<T> & o) const;

        /** 取之间元素的最大
        @version NIIEngine 3.0.0
        */
        Vector3<T> ceil(const Vector3<T> & o) const;

        /**生成一个向量垂直于这个向量
        @version NIIEngine 3.0.0
        */
        Vector3<T> vertical() const;

        /** 生成一个相应弧度内的随机向量
        @param[in] angle 偏离弧度
        @return 向量,没有单位化的
        @version NIIEngine 3.0.0
        */
        Vector3<T> random(const Radian & angle) const;

        /** 获取两个向量之间的夹角
        @remark 向量没有被单位长度话但他们必须描述的是方向
        @version NIIEngine 3.0.0
        */
        Radian angle(const Vector3<T> & o) const;

        /** 给定法线计算出的平面反射的向量
        @remark 注 假设方法调用者是一个点而且远离这个平面,如果不是只把它颠倒过来
        @version NIIEngine 3.0.0
        */
        Vector3<T> reflect(const Vector3<T> & normal) const;

        /** 是否相等
        @remark 浮点型向量在像素概念中很可能会是有相同的结果
        @param[in] o 另一个向量
        @param[in] deviation 误差范围
        */
        bool equal(const Vector3<T> & o, NIIf deviation = 0.001) const;

        /** 法向量
        @param[in] left
        @param[in] down
        @version NIIEngine 3.0.0
        */
        void NormalVectors(Vector3<T> & left, Vector3<T> & down);

        /** 正交函数
        @param[in] left
        @param[in] up
        @version NIIEngine 3.0.0
        */
        void OrthogonalBasis(Vector3<T> & left, Vector3<T> & up);

        /** 投射到一个平面
        @remark 参数1,2描述一个平面
        @param[in] normal 面法线
        @param[in] c 面常量
        @version NIIEngine 3.0.0
        */
        void ProjectOntoPlane(const Vector3<T> & normal, NIIf c);

        /** 沿一个平面投射
        @remark 参数1,2描述一个平面
        @param[in] normal 面法线
        @param[in] c 面常量
        @version NIIEngine 3.0.0
        */
        bool ProjectAlongPlane(const Vector3<T> & normal, NIIf c, NIIf epsilon);

        /// 指定的静态常量
        static const Vector3<T> ZERO;
        static const Vector3<T> UNIT;
        static const Vector3<T> X;
        static const Vector3<T> Y;
        static const Vector3<T> Z;
        static const Vector3<T> INV_X;
        static const Vector3<T> INV_Y;
        static const Vector3<T> INV_Z;

        typedef Vector3<T>          value_type;
        typedef value_type &        ref;
        typedef const value_type &  const_ref;
        typedef value_type *        pointer;
        typedef const value_type *  const_pointer;
        typedef Nidx              index_type;
    public:
        T x, y, z;
    };
    //impl
    //------------------------------------------------------------------------
    template <typename T> const Vector3<T> Vector3<T>::ZERO = Vector3<T>(0, 0, 0);
    template <typename T> const Vector3<T> Vector3<T>::UNIT = Vector3<T>(1, 1, 1);
    template <typename T> const Vector3<T> Vector3<T>::X    = Vector3<T>(1, 0, 0);
    template <typename T> const Vector3<T> Vector3<T>::Y    = Vector3<T>(0, 1, 0);
    template <typename T> const Vector3<T> Vector3<T>::Z    = Vector3<T>(0, 0, 1);
    template <typename T> const Vector3<T> Vector3<T>::INV_X = Vector3<T>(-1, 0, 0);
    template <typename T> const Vector3<T> Vector3<T>::INV_Y = Vector3<T>(0, -1, 0);
    template <typename T> const Vector3<T> Vector3<T>::INV_Z = Vector3<T>(0, 0, -1);
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T>::Vector3(){}
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T>::Vector3(const T & t) : x(t), y(t), z(t){}
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T>::Vector3(const T & _x, const T & _y, const T & _z) : x(_x), y(_y), z(_z){}
    //-------------------------------------------------------------------------
    template <typename T> inline Vector3<T>::Vector3(const Vector3 & o) : x(o.x), y(o.y), z(o.z){}
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
    //-------------------------------------------------------------------------
    template <typename T> inline T Vector3<T>::volume() const
    {
        return x * y * z;
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

        if (!zeroDot[0] && !zeroDot[1] && Math::Sign(dot[0]) != Math::Sign(dot[1]))
        {
            return false;
        }

        v1 = a - c;
        v2 = this - c;

        dot[2] = v1.crossProduct(v2).dotProduct(normal);
        zeroDot[2] = Math::isEqual(dot[2], 0.0f, 1e-3);

        if ((!zeroDot[0] && !zeroDot[2] && Math::Sign(dot[0]) != Math::Sign(dot[2])) ||
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
        if (!scale)
        {
            x = 0;
            y = 0;
            z = 0;
        }
        else
        {
            lenS = lengthSquared();
            if (lenS > scale * scale)
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
        if (o.x < x)
            temp.x = o.x;
        else
            temp.x = x;

        if (o.y < y)
            temp.y = o.y;
        else
            temp.y = y;

        if (o.z < z)
            temp.z = o.z;
        else
            temp.z = z;

        return temp;
    }
    //-------------------------------------------------------------------------
    template <typename T> Vector3<T> Vector3<T>::ceil(const Vector3<T> & o) const
    {
        Vector3<T> temp;
        if (o.x > x)
            temp.x = o.x;
        else
            temp.x = x;

        if (o.y > y)
            temp.y = o.y;
        else
            temp.y = y;

        if (o.z > z)
            temp.z = o.z;
        else
            temp.z = z;

        return temp;
    }
    //-------------------------------------------------------------------------
    template <typename T> Vector3<T> Vector3<T>::vertical() const
    {
        Vector3<T> first = this->crossProduct(Vector3<T>::X);

        if (first.lengthSquared() < 0.000000000001)
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
        if (!d)
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

        if (Math::Fabs(z) > 0.7f)
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

        if (c != 1.0)
        {
            if (backoff < 0)
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
        len = dotProduct(normal, cross);

        if (Math::Fabs(len) < epsilon)
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
    template <typename T> inline bool Vector3<T>::operator <= (const Vector3<T> & o) const
    {
        return (x <= o.x && y <= o.y && z <= o.z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline bool Vector3<T>::operator > (const Vector3<T> & o) const
    {
        return (x > o.x && y > o.y && z > o.z);
    }
    //-------------------------------------------------------------------------
    template <typename T> inline bool Vector3<T>::operator >= (const Vector3<T> & o) const
    {
        return (x >= o.x && y >= o.y && z >= o.z);
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
        if (fabs(o.x - x + o.y - y + z - o.z) < deviation)
            return true;
        return false;
    }
    //-------------------------------------------------------------------------

    typedef Vector3<NIIi> Vector3i;
    typedef Vector3<Nui>  Vector3ui;
    typedef Vector3<NIIf> Vector3f;
    typedef Vector3<NIIf> vector3;
    typedef vector<Vector3f>::type Vector3List;

    template<typename T> Nostream & operator << (Nostream & o, const Vector3<T> & v)
    {
        o << "Vector3(" << v.x << "," << v.y << "," << v.z << ")";
        return o;
    }

}
#endif