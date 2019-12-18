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

#ifndef NII_JOYwHEEL_CONTROLPATTERN_H
#define NII_JOYwHEEL_CONTROLPATTERN_H

#include "NiiPreInclude.h"
#include "NiiJoyPadControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    /** ��Ϸҡ���������
    @remark �����ɻ�ҡ�˸���
    @note 
        ���ڴ��ϵͳ�ں˶�ȡҡ�˼�¼���Զ��л����,����Ϊ�˸��õ���Ʊ��ģ�鲻Ӧ��
        ��ʹ�ö��л���ģʽ
    @version NIIEngine 3.0.0 �߼�api
    */
    class _EngineAPI JoyWheelControlPattern : public JoyPadControlPattern
    {
    protected:
		JoyWheelControlPattern();

		JoyWheelControlPattern(Nid id, const ViewWindow * own);

        virtual ~JoyWheelControlPattern();

        /** ����ҡ��
        @remark ����������������¼�
        @param[in] num
        */
        void wheel(NIIf num);

        /** ����ҡ��
        @remark �������������ҡ���¼�,ʹ�����һ�����õ����ݻ���
        */
        void wheel();

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
		NIIf mWheel;					///< ҡ��ֵ
        JoyWheelControlItem * mTarget;	///< ��ǰ�Խ��Ŀ��Ƶ�Ԫ
    };

	/** ���ܿ�����
	@remark ���ڶ��������ԣ���ȫ����
	@version NIIEngine 3.0.0
	*/
	class DummyJoyWheelControlPattern : public JoyWheelControlPattern
	{
	public:
		DummyJoyWheelControlPattern() {}

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