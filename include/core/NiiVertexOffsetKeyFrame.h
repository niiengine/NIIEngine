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

#ifndef _NII_VERTEX_OFFSET_KEYFRAME_H_
#define _NII_VERTEX_OFFSET_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"

namespace NII
{
    /** ����ƫ�ƹؼ�֡
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexOffsetKeyFrame : public KeyFrame
    {
        friend class VertexOffsetKeyFrameTrack;
    public:
        /** ƫ������
        @version NIIEngine 3.0.0
        */
        struct VertexOffset
        {
            Nui16 mTargetIndex;
            NIIf mWeight;
        };
        typedef vector<VertexOffset>::type Offsets;
    public:
        VertexOffsetKeyFrame(const KeyFrameTrack * own, TimeDurMS pos);
        ~VertexOffsetKeyFrame();

        /** ���ƫ�Ƽ�
        @version NIIEngine 3.0.0
        */
        void add(Nui16 index, NIIf weight);

        /** ����ƫ�Ƽ�ȫ��
        @version NIIEngine 3.0.0
        */
        void set(Nui16 index, NIIf weight);

        /** ��ȥƫ�Ƽ�
        @param[in] index mTargetIndex����
        @version NIIEngine 3.0.0
        */
        void remove(Nui16 index);

        /** ��ȥ����ƫ�Ƽ�
        @version NIIEngine 3.0.0
        */
        void removeAll();

        /** ��ȡƫ�Ƽ�
        @version NIIEngine 3.0.0 �߼�api
        */
        const Offsets & getValue() const;
    protected:
        KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        Offsets mOffsets;
    };
}
#endif