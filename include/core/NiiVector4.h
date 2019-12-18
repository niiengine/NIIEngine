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

#ifndef _NII_Vector4_H_
#define _NII_Vector4_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"

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
    #include "NiiVector4.inl"

    typedef Vector4<NIIi>   Vector4i;
    typedef Vector4<NIIf>   Vector4f;
    typedef Vector4f        vector4;

    template<typename T> std::ostream & operator << (std::ostream & o, const Vector4<T> & v)
    {
        o << "Vector4(" << v.x << "," << v.y << "," << v.z << "," << v.w << ")";
        return o;
    }
}
#endif