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

        /** 当按下时候触发
        @param[in] args 触屏事件参数
        */
        virtual void onPress(const TouchControlArgs * args);

        /** 当按后释放时触发
        @param[in] args 触屏事件参数
        */
        virtual void onRelease(const TouchControlArgs * args);

        /** 当按下后移动时触发
        @param[in] args 触屏事件参数
        */
        virtual void onMove(const TouchControlArgs * args);

        /** 当取消时触发
        @param[in] args 触屏事件参数
        */
        virtual void onCancel(const TouchControlArgs * args);

        /// @copydetails ControlItem::getType
        ControlDevType getType() const;
    protected:
        /// @copydetails ControlItem::raise
        void raise(const EventArgs * arg);
    };

    /// 傀儡
    class DummyMultiTouchControlItem : public TouchControlItem, public ControlAlloc
    {
    public:
        DummyMultiTouchControlItem() {}
        ~DummyMultiTouchControlItem() {}
    };

    /** 触屏控制器
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
        TouchControlItem * mTarget;        ///< 当前对焦的控制单元(本应该在父对象中出现的,但存在性能问题)
    };

    /** 触屏控制器
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