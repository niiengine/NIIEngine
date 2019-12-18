/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-9-1

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

#ifndef _NII_UNIT_KEY_FRAME_H_
#define _NII_UNIT_KEY_FRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"
#include "NiiFusionData.h"

namespace NII
{
    /** ��ֵ�͵�ר�ùؼ�֡.
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UnitKeyFrame : public KeyFrame
    {
    public:
        /** ֡����ģʽ
        @version NIIEngine 3.0.0
        */
        enum Mode
        {
            M_Constant,     ///< ������ֵ��
            M_Point,        ///< ֡�����
            M_Square,       ///< ������ʽ����
            M_SquareRoot    ///< ������ʽ����
        };

        UnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos);
        UnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos, Mode mode, 
            const FusionData & value);
        virtual ~UnitKeyFrame();

        /** ���ò�ֵ����ģʽ
        @version NIIEngine 3.0.0
        */
        void setMode(Mode p);

        /** ��ȡ��ֵ����ģʽ
        @version NIIEngine 3.0.0
        */
        Mode getMode() const;

        /** ��������ؼ�֡����ֵ
        @remark ���еĹؼ�ֵ֡������һ�µ���������
        @version NIIEngine 3.0.0
        */
        void setValue(const FusionData & val);

        /** ��ȡ����ؼ�֡����ֵ
        @remark ���еĹؼ�ֵ֡������һ�µ���������
        @version NIIEngine 3.0.0
        */
        const FusionData & getValue() const;

        /** ��ȡ֡����ģʽ����
        @param[in] pos[0, 1]
        @return ����ģʽ���[0, 1]
        @version NIIEngine 3.0.0
        */
        NIIf getFactor(NIIf pos);
    protected:
        /// @copydetails KeyFrame::clone
        virtual KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        Mode mMode;
        FusionData mValue;
    };
}

#endif