/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-8

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

#ifndef _NII_JOYSTICK_CONTROLITEM_H_
#define _NII_JOYSTICK_CONTROLITEM_H_

#include "NiiPreInclude.h"
#include "NiiJoyPadControlItem.h"
#include "NiiJoyStickControlSliderArgs.h"
#include "NiiJoyStickControlPovArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 摇杆控制单元
    @remark
        游戏对象可以灵活的转换控制单元从而不同的效果,把部分事件写出来获得更多的处理
        性能提升.如果需要使用它,需要继承它并重写部分或全部方法
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyStickControlItem : public JoyPadControlItem
    {
    public:
		JoyStickControlItem() {}
		virtual ~JoyStickControlItem() {}

        ///@copydetails ControlItem::getType
		ControlDevType getType() const
		{
			return CDT_JoyStick;
		}
    public:
        /** 当摇杆中的滑块推动时触发
        @param[in] args 摇杆按键事件参数
        @version NIIEngine 3.0.0
        */
		virtual void onSlipSlider(const JoyStickControlSliderArgs * args) {}

        /** 当摇杆中的方向键按下时触发
        @param[in] args 摇杆杆上按键事件参数
        @version NIIEngine 3.0.0
        */
		virtual void onPushPOV(const JoyStickControlPovArgs * args) {}
    };

	class _EngineAPI DummyJoyStickControlItem : public JoyStickControlItem, public ControlAlloc
	{
	public:
		DummyJoyStickControlItem() {}
	};
}
}
#endif