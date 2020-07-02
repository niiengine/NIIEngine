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
    /** �������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ProgressBar : public Widget
    {
    public:
        ProgressBar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        virtual ~ProgressBar();

        /** �������Զ�����
        @version NIIEngine 3.0.0
        */
        void step();

        /** ������ǰ����
        @version NIIEngine 3.0.0
        */
        void adjust(NIIf delta);

        /** ���õ�ǰ����
        @version NIIEngine 3.0.0
        */
        void setProgress(NIIf p);

        /** ��ȡ��ǰ����
        @version NIIEngine 3.0.0
        */
        NIIf getProgress() const;

        /** ���ò���
        @version NIIEngine 3.0.0
        */
        void setStep(NIIf p);

        /** ���ò���
        @version NIIEngine 3.0.0
        */
        NIIf getStep() const;
    protected:
        /// @copydetials PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** ���ȱ仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onPrcChange(const WidgetEventArgs * arg);

        /** ���ȱ仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onPrcDone(const WidgetEventArgs * arg);
    public:
        /** ���ȿ�ʼ�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID PrcStartEvent;

        /** ���ȱ仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID PrcChangeEvent;

        /** ��������¼�
        @version NIIEngine 3.0.0
        */
        static const EventID PrcDoneEvent;

        /// �¼�����
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