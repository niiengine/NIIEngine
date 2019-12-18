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

#ifndef _NII_JOYSTICK_CONTROL_POV_ARGS_H_
#define _NII_JOYSTICK_CONTROL_POV_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiJoyDevControlArgs.h"
#include "NiiPovDirection.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickControlPovArgs : public JoyDevControlArgs
    {
    public:
        JoyStickControlPovArgs(Nui8 pov, PovType dir) :
			JoyDevControlArgs(JDCT_Pov),
			mPov(pov),
			mDirection(dir) {}

        Nui8 mPov;                      ///< ����
        PovType mDirection;             ///< pov����
    };
}
}

#endif