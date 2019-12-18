/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2016-3-15

�ı�����: utf-8

������˾: �������ǿƼ����޹�˾

�������: ����������

��̷��: ͳ��ʽ

����ģʽ: �ֲ�ʽ

�ڲ��ɷ�: UI���� ������� ��Ƶ���� ������� �¼���������(��ɢ�����)

��Ҫ�ɷ�: c++(80%) c(20%)

��;: ����ϵͳ����(��Χ����ϵͳ�ں�api)
      ��άӦ�����
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_AnimationDataValue_H
#define _NII_AnimationDataValue_H

#include "NiiPreInclude.h"
#include "NiiDataEquation.h"

namespace NII
{
    /** �������ȿ�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationDataValue : public DataValue<NIIf>
    {
    public:
        AnimationDataValue(AnimationFusion * target);

        ~AnimationDataValue();

        /** ��ȡ����ֵ
        @version NIIEngine 3.0.0
        */
        const NIIf getValue() const;

        /** ���ý���ֵ
        @version NIIEngine 3.0.0
        */
        void setValue(const NIIf & value);
    protected:
        AnimationFusion * mTargetFusion;
    };
}

#endif