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

#ifndef _NII_RELPOS_H_
#define _NII_RELPOS_H_

#include "NiiPreInclude.h"
#include "NiiRect.h"

namespace NII
{
    /** ��������е�λ��
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