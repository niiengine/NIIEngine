/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-7

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
	  ��άӦ�����
		������Ϣϵͳ���(GIS)
		��Ӱ���������ع����
		������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/
#ifndef _NII_PLANE_SIZE_H_
#define _NII_PLANE_SIZE_H_

#include "NiiPreInclude.h"
#include "NiiCommon.h"
#include "NiiException.h"

namespace NII
{
    /** ������ƽ���д��ڵĴ�С
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
			return PlaneSize<T>(s.mWidth == 0 ��c : c / s.mWidth, s.mHeight == 0 ��c : c / s.mHeight);
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