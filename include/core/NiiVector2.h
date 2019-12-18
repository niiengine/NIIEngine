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

#ifndef _NII_VECTOR2_H_
#define _NII_VECTOR2_H_

#include "NiiPreInclude.h"
#include "NiiMath.h"
#include "NiiPlaneSize.h"

namespace NII
{
    /** 二维向量
    @remark 可以用来表示二维位置,二维方向和二维缩放
    @version NIIEngine 3.0.0
    */
    template <typename T> class Vector2
    {
    public:
        Vector2();

        Vector2(const T & t);

        Vector2(const T & x, const T & y);

        Vector2(const Vector2<T> & o);

        Vector2<T> & operator =(const Vector2<T> & o);

        Vector2<T> & operator =(const T & t);

        Vector2<T> operator +(const Vector2<T> & o) const;

        Vector2<T> operator -(const Vector2<T> & o) const;

        Vector2<T> operator *(const Vector2<T> & o) const;

        Vector2<T> operator /(const Vector2<T> & o) const;

        Vector2<T> operator +(const T & t) const;

        Vector2<T> operator -(const T & t) const;

        Vector2<T> operator *(const T & t) const;

        Vector2<T> operator /(const T & t) const;

        Vector2<T> operator +() const;

        Vector2<T> operator -() const;

		friend Vector2<T> operator +(const T & t, const Vector2<T> & o)
		{
			return Vector2<T>(t + o.x, t + o.y);
		}

		friend Vector2<T> operator -(const T & t, const Vector2<T> & o)
		{
			return Vector2<T>(t - o.x, t - o.y);
		}

		friend Vector2<T> operator *(const T & t, const Vector2<T> & o)
		{
			return Vector2<T>(t * o.x, t * o.y);
		}

		friend Vector2<T> operator /(const T & t, const Vector2<T> & o)
		{
			return Vector2<T>(o.x == 0 ? t : t / o.x, o.y == 0 ? t : t / o.y);
		}

        bool operator ==(const Vector2<T> & o) const;

        bool operator !=(const Vector2<T> & o) const;

        bool operator < (const Vector2<T> & o) const;

        bool operator > (const Vector2<T> & o) const;

        Vector2<T> & operator +=(const Vector2<T> & o);

        Vector2<T> & operator -=(const Vector2<T> & o);

        Vector2<T> & operator *=(const Vector2<T> & o);

        Vector2<T> & operator /=(const Vector2<T> & o);

        Vector2<T> & operator +=(const T & t);

        Vector2<T> & operator -=(const T & t);

        Vector2<T> & operator *=(const T & t);

        Vector2<T> & operator /=(const T & t);

		/** 获取数据指针
		@version NIIEngine 3.0.0
		*/
		T * data();

        /** 获取数据指针
        @version NIIEngine 3.0.0
        */
        const T * data() const;

        /** 返回向量长度大小
        @note
            返回方根,浮点操作对线程有压力,如仅比较操作使用 lengthSquared()
        @version NIIEngine 3.0.0
        */
        T length() const;

        /** length()的快速算法,快但不准确
        @version NIIEngine 3.0.0
        */
        T lengthFast() const;

        /** 返回向量长度大小的2次方
        @remark
            这个方法具体效果比 length () 方法差一步 但能换来效率的优势如果你只是想在
            它们中找到更大(长)或更小(短)你可以用这个方法代替
        @version NIIEngine 3.0.0
        */
        T lengthSquared() const;

        /** 返回与另一个向量之间的距离
        @note
            这个方法需要用到具体长度的方法,会带来CPU效率问题,如果你不需要具体的距离
            问题可以使用squaredDistance()代替
        @version NIIEngine 3.0.0
        */
        T distance(const Vector2<T> & o) const;

        /** 返回与另一个向量的距离的2次方
        @remark
            这个方法具体效果比 distance () 方法差一步 但能换来效率的优势如果你只是
            想在它们中找到距离更大(长)或更小(短)你可以用这个方法代替
        */
        T distanceSquared(const Vector2<T> & o) const;

        /** 计算一个向量始点与另一个向量始点之间构成的新向量(如 力) 点积
        @remark
            点积可以用来计算两个向量之间的夹角,只有两个同时是单位向量 其结果才是关于
            余弦的夹角；否则使用必须这两个向量的长度才能求出关于余弦的夹角,这个结果可
            以用来计算到一个面上某点的距离
        @param[in] o 需要一个向量和本方法的发出向量
        @return 一个描述点积的浮点类型值
        @version NIIEngine 3.0.0
        */
        T dotProduct(const Vector2<T> & o) const;

        /** 计算2个向量相交产生的二维面积 该结果是一个单一的浮点值大小是该向量组成三角
            形面积的2倍(叉积)
        @version NIIEngine 3.0.0
        */
        T crossProduct(const Vector2<T> & o) const;

        /** 是否在三角形里
        @version NIIEngine 3.0.0
        */
        bool isInTriangle(const Vector2<T> & a, const Vector2<T> & b, const Vector2<T> & c);

        /** 单位化向量
        @remark 单位化,数据范围[0,1]
        @note 这个方法不能转变0数量的向量,不过不会改变它们的内容
        @return 一个顶点首要的长度
        @version NIIEngine 3.0.0
        */
        T normalise();

        /** normalise()的快速算法,快但不准确
        @remark Lod 动态策略中有帮助
        @version NIIEngine 3.0.0
        */
        T normaliseFast();

        /** normalise 副本返回
        @version NIIEngine 3.0.0
        */
        void normalise(Vector2<T> & out) const;

        /** 与另一个向量的夹角
        @param[in] o
        @version NIIEngine 3.0.0
        */
        T angle(const Vector2<T> & o) const;

        /** 返回向量之间的角度(弧度)
        @remark 角度 / 360 = 弧度 / 6.2832~
        @version NIIEngine 3.0.0
        */
        Radian angle2(const Vector2<T> & o) const;

        /** 生成一个相应弧度内的随机向量
        @param[in] angle 偏离弧度
        @return 向量,没有单位化的
        @version NIIEngine 3.0.0
        */
        Vector2<T> random(T angle) const;

        /** 返回两顶点距离的中间数
        @version NIIEngine 3.0.0
        */
        Vector2<T> middle(const Vector2<T> & o) const;

        /** 取最小
        @remark
        */
        Vector2<T> floor(const Vector2<T> & o);

        /** 取最大
        @version NIIEngine 3.0.0
        */
        Vector2<T> ceil(const Vector2<T> & o);

        /** 给定法线计算出的平面反射的向量
        @version NIIEngine 3.0.0
        */
        Vector2<T> reflect(const Vector2<T> & normal) const;

        /** 缩放向量
        @version NIIEngine 3.0.0
        */
        Vector2<T> scale(NIIf f);

        /**返回一个垂直于这向量的向量
        @remark 返回一个垂直于这向量的向量,仅是方向
        @version NIIEngine 3.0.0
        */
        Vector2<T> vertical() const;

        static const Vector2<T> ZERO;
        static const Vector2<T> UNIT;
        static const Vector2<T> X;
        static const Vector2<T> Y;
        static const Vector2<T> INV_X;
        static const Vector2<T> INV_Y;

        typedef Vector2<T>          value_type;
        typedef value_type &        ref;
        typedef const value_type &  const_ref;
        typedef value_type *        pointer;
        typedef const value_type *  const_pointer;
        typedef Nidx              index_type;
    public:
        T x, y;
    };

    typedef Vector2<NIIi> Vector2i;
    typedef Vector2<NIIf> Vector2f;
    typedef Vector2f      vector2;

    template<typename T> std::ostream & operator << (std::ostream & o, const Vector2<T> & v)
    {
        o << "Vector2(" << v.x << "," << v.y << ")";
        return o;
    }
#include "NiiVector2.inl"
}
#endif