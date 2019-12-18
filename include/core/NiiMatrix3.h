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

#ifndef _NII_MATRIX3_H_
#define _NII_MATRIX3_H_

#include "NiiPreInclude.h"
#include "NiiVector3.h"

namespace NII
{
    /** 一个3x3矩阵,可以描述事物围绕轴旋转.
    @remark
        使用数学结构的标准,自动处理各种图形引擎之间的差异,如OpenGl和Direct3D中的
        一些存储和乘法顺序不同,使用右手坐标系.
    @note
        标准数学中使用行优先存储和列乘行的方式,OpenGl和Direct3D各有所不同,OpenGl
        使用列优先存储,Direct3D使用行乘列.所以均要做转换.NIIEngine自动完成它们和
        标准数学的差异,所以使用NIIEngine时只要记住一点就是使用标准数学思想处理问题.
        标准数学 M*V 的显示布局的矩阵如下:
        <pre>
        {[0][0] [0][1] [0][2]} {x}
        |[1][0] [1][1] [1][2]|*{y}
        {[2][0] [2][1] [2][2]} {z}
        </pre>
    @note 使用 行优先存储 和 列乘行 和 右手坐标系 方式, 仿射变换也就是变换中的一个
        特殊优化效率的例子,如果使用普通的变换结果是相同的,但就是慢点点.除非应用在特
        效凹凸镜面或初始化视口等需要用到投影级距阵,视图级后的距阵基本是仿射距阵
    @note 如果是用于投影设置的距阵,opengl 深度范围是[-1, 1] 而 d3d 则是 [-1, 0]
    @version NIIEngine 3.0.0
    */
    template <typename TN> class Matrix3
    {
    public:
        Matrix3();

        Matrix3(const Matrix3<TN> & o);
        Matrix3(const Matrix4<TN> & o);

        Matrix3(TN _00, TN _01, TN _02, TN _10, TN _11, TN _12, TN _20, TN _21, TN _22);

        /** 构造函数
        @param[in] x x轴空间
        @param[in] y y轴空间
        @param[in] z z轴空间
        */
        Matrix3(const Vector3<TN> & x, const Vector3<TN> & y, const Vector3<TN> & z);

        /** 获取一列
        @param[in] col 列下标
        */
        Vector3<TN> get(NCount col)const;

        /** 设置一列的值
        @param[in] col 列下标
        @param[in] v 值
        */
        void set(NCount col, const Vector3<TN> & v);

        /** 成员访问, 允许使用this[row][col]结构
        @remark 下标级别的操作,不能超过2
        */
        TN * operator[](NCount row) const;

        /** 同类操作
        @version NIIEngine 3.0.0
        */
        Matrix3<TN> & operator = (const Matrix3<TN> & o);

        /** 缩类操作
        @remark 平移和投射部分将被抹掉
        @version NIIEngine 3.0.0
        */
        Matrix3<TN> & operator = (const Matrix4<TN> & o);

        bool operator == (const Matrix3<TN> & o) const;
        bool operator != (const Matrix3<TN> & o) const;

        /** 相加
        @remark *this + o
        @version NIIEngine 3.0.0
        */
        Matrix3<TN> operator + (const Matrix3<TN> & o) const;

        /** 相减
        @remark *this - o
        @version NIIEngine 3.0.0
        */
        Matrix3<TN> operator - (const Matrix3<TN> & o) const;

        /** 相乘
        @remark 标准数学,重要的基础算术 *this * o
        @version NIIEngine 3.0.0
        */
        Matrix3<TN> operator * (const Matrix3<TN> & o) const;

        /** 负数形式
        @remark -*this
        @version NIIEngine 3.0.0
        */
        Matrix3<TN> operator -() const;

        /** 矩阵 * 向量
        @remark [3x3 * 3x1 = 3x1]
        @version NIIEngine 3.0.0
        */
        Vector3<TN> operator * (const Vector3<TN> & o) const;

        /** 向量 * 矩阵
        @remark [1x3 * 3x3 = 1x3]
        @version NIIEngine 3.0.0
        */
        friend Vector3<TN> operator *(const Vector3<TN> & v, const Matrix3<TN> & m);

        /** 矩阵 * 缩放
        @version NIIEngine 3.0.0
        */
        Matrix3<TN> operator * (TN scale) const;

        /** 缩放 * 矩阵
        @version NIIEngine 3.0.0
        */
        friend Matrix3<TN> operator *(TN scale, const Matrix3<TN> & o);

        /** 转置距阵
        @remark (*this)^T
        @version NIIEngine 3.0.0
        */
        Matrix3<TN> T() const;

        /** 逆矩阵
        @remark 数学条件,必须是非奇异矩阵,否则会自动结束操作 m3^-1
        @param[in] m3 需要翻转的矩阵
        @param[in] functor 行列式的值最大限度,无限小越像条直线
        @return 参数一直接返回
        @version NIIEngine 3.0.0
        */
        bool inverse(Matrix3<TN> & m3, NIIf functor = 1e-06) const;

        /** 逆矩阵
        @remark 数学条件,必须是非奇异矩阵,否则会自动结束操作 (*this)^-1
        @param[in] functor 行列式的值最大限度,无限小越像条直线
        @return 这个对象
        @version NIIEngine 3.0.0
        */
        Matrix3<TN> inverse(TN functor = 1e-06) const;

        /** 把一个顶点在这个矩阵中的投射位置
        @remark 和operator *(const Vector3<TN> &)操作一样
        @param[in] src 顶点
        @param[in] dst 顶点投射后的位置
        */
        void project(const Vector3<TN> & src, Vector3<TN> & dst);

        /// 是否涉及一个缩放
        bool isScale() const;

        /** 行列式
        @remark 标准数学
        @note det = 1或-1则为正交距阵
        @version NIIEngine 3.0.0
        */
        TN det() const;

        /** 正交化
        @remark Gram-Schmidt规化正交化
        @version NIIEngine 3.0.0
        */
        void Orthonormalize();

        /** 奇异值分解
        @param[out] L
        @param[out] S
        @param[out] R
        @version NIIEngine 3.0.0
        */
        void SVD(Matrix3<TN> & L, Vector3<TN> & S, Matrix3<TN> & R) const;

        /** 奇异值合成
        @param[in] L
        @param[in] S
        @param[in] R
        @version NIIEngine 3.0.0
        */
        void SVC(const Matrix3<TN> & L, const Vector3<TN> & S, const Matrix3<TN> & R);

        /** 提取轴/角度
        @param[out] axis 轴
        @param[out] radian 角度
        @note 必须是正交矩阵
        @version NIIEngine 3.0.0
        */
        void to(Vector3<TN> & axis, Radian & radian) const;

        /** 设置轴/角度
        @param[in] axis 轴
        @param[in] radian 角度
        @version NIIEngine 3.0.0
        */
        void from(const Vector3<TN> & axis, const Radian & radian);

        /** 正交的Q,对角线D,错切(剪切)变换U
        @param[out] U, U[0]错切(x,y),U[1]错切(x,z),U[2]错切(y,z)
        @version NIIEngine 3.0.0
        */
        void getQDU(Matrix3<TN> & Q, Vector3<TN> & D, Vector3<TN> & U) const;
    protected:
        static void Bidiagonalize(Matrix3<TN> & A, Matrix3<TN> & L, Matrix3<TN> & R);

        static void GolubKahanStep(Matrix3<TN> & A, Matrix3<TN> & L, Matrix3<TN> & R);
    public:
        static const NIIf EPSILON;
        static const Matrix3<TN> ZERO;
        static const Matrix3<TN> IDENTITY;

        union
        {
            TN m[3][3];
            TN _m[9];
        };
    };
    #include "NiiMatrix3.inl"

    template <typename TN> std::ostream & operator << (std::ostream & o, const Matrix3<TN> & m3)
    {
        o << "Matrix3<TN>("
            << m3[0][0] << "," << m3[0][1] << "," << m3[0][2] << ","
            << m3[1][0] << "," << m3[1][1] << "," << m3[1][2] << ","
            << m3[2][0] << "," << m3[2][1] << "," << m3[2][2] << ")";
        return o;
    }
    typedef Matrix3<NIIi>	Matrix3i;
    typedef Matrix3<NIIf>	Matrix3f;
    typedef Matrix3<NIId>	Matrix3d;
    typedef Matrix3f		matrix3;

}
#endif