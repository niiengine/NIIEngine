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

#ifndef _NII_TRACK_TIME_H_
#define _NII_TRACK_TIME_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** �ؼ�֡ʱ������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TrackTime
    {
    public:
        TrackTime(TimeDurMS pos);
        TrackTime(TimeDurMS pos, Nidx index, Nidx lindex);

        /** �Ƿ���Ч
        @version NIIEngine 3.0.0
        */
        bool isValid() const;

        /** ��ȡʱ��
        @version NIIEngine 3.0.0
        */
        TimeDurMS getTimePos() const;

        /** ��ȡ������ʼ�ؼ�֡
        @version NIIEngine 3.0.0
        */
        Nidx getKeyIndex() const;
        
        /** ���õ�ǰʱ��ؼ�֡
        @version NIIEngine 3.0.0
        */
        void setLocalKeyIndex(Nidx idx);
        
        /** ��ȡ��ǰʱ��ؼ�֡
        @version NIIEngine 3.0.0
        */
        Nidx getLocalKeyIndex() const;
    protected:
        TimeDurMS mTimePos;
        Nidx mKeyIndex;
        Nidx mLocalKeyIndex;
    };
}

#endif