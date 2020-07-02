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

#ifndef _NII_KEYBOARD_CONTROLPATTERN_H_
#define _NII_KEYBOARD_CONTROLPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiControlPattern.h"
#include "NiiKeyBoardControlItem.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 外部设备控制处理器类(驱动)
    @remark 使用的是104标准的键盘设计
    @note
        由于大多系统内核读取键盘记录是以队列缓存的,所以为了更好的设计编程模块不应该
        再使用队列缓存模式
    @version NIIEngine 3.0.0 高级api
    */
    class _EngineAPI KeyboardControlPattern : public ControlPattern
    {
    public:
        /** 打开指定键盘锁
        @remark 属于内部方法级别,明确调用会改变键盘状态
        @param[in] type 启用类型
        @version NIIEngine 3.0.0
        */
        void on(KeyboardAlterLockType type);

        /** 关闭指定键盘锁
        @remark 属于内部方法级别,明确调用会改变键盘状态
        @paramp[in] type 启用类型
        @version NIIEngine 3.0.0
        */
        void off(KeyboardAlterLockType type);

        /** 同步目标对象状态
        @remark 同步 capslock/scrolllock/numlock 等状态
        @version NIIEngine 3.0.0 高级api
        */
        virtual void syn(KeyboardAlterLockType type);

        /** 指定按键是否被按下
        @remark 异步状态
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        bool isPress(KeyValue key);

        /** 指定按键是否已释放
        @remark 异步状态
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        bool isRelease(KeyValue key);

        /** 获取实际按键字符文本
        @remark Unicode编码
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        virtual Nui32 getKeyCode(KeyValue key);

        /** 获取实际按键字符描述
        @remark
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0
        */
        virtual String getKeyText(KeyValue key);

        ///@copedetails ControlPattern::getType
        ControlDevType getType() const;
    protected:
        /** 默认构造函数
        @remark 使用protected类型,防止滥用
        */
        KeyboardControlPattern();

        /** 默认构造函数
        @remark 使用protected类型,防止滥用
        @param[in] id
            控制器的唯一标识,所有控制器中唯一(包含同类型,和不同类型的概念)
        @param[in] own 这个控制属于哪个窗体的
        */
        KeyboardControlPattern(Nid id, const ViewWindow * own);

        virtual ~KeyboardControlPattern();

        /** 激活按下编码
        @remark 这个方法引发按键事件,
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0 高级api
        */
        void press(const KeyValue key);

        /** 释放按下编码
        @remark 这个方法引发按键事件
        @param[in] key 按键(使用标识)
        @version NIIEngine 3.0.0 高级api
        */
        void release(const KeyValue key);

        /// @copydetails ControlPattern::apply
        void apply(ControlItem * item);

        /// @copydetails ControlPattern::detach
        void detach();
    protected:
        KeyboardControlItem * mTarget;              ///< 当前对焦的控制单元
        Nui32 mBuffer[8];                           ///< 按键缓存
        Nui8 mModifiers;                            ///< 复码模式
    };

    /** 傀儡
    @version NIIEngine 3.0.0
    */
    class DummyKeyboardControlPattern : public KeyboardControlPattern
    {
    public:
        DummyKeyboardControlPattern() {}

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