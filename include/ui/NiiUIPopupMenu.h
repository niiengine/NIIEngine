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
#ifndef _NII_UI_PopupMenu_H_
#define _NII_UI_PopupMenu_H_

#include "NiiUIPreInclude.h"
#include "NiiUIMenubar.h"

namespace NII
{
namespace UI
{
    /** 弹出菜单栏
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PopupMenu : public Menubar
    {
    public:
        PopupMenu(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~PopupMenu();

        /** 是否处于打开状态
        @version NIIEngine 3.0.0
        */
        inline bool isOpenState() const{ return mOpenState; }

        /** 设置谈入时段
        @version NIIEngine 3.0.0
        */
        inline void setFadeInTime(TimeDurMS time){ mPopupFadeIn = time; }

        /** 获取谈入时段
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getFadeInTime() const{ return mPopupFadeIn; }

        /** 设置谈出时段
        @version NIIEngine 3.0.0
        */
        inline void setFadeOutTime(TimeDurMS time){ mPopupFadeOut = time; }

        /** 获取谈出时段
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getFadeOutTime() const{ return mPopupFadeOut; }

        /** 激发打开
        @version NIIEngine 3.0.0
        */
        void toggleOpen(bool parent = true);

        /** 激发关闭
        @version NIIEngine 3.0.0
        */
        void toggleClose(bool parent = true);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);
    protected:
        /// @copydetails Widget::updateImpl
        virtual void updateImpl(TimeDurMS cost);

        /// @copydetails PopupMenu::layoutContent
        virtual void layoutContent();

        /// @copydetails PopupMenu::getContentSize
        virtual PlaneSizef getContentSize() const;

        /// @copydetails Container::onAlpha
        virtual void onAlpha(const WidgetEventArgs * arg);

        /// @copydetails Widget::onDestroy
        virtual void onDestroy(const WidgetEventArgs * arg);

        /// @copydetails Widget::onShow
        virtual void onShow(const WidgetEventArgs * arg);

        /// @copydetails Menubar::onHide
        virtual void onHide(const WidgetEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);
    protected:
        TimeDurMS mPopupFadeOut;
        TimeDurMS mPopupFadeIn;
        TimeDurMS mFadeCost;
        NIIf mOrgAlpha;
        bool mFading;
        bool mFadeOut;
        bool mOpenState;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PopupMenuView : public ListContainerView
    {
    public:
        PopupMenuView(WidgetViewlID wmid);

        /// @copydetails ListContainerView::flush
        void flush();

		/// @copydetails ListContainerView::getRenderArea
        Rectf getRenderArea() const;

        static const StateID ItemRenderAreaState;
        static const StateID StateCount;
    };
}
}
#endif