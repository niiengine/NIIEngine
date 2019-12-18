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

#ifndef _NII_VERTEX_UNIT_KEYFRAME_H_
#define _NII_VERTEX_UNIT_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"
#include "NiiVertexBuffer.h"

namespace NII
{
    /** ���㵥Ԫ�ؼ�֡(ȫ).
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexUnitKeyFrame : public KeyFrame
    {
    public:
        VertexUnitKeyFrame(const KeyFrameTrack * o, TimeDurMS pos);
        ~VertexUnitKeyFrame();

        /** ���ö��㻺�����Ϊ����ؼ����λ��
        @param[in] buf ���㻺��
        @version NIIEngine 3.0.0
        */
        void setValue(VertexBuffer * buf);

        /** ��ȡ���㻺�����Ϊ����ؼ����λ��
        @version NIIEngine 3.0.0
        */
        VertexBuffer * getValue() const;
    protected:
        KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        VertexBuffer * mBuffer;
    };
}
#endif