/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-2-8

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
#ifndef _NII_UI_SLIDER_H_
#define _NII_UI_SLIDER_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** 滑块单元
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Slider : public Container
    {
		friend class DefaultSliderWidgetModel;
    public:
        Slider(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);
        virtual ~Slider();

        /** 设置当前值
        @version NIIEngine 3.0.0
        */
        void setCurrent(NIIf value);
        
        /** 获取当前值
        @version NIIEngine 3.0.0
        */
        NIIf getCurrent() const;

        /** 设置最大值
        @version NIIEngine 3.0.0
        */
        void setMaxValue(NIIf value);
        
        /** 获取最大值
        @version NIIEngine 3.0.0
        */
        NIIf getMaxValue() const;

        /** 设置步进值
        @version NIIEngine 3.0.0
        */
        void setStepValue(NIIf value);
        
        /** 获取步进值
        @version NIIEngine 3.0.0
        */
        NIIf getStepValue() const;

        /** 获取位置指标
        @version NIIEngine 3.0.0
        */
        PosButton * getThumb() const;
    protected:
        /// @copydetails Container::init
        virtual	void init();

        /// @copydetails PropertyCmdObj::init
        bool init(PropertyCmdSet * dest);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual	void onCursorDrag(const CursorEventArgs * arg);

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void ThumbMoveMFunc(const EventArgs * e);

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void ThumbBeginMFunc(const EventArgs * e);

        /** 位置指标函数
        @version NIIEngine 3.0.0 高级api
        */
        void ThumbEndMFunc(const EventArgs * e);

        /// @copydetails Widget::check
        virtual bool check(const WidgetModel * style) const;

        /** 值改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onValueChange(const WidgetEventArgs * arg);

        /** 指标开始移动时触发
        @version NIIEngine 3.0.0
        */
        virtual void onThumbMoveBegin(const WidgetEventArgs * arg);

        /** 指标结束移动时触发
        @version NIIEngine 3.0.0
        */
        virtual void onThumbMoveEnd(const WidgetEventArgs * arg);
    public:
        /** 值改变时
        @version NIIEngine 3.0.0
        */
        static const EventID ValueChangeEvent;

        /** 指标开始移动时
        @version NIIEngine 3.0.0
        */
        static const EventID ThumbMoveBeginEvent;

        /** 指标结束移动时
        @version NIIEngine 3.0.0
        */
        static const EventID TumbMoveEndEvent;
        
        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        PosButton * mThumb;
        NIIf mCurrentValue;
        NIIf mMaxValue;
        NIIf mStepValue;
    };
}
}
#endif