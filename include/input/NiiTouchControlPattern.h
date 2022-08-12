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

#ifndef _NII_TOUCH_CONTROLPATTERN_H_
#define _NII_TOUCH_CONTROLPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiControlPattern.h"
#include "NiiTouchControlArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TouchControlItem : public ControlItem
    {
    public:
        TouchControlItem();

        /** ������ʱ�򴥷�
        @param[in] args �����¼�����
        */
        virtual void onPress(const TouchControlArgs * args);

        /** �������ͷ�ʱ����
        @param[in] args �����¼�����
        */
        virtual void onRelease(const TouchControlArgs * args);

        /** �����º��ƶ�ʱ����
        @param[in] args �����¼�����
        */
        virtual void onMove(const TouchControlArgs * args);

        /** ��ȡ��ʱ����
        @param[in] args �����¼�����
        */
        virtual void onCancel(const TouchControlArgs * args);

        /// @copydetails ControlItem::getType
        ControlDevType getType() const;
    protected:
        /// @copydetails ControlItem::raise
        void raise(const EventArgs * arg);
    };

    /// ����
    class DummyMultiTouchControlItem : public TouchControlItem, public ControlAlloc
    {
    public:
        DummyMultiTouchControlItem() {}
        ~DummyMultiTouchControlItem() {}
    };

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