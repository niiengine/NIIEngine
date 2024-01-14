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
#ifndef GSGS_MENUBAR_H
#define GSGS_MENUBAR_H

#include "gsgsPreInclude.h"
#include "gsgsEvent.h"
#include <wx/menu.h>

namespace gsgs
{
    class _gsgsAPI MenuBar : public wxPanel
    {
    public:
        struct MenuInfo {
            wxString label;
            wxMenu* menu;
            wxRect rect;
        };
    public:
        MenuBar(wxWindow * parent, wxMenuBar* bar);
        ~MenuBar();
        inline wxMenuBar * getMenu() { return mMenu; }
    protected:
        MenuBar() {}

        void OnMouseMotion(wxMouseEvent & e);
        void OnLeaveWindow(wxMouseEvent& e);
        void OnPaint(wxPaintEvent& e);
        void OnEraseBG(wxEraseEvent& e);
        void OnLeftDown(wxMouseEvent& e);
        void OnLeftUp(wxMouseEvent& e);
        void OnFrameTitle(gsgs::Event& e);
    protected:
        typedef std::vector<MenuInfo> Vector_t;

        Vector_t mMenuList;
        wxString mName;
        wxString mFrameTitle;
        wxMenuBar * mMenu;
        int mViewHeight;
        int mHoverIndex;
        int mSelectIndex;
        bool mMotionEnter;
        bool mPopupOpen;
        char mCloseState;
        char mMaxState;
        char mMinState;
    };
}

#endif