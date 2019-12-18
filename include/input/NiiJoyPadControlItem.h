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

#ifndef _NII_JOYPAD_CONTROL_ITEM_H_
#define _NII_JOYPAD_CONTROL_ITEM_H_

#include "NiiPreInclude.h"
#include "NiiControlItem.h"
#include "NiiJoyPadControlButtonArgs.h"
#include "NiiJoyPadControlAxisArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 手柄控制单元
    @version NIIEngine 3.0.0
    */
    class _EngineAPI JoyPadControlItem : public ControlItem
    {
    public:
		JoyPadControlItem() {}
		virtual ~JoyPadControlItem() {}
        
        ///@copydetails ControlItem::getType
        virtual ControlDevType getType() const 
		{
			return CDT_JoyPad;
		};
    public:
        /** 当摇杆中的普通键按下时触发
        @param[in] args 摇杆按键事件参数
        @version NIIEngine 3.0.0
        */
		virtual void onPressButton(const JoyPadControlButtonArgs * args) {}
        
        /** 当摇杆中的普通键按下后释放时触发
        @param[in] args
        @version NIIEngine 3.0.0
        */
		virtual void onReleaseButton(const JoyPadControlButtonArgs * args) {}
    
        /** 当摇杆中的摇杆摇动时触发
        @param[in] args 摇杆杆事件参数
        @version NIIEngine 3.0.0
        */
		virtual void onTwistAxis(const JoyPadControlAxisArgs * args) {}
    };

	class _EngineAPI DummyJoyPadControlItem : public JoyPadControlItem, public ControlAlloc
	{
	public:
		DummyJoyPadControlItem() {}
	};
}
}
#endif