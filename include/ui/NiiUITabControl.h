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

#ifndef _NII_TABCONTROL_H_
#define _NII_TABCONTROL_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"
#include "NiiEventSignal.h"
#include "NiiUITabButton.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TabControl : public Container
    {
    public:
        enum TabType
        {
            Top,
            Left,
            Right,
            Bottom
        };
    public:
        TabControl(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~TabControl();
        
        /**
        @version NIIEngine 3.0.0
        */
        void addTab(Container * obj);

        /** 
        @version NIIEngine 3.0.0
        */
        void removeTab(const String & name);

        /** 
        @version NIIEngine 3.0.0
        */
        void removeTab(WidgetID ID);

        /**
        @version NIIEngine 3.0.0
        */
        inline NCount getTabCount() const{ return getTabContentPane()->getCount(); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setTabType(TabType type){ mTabType = type; layout();}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline TabType getTabType() const { return mTabType; }

        /**
        @version NIIEngine 3.0.0
        */
        void selectTab(const String & name);

        /**
        @version NIIEngine 3.0.0
        */
        void selectTab(WidgetID ID);

        /**
        @version NIIEngine 3.0.0
        */
        void selectTab(Nidx index);

        /**
        @version NIIEngine 3.0.0
        */
        void visibleTab(const String &name);

        /**
        @version NIIEngine 3.0.0
        */
        void visibleTab(WidgetID ID);

        /** 
        @version NIIEngine 3.0.0
        */
        void visibleTab(Nidx index);

        /** 
        @version NIIEngine 3.0.0
        */
        inline Widget * getTabContent(Nidx index) const { return (index >= mTabList.size ()) ? 0 : mTabList[index]->getDest(); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline Widget * getTabContent(const String & name) const{ return getTabContentPane()->get(name); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline Widget * getTabContent(WidgetID id) const{ return getTabContentPane()->get(id, false); }

        /**
        @version NIIEngine 3.0.0
        */
        bool isTabContentSelect(Container * obj) const;

        /** 
        @version NIIEngine 3.0.0
        */
        Nidx getSelectIndex() const;

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setTabHeight(const RelPosf & height){ mTabHeight = height; layout(); }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline const RelPosf & getTabHeight() const { return mTabHeight; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline void setTabTextOffset(const RelPosf & oft){ mTextOffset = oft; layout();}

        /** 
        @version NIIEngine 3.0.0
        */
        inline const RelPosf & getTabTextOffset() const { return mTextOffset; }
        
        /// @copydetails Widget::layout
        void layout();

        /// @copydetails Container::addImpl
        virtual void addImpl(Widget * sub);

        /// @copydetails Container::removeImpl
        virtual void removeImpl(Widget * sub);

        /// @copydetails Container::getImpl
        virtual Widget * getImpl(const String & str, bool r = false) const;
    protected:
        /// @copydetails Container::initChild
        virtual void initChild();

        /// @copydetails Container::queuePixelImpl
        virtual void queuePixelImpl(){}

        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * view) const;

        /**
        @version NIIEngine 3.0.0
        */
        virtual void selectTabImpl(Widget * wnd);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void visibleTabImpl(Widget * wnd);

        /**
        @version NIIEngine 3.0.0
        */
        void removeTabImpl(Container * win);
        
        /**
        @version NIIEngine 3.0.0
        */
        TabButton * createTabButton(WidgetID wid) const;

        /**
        @version NIIEngine 3.0.0
        */
        Container * getTabButtonPane() const;

        /**
        @version NIIEngine 3.0.0
        */
        Container * getTabContentPane() const;

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onFont(const FontEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onSelect(const WidgetEventArgs * arg);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::writeImpl
        virtual NCount writeImpl(XmlSerializer * dest) const;

        void TabTextPrc(const EventArgs * args);
        
        void TabButtonClickPrc(const EventArgs * args);
        
        void TabScrollPrc(const EventArgs * e);
        
        void TabDragPrc(const EventArgs * e);
        
        void TabWheelPrc(const EventArgs * e);
    public:
        static const EventID SelectEvent;
        
        static const EventID EventCount;
    protected:
        typedef vector<TabButton *>::type TabList;
        typedef map<Widget *, EventSignal>::type EventSignals;
        
        TabList mTabList;
        EventSignals mSignalList;
        RelPosf mTabHeight;
        RelPosf mTextOffset;
        TabType mTabType;
        NIIf mTabOffset;
        NIIf mDragPos;
    };
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TabControlView : public WidgetView
    {
    public:
        TabControlView(WidgetModelID wsid);

        /**
        @version NIIEngine 3.0.0
        */
        TabButton * createTabButton(WidgetID wid) const;

        /**
        @version NIIEngine 3.0.0
        */
        inline void setTabType(FactoryID type){ mTabType = type; }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline FactoryID getTabType() const{ return mTabType;}

        /// @copydetails WidgetView::flush
        void flush();
    protected:
        FactoryID mTabType;
    };
}
}
#endif