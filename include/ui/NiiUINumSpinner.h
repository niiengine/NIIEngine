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
#ifndef _NII_UI_NUMSPINNER_H_
#define _NII_UI_NUMSPINNER_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** 数值调控
    @version NIIEngine 3.0.0
    */
    class _EngineAPI NumSpinner : public Container
    {
    public:
        /** 数字模式
        @version NIIEngine 3.0.0
        */
        enum NumMode
        {
            NM_Int,
            NM_Float,
            NM_Hex,
            NM_Octal
        };
    public:
        NumSpinner(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
			LangID lid = N_PrimaryLang);
        virtual ~NumSpinner();

        /** 设置当前值
        @version NIIEngine 3.0.0
        */
        void setCurrent(NIIf value);

        /** 获取当前值
        @version NIIEngine 3.0.0
        */
        NIIf getCurrent() const;

        /** 设置步进值
        @version NIIEngine 3.0.0
        */
        void setStepValue(NIIf value);

        /** 获取步进值
        @version NIIEngine 3.0.0
        */
        NIIf getStepValue() const;

        /** 设置最大值
        @version NIIEngine 3.0.0
        */
        void setMaxValue(NIIf value);

        /** 获取最大值
        @version NIIEngine 3.0.0
        */
        NIIf getMaxValue() const;

        /** 设置最小值
        @version NIIEngine 3.0.0
        */
        void setMinValue(NIIf value);

        /** 获取最小值
        @version NIIEngine 3.0.0
        */
        NIIf getMinValue() const;

        /** 设置数值模式
        @version NIIEngine 3.0.0
        */
        void setNumMode(NumMode mode);

        /** 获取数值模式
        @version NIIEngine 3.0.0
        */
        NumMode getNumMode() const;
    protected:
	    /// @copydetails Container::init
        void init();

        /// @copydetails PropertyCmdObj::init
		bool init(PropertyCmdSet * dest);

        /** 获取文本区域
        @version NIIEngine 3.0.0
        */
        Editbox * getTextArea() const;

        /** 获取增值按纽
        @version NIIEngine 3.0.0
        */
        PushButton * getIncrease() const;

        /** 获取减值按纽
        @version NIIEngine 3.0.0
        */
        PushButton * getDecrease() const;

        /** 获取区域值
        @version NIIEngine 3.0.0
        */
		NIIf getAreaValue() const;

        /** 获取值字符串
        @version NIIEngine 3.0.0
        */
        String getValueText() const;

        /** 当活动时触发
        @version NIIEngine 3.0.0
        */
        virtual void onActivate(const ActivationEventArgs * arg);

        /** 当字体变化时触发
        @version NIIEngine 3.0.0
        */
        virtual	void onFontChange(const FontEventArgs * arg);

        /** 当值模式时触发
        @version NIIEngine 3.0.0
        */
        virtual void onNumModeChange(const WidgetEventArgs * arg);

        /** 当文本改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onTextChange(const WidgetEventArgs * arg);

        /** 当值变化是时触发
        @version NIIEngine 3.0.0
        */
        virtual void onValueChange(const WidgetEventArgs * arg);

        /** 当步进值改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onStepValueChange(const WidgetEventArgs * arg);

        /** 当最大值改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onMaxValueChange(const WidgetEventArgs * arg);

        /** 当最小值改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onMinValueChange(const WidgetEventArgs * arg);

        /** 增值函数
        @version NIIEngine 3.0.0
        */
        void IncreaseMFunc(const EventArgs * e);

        /** 减值函数
        @version NIIEngine 3.0.0
        */
		void DecreaseMFunc(const EventArgs * e);

        /** 文本改变函数
        @version NIIEngine 3.0.0
        */
        void TextChangeMFunc(const EventArgs * e);
    public:
        /** 值改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID ValueChangeEvent;

        /** 步进改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID StepValueChangeEvent;

        /** 最大值改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID MaxValueChangeEvent;

        /** 最小值改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID MinValueChangeEvent;

        /** 数值模式改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID NumModeChangeEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        Editbox * mTextArea;
        PushButton * mIncrease;
        PushButton * mDecrease;
        NumMode mNumMode;
        NIIf mStepValue;
        NIIf mCurrentValue;
        NIIf mMinValue;
        NIIf mMaxValue;
    };
}
}
#endif