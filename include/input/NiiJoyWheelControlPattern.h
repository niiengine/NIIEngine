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
		NIIf mWheel;					///< 摇盘值
        JoyWheelControlItem * mTarget;	///< 当前对焦的控制单元
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