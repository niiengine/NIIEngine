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

#ifndef _NII_CONTROL_DEV_TYPE_H_
#define _NII_CONTROL_DEV_TYPE_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** ����������
    @version NIIEngine 3.0.0
    */
    enum ControlDevType
    {
        CDT_Keyboard = 1,
        CDT_Mouse = 2,
        CDT_JoyStick = 3,
        CDT_JoyPad = 4,
        CDT_JoyHandle = 5,
        CDT_JoyWheel = 6,
        CDT_Touch = 7
    };
}
#endif