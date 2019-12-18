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

#ifndef _NII_MOUSE_PRESS_SERIAL_ITEM_ARGS_H_
#define _NII_MOUSE_PRESS_SERIAL_ITEM_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiSerialItemArgs.h"
#include "NiiMouseButton.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MousePressSerialItemArgs : public SerialItemArgs
    {
    public:
        MousePressSerialItemArgs(MouseButton b) :
			mButton(b) {}

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * other) const 
		{
			MouseButton src = static_cast<const MousePressArgs *>(o)->mButton;
			if (mButton == src)
				return true;
			return false;
		}

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * other) const 
		{
			MouseButton src = static_cast<const MousePressArgs *>(o)->mButton;
			if (mButton == src)
				return false;
			return true;
		}

        MouseButton mButton;            ///<
    };
}
}

#endif