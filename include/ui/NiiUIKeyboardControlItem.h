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

#ifndef _NII_UI_KEYBOARD_CONTROL_ITEM_H_
#define _NII_UI_KEYBOARD_CONTROL_ITEM_H_

#include "NiiUIPreInclude.h"
#include "NiiKeyBoardControlItem.h"

namespace NII
{
namespace UI
{
    /** UI��Ԫר�õļ��̿��Ƶ�Ԫ
    @remark 
        ��UI��ϵ�о����ѻ���ת�����������ʹ��,�ܻ�ȡ����Ĺ���
    */
    class _EngineAPI KeyBoardControlItem : public NII_MEDIA::KeyboardControlItem
    {
    public:
		KeyBoardControlItem();
        virtual ~KeyBoardControlItem();
    };
}
}
#endif