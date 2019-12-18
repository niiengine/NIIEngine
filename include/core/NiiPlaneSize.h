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
#ifndef _NII_PLANE_SIZE_H_
#define _NII_PLANE_SIZE_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"
#include "NiiException.h"

namespace NII
{
    /** 描述在平面中存在的大小
    @version NIIEngine 3.0.0
    */
    template<typename T> class PlaneSize
    {
    public:
        PlaneSize(){}

        PlaneSize(T w, T h) : mWidth(w), mHeight(h){}

        PlaneSize(const PlaneSize<T> & o) : mWidth(o.mWidth), mHeight(o.mHeight){}

        inline bool operator==(const PlaneSize<T> & o) const
        {
            return mWidth == o.mWidth && mHeight == o.mHeight;
        }

        inline bool operator!=(const PlaneSize<T> & o) const
        {
            return !operator==(o);
        }

		inline PlaneSize<T> operator+(const T & c) const
		{
			return PlaneSize<T>(mWidth + c, mHeight + c);
		}

		inline PlaneSize<T> operator-(const T & c) const
		{
			return PlaneSize<T>(mWidth - c, mHeight - c);
		}

        inline PlaneSize<T> operator*(const T & c) const
        {
            return PlaneSize<T>(mWidth * c, mHeight * c);
        }

		inline PlaneSize<T> operator/(const T & c) const
		{
			return PlaneSize<T>(mWidth / c, mHeight / c);
		}

        inline PlaneSize<T> operator+(const PlaneSize<T> & s) const
        {
            return PlaneSize<T>(mWidth + s.mWidth, mHeight + s.mHeight);
        }

        inline PlaneSize<T> operator-(const PlaneSize<T> & s) const
        {
            return PlaneSize<T>(mWidth - s.mWidth, mHeight - s.mHeight);
        }

		inline PlaneSize<T> operator*(const PlaneSize<T> & s) const
		{
			return PlaneSize<T>(mWidth * s.mWidth, mHeight * s.mHeight);
		}

		inline PlaneSize<T> operator/(const PlaneSize<T> & s) const
		{
			return PlaneSize<T>(s.mWidth == 0 ? mWidth : mWidth / s.mWidth,
				s.mHeight == 0 ? mHeight : mHeight / s.mHeight);
		}

		friend inline PlaneSize<T> operator*(const T & c, const PlaneSize<T> & s)
		{
			return PlaneSize<T>(c * s.mWidth, c * s.mHeight);
		}

		friend inline PlaneSize<T> operator/(const T & c, const PlaneSize<T> & s)
		{
			return PlaneSize<T>(s.mWidth == 0 ？c : c / s.mWidth, s.mHeight == 0 ？c : c / s.mHeight);
		}

        inline void clamp(const PlaneSize<T> & min, const PlaneSize<T> & max)
        {
            N_assert(min.mWidth <= max.mWidth, "error");
            N_assert(min.mHeight <= max.mHeight, "error");

            if(mWidth < min.mWidth)
                mWidth = min.mWidth;
            else if(mWidth > max.mWidth)
                mWidth = max.mWidth;

            if(mHeight < min.mHeight)
                mHeight = min.mHeight;
            else if(mHeight > max.mHeight)
                mHeight = max.mHeight;
        }

        void scale(AspectMode mode, NIIf ratio)
        {
            N_assert(ratio > 0, "error");

            if(mode == AM_None || (mWidth <= 0 && mHeight <= 0))
                return;

            T factor;
            switch(mode)
            {
            case AM_Shrink:
            {
                factor = mHeight * ratio;
                if (factor <= mWidth)
                {
                    mWidth = factor;
                }
                else
                {
                    mHeight = mWidth / ratio;
                }
                break;
            }
            case AM_Stretch:
            {
                factor = mWidth * ratio;
                if (factor <= mHeight)
                {
                    mHeight = factor;
                }
                else
                {
                    mWidth = mHeight / ratio;
                }
                break;
            }
            default:
            {
                factor = mHeight * ratio;
                if (factor >= mWidth)
                {
                    mWidth = factor;
                }
                else
                {
                    mHeight = mWidth / ratio;
                }
                break;
            }
            }
        }
    public:
        T mWidth;
        T mHeight;
    };
    
    template<typename T> std::ostream & operator << (std::ostream & st, const PlaneSize<T> & s)
    {
        st << "PlaneSize(" << s.mWidth << "," << s.mHeight << ")";
        return st;
    }

    typedef PlaneSize<NIIf> PlaneSizef;
    typedef PlaneSize<NIIi> PlaneSizei;
}
#endif