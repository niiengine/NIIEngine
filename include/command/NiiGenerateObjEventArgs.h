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
        �������������������(CAD)
        ������Ϣϵͳ���(GIS)
        ��Ӱ���������ع����
        ������Ϸ���

ƫ����;: ������Ϸ���

��ҳ: www.niiengine.com ��������: niiengine@gmail.com OR niiengine@163.com

��Ȩ��ʽ:��ҵ��Ȩ(www.niiengine.com/license)(3��)
------------------------------------------------------------------------------
*/

#ifndef _NII_GENERATE_OBJ_EVENT_ARGS_H_
#define _NII_GENERATE_OBJ_EVENT_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiMechanismArgs.h"

namespace NII
{
namespace NII_COMMAND
{
    /** ����һ����Ϸ������Ҫ�Ĳ���
    @remark �����ͳ��,�������麯��
    */
    class _EngineAPI GenerateObjEventArgs : public MechanismArgs
    {
    public:
		GenerateObjEventArgs(const CommandObj & co, const EventObj & eo,
            void * data, TimeDurMS vtime, bool audestroy) :
			MechanismArgs(co, eo),
			mOperatorData(data),
			mTime(vtime),
			mAutoDestroy(audestroy) {}
        ~GenerateObjEventArgs() {}

        TimeDurMS mTime;                ///< ���ɶ������������(��λ:��,0.0Ϊ���ö���)
        bool mAutoDestroy;              ///< �Ƿ��Զ�ɾ������Ҫ�����ö���
        const void * mOperatorData;     ///< ����������Ҫ�Ĳ���
    };
}
}
#endif