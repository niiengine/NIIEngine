/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-9-21

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

#ifndef _NII_PROPERTY_ID_KEYFRAME_H_
#define _NII_PROPERTY_ID_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiUnitKeyFrame.h"

namespace NII
{
    /** �������Թ����ؼ�֡
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PropertyKeyFrame : public UnitKeyFrame
    {
    public:
        PropertyKeyFrame(const KeyFrameTrack * o, TimeDurMS pos);
        PropertyKeyFrame(const KeyFrameTrack * o, TimeDurMS pos, Mode mode,
            const FusionData & value, PropertyID pid = 0);
        ~PropertyKeyFrame();

        /** ���ö�������ֵΪ��ϵ�ֵ
        @version NIIEngine 3.0.0
        */
        void setPropertyValue(PropertyFusion * obj);

        /** ��ȡ�������Ի��ʹ�õ�ֵ
        @version NIIEngine 3.0.0
        */
        const FusionData & getPropertyValue(PropertyFusion * obj) const;

        /** ����ֵ��Դ�Ķ�������
        @version NIIEngine 3.0.0
        */
        void setProperty(PropertyID pid);

        /** ��ȡֵ��Դ�Ķ�������
        @version NIIEngine 3.0.0
        */
        PropertyID getProperty() const;
    protected:
        /// @copydetails KeyFrame::clone
        virtual KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        PropertyID mProperty;
    };
}
#endif