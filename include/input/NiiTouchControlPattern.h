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

#ifndef _NII_TOUCH_CONTROLPATTERN_H_
#define _NII_TOUCH_CONTROLPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiControlPattern.h"
#include "NiiTouchControlItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** ����������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TouchControlPattern : public ControlPattern
    {
    public:
        TouchControlPattern();

        TouchControlPattern(Nid id, const ViewWindow * main);

        virtual ~TouchControlPattern();

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        /// @copydetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        TouchControlItem * mTarget;        ///< ��ǰ�Խ��Ŀ��Ƶ�Ԫ(��Ӧ���ڸ������г��ֵ�,��������������)
    };

    /** ����������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DummyTouchControlPattern : public TouchControlPattern
    {
    public:
        DummyTouchControlPattern() {}

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