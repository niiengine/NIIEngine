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
        NIIf mWheel;                    ///< ҡ��ֵ
        JoyWheelControlItem * mTarget;    ///< ��ǰ�Խ��Ŀ��Ƶ�Ԫ
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