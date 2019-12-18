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

#ifndef _NII_JOYHAND_CONTROL_PATTERN_H_
#define _NII_JOYHAND_CONTROL_PATTERN_H_

#include "NiiPreInclude.h"
#include "NiiJoyStickControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    /** ��Ϸ�ְ����Ϳ�����
    @remark ������������Ϸ�Ѹ���
    @note
        ���ڴ��ϵͳ�ں˶�ȡ�ְѼ�¼���Զ��л����,����Ϊ�˸��õ���Ʊ��ģ�鲻Ӧ��
        ��ʹ�ö��л���ģʽ
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI JoyHandleControlPattern : public JoyStickControlPattern
    {
    public:
        virtual ~JoyHandleControlPattern();

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copydetails ControlItem::getType
        ControlDevType getType() const;
    protected:
		JoyHandleControlPattern();

		JoyHandleControlPattern(Nid id, const ViewWindow * own);

		void handle(NIIi v, NIIf x, NIIf y, NIIf z);
    };

	/** ���ܿ�����
	@version NIIEngine 3.0.0
	*/
	class DummyJoyHandleControlPattern : public JoyHandleControlPattern
	{
	public:
		DummyJoyHandleControlPattern() {}

		///@copydetails ControlPattern::_init
		bool _init() { return true; }

		///@copydetails ControlPattern::_end
		bool _end() { return true; }

		///@copydetails ControlPattern::_update
		void _update() {}

		///@copydetails KeyboardControlPattern::_reset
		void _reset() {}

		///@copydetails KeyboardControlPattern::_start
		void _start() {}

		///@copydetails KeyboardControlPattern::_pause
		void _pause() {}

		///@copydetails KeyboardControlPattern::_stop
		void _stop() {}
	};
}
}
#endif