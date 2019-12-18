/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-7

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

#ifndef _NII_BOX_H_
#define _NII_BOX_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 3D�ռ��
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Box
    {
    public:
        Box() : 
            mLeft(0), 
            mTop(0), 
            mRight(1), 
            mBottom(1), 
            mFront(0), 
            mBack(1)
        {
        }
        
        /** �������
        @remark (mFront = 0 and mBack = 1)
        @param[in] l ���
        @param[in] t �ϱ�
        @param[in] r �ұ�
        @param[in] b �±�
        */
        Box(NCount l, NCount t, NCount r, NCount b):
            mLeft(l),
            mTop(t),
            mRight(r),
            mBottom(b),
            mFront(0),
            mBack(1)
        {
        }
        
        /** �������
        @param[in] l ���
        @param[in] t �ϱ�
        @param[in] f ǰ��
        @param[in] r �ұ�
        @param[in] bt �±�
        @param[in] bb ���
        */
        Box(NCount l, NCount t, NCount f, NCount r, NCount bt, NCount bb):
            mLeft(l),
            mTop(t),
            mRight(r),
            mBottom(bt),
            mFront(f),
            mBack(bb)
        {
        }

        /** �Ƿ����
        @verison NIIEngine 3.0.0
        */
        bool contains(const Box & o) const
        {
            return (o.mLeft >= mLeft && o.mTop >= mTop && o.mFront >= mFront &&
                o.mRight <= mRight && o.mBottom <= mBottom && o.mBack <= mBack);
        }

        /** ��ȡ������ӵĿ�
        @version NIIEngine 3.0.0
        */
        NCount getWidth() const { return mRight - mLeft; }

        /** ��ȡ������ӵĸ�
        @version NIIEngine 3.0.0
        */
        NCount getHeight() const { return mBottom - mTop; }

        /** ��ȡ������ӵ���
        @version NIIEngine 3.0.0
        */
        NCount getDepth() const { return mBack - mFront; }
    public:
        NCount mLeft, mTop, mRight, mBottom, mFront, mBack;
    };
}
#endif