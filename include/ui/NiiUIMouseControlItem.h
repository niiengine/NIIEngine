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

#ifndef _NII_UI_MOUSE_CONTROL_ITEM_H_
#define _NII_UI_MOUSE_CONTROL_ITEM_H_

#include "NiiUIPreInclude.h"
#include "NiiMouseControlItem.h"

using namespace NII::NII_MEDIA;

namespace NII
{
namespace UI
{
    /** UI单元专用的鼠标控制单元
    @remark
        在UI体系中尽量把基类转换成这个子类使用,能获取更多的功能
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MouseControlItem : public NII_MEDIA::MouseControlItem, public PropertyCmdObj
    {
    public:
        MouseControlItem(LangID lid = N_PrimaryLang);
        virtual ~MouseControlItem();

        /** 应用到单元
        @version NIIEngine 3.0.0
        */
        void apply(Widget * dst);

        /** 更新时间状态
        @param[in] cost
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /** 设置引发自动事件条件需要的时间阀
        @param delay 单位秒
        @return Nothing.
        */
        void setThreshold(TimeDurMS delay);

        /** 或引发自动事件条件需要的时间阀
        @return 单位秒
        */
        TimeDurMS getThreshold() const;

        /** 引发自动事件条件达成后,自动引发的时间间隔
        @param rate 单位秒
        @version NIIEngine 3.0.0
        */
        void setRate(TimeDurMS rate);

        /** 引发自动事件条件达成后,自动引发的时间间隔
        @return 单位秒
        @version NIIEngine 3.0.0
        */
        TimeDurMS getRate() const;

        /** 设置是否自动重复生成鼠标按下事件
        @param set true 启动;false 禁用
        */
        void setAutoEvent(bool set);

        /** 是否自动重复生成鼠标按下事件
        */
        bool isAutoEvent() const;

        /// @copydetails PropertyCmdObj::init
        virtual bool init(PropertyCmdSet * dst);
    protected:
        /// @copydetails MouseControlItem::onPress
        virtual void onPress(const MousePressArgs * args);

        /// @copydetails MouseControlItem::onRelease
        virtual void onRelease(const MousePressArgs * args);

        /// @copydetails MouseControlItem::onMove
        virtual void onMove(const MousePressArgs * args);
    private:
        /**
        @brief Fires off a repeated mouse button down event for this window.
        */
        void rise(MouseButton button);
    protected:
        Widget * mDest;             ///< 当前响应的目标
        TimeDurMS mRate;            ///< 引发自动事件条件达成后,自动引发的时间
        TimeDurMS mThreshold;       ///< 引发自动事件条件需要的时间阀
        MouseButton mRepeatButton;  ///< 引发自动事件的按键
        TimeDurMS mRepeatElapse;    ///< 当前达成的时间阀
        bool mAutoEvent;            ///< 是否自动重复生成鼠标按下事件
        bool mRepeat;               ///< 引发自动事件达成
    };
}
}
#endif