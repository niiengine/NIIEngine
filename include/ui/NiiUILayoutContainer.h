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

#ifndef _NII_UI_LAYOUTCONTAINER_H_
#define _NII_UI_LAYOUTCONTAINER_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"

namespace NII
{
namespace UI
{
    /** �Ű�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI LayoutContainer : public Container
    {
    public:
        LayoutContainer(ScriptTypeID stid, WidgetID wid, FactoryID fid, Container * own, 
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);

        virtual ~LayoutContainer();

        /// @copydetails Widget::update
        virtual void update(TimeDurMS cost);

        /// @copydetails Widget::getChildInnerArea
        virtual const CalcRect & getChildInnerArea() const;

        /// @copydetails Widget::notifyPosChange
        virtual void notifyPosChange();

        /// @copydoc Container::addImpl
        virtual void addImpl(Widget * sub);

        /// @copydoc Container::removeImpl
        virtual void removeImpl(Widget * sub);
    protected:
        /// @copydetails Widget::getInnerAreaImpl
        virtual Rectf getInnerAreaImpl(bool pixelAlign) const;
        
		/** ִ���Ű�
		@version NIIEngine 3.0.0
		*/
		virtual void layoutImpl() = 0;

        /** ��ȡ�ӳ�Ա����ʵ��
        @version NIIEngine 3.0.0
        */
        Rectf getChildAreaImpl(bool pixelAlign) const;

        /** �Ű溯��
        @version NIIEngine 3.0.0 �߼�api
        */
        virtual void LayoutPrc(const EventArgs * e);
    protected:
        typedef multimap<Widget *, SignalPtr>::type SignalList;
        SignalList mSignalList;
        CalcRect mChildInnerArea;
        bool mLayout;
    };
    
    /** �����Ű�����
    @version NIIEngine 3.0.0
    */
    class _EngineAPI IndexLayoutContainer : public LayoutContainer
    {
    public:
        IndexLayoutContainer(ScriptTypeID stid, WidgetID wid, FactoryID fid, Container * own, 
            const String & name = N_StrBlank, LangID lid = N_PrimaryLang);

        virtual ~IndexLayoutContainer();

        using Container::add;

        using Container::remove;

        /** ����Ԫ��
        @version NIIEngine 3.0.0
        */
        void swap(Nidx sub1, Nidx sub2);

        /** ����Ԫ��
        @version NIIEngine 3.0.0
        */
        void swap(Widget * sub1, Widget * sub2);

        /** ����Ԫ��
        @version NIIEngine 3.0.0
        */
        void move(Widget * sub, Nidx index);

        /** ��ǰԪ��
        @version NIIEngine 3.0.0
        */
        void forward(Widget * sub, NCount count = 1);

        /** ���
        @version NIIEngine 3.0.0
        */
        void add(Widget * sub, Nidx index);

        /** ��ȥ
        @version NIIEngine 3.0.0
        */
        void remove(Nidx index);
    public:
        /** ��Ԫ������仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ChildOrderChangeEvent;

        static const EventID EventCount;
    protected:
        /** ��Ԫ������仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onChildOrderChange(const WidgetEventArgs * arg);
    };
    
    /** ��ֱ�Ű�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI VerticalLayoutContainer : public IndexLayoutContainer
    {
    public:
        VerticalLayoutContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~VerticalLayoutContainer();

        /// @copydoc LayoutContainer::layoutImpl
        virtual void layoutImpl();
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI HorizontalLayoutContainer : public IndexLayoutContainer
    {
    public:
        HorizontalLayoutContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~HorizontalLayoutContainer();

        /// @copydoc LayoutContainer::layoutImpl
        virtual void layoutImpl();
    };
    
    /** �����Ű�
    @version NIIEngine 3.0.0
    */
    class _EngineAPI GridLayoutContainer : public LayoutContainer
    {
    public:
        /** �Զ���������
        @version NIIEngine 3.0.0
        */
        enum LayoutType
        {
            LT_None,
            LT_TopToBottom,
            LT_LeftToRight
        };
    public:
        GridLayoutContainer(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~GridLayoutContainer();

        /** �����Զ���������
        @version NIIEngine 3.0.0
        */
        void setLayoutType(LayoutType type)         {mLayoutType = type; mNextIndex = 0;}

        /** ��ȡ�Զ���������
        @version NIIEngine 3.0.0
        */
        inline LayoutType getLayoutType() const     {return mLayoutType;}

        /** ������������
        @version NIIEngine 3.0.0
        */
        void setGrid(NCount width, NCount height);

        /** �������������
        @version NIIEngine 3.0.0
        */
        inline NCount getGridWidth() const          { return mGridWidth; }

        /** �������������
        @version NIIEngine 3.0.0
        */
        inline NCount getGridHeight() const         { return mGridHeight; }

        /** ������������
        @version NIIEngine 3.0.0
        */
        void setGrid(const PlaneSizei & size);

        /** ��ȡ��������
        @version NIIEngine 3.0.0
        */
        PlaneSizei getGrid() const;
        
        using Container::add;

        using Container::remove;

        using Container::get;

        /** �����Ԫ��
        @version NIIEngine 3.0.0
        */
        void add(Widget * sub, NCount x, NCount y);

        /** ��ȡ��Ԫ��
        @version NIIEngine 3.0.0
        */
        Widget * get(Nidx x, Nidx y);

        /** ��ȥ��Ԫ��
        @version NIIEngine 3.0.0
        */
        void remove(Nidx x, Nidx y);

        /** ��Ԫ�ػ�λ
        @version NIIEngine 3.0.0
        */
        void swap(Nidx sub1, Nidx sub2);

        /** ��Ԫ�ػ�λ
        @version NIIEngine 3.0.0
        */
        void swap(NCount x1, NCount y1, NCount x2, NCount y2);

        /** ��Ԫ�ػ�λ
        @version NIIEngine 3.0.0
        */
        void swap(Widget * sub1, Widget * sub2);

        /** ��ȥ����Ԫ��
        @version NIIEngine 3.0.0
        */
        void move(Widget * wnd, NCount x1, NCount y1);

        /** ������һ���Ű�����
        @version NIIEngine 3.0.0
        */
        inline void setNextLayoutIndex(Nidx idx)    { mNextIndex = idx; }

        /** ��ȡ��һ���Ű�����
        @version NIIEngine 3.0.0
        */
        inline Nidx getNextLayoutIndex() const      {return mNextIndex;}

        /// @copydoc LayoutContainer::layoutImpl
        virtual void layoutImpl();

        /// @copydetails Container::addImpl
        virtual void addImpl(Widget * sub);

        /// @copydetails Container::removeImpl
        virtual void removeImpl(Widget * sub);
    public:
        /** ��Ԫ������仯�¼�
        @version NIIEngine 3.0.0
        */
        static const EventID ChildOrderChangeEvent;

        /// �¼�����
        static const EventID EventCount;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** ��Ԫ������仯ʱ����
        @version NIIEngine 3.0.0
        */
        virtual void onChildOrderChange(const WidgetEventArgs * arg);
    protected:
        /** ռλʹ�õ�Ԫ��
        @version NIIEngine 3.0.0
        */
        Widget * createDummy();

        /** ��ȡλ�����ڵ���������
        @version NIIEngien 3.0.0 �ڲ�api
        */
        Nidx getGridIndex(NCount x, NCount y, NCount gridW, NCount gridH) const;

        /** ��ȡʵ�ʵ���������
        @version NIIEngien 3.0.0 �ڲ�api
        */
        Nidx getGridIndex(Nidx index) const;

        /** ��ȡ�������ڵ�����λ��
        @version NIIEngien 3.0.0 �ڲ�api
        */
        void getGridPos(Nidx index, NCount & outx, NCount & outy, NCount gridW, NCount gridH) const;
    protected:
        NCount mGridWidth;
        NCount mGridHeight;
        LayoutType mLayoutType;
        Nidx mNextIndex;
        NCount mNextGridX;
        NCount mNextGridY;
    };
}
}
#endif