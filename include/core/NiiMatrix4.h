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
        explicit Matrix4(const TN * data) { memcpy(_m, data, 16 * sizeof(TN)); }
        Matrix4(TN _00, TN _01, TN _02, TN _03, TN _10, TN _11, TN _12, TN _13,
            TN _20, TN _21, TN _22, TN _23, TN _30, TN _31, TN _32, TN _33);

        /// 旋转/缩放3x3矩阵,无平移构建
        explicit Matrix4(const Matrix3<TN> & v);

        /// 旋转/缩放四元组,无平移构建
        explicit Matrix4(const Quaternion & r);

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
        @note 两者必须是仿射矩阵
        @version NIIEngine 3.0.0
        */
        Matrix4<TN> affine(const Matrix4<TN> & o) const;

        /** 仿射矩阵专用的三维向量变换
        @remark 意义和 operator *(const Vector3<TN> &) 相同
        @note 必须是仿射矩阵
        @version NIIEngine 3.0.0
        */
        Vector3<TN> affine(const Vector3<TN> & v) const;

        /** 专门为仿射矩阵的四维向量变换
        @remark 意义和 operator *(const Vector4<TN> &) 相同
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
    
    //----------------------------------------------------------------------------
    template <typename TN> const Matrix4<TN> Matrix4<TN>::ZERO =
        Matrix4<TN>(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    //----------------------------------------------------------------------------
    template <typename TN> const Matrix4<TN> Matrix4<TN>::AFFINE =
        Matrix4<TN>(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
    //----------------------------------------------------------------------------
    template <typename TN> const Matrix4<TN> Matrix4<TN>::IDENTITY =
        Matrix4<TN>(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    //----------------------------------------------------------------------------
    template <typename TN> Matrix4<TN>::Matrix4()
    {
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline Matrix4<TN>::Matrix4(
        TN _00, TN _01, TN _02, TN _03, TN _10, TN _11, TN _12, TN _13,
        TN _20, TN _21, TN _22, TN _23, TN _30, TN _31, TN _32, TN _33)
    {
        m[0][0] = _00; m[0][1] = _01; m[0][2] = _02; m[0][3] = _03;
        m[1][0] = _10; m[1][1] = _11; m[1][2] = _12; m[1][3] = _13;
        m[2][0] = _20; m[2][1] = _21; m[2][2] = _22; m[2][3] = _23;
        m[3][0] = _30; m[3][1] = _31; m[3][2] = _32; m[3][3] = _33;
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline Matrix4<TN>::Matrix4(const Matrix3<TN> & v)
    {
        operator=(IDENTITY);
        operator=(v);
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline Matrix4<TN>::Matrix4(const Quaternion & q)
    {
		NIIf fTx = 2.0f * q.x;
		NIIf fTy = 2.0f * q.y;
		NIIf fTz = 2.0f * q.z;
		NIIf fTwx = fTx * q.w;
		NIIf fTwy = fTy * q.w;
		NIIf fTwz = fTz * q.w;
		NIIf fTxx = fTx * q.x;
		NIIf fTxy = fTy * q.x;
		NIIf fTxz = fTz * q.x;
		NIIf fTyy = fTy * q.y;
		NIIf fTyz = fTz * q.y;
		NIIf fTzz = fTz * q.z;

		m[0][0] = 1.0f - (fTyy + fTzz);
		m[0][1] = fTxy - fTwz;
		m[0][2] = fTxz + fTwy;
		m[0][3] = 0;
		m[1][0] = fTxy + fTwz;
		m[1][1] = 1.0f - (fTxx + fTzz);
		m[1][2] = fTyz - fTwx;
		m[1][3] = 0;
		m[2][0] = fTxz - fTwy;
		m[2][1] = fTyz + fTwx;
		m[2][2] = 1.0f - (fTxx + fTyy);
		m[2][3] = 0;
		m[3][0] = 0;
		m[3][1] = 0;
		m[3][2] = 0;
		m[3][3] = 1;
    }
    //----------------------------------------------------------------------------
    template <typename TN> void Matrix4<TN>::swap(Matrix4<TN> & o)
    {
        std::swap(m[0][0], o.m[0][0]);std::swap(m[0][1], o.m[0][1]);
        std::swap(m[0][2], o.m[0][2]);std::swap(m[0][3], o.m[0][3]);
        std::swap(m[1][0], o.m[1][0]);std::swap(m[1][1], o.m[1][1]);
        std::swap(m[1][2], o.m[1][2]);std::swap(m[1][3], o.m[1][3]);
        std::swap(m[2][0], o.m[2][0]);std::swap(m[2][1], o.m[2][1]);
        std::swap(m[2][2], o.m[2][2]);std::swap(m[2][3], o.m[2][3]);
        std::swap(m[3][0], o.m[3][0]);std::swap(m[3][1], o.m[3][1]);
        std::swap(m[3][2], o.m[3][2]);std::swap(m[3][3], o.m[3][3]);
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline TN * Matrix4<TN>::operator [](NCount row)
    {
        assert(row < 4);
        return m[row];
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline const TN * Matrix4<TN>::operator [](NCount row) const
    {
        assert(row < 4);
        return m[row];
    }
    //----------------------------------------------------------------------------
    template <typename TN> Matrix4<TN> Matrix4<TN>::concatenate(const Matrix4<TN> & o) const
    {
        Matrix4<TN> r;
        r.m[0][0] = m[0][0] * o.m[0][0] + m[0][1] * o.m[1][0] + m[0][2] * o.m[2][0] + m[0][3] * o.m[3][0];
        r.m[0][1] = m[0][0] * o.m[0][1] + m[0][1] * o.m[1][1] + m[0][2] * o.m[2][1] + m[0][3] * o.m[3][1];
        r.m[0][2] = m[0][0] * o.m[0][2] + m[0][1] * o.m[1][2] + m[0][2] * o.m[2][2] + m[0][3] * o.m[3][2];
        r.m[0][3] = m[0][0] * o.m[0][3] + m[0][1] * o.m[1][3] + m[0][2] * o.m[2][3] + m[0][3] * o.m[3][3];

        r.m[1][0] = m[1][0] * o.m[0][0] + m[1][1] * o.m[1][0] + m[1][2] * o.m[2][0] + m[1][3] * o.m[3][0];
        r.m[1][1] = m[1][0] * o.m[0][1] + m[1][1] * o.m[1][1] + m[1][2] * o.m[2][1] + m[1][3] * o.m[3][1];
        r.m[1][2] = m[1][0] * o.m[0][2] + m[1][1] * o.m[1][2] + m[1][2] * o.m[2][2] + m[1][3] * o.m[3][2];
        r.m[1][3] = m[1][0] * o.m[0][3] + m[1][1] * o.m[1][3] + m[1][2] * o.m[2][3] + m[1][3] * o.m[3][3];

        r.m[2][0] = m[2][0] * o.m[0][0] + m[2][1] * o.m[1][0] + m[2][2] * o.m[2][0] + m[2][3] * o.m[3][0];
        r.m[2][1] = m[2][0] * o.m[0][1] + m[2][1] * o.m[1][1] + m[2][2] * o.m[2][1] + m[2][3] * o.m[3][1];
        r.m[2][2] = m[2][0] * o.m[0][2] + m[2][1] * o.m[1][2] + m[2][2] * o.m[2][2] + m[2][3] * o.m[3][2];
        r.m[2][3] = m[2][0] * o.m[0][3] + m[2][1] * o.m[1][3] + m[2][2] * o.m[2][3] + m[2][3] * o.m[3][3];

        r.m[3][0] = m[3][0] * o.m[0][0] + m[3][1] * o.m[1][0] + m[3][2] * o.m[2][0] + m[3][3] * o.m[3][0];
        r.m[3][1] = m[3][0] * o.m[0][1] + m[3][1] * o.m[1][1] + m[3][2] * o.m[2][1] + m[3][3] * o.m[3][1];
        r.m[3][2] = m[3][0] * o.m[0][2] + m[3][1] * o.m[1][2] + m[3][2] * o.m[2][2] + m[3][3] * o.m[3][2];
        r.m[3][3] = m[3][0] * o.m[0][3] + m[3][1] * o.m[1][3] + m[3][2] * o.m[2][3] + m[3][3] * o.m[3][3];
        return r;
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline Matrix4<TN> Matrix4<TN>::operator *(const Matrix4<TN> & o) const
    {
        return concatenate(o);
    }
    //----------------------------------------------------------------------------
    template <typename TN> Vector3<TN> Matrix4<TN>::operator *(const Vector3<TN> & v) const
    {
        Vector3<TN> r;

        TN invW = 1.0f / (m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3]);
        r.x = (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) * invW;
        r.y = (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) * invW;
        r.z = (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) * invW;

        return r;
    }
    //----------------------------------------------------------------------------
    template <typename TN> Vector4<TN> Matrix4<TN>::operator *(const Vector4<TN> & v) const
    {
        return Vector4<TN>(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w);
    }/*
    //----------------------------------------------------------------------------
    template <typename TN> Vector4<TN> operator *(const Vector4<TN> & v, const Matrix4<TN> & mat)
    {
        return Vector4<TN>(
            v.x * mat[0][0] + v.y * mat[1][0] + v.z * mat[2][0] + v.w * mat[3][0],
            v.x * mat[0][1] + v.y * mat[1][1] + v.z * mat[2][1] + v.w * mat[3][1],
            v.x * mat[0][2] + v.y * mat[1][2] + v.z * mat[2][2] + v.w * mat[3][2],
            v.x * mat[0][3] + v.y * mat[1][3] + v.z * mat[2][3] + v.w * mat[3][3] );
    }*/
    //----------------------------------------------------------------------------
    template <typename TN> Matrix4<TN> Matrix4<TN>::operator +(const Matrix4<TN> & o) const
    {
        Matrix4<TN> r;
        r.m[0][0] = m[0][0] + o.m[0][0];r.m[0][1] = m[0][1] + o.m[0][1];
        r.m[0][2] = m[0][2] + o.m[0][2];r.m[0][3] = m[0][3] + o.m[0][3];

        r.m[1][0] = m[1][0] + o.m[1][0];r.m[1][1] = m[1][1] + o.m[1][1];
        r.m[1][2] = m[1][2] + o.m[1][2];r.m[1][3] = m[1][3] + o.m[1][3];

        r.m[2][0] = m[2][0] + o.m[2][0];r.m[2][1] = m[2][1] + o.m[2][1];
        r.m[2][2] = m[2][2] + o.m[2][2];r.m[2][3] = m[2][3] + o.m[2][3];

        r.m[3][0] = m[3][0] + o.m[3][0];r.m[3][1] = m[3][1] + o.m[3][1];
        r.m[3][2] = m[3][2] + o.m[3][2];r.m[3][3] = m[3][3] + o.m[3][3];
        return r;
    }
    //----------------------------------------------------------------------------
    template <typename TN> Matrix4<TN> Matrix4<TN>::operator -(const Matrix4<TN> & o) const
    {
        Matrix4<TN> r;
        r.m[0][0] = m[0][0] - o.m[0][0];r.m[0][1] = m[0][1] - o.m[0][1];
        r.m[0][2] = m[0][2] - o.m[0][2];r.m[0][3] = m[0][3] - o.m[0][3];

        r.m[1][0] = m[1][0] - o.m[1][0];r.m[1][1] = m[1][1] - o.m[1][1];
        r.m[1][2] = m[1][2] - o.m[1][2];r.m[1][3] = m[1][3] - o.m[1][3];

        r.m[2][0] = m[2][0] - o.m[2][0];r.m[2][1] = m[2][1] - o.m[2][1];
        r.m[2][2] = m[2][2] - o.m[2][2];r.m[2][3] = m[2][3] - o.m[2][3];

        r.m[3][0] = m[3][0] - o.m[3][0];r.m[3][1] = m[3][1] - o.m[3][1];
        r.m[3][2] = m[3][2] - o.m[3][2];r.m[3][3] = m[3][3] - o.m[3][3];
        return r;
    }
    //----------------------------------------------------------------------------
    template <typename TN> bool Matrix4<TN>::operator ==(const Matrix4<TN> & o) const
    {
        if(m[0][0] != o.m[0][0] || m[0][1] != o.m[0][1] || m[0][2] != o.m[0][2] || m[0][3] != o.m[0][3] ||
           m[1][0] != o.m[1][0] || m[1][1] != o.m[1][1] || m[1][2] != o.m[1][2] || m[1][3] != o.m[1][3] ||
           m[2][0] != o.m[2][0] || m[2][1] != o.m[2][1] || m[2][2] != o.m[2][2] || m[2][3] != o.m[2][3] ||
           m[3][0] != o.m[3][0] || m[3][1] != o.m[3][1] || m[3][2] != o.m[3][2] || m[3][3] != o.m[3][3])
            return false;
        return true;
    }
    //----------------------------------------------------------------------------
    template <typename TN> bool Matrix4<TN>::operator !=(const Matrix4<TN> & o) const
    {
        if(m[0][0] != o.m[0][0] || m[0][1] != o.m[0][1] || m[0][2] != o.m[0][2] || m[0][3] != o.m[0][3] ||
           m[1][0] != o.m[1][0] || m[1][1] != o.m[1][1] || m[1][2] != o.m[1][2] || m[1][3] != o.m[1][3] ||
           m[2][0] != o.m[2][0] || m[2][1] != o.m[2][1] || m[2][2] != o.m[2][2] || m[2][3] != o.m[2][3] ||
           m[3][0] != o.m[3][0] || m[3][1] != o.m[3][1] || m[3][2] != o.m[3][2] || m[3][3] != o.m[3][3])
            return true;
        return false;
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline void Matrix4<TN>::operator =(const Matrix3<TN> & o)
    {
        m[0][0] = o.m[0][0]; m[0][1] = o.m[0][1]; m[0][2] = o.m[0][2];
        m[1][0] = o.m[1][0]; m[1][1] = o.m[1][1]; m[1][2] = o.m[1][2];
        m[2][0] = o.m[2][0]; m[2][1] = o.m[2][1]; m[2][2] = o.m[2][2];
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline Matrix4<TN> Matrix4<TN>::T() const
    {
        return Matrix4<TN>(
            m[0][0], m[1][0], m[2][0], m[3][0],
            m[0][1], m[1][1], m[2][1], m[3][1],
            m[0][2], m[1][2], m[2][2], m[3][2],
            m[0][3], m[1][3], m[2][3], m[3][3]);
    }
    //----------------------------------------------------------------------------
    template <typename TN> void Matrix4<TN>::T(TN * d, const Matrix4<TN> & o2)
    {
        d[0]  = d[0] * o2.m[0][0] + d[1] * o2.m[1][0] + d[2] * o2.m[2][0] + d[3] * o2.m[3][0];
        d[4]  = d[0] * o2.m[0][1] + d[1] * o2.m[1][1] + d[2] * o2.m[2][1] + d[3] * o2.m[3][1];
        d[8]  = d[0] * o2.m[0][2] + d[1] * o2.m[1][2] + d[2] * o2.m[2][2] + d[3] * o2.m[3][2];
        d[12] = d[0] * o2.m[0][3] + d[1] * o2.m[1][3] + d[2] * o2.m[2][3] + d[3] * o2.m[3][3];

        d[1]  = d[4] * o2.m[0][0] + d[5] * o2.m[1][0] + d[6] * o2.m[2][0] + d[7] * o2.m[3][0];
        d[5]  = d[4] * o2.m[0][1] + d[5] * o2.m[1][1] + d[6] * o2.m[2][1] + d[7] * o2.m[3][1];
        d[9]  = d[4] * o2.m[0][2] + d[5] * o2.m[1][2] + d[6] * o2.m[2][2] + d[7] * o2.m[3][2];
        d[13] = d[4] * o2.m[0][3] + d[5] * o2.m[1][3] + d[6] * o2.m[2][3] + d[7] * o2.m[3][3];

        d[2]  = d[8] * o2.m[0][0] + d[9] * o2.m[1][0] + d[10] * o2.m[2][0] + d[11] * o2.m[3][0];
        d[6]  = d[8] * o2.m[0][1] + d[9] * o2.m[1][1] + d[10] * o2.m[2][1] + d[11] * o2.m[3][1];
        d[10] = d[8] * o2.m[0][2] + d[9] * o2.m[1][2] + d[10] * o2.m[2][2] + d[11] * o2.m[3][2];
        d[14] = d[8] * o2.m[0][3] + d[9] * o2.m[1][3] + d[10] * o2.m[2][3] + d[11] * o2.m[3][3];

        d[3]  = d[12] * o2.m[0][0] + d[13] * o2.m[1][0] + d[14] * o2.m[2][0] + d[15] * o2.m[3][0];
        d[7]  = d[12] * o2.m[0][1] + d[13] * o2.m[1][1] + d[14] * o2.m[2][1] + d[15] * o2.m[3][1];
        d[11] = d[12] * o2.m[0][2] + d[13] * o2.m[1][2] + d[14] * o2.m[2][2] + d[15] * o2.m[3][2];
        d[15] = d[12] * o2.m[0][3] + d[13] * o2.m[1][3] + d[14] * o2.m[2][3] + d[15] * o2.m[3][3];
    }
    //----------------------------------------------------------------------------
    template <typename TN> const Matrix4<TN> & Matrix4<TN>::Tmul(const Matrix4<TN> & o) const
    {
        m[0][0] = m[0][0] * o.m[0][0] + m[1][0] * o.m[1][0] + m[2][0] * o.m[2][0] + m[3][0] * o.m[3][0];
        m[0][1] = m[0][0] * o.m[0][1] + m[1][0] * o.m[1][1] + m[2][0] * o.m[2][1] + m[3][0] * o.m[3][1];
        m[0][2] = m[0][0] * o.m[0][2] + m[1][0] * o.m[1][2] + m[2][0] * o.m[2][2] + m[3][0] * o.m[3][2];
        m[0][3] = m[0][0] * o.m[0][3] + m[1][0] * o.m[1][3] + m[2][0] * o.m[2][3] + m[3][0] * o.m[3][3];

        m[1][0] = m[0][1] * o.m[0][0] + m[1][1] * o.m[1][0] + m[2][1] * o.m[2][0] + m[3][1] * o.m[3][0];
        m[1][1] = m[0][1] * o.m[0][1] + m[1][1] * o.m[1][1] + m[2][1] * o.m[2][1] + m[3][1] * o.m[3][1];
        m[1][2] = m[0][1] * o.m[0][2] + m[1][1] * o.m[1][2] + m[2][1] * o.m[2][2] + m[3][1] * o.m[3][2];
        m[1][3] = m[0][1] * o.m[0][3] + m[1][1] * o.m[1][3] + m[2][1] * o.m[2][3] + m[3][1] * o.m[3][3];

        m[2][0] = m[0][2] * o.m[0][0] + m[1][2] * o.m[1][0] + m[2][2] * o.m[2][0] + m[3][2] * o.m[3][0];
        m[2][1] = m[0][2] * o.m[0][1] + m[1][2] * o.m[1][1] + m[2][2] * o.m[2][1] + m[3][2] * o.m[3][1];
        m[2][2] = m[0][2] * o.m[0][2] + m[1][2] * o.m[1][2] + m[2][2] * o.m[2][2] + m[3][2] * o.m[3][2];
        m[2][3] = m[0][2] * o.m[0][3] + m[1][2] * o.m[1][3] + m[2][2] * o.m[2][3] + m[3][2] * o.m[3][3];

        m[3][0] = m[0][3] * o.m[0][0] + m[1][3] * o.m[1][0] + m[2][3] * o.m[2][0] + m[3][3] * o.m[3][0];
        m[3][1] = m[0][3] * o.m[0][1] + m[1][3] * o.m[1][1] + m[2][3] * o.m[2][1] + m[3][3] * o.m[3][1];
        m[3][2] = m[0][3] * o.m[0][2] + m[1][3] * o.m[1][2] + m[2][3] * o.m[2][2] + m[3][3] * o.m[3][2];
        m[3][3] = m[0][3] * o.m[0][3] + m[1][3] * o.m[1][3] + m[2][3] * o.m[2][3] + m[3][3] * o.m[3][3];
        return *this;
    }
    //----------------------------------------------------------------------------
    template <typename TN> const Matrix4<TN> & Matrix4<TN>::mulT(const Matrix4<TN> & o) const
    {
        m[0][0] = m[0][0] * o.m[0][0] + m[0][1] * o.m[0][1] + m[0][2] * o.m[0][2] + m[0][3] * o.m[0][3];
        m[0][1] = m[0][0] * o.m[1][0] + m[0][1] * o.m[1][1] + m[0][2] * o.m[1][2] + m[0][3] * o.m[1][3];
        m[0][2] = m[0][0] * o.m[2][0] + m[0][1] * o.m[2][1] + m[0][2] * o.m[2][2] + m[0][3] * o.m[2][3];
        m[0][3] = m[0][0] * o.m[3][0] + m[0][1] * o.m[3][1] + m[0][2] * o.m[3][2] + m[0][3] * o.m[3][3];

        m[1][0] = m[1][0] * o.m[0][0] + m[1][1] * o.m[0][1] + m[1][2] * o.m[0][2] + m[1][3] * o.m[0][3];
        m[1][1] = m[1][0] * o.m[1][0] + m[1][1] * o.m[1][1] + m[1][2] * o.m[1][2] + m[1][3] * o.m[1][3];
        m[1][2] = m[1][0] * o.m[2][0] + m[1][1] * o.m[2][1] + m[1][2] * o.m[2][2] + m[1][3] * o.m[2][3];
        m[1][3] = m[1][0] * o.m[3][0] + m[1][1] * o.m[3][1] + m[1][2] * o.m[3][2] + m[1][3] * o.m[3][3];

        m[2][0] = m[2][0] * o.m[0][0] + m[2][1] * o.m[0][1] + m[2][2] * o.m[0][2] + m[2][3] * o.m[0][3];
        m[2][1] = m[2][0] * o.m[1][0] + m[2][1] * o.m[1][1] + m[2][2] * o.m[1][2] + m[2][3] * o.m[1][3];
        m[2][2] = m[2][0] * o.m[2][0] + m[2][1] * o.m[2][1] + m[2][2] * o.m[2][2] + m[2][3] * o.m[2][3];
        m[2][3] = m[2][0] * o.m[3][0] + m[2][1] * o.m[3][1] + m[2][2] * o.m[3][2] + m[2][3] * o.m[3][3];

        m[3][0] = m[3][0] * o.m[0][0] + m[3][1] * o.m[0][1] + m[3][2] * o.m[0][2] + m[3][3] * o.m[0][3];
        m[3][1] = m[3][0] * o.m[1][0] + m[3][1] * o.m[1][1] + m[3][2] * o.m[1][2] + m[3][3] * o.m[1][3];
        m[3][2] = m[3][0] * o.m[2][0] + m[3][1] * o.m[2][1] + m[3][2] * o.m[2][2] + m[3][3] * o.m[2][3];
        m[3][3] = m[3][0] * o.m[3][0] + m[3][1] * o.m[3][1] + m[3][2] * o.m[3][2] + m[3][3] * o.m[3][3];
        return *this;
    }
    //----------------------------------------------------------------------------
    template <typename TN> const Matrix4<TN> & Matrix4<TN>::TmulT(const Matrix4<TN> & o) const
    {
        m[0][0] = m[0][0] * o.m[0][0] + m[1][0] * o.m[0][1] + m[2][0] * o.m[0][2] + m[3][0] * o.m[0][3];
        m[0][1] = m[0][0] * o.m[1][0] + m[1][0] * o.m[1][1] + m[2][0] * o.m[1][2] + m[3][0] * o.m[1][3];
        m[0][2] = m[0][0] * o.m[2][0] + m[1][0] * o.m[2][1] + m[2][0] * o.m[2][2] + m[3][0] * o.m[2][3];
        m[0][3] = m[0][0] * o.m[3][0] + m[1][0] * o.m[3][1] + m[2][0] * o.m[3][2] + m[3][0] * o.m[3][3];

        m[1][0] = m[0][1] * o.m[0][0] + m[1][1] * o.m[0][1] + m[2][1] * o.m[0][2] + m[3][1] * o.m[0][3];
        m[1][1] = m[0][1] * o.m[1][0] + m[1][1] * o.m[1][1] + m[2][1] * o.m[1][2] + m[3][1] * o.m[1][3];
        m[1][2] = m[0][1] * o.m[2][0] + m[1][1] * o.m[2][1] + m[2][1] * o.m[2][2] + m[3][1] * o.m[2][3];
        m[1][3] = m[0][1] * o.m[3][0] + m[1][1] * o.m[3][1] + m[2][1] * o.m[3][2] + m[3][1] * o.m[3][3];

        m[2][0] = m[0][2] * o.m[0][0] + m[1][2] * o.m[0][1] + m[2][2] * o.m[0][2] + m[3][2] * o.m[0][3];
        m[2][1] = m[0][2] * o.m[1][0] + m[1][2] * o.m[1][1] + m[2][2] * o.m[1][2] + m[3][2] * o.m[1][3];
        m[2][2] = m[0][2] * o.m[2][0] + m[1][2] * o.m[2][1] + m[2][2] * o.m[2][2] + m[3][2] * o.m[2][3];
        m[2][3] = m[0][2] * o.m[3][0] + m[1][2] * o.m[3][1] + m[2][2] * o.m[3][2] + m[3][2] * o.m[3][3];

        m[3][0] = m[0][3] * o.m[0][0] + m[1][3] * o.m[0][1] + m[2][3] * o.m[0][2] + m[3][3] * o.m[0][3];
        m[3][1] = m[0][3] * o.m[1][0] + m[1][3] * o.m[1][1] + m[2][3] * o.m[1][2] + m[3][3] * o.m[1][3];
        m[3][2] = m[0][3] * o.m[2][0] + m[1][3] * o.m[2][1] + m[2][3] * o.m[2][2] + m[3][3] * o.m[2][3];
        m[3][3] = m[0][3] * o.m[3][0] + m[1][3] * o.m[3][1] + m[2][3] * o.m[3][2] + m[3][3] * o.m[3][3];
        return *this;
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline void Matrix4<TN>::setTrans(const Vector3<TN> & v)
    {
        m[0][3] = v.x;
        m[1][3] = v.y;
        m[2][3] = v.z;
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline Vector3<TN> Matrix4<TN>::getTrans() const
    {
        return Vector3<TN>(m[0][3], m[1][3], m[2][3]);
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline void Matrix4<TN>::setScale(const Vector3<TN> & v)
    {
        m[0][0] = v.x;
        m[1][1] = v.y;
        m[2][2] = v.z;
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline Vector3<TN> Matrix4<TN>::getScale() const
    {
        return Vector3<TN>(m[0][0], m[1][1], m[2][2]);
    }
    //----------------------------------------------------------------------------
    template <typename TN> void Matrix4<TN>::set(const Vector3<TN> & p,
        const Vector3<TN> & s, const Quaternion & o)
    {
        Matrix3<TN> rot, scale;

		NIIf fTx = 2.0f * o.x;
		NIIf fTy = 2.0f * o.y;
		NIIf fTz = 2.0f * o.z;
		NIIf fTwx = fTx * o.w;
		NIIf fTwy = fTy * o.w;
		NIIf fTwz = fTz * o.w;
		NIIf fTxx = fTx * o.x;
		NIIf fTxy = fTy * o.x;
		NIIf fTxz = fTz * o.x;
		NIIf fTyy = fTy * o.y;
		NIIf fTyz = fTz * o.y;
		NIIf fTzz = fTz * o.z;

		rot[0][0] = 1.0f - (fTyy + fTzz);
		rot[0][1] = fTxy - fTwz;
		rot[0][2] = fTxz + fTwy;
		rot[1][0] = fTxy + fTwz;
		rot[1][1] = 1.0f - (fTxx + fTzz);
		rot[1][2] = fTyz - fTwx;
		rot[2][0] = fTxz - fTwy;
		rot[2][1] = fTyz + fTwx;
		rot[2][2] = 1.0f - (fTxx + fTyy);

        scale = Matrix3<TN>::ZERO;
        scale[0][0] = s.x;
        scale[1][1] = s.y;
        scale[2][2] = s.z;

        *this = rot * scale;
        this->setTrans(p);

        m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
    }
    //-----------------------------------------------------------------------
	template <typename TN> void Matrix4<TN>::get(Vector3<TN> & p, 
        Vector3<TN> & s, Quaternion & o) const
	{
		assert(isAffine());

		Matrix3<TN> m3x3 = *this;
		Matrix3<TN> rot;
		Vector3<TN> vecU;
		m3x3.getQDU(rot, s, vecU);

		NIIf fTrace = rot[0][0] + rot[1][1] + rot[2][2];
		NIIf fRoot;
		if (fTrace > 0.0)
		{
			// |w| > 1/2, may as well choose w > 1/2
			fRoot = Math::sqrt(fTrace + 1.0f);  // 2w
			o.w = 0.5f * fRoot;
			fRoot = 0.5f / fRoot;  // 1/(4w)
			o.x = (rot[2][1] - rot[1][2]) * fRoot;
			o.y = (rot[0][2] - rot[2][0]) * fRoot;
			o.z = (rot[1][0] - rot[0][1]) * fRoot;
		}
		else
		{
			// |w| <= 1/2
			static NCount s_iNext[3] = { 1, 2, 0 };
			NCount i = 0;
			if (rot[1][1] > rot[0][0])
				i = 1;
			if (rot[2][2] > rot[i][i])
				i = 2;
			NCount j = s_iNext[i];
			NCount k = s_iNext[j];

			fRoot = Math::sqrt(rot[i][i] - rot[j][j] - rot[k][k] + 1.0f);
			NIIf * apkQuat[3] = { &o.x, &o.y, &o.z };
			*apkQuat[i] = 0.5f * fRoot;
			fRoot = 0.5f / fRoot;
			o.w = (rot[k][j] - rot[j][k]) * fRoot;
			*apkQuat[j] = (rot[j][i] + rot[i][j]) * fRoot;
			*apkQuat[k] = (rot[k][i] + rot[i][k]) * fRoot;
		}

		p = Vector3<TN>(m[0][3], m[1][3], m[2][3]);
	}
    //----------------------------------------------------------------------------
    template <typename TN> bool Matrix4<TN>::isScale() const
    {
        // 检查列向量尺度(==本地轴)
        TN t = m[0][0] * m[0][0] + m[1][0] * m[1][0] + m[2][0] * m[2][0];
        if(fabs(1.0 - t) > 1e-04)
            return true;

        t = m[0][1] * m[0][1] + m[1][1] * m[1][1] + m[2][1] * m[2][1];
        if(fabs(1.0 - t) > 1e-04)
            return true;

        t = m[0][2] * m[0][2] + m[1][2] * m[1][2] + m[2][2] * m[2][2];
        if(fabs(1.0 - t) > 1e-04)
            return true;

        return false;
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline bool Matrix4<TN>::isNegativeScale() const
    {
        return det() < 0;
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline Matrix4<TN> Matrix4<TN>::operator *(const TN & s) const
    {
        return Matrix4<TN>(
            s * m[0][0], s * m[0][1], s * m[0][2], s * m[0][3],
            s * m[1][0], s * m[1][1], s * m[1][2], s * m[1][3],
            s * m[2][0], s * m[2][1], s * m[2][2], s * m[2][3],
            s * m[3][0], s * m[3][1], s * m[3][2], s * m[3][3]);
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline bool Matrix4<TN>::isAffine(void) const
    {
        return m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1;
    }
    //----------------------------------------------------------------------------
    template <typename TN> Matrix4<TN> Matrix4<TN>::affine(const Matrix4<TN> & o) const
    {
        assert(isAffine() && o.isAffine());

        return Matrix4<TN>(
            m[0][0] * o.m[0][0] + m[0][1] * o.m[1][0] + m[0][2] * o.m[2][0],
            m[0][0] * o.m[0][1] + m[0][1] * o.m[1][1] + m[0][2] * o.m[2][1],
            m[0][0] * o.m[0][2] + m[0][1] * o.m[1][2] + m[0][2] * o.m[2][2],
            m[0][0] * o.m[0][3] + m[0][1] * o.m[1][3] + m[0][2] * o.m[2][3] + m[0][3],

            m[1][0] * o.m[0][0] + m[1][1] * o.m[1][0] + m[1][2] * o.m[2][0],
            m[1][0] * o.m[0][1] + m[1][1] * o.m[1][1] + m[1][2] * o.m[2][1],
            m[1][0] * o.m[0][2] + m[1][1] * o.m[1][2] + m[1][2] * o.m[2][2],
            m[1][0] * o.m[0][3] + m[1][1] * o.m[1][3] + m[1][2] * o.m[2][3] + m[1][3],

            m[2][0] * o.m[0][0] + m[2][1] * o.m[1][0] + m[2][2] * o.m[2][0],
            m[2][0] * o.m[0][1] + m[2][1] * o.m[1][1] + m[2][2] * o.m[2][1],
            m[2][0] * o.m[0][2] + m[2][1] * o.m[1][2] + m[2][2] * o.m[2][2],
            m[2][0] * o.m[0][3] + m[2][1] * o.m[1][3] + m[2][2] * o.m[2][3] + m[2][3],

            0, 0, 0, 1);
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline Vector3<TN> Matrix4<TN>::affine(const Vector3<TN> & v) const
    {
        assert(isAffine());

        return Vector3<TN>(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3],
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]);
    }
    //----------------------------------------------------------------------------
    template <typename TN> inline Vector4<TN> Matrix4<TN>::affine(const Vector4<TN> & v) const
    {
        assert(isAffine());

        return Vector4<TN>(
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w,
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
            v.w);
    }
    //----------------------------------------------------------------------------
    template <typename TN> static NIIf MINOR(const Matrix4<TN>& m, NCount r0,
        NCount r1, NCount r2, NCount c0, NCount c1, NCount c2)
    {
        return m[r0][c0] * (m[r1][c1] * m[r2][c2] - m[r2][c1] * m[r1][c2]) -
            m[r0][c1] * (m[r1][c0] * m[r2][c2] - m[r2][c0] * m[r1][c2]) +
            m[r0][c2] * (m[r1][c0] * m[r2][c1] - m[r2][c0] * m[r1][c1]);
    }
    //----------------------------------------------------------------------------
    template <typename TN> Matrix4<TN> Matrix4<TN>::adjoint() const
    {
        return Matrix4<TN>(
            MINOR(*this, 1, 2, 3, 1, 2, 3), -MINOR(*this, 0, 2, 3, 1, 2, 3),
            MINOR(*this, 0, 1, 3, 1, 2, 3), -MINOR(*this, 0, 1, 2, 1, 2, 3),

            -MINOR(*this, 1, 2, 3, 0, 2, 3), MINOR(*this, 0, 2, 3, 0, 2, 3),
            -MINOR(*this, 0, 1, 3, 0, 2, 3), MINOR(*this, 0, 1, 2, 0, 2, 3),

            MINOR(*this, 1, 2, 3, 0, 1, 3), -MINOR(*this, 0, 2, 3, 0, 1, 3),
            MINOR(*this, 0, 1, 3, 0, 1, 3), -MINOR(*this, 0, 1, 2, 0, 1, 3),

            -MINOR(*this, 1, 2, 3, 0, 1, 2), MINOR(*this, 0, 2, 3, 0, 1, 2),
            -MINOR(*this, 0, 1, 3, 0, 1, 2), MINOR(*this, 0, 1, 2, 0, 1, 2) );
    }
    //------------------------------------------------------------------------
    template <typename TN> TN Matrix4<TN>::det() const
    {
        return
            m[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) -
            m[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) +
            m[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) -
            m[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2);
    }
    //------------------------------------------------------------------------
    template <typename TN> Matrix4<TN> Matrix4<TN>::inverse() const
    {
        TN m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
        TN m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
        TN m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
        TN m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

        TN v0 = m20 * m31 - m21 * m30;
        TN v1 = m20 * m32 - m22 * m30;
        TN v2 = m20 * m33 - m23 * m30;
        TN v3 = m21 * m32 - m22 * m31;
        TN v4 = m21 * m33 - m23 * m31;
        TN v5 = m22 * m33 - m23 * m32;

        TN t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
        TN t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
        TN t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
        TN t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

        TN invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

        TN d00 = t00 * invDet;
        TN d10 = t10 * invDet;
        TN d20 = t20 * invDet;
        TN d30 = t30 * invDet;

        TN d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        TN d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        TN d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        TN d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m10 * m31 - m11 * m30;
        v1 = m10 * m32 - m12 * m30;
        v2 = m10 * m33 - m13 * m30;
        v3 = m11 * m32 - m12 * m31;
        v4 = m11 * m33 - m13 * m31;
        v5 = m12 * m33 - m13 * m32;

        TN d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        TN d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        TN d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        TN d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        v0 = m21 * m10 - m20 * m11;
        v1 = m22 * m10 - m20 * m12;
        v2 = m23 * m10 - m20 * m13;
        v3 = m22 * m11 - m21 * m12;
        v4 = m23 * m11 - m21 * m13;
        v5 = m23 * m12 - m22 * m13;

        TN d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
        TN d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
        TN d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
        TN d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

        return Matrix4<TN>(d00, d01, d02, d03, d10, d11, d12, d13,
            d20, d21, d22, d23, d30, d31, d32, d33);
    }
    //----------------------------------------------------------------
    template <typename TN> Matrix4<TN> Matrix4<TN>::inverseAffine() const
    {
        assert(isAffine());

        TN m10 = m[1][0], m11 = m[1][1], m12 = m[1][2];
        TN m20 = m[2][0], m21 = m[2][1], m22 = m[2][2];

        TN t00 = m22 * m11 - m21 * m12;
        TN t10 = m20 * m12 - m22 * m10;
        TN t20 = m21 * m10 - m20 * m11;

        TN m00 = m[0][0], m01 = m[0][1], m02 = m[0][2];

        TN invDet = 1 / (m00 * t00 + m01 * t10 + m02 * t20);

        t00 *= invDet; t10 *= invDet; t20 *= invDet;

        m00 *= invDet; m01 *= invDet; m02 *= invDet;

        TN r00 = t00;
        TN r01 = m02 * m21 - m01 * m22;
        TN r02 = m01 * m12 - m02 * m11;

        TN r10 = t10;
        TN r11 = m00 * m22 - m02 * m20;
        TN r12 = m02 * m10 - m00 * m12;

        TN r20 = t20;
        TN r21 = m01 * m20 - m00 * m21;
        TN r22 = m00 * m11 - m01 * m10;

        TN m03 = m[0][3], m13 = m[1][3], m23 = m[2][3];

        TN r03 = - (r00 * m03 + r01 * m13 + r02 * m23);
        TN r13 = - (r10 * m03 + r11 * m13 + r12 * m23);
        TN r23 = - (r20 * m03 + r21 * m13 + r22 * m23);

        return Matrix4<TN>(r00, r01, r02, r03, r10, r11, r12, r13,
            r20, r21, r22, r23, 0, 0, 0, 1);
    }
    //-------------------------------------------------------------------------
    template <typename TN> void Matrix4<TN>::makeInverseTransform(const Vector3<TN> & p,
        const Vector3<TN> & s, const Quaternion & o)
    {
        Vector3<TN> invTranslate = -p;
        Vector3<TN> invScale(1/s.x, 1/s.y, 1/s.z);
        Quaternion invRot = o.inverse();
        // Because we're inverting, order is translation, rotation, scale
        // So make translation relative to scale & rotation
        invTranslate *= invScale; // scale
        invTranslate = invRot * invTranslate; // rotate
        // Next, make a 3x3 rotation matrix and apply inverse scale
        Matrix3<TN> rot, scale;
        invRot.ToRotationMatrix(rot);
        scale = Matrix3<TN>::ZERO;
        scale[0][0] = invScale.x;
        scale[1][1] = invScale.y;
        scale[2][2] = invScale.z;
        // Set up final matrix with scale, rotation and translation
        *this = scale * rot;
        this->setTrans(invTranslate);

        m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
    }
    //-----------------------------------------------------------------------
    template <typename TN> void Matrix4<TN>::makeObliqueProjection(const Vector3f & n,
        const Vector3f & p, const Vector3f & dir)
    {
        //     +-                         -+
        // M = | I-2*n*n^TN    2*Dot(n,p)*n |
        //     |     0^TN            1      |
        //     +-                         -+
        //
        // where p is a point on the plane and n is a unit-length plane normal.
        NIIf twoDotNO = 2 * n.dotProduct(p);
        _m[ 0] = 1 - 2 * n[0] * n[0];
        _m[ 1] = -2 * n[0] * n[1];
        _m[ 2] = -2 * n[0] * n[2];
        _m[ 3] = twoDotNO * n[0];
        _m[ 4] = -2 * n[1] * n[0];
        _m[ 5] = 1 - 2 * n[1] * n[1];
        _m[ 6] = -2 * n[1] * n[2];
        _m[ 7] = twoDotNO * n[1];
        _m[ 8] = -2 * n[2] * n[0];
        _m[ 9] = -2 * n[2] * n[1];
        _m[10] = 1 - 2 * n[2] * n[2];
        _m[11] = twoDotNO * n[2];
        _m[12] = 0;
        _m[13] = 0;
        _m[14] = 0;
        _m[15] = 1;
    }
    //-----------------------------------------------------------------------
    template <typename TN> void Matrix4<TN>::makePerspectiveProjection(
        const Vector3f & n, const Vector3f & p, const Vector3f & eye)
    {
        //     +-                                                           -+
        // M = | Dot(n,eye-p)*I - eye*n^TN    -(Dot(n,eye-p)*I - eye*n^TN)*eye |
        //     |        -n^t                      Dot(n,eye)                 |
        //     +-                                                           -+
        //
        // where E is the eye point, P is a point on the plane, and N is a
        // unit-length plane normal.
        NIIf dotND = n.dotProduct(eye - p);
        _m[ 0] = dotND - eye[0] * n[0];
        _m[ 1] = -eye[0] * n[1];
        _m[ 2] = -eye[0] * n[2];
        _m[ 3] = -(_m[0] * eye[0] + _m[1] * eye[1] + _m[2] * eye[2]);
        _m[ 4] = -eye[1] * n[0];
        _m[ 5] = dotND - eye[1] * n[1];
        _m[ 6] = -eye[1] * n[2];
        _m[ 7] = -(_m[4] * eye[0] + _m[5] * eye[1] + _m[6] * eye[2]);
        _m[ 8] = -eye[2] * n[0];
        _m[ 9] = -eye[2] * n[1];
        _m[10] = dotND- eye[2] * n[2];
        _m[11] = -(_m[8] * eye[0] + _m[9] * eye[1] + _m[10] * eye[2]);
        _m[12] = -n[0];
        _m[13] = -n[1];
        _m[14] = -n[2];
        _m[15] = n.dotProduct(eye);
    }
    //-----------------------------------------------------------------------
    template <typename TN> void Matrix4<TN>::makeReflection(const Vector3f & n,
        const Vector3f & origin)
    {
        //     +-                         -+
        // M = | I-2*N*N^TN    2*Dot(N,P)*N |
        //     |     0^TN            1      |
        //     +-                         -+
        //
        // where P is a point on the plane and N is a unit-length plane normal.

        NIIf twoDotNO = 2 * n.dotProduct(origin);
        _m[ 0] = 1 - 2 * n[0] * n[0];
        _m[ 1] = -2 * n[0] * n[1];
        _m[ 2] = -2 * n[0] * n[2];
        _m[ 3] = twoDotNO * n[0];
        _m[ 4] = -2 * n[1] * n[0];
        _m[ 5] = 1 - 2 * n[1] * n[1];
        _m[ 6] = -2 * n[1] * n[2];
        _m[ 7] = twoDotNO * n[1];
        _m[ 8] = -2 * n[2] * n[0];
        _m[ 9] = -2 * n[2] * n[1];
        _m[10] = 1 - 2 * n[2] * n[2];
        _m[11] = twoDotNO * n[2];
        _m[12] = 0;
        _m[13] = 0;
        _m[14] = 0;
        _m[15] = 1;
    }
    //--------------------------------------------------------------------------
    typedef Matrix4<NIIi>   Matrix4i;
    typedef Matrix4<NIIf>   Matrix4f;
    typedef Matrix4<NIId>   Matrix4d;
    typedef Matrix4f        matrix4;

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
    
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // PerspectiveMatrix4
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    inline PerspectiveMatrix4::PerspectiveMatrix4(NIIf fovx, NIIf fovy, NIIf mfovyx,
        NIIf mfovy, NIIf minz, NIIf maxz) :
            Matrix4f(
                mfovyx / Math::Tan(fovx),    0.0f,             0.0f,                0.0f,
                0.0f,             mfovy / Math::Tan(fovy),     0.0f,                0.0f,
                0.0f,                        0.0f,     maxz / (maxz - minz),        1.0f,
                0.0f,                        0.0f,   -minz * (maxz / (maxz - minz)),0.0f)
    {
    }
    //--------------------------------------------------------------------------
    inline PerspectiveMatrix4::PerspectiveMatrix4(NIIf FOV, NIIf width, NIIf height,
        NIIf minz, NIIf maxz) :
        Matrix4f(
            1.0f / Math::Tan(FOV),  0.0f,                   0.0f,              0.0f,
            0.0f,       width / Math::Tan(FOV) / height,    0.0f,              0.0f,
            0.0f,                   0.0f,           maxz / (maxz - minz),      1.0f,
            0.0f,                   0.0f,      -minz * (maxz / (maxz - minz)), 0.0f)
    {
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // OrthoMatrix4
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    inline OrthoMatrix4::OrthoMatrix4(NIIf width, NIIf height, NIIf zscale, NIIf zoffset) :
        Matrix4f(
            1.0f / width,    0.0f,            0.0f,       0.0f,
            0.0f,            1.0f / height,   0.0f,       0.0f,
            0.0f,            0.0f,           zscale,      0.0f,
            0.0f,            0.0f,     zoffset * zscale,  1.0f)
    {
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // TranslationMatrix4
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    inline TranslationMatrix4::TranslationMatrix4(const Vector3f & oft) :
        Matrix4f(1.0, 0.0, 0.0, oft.x, 0.0, 1.0, 0.0, oft.y,
            0.0, 0.0, 1.0, oft.z, 0.0, 0.0, 0.0, 1.0)
    {
    }
    //--------------------------------------------------------------------------
    inline TranslationMatrix4::TranslationMatrix4(NIIf oftx, NIIf ofty, NIIf oftz) :
        Matrix4f(1.0, 0.0, 0.0, oftx, 0.0, 1.0, 0.0, ofty,
            0.0, 0.0, 1.0, oftz, 0.0, 0.0, 0.0, 1.0)
    {
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // ScaleMatrix4
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    inline ScaleMatrix4::ScaleMatrix4(const Vector3f & v) :
        Matrix4f(v.x, 0.0, 0.0, 0.0, 0.0, v.y, 0.0, 0.0,
            0.0, 0.0, v.z, 0.0, 0.0, 0.0, 0.0, 1.0)
    {
    }
    //--------------------------------------------------------------------------
    inline ScaleMatrix4::ScaleMatrix4(NIIf x, NIIf y, NIIf z) :
        Matrix4f(x, 0.0, 0.0, 0.0, 0.0, y, 0.0, 0.0,
            0.0, 0.0, z, 0.0, 0.0, 0.0, 0.0, 1.0)
    {
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // RotationMatrix4
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    inline RotationMatrix4::RotationMatrix4(NIIf pitch, NIIf yaw, NIIf roll)
    {
        NIIf SR = Math::Sin(roll),
             SP = Math::Sin(pitch),
             SY = Math::Sin(yaw),
             CR = Math::Cos(roll),
             CP = Math::Cos(pitch),
             CY = Math::Cos(yaw);

        m[0][0] = CP * CY;m[0][1] = CP * SY;m[0][2] = SP;m[0][3] = 0.f;

        m[1][0] = SR * SP * CY - CR * SY;
        m[1][1] = SR * SP * SY + CR * CY;
        m[1][2] = - SR * CP;
        m[1][3] = 0.f;

        m[2][0] = -(CR * SP * CY + SR * SY);
        m[2][1] = CY * SR - CR * SP * SY;
        m[2][2] = CR * CP;
        m[2][3] = 0.f;

        m[3][0] = 0.f; m[3][1] = 0.f; m[3][2] = 0.f; m[3][3] = 1.0;
    }
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    // InverseRotationMatrix4
    //--------------------------------------------------------------------------
    //--------------------------------------------------------------------------
    inline InverseRotationMatrix4::InverseRotationMatrix4(NIIf pitch , NIIf yaw , NIIf roll) :
        Matrix4f(
            Matrix4f(    // yaw
                Math::Cos(-yaw),  Math::Sin(-yaw), 0.0f, 0.0f,
                -Math::Sin(-yaw), Math::Cos(-yaw), 0.0f, 0.0f,
                0.0f,             0.0f,            1.0f, 0.0f,
                0.0f,             0.0f,            0.0f, 1.0f) *
            Matrix4f(    // pitch
                Math::Cos(-pitch), 0.0f, Math::Sin(-pitch), 0.0f,
                0.0f,              1.0f,           0.0f,    0.0f,
                -Math::Sin(-pitch),0.0f, Math::Cos(-pitch), 0.0f,
                0.0f,              0.0f,           0.0f,    1.0f) *
            Matrix4f(    // roll
                1.0f,   0.0f,             0.0f,              0.0f,
                0.0f,   Math::Cos(-roll), -Math::Sin(-roll), 0.0f,
                0.0f,   Math::Sin(-roll), Math::Cos(-roll),  0.0f,
                0.0f,   0.0f,             0.0f,              1.0f))
    {
    }
    //--------------------------------------------------------------------------
    template <typename TN> Nostream & operator << (Nostream & o, const Matrix4<TN> & m)
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
}
#endif