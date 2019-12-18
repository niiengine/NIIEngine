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
#ifndef _NII_RADIAN_H_
#define _NII_RADIAN_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ����
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