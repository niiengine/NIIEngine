/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

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

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_QUATERNION_H_
#define _NII_QUATERNION_H_

#include "NiiPreInclude.h"
#include "NiiRadian.h"
#include "NiiMatrix3.h"

namespace NII
{
    /** 四元组
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Quaternion
    {
    public:
        Quaternion();
        Quaternion(const Quaternion & q);
        Quaternion(NIIf w, NIIf x, NIIf y, NIIf z);

        /** 数据访问
        @version NIIEngine 3.0.0
        */
        inline NIIf & operator [](NCount i)
        {
            return *(&w + i);
        }

        /** 等于
        @version NIIEngine 3.0.0
        */
        bool operator== (const Quaternion & o) const;

        /** 不等于
        @version NIIEngine 3.0.0
        */
        bool operator!= (const Quaternion & o) const;

        /** 负数对象
        @version NIIEngine 3.0.0
        */
        Quaternion operator- () const;

        /** 等于
        @version NIIEngine 3.0.0
        */
        Quaternion & operator= (const Quaternion & o);

        /** 加法
        @version NIIEngine 3.0.0
        */
        Quaternion operator+ (const Quaternion & o) const;

        /** 减法
        @version NIIEngine 3.0.0
        */
        Quaternion operator- (const Quaternion & o) const;

        /** 乘法
        @version NIIEngine 3.0.0
        */
        Quaternion operator* (const Quaternion & o) const;

        /** 乘法
        @version NIIEngine 3.0.0
        */
        Quaternion operator* (NIIf factor) const;

        /** 变化向量
        @version NIIEngine 3.0.0
        */
        Vector3f operator* (const Vector3f & o) const;

        /** 乘法
        @version NIIEngine 3.0.0
        */
        _EngineAPI friend Quaternion operator* (NIIf factor, const Quaternion & q);

        /** 是否相等
        @param[in] o 另一个对象对象
        @param[in] deviation 允许误差
        @version NIIEngine 3.0.0
        */
        bool equals(const Quaternion & o, const Radian & deviation) const;

        /** 插值
        @remarks 比 nlerp 慢,但准确精细.
        @note fn(0.0) = src, fn(1.0) = dst,一般用于模型插值,低细节LOD也可以使用nlerp.
        @version NIIEngine 3.0.0
        */
        static Quaternion slerp(NIIf pos, const Quaternion & src, const Quaternion & dst,
            bool nearest = false);

        /** 插值
        @remarks 比 slerp 快但不准确
        @note fn(0.0) = src, fn(1.0) = dst,一般用于UI单元转变,低细节LOD网格骨骼变换使用.
        @version NIIEngine 3.0.0
        */
        static Quaternion nlerp(NIIf pos, const Quaternion & src, const Quaternion & dst,
            bool nearest = false);

        /** slerp二次插值
        @version NIIEngine 3.0.0
        */
        static Quaternion squad(NIIf fT, const Quaternion & src, const Quaternion & a,
            const Quaternion & b, const Quaternion & dst, bool nearest = false);

        /** 获取向量间的旋转角度
        @param[in] o1 一个向量
        @param[in] o2 一个向量
        @param[in] zeroAxis 当向量与向量之间的旋转接近零动作时的结果
        @version NIIEngine 3.0.0
        */
        static Quaternion getRotation(const Vector3f & o1, const Vector3f & o2,
            const Vector3f & zeroAxis = Vector3f::ZERO);

        /** 交换数据
        @version NIIEngine 3.0.0
        */
        inline void swap(Quaternion & o)
        {
            std::swap(w, o.w);
            std::swap(x, o.x);
            std::swap(y, o.y);
            std::swap(z, o.z);
        }

        /** 从旋转矩阵构建
        @version NIIEngine 3.0.0
        */
        void from(const Matrix3f & m);

        /** 分解到旋转矩阵
        @version NIIEngine 3.0.0
        */
        void to(Matrix3f & m) const;

        /** 从轴/角度(弧度)构建
        @version NIIEngine 3.0.0
        */
        void from(const Radian & angle, const Vector3f & axis);

        /** 分解到轴/角度(弧度)
        @version NIIEngine 3.0.0
        */
        void to(Radian & angle, Vector3f & axis) const;

        /** 从三个轴构建
        @version NIIEngine 3.0.0
        */
        void from(const Vector3f * axis);

        /** 分解到三个轴
        @version NIIEngine 3.0.0
        */
        void to(Vector3f * axis) const;

        /** 从三个轴构建
        @version NIIEngine 3.0.0
        */
        void from(const Vector3f & x, const Vector3f & y, const Vector3f & z);

        /** 分解到三个轴
        @version NIIEngine 3.0.0
        */
        void to(Vector3f & x, Vector3f & y, Vector3f & z) const;

        /** 构建一个四元组(轴旋转)
        @version NIIEngine 3.0.0
        */
        void from(const Radian & x, const Radian & y, const Radian & z);

        /** 获取x轴方向
        @version NIIEngine 3.0.0
        */
        Vector3f getXAxis() const;

        /** 获取y轴方向
        @version NIIEngine 3.0.0
        */
        Vector3f getYAxis() const;

        /** 获取z轴方向
        @version NIIEngine 3.0.0
        */
        Vector3f getZAxis() const;

        /** 单位化
        @return 原 lengthSquared 结果
        @version NIIEngine 3.0.0
        */
        NIIf normalise();

        /** 倒
        @note 适合非零四元数
        @version NIIEngine 3.0.0
        */
        Quaternion inverse() const;

        /** 倒
        @note 适合已经是单位化的四元组
        @version NIIEngine 3.0.0
        */
        Quaternion inverseUnit() const;

        /** 返回四元数的叉积
        @version NIIEngine 3.0.0
        */
        NIIf dot(const Quaternion & o) const;

        /** 叉积的根
        @version NIIEngine 3.0.0
        */
        NIIf length() const;

        /** 叉积
        @version NIIEngine 3.0.0
        */
        NIIf lengthSquared() const;

        /** 指数
        @remark q = (0, theta * v) exp(Q) = (cos(theta), sin(theta) * v)
        @version NIIEngine 3.0.0
        */
        Quaternion Exp() const;

        /** 对数
        @remark q = (cos(theta), sin(theta) * v) ln(Q) = (0, theta * v)
        @version NIIEngine 3.0.0
        */
        Quaternion Ln() const;

        /** 获取 x 偏转
        @param[in] prjlocal
        @return 最短距离
        @version NIIEngine 3.0.0
        */
        Radian getRoll(bool prjlocal = true) const;

        /** 获取 y 偏转
        @param[in] prjlocal
        @return 最短距离
        @version NIIEngine 3.0.0
        */
        Radian getPitch(bool prjlocal = true) const;

        /** 获取 z 偏转
        @param[in] prjlocal
        @return 最短距离
        @version NIIEngine 3.0.0
        */
        Radian getYaw(bool prjlocal = true) const;

        /** 值是否有效
        @version NIIEngine 3.0.0
        */
        bool isNaN() const;

        static const Quaternion ZERO;
        static const Quaternion IDENTITY;
    public:
        NIIf w, x, y, z;
    };

    inline _EngineAPI Ntostream & operator <<(Ntostream & o, const Quaternion & q)
    {
        o << "Quaternion(" << q.w << "," << q.x << "," << q.y << "," << q.z << ")";
        return o;
    }
}
#endif