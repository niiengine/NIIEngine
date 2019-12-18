/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2015-7-22

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

#ifndef _NII_VERTEX_KEYFRAME_TRACK_H_
#define _NII_VERTEX_KEYFRAME_TRACK_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrameTrack.h"

namespace NII
{
    /** ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VertexKeyFrameTrack : public KeyFrameTrack
    {
    public:
        VertexKeyFrameTrack(const Animation * o, KeyFrameTrackID id);
        VertexKeyFrameTrack(const Animation * o, KeyFrameTrackID id,
            VertexData * target, CalcMode mode = M_CPU);
            
        virtual ~VertexKeyFrameTrack();
    
        /** Ӧ�û��
        @version NIIEngine 3.0.0
        */
        virtual void apply(AnimationFusion * dst, VertexData * target, TrackTime * tt, NIIf weight = 1.0) const = 0;
        
        /** �Ƿ��϶��㷨��
        @version NIIEngine 3.0.0
        */
        virtual bool isNromalsFusion() const = 0;
        
        /** ����Ŀ����ʵ��
        @param[in] data
        @version NIIEngine 3.0.0
        */
        void setTarget(VertexData * obj);

        /** ��ȡĿ����ʵ��
        @return
        @version NIIEngine 3.0.0
        */
        VertexData * getTarget() const;
        
        /** ����Ŀ��ģʽ
        @param[in] m ģʽ
        @version NIIEngine 3.0.0
        */
        void setMode(CalcMode m);

        /** ���ȡĿ��ģʽ
        @version NIIEngine 3.0.0
        */
        CalcMode getMode() const;
    protected:
        VertexData * mTarget;
        CalcMode mMode;
    };
}
#endif