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

#ifndef _NII_UI_Widget_CONTROL_ITEM_H_
#define _NII_UI_Widget_CONTROL_ITEM_H_

#include "NiiUIPreInclude.h"
#include "NiiKeyBoardControlItem.h"
#include "NiiMouseControlItem.h"
#include "NiiJoyDevControlItem.h"
#include "NiiTouchControlItem.h"

using namespace NII::NII_MEDIA;
namespace NII
{
namespace UI
{
    /** UI单元专用的键盘控制单元
    @remark 
        在UI体系中尽量把基类转换成这个子类使用,能获取更多的功能
    */
    class _EngineAPI KeyBoardControlItem : public NII_MEDIA::KeyboardControlItem
    {
    public:
        KeyBoardControlItem();
        virtual ~KeyBoardControlItem();
    };
    
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

        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dst);
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
    
    /** UI单元专用的手柄控制单元
    @remark
        在UI体系中尽量把基类转换成这个子类使用,能获取更多的功能
    */
    class _EngineAPI JoyPadControlItem : public NII_MEDIA::JoyPadControlItem
    {
    public:
        JoyPadControlItem();
        virtual ~JoyPadControlItem();
    };
    
    /** UI单元专用的触屏控制单元
    @remark 
        在UI体系中尽量把基类转换成这个子类使用,能获取更多的功能
    */
    class _EngineAPI TouchControlItem : public NII_MEDIA::TouchControlItem
    {
    public:
        TouchControlItem();
        virtual ~TouchControlItem();
    };
}
}
#endif