/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-20

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
#ifndef _NII_ANGLE_H_
#define _NII_ANGLE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 角度
    @remark [0, 360]
    @version NIIEngine 3.0.0
    */
    class Angle
    {
    public:
        explicit Angle(NIIf f);
        Angle(const Radian & r);

        Angle & operator = (const NIIf & f)     { mValue = f; return *this; }
        Angle & operator = (const Angle & a)    { mValue = a.mValue; return *this; }
        Angle & operator = (const Radian & r);

        NIIf getRadian() const;

        bool operator <  (const Angle & a) const { return mValue <  a.mValue; }
        bool operator <= (const Angle & a) const { return mValue <= a.mValue; }
        bool operator == (const Angle & a) const { return mValue == a.mValue; }
        bool operator != (const Angle & a) const { return mValue != a.mValue; }
        bool operator >= (const Angle & a) const { return mValue >= a.mValue; }
        bool operator >  (const Angle & a) const { return mValue >  a.mValue; }

        Angle operator + () const                { return Angle(mValue); }
        Angle operator - () const                { return Angle(-mValue); }

        Angle operator + (const Angle & a) const { return Angle (mValue + a.mValue); }
        Angle operator + (const Radian * r) const;
        Angle operator - (const Angle & a) const { return Angle(mValue - a.mValue); }
        Angle operator - (const Radian * r) const;

        _EngineAPI friend Angle operator * (NIIf a, const Angle & b){ return Angle (a * b.mValue); }
        _EngineAPI friend Angle operator / (NIIf a, const Angle & b){ return Angle (a / b.mValue); }

        Angle operator * (NIIf f) const          { return Angle(mValue * f); }
        Angle operator / (NIIf f) const          { return Angle(mValue / f); }

        Angle & operator += (const Angle & a)    { mValue += a.mValue; return *this; }
        Angle & operator += (const Radian * r);
        Angle & operator -= (const Angle & a)    { mValue -= a.mValue; return *this; }
        Angle & operator -= (const Radian * r);
        Angle & operator *= (NIIf f)             { mValue *= f; return *this; }
        Angle & operator /= (NIIf f)             { mValue /= f; return *this; }

        Angle operator * (const Angle & a) const{ return Angle(mValue * a.mValue); }
        Angle operator / (const Angle & a) const{ return Angle(mValue / a.mValue); }

        inline NIIf getValue() const { return mValue; }
    public:
        NIIf mValue;
    };

    inline _EngineAPI std::ostream & operator << (std::ostream & o, const Angle & v)
    {
        o << "Angle(" << v.getValue() << ")";
        return o;
    }
}

#endif