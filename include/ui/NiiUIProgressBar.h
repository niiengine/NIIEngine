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

#ifndef _NII_UI_ProgressBar_H_
#define _NII_UI_ProgressBar_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"
#include "NiiUIWidgetView.h"

namespace NII
{
namespace UI
{
    /** 处理进度
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ProgressBar : public Widget
    {
    public:
        ProgressBar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        virtual ~ProgressBar();

        /** 按步进自动更新
        @version NIIEngine 3.0.0
        */
        void step();

        /** 调整当前进度
        @version NIIEngine 3.0.0
        */
        void adjust(NIIf delta);

        /** 设置当前进度
        @version NIIEngine 3.0.0
        */
        void setProgress(NIIf p);

        /** 获取当前进度
        @version NIIEngine 3.0.0
        */
        NIIf getProgress() const;

        /** 设置步进
        @version NIIEngine 3.0.0
        */
        void setStep(NIIf p);

        /** 设置步进
        @version NIIEngine 3.0.0
        */
        NIIf getStep() const;
    protected:
        /// @copydetials PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** 进度变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onPrcChange(const WidgetEventArgs * arg);

        /** 进度变化时触发
        @version NIIEngine 3.0.0
        */
        virtual void onPrcDone(const WidgetEventArgs * arg);
    public:
        /** 进度开始事件
        @version NIIEngine 3.0.0
        */
        static const EventID PrcStartEvent;

        /** 进度变化事件
        @version NIIEngine 3.0.0
        */
        static const EventID PrcChangeEvent;

        /** 进度完成事件
        @version NIIEngine 3.0.0
        */
        static const EventID PrcDoneEvent;

        /// 事件数量
        static const EventID EventCount;
    protected:
        NIIf mProgress;
        NIIf mStep;
    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ProgressBarView : public WidgetView
    {
    public:
        ProgressBarView(WidgetModelID type);

        bool isVertical() const;
        bool isReversed() const;
        void setVertical(bool setting);
        void setReversed(bool setting);

        /// @copydetails WidgetView::flush
        void flush();
    public:
        static const StateID EnableProgressState;
        static const StateID DisableProgressState;
        static const StateID StateCount;
    protected:
        bool mVDirection;
        bool mReverse;
    };
}
}
#endif