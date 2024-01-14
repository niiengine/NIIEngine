
#include "gsgsMenuBar.h"
//#include "gsgsFrame.h"
#include "gsgsButton.h"
#include "gsgsImageManager.h"
#include <wx/dcmemory.h>
#include <wx/dcbuffer.h>
#define CloseRect(p, a)        const wxRect a(p.GetRight() - 20, 8, 12, 12);
#define MaxRect(p, a)          const wxRect a(p.GetRight() - 50, 8, 12, 12);
#define MinRect(p, a)          const wxRect a(p.GetRight() - 80, 8, 12, 12);
#define CloseRectSelect     1000000
#define AlterRectSelect     1000001
#define MinRectSelect       1000002

namespace gsgs
{
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // MenuBar
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    #define MARGIN_X 5
    #define MARGIN_Y 5
    const wxEventType wxEVT_PREVIEW_BAR_SELECTED = wxNewEventType();
    MenuBar::MenuBar(wxWindow * parent, wxMenuBar* bar) :
        wxPanel(parent)
        , mMenu(bar)
        , mViewHeight(30)
        , mSelectIndex(-1)
        , mHoverIndex(-1)
        , mPopupOpen(false)
    {
        //mFrame->SetMenuBar(bar);
        mCloseState = Button::BST_Normal;
        mMaxState = Button::BST_Normal;
        mMinState = Button::BST_Normal;
        for (size_t i = 0; i < mMenu->GetMenuCount(); i++) {
            MenuInfo mi;
            mi.label = mMenu->GetMenuLabelText(i);
            mi.menu = mMenu->GetMenu(i);
            mi.menu->Detach();
            //mi.menu->Bind(wxEVT_MENU_CLOSE, &MenuBar::OnMenuClose, this);
            mMenuList.push_back(mi);
        }

        // Remove the menus
        /*size_t nCount = mMenu->GetMenuCount();
        for (size_t i = 0; i < nCount; ++i) {
            mMenu->Remove(0);
        }*/
        mName = wxT("MENU_BAR_ID");
        //#ifndef __WXGTK__
        // The parent (hidden frame) will destroy it for us)
        //mMenu->Destroy();
        //#endif
        int width;
        wxBitmap bmp(30, 30);
        wxMemoryDC memDc(bmp);
        wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        memDc.GetTextExtent(wxT("Tp"), &width, &mViewHeight, NULL, NULL, &font);

        mViewHeight += 2 * MARGIN_Y;
        SetSizeHints(parent->GetSize().GetWidth(), mViewHeight);
        Bind(wxEVT_PAINT,               &MenuBar::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND,    &MenuBar::OnEraseBG, this);
        Bind(wxEVT_LEFT_DOWN,           &MenuBar::OnLeftDown, this);
        Bind(wxEVT_RIGHT_DOWN,          &MenuBar::OnLeftDown, this);
        Bind(wxEVT_LEFT_UP,             &MenuBar::OnLeftUp, this);
        Bind(wxEVT_RIGHT_UP,            &MenuBar::OnLeftUp, this);
        Bind(wxEVT_MOTION,              &MenuBar::OnMouseMotion, this);
        Bind(wxEVT_LEAVE_WINDOW,        &MenuBar::OnLeaveWindow, this);
        gsgs_Event().Connect(wxEVT_CL_FRAME_TITLE, gsgsEventHandler(MenuBar::OnFrameTitle), NULL, this);
        //Bind(wxEVT_MENU_CLOSE,        &MenuBar::OnMenuClose, this);
    }
    //-----------------------------------------------------------------
    MenuBar::~MenuBar()
    {
#if defined(__WXGTK__) && wxVERSION_NUMBER < 2904
        if (mMenu && mMenu->m_widget) {
            // Release our reference and set m_widget to NULL this is to avoid freeing the widget again in the d-tor of wxWindow
            mMenu->m_widget = NULL;
        }
#endif
        for (size_t i = 0; i < mMenuList.size(); i++) {
            mMenuList.at(i).menu->Attach(mMenu);
            //mMenuList.at(i).menu->Unbind(wxEVT_MENU_CLOSE, &MenuBar::OnMenuClose, this);
            //wxDELETE(mMenuList.at(i).menu);
        }
        mMenuList.clear();
        wxDELETE(mMenu);
        Unbind(wxEVT_PAINT,             &MenuBar::OnPaint, this);
        Unbind(wxEVT_ERASE_BACKGROUND,  &MenuBar::OnEraseBG, this);
        Unbind(wxEVT_LEFT_DOWN,         &MenuBar::OnLeftDown, this);
        Unbind(wxEVT_RIGHT_DOWN,        &MenuBar::OnLeftDown, this);
        Unbind(wxEVT_LEFT_UP,           &MenuBar::OnLeftUp, this);
        Unbind(wxEVT_RIGHT_UP,          &MenuBar::OnLeftUp, this);
        Unbind(wxEVT_MOTION,            &MenuBar::OnMouseMotion, this);
        Unbind(wxEVT_LEAVE_WINDOW,      &MenuBar::OnLeaveWindow, this);
        Unbind(wxEVT_CL_FRAME_TITLE,    &MenuBar::OnFrameTitle, this);
        gsgs_Event().Disconnect(wxEVT_CL_FRAME_TITLE, gsgsEventHandler(MenuBar::OnFrameTitle), NULL, this);
        //Unbind(wxEVT_MENU_CLOSE,      &MenuBar::OnMenuClose, this);
    }
    //-----------------------------------------------------------------
    void MenuBar::OnMouseMotion(wxMouseEvent & e)
    {
        e.Skip();
        wxRect crect = GetClientRect();
        wxPoint pt = e.GetPosition();
        bool find = false;
        mCloseState = Button::BST_Normal;
        mMaxState = Button::BST_Normal;
        mMinState = Button::BST_Normal;

        size_t lastindex = mHoverIndex;
        for (size_t i = 0; i < mMenuList.size(); i++) {
            if (mMenuList.at(i).rect.Contains(pt)) {
                mHoverIndex = i;
                find = true;
                break;
            }
        }
        if (!find)
        {
            CloseRect(crect, closeRect);
            MaxRect(crect, maxRect);
            MinRect(crect, minRect);

            if (closeRect.Contains(pt))
            {
                mCloseState = Button::BST_Hover;
                mHoverIndex = CloseRectSelect;
            }
            else if (maxRect.Contains(pt))
            {
                mMaxState = Button::BST_Hover;
                mHoverIndex = AlterRectSelect;
            }
            else if (minRect.Contains(pt))
            {
                mMinState = Button::BST_Hover;
                mHoverIndex = MinRectSelect;
            }
            else
                mHoverIndex = -1;
        }

        if (lastindex != mHoverIndex)
            Refresh();
    }
    //-----------------------------------------------------------------
    /*void MenuBar::OnMenuClose(wxMenuEvent & e)
    {
        mSelectIndex = -1;
        mPopupOpen = false;
        Refresh();
    }*/
    //-----------------------------------------------------------------
    void MenuBar::OnFrameTitle(gsgs::Event & e)
    {
        mFrameTitle = e.GetString();
        Refresh();
    }
    //-----------------------------------------------------------------
    void MenuBar::OnLeaveWindow(wxMouseEvent& e)
    {
        e.Skip();

        mCloseState = Button::BST_Normal;
        mMaxState = Button::BST_Normal;
        mMinState = Button::BST_Normal;
        mSelectIndex = -1;
        mHoverIndex = -1;
        Refresh();
    }
    //-----------------------------------------------------------------
    void MenuBar::OnPaint(wxPaintEvent& e)
    {
        int width, height;
        wxRect crect = GetClientRect();
        wxBufferedPaintDC dc(this);
        wxFont font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

        const wxColour & bgColor = gsgs_Style().GetAppBgColour();
        dc.SetPen(bgColor);
        dc.SetBrush(bgColor);
        dc.DrawRectangle(crect);
        dc.SetTextForeground(gsgs_Style().GetAppTextColour());

        int posx = 89; // logo
        int posy = MARGIN_Y;

        /*const wxBitmap & logo = gsgs_Image().LoadBitmap("gsgs-logo", 24);
        if (logo.IsOk()) {
            dc.DrawBitmap(logo, 2, 2);
        }*/
        const wxBitmap & trademark = gsgs_Image().LoadBitmap("guangshangushi", 24);
        if (trademark.IsOk()) {
            dc.DrawBitmap(trademark, 2, 2);
        }

        size_t count = mMenuList.size();
        for (size_t i = 0; i < count; ++i)
        {
            wxString menuTitle = mMenuList.at(i).label;
            dc.GetTextExtent(menuTitle, &width, &height, NULL, NULL, &font);
            const wxRect labelRect(posx, 0, MARGIN_X + width + MARGIN_X, mViewHeight);
            if (mHoverIndex == i)
            {
                wxRect trect = labelRect.Deflate(2, 2);
                dc.SetPen(gsgs_Style().GetAppActiveTextColour());
                dc.SetBrush(gsgs_Style().GetAppActiveTextColour());
                dc.DrawRectangle(trect);
            }
            dc.SetFont(font);
            dc.DrawText(menuTitle, labelRect.GetX() + MARGIN_X, posy);
            posx += labelRect.GetWidth();
            mMenuList.at(i).rect = labelRect;
        }
        posx += 200;
        //wxString menuTitle = gsgs_Frame()->GetTitle();
        dc.GetTextExtent(mFrameTitle, &width, &height, NULL, NULL, &font);

        wxRect labelRect(posx, 0, MARGIN_X + width + MARGIN_X, mViewHeight);
        dc.SetFont(font);
        dc.DrawText(mFrameTitle, labelRect.GetX() + MARGIN_X, posy);
        //posx += labelRect.GetWidth();

        CloseRect(crect, closeRect);
        MaxRect(crect, maxRect);
        MinRect(crect, minRect);

        gsgs_Style().DrawClose(dc, this, closeRect, gsgs_Style().GetAppTextColour(),
            gsgs_Style().GetAppActiveTextColour(), mCloseState, 2);
        gsgs_Style().DrawAlter(dc, this, maxRect, gsgs_Style().GetAppTextColour(),
            gsgs_Style().GetAppActiveTextColour(), mMaxState);
        gsgs_Style().DrawMinimize(dc, this, minRect, gsgs_Style().GetAppTextColour(),
            gsgs_Style().GetAppActiveTextColour(), mMinState, 2);
    }
    //-----------------------------------------------------------------
    void MenuBar::OnEraseBG(wxEraseEvent&)
    {
    }
    //-----------------------------------------------------------------
    void MenuBar::OnLeftDown(wxMouseEvent& e)
    {
        mSelectIndex = mHoverIndex;
        if (mSelectIndex == -1) {
            wxCommandEvent evt(wxEVT_PREVIEW_BAR_SELECTED);
            evt.SetString(wxT(""));
            gsgs_Event().AddPendingEvent(evt);
            return;
        }
        if (mSelectIndex < mMenuList.size())
        {
            wxMenu* menu = mMenuList.at(mSelectIndex).menu;
            if (menu) {
                Refresh();
                wxCommandEvent evt(wxEVT_PREVIEW_BAR_SELECTED);
                wxString s;
                s << mName << wxT(":") << mMenuList.at(mSelectIndex).label;
                evt.SetString(s);
                gsgs_Event().AddPendingEvent(evt);
                PopupMenu(menu, mMenuList.at(mSelectIndex).rect.GetBottomLeft());
                mPopupOpen = true;
            }
        }
        if (mSelectIndex == mHoverIndex)
        {
            if (mHoverIndex == CloseRectSelect)
            {
                mCloseState = Button::BST_Press;
                Refresh();
            }
            else if (mHoverIndex == AlterRectSelect)
            {
                mMaxState = Button::BST_Press;
                Refresh();
            }
            else if (mHoverIndex == MinRectSelect)
            {
                mMinState = Button::BST_Press;
                Refresh();
            }
        }
    }
    //-----------------------------------------------------------------
    void MenuBar::OnLeftUp(wxMouseEvent& e)
    {
        e.Skip();
        if (mSelectIndex == mHoverIndex)
        {
            if (mHoverIndex == CloseRectSelect)
            {
                mCloseState = Button::BST_Hover;
                Refresh();
                wxCommandEvent evt(GSGS_ProgramClose);
                gsgs_Event().AddPendingEvent(evt);
            }
            else if (mHoverIndex == AlterRectSelect)
            {
                mMaxState = Button::BST_Hover;
                Refresh();
                wxCommandEvent evt(GSGS_ProgramMaximize);
                gsgs_Event().AddPendingEvent(evt);
            }
            else if (mHoverIndex == MinRectSelect)
            {
                mMinState = Button::BST_Hover;
                Refresh();
                wxCommandEvent evt(GSGS_ProgramMinimize);
                gsgs_Event().AddPendingEvent(evt);
            }
        }
    }
    //-----------------------------------------------------------------
}