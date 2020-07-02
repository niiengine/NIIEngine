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
#ifndef _NII_UI_SLIDER_H_
#define _NII_UI_SLIDER_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"
#include "NiiUIWidgetView.h"

namespace NII
{
namespace UI
{
    /** ���鵥Ԫ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Slider : public Container
    {
        friend class SliderView;
    public:
        Slider(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        virtual ~Slider();

        /** ���õ�ǰֵ
        @version NIIEngine 3.0.0
        */
        void setCurrent(NIIf value);
        
        /** ��ȡ��ǰֵ
        @version NIIEngine 3.0.0
        */
        NIIf getCurrent() const;

        /** �������ֵ
        @version NIIEngine 3.0.0
        */
        void setMaxValue(NIIf value);
        
        /** ��ȡ���ֵ
        @version NIIEngine 3.0.0
        */
        NIIf getMaxValue() const;

        /** ���ò���ֵ
        @version NIIEngine 3.0.0
        */
        void setStepValue(NIIf value);
        
        /** ��ȡ����ֵ
        @version NIIEngine 3.0.0
        */
        NIIf getStepValue() const;

        /** ��ȡλ��ָ��
        @version NIIEngine 3.0.0
        */
        PosButton * getThumb() const;
    protected:
        /// @copydetails Container::initChild
        virtual void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbMoveMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbBeginMFunc(const EventArgs * e);

        /** λ��ָ�꺯��
        @version NIIEngine 3.0.0 �߼�api
        */
        void ThumbEndMFunc(const EventArgs * e);

        /// @copydetails Widget::validImpl
        virtual bool validImpl(const WidgetView * style) const;

        /** ֵ�ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onValueChange(const WidgetEventArgs * arg);

        /** ָ�꿪ʼ�ƶ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onThumbMoveBegin(const WidgetEventArgs * arg);

        /** ָ������ƶ�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onThumbMoveEnd(const WidgetEventArgs * arg);
    public:
        /** ֵ�ı�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ValueEvent;

        /** ָ�꿪ʼ�ƶ�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ThumbMoveBeginEvent;

        /** ָ������ƶ�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID TumbMoveEndEvent;
        
        /** �¼�����
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        PosButton * mThumb;
        NIIf mCurrentValue;
        NIIf mMaxValue;
        NIIf mStepValue;
    };
    
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SliderView : public WidgetView
    {
        friend class Slider;
    public:
        SliderView(WidgetModelID type);

        bool isVertical() const;

        void setVertical(bool b);

        bool isReversedDirection() const;

        void setReversedDirection(bool b);

        /// @copydetails WidgetView::flush
        void flush();

        /// @copydetails WidgetView::layout
        void layout();
    protected:
        virtual void updateThumb();

        virtual NIIf getThumbValue() const;

        virtual NIIf getDirection(const Vector2f & pt) const;
    protected:
        bool mVDirection;
        bool mReverse;
    };
}
}
#endif