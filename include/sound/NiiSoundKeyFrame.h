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

#ifndef _NII_SOUND_KEYFRAME_H_
#define _NII_SOUND_KEYFRAME_H_

#include "NiiPreInclude.h"
#include "NiiKeyFrame.h"

namespace NII
{
    /** �ڵ�ר�ùؼ�֡
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SoundKeyFrame : public KeyFrame
    {
    public:
        SoundKeyFrame(const KeyFrameTrack * own, TimeDurMS pos);
        ~SoundKeyFrame();

        SoundKeyFrame & operator = (const SoundKeyFrame & o);
        
        /** �����Դ
        @version NIIEngine 3.0.0
        */
        void addResource(ResourceID rid);

        /** ��ȥ��Դ
        @version NIIEngine 3.0.0
        */
        void removeResource(ResourceID rid);

        /** ��ȡʹ�õ���Դ
        @version NIIEngine 3.0.0
        */
        const ResourceIDList & getList() const;
        
        /** ����
        @version NIIEngine 3.0.0
        */
        void play(SpaceNode * node);
    protected:
        KeyFrame * clone(const KeyFrameTrack * o) const;
    protected:
        ResourceIDList mResourceList;
    };
}
#endif