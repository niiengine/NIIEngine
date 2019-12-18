/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

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

#ifndef _NII_RELPOS_H_
#define _NII_RELPOS_H_

#include "NiiPreInclude.h"
#include "NiiRect.h"

namespace NII
{
    /** 相对坐标中的位置
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RelPos
    {
    public:
        RelPos(){}

        RelPos(NIIf scale, NIIf oft):
            mScale(scale),
            mOffset(oft)
        {
        }

        RelPos(const RelPos & o):
            mScale(o.mScale),
            mOffset(o.mOffset)
        {
        }

		inline bool operator ==(NIIf num) const
		{
			return mScale == num && mOffset == num;
		}

        inline bool operator ==(const RelPos & o) const
        {
            return mScale == o.mScale && mOffset == o.mOffset;
        }

        inline bool operator !=(const RelPos & o) const
        {
            return mScale != o.mScale || mOffset != o.mOffset;
        }

        inline RelPos operator +(const RelPos & o) const
        {
            return RelPos(mScale + o.mScale, mOffset + o.mOffset);
        }

        inline RelPos operator -(const RelPos & o) const
        {
            return RelPos(mScale - o.mScale, mOffset - o.mOffset);
        }

        inline RelPos operator *(const RelPos & o) const
        {
            return RelPos(mScale * o.mScale, mOffset * o.mOffset);
        }

        inline RelPos operator /(const RelPos & o) const
        {
            return RelPos(o.mScale == 0.0f ? 0.0f : mScale / o.mScale,
                o.mOffset == 0.0f ? 0.0f : mOffset / o.mOffset);
        }

        inline const RelPos & operator +=(const RelPos & o)
        {
            mScale += o.mScale;
            mOffset += o.mOffset;
            return *this;
        }

        inline const RelPos & operator -=(const RelPos & o)
        {
            mScale -= o.mScale;
            mOffset -= o.mOffset;
            return *this;
        }

        inline const RelPos & operator *=(const RelPos & o)
        {
            mScale *= o.mScale;
            mOffset *= o.mOffset;
            return *this;
        }

        inline const RelPos & operator /=(const RelPos & o)
        {
            mScale = (o.mScale == 0.0f ? 0.0f : mScale / o.mScale);
            mOffset = (o.mOffset == 0.0f ? 0.0f : mOffset / o.mOffset);
            return *this;
        }

        inline RelPos operator +(NIIf val) const
        {
            return RelPos(mScale, mOffset + val);
        }

        friend inline RelPos operator +(NIIf val, const RelPos & o)
        {
            return RelPos(o.mScale, val + o.mOffset);
        }

        inline RelPos operator -(NIIf val) const
        {
            return RelPos(mScale, mOffset - val);
        }

        friend inline RelPos operator -(NIIf val, const RelPos & o)
        {
            return RelPos(o.mScale, val - o.mOffset);
        }

        inline RelPos operator *(NIIf val) const
        {
            return RelPos(mScale * val, mOffset * val);
        }

        friend inline RelPos operator *(NIIf val, const RelPos & o)
        {
            return RelPos(val * o.mScale, val * o.mOffset);
        }

        inline RelPos operator /(NIIf val) const
        {
            return val == 0.0f ? RelPos(0.0f, 0.0f) : RelPos( mScale / val, mOffset / val);
        }

        friend inline RelPos operator /(NIIf val, const RelPos & u)
        {
            return RelPos(u.mScale == 0.0f ? 0.0f : val / u.mScale, u.mOffset == 0.0f ? 0.0f : val / u.mOffset);
        }

        inline NIIf abs(NIIf base, bool pixel = true) const
        {
            return pixel ? N_ToPixel(base * mScale + mOffset) : base * mScale + mOffset;
        }
    public:
        NIIf mScale;
        NIIf mOffset;
    };
    inline _EngineAPI std::ostream & operator << (std::ostream & s, const RelPos & v)
    {
        s << "RelPos(" << v.mScale << "," << v.mOffset << ")";
        return s;
    }
    typedef TRect<RelPos> RelRect;
}
#endif