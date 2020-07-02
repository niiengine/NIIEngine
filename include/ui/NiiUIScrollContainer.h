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
#ifndef _NII_UI_ScrollContainer_H_
#define _NII_UI_ScrollContainer_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** ������������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScrollContainer : public Container
    {
    public:
        ScrollContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        ~ScrollContainer();

        /** �������������Զ���С
        @version NIIEngine 3.0.0
        */
        void setContentAutoSize(bool b);

        /** ��ȡ���������Զ���С
        @version NIIEngine 3.0.0
        */
        bool isContentAutoSize() const;

        /** �������������С
        @version NIIEngine 3.0.0
        */
        void setContentArea(const Rectf & area);

        /** ��ȡ���������С
        @version NIIEngine 3.0.0
        */
        const Rectf & getContentArea() const;

        /** �����Ƿ�ʼ����ʾy�������
        @version NIIEngine 3.0.0
        */
        void setForceVScrollbar(bool b);

        /** ��ȡ�Ƿ�ʼ����ʾy�������
        @version NIIEngine 3.0.0
        */
        bool isForceVScrollbar() const;

        /** �����Ƿ�ʼ����ʾx�������
        @version NIIEngine 3.0.0
        */
        void setForceHScrollbar(bool b);

        /** ��ȡ�Ƿ�ʼ����ʾx�������
        @version NIIEngine 3.0.0
        */
        bool isForceHScrollbar() const;

        /** ����x�����������
        @version NIIEngine 3.0.0
        */
        void setHStepSize(NIIf step);

        /** ��ȡx�����������
        @version NIIEngine 3.0.0
        */
        NIIf getHStepSize() const;

        /** ����x�������ҳ��
        @version NIIEngine 3.0.0
        */
        void setHPageRemain(NIIf overlap);

        /** ��ȡx�������ҳ��
        @version NIIEngine 3.0.0
        */
        NIIf getHPageRemain() const;

        /** ����y�����������
        @version NIIEngine 3.0.0
        */
        void setVStepSize(NIIf step);

        /** ����y�����������
        @version NIIEngine 3.0.0
        */
        NIIf getVStepSize() const;

        /** ����y�������ҳ��
        @version NIIEngine 3.0.0
        */
        void setVPageRemain(NIIf overlap);

        /** ����y�������ҳ��
        @version NIIEngine 3.0.0
        */
        NIIf getVPageRemain() const;

        /** ����x�������λ��
        @note �ٷּ�
        @version NIIEngine 3.0.0
        */
        void setHScrollPos(NIIf pos);

        /** ��ȡx�������λ��
        @note �ٷּ�
        @version NIIEngine 3.0.0
        */
        NIIf getHScrollPos() const;

        /** ����y�������λ��
        @note �ٷּ�
        @version NIIEngine 3.0.0
        */
        void setVScrollPos(NIIf pos);

        /** ��ȡy�������λ��
        @note �ٷּ�
        @version NIIEngine 3.0.0
        */
        NIIf getVScrollPos() const;

        /** ��ȡy�������
        @version NIIEngine 3.0.0
        */
        Scrollbar * getVScrollbar() const;

        /** ��ȡx�������
        @version NIIEngine 3.0.0
        */
        Scrollbar * getHScrollbar() const;
        
        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        SizeContainer * getContentContainer() const;

        /// @copydetails Widget::destroy
        void destroy();

        /// @copydetails Container::addImpl
        void addImpl(Widget * sub);

        /// @copydetails Container::removeImpl
        void removeImpl(Widget * sub);
    protected:
        /// @copydetails Container::initChild
        void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** ���¹�����
        @version NIIEngine 3.0.0
        */
        void updateScrollbar();

        /** ������������
        @version NIIEngine 3.0.0
        */
        void updateContentArea();

        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * style) const;
    protected:
        void ContentAreaScrollMFunc(const EventArgs * arg);
        void AutoSizeChangeMFunc(const EventArgs * arg);

        /** ����������ı�ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onContentAreaChange(const WidgetEventArgs * arg);

        /** �������������ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onContentAreaScroll(const WidgetEventArgs * arg);
        
        /** ���Զ���С���ñ仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onAutoSizeChange(const WidgetEventArgs * arg);
        
        /** ��y��������ñ仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onVScrollbarSetChange(const WidgetEventArgs * arg);

        /** ��x��������ñ仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onHScrollbarSetChange(const WidgetEventArgs * arg);

        /// @copydetails Container::onSize
        void onSize(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        void onCursorDrag(const CursorEventArgs * arg);

        /// @copydetails Widget::getImpl
        Widget * getImpl(const String & name, bool r = false) const;

        /** ����������
        @version NIIEngine 3.0.0
        */
        void ContentAreaMFunc(const EventArgs * e);
    public:
        /** ��������ı�ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ContentAreaChangeEvent;

        /** y��������ñ仯ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID VScrollbarSetEvent;

        /** x��������ñ仯ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID HScrollbarSetEvent;

        /** �Զ���С���ñ仯ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID AutoSizeChangeEvent;

        /** �����������ʱ
        @version NIIEngine 3.0.0
        */
        static const EventID ContentAreaScrollEvent;

        /** ��������
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        SizeContainer * mContentContainer;
        Scrollbar * mXScrollbar;
        Scrollbar * mYScrollbar;
        Rectf mContentRect;
        NIIf mVScrollStep;
        NIIf mHScrollStep;
        NIIf mVScrollRemain;
        NIIf mHScrollRemain;
        SignalPtr mContentChangeSingal;
        SignalPtr mAutoSizeChangeSingal;
        bool mForceVScroll;
        bool mForceHScroll;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScrollContainerView : public WidgetView
    {
    public:
        ScrollContainerView(WidgetModelID wsid);

        virtual Rectf getViewableArea() const;

        /// @copydetails WidgetView::flush
        void flush();

        /// @copydetails WidgetView::getArea
        Rectf getArea() const;
    protected:
        /// @copydetails WidgetView::onStyleAttach
        void onStyleAttach();

        /// @copydetails WidgetView::onStyleDetach
        void onStyleDetach();

    private:
        bool mStyleValid;
    };
}
}
#endif