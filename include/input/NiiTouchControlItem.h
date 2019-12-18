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

#ifndef _NII_TOUCH_CONTROL_ITEM_H_
#define _NII_TOUCH_CONTROL_ITEM_H_

#include "NiiPreInclude.h"
#include "NiiControlItem.h"
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
        DummyMultiTouchControlItem(){}
        ~DummyMultiTouchControlItem(){}
    };    
}
}
#endif