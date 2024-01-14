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

#ifndef NII_OSX_KEYBOARD_CONTROL_PATTERN_H
#define NII_OSX_KEYBOARD_CONTROL_PATTERN_H

#include "NiiOSXControlPatternPrereq.h"
#include "NiiKeyboardControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    /** OSX平台的键盘控制器
    @remark 平台专用键盘控制器
    @author 冇得弹
    @mail zelda_ok@163.com
    */
    class OSXKeyboardControlPattern : public KeyboardControlPattern
    {
    public:
        enum OSXKeyboardEventType
        {
            MKET_KEYUP,
            MKET_KEYDOWN,
            MKET_KEYREPEAT,
        };
        class OSXKeyStackEvent
        {
        public:
            OSXKeyStackEvent(KeyValue key, OSXKeyboardEventType type) :
                mKey(key),
                mType(type) {}

            OSXKeyboardEventType mType;
            KeyValue mKey;                          ///< 按键的编码
        };
        typedef list<OSXKeyStackEvent>::type Events;
    public:
        /** 通过管理器类可以调用这个方法
        @param[in] id 主控制设备
        @param[in] own 是否使用缓存模式
        @param[in] repeat 键盘自动重复键入模式
        */
        OSXKeyboardControlPattern(Nid id);

        /// 这个方法把这个实例从输入管理器中移去
        ~OSXKeyboardControlPattern();

        /** 当按键按下的时候触发
        @remark 不包含键盘状态
        @param[in] event 回调事件
        */
        void onKeyDown(EventRef event);

        /** 当按键释放的时候触发
        @remark 不包含键盘状态
        @param[in] event 回调事件
        */
        void onKeyUp(EventRef event);

        /** 当键盘状态变化时候触发
        @remark 只包含键盘状态
        @param[in] event 回调事件
        */
        void onKeyMod(EventRef event);

        /// @copydetails Pattern::getQuality
        NIIb getQuality() const;

        /// @copydetails Pattern::getPerformance
        NIIb getPerformance() const;

        /// @copydetails KeyboardControlPattern::syn
        void syn(KeyboardAlterLockType type) const;
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
        /** 建立起一个设备接口
        @remark osx专有
        */
        void setupHid();

        /** 查找capslock numlock scrlock的位置
        @remark osx专有
        */
        void findCookie();
    private:
        IOHIDDeviceInterface ** mMain;      ///< 输入核心
        IOHIDElementCookie mCaps;           ///< capslock的缓存
        IOHIDElementCookie mNum;            ///< numlock的缓存
        IOHIDElementCookie mScr;            ///< scrlock的缓存
        UInt32 mPrevModMask;                ///< 前一个
        EventHandlerUPP mKeyDown;           ///< 按下回调
        EventHandlerUPP mKeyUp;             ///< 释放回调
        EventHandlerUPP mKeyMod;            ///< 滑轮回调
        EventHandlerRef mKeyDownEventRef;   ///< 回调引用
        EventHandlerRef mKeyUpEventRef;     ///< 回调引用
        EventHandlerRef mKeyModEventRef;    ///< 回调引用
        Events mEvents;                     ///< 事件队列
    };
}
}
#endif