/*
-----------------------------------------------------------------------------
���Ͷ�ý����

ʱ��: 2014-5-8

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

#ifndef _NII_JOYPAD_CONTROL_BUTTON_ARGS_H_
#define _NII_JOYPAD_CONTROL_BUTTON_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiJoyDevControlArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    /** �ֱ����Ʋ���
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadControlButtonArgs : public JoyDevControlArgs
    {
    public:
        JoyPadControlButtonArgs(Nui8 b) :
			JoyDevControlArgs(JDCT_Button),
			mButton(b) {}

        Nui8 mButton;                       ///< ����������
    };
}
}
#endif