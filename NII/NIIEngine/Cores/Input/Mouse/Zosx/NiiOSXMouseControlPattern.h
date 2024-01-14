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

#ifndef _NII_OSX_MOUSE_CONTROLPATTERN_H_
#define _NII_OSX_MOUSE_CONTROLPATTERN_H_

#include "NiiOSXControlPatternPrereq.h"
#include "NiiMouseControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 这是一个专门为NII引擎包装的Object Oriented Input System键盘部分
    @author 冇得弹
    @mail zelda_ok@163.com
    */
    class OSXMouseControlPattern : public MouseControlPattern
    {
    public:
        /// 防止滥用
        OSXMouseControlPattern();

        /** 通过管理器类可以调用这个方法
        @param[in] main 主控制窗体(需要它的大小)
        @param[in] input 主控制设备
        @param[in] buff 是否使用缓存模式
        */
        OSXMouseControlPattern(Nid id);

        /// 这个方法把这个实例从输入管理器中移去
        ~OSXMouseControlPattern();

        /// 鼠标移动事件发生
        void onMouseMove(EventRef event);

        /// 鼠标按键按下事件发生
        void onMousePress(EventRef event);

        /// 鼠标按键释放事件发生
        void onMouseRelease(EventRef event);

        /// 鼠标滚动键事件发生
        void onMouseWheel(EventRef event);

        /// @copydetails Pattern::getQuality
        NIIb getQuality() const;

        /// @copydetails Pattern::getPerformance
        NIIb getPerformance() const;
    protected:
        /// @copydetails Pattern::_init
        bool _init();

        /// @copydetails Pattern::_update
        void _update();

        /// @copydetails Pattern::_end
        bool _end(){}

        /// @copydetials ControlPattern::_reset
        void _reset();

        /// @copydetails ControlPattern::_start
        void _start();

        /// @copydetails ControlPattern::_pause
        void _pause();

        /// @copyetails ControlPattern::_stop
        void _stop();

        /// @copydetails Pattern::getPID
        PatternID getPID() const;
    private:
        Ni32 mTempX;                                ///<
        Ni32 mTempY;                                ///<
        Ni32 mTempZ;                                ///<
        WindowRef mWin;                             ///<
        EventHandlerUPP mMouseMove;                 ///<
        EventHandlerUPP mMouseRelease;              ///<
        EventHandlerUPP mMousePress;                ///<
        EventHandlerUPP mMouseWheel;                ///<
		EventHandlerUPP mWindowFocus;               ///<
        EventHandlerRef mMouseMoveRef;              ///<
        EventHandlerRef mMousePressRef;             ///<
        EventHandlerRef mMouseReleaseRef;           ///<
        EventHandlerRef mMouseWheelRef;             ///<
		EventHandlerRef mWindowFocusEventRef;       ///<

		bool mFocusChange;                          ///<
		bool mRelationChange;                       ///<
    };
}
}
#endif