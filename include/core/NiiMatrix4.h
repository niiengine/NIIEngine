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

#ifndef _NII_MATRIX4_H_
#define _NII_MATRIX4_H_

#include "NiiPreInclude.h"
#include "NiiMatrix3.h"
#include "NiiVector4.h"
#include "NiiQuaternion.h"

namespace NII
{
    /** 标准的4X4矩阵
    @remark
        使用数学结构的标准,自动处理各种图形引擎之间的差异,如OpenGl和Direct3D中的
        一些存储和乘法顺序不同,使用右手坐标系.
    @note
        标准数学中使用行优先存储和列乘行的方式,OpenGl和Direct3D各有所不同,OpenGl
        使用列优先存储,Direct3D使用行乘列.所以均要做转换.NIIEngine自动完成它们和
        标准数学的差异,所以使用NIIEngine时只要记住一点就是使用标准数学思想处理问题.
        标准数学 M*V 的显示布局的矩阵如下:
        <pre>
        {[0][0] [0][1] [0][2] [0][3]} {x}
        |[1][0] [1][1] [1][2] [1][3]|*{y}
        |[2][0] [2][1] [2][2] [2][3]| {z}
        {[3][0] [3][1] [3][2] [3][3]} {w}
        </pre>
    @note 使用 行优先存储 和 列乘行 和 右手坐标系 方式, 仿射变换也就是变换中的一个
        特殊优化效率的例子,如果使用普通的变换结果是相同的,但就是慢点.除非应用在特
        效凹凸镜面或初始化视口等需要用到投影级距阵.视图级后的距阵基本是仿射距阵
    @note 如果是用于投影设置的距阵,opengl 深度范围是[-1, 1] 而 d3d 则是 [-1, 0]
    @version NIIEngine 3.0.0
    */
    template <typename TN> class Matrix4
    {
    public:
        Matrix4();

        Matrix4(TN _00, TN _01, TN _02, TN _03, TN _10, TN _11, TN _12, TN _13,
            TN _20, TN _21, TN _22, TN _23, TN _30, TN _31, TN _32, TN _33);

        /// 旋转/缩放3x3矩阵,无平移构建
        Matrix4(const Matrix3<TN> & v);

        /// 旋转/缩放四元组,无平移构建
        Matrix4(const Quaternion & r);

        /** 交换内容
        @version NIIEngine 3.0.0
        */
        void swap(Matrix4<TN> & o);

        TN * operator [](NCount row);

        const TN * operator [](NCount row) const;

        /** 级联
        @version NIIEngine 3.0.0
        */
        Matrix4<TN> concatenate(const Matrix4<TN> & o) const;

        /** 前乘
        @version NIIEngine 3.0.0
        */
        Matrix4<TN> operator * (const Matrix4<TN> & o) const;

        /** 矢量变换
        @version NIIEngine 3.0.0
        */
        Vector3<TN> operator * (const Vector3<TN> & v) const;

        /** 矩阵与向量乘
        @version NIIEngine 3.0.0
        */
        Vector4<TN> operator * (const Vector4<TN> & v) const;

        /** 向量与矩阵乘
        @version NIIEngine 3.0.0
        */
		friend Vector4<TN> operator * (const Vector4<TN> & v, const Matrix4<TN> & mat)
		{
			return Vector4<TN>(
				v.x * mat[0][0] + v.y * mat[1][0] + v.z * mat[2][0] + v.w * mat[3][0],
				v.x * mat[0][1] + v.y * mat[1][1] + v.z * mat[2][1] + v.w * mat[3][1],
				v.x * mat[0][2] + v.y * mat[1][2] + v.z * mat[2][2] + v.w * mat[3][2],
				v.x * mat[0][3] + v.y * mat[1][3] + v.z * mat[2][3] + v.w * mat[3][3]);
		}

        /** 矩阵 * 缩放
        @version NIIEngine 3.0.0
        */
        Matrix4<TN> operator *(const TN & s) const;

        /// 矩阵加法
        Matrix4<TN> operator + (const Matrix4<TN> & o) const;

        /// 矩阵的减法
        Matrix4<TN> operator - (const Matrix4<TN> & o) const;

        /// 相等
        bool operator == (const Matrix4<TN> & o) const;

        /// 不相等
        bool operator != (const Matrix4<TN> & o) const;

        /// 从矩阵中分配
        void operator = (const Matrix3<TN> & o);

        /** 连接两个仿射矩阵
        @remark 意义和 concatenate 相同
        @return 变换结果
        @note 两者必须是仿射矩阵
        @version NIIEngine 3.0.0
        */
        Matrix4<TN> affine(const Matrix4<TN> & o) const;

        /** 仿射矩阵专用的三维向量变换
        @remark 意义和 operator *(const Vector3<TN> &) 相同
        @return 变换结果
        @note 必须是仿射矩阵
        @version NIIEngine 3.0.0
        */
        Vector3<TN> affine(const Vector3<TN> & v) const;

        /** 专门为仿射矩阵的四维向量变换
        @remark 意义和 operator *(const Vector4<TN> &) 相同
        @return 变换结果
        @note 必须是仿射矩阵
        @version NIIEngine 3.0.0
        */
        Vector4<TN> affine(const Vector4<TN> & v) const;

        /** 转置矩阵 (*this)^T
        @version NIIEngine 3.0.0
        */
        Matrix4<TN> T() const;

        /** 伴随矩阵
        @remark 标准数学
        @version NIIEngine 3.0.0
        */
        Matrix4<TN> adjoint() const;

        /** 行列式
        @remark 标准数学
        @version NIIEngine 3.0.0
        */
        TN det() const;

        /** 逆矩阵
        @remark 标准数学
        @version NIIEngine 3.0.0
        */
        Matrix4<TN> inverse() const;

        /** 逆矩阵(仿射变换)
        @note 必须是仿射矩阵,结果和 inverse 相同,仅仅是比较快点
        @version NIIEngine 3.0.0
        */
        Matrix4<TN> inverseAffine() const;

        /** 优化算法
        @remark
            out = (out * o2)^T 标准数学,这个方法很合适opengl,d3d使用
        @version NIIEngine 3.0.0
        */
        static void T(TN * out, const Matrix4<TN> & o2);

        /** 优化算法
        @remark 先转置后再乘一个矩阵 *this = *this^T * o
        @version NIIEngine 3.0.0
        */
        const Matrix4<TN> & Tmul(const Matrix4<TN> & o) const;

        /** 优化算法
        @remark 乘一个矩阵的转置 *this = *this * o^T
        @version NIIEngine 3.0.0
        */
        const Matrix4<TN> & mulT(const Matrix4<TN> & o) const;

        /** 优化算法
        @remark 先转置后再乘一个矩阵的转置 *this = *this^T * o^T
        @version NIIEngine 3.0.0
        */
        const Matrix4<TN> & TmulT(const Matrix4<TN> & o) const;

        /** 设置平移部分
        @version NIIEngine 3.0.0
        */
        void setTrans(const Vector3<TN> & v);

        /** 获取平移部分
        @version NIIEngine 3.0.0
        */
        Vector3<TN> getTrans() const;

        /** 设置缩放部分
        @version NIIEngine 3.0.0
        */
        void setScale(const Vector3<TN> & v);

        /** 获取缩放部分
        @version NIIEngine 3.0.0
        */
        Vector3<TN> getScale() const;

        /** 合成仿射矩阵
        @param[out] p 位置
        @param[out] s 缩放
        @param[out] o 方向
        @version NIIEngine 3.0.0
        */
        void set(const Vector3<TN> & p, const Vector3<TN> & s, const Quaternion & o);

        /** 分解仿射矩阵
        @param[out] p 位置
        @param[out] s 缩放
        @param[out] o 方向
        @version NIIEngine 3.0.0
        */
        void get(Vector3<TN> & p, Vector3<TN> & s, Quaternion & o) const;

        /** 是否涉及缩放
        @version NIIEngine 3.0.0
        */
        bool isScale() const;

        /** 是否涉及反缩放
        @version NIIEngine 3.0.0
        */
        bool isNegativeScale() const;

        /** 否是否仿射矩阵.
        @remark 没有投影的矩阵
        @version NIIEngine 3.0.0
        */
        bool isAffine() const;

        /** 构建
        @param[in] p 位置
        @param[in] s 缩放
        @param[in] o 旋转
        @version NIIEngine 3.0.0
        */
        void makeInverseTransform(const Vector3<TN> & p, const Vector3<TN> & s,
            const Quaternion & o);

        /** 构建
        @param[in] n 平面的单位法线
        @param[in] o 在平面上的点
        @param[in] dir 方向
        @version NIIEngine 3.0.0
        */
        void makeObliqueProjection(const Vector3f & n, const Vector3f & o,
            const Vector3f & dir);

        /** 构建
        @param[in] n 平面的单位法线
        @param[in] o 在平面上的点
        @param[in] eye 视点
        @version NIIEngine 3.0.0
        */
        void makePerspectiveProjection(const Vector3f & n, const Vector3f & o,
            const Vector3f & eye);

        /** 构建
        @remark 平面反射矩阵
        @param[in] n 平面的单位法线
        @param[in] o 在平面上的点
        @version NIIEngine 3.0.0
        */
        void makeReflection(const Vector3f & n, const Vector3f & o);

        static const Matrix4<TN> ZERO;
        static const Matrix4<TN> AFFINE;
        static const Matrix4<TN> IDENTITY;
    public:
        union
        {
            TN m[4][4];
            TN _m[16];
        };
    };

    typedef Matrix4<NIIi>   Matrix4i;
    typedef Matrix4<NIIf>   Matrix4f;
    typedef Matrix4<NIId>   Matrix4d;
    typedef Matrix4f		matrix4;

    /** 平移矩阵
    @version NIIEngine 3.0.0
    */
    class TranslationMatrix4 : public Matrix4f
    {
    public:
        TranslationMatrix4(const Vector3f & oft);

        TranslationMatrix4(NIIf oftx, NIIf ofty, NIIf oftz);
    };

    /** 缩放矩阵
    @version NIIEngine 3.0.0
    */
    class ScaleMatrix4 : public Matrix4f
    {
    public:
        ScaleMatrix4(const Vector3f & v);

        ScaleMatrix4(NIIf x, NIIf y, NIIf z);
    };

    /** 旋转矩阵
    @version NIIEngine 3.0.0
    */
    class RotationMatrix4 : public Matrix4f
    {
    public:
        /** 通过3个量获得一个旋转矩针
        @param[in] pitch Looking up and down (0=Straight Ahead, +Up, -Down).
        @param[in] yaw Rotating around (running in circles), 0=East, +North, -South.
        @param[in] roll Rotation about axis of screen, 0=Straight, +Clockwise, -CCW.
        */
        RotationMatrix4(NIIf pitch, NIIf yaw, NIIf roll);
    };

    /** 透视投影矩阵
    @version NIIEngine 3.0.0
    */
    class PerspectiveMatrix4 : public Matrix4f
    {
    public:
        PerspectiveMatrix4(NIIf fovx, NIIf fovy, NIIf mfovyx, NIIf mfovy , NIIf minz, NIIf maxz);

        PerspectiveMatrix4(NIIf FOV, NIIf width, NIIf height, NIIf minz, NIIf maxz);
    };

    /** 正交矩阵
    @version NIIEngine 3.0.0
    */
    class OrthoMatrix4 : public Matrix4f
    {
    public:
        OrthoMatrix4(NIIf width, NIIf height, NIIf zscale, NIIf zoffset);
    };

    /** 逆翻转矩阵
    @version NIIEngine 3.0.0
    */
    class InverseRotationMatrix4 : public Matrix4f
    {
    public:
        /** 通过3个量获得一个逆旋转矩针
        @param[in] pitch Looking up and down (0=Straight Ahead, +Up, -Down).
        @param[in] yaw Rotating around (running in circles), 0=East, +North, -South.
        @param[in] roll Rotation about axis of screen, 0=Straight, +Clockwise, -CCW.
        */
        InverseRotationMatrix4(NIIf pitch , NIIf yaw , NIIf roll);
    };

    template <typename TN> std::ostream & operator << (std::ostream & o, const Matrix4<TN> & m)
    {
        o << "Matrix4<TN>(";
        for (NCount i = 0; i < 4; ++i)
        {
            o << " row" << (unsigned)i << "{";
            for (NCount j = 0; j < 4; ++j)
            {
                o << m[i][j] << ",";
            }
            o << "}";
        }
        o << ")";
        return o;
    }
#include "NiiMatrix4.inl"
}
#endif