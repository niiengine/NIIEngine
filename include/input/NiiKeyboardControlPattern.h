/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_KEYBOARD_CONTROLPATTERN_H_
#define _NII_KEYBOARD_CONTROLPATTERN_H_

#include "NiiPreInclude.h"
#include "NiiControlPattern.h"
#include "NiiKeyBoardKey.h"

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