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

#ifndef _NII_JOYHAND_CONTROL_PATTERN_H_
#define _NII_JOYHAND_CONTROL_PATTERN_H_

#include "NiiPreInclude.h"
#include "NiiJoyStickControlPattern.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 游戏手把类型控制器
    @remark 包含手势类游戏把概念
    @note
        由于大多系统内核读取手把记录是以队列缓存的,所以为了更好的设计编程模块不应该
        再使用队列缓存模式
    @version NIIEngine 3.0.0 高级api
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

	/** 傀儡控制器
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