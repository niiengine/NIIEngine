/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-5-20

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
#ifndef _NII_ANGLE_H_
#define _NII_ANGLE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** �Ƕ�
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