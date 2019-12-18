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

#ifndef _NII_JOYDEV_SERIAL_ITEM_ARGS_H_
#define _NII_JOYDEV_SERIAL_ITEM_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiSerialItem.h"
#include "NiiJoyDevControlType.h"
#include "NiiPovDirection.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyDevSerialItemArgs : public SerialItemArgs
    {
    public:
        JoyDevSerialItemArgs(JoyDevControlType t) :
            mType(t) {}
        virtual ~JoyDevSerialItemArgs() {}
        
        JoyDevControlType mType;   ///< ����
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadSerialButtonItemArgs : public JoyDevSerialItemArgs
    {
    public:
        JoyPadSerialButtonItemArgs(Nui8 b);

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * other) const;

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * other) const;

        Nui8 mButton;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickSerialPovItemArgs : public JoyDevSerialItemArgs
    {
    public:
        JoyStickSerialPovItemArgs(Nui8 pov, PovType dir);

        /// @copydetails ItemSerialArgs::operator ==
        bool operator == (const EventArgs * o) const;

        /// @copydetails ItemSerialArgs::operator !=
        bool operator != (const EventArgs * o) const;

        Nui8 mPov;                      ///< 
        PovType mDirection;             ///< pov����
    };
}
}
#endif