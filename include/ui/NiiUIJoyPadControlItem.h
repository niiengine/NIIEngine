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

#ifndef _NII_UI_JOY_PAD_CONTROL_ITEM_H_
#define _NII_UI_JOY_PAD_CONTROL_ITEM_H_

#include "NiiUIPreInclude.h"
#include "NiiJoyPadControlItem.h"

namespace NII
{
namespace UI
{
    /** UI��Ԫר�õ��ֱ����Ƶ�Ԫ
    @remark
        ��UI��ϵ�о����ѻ���ת�����������ʹ��,�ܻ�ȡ����Ĺ���
    */
    class _EngineAPI JoyPadControlItem : public NII_MEDIA::JoyPadControlItem
    {
    public:
		JoyPadControlItem();
        virtual ~JoyPadControlItem();
    };
}
}
#endif