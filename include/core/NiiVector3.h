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

        bool operator > (const Vector3<T> & o) const;

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
    #include "NiiVector3.inl"

    typedef Vector3<NIIi> Vector3i;
    typedef Vector3<NIIf> Vector3f;
    typedef Vector3<NIIf> vector3;
    typedef vector<Vector3f>::type Vector3List;

    template<typename T> std::ostream & operator << (std::ostream & o, const Vector3<T> & v)
    {
        o << "Vector3(" << v.x << "," << v.y << "," << v.z << ")";
        return o;
    }

}
#endif