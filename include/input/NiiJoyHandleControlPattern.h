/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
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