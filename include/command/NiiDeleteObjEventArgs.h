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

#ifndef _NII_DELETE_OBJ_EVENT_ARGS_H_
#define _NII_DELETE_OBJ_EVENT_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiMechanismArgs.h"

namespace NII
{
namespace NII_COMMAND
{
    /** ɾ��һ����Ϸ������Ҫ�Ĳ���
    @remark �����ͳ��,�������麯��
    */
    class _EngineAPI DeleteObjEventArgs : public MechanismArgs
    {
    public:
        DeleteObjEventArgs(const CommandObj & co, const EventObj & eo)��
			MechanismArgs(co, eo) {}
        ~DeleteObjEventArgs() {}
    };
}
}    
#endif