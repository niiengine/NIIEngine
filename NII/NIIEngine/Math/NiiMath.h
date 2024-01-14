/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/

#ifndef _NII_MATH_H_
#define _NII_MATH_H_

#include "NiiPreInclude.h"
#include "NiiRadian.h"
#include "NiiAngle.h"

namespace NII
{
    /** 三角函数
    @note 注意使用的精度,如果LOD有足够的大,或许可以使用(table)表级函数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Math
    {
    public:
        /** 默认构造函数
        @param[in] triTable sin/tan表大小
        */
        Math(NCount triTable = 4096);
        ~Math();

        /** 正切函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static inline NIIf Tan(NIIf value, bool table = false)
        {
            return table ? tanTable(value) : tanf(value);
        }

        /** 正切函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static inline NIIf Tan(const Radian & value, bool table = false)
        {
            return table ? tanTable(value.getValue()) : tan(value.getValue());
        }

        /** 正切函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static NIIf Tan16(NIIf value, bool table = false);

        /** 正切函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static inline NIId Tan64(NIId value, bool table = false)
        {
            return table ? tanTable((NIIf)value) : tan((NIId)value);
        }

        /** 正弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static inline NIIf Sin(NIIf value, bool table = false)
        {
            return table ? sinTable(value) : sinf(value);
        }

        /** 正弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static inline NIIf Sin(const Radian & value, bool table = false)
        {
            return table ? sinTable(value.getValue()) : sin(value.getValue());
        }

        /** 正弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static NIIf Sin16(NIIf value, bool table = false);

        /** 正弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static inline NIId Sin64(NIId value, bool table = false)
        {
            return table ? sinTable((NIIf)value) : sin((NIId)value);
        }

        /** 余弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static inline NIIf Cos(NIIf value, bool table = false)
        {
            return table ? sinTable(value + PI_2) : cosf(value);
        }

        /** 余弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static inline NIIf Cos(const Radian & value, bool table = false)
        {
            return table ? sinTable(value.getValue() + PI_2) : cos(value.getValue());
        }

        /** 余弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static NIIf Cos16(NIIf value, bool table = false);

        /** 余弦
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static inline NIId Cos64(NIId value, bool table = false)
        {
            return table ? sinTable((NIIf)value + PI_2) : cos((NIId)value);
        }

        /** 反余弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static Radian ACos(NIIf value);

        /** 反余弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static Radian ACos16(NIIf value);

        /** 反余弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static NIId ACos64(NIId value);

        /** 反正弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static Radian ASin(NIIf value);

        /** 反正弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static Radian ASin16(NIIf value);

        /** 反正弦函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static NIId ASin64(NIId value);

        /** 反正切函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static inline Radian ATan(NIIf value)
        {
            return Radian(atanf(value));
        }

        /** 反正切函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static Radian ATan16(NIIf value);

        /** 反正切函数
        @param[in] value 弧度
        @version NIIEngine 3.0.0
        */
        static inline NIId ATan64(NIId value)
        {
            return atan(value);
        }

        /** 反正切函数
        @version NIIEngine 3.0.0
        */
        static inline Radian ATan2(NIIf fY, NIIf fX)
        {
            return Radian(atan2f(fY,fX));
        }

        /** 反正切函数
        @version NIIEngine 3.0.0
        */
        static Radian ATan216(NIIf fY,NIIf fX);

        /** 反正切函数
        @version NIIEngine 3.0.0
        */
        static inline NIId ATan264(NIId fY,NIIf fX)
        {
            return atan2(fY,fX);
        }

        /** 平方
        @version NIIEngine 3.0.0
        */
        static inline NIIf Sqr(NIIf value)
        {
            return value * value;
        }

        /** 平方根
        @version NIIEngine 3..0.0
        */
        static inline NIIf sqrt(NIIf value)
        {
            // 会快点? more faster? RInvSqrt?
#if _DEBUG
            return sqrtf(value);
#else
            return value * invSqrt(value);
#endif
        }

        /** 平方根
        @version NIIEngine 3..0.0
        */
        static inline Radian sqrt(const Radian & value)
        {
            return Radian(sqrtf(value.getValue()));
        }

        /** 平方根
        @version NIIEngine 3..0.0
        */
        static inline Angle sqrt(const Angle & value)
        {
            return Angle(sqrtf(value.getValue()));
        }

        /** 平方根
        @version NIIEngine 3..0.0
        */
        static inline NIIf Sqrt16(NIIf value)
        {
            return value * invSqrt16(value);
        }

        /** 平方根
        @version NIIEngine 3..0.0
        */
        static inline NIId Sqrt64(NIId value)
        {
            return value * invSqrt64(value);
        }

        /** 1/sqrt(x)
        @version NIIEngine 3.0.0
        */
        static NIIf invSqrt(NIIf value, bool table = false);

        /** 1/sqrt(x)
        @version NIIEngine 3.0.0
        */
        static NIIf invSqrt16(NIIf value);

        /** 1/sqrt(x)
        @version NIIEngine 3.0.0
        */
        static NIId invSqrt64(NIId value);

        /** 1/sqrt(x)
        @note 快不准确
        @version NIIEngine 3.0.0
        */
        static NIIf RInvSqrt(NIIf value);

        /** 对数
        @version NIIEngine 3.0.0
        */
        static inline NIIf Log(NIIf value)
        {
            return logf(value);
        }

        /** 对数
        @version NIIEngine 3.0.0
        */
        static  NIIf Log16(NIIf value);

        /** 对数
        @version NIIEngine 3.0.0
        */
        static inline NIId Log64(NIId value)
        {
            return log(value);
        }

        /** 对数 / log(2)
        @version NIIEngine 3.0.0
        */
        static inline NIIf Log2(NIIf value)
        {
            return log(value) / LOG2;
        }

        /** 对数 / log(2)
        @version NIIEngine 3.0.0
        */
        static NIIi ILog2(NIIf value);

        /** 次幂
        @version NIIEngine 3.0.0
        */
        static inline NIIf Pow(NIIf base, NIIf f)
        {
            return powf(base, f);
        }

        /** 次幂
        @version NIIEngine 3.0.0
        */
        static inline NIIf Pow16(NIIf base, NIIf f)
        {
            return NIIf(Exp16(f * Log16(base)));
        }

        /** 次幂
        @version NIIEngine 3.0.0
        */
        static inline NIId Pow64(NIId base, NIIf f)
        {
            return pow(base, f);
        }

        /** e的次幂
        @version NIIEngine 3.0.0
        */
        static inline NIIf Exp(NIIf value)
        {
            return expf(value);
        }

        /** e的次幂
        @version NIIEngine 3.0.0
        */
        static NIIf Exp16(NIIf value);

        /** e的次幂
        @version NIIEngine 3.0.0
        */
        static inline NIId Exp64(NIId value)
        {
            return exp(value);
        }

        /** 绝对值
        @version NIIEngine 3.0.0
        */
        static inline NIIf Abs(NIIf value) { return fabs(value); }

        /** 绝对值
        @version NIIEngine 3.0.0
        */
        static inline NIIi IAbs(NIIi value){ return (value >= 0 ? value : -value); }

        /** 绝对值
        @version NIIEngine 3.0.0
        */
        static inline Angle Abs(const Angle & value)
        {
            return Angle(fabs(value.getValue()));
        }

        /** 绝对值
        @version NIIEngine 3.0.0
        */
        static inline Radian Abs(const Radian & value)
        {
            return Radian(fabs(value.getValue()));
        }

        /** 返回比参数大的最小整数
        @version NIIEngine 3.0.0
        */
        static inline NIIf Ceil(NIIf value)
        {
            return NIIf(ceil(value));
        }

        /** 返回比参数大的最小整数
        @version NIIEngine 3.0.0
        */
        static inline NIIi ICeil(NIIf value)
        {
            return NIIi(ceil(value));
        }

        /** 返回比参数小的最大整数
        @version NIIEngine 3.0.0
        */
        static inline NIIf Floor(NIIf value)
        {
            return NIIf(floor(value));
        }

        /** 返回比参数小的最大整数
        @version NIIEngine 3.0.0
        */
        static inline NIIi IFloor(NIIf value)
        {
            return NIIi(floor(value));
        }

        /** 符号函数
        @version NIIEngine 3.0.0
        */
        static NIIf Sign(NIIf value);

        /** 符号函数
        @version NIIEngine 3.0.0
        */
        static NIIi ISign(NIIi value);

        /** 符号函数
        @version NIIEngine 3.0.0
        */
        static inline Radian Sign(const Radian & rValue)
        {
            return Radian(Sign(rValue.getValue()));
        }

        /** 符号函数
        @version NIIEngine 3.0.0
        */
        static inline Angle Sign (const Angle & value)
        {
            return Angle(Sign(value.getValue()));
        }

        /** 复合函数
        @remark 分别求出 value 的 sin 和 cos
        @version NIIEngine 3.0.0
        */
        static void SinCos(NIIf value, NIIf & sinvalue, NIIf & cosvalue);

        /** 复合函数
        @remark 分别求出 value 的 sin 和 cos
        @version NIIEngine 3.0.0
        */
        static void SinCos16(NIIf value, NIIf & sinvalue, NIIf & cosvalue);

        /** 复合函数
        @remark 分别求出 value 的 sin 和 cos
        @version NIIEngine 3.0.0
        */
        static void SinCos64(NIId value, NIIf & sinvalue, NIIf & cosvalue);

        /** 不是一个数字
        @version NIIEngine 3.0.0
        */
        static inline bool isNaN(NIIf f)
        {
            return f != f;
        }

        /** 是以2为底的次幂
        @version NIIEngine 3.0.0
        */
        static inline bool isPowOfTwo(NIIi x)
        {
            return (x &(x - 1)) == 0 && (x > 0);
        }

        /** 两浮点是否相等
        @version NIIEngine 3.0.0
        */
        static inline bool isEqual(NIIf a, NIIf b, NIIf tolerance = FLT_EPSILON)
        {
            return (fabs(b - a) <= tolerance);
        }

        /** 随机值
        @return [0,1].
        */
        static NIIf randomf();

        /** 随机值
        @version NIIEngine 3.0.0
        */
        static NIIi randomi();

        /** 范围随机值
        @return [low, high]
        @version NIIEngine 3.0.0
        */
        static NIIf random(NIIf low, NIIf high);

        /** 范围随机值
        @return [low, high]
        @version NIIEngine 3.0.0
        */
        static NIIi randomi(NIIi low, NIIi high);

        /** 范围随机值
        @return [-1, 1]
        @version NIIEngine 3.0.0
        */
        static NIIf randomNegPos();

        static const NIIf POS_INFINITY; ///< 无限大
        static const NIIf NEG_INFINITY; ///< 无限小
        static const NIIf PI;           ///< pi
        static const NIIf PI2X;         ///< pi * 2
        static const NIIf PI_2;         ///< pi / 2
        static const NIIf PI_4;         ///< pi / 4
        static const NIIf Rad_Ang;        ///< pi / 180
        static const NIIf Ang_Rad;        ///< 180 / pi
        static const NIIf SQRT_2;        ///< sqrt(2)
        static const NIIf SQRT_3;        ///< sqrt(3)
        static const NIIf SQRT_1_2;        ///< sqrt(1/2)
        static const NIIf SQRT_1_3;        ///< sqrt(1/3)
        static const NIIf LOG2;         ///< log(2)
        static const NIIf E;            ///< 数学 e 值
    protected:
        /** 建立三角值表
        @version NIIEngine 3.0.0 Inner
        */
        void setupTriTable();

        /** 建立根值表
        @version NIIEngine 3.0.0 Inner
        */
        void setupSqrtTable();

        /** 表寻值
        @version NIIEngine 3.0.0 Inner
        */
        static NIIf sinTable(NIIf value);

        /** 表寻值
        @version NIIEngine 3.0.0 Inner
        */
        static NIIf tanTable(NIIf value);
    private:
        static NCount mTriMapSize;
        static NIIf mTriMapFactor;
        static NIIf * mSinTable;    ///< sin表
        static NIIf * mTanTable;    ///< tan表
        static Nui32 * mSqrtTable;  ///< 根值表
    };
}
#endif