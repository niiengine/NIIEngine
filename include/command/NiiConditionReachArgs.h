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

#ifndef _NII_CONDITION_REACH_ARGS_H_
#define _NII_CONDITION_REACH_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"
#include "NiiCondition.h"

namespace NII
{
namespace NII_COMMAND
{
    class _EngineAPI ConditionReachArgs : public EventArgs
    {
    public:
		ConditionReachArgs(const Condition * co) : mCondition(co){}
        ~ConditionReachArgs(){}
        
        const Condition * mCondition;
    };
}
}

#endif