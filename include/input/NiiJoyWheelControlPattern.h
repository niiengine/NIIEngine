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
    /** 游戏摇杆类控制器
    @remark 包含飞机摇杆概念
    @note 
        由于大多系统内核读取摇杆记录是以队列缓存的,所以为了更好的设计编程模块不应该
        再使用队列缓存模式
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI JoyWheelControlPattern : public JoyPadControlPattern
    {
    protected:
        JoyWheelControlPattern();

        JoyWheelControlPattern(Nid id, const ViewWindow * own);

        virtual ~JoyWheelControlPattern();

        /** 滚动摇盘
        @remark 这个方法引发划块事件
        @param[in] num
        */
        void wheel(NIIf num);

        /** 滚动摇盘
        @remark 这个方法引滚动摇盘事件,使用最近一次设置的数据缓存
        */
        void wheel();

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        NIIf mWheel;                    ///< 摇盘值
        JoyWheelControlItem * mTarget;    ///< 当前对焦的控制单元
    };

    /** 傀儡控制器
    @remark 用于对象完整性，或全局性
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