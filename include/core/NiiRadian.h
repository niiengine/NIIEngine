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
#ifndef _NII_RADIAN_H_
#define _NII_RADIAN_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 弧度
    @remark [0, 2pai]
    @version NIIEngine 3.0.0
    */
    class Radian
    {
    public:
        Radian() {}
        explicit Radian(NIIf f);
        Radian(const Angle & a);

        Radian & operator = (const NIIf & f)    { mValue = f; return *this; }
        Radian & operator = (const Radian & r)  { mValue = r.mValue; return *this; }
        Radian & operator = (const Angle & a);

        NIIf getAngle() const;

        bool operator <  (const Radian & r) const { return mValue <  r.mValue; }
        bool operator <= (const Radian & r) const { return mValue <= r.mValue; }
        bool operator == (const Radian & r) const { return mValue == r.mValue; }
        bool operator != (const Radian & r) const { return mValue != r.mValue; }
        bool operator >= (const Radian & r) const { return mValue >= r.mValue; }
        bool operator >  (const Radian & r) const { return mValue >  r.mValue; }

        Radian operator + () const                { return Radian(mValue); }
        Radian operator - () const                { return Radian(-mValue); }

        Radian operator + (const Radian & r) const{ return Radian(mValue + r.mValue); }
        Radian operator - (const Radian & r) const{ return Radian(mValue - r.mValue); }

        Radian operator + (const Angle * a) const;
        Radian operator - (const Angle * a) const;

        Radian operator * (NIIf f) const          { return Radian(mValue * f); }
        Radian operator / (NIIf f) const          { return Radian (mValue / f); }
        Radian & operator *= (NIIf f)             { mValue *= f; return *this; }
        Radian & operator /= (NIIf f)             { mValue /= f; return *this; }

        _EngineAPI friend Radian operator * (NIIf f, const Radian & r){ return Radian(f * r.mValue);}
        _EngineAPI friend Radian operator / (NIIf f, const Radian & r){ return Radian(f / r.mValue); }

        Radian operator * (const Radian & r) const{ return Radian(mValue * r.mValue); }
        Radian operator / (const Radian & r) const{ return Radian(mValue / r.mValue); }

        Radian & operator +=(const Radian & r)    { mValue += r.mValue; return *this; }
        Radian & operator -= (const Radian & r)   { mValue -= r.mValue; return *this; }
        Radian & operator += (const Angle * a);
        Radian & operator -= (const Angle * a);

        inline NIIf getValue() const { return mValue; }
    public:
        NIIf mValue;
    };

    inline _EngineAPI std::ostream & operator << (std::ostream & o, const Radian & v)
    {
        o << "Radian(" << v.getValue() << ")";
        return o;
    }
}
#endif