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
        NIIf getCurrent() const             { return mCurrentValue; }

        /** 设置步进值
        @version NIIEngine 3.0.0
        */
        void setStepValue(NIIf value);

        /** 获取步进值
        @version NIIEngine 3.0.0
        */
        NIIf getStepValue() const           { return mStepValue; }

        /** 设置最大值
        @version NIIEngine 3.0.0
        */
        void setMaxValue(NIIf value);

        /** 获取最大值
        @version NIIEngine 3.0.0
        */
        NIIf getMaxValue() const            { return mMaxValue; }

        /** 设置最小值
        @version NIIEngine 3.0.0
        */
        void setMinValue(NIIf value);

        /** 获取最小值
        @version NIIEngine 3.0.0
        */
        NIIf getMinValue() const            { return mMinValue; }

        /** 设置数值模式
        @version NIIEngine 3.0.0
        */
        void setNumMode(NumMode mode);

        /** 获取数值模式
        @version NIIEngine 3.0.0
        */
        NumMode getNumMode() const          { return mNumMode; }

        /** 设置文本区域
        @version NIIEngine 3.0.0
        */
        void setTextAreaView(StyleViewID id) { mTextAreaView = id; }

        /** 获取文本区域
        @version NIIEngine 3.0.0
        */
        StyleViewID getTextAreaView() const { return mTextAreaView; }

        /** 设置增值按纽
        @version NIIEngine 3.0.0
        */
        void setIncreaseView(StyleViewID id) { mIncreaseView = id; }

        /** 获取增值按纽
        @version NIIEngine 3.0.0
        */
        StyleViewID getIncreaseView() const { return mIncreaseView; }

        /** 设置减值按纽
        @version NIIEngine 3.0.0
        */
        void setDecreaseView(StyleViewID id) { mDecreaseView = id; }

        /** 获取减值按纽
        @version NIIEngine 3.0.0
        */
        StyleViewID getDecreaseView() const { return mDecreaseView; }

        /** 获取文本区域
        @version NIIEngine 3.0.0
        */
        Editbox * getTextArea() const { return mTextArea; }

        /** 获取增值按纽
        @version NIIEngine 3.0.0
        */
        PushButton * getIncrease() const { return mIncrease; }

        /** 获取减值按纽
        @version NIIEngine 3.0.0
        */
        PushButton * getDecrease() const { return mDecrease; }
    protected:
        /// @copydetails Container::initChild
        void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

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
        virtual void onFont(const FontEventArgs * arg);

        /** 当值模式时触发
        @version NIIEngine 3.0.0
        */
        virtual void onNumModeChange(const WidgetEventArgs * arg);

        /** 当文本改变时触发
        @version NIIEngine 3.0.0
        */
        virtual void onText(const WidgetEventArgs * arg);

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
        static const EventID ValueEvent;

        /** 步进改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID StepValueEvent;

        /** 最大值改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID MaxValueEvent;

        /** 最小值改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID MinValueEvent;

        /** 数值模式改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID NumModeEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        Editbox * mTextArea;
        PushButton * mIncrease;
        PushButton * mDecrease;
        StyleViewID mTextAreaView;
        StyleViewID mIncreaseView;
        StyleViewID mDecreaseView;
        NumMode mNumMode;
        NIIf mStepValue;
        NIIf mCurrentValue;
        NIIf mMinValue;
        NIIf mMaxValue;
    };
}
}
#endif