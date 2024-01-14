#include "gsgsNotebook.h"
#include "gsgsJSON.h"
#include "gsgsConfig.h"
#include "gsgsLogManager.h"
#include "imanager.h"
#include "globals.h"
#include "gsgsEditorConfig.h"
#include "gsgsStyleManager.h"
#include "gsgsEvent.h"
#include "ieditor.h"
#include "gsgsLexerStyle.h"
#include "plugin.h"
#include "plugin_general_wxcp.h"
#include <wx/gdicmn.h>
#include <wx/settings.h>
#include <wx/dcmemory.h>
#include <wx/stc/stc.h>
#include <wx/renderer.h>
#include <wx/font.h>
#include <wx/app.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/menu.h>
#include <wx/regex.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xh_bmp.h>
#include <wx/xrc/xmlres.h>

#define Y_PADDING_BASE 3

#ifdef __WXMSW__
#define X_PADDING 12
#else
#define X_PADDING 10
#endif

#ifdef __WXMAC__
#include <wx/osx/private.h>
#endif

#define X_BUTTON_SIZE 11
#define DRAW_LINE(__p1, __p2) dc.DrawLine(__p1, __p2);  dc.DrawLine(__p1, __p2);  dc.DrawLine(__p1, __p2);  dc.DrawLine(__p1, __p2);

wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CHANGING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CHANGED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSING, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_PAGE_CLOSE_BUTTON, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TAB_DCLICKED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TABAREA_DCLICKED, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_TAB_CONTEXT_MENU, wxBookCtrlEvent);
wxDEFINE_EVENT(wxEVT_BOOK_FILELIST_BUTTON_CLICKED, ContextMenuEvent);

extern void Notebook_Init_Bitmaps();
namespace gsgs
{
    static int x_button_size = 12;
    static int Y_PADDING = Y_PADDING_BASE;
    static int dropdown_button_size = 20;
    /*class NotebookTheme
    {
    public:
        enum eNotebookTheme {
            kDefault,
            kDark
        };
    public:
        NotebookTheme() {}

        static NotebookTheme GetTheme(NotebookTheme::eNotebookTheme theme)
        {
            NotebookTheme notebookTheme;
            switch (theme)
            {
            case kDefault:
                notebookTheme.m_penColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DDKSHADOW);
                notebookTheme.m_activeTabPenColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
                notebookTheme.m_innerPenColour = *wxWHITE;
                //notebookTheme.m_bgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
                notebookTheme.m_bgColour = gsgs_Style().GetAppBgColour();
                notebookTheme.m_activeTabTextColour = *wxBLACK;
                notebookTheme.m_tabTextColour = *wxBLACK;
                notebookTheme.m_tabBgColour = wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW);
                notebookTheme.m_activeTabBgColour = *wxWHITE;
                break;
            case kDark:
                notebookTheme.m_penColour = *wxBLACK;           //wxColour("rgb(60, 61, 56)");
                notebookTheme.m_activeTabPenColour = wxColour("rgb(60, 61, 56)");
                notebookTheme.m_innerPenColour = wxColour("rgb(102, 102, 100)");
                notebookTheme.m_bgColour = wxColour("rgb(22, 23, 19)");
                notebookTheme.m_activeTabTextColour = *wxWHITE;
                notebookTheme.m_tabTextColour = wxColour("rgb(193, 193, 191)");
                notebookTheme.m_tabBgColour = wxColour("rgb(61, 61, 58)");
                notebookTheme.m_activeTabBgColour = wxColour("rgb(44, 45, 39)");
                break;
            }
            return notebookTheme;
        }

        inline void SetActiveTabBgColour(const wxColour& colour) { m_activeTabBgColour = colour; }

        inline void SetActiveTabPenColour(const wxColour& colour) { m_activeTabPenColour = colour; }

        inline void SetActiveTabTextColour(const wxColour& colour) { m_activeTabTextColour = colour; }

        inline void SetBgColour(const wxColour& colour) { m_bgColour = colour; }
        inline void SetInnerPenColour(const wxColour& colour) { m_innerPenColour = colour; }
        inline void SetPenColour(const wxColour& colour) { m_penColour = colour; }
        inline void SetTabBgColour(const wxColour& colour) { m_tabBgColour = colour; }
        inline void SetTabTextColour(const wxColour& colour) { m_tabTextColour = colour; }

        inline const wxColour& GetActiveTabBgColour() const { return m_activeTabBgColour; }
        inline const wxColour& GetActiveTabPenColour() const { return m_activeTabPenColour; }
        inline const wxColour& GetActiveTabTextColour() const { return m_activeTabTextColour; }
        inline const wxColour& GetBgColour() const { return m_bgColour; }
        inline const wxColour& GetInnerPenColour() const { return m_innerPenColour; }
        inline const wxColour& GetPenColour() const { return m_penColour; }
        inline const wxColour& GetTabBgColour() const { return m_tabBgColour; }
        inline const wxColour& GetTabTextColour() const { return m_tabTextColour; }
    protected:
        // The tab area background colour
        wxColour m_bgColour;
        // The tab area pen colour (used to mark the tab area borders)
        wxColour m_penColour;
        wxColour m_activeTabPenColour;
        wxColour m_innerPenColour;
        // The text colour
        wxColour m_activeTabTextColour;
        wxColour m_tabTextColour;
        // A singe tab background colour
        wxColour m_activeTabBgColour;
        wxColour m_tabBgColour;
    };*/
    // -------------------------------------------------------------------------------
    NotebookTabArt::NotebookTabArt()
        : m_tabRadius(0.0)
    {
        RefreshColours(0);
    }
    // -------------------------------------------------------------------------------
    NotebookTabArt::~NotebookTabArt() {}
    // -------------------------------------------------------------------------------
    void NotebookTabArt::DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
    {
        wxUnusedVar(wnd);
        dc.SetPen(m_bgColour);
        dc.SetBrush(m_bgColour);
        dc.DrawRectangle(rect);
        m_windowListButtonRect = wxRect();
    }
    // -------------------------------------------------------------------------------
#define DRAW_LINE(__p1, __p2) \
        dc.DrawLine(__p1, __p2);  \
        dc.DrawLine(__p1, __p2);  \
        dc.DrawLine(__p1, __p2);  \
        dc.DrawLine(__p1, __p2);
    // -------------------------------------------------------------------------------
    void NotebookTabArt::DrawTab(wxDC& wxdc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& in_rect,
        int close_button_state, wxRect* out_tab_rect, wxRect* out_button_rect,
        int* x_extent)
    {
#ifdef __WXMSW__
        wxGCDC dc;
        gsgs_Style().GetGCDC(wxdc, dc);
#else
        wxDC& dc = wxdc;
#endif

        if (in_rect.GetHeight() == 0) { return; } // Tabs are not visible
        int curx = 0;

        wxColour penColour = page.active ? m_activeTabPenColour : m_penColour;
        wxColour bgColour = page.active ? m_activeTabBgColour : m_tabBgColour;

        wxSize sz = GetTabSize(dc, wnd, page.caption, page.bitmap, page.active, close_button_state, x_extent);
        if (sz.GetHeight() < in_rect.GetHeight()) { sz.SetHeight(in_rect.GetHeight()); }

        wxRect rr(in_rect.GetTopLeft(), sz);

        // AUI tab does not really "touching" the bottom rect
        // so we need to change the offsets a bit
        rr.y += 2;

        /// the tab start position (x)
        curx = rr.x + X_PADDING;

        // Set clipping region
        bool tabTruncated = false;
        if (!m_windowListButtonRect.IsEmpty() && (rr.GetTopRight().x >= m_windowListButtonRect.GetX())) {
            rr.SetWidth(rr.GetWidth() - (rr.GetTopRight().x - m_windowListButtonRect.GetX()));
            if (rr.GetWidth() < 0) { rr.SetWidth(0); }
            tabTruncated = true;
        }

        dc.SetClippingRegion(rr);
        dc.SetPen(penColour);
        dc.SetBrush(bgColour);
        dc.DrawRectangle(rr);
        if (tabTruncated) {
            // Erase the right side line
            dc.SetPen(bgColour);
            dc.DrawLine(rr.GetTopRight(), rr.GetBottomRight());
            // Restore the pen
            dc.SetPen(penColour);
        }
        if (!page.active) {
            // Draw 2 lines at the bottom rect
            // one with the background colour of the active tab and the second
            // with the active tab pen colour
            wxPoint p1, p2;
            p1 = rr.GetBottomLeft();
            p2 = rr.GetBottomRight();

            // We reduce 2 pixels that we added earlier (see ~20 line above)
            p1.x += 1;
            p1.y -= 2;
            p2.x -= 1;
            p2.y -= 2;

            dc.SetPen(m_penColour);
            dc.DrawLine(p1, p2);
        }
        else {
            wxPoint p1, p2;
            p1 = rr.GetBottomLeft();
            p2 = rr.GetBottomRight();

            dc.SetPen(m_activeTabBgColour);
            for (int i = 0; i < 3; ++i) {
                DRAW_LINE(p1, p2);
                p1.y += 1;
                p2.y += 1;
            }

            // Mark the active tab
            p1 = rr.GetTopRight();
            p2 = rr.GetTopLeft();

            // Since the pen width is 3, start the drawing
            // One pixel below
            p1.y += 1;
            p2.y += 1;
            dc.SetPen(wxPen(m_markerColour, 3));
            dc.DrawLine(p1, p2);
        }

        wxString caption = page.caption;
        if (caption.IsEmpty()) { caption = "Tp"; }

        wxFont fnt = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        dc.SetFont(fnt);
        wxSize ext = dc.GetTextExtent(caption);
        if (caption == "Tp") { caption.Clear(); }

        /// Draw the bitmap
        if (page.bitmap.IsOk()) {
            int bmpy = (rr.y + (rr.height - page.bitmap.GetScaledHeight()) / 2);
            dc.DrawBitmap(page.bitmap, curx, bmpy);
            curx += page.bitmap.GetScaledWidth();
            curx += X_PADDING;
        }

        /// Draw the text
        wxColour textColour = page.active ? m_activeTabTextColour : m_tabTextColour;
        dc.SetTextForeground(textColour);
        wxDouble textYOffCorrd = (rr.y + (rr.height - ext.y) / 2);
        dc.DrawText(page.caption, curx, textYOffCorrd);

        // advance the X offset
        curx += ext.x;
        curx += X_PADDING;

        /// Draw the X button on the tab
        if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN) {
            gsgs::Button::State btnState = gsgs::Button::State::BST_Normal;
            switch (close_button_state) {
            case wxAUI_BUTTON_STATE_HOVER:
                btnState = gsgs::Button::State::BST_Hover;
                break;
            case wxAUI_BUTTON_STATE_PRESSED:
                btnState = gsgs::Button::State::BST_Press;
                break;
            }
            int btny = (rr.y + (rr.height - x_button_size) / 2);
            btny += 2; // We add 2 more pixels here cause of the marker line

            wxRect xRect = wxRect(curx, btny, x_button_size, x_button_size);
            gsgs_Style().DrawClose(dc, wnd, xRect, m_markerColour, bgColour, btnState);
            *out_button_rect = xRect;
            curx += x_button_size;
            curx += X_PADDING;
        }

        *out_tab_rect = rr;
        dc.DestroyClippingRegion();
    }
    // -------------------------------------------------------------------------------
    wxSize NotebookTabArt::GetTabSize(wxDC& dc, wxWindow* WXUNUSED(wnd), const wxString& caption,
        const wxBitmap& bitmap, bool active, int close_button_state, int* x_extent)
    {
        wxCoord measured_textx;
        wxCoord measured_texty;

        // +------------------+
        // |.[bmp].[text].[x].|
        // +------------------+

        wxFont f = gsgs_Style().GetDefaultGuiFont();
        dc.SetFont(f);
        dc.GetTextExtent(caption, &measured_textx, &measured_texty);

        // add padding around the text
        wxCoord tab_width = X_PADDING;
        wxCoord tab_height = measured_texty;
        tab_height += (2 * Y_PADDING);

        // if there's a bitmap, add space for it
        if (bitmap.IsOk()) {
            tab_width += bitmap.GetWidth();
            tab_width += X_PADDING; // right side bitmap padding
        }

        tab_width += measured_textx;
        tab_width += X_PADDING;

        // if the close button is showing, add space for it
        if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN) { tab_width += x_button_size + X_PADDING; }

        if (m_flags & wxAUI_NB_TAB_FIXED_WIDTH) {
            tab_width = 100;
        }
        *x_extent = tab_width;
        return wxSize(tab_width, tab_height);
    }
    // -------------------------------------------------------------------------------
    void NotebookTabArt::RefreshColours(long style)
    {
        // Base colours
        wxColour faceColour = gsgs_Style().GetPanelBgColour();
        wxColour textColour = gsgs_Style().GetPanelTextColour();

        // Active tab colours
        m_activeTabTextColour = textColour;
        if (gsgs::Colour::IsDark(faceColour)) {
            // Make the active tab darker
            m_activeTabBgColour = faceColour.ChangeLightness(60);
            m_activeTabPenColour = m_activeTabBgColour;

        }
        else {
            // Make it lighter
            m_activeTabBgColour = faceColour.ChangeLightness(150);
            m_activeTabPenColour = faceColour.ChangeLightness(70);
        }

        // Inactive tab colours
        m_tabTextColour = textColour;
        m_tabBgColour = faceColour.ChangeLightness(95);
        m_penColour = faceColour.ChangeLightness(85);
        m_markerColour = gsgs_Style().GetCaptionColour();
        m_bgColour = faceColour;

        if (style & kNotebook_DynamicColours) {
            LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
            if (lexer) {
                m_activeTabBgColour = lexer->GetProperty(0).GetBgColour();
                m_activeTabPenColour = m_activeTabBgColour;
            }
        }
        if (gsgs::Colour::IsDark(m_activeTabBgColour)) { m_activeTabTextColour = *wxWHITE; }
        // Update the tab height based on the user settings
        Y_PADDING = Y_PADDING_BASE + gsgs_EditorConfig().GetOptions()->GetNotebookTabHeight();
    }
    // -------------------------------------------------------------------------------
    void NotebookTabArt::DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& in_rect, int bitmap_id,
        int button_state, int orientation, wxRect* out_rect)
    {
        if (bitmap_id == wxAUI_BUTTON_WINDOWLIST) {
            m_windowListButtonRect = wxRect((in_rect.x + in_rect.GetWidth() - dropdown_button_size),
                in_rect.y + ((in_rect.height - dropdown_button_size) / 2), dropdown_button_size,
                dropdown_button_size);
            gsgs_Style().DrawDropDownArrow(wnd, dc, m_windowListButtonRect, false, 1, m_markerColour);
            *out_rect = m_windowListButtonRect;
        }
        else {
            wxAuiDefaultTabArt::DrawButton(dc, wnd, in_rect, bitmap_id, button_state, orientation, out_rect);
        }
    }
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // DnD_Data
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    struct DnD_Data {
        TabCtrl* srcTabCtrl;
        int tabIndex;

        DnD_Data()
            : srcTabCtrl(NULL)
            , tabIndex(wxNOT_FOUND)
        {
        }
        void Clear()
        {
            srcTabCtrl = NULL;
            tabIndex = wxNOT_FOUND;
        }
    };
    // -------------------------------------------------------------------------------
    static DnD_Data s_clTabCtrlDnD_Data;
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // TextDropTarget
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    class TextDropTarget : public wxTextDropTarget
    {
    public:
        TextDropTarget(TabCtrl* tabCtrl) : mTab(tabCtrl), m_notebook(NULL) {}
        TextDropTarget(Notebook* notebook) : mTab(NULL), m_notebook(notebook) {}
        virtual ~TextDropTarget() {}

        bool OnDropText(wxCoord x, wxCoord y, const wxString& data)
        {
            int nTabIndex = s_clTabCtrlDnD_Data.tabIndex;
            TabCtrl* ctrl = s_clTabCtrlDnD_Data.srcTabCtrl;
            s_clTabCtrlDnD_Data.Clear();

            if (mTab == ctrl)
            {
                // Test the drop tab index
                int tabidx, visibleidx;
                eDirection align;
                mTab->TestPoint(wxPoint(x, y), tabidx, visibleidx, align);

                // if the tab being dragged and the one we drop it on are the same
                // return false
                if (nTabIndex == tabidx) return false;
                mTab->MoveActiveToIndex(tabidx, align);
            }
            else if (ctrl)
            {
                int tabidx, visibleidx;
                eDirection align;
                mTab->TestPoint(wxPoint(x, y), tabidx, visibleidx, align);

                // DnD to another notebook control
                TabInfoPtr_t movingTab = ctrl->GetTabInfo(nTabIndex);
                Notebook* sourceBook = dynamic_cast<Notebook*>(ctrl->getMe()->GetParent());
                Notebook* targetBook = dynamic_cast<Notebook*>(mTab->getMe()->GetParent());
                if (!sourceBook || !targetBook) return false;

                // To allow moving tabs betwen notebooks, both must have the kNotebook_AllowForeignDnD style bit enabled
                if (!(sourceBook->GetStyle() & kNotebook_AllowForeignDnD)) return false;
                if (!(targetBook->GetStyle() & kNotebook_AllowForeignDnD)) return false;

                sourceBook->RemovePage(nTabIndex, false);
                if (tabidx == wxNOT_FOUND)
                {
                    targetBook->AddPage(movingTab->GetWindow(), movingTab->GetLabel(), true, movingTab->GetBitmap());
                }
                else
                {
                    targetBook->InsertPage(tabidx, movingTab->GetWindow(), movingTab->GetLabel(), true, movingTab->GetBitmap());
                }
                return true;
            }
            else if (m_notebook)
            {
                wxWindowUpdateLocker locker(wxTheApp->GetTopWindow());
                int where = m_notebook->HitTest(m_notebook->ScreenToClient(wxGetMousePosition()));
                // If the drop tab and the source tab are the same, do nothing
                if (where == nTabIndex) { return false; }
                // Get the tab info before we remove it
                wxWindow* movingPage = m_notebook->GetPage(nTabIndex);
                wxWindow* dropPage = m_notebook->GetPage(where);
                if (!movingPage || !dropPage) { return false; }
                wxString label = m_notebook->GetPageText(nTabIndex);
                wxBitmap bmp = m_notebook->GetPageBitmap(nTabIndex);

                // Remove the page and insert it at the drop index
                m_notebook->RemovePage(nTabIndex, false);
                // Locate the drop index (since we removed a page, the drop index is no longer correct)
                where = m_notebook->GetPageIndex(dropPage);
                m_notebook->InsertPage(where, movingPage, label, true, bmp);
            }
            return true;
        }
    private:
        TabCtrl* mTab;
        Notebook* m_notebook;
    };
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // WindowStack
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    WindowStack::WindowStack(wxWindow* parent, wxWindowID id)
        : wxWindow(parent, id),
        m_activeWin(0)
    {
        SetAutoLayout(false);
        SetSizer(new wxBoxSizer(wxVERTICAL));
        Bind(wxEVT_SIZE, &WindowStack::OnSize, this);
        gsgs_Style().RegisterWindow(this);
    }
    // -------------------------------------------------------------------------------
    WindowStack::~WindowStack()
    {
        Unbind(wxEVT_SIZE, &WindowStack::OnSize, this);
        gsgs_Style().UnRegisterWindow(this);
    }
    // -------------------------------------------------------------------------------
    void WindowStack::Select(wxWindow* win)
    {
        int index = FindPage(win);
        if(index == wxNOT_FOUND) 
            return;
        ChangeSelection(index);
    }
    // -------------------------------------------------------------------------------
    void WindowStack::Clear()
    {
        m_activeWin = nullptr;
        std::for_each(mWinList.begin(), mWinList.end(), [&](wxWindow* w) 
        {
            w->Hide();
            w->Destroy();
        });
        mWinList.clear();
    }
    // -------------------------------------------------------------------------------
    bool WindowStack::Remove(wxWindow* win)
    {
        int index = FindPage(win);
        if(index == wxNOT_FOUND) 
            return false;
        GetSizer()->Detach(win);
        mWinList.erase(mWinList.begin() + index);
        if(win == m_activeWin) 
            m_activeWin = nullptr;
        return true;
    }
    // -------------------------------------------------------------------------------
    bool WindowStack::Add(wxWindow* win, bool select)
    {
        if(FindPage(win) != wxNOT_FOUND)
            return false;
        win->Reparent(this);
        mWinList.push_back(win);
        GetSizer()->Add(win, 1, wxEXPAND, 0);
        //if(select) 
        //{
        //    DoSelect(win);
        //} 
        //else 
        //{
            GetSizer()->Hide(win);
        //}
        return true;
    }
    // -------------------------------------------------------------------------------
    bool WindowStack::Contains(wxWindow* win) 
    { 
        return FindPage(win) != wxNOT_FOUND; 
    }
    // -------------------------------------------------------------------------------
    int WindowStack::FindPage(wxWindow* page) const
    {
        for(size_t i = 0; i < mWinList.size(); ++i) 
        {
            if(mWinList[i] == page) return i;
        }
        return wxNOT_FOUND;
    }
    // -------------------------------------------------------------------------------
    void WindowStack::ChangeSelection(size_t index)
    {
        if(index < mWinList.size()) 
            DoSelect(mWinList[index]);
    }
    // -------------------------------------------------------------------------------
    void WindowStack::DoSelect(wxWindow* win)
    {
        if(win && m_activeWin != win)
        {
            // Firsr, show the window
            wxWindowUpdateLocker pplock(GetParent());

            wxSize psize = GetSize();
            win->SetSize(0, 0, psize.x, psize.y);
            GetSizer()->Show(win);
            if (m_activeWin)
            {
                GetSizer()->Hide(m_activeWin);
#ifdef __WXOSX__
                if (m_activeWin) { m_activeWin->Refresh(); }
#endif
            }
            m_activeWin = win;
            // Hide the rest
            //CallAfter(&WindowStack::DoHideNoActiveWindows);
        }
    }
    // -------------------------------------------------------------------------------
    void WindowStack::OnSize(wxSizeEvent & e)
    {
        e.Skip();
        if (m_activeWin)
        {
            wxWindowUpdateLocker pplock(m_activeWin);

            wxSize psize = GetSize();
            m_activeWin->SetSize(0, 0, psize.x, psize.y);
        }
    }
    // -------------------------------------------------------------------------------
    void WindowStack::DoHideNoActiveWindows()
    {
        std::for_each(mWinList.begin(), mWinList.end(), [&](wxWindow* w) 
        {if (w != m_activeWin) { w->Hide(); }});

    #ifdef __WXOSX__
        if(m_activeWin) { m_activeWin->Refresh(); }
    #endif
    }
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // CurvedTabView
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    class _gsgsAPI CurvedTabView : public TabView
    {
    public:
        CurvedTabView() :
            TabView("TRAPEZOID")
        {
            bottomAreaHeight = 5;
            majorCurveWidth = 15;
            smallCurveWidth = 4;
            overlapWidth = 20;
            verticalOverlapWidth = 3;
        }

        virtual ~CurvedTabView()
        {
        }

        void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const TabInfo& tabInfo, const TabColours& colours, size_t style)
        {
            const int TOP_SMALL_HEIGHT = 0;
            wxColour bgColour(tabInfo.IsActive() ? colours.mActiveBg : colours.mInactiveBg);
            wxColour penColour(tabInfo.IsActive() ? colours.mActivePen : colours.mInactivePen);
            wxFont font = GetTabFont(false);
            fontDC.SetTextForeground(tabInfo.IsActive() ? colours.mActiveText : colours.mInactiveText);
            fontDC.SetFont(font);

            if (style & kNotebook_BottomTabs) {
                // Bottom tabs
                {
                    wxPoint points[6];
                    points[0] = tabInfo.m_rect.GetTopLeft();

                    points[1].x = points[0].x + majorCurveWidth;
                    points[1].y = tabInfo.m_rect.GetBottomLeft().y - TOP_SMALL_HEIGHT;

                    points[2].x = points[1].x + smallCurveWidth;
                    points[2].y = points[1].y + TOP_SMALL_HEIGHT;

                    points[3].x = points[0].x + (tabInfo.m_rect.GetWidth() - (majorCurveWidth + smallCurveWidth));
                    points[3].y = points[2].y;

                    points[4].x = points[3].x + smallCurveWidth;
                    points[4].y = points[1].y;

                    points[5] = tabInfo.m_rect.GetTopRight();

                    dc.SetPen(penColour);
                    dc.SetBrush(bgColour);
                    dc.DrawPolygon(6, points);
                }
                {
                    wxPoint points[6];
                    points[0] = tabInfo.m_rect.GetTopLeft();
                    points[0].x += 1;

                    points[1].x = points[0].x + majorCurveWidth;
                    points[1].y = tabInfo.m_rect.GetBottomLeft().y - TOP_SMALL_HEIGHT - 1;

                    points[2].x = points[1].x + smallCurveWidth;
                    points[2].y = points[1].y + TOP_SMALL_HEIGHT;

                    points[3].x = points[0].x + (tabInfo.m_rect.GetWidth() - 2 - (majorCurveWidth + smallCurveWidth));
                    points[3].y = points[2].y;

                    points[4].x = points[3].x + smallCurveWidth;
                    points[4].y = points[1].y;

                    points[5] = tabInfo.m_rect.GetTopRight();
                    points[5].x -= 2;

                    dc.SetPen(tabInfo.IsActive() ? colours.mActiveInnerPen : colours.mInactiveInnerPen);
                    dc.SetBrush(bgColour);
                    dc.DrawPolygon(6, points);
                }
            }
            else {
                // Default tabs (placed at the top)
                {
                    wxPoint points[6];
                    points[0] = tabInfo.m_rect.GetBottomLeft();

                    points[1].x = points[0].x + majorCurveWidth;
                    points[1].y = tabInfo.m_rect.GetLeftTop().y + TOP_SMALL_HEIGHT;

                    points[2].x = points[1].x + smallCurveWidth;
                    points[2].y = points[1].y - TOP_SMALL_HEIGHT;

                    points[3].x = points[0].x + (tabInfo.m_rect.GetWidth() - (majorCurveWidth + smallCurveWidth));
                    points[3].y = points[2].y;

                    points[4].x = points[3].x + smallCurveWidth;
                    points[4].y = points[3].y + TOP_SMALL_HEIGHT;

                    points[5] = tabInfo.m_rect.GetBottomRight();

                    dc.SetPen(penColour);
                    dc.SetBrush(bgColour);
                    dc.DrawPolygon(6, points);
                }

                {
                    wxPoint points[6];
                    points[0] = tabInfo.m_rect.GetBottomLeft();
                    points[0].x += 1;

                    points[1].x = points[0].x + majorCurveWidth;
                    points[1].y = tabInfo.m_rect.GetLeftTop().y + TOP_SMALL_HEIGHT + 1;

                    points[2].x = points[1].x + smallCurveWidth;
                    points[2].y = points[1].y - TOP_SMALL_HEIGHT;

                    points[3].x = points[0].x + (tabInfo.m_rect.GetWidth() - 2 - (majorCurveWidth + smallCurveWidth));
                    points[3].y = points[2].y;

                    points[4].x = points[3].x + smallCurveWidth;
                    points[4].y = points[3].y + TOP_SMALL_HEIGHT;

                    points[5] = tabInfo.m_rect.GetBottomRight();
                    points[5].x -= 2;

                    dc.SetPen(tabInfo.IsActive() ? colours.mActiveInnerPen : colours.mInactiveInnerPen);
                    dc.SetBrush(bgColour);
                    dc.DrawPolygon(6, points);
                }
            }

            if (tabInfo.GetBitmap().IsOk()) {
                const wxBitmap& bmp = (!tabInfo.IsActive() && tabInfo.GetDisabledBitmp().IsOk()) ? tabInfo.GetDisabledBitmp()
                    : tabInfo.GetBitmap();
                dc.DrawBitmap(bmp, tabInfo.m_bmpX + tabInfo.m_rect.GetX(), tabInfo.m_bmpY);
            }
            fontDC.DrawText(tabInfo.m_label, tabInfo.m_textX + tabInfo.m_rect.GetX(), tabInfo.m_textY);
            if (tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
                DrawCloseButton(parent, dc, tabInfo, colours);
            }
        }

        void DrawBottomRect(wxWindow* parent, TabInfoPtr_t tabinfo, const wxRect& rect, wxDC& dc, const TabColours& colours, size_t style)
        {
            wxPoint pt1, pt2;
            dc.SetPen(colours.mActivePen);
            if (style & kNotebook_BottomTabs) {
                // bottom tabs
                pt1 = rect.GetTopLeft();
                pt2 = rect.GetTopRight();
                DRAW_LINE(pt1, pt2);

            }
            else {
                // Top tabs
                pt1 = rect.GetBottomLeft();
                pt2 = rect.GetBottomRight();
                DRAW_LINE(pt1, pt2);
            }

            ClearActiveTabExtraLine(tabinfo, dc, colours, style);
        }
    };
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // GTKTabView
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    class GTKTabView : public TabView
    {
    public:
        GTKTabView() :
            TabView("GSGS.2.0.0")
        {
    #ifdef __WXGTK__
            bottomAreaHeight = 0;
    #else
            bottomAreaHeight = 3;
    #endif
            majorCurveWidth = 0;
            smallCurveWidth = 0;
            overlapWidth = 0;
            verticalOverlapWidth = 0;
            xSpacer = 3;
            //ySpacer = gsgs_EditorConfig().GetOptions()->GetNotebookTabHeight() + 2;
            ySpacer = 2;
        }
        virtual ~GTKTabView()
        {
        }

        virtual void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const TabInfo& tabInfo, const TabColours& colours, size_t style)
        {
            /*wxColour mActiveBg = Colour::IsDark(colours.mArea)
                ? colours.mArea.ChangeLightness(105)
                : colours.mArea.ChangeLightness(125);*/
            wxColour bgColour;
            wxColour penColour;
            if (tabInfo.IsActive())
            {
                bgColour = colours.mMarker;
                penColour = colours.mMarker;
            }
            else if (tabInfo.getXButtonMark() & Button::State::BST_Visable)
            {
                bgColour = gsgs_Style().GetHoverMarkerColour();
                penColour = colours.mMarker;
            }
            else
            {
                bgColour = colours.mArea;
                penColour = colours.mArea;
            }
            wxFont font = GetTabFont(false);
            fontDC.SetTextForeground(tabInfo.IsActive() ? colours.mActiveText : colours.mInactiveText);
            fontDC.SetFont(font);

            wxRect rr = tabInfo.m_rect;

            dc.SetBrush(bgColour);
            dc.SetPen(bgColour);
            dc.DrawRectangle(rr);

            // Restore the pen
            penColour = bgColour;
            dc.SetPen(penColour);

            bool bVerticalTabs = IS_VERTICAL_TABS(style);
            // Draw bitmap
            if (tabInfo.GetBitmap().IsOk() && !bVerticalTabs)
            {
                const wxBitmap& bmp = (!tabInfo.IsActive() && tabInfo.GetDisabledBitmp().IsOk()) ?
                    tabInfo.GetDisabledBitmp() : tabInfo.GetBitmap();
                dc.DrawBitmap(bmp, tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY + rr.GetY());
            }
            wxString label = tabInfo.m_label;
            if (bVerticalTabs)
            {
                // Check that the text can fit into the tab label
                int textEndCoord = tabInfo.m_textX + tabInfo.m_textWidth;
                int tabEndCoord = tabInfo.GetRect().GetRightTop().x;
                if ((textEndCoord + TabView::GetMarkerWidth()) > tabEndCoord) {
                    int newSize = tabEndCoord - tabInfo.m_textX;
                    gsgs_Style().TruncateText(tabInfo.m_label, newSize, dc, label);
                }
            }

            fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY + rr.GetY());
            if ((style & kNotebook_CloseButtonOnActiveTab))
            {
                DrawCloseButton(parent, dc, tabInfo, colours);
            }
        }

        void DrawBottomRect(wxWindow*, TabInfoPtr_t, const wxRect&, wxDC&, const TabColours&, size_t)
        {}

        void DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const TabColours& colours, size_t style)
        {
            //wxColour bgColour(colours.mArea);
            dc.SetPen(colours.mArea);
            dc.SetBrush(colours.mArea);
            dc.DrawRectangle(rect);
        }

        void FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const TabColours& colours, size_t style)
        {
            wxUnusedVar(parent);
            wxUnusedVar(dc);
            wxUnusedVar(rect);
            wxUnusedVar(colours);
            wxUnusedVar(style);
        }

        void AdjustColours(TabColours& colours, size_t style)
        {
            bool useDefaults = true;
            if (style & kNotebook_DynamicColours)
            {
                wxString globalTheme = gsgs_Style().GetGlobalTheme();
                if (!globalTheme.IsEmpty()) {
                    LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("c++", globalTheme);
                    if (lexer && lexer->IsDark())
                    {
                        // Dark theme, update all the colours
                        colours.mActiveBg = lexer->GetProperty(0).GetBgColour();
                        colours.mActiveInnerPen = colours.mActiveBg;
                        colours.mActivePen = colours.mActiveBg.ChangeLightness(110);
                        colours.mActiveText = *wxWHITE;
                        colours.mInactiveText = wxColour("#909497");
                        colours.mArea = colours.mActiveBg.ChangeLightness(110);
                        useDefaults = false;
                    }
                }
            }

            if (useDefaults)
            {
                //colours.mActiveBg = gsgs_Style().GetColour(wxSYS_COLOUR_3DFACE);
                colours.mActiveBg = gsgs_Style().GetAppBgColour();
                colours.mActiveInnerPen = colours.mActiveBg;
                colours.mArea = colours.mInactiveBg;
                colours.mActivePen = colours.mActiveBg;
                colours.mActiveText = gsgs_Style().GetPanelTextColour();
                colours.mInactiveText = Colour::IsDark(colours.mActiveText) ?
                    colours.mActiveText.ChangeLightness(140) : colours.mActiveText.ChangeLightness(60);
            }
        }
    };
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // ScrollTabView
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    class ScrollTabView : public TabView
    {
    public:
        ScrollTabView() :
            TabView("SCROLL")
        {
#ifdef __WXGTK__
            bottomAreaHeight = 0;
#else
            bottomAreaHeight = 3;
#endif
            majorCurveWidth = 0;
            smallCurveWidth = 0;
            overlapWidth = 0;
            verticalOverlapWidth = 0;
            xSpacer = 3;
            //ySpacer = gsgs_EditorConfig().GetOptions()->GetNotebookTabHeight() + 2;
        }
        virtual ~ScrollTabView()
        {
        }

        virtual void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const TabInfo& tabInfo, const TabColours& colours, size_t style)
        {
            /*wxColour mActiveBg = Colour::IsDark(colours.mArea)
                ? colours.mArea.ChangeLightness(105) : colours.mArea.ChangeLightness(125);*/
            wxColour bgColour;
            wxColour penColour;
            if (tabInfo.IsActive())
            {
                bgColour = colours.mMarker;
                penColour = colours.mMarker;
            }
            else if (tabInfo.getXButtonMark() & Button::State::BST_Visable)
            {
                bgColour = gsgs_Style().GetHoverMarkerColour();
                penColour = colours.mMarker;
            }
            else
            {
                bgColour = colours.mArea;
                penColour = colours.mArea;
            }
            //wxFont font = GetTabFont(false);
            wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("ËÎÌו"));
            fontDC.SetTextForeground(tabInfo.IsActive() ? gsgs_Style().GetActiveMarkerColour() : colours.mInactiveText);
            fontDC.SetFont(font);

            wxRect rr = tabInfo.m_rect;

            dc.SetBrush(bgColour);
            dc.SetPen(bgColour);
            dc.DrawRectangle(rr);

            // Restore the pen
            //penColour = bgColour;
            //dc.SetPen(penColour);

            bool bVerticalTabs = IS_VERTICAL_TABS(style);
            // Draw bitmap
            if (tabInfo.GetBitmap().IsOk())
            {
                wxSize btms = tabInfo.GetBitmap().GetScaledSize();
                if (btms.x > tabInfo.GetWidth() || btms.y > tabInfo.GetHeight())
                    dc.SetClippingRegion(tabInfo.m_rect);
                const wxBitmap& bmp = (!tabInfo.IsActive() && tabInfo.GetDisabledBitmp().IsOk()) ?
                    tabInfo.GetDisabledBitmp() : tabInfo.GetBitmap();
                dc.DrawBitmap(bmp, tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY + rr.GetY(), true);
                if (btms.x > tabInfo.GetWidth() || btms.y > tabInfo.GetHeight())
                    dc.DestroyClippingRegion();
            }
            wxString label = tabInfo.m_label;
            if (bVerticalTabs)
            {
                // Check that the text can fit into the tab label
                int textEndCoord = tabInfo.m_textX + tabInfo.m_textWidth;
                int tabEndCoord = tabInfo.GetRect().GetRightTop().x;
                if ((textEndCoord + TabView::GetMarkerWidth()) > tabEndCoord) {
                    int newSize = tabEndCoord - tabInfo.m_textX;
                    gsgs_Style().TruncateText(tabInfo.m_label, newSize, dc, label);
                }
            }
            if (tabInfo.IsActive())
            {
                dc.SetBrush(gsgs_Style().GetAppBgColour());
                dc.SetPen(gsgs_Style().GetAppBgColour());
            }
            if (bVerticalTabs)
            {
                dc.DrawRectangle(0, tabInfo.m_textY + rr.GetY() - 5, rr.GetWidth(), tabInfo.m_textHeight + 10);
            }
            else
            {
                dc.DrawRectangle(rr.GetX(), tabInfo.m_textY -5, rr.GetWidth(), tabInfo.m_textHeight + 10);
            }
            // Restore the pen
            penColour = bgColour;
            dc.SetBrush(penColour);
            dc.SetPen(penColour);
            fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY + rr.GetY());
            /*if ((style & kNotebook_CloseButtonOnActiveTab))
            {
                DrawCloseButton(parent, dc, tabInfo, colours);
            }*/
        }

        void DrawBottomRect(wxWindow*, TabInfoPtr_t, const wxRect&, wxDC&, const TabColours&, size_t)
        {}

        void DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const TabColours& colours, size_t style)
        {
            //wxColour bgColour(colours.mArea);
            dc.SetPen(colours.mArea);
            dc.SetBrush(colours.mArea);
            dc.DrawRectangle(rect);
        }

        void FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const TabColours& colours, size_t style)
        {
            wxUnusedVar(parent);
            wxUnusedVar(dc);
            wxUnusedVar(rect);
            wxUnusedVar(colours);
            wxUnusedVar(style);
        }

        void AdjustColours(TabColours& colours, size_t style)
        {
            bool useDefaults = true;
            if (style & kNotebook_DynamicColours)
            {
                wxString globalTheme = gsgs_Style().GetGlobalTheme();
                if (!globalTheme.IsEmpty()) {
                    LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("c++", globalTheme);
                    if (lexer && lexer->IsDark())
                    {
                        // Dark theme, update all the colours
                        colours.mActiveBg = lexer->GetProperty(0).GetBgColour();
                        colours.mActiveInnerPen = colours.mActiveBg;
                        colours.mActivePen = colours.mActiveBg.ChangeLightness(110);
                        colours.mActiveText = *wxWHITE;
                        colours.mInactiveText = wxColour("#909497");
                        colours.mArea = colours.mActiveBg.ChangeLightness(110);
                        useDefaults = false;
                    }
                }
            }

            if (useDefaults)
            {
                //colours.mActiveBg = gsgs_Style().GetColour(wxSYS_COLOUR_3DFACE);
                colours.mActiveBg = gsgs_Style().GetAppBgColour();
                colours.mActiveInnerPen = colours.mActiveBg;
                colours.mArea = colours.mInactiveBg;
                colours.mActivePen = colours.mActiveBg;
                colours.mActiveText = gsgs_Style().GetPanelTextColour();
                colours.mInactiveText = Colour::IsDark(colours.mActiveText) ?
                    colours.mActiveText.ChangeLightness(140) : colours.mActiveText.ChangeLightness(60);
            }
        }
    };
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // SquareTabView
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    class SquareTabView : public TabView
    {
    public:
        SquareTabView()
            : TabView("MINIMAL")
        {
    #ifdef __WXGTK__
            bottomAreaHeight = 0;
    #else
            bottomAreaHeight = 3;
    #endif
            majorCurveWidth = 0;
            smallCurveWidth = 0;
            overlapWidth = 0;
            verticalOverlapWidth = 0;
            xSpacer = 3;
            //ySpacer = gsgs_EditorConfig().GetOptions()->GetNotebookTabHeight();
        }

        virtual ~SquareTabView()
        {

        }

        virtual void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const TabInfo& tabInfo, const TabColours& colours, size_t style)
        {
            wxColour mInactivePen = colours.mInactivePen;
            bool isDark = Colour::IsDark(colours.mActiveBg);

            wxColour bgColour;
            wxColour textColour;
            if (tabInfo.IsActive()) {
                bgColour = colours.mActiveBg;
                textColour = colours.mActiveText;
                if (isDark) { bgColour = bgColour.ChangeLightness(80); }
            }
            else {
                bgColour = colours.mInactiveBg;
                textColour = colours.mInactiveText;
                if (isDark) { textColour = textColour.ChangeLightness(90); }
            }

            wxColour penColour(tabInfo.IsActive() ? colours.mActivePen : mInactivePen);
            wxColour separatorColour = penColour.ChangeLightness(110);

            wxFont font = GetTabFont(false);
            fontDC.SetTextForeground(textColour);
            fontDC.SetFont(font);

            wxRect rr = tabInfo.m_rect;

            dc.SetBrush(bgColour);
            dc.SetPen(bgColour);
            dc.DrawRectangle(rr);

            // Restore the pen
            penColour = bgColour;
            dc.SetPen(penColour);

            bool bVerticalTabs = IS_VERTICAL_TABS(style);
            // Draw bitmap
            if (tabInfo.GetBitmap().IsOk() && !bVerticalTabs) {
                const wxBitmap& bmp = (!tabInfo.IsActive() && tabInfo.GetDisabledBitmp().IsOk()) ? tabInfo.GetDisabledBitmp()
                    : tabInfo.GetBitmap();
                dc.DrawBitmap(bmp, tabInfo.m_bmpX + rr.GetX(), tabInfo.m_bmpY + rr.GetY());
            }

            wxString label = tabInfo.m_label;
            if (bVerticalTabs) {
                // Check that the text can fit into the tab label
                int textEndCoord = tabInfo.m_textX + tabInfo.m_textWidth;
                int tabEndCoord = tabInfo.GetRect().GetRightTop().x;
                if (textEndCoord > tabEndCoord) {
                    int newSize = tabEndCoord - tabInfo.m_textX;
                    gsgs_Style().TruncateText(tabInfo.m_label, newSize, dc, label);
                }
            }

            fontDC.DrawText(label, tabInfo.m_textX + rr.GetX(), tabInfo.m_textY + rr.GetY());
            if (tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
                DrawCloseButton(parent, dc, tabInfo, colours);
            }
            if (tabInfo.IsActive()) { DrawMarker(dc, tabInfo, colours, style); }
            // Draw the separator line
            if (bVerticalTabs) {
                wxPoint pt_1 = rr.GetRightBottom();
                wxPoint pt_2 = rr.GetLeftBottom();
                dc.SetPen(separatorColour);
                dc.DrawLine(pt_1, pt_2);
            }
            else {
                wxPoint pt_1 = rr.GetRightTop();
                wxPoint pt_2 = rr.GetRightBottom();
                dc.SetPen(separatorColour);
                dc.DrawLine(pt_1, pt_2);
            }
        }

        void DrawBottomRect(wxWindow* parent, TabInfoPtr_t tabinfo, const wxRect& rect, wxDC& dc, const TabColours& colours, size_t style)
        {
            // ClearActiveTabExtraLine(tabinfo, dc, colours, style);
        }
    };
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // ClassicTabView
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    class ClassicTabView : public TabView
    {
    public:
        ClassicTabView()
            : TabView("DEFAULT")
        {
            bottomAreaHeight = 0;
            smallCurveWidth = 0;
            majorCurveWidth = 0;
            overlapWidth = 0;
            verticalOverlapWidth = 0;
        }
        virtual ~ClassicTabView()
        {
        }
        void Draw(wxWindow* parent, wxDC& dc, wxDC& fontDC, const TabInfo& tabInfo, const TabColours& colors, size_t style)
        {
            const int tabRaius = 1.5;

            TabColours colours = colors;
            if (Colour::IsDark(colours.mActiveBg)) {
                setDark(colours, colours.mActiveBg);
            }
            else {
                setLight(colours, colours.mActiveBg);
            }

            wxColour bgColour(tabInfo.IsActive() ? colours.mActiveBg : colours.mInactiveBg);
            wxColour penColour(tabInfo.IsActive() ? colours.mActivePen : colours.mInactivePen);
            wxFont font = GetTabFont(false);
            fontDC.SetTextForeground(tabInfo.IsActive() ? colours.mActiveText : colours.mInactiveText);
            fontDC.SetFont(font);

            if (style & kNotebook_BottomTabs) {
                // Bottom tabs
                wxRect tabRect = tabInfo.GetRect();
                tabRect.SetHeight(tabRect.GetHeight() + tabRaius);
                tabRect.SetY(tabRect.GetY() - tabRaius);
                {
                    dc.SetPen(penColour);
                    dc.SetBrush(bgColour);
                    dc.DrawRoundedRectangle(tabRect, tabRaius);
                }
                {
                    tabRect.Deflate(1); // The inner border
                    dc.SetPen(tabInfo.IsActive() ? colours.mActiveInnerPen : colours.mInactiveInnerPen);
                    dc.SetBrush(bgColour);
                    dc.DrawRoundedRectangle(tabRect, tabRaius);
                }
            }
            else {
                // Default tabs (placed at the top)
                wxRect tabRect = tabInfo.GetRect();
                tabRect.SetHeight(tabRect.GetHeight() + tabRaius);
                {
                    dc.SetPen(penColour);
                    dc.SetBrush(bgColour);
                    dc.DrawRoundedRectangle(tabRect, tabRaius);
                }
                {
                    tabRect.Deflate(1); // The inner border
                    dc.SetPen(tabInfo.IsActive() ? colours.mActiveInnerPen : colours.mInactiveInnerPen);
                    dc.SetBrush(bgColour);
                    dc.DrawRoundedRectangle(tabRect, tabRaius);
                }
            }

            // Draw bitmap
            if (tabInfo.GetBitmap().IsOk()) {
                const wxBitmap& bmp = (!tabInfo.IsActive() && tabInfo.GetDisabledBitmp().IsOk()) ? tabInfo.GetDisabledBitmp()
                    : tabInfo.GetBitmap();
                dc.DrawBitmap(bmp, tabInfo.m_bmpX + tabInfo.m_rect.GetX(), tabInfo.m_bmpY);
            }
            fontDC.DrawText(tabInfo.m_label, tabInfo.m_textX + tabInfo.m_rect.GetX(), tabInfo.m_textY);
            if (tabInfo.IsActive() && (style & kNotebook_CloseButtonOnActiveTab)) {
                DrawCloseButton(parent, dc, tabInfo, colours);
            }
        }
        void DrawBottomRect(wxWindow*, TabInfoPtr_t, const wxRect&, wxDC&, const TabColours&, size_t)
        {
        }
        void DrawBackground(wxWindow* parent, wxDC& dc, const wxRect& rect, const TabColours& colours, size_t style)
        {
            TabColours c = colours;
            if (Colour::IsDark(c.mActiveBg)) {
                setDark(c, c.mActiveBg);
            }
            else {
                setLight(c, c.mActiveBg);
            }
            TabView::DrawBackground(parent, dc, rect, c, style);
        }
        void FinaliseBackground(wxWindow* parent, wxDC& dc, const wxRect& clientRect, const TabColours& colours, size_t style)
        {
            wxUnusedVar(parent);
            TabColours c = colours;
            if (Colour::IsDark(c.mActiveBg)) {
                setDark(c, c.mActiveBg);
            }
            else {
                setLight(c, c.mActiveBg);
            }

            dc.SetPen(c.mActivePen);
            if (style & kNotebook_BottomTabs) {
                dc.DrawLine(clientRect.GetTopLeft(), clientRect.GetTopRight());
            }
            else {
                dc.DrawLine(clientRect.GetBottomLeft(), clientRect.GetBottomRight());
            }
        }
    public:
        static void setDark(TabColours& colours, const wxColour& activeTabBGColour)
        {
            // Active tab
            Colour c;
            c.setBase(activeTabBGColour.ChangeLightness(110));

            colours.mActiveText = c.GetItemTextColour();
            colours.mActiveBg = activeTabBGColour;
            colours.mActivePen = c.GetBorderColour();
            colours.mActiveInnerPen = c.GetBgColour();

            // Inactive tab
            colours.mInactiveBg = c.GetBgColour();
            colours.mInactiveText = c.GetItemTextColour().ChangeLightness(90);
            colours.mInactivePen = c.GetBgColour();
            colours.mInactiveInnerPen = c.GetBgColour();
            colours.mArea = c.GetBgColour();
        }
        static void setLight(TabColours& colours, const wxColour& activeTabBGColour)
        {
            // Active tab
            Colour c;
            c.setBase(activeTabBGColour.ChangeLightness(90));

            colours.mActiveText = c.GetItemTextColour();
            colours.mActiveBg = activeTabBGColour;
            colours.mActivePen = c.GetBorderColour();
            colours.mActiveInnerPen = c.GetBgColour();

            // Inactive tab
            colours.mInactiveBg = c.GetBgColour();
            colours.mInactiveText = c.GetGrayText();
            colours.mInactivePen = c.GetBgColour();
            colours.mInactiveInnerPen = c.GetBgColour();
            colours.mArea = c.GetBgColour();
        }
    };
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // TabColours
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    TabColours::TabColours() { setDark(); }
    // -------------------------------------------------------------------------------
    void TabColours::set(const wxColour & baseColour, const wxColour & textColour)
    {
    #if CL_BUILD
        if (Colour::IsDark(baseColour)) {
            mActiveText = "WHITE";
            mActiveBg = baseColour;
            mActivePen = baseColour.ChangeLightness(80);
            mActiveInnerPen = baseColour.ChangeLightness(120);

            mInactiveText = "WHITE";
            mInactiveBg = baseColour.ChangeLightness(110);
            mInactivePen = mInactiveBg.ChangeLightness(80);
            mInactiveInnerPen = mInactivePen; // mInactiveBg.ChangeLightness(120);

            mArea = baseColour.ChangeLightness(130);
        }
        else {
            mActiveText = "BLACK";
            mActiveBg = baseColour;
            mActivePen = baseColour.ChangeLightness(80);
            mActiveInnerPen = "WHITE";

            mInactiveText = "BLACK";
            mInactiveBg = baseColour.ChangeLightness(90);
            mInactivePen = mInactiveBg.ChangeLightness(80);
            mInactiveInnerPen = mInactivePen; // baseColour;

            mArea = baseColour.ChangeLightness(130);
        }
    #else
        wxUnusedVar(baseColour);
        wxUnusedVar(textColour);
    #endif
    }
    // -------------------------------------------------------------------------------
    void TabColours::setDark()
    {
        setLight();
        mActiveText = "WHITE";
        mActiveBg = *wxBLACK;
    }
    // -------------------------------------------------------------------------------
    void TabColours::setLight()
    {
        wxColour faceColour = gsgs_Style().GetPanelBgColour();
        mActiveText = gsgs_Style().GetPanelTextColour();
        mInactiveText = gsgs_Style().GetColour(wxSYS_COLOUR_GRAYTEXT);
        if (Colour::IsDark(faceColour)) {
            // Make the active tab draker
            mActiveBg = faceColour.ChangeLightness(60);
            mActivePen = mActiveBg;
            mInactiveText = mActiveText;
        }
        else {
            // Make it lighter
            mActiveBg = faceColour;
            mActivePen = faceColour.ChangeLightness(70);
        }

        mActiveInnerPen = mActiveBg;
        if (Colour::IsDark(mActiveBg)) { mActiveText = *wxWHITE; }

        mArea = faceColour;
        gsgs_Config().Read("ActiveTabMarkerColour", mMarker, gsgs_Style().GetActiveMarkerColour());

    #ifdef __WXMSW__
        mInactiveBg = faceColour.ChangeLightness(90);
    #else
        mInactiveBg = mArea.ChangeLightness(90);
    #endif
        mInactivePen = mArea.ChangeLightness(70);
        mInactiveInnerPen = mArea;
    }
    // -------------------------------------------------------------------------------
    bool TabColours::isDark() const 
    { 
        return Colour::IsDark(mActiveBg); 
    }
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    // TabInfo
    //--------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    TabInfo::TabInfo(TabCtrl* tabCtrl, size_t style, wxWindow* page, const wxString& text, const wxBitmap& bmp)
        : m_bitmap(bmp)
        , mTab(tabCtrl)
        , m_label(text)
        , m_window(page)
        , m_textWidth(0)
        , m_textHeight(0)
    {
        mXButtonMark = 0;
        if (m_bitmap.IsOk()) 
        { 
            m_disabledBitmp = gsgs_Style().CreateDisabledBitmap(m_bitmap); 
        }
    }
    // -------------------------------------------------------------------------------
    TabInfo::TabInfo(TabCtrl* tabCtrl)
        : mTab(tabCtrl)
        , m_window(NULL)
        , m_textX(wxNOT_FOUND)
        , m_textY(wxNOT_FOUND)
        , m_bmpX(wxNOT_FOUND)
        , m_bmpY(wxNOT_FOUND)
        , m_textWidth(0)
        , m_textHeight(0)
    {
        mXButtonMark = 0;
    }
    // -------------------------------------------------------------------------------
    void TabInfo::SetBitmap(const wxBitmap& bitmap, size_t)
    {
        m_bitmap = bitmap;
        if (m_bitmap.IsOk())
        {
            m_disabledBitmp = gsgs_Style().CreateDisabledBitmap(m_bitmap);
        }
    }
    // -------------------------------------------------------------------------------
    void TabInfo::SetLabel(const wxString& label, size_t)
    {
        m_label = label;
    }
    // -------------------------------------------------------------------------------
    void TabInfo::SetActive(bool active, size_t)
    {
        if (active)
            mXButtonMark = Button::BST_Active | Button::BST_Normal;
        else
            mXButtonMark = Button::BST_Normal;
    }
    // -------------------------------------------------------------------------------
    wxRect TabInfo::GetCloseButtonRect() const
    {
        wxRect xRect(GetRect().x + GetBmpCloseX(), GetRect().y + GetBmpCloseY(), TabView::GetXButtonSize(),
            TabView::GetXButtonSize());
        return xRect;
    }
    //--------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // TabView
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    TabView::TabView(const wxString& name)
        : bottomAreaHeight(0)
        , majorCurveWidth(0)
        , smallCurveWidth(0)
        , overlapWidth(0)
        , verticalOverlapWidth(0)
        , xSpacer(10)
        , m_name(name)
    {
        ySpacer = gsgs_EditorConfig().GetOptions()->GetNotebookTabHeight();
    }
    // -------------------------------------------------------------------------------
    wxFont TabView::GetTabFont(bool bold)
    {
        wxFont f = gsgs_Style().GetDefaultGuiFont();
        if (bold) { f.SetWeight(wxFONTWEIGHT_BOLD); }
        return f;
    }
    // -------------------------------------------------------------------------------
    #define DRAW_LINE(__p1, __p2) \
        dc.DrawLine(__p1, __p2);  \
        dc.DrawLine(__p1, __p2);  \
        dc.DrawLine(__p1, __p2);  \
        dc.DrawLine(__p1, __p2);
    // -------------------------------------------------------------------------------
    void TabView::ClearActiveTabExtraLine(TabInfoPtr_t tabinfo, wxDC& dc, const TabColours& colours, size_t style)
    {
        wxPoint pt1, pt2;
        dc.SetPen(colours.mActivePen);
        if (style & kNotebook_LeftTabs) {
            dc.SetPen(colours.mActiveBg);
            pt1 = tabinfo->GetRect().GetTopRight();
            pt2 = tabinfo->GetRect().GetBottomRight();
            pt2.y -= 1;
            DRAW_LINE(pt1, pt2);

            pt1.x -= 1;
            pt2.x -= 1;
            DRAW_LINE(pt1, pt2);

        }
        else if (style & kNotebook_RightTabs) {
            // Right tabs
            dc.SetPen(colours.mActiveBg);
            pt1 = tabinfo->GetRect().GetTopLeft();
            pt2 = tabinfo->GetRect().GetBottomLeft();
            pt2.y -= 1;
            DRAW_LINE(pt1, pt2);

        }
        else if (style & kNotebook_BottomTabs) {
            // bottom tabs
            dc.SetPen(colours.mActiveBg);
            pt1 = tabinfo->GetRect().GetTopLeft();
            pt2 = tabinfo->GetRect().GetTopRight();
            pt1.x += 1;
            pt2.x -= 1;
            DRAW_LINE(pt1, pt2);

        }
        else {
            // Top tabs
            dc.SetPen(colours.mActiveBg);
            pt1 = tabinfo->GetRect().GetBottomLeft();
            pt2 = tabinfo->GetRect().GetBottomRight();
    #ifndef __WXOSX__
            pt1.x += 1;
            pt2.x -= 1;
    #else
            pt1.x += 2; // Skip the marker on the left side drawn in pen 3 pixel width
                        //        pt2.x -= 1;
    #endif
            DRAW_LINE(pt1, pt2);
            pt1.y += 1;
            pt2.y += 1;
            DRAW_LINE(pt1, pt2);
        }
    }
    // -------------------------------------------------------------------------------
    void TabView::DrawCloseButton(wxWindow* win, wxDC& dc, const TabInfo& tabInfo, const TabColours& colours)
    {
        int drmark = tabInfo.getXButtonMark();
        if (drmark & (Button::State::BST_Visable | Button::State::BST_Active))
            drmark &= ~(Button::State::BST_Visable | Button::State::BST_Active);
        else
            return;
        // Draw the X button
        wxRect buttonRect = wxRect(tabInfo.m_bmpCloseX + tabInfo.GetRect().GetX(),
            tabInfo.m_bmpCloseY + tabInfo.GetRect().GetY(), X_BUTTON_SIZE, X_BUTTON_SIZE);
        //buttonRect = buttonRect.CenterIn(tabInfo.GetRect(), wxVERTICAL);
        gsgs_Style().DrawClose(dc, win, buttonRect, colours.mActiveText, colours.mMarker, drmark, 2);
    }
    // -------------------------------------------------------------------------------
    void TabView::DrawChevron(wxWindow* win, wxDC& dc, const wxRect& rect, const TabColours& colours, bool hover)
    {
        wxColour buttonColour;
        if (Colour::IsDark(colours.mArea)) {
            buttonColour = colours.mArea.ChangeLightness(150);
        }
        else {
            buttonColour = colours.mArea.ChangeLightness(50);
        }
        gsgs_Style().DrawDropDownArrow(win, dc, rect, hover, 1, buttonColour);
    }
    // -------------------------------------------------------------------------------
    int TabCtrl::GetDefaultBitmapHeight(int yspacer)
    {
        int bmpHeight = 0;
        wxBitmap dummyBmp = gsgs_Image().LoadBitmap("cog");
        if (dummyBmp.IsOk())
        {
            bmpHeight = dummyBmp.GetScaledHeight() + (2 * yspacer);
        }
        else
        {
            bmpHeight = 20;
        }
        return bmpHeight;
    }
    // -------------------------------------------------------------------------------
    void TabCtrl::CalculateOffsets(TabInfo * info, size_t style, const wxRect & rect)
    {
        wxGCDC dc;
        int Y_spacer = mView->ySpacer;
        int X_spacer = mView->xSpacer;
        int M_spacer = mView->majorCurveWidth;
        int S_spacer = mView->smallCurveWidth;

        wxFont font = TabView::GetTabFont(true);
        dc.SetFont(font);

        wxSize sz = dc.GetTextExtent(info->m_label);

        info->m_width = X_spacer + M_spacer + S_spacer;
        //info->m_height = sz.y + Y_spacer;
        info->m_height = rect.height;

        bool bVerticalTabs = IS_VERTICAL_TABS(style);

        if (info->m_bitmap.IsOk() && !bVerticalTabs) {
            info->m_bmpX = info->m_width;
            info->m_width += X_spacer;
            info->m_width += info->m_bitmap.GetScaledWidth();
            info->m_bmpY = ((info->m_height - info->m_bitmap.GetScaledHeight()) / 2);
        }
        else
        {
            info->m_bmpX = wxNOT_FOUND;
            info->m_bmpY = wxNOT_FOUND;
        }

        // Text
        info->m_textX = info->m_width;
        info->m_textY = ((info->m_height - sz.y) / 2);
        info->m_width += sz.x;
        info->m_textWidth = sz.x;
        info->m_textHeight = sz.y;
        // x button
        wxRect xrect;
        if ((style & kNotebook_CloseButtonOnActiveTab)) {
            info->m_width += X_spacer;
            xrect = wxRect(info->m_width, info->m_height, X_BUTTON_SIZE, X_BUTTON_SIZE);
            info->m_bmpCloseX = xrect.GetX();
            //m_bmpCloseY = 0; // we will fix this later
            info->m_bmpCloseY = ((info->m_height - X_BUTTON_SIZE) / 2);
            info->m_width += xrect.GetWidth();
        }

        info->m_width += X_spacer + M_spacer + S_spacer;
        if ((style & kNotebook_UnderlineActiveTab) && bVerticalTabs) 
        { 
            info->m_width += 8; 
        }

        info->m_rect.SetWidth(info->m_width);
        info->m_rect.SetHeight(info->m_height);
    }
    //--------------------------------------------------------------------------------
    wxRect TabCtrl::GetContentArea() const
    {
        return getMe()->GetClientRect();
    }
    // -------------------------------------------------------------------------------
    TabViewPtr_t TabView::CreateRenderer(size_t tabStyle)
    {
        wxString tab;
        gsgs_Config().Read("TabStyle", tab, wxString("gsgs.2.0.0"));
        wxString name = tab.Upper();
        if (tabStyle & kNotebook_ScrollTabs)
        {
            return TabViewPtr_t(new ScrollTabView());
        }
        if ((tabStyle & kNotebook_LeftTabs) || (tabStyle & kNotebook_RightTabs))
        {
            if (name == "MINIMAL") {
                return TabViewPtr_t(new SquareTabView());
            }
            else {
                return TabViewPtr_t(new GTKTabView());
            }
        }
        if (name == "MINIMAL") {
            return TabViewPtr_t(new SquareTabView());
        }
        else if (name == "TRAPEZOID") {
            return TabViewPtr_t(new CurvedTabView());
        }
        else if (name == "GSGS.2.0.0") {
            return TabViewPtr_t(new GTKTabView());
        }
        else {
            return TabViewPtr_t(new ClassicTabView());
        }
    }
    // -------------------------------------------------------------------------------
    wxArrayString TabView::GetRenderers()
    {
        wxArrayString renderers;
        renderers.Add("gsgs.2.0.0");
        //renderers.Add("MINIMAL");
        //renderers.Add("TRAPEZOID");
        //renderers.Add("DEFAULT");
        return renderers;
    }
    // -------------------------------------------------------------------------------
    int TabView::GetMarkerWidth()
    {
    #ifdef __WXOSX__
        return 2;
    #else
        return 3;
    #endif
    }
    // -------------------------------------------------------------------------------
    void TabView::DrawBackground(wxWindow*, wxDC& dc, const wxRect& clientRect, const TabColours& colours, size_t)
    {
        dc.SetPen(colours.mArea);
        dc.SetBrush(colours.mArea);
        dc.DrawRectangle(clientRect);
    }
    // -------------------------------------------------------------------------------
    void TabView::FinaliseBackground(wxWindow*, wxDC&, const wxRect&, const TabColours&, size_t)
    {
    }
    // -------------------------------------------------------------------------------
    void TabView::AdjustColours(TabColours& colours, size_t style)
    {
        if (style & kNotebook_DynamicColours) {
            wxString globalTheme = gsgs_Style().GetGlobalTheme();
            if (!globalTheme.IsEmpty()) {
                LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("c++", globalTheme);
                if (lexer && lexer->IsDark()) {
                    // Dark
                    colours.mActiveBg = lexer->GetProperty(0).GetBgColour();
                    colours.mActiveInnerPen = colours.mActiveBg;
                    colours.mActivePen = colours.mActiveBg.ChangeLightness(110);
                    colours.mActiveText = *wxWHITE;
                }
                else if (lexer) {
                    // Light
                    colours.mActiveBg = lexer->GetProperty(0).GetBgColour();
                    colours.mActiveInnerPen = colours.mActiveBg;
                    colours.mActiveText = *wxBLACK;
                }
            }
        }
    }
    // -------------------------------------------------------------------------------
    void TabView::DrawMarker(wxDC& dc, const TabInfo& tabInfo, const TabColours& colours, size_t style)
    {
        if (style & kNotebook_UnderlineActiveTab)
        {
            wxPen markerPen(colours.mMarker);
            wxPoint p1, p2;
            if ((style & kNotebook_LeftTabs)) {
                p1 = tabInfo.GetRect().GetTopRight();
                p2 = tabInfo.GetRect().GetBottomRight();
                dc.SetPen(markerPen);
                DrawMarkerLine(dc, p1, p2, wxLEFT);
            }
            else if (style & kNotebook_RightTabs) {
                // Right tabs
                p1 = tabInfo.GetRect().GetTopLeft();
                p2 = tabInfo.GetRect().GetBottomLeft();
                dc.SetPen(markerPen);
                DrawMarkerLine(dc, p1, p2, wxRIGHT);
            }
            else if (style & kNotebook_BottomTabs) {
                // Bottom tabs
                p1 = tabInfo.GetRect().GetTopLeft();
                p2 = tabInfo.GetRect().GetTopRight();
                dc.SetPen(markerPen);
                DrawMarkerLine(dc, p1, p2, wxDOWN);
            }
            else {
                // Top tabs
                p1 = tabInfo.GetRect().GetBottomLeft();
                p2 = tabInfo.GetRect().GetBottomRight();
                dc.SetPen(markerPen);
                DrawMarkerLine(dc, p1, p2, wxUP);
            }
        }
    }
    // -------------------------------------------------------------------------------
    void TabView::DrawMarkerLine(wxDC& dc, const wxPoint& p1, const wxPoint& p2, wxDirection direction)
    {
        const int width = GetMarkerWidth();
        wxPoint point1 = p1;
        wxPoint point2 = p2;
        for (int i = 0; i < width; ++i) {
            dc.DrawLine(point1, point2);
            if (direction == wxDOWN) {
                point1.y++;
                point2.y++;
            }
            else if (direction == wxUP) {
                point1.y--;
                point2.y--;
            }
            else if (direction == wxLEFT) {
                point1.x--;
                point2.x--;
            }
            else {
                // wxRIGHT
                point1.x++;
                point2.x++;
            }
        }
    }
    // -------------------------------------------------------------------------------
    int TabView::GetXButtonSize() 
    { 
        return X_BUTTON_SIZE; 
    }
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // TabCtrl
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    TabCtrl::TabCtrl(wxWindow* notebook, size_t style, const wxSize & size) :
         m_style(style)
        , m_contextMenu(NULL)
        , mTabDragStartTime((time_t)-1)
        , mShowTabList(false)
        , mDDBHover(false)
        ,mVisibleIdx(wxNOT_FOUND)
        ,mTabIdx(wxNOT_FOUND)
        ,mLastCloseIdx(wxNOT_FOUND)
    {
        mView = TabView::CreateRenderer(m_style);

        m_history.reset(new TabHistory());
    }
    // -------------------------------------------------------------------------------
    TabCtrl::~TabCtrl()
    {
        wxDELETE(m_contextMenu);
    }
    // -------------------------------------------------------------------------------
    void TabCtrl::DoSetBestSize()
    {
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::ShiftRight(TabInfoVec_t& tabs, const wxRect & crect, int gab)
    {
        // Move the first tab from the list and adjust the remainder
        // of the tabs x coordiate
        if (tabs.size())
        {
            int width = 0;
            TabInfoVec_t::iterator i, e, iend = tabs.end();
            for (e = i = tabs.begin(); i != iend; ++i)
            {
                if (width < gab + crect.GetWidth())
                {
                    wxRect& op = (*i)->GetRect();
                    op.SetX(op.x - gab + mView->overlapWidth);
                    if (op.x + op.width <= 0)
                        e = i;
                }
                else
                {
                    if (i != tabs.begin())
                    {
                        tabs.erase(i, tabs.end());
                    }
                    break;
                }
                width += (*i)->GetWidth();
            }
            if (e != tabs.begin())
            {
                tabs.erase(tabs.begin(), ++e);
            }
            return true;
        }
        return false;
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::IsActiveTabInList(const TabInfoVec_t& tabs) const
    {
        for (size_t i = 0; i < tabs.size(); ++i)
        {
            if (tabs.at(i)->IsActive())
                return true;
        }
        return false;
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::IsActiveTabVisible(const TabInfoVec_t& tabs, const wxRect & crect, int * gap) const
    {
        return false;
    }
    // -------------------------------------------------------------------------------
    void TabCtrl::DoUpdateCoordiantes(TabInfoVec_t& tabs, const wxRect & crect)
    {
        int majorDimension = 0;
        /*if (IsVerticalStyle() && (GetStyle() & kNotebook_ShowFileListButton)) 
        { 
            majorDimension = CHEVRON_SIZE; 
        }*/

        for (size_t i = 0; i < tabs.size(); ++i) {
            TabInfoPtr_t tab = tabs.at(i);
            if (IsVerticalStyle()) {
                tab->GetRect().SetX(0);
                tab->GetRect().SetY(majorDimension);
                tab->GetRect().SetWidth(crect.GetWidth());
                tab->GetRect().SetHeight(tab->GetHeight());
                majorDimension += tab->GetHeight();
            }
            else {
                tab->GetRect().SetX(majorDimension);
                tab->GetRect().SetY(0);
                tab->GetRect().SetWidth(tab->GetWidth());
                tab->GetRect().SetHeight(crect.GetHeight());
                majorDimension += tab->GetWidth() - mView->overlapWidth;
            }
        }
    }
    // -------------------------------------------------------------------------------
    void TabCtrl::UpdateVisibleTabs(bool force)
    {
        if (m_tabs.size())
        {
            wxRect crc = GetContentArea();
            if (!force)
            {
                if (IsActiveTabInList(mVisibleList) && IsActiveTabVisible(mVisibleList, crc))
                    return;
            }
            for (size_t i = 0; i < m_tabs.size(); ++i)
            {
                CalculateOffsets(m_tabs[i].get(), GetStyle(), crc);
            }
            DoUpdateCoordiantes(m_tabs, crc);
            int gab = 0;
            mVisibleList = m_tabs;
            if (IsVerticalStyle())
            {
                if (!IsActiveTabVisible(mVisibleList, crc, &gab))
                {
                    ShiftBottom(mVisibleList, crc, gab);
                }
            }
            else
            {
                if (!IsActiveTabVisible(mVisibleList, crc, &gab))
                {
                    ShiftRight(mVisibleList, crc, gab);
                }
            }
        }
    }
    // -------------------------------------------------------------------------------
    int TabCtrl::ChangeSelection(size_t tabIdx, bool refresh)
    {
        int oldSelection = GetSelection();
        if (!IsIndexValid(tabIdx)) 
            return oldSelection;

        mActiveTab = TabInfoPtr_t(NULL);
        for (size_t i = 0; i < m_tabs.size(); ++i) 
        {
            TabInfoPtr_t tab = m_tabs.at(i);
            if (i == tabIdx)
            {
                mActiveTab = tab;
                tab->SetActive(true, GetStyle());
            }
            else
            {
                tab->SetActive(false, GetStyle());
            }
        }

        if (mActiveTab) 
        {
            GetStack()->Select(mActiveTab->GetWindow());
            mActiveTab->GetWindow()->CallAfter(&wxWindow::SetFocus);
        }

        if (refresh)
        {
            getMe()->Refresh();
        }
        return oldSelection;
    }
    // -------------------------------------------------------------------------------
    int TabCtrl::SetSelection(size_t tabIdx)
    {
        if (tabIdx >= m_tabs.size()) 
            return wxNOT_FOUND;

        int oldSelection = GetSelection();
        /// Do nothing if the tab is already selected
        if (oldSelection == (int)tabIdx)
        {
            return wxNOT_FOUND;
        }

        {
            wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGING);
            event.SetEventObject(getMe()->GetParent());
            event.SetSelection(tabIdx);
            event.SetOldSelection(oldSelection);
            getMe()->GetParent()->GetEventHandler()->ProcessEvent(event);

            if (!event.IsAllowed()) 
            {
                return wxNOT_FOUND; // Vetoed by the user
            }
        }
        ChangeSelection(tabIdx, false);
        UpdateVisibleTabs(false);
        // Keep this page
        m_history->Push(GetPage(tabIdx));

        // Fire an event
        {
            wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
            event.SetEventObject(getMe()->GetParent());
            event.SetSelection(GetSelection());
            event.SetOldSelection(oldSelection);
            getMe()->GetParent()->GetEventHandler()->ProcessEvent(event);
        }

        getMe()->Refresh();
        return wxNOT_FOUND;
    }
    // -------------------------------------------------------------------------------
    int TabCtrl::GetSelection() const
    {
        for (size_t i = 0; i < m_tabs.size(); ++i)
        {
            TabInfoPtr_t tab = m_tabs.at(i);
            if (tab->IsActive()) 
                return i;
        }
        return wxNOT_FOUND;
    }
    // -------------------------------------------------------------------------------
    TabInfoPtr_t TabCtrl::GetTabInfo(size_t index)
    {
        if (!IsIndexValid(index))
            return TabInfoPtr_t(NULL);
        return m_tabs.at(index);
    }
    // -------------------------------------------------------------------------------
    TabInfoPtr_t TabCtrl::GetTabInfo(wxWindow* page)
    {
        for (size_t i = 0; i < m_tabs.size(); ++i)
        {
            TabInfoPtr_t tab = m_tabs.at(i);
            if (tab->GetWindow() == page)
                return tab;
        }
        return TabInfoPtr_t(NULL);
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::SetPageText(size_t page, const wxString& text)
    {
        TabInfoPtr_t tab = GetTabInfo(page);
        if (!tab)
            return false;

        int oldWidth = tab->GetWidth();
        tab->SetLabel(text, GetStyle());
        CalculateOffsets(tab.get(), GetStyle(), GetContentArea());
        int newWidth = tab->GetWidth();

        // Update the width of the tabs from the current tab by "diff"
        DoUpdateXCoordFromPage(tab->GetWindow(), newWidth - oldWidth);

        // Redraw the tab control
        getMe()->Refresh();
        return true;
    }
    // -------------------------------------------------------------------------------
    void TabCtrl::DoUpdateXCoordFromPage(wxWindow* page, int diff)
    {
        // Update the coordinates starting from the current tab
        bool foundActiveTab = false;
        for (size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (!foundActiveTab && (m_tabs.at(i)->GetWindow() == page))
            {
                foundActiveTab = true;
            }
            else if (foundActiveTab) 
            {
                m_tabs.at(i)->GetRect().SetX(m_tabs.at(i)->GetRect().GetX() + diff);
            }
        }
    }
    // -------------------------------------------------------------------------------
    TabInfoPtr_t TabCtrl::GetActiveTabInfo()
    {
        /*for (size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (m_tabs.at(i)->IsActive()) { return m_tabs.at(i); }
        }
        return TabInfoPtr_t(NULL);
        */
        return mActiveTab;
    }
    // -------------------------------------------------------------------------------
    /*void TabCtrl::AddPage(TabInfoPtr_t tab) 
    { 
        InsertPage(m_tabs.size(), tab); 
    }*/
    // -------------------------------------------------------------------------------
    WindowStack* TabCtrl::GetStack()
    { 
        return static_cast<Notebook*>(getMe()->GetParent())->mWinList;
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::InsertPage(size_t index, TabInfoPtr_t tab)
    {
        int oldSelection = GetSelection();
        if (index > m_tabs.size()) return false;
        m_tabs.insert(m_tabs.begin() + index, tab);
        bool sendPageChangedEvent = (oldSelection == wxNOT_FOUND) || tab->IsActive();

        //int tabIndex = index;
        GetStack()->Add(tab->GetWindow(), false);
        if(tab->IsActive())
            ChangeSelection(index, false);
        if (sendPageChangedEvent) {
            // Send an event
            wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
            event.SetEventObject(getMe()->GetParent());
            event.SetSelection(GetSelection());
            event.SetOldSelection(oldSelection);
            getMe()->GetParent()->GetEventHandler()->ProcessEvent(event);
        }
        m_history->Push(tab->GetWindow());
        UpdateVisibleTabs(true);
        getMe()->Refresh();
        return true;
    }
    // -------------------------------------------------------------------------------
    wxString TabCtrl::GetPageText(size_t page) const
    {
        if (IsIndexValid(page))
            return m_tabs.at(page)->GetLabel();
        return "";
    }
    // -------------------------------------------------------------------------------
    wxBitmap TabCtrl::GetPageBitmap(size_t index) const
    {
        if (IsIndexValid(index))
            return m_tabs.at(index)->GetBitmap();
        return wxNullBitmap;
    }
    // -------------------------------------------------------------------------------
    void TabCtrl::SetPageBitmap(size_t index, const wxBitmap& bmp, bool refresh)
    {
        TabInfoPtr_t tab = GetTabInfo(index);
        if (tab)
        {
            // Set the new bitmap and calc the difference
            int oldWidth = tab->GetWidth();
            tab->SetBitmap(bmp, GetStyle());
            CalculateOffsets(tab.get(), GetStyle(), GetContentArea());
            int newWidth = tab->GetWidth();

            // Update the width of the tabs from the current tab by "diff"
            DoUpdateXCoordFromPage(tab->GetWindow(), newWidth - oldWidth);

            // Redraw the tab control
            if(refresh)
                getMe()->Refresh();
        }
    }
    // -------------------------------------------------------------------------------
    void TabCtrl::TestPoint(const wxPoint& pt, int& tabidx, int& visibleidx, eDirection& align)
    {
        tabidx = wxNOT_FOUND;
        visibleidx = wxNOT_FOUND;
        align = eDirection::kInvalid;

        if (mVisibleList.empty()) 
            return;

        //TabInfoPtr_t mActiveTab = GetActiveTabInfo();
        if (mActiveTab && mActiveTab->GetRect().Contains(pt)) 
        {
            for (size_t i = 0; i < mVisibleList.size(); ++i) 
            {
                if (mVisibleList.at(i)->GetWindow() == mActiveTab->GetWindow()) 
                {
                    visibleidx = i;
                    tabidx = DoGetPageIndex(mVisibleList.at(i)->GetWindow());
                    return;
                }
            }
        }

        for (size_t i = 0; i < mVisibleList.size(); ++i) 
        {
            TabInfoPtr_t tab = mVisibleList.at(i);
            wxRect r(tab->GetRect());
            if (r.Contains(pt)) 
            {
                if (IsVerticalStyle()) 
                {
                    if (pt.y > ((r.GetHeight() / 2) + r.GetY())) 
                    {
                        // the point is on the RIGHT side
                        align = eDirection::kUp;
                    }
                    else 
                    {
                        align = eDirection::kDown;
                    }
                }
                else 
                {
                    if (pt.x > ((r.GetWidth() / 2) + r.GetX())) 
                    {
                        // the point is on the RIGHT side
                        align = eDirection::kRight;
                    }
                    else {
                        align = eDirection::kLeft;
                    }
                }
                visibleidx = i;
                tabidx = DoGetPageIndex(tab->GetWindow());
                return;
            }
        }
    }
    // -------------------------------------------------------------------------------
    void TabCtrl::SetStyle(size_t style, bool refresh)
    {
        m_style = style;

        if (IsVerticalStyle()) {
            getMe()->SetSizeHints(mWidth, -1);
            getMe()->SetSize(mWidth, -1);
        }
        else {
            getMe()->SetSizeHints(-1, mHeight);
            getMe()->SetSize(-1, mHeight);
        }

        if (m_style & kNotebook_DarkTabs) {
            m_colours.setDark();
        }
        else {
            m_colours.setLight();
        }
        mView->AdjustColours(m_colours, GetStyle());
        mVisibleList.clear();
        UpdateVisibleTabs(true);
        if (refresh)
        {
            wxWindowUpdateLocker lllt(mMe);
            mMe->Layout();
        }
    }
    // -------------------------------------------------------------------------------
    wxWindow * TabCtrl::GetPage(size_t index) const
    {
        if (IsIndexValid(index))
            return m_tabs.at(index)->GetWindow();
        return NULL;
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::IsIndexValid(size_t index) const 
    { 
        return (index < m_tabs.size()); 
    }
    // -------------------------------------------------------------------------------
    int TabCtrl::FindPage(wxWindow * page) const
    {
        for (size_t i = 0; i < m_tabs.size(); ++i) 
        {
            if (m_tabs.at(i)->GetWindow() == page) 
                return i;
        }
        return wxNOT_FOUND;
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::RemovePage(size_t page, bool notify, bool deletePage)
    {
        wxWindow* nextSelection = NULL;
        if (!IsIndexValid(page)) return false;
        bool deletingSelection = ((int)page == GetSelection());

        if (notify) 
        {
            wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSING);
            event.SetEventObject(getMe()->GetParent());
            event.SetSelection(page);
            getMe()->GetParent()->GetEventHandler()->ProcessEvent(event);
            if (!event.IsAllowed()) 
            {
                // Vetoed
                return false;
            }
        }

        // Remove the tab from the "all-tabs" list
        TabInfoPtr_t tab = m_tabs.at(page);
        m_tabs.erase(m_tabs.begin() + page);

        // Remove this page from the history
        m_history->Pop(tab->GetWindow());

        // Choose a new selection, but only if we are deleting the active tab
        nextSelection = NULL;
        if (deletingSelection) 
        {
            while (!m_history->GetHistory().empty() && !nextSelection) 
            {
                nextSelection = m_history->PrevPage();
                if (!GetTabInfo(nextSelection)) 
                {
                    // The history contains a tab that no longer exists
                    m_history->Pop(nextSelection);
                    nextSelection = NULL;
                }
            }
            // It is OK to end up with a null next selection, we will handle it later

            // Always make sure we have something to select...
            if (!nextSelection && !m_tabs.empty()) 
            { 
                nextSelection = m_tabs.at(0)->GetWindow(); 
            }

            int nextSel = DoGetPageIndex(nextSelection);
            if (nextSel != wxNOT_FOUND) 
            {
                ChangeSelection(nextSel, false);
                if (notify) 
                {
                    wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CHANGED);
                    event.SetEventObject(getMe()->GetParent());
                    event.SetSelection(nextSel);
                    getMe()->GetParent()->GetEventHandler()->ProcessEvent(event);
                }
            }
        }

        // Now remove the page from the notebook. We will delete the page
        // ourself, so there is no need to call DeletePage
        GetStack()->Remove(tab->GetWindow());
        if (deletePage) 
        {
            // Destory the page
            tab->GetWindow()->Destroy();
        }
        else {
            // Just hide it
            tab->GetWindow()->Hide();
        }

        if (notify) 
        {
            wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSED);
            event.SetEventObject(getMe()->GetParent());
            getMe()->GetParent()->GetEventHandler()->ProcessEvent(event);
        }
        UpdateVisibleTabs(true);
        getMe()->Refresh();
        return true;
    }
    // -------------------------------------------------------------------------------
    int TabCtrl::DoGetPageIndex(wxWindow * win) const
    {
        for (size_t i = 0; i < m_tabs.size(); ++i) 
        {
            if (m_tabs.at(i)->GetWindow() == win) 
                return i;
        }
        return wxNOT_FOUND;
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::DeleteAllPages()
    {
        GetStack()->Clear();
        m_tabs.clear();
        mVisibleList.clear();
        m_history->Clear();
        getMe()->Refresh();
        return true;
    }
    // -------------------------------------------------------------------------------
    void TabCtrl::GetAllPages(WindowList& pages)
    {
        std::for_each(m_tabs.begin(), m_tabs.end(),
            [&](TabInfoPtr_t tabInfo) { pages.push_back(tabInfo->GetWindow()); });
    }
    // -------------------------------------------------------------------------------
    void TabCtrl::SetMenu(wxMenu* menu)
    {
        wxDELETE(m_contextMenu);
        m_contextMenu = menu;
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::SetPageToolTip(size_t page, const wxString& tooltip)
    {
        TabInfoPtr_t tab = GetTabInfo(page);
        if (tab)
        {
            tab->SetTooltip(tooltip);
            return true;
        }
        return false;
    }
    // -------------------------------------------------------------------------------
    int TabCtrl::DoGetPageIndex(const wxString& label) const
    {
        for (size_t i = 0; i < m_tabs.size(); ++i)
        {
            if (m_tabs.at(i)->GetLabel() == label)
                return i;
        }
        return wxNOT_FOUND;
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::MoveActiveToIndex(int newIndex, eDirection direction)
    {
        int activeTabInex = GetSelection();

        // Sanity
        if (newIndex == wxNOT_FOUND) return false;
        if (activeTabInex == wxNOT_FOUND) return false;
        if ((newIndex < 0) || (newIndex >= (int)m_tabs.size())) return false;

        bool movingTabRight;
        if (direction == eDirection::kInvalid) {
            movingTabRight = (newIndex > activeTabInex);
        }
        else if ((direction == eDirection::kRight) || (direction == eDirection::kUp)) {
            movingTabRight = true;
        }
        else {
            movingTabRight = false;
        }

        //TabInfoPtr_t movingTab = GetActiveTabInfo();
        TabInfoPtr_t movingTab = mActiveTab;
        TabInfoPtr_t insertBeforeTab = m_tabs.at(newIndex);

        if (!movingTab) return false;

        // Step 1:
        // Remove the tab from both the active and from the visible tabs array
        TabInfoVec_t::iterator iter = std::find_if(mVisibleList.begin(), mVisibleList.end(), [&](TabInfoPtr_t t) {
            if (t->GetWindow() == movingTab->GetWindow()) { return true; }
            return false;
        });
        if (iter != mVisibleList.end()) { mVisibleList.erase(iter); }
        iter = std::find_if(m_tabs.begin(), m_tabs.end(), [&](TabInfoPtr_t t) {
            if (t->GetWindow() == movingTab->GetWindow()) { return true; }
            return false;
        });
        if (iter != m_tabs.end()) { m_tabs.erase(iter); }

        // Step 2:
        // Insert 'tab' in its new position (in both arrays)
        iter = std::find_if(m_tabs.begin(), m_tabs.end(), [&](TabInfoPtr_t t) {
            if (t->GetWindow() == insertBeforeTab->GetWindow()) { return true; }
            return false;
        });

        if (movingTabRight) {
            ++iter;
            // inser the new tab _after_
            if (iter != m_tabs.end()) {
                m_tabs.insert(iter, movingTab);
            }
            else {
                m_tabs.push_back(movingTab);
            }

            iter = std::find_if(mVisibleList.begin(), mVisibleList.end(), [&](TabInfoPtr_t t) {
                if (t->GetWindow() == insertBeforeTab->GetWindow()) { return true; }
                return false;
            });
            ++iter;
            if (iter != mVisibleList.end()) {
                mVisibleList.insert(iter, movingTab);
            }
            else {
                mVisibleList.push_back(movingTab);
            }
        }
        else {
            if (iter != m_tabs.end()) { m_tabs.insert(iter, movingTab); }

            iter = std::find_if(mVisibleList.begin(), mVisibleList.end(), [&](TabInfoPtr_t t) {
                if (t->GetWindow() == insertBeforeTab->GetWindow()) { return true; }
                return false;
            });
            if (iter != mVisibleList.end()) { mVisibleList.insert(iter, movingTab); }
        }
        // Step 3:
        // Update the visible tabs coordinates
        DoUpdateCoordiantes(mVisibleList, GetContentArea());

        // And finally: Refresh
        getMe()->Refresh();
        return true;
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::IsVerticalStyle() const
    { 
        return (m_style & kNotebook_RightTabs) || (m_style & kNotebook_LeftTabs); 
    }
    // -------------------------------------------------------------------------------
    bool TabCtrl::ShiftBottom(TabInfoVec_t& tabs, const wxRect & crect, int gab)
    {
        // Move the first tab from the list and adjust the remainder
        // of the tabs y coordiate
        if (tabs.size()) 
        {
            int height = 0;
            TabInfoVec_t::iterator i, e, iend = tabs.end();
            for (e = i = tabs.begin(); i != iend; ++i)
            {
                if (height < gab + crect.GetHeight())
                {
                    wxRect& op = (*i)->GetRect();
                    op.SetY(op.y - gab + mView->verticalOverlapWidth);
                    if (op.y + op.height <= 0)
                        e = i;
                }
                else
                {
                    if (i != tabs.begin())
                    {
                        tabs.erase(i, tabs.end());
                    }
                    break;
                }
                height += (*i)->GetHeight();
            }
            if (e != tabs.begin())
            {
                tabs.erase(tabs.begin(), ++e);
            }
            return true;
        }
        return false;
    }
    // -------------------------------------------------------------------------------
    void TabCtrl::SetArt(TabViewPtr_t art)
    {
        mView = art;
        if ((m_style & kNotebook_DarkTabs)) 
        {
            m_colours.setDark();
        }
        else 
        {
            m_colours.setLight();
        }
        DoSetBestSize();
        getMe()->Refresh();
    }
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // DefaultTabCtrl
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    DefaultTabCtrl::DefaultTabCtrl(wxWindow* notebook, size_t style, const wxSize & size) :
        TabCtrl(notebook, style, size),
        wxPanel(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxWANTS_CHARS | wxTAB_TRAVERSAL)
    {
        mMe = this;
        SetAutoLayout(false);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        DoSetBestSize();

        SetDropTarget(new TextDropTarget(this));

        if (m_style & kNotebook_DarkTabs)
        {
            m_colours.setDark();
        }
        else
        {
            m_colours.setLight();
        }
        SetStyle(m_style, false);


        Bind(wxEVT_PAINT, &DefaultTabCtrl::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) {});
        Bind(wxEVT_SIZE, &DefaultTabCtrl::OnSize, this);
        Bind(wxEVT_LEFT_DOWN, &DefaultTabCtrl::OnLeftDown, this);
        Bind(wxEVT_LEFT_UP, &DefaultTabCtrl::OnLeftUp, this);
        Bind(wxEVT_MOTION, &DefaultTabCtrl::OnMotion, this);
        Bind(wxEVT_LEAVE_WINDOW, &DefaultTabCtrl::OnLeaveWindow, this);
        Bind(wxEVT_MIDDLE_UP, &DefaultTabCtrl::OnMouseMiddleClick, this);
        Bind(wxEVT_MOUSEWHEEL, &DefaultTabCtrl::OnMouseScroll, this);
        Bind(wxEVT_CONTEXT_MENU, &DefaultTabCtrl::OnContextMenu, this);
        Bind(wxEVT_LEFT_DCLICK, &DefaultTabCtrl::OnLeftDClick, this);
        gsgs_Event().Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &DefaultTabCtrl::OnColoursChanged, this);
    }
    // -------------------------------------------------------------------------------
    DefaultTabCtrl::~DefaultTabCtrl()
    {
        Unbind(wxEVT_PAINT, &DefaultTabCtrl::OnPaint, this);
        Unbind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) {});
        Unbind(wxEVT_SIZE, &DefaultTabCtrl::OnSize, this);
        Unbind(wxEVT_LEFT_DOWN, &DefaultTabCtrl::OnLeftDown, this);
        Unbind(wxEVT_LEFT_UP, &DefaultTabCtrl::OnLeftUp, this);
        Unbind(wxEVT_MOTION, &DefaultTabCtrl::OnMotion, this);
        Unbind(wxEVT_LEAVE_WINDOW, &DefaultTabCtrl::OnLeaveWindow, this);
        Unbind(wxEVT_MIDDLE_UP, &DefaultTabCtrl::OnMouseMiddleClick, this);
        Unbind(wxEVT_MOUSEWHEEL, &DefaultTabCtrl::OnMouseScroll, this);
        Unbind(wxEVT_CONTEXT_MENU, &DefaultTabCtrl::OnContextMenu, this);
        Unbind(wxEVT_LEFT_DCLICK, &DefaultTabCtrl::OnLeftDClick, this);
        gsgs_Event().Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &DefaultTabCtrl::OnColoursChanged, this);
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::DoSetBestSize()
    {
        wxWindowUpdateLocker ttlock(this);
        wxBitmap bmp(1, 1);
        wxMemoryDC memoryDC(bmp);
        wxGCDC gcdc(memoryDC);

        wxFont font = TabView::GetTabFont(true);
        gcdc.SetFont(font);

        wxSize sz = gcdc.GetTextExtent("_WORKSPACE_");
        int bmpHeight = GetDefaultBitmapHeight(mView->ySpacer);

        mHeight = wxMax(sz.GetHeight() + mView->ySpacer, bmpHeight);
        mWidth = sz.GetWidth();

        if (IsVerticalStyle())
        {
            SetSizeHints(mWidth, -1);
            SetSize(mWidth, -1);
        }
        else
        {
            SetSizeHints(-1, mHeight);
            SetSize(-1, mHeight);
        }
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::DoShowTabList()
    {
        if (m_tabs.empty()) return;

        const int curselection = GetSelection();
        wxMenu menu;
        const int firstTabPageID = 13457;
        int pageMenuID = firstTabPageID;

        // Optionally make a sorted view of tabs.
        std::vector<size_t> sortedIndexes(m_tabs.size());
        {
            // std is C++11 at the moment, so no generalized capture.
            size_t index = 0;
            std::generate(sortedIndexes.begin(), sortedIndexes.end(), [&index]() { return index++; });
        }

        if (gsgs_EditorConfig().GetOptions()->IsSortTabsDropdownAlphabetically()) {
            std::sort(sortedIndexes.begin(), sortedIndexes.end(),
                [this](size_t i1, size_t i2) { return m_tabs[i1]->m_label.CmpNoCase(m_tabs[i2]->m_label) < 0; });
        }

        for (auto sortedIndex : sortedIndexes) {
            TabInfoPtr_t tab = m_tabs.at(sortedIndex);
            wxString tab_label = tab->GetLabel();
            wxMenuItem* item = new wxMenuItem(&menu, pageMenuID, tab->GetLabel(), "", wxITEM_CHECK);
            menu.Append(item);
            item->Check(tab->IsActive());
            menu.Bind(wxEVT_MENU,
                [=](wxCommandEvent& event) {
                Notebook* book = dynamic_cast<Notebook*>(getMe()->GetParent());
                int newSelection = book->GetPageIndex(tab_label);
                if (newSelection != curselection) { book->SetSelection(newSelection); }
            },
                pageMenuID);
            pageMenuID++;
        }

        // Let others handle this event as well
        ContextMenuEvent menuEvent(wxEVT_BOOK_FILELIST_BUTTON_CLICKED);
        menuEvent.SetMenu(&menu);
        menuEvent.SetEventObject(getMe()->GetParent()); // The Notebook
        getMe()->GetParent()->GetEventHandler()->ProcessEvent(menuEvent);

#ifdef __WXGTK__
        PopupMenu(&menu);
#else
        getMe()->PopupMenu(&menu, mDDBRect.GetBottomLeft());
#endif
    }
    // -------------------------------------------------------------------------------
    bool DefaultTabCtrl::IsActiveTabVisible(const TabInfoVec_t& tabs, const wxRect & crect, int * gap) const
    {
        wxRect clientRect(crect);
        if (gap)
            *gap = 0;
        if ((GetStyle() & kNotebook_ShowFileListButton) && !IsVerticalStyle())
        {
            clientRect.SetWidth(clientRect.GetWidth() - CHEVRON_SIZE - CHEVRONPadding_SIZE * 2);
        }
        else if ((GetStyle() & kNotebook_ShowFileListButton) && IsVerticalStyle())
        {
            // Vertical tabs
            clientRect.SetHeight(clientRect.GetHeight() - CHEVRON_SIZE - CHEVRONPadding_SIZE * 2);
        }

        if (mActiveTab)
        {
            if (IsVerticalStyle())
            {
                const wxRect & tabRect = mActiveTab->GetRect();
                if (clientRect.Intersects(tabRect))
                {
                    return true;
                }
                else
                {
                    if (gap)
                    {
                        *gap = tabRect.y - clientRect.y - clientRect.height + tabRect.height;
                    }
                }
            }
            else
            {
                const wxRect & tabRect = mActiveTab->GetRect();
                //wxRect tabRect1 = tabRect;
                //tabRect1.SetWidth(tabRect1.GetWidth() * 0.7);
                if (clientRect.Contains(tabRect))
                {
                    return true;
                }
                else
                {
                    if (gap)
                    {
                        *gap = tabRect.x - clientRect.x - clientRect.width + tabRect.width;
                    }
                }
            }
        }
        
        return false;
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::OnSize(wxSizeEvent& event)
    {
        UpdateVisibleTabs(true);
        wxRect rect(GetClientRect());
        if (GetStyle() & kNotebook_ShowFileListButton)
        {
            if (IsVerticalStyle())
            {
                wxPoint rightPoint = rect.GetRightBottom();
                rightPoint.y -= CHEVRON_SIZE + CHEVRONPadding_SIZE;
                rightPoint.x += (rect.GetWidth() - CHEVRON_SIZE) / 2;
                mDDBRect = wxRect(rightPoint, wxSize(CHEVRON_SIZE, CHEVRON_SIZE));
                //mDDBRect = wxRect(rect.GetTopLeft(), wxSize(CHEVRON_SIZE, CHEVRON_SIZE));
                //rect.y = mDDBRect.GetBottomLeft().y;
            }
            else
            {
                wxPoint rightPoint = rect.GetRightTop();
                rightPoint.x -= CHEVRON_SIZE + CHEVRONPadding_SIZE;
                rightPoint.y += (rect.GetHeight() - CHEVRON_SIZE) / 2;
                mDDBRect = wxRect(rightPoint, wxSize(CHEVRON_SIZE, CHEVRON_SIZE));
            }
        }
        else
        {
            mDDBRect = wxRect();
        }
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::OnPaint(wxPaintEvent & e)
    {
        wxAutoBufferedPaintDC gcdc(this);

        wxRect clientRect(GetClientRect());
        if (clientRect.width <= 3 || clientRect.height <= 3)
            return;

        wxRect rect = clientRect;
        if (GetStyle() & kNotebook_ShowFileListButton)
        {
            if (IsVerticalStyle())
            {
                rect.SetHeight(rect.GetHeight() - (rect.GetRightTop().x - mDDBRect.y));
            }
            else
            {
                rect.SetWidth(rect.GetWidth() - (rect.GetRightBottom().x - mDDBRect.x));
            }
        }
        if (m_tabs.empty())
        {
            gcdc.SetPen(m_colours.mArea);
            gcdc.SetBrush(m_colours.mArea);
            gcdc.DrawRectangle(GetClientRect());
            return;
        }
        if (rect.GetSize().x <= 0 || rect.GetSize().y <= 0)
        {
            return;
        }

#ifdef __WXOSX__
        clientRect.Inflate(1);
#endif

        mView->DrawBackground(this, gcdc, clientRect, m_colours, m_style);

        if (!IsVerticalStyle() && (GetStyle() & kNotebook_ShowFileListButton))
        {
            gcdc.SetClippingRegion(clientRect.x, clientRect.y, mDDBRect.x - CHEVRONPadding_SIZE, clientRect.height);
        }

        //mView->DrawBackground(this, gcdc, rect, m_colours, m_style);

        for (int i = (mVisibleList.size() - 1); i >= 0; --i)
        {
            TabInfoPtr_t tab = mVisibleList.at(i);
            mView->Draw(this, gcdc, gcdc, *tab.get(), m_colours, m_style);
        }

        if (!IsVerticalStyle() && (GetStyle() & kNotebook_ShowFileListButton))
        {
            gcdc.DestroyClippingRegion();
        }
        if ((GetStyle() & kNotebook_ShowFileListButton))
        {
            if (IsVerticalStyle())
            {
                gcdc.SetBrush(m_colours.mInactiveBg);
                gcdc.SetPen(m_colours.mInactiveBg);
                gcdc.DrawRectangle(mDDBRect);
            }
            gcdc.SetPen(m_colours.mInactivePen);
            mView->DrawChevron(this, gcdc, mDDBRect, m_colours, mDDBHover);
        }
        mView->FinaliseBackground(this, gcdc, clientRect, m_colours, m_style);
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::OnLeftDown(wxMouseEvent& event)
    {
        event.Skip();

        if (mVisibleIdx == wxNOT_FOUND)
            return;

        if ((GetStyle() & kNotebook_ShowFileListButton) && mDDBRect.Contains(event.GetPosition()))
        {
            return;
        }

        if ((GetStyle() & kNotebook_CloseButtonOnActiveTab)) {
            TabInfoPtr_t t = mVisibleList.at(mVisibleIdx);
            if (t->GetCloseButtonRect().Contains(event.GetPosition()))
            {
                t->removeXButtonMark(Button::State::BST_Normal | Button::State::BST_Hover);
                t->addXButtonMark(Button::State::BST_Press | Button::State::BST_Visable);
                mLastCloseIdx = mTabIdx;
                Refresh();
                return;
            }
        }
        mLastCloseIdx = wxNOT_FOUND;

        if (GetSelection() != mTabIdx) { SetSelection(mTabIdx); }

        if ((m_style & kNotebook_AllowDnD))
        {
            wxCHECK_RET(!mTabDragStartTime.IsValid(), "A leftdown event when Tab DnD was already starting/started");
            mTabDragStartTime = wxDateTime::UNow();
            mTabDragStartPos = wxPoint(event.GetX(), event.GetY());
        }
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::OnLeftUp(wxMouseEvent& event)
    {
        event.Skip();

        mTabDragStartTime.Set((time_t)-1); // Not considering D'n'D so reset any saved values
        mTabDragStartPos = wxPoint();

        if ((GetStyle() & kNotebook_ShowFileListButton) && mDDBRect.Contains(event.GetPosition())) {
            CallAfter(&DefaultTabCtrl::DoShowTabList);
            mShowTabList = true;
        }
        else
        {
            if (mLastCloseIdx != wxNOT_FOUND)
            {
                if ((GetStyle() & kNotebook_CloseButtonOnActiveTab))
                {
                    wxRect xRect = m_tabs.at(mLastCloseIdx)->GetCloseButtonRect();
                    if (xRect.Contains(event.GetPosition()))
                    {
                        if (GetStyle() & kNotebook_CloseButtonOnActiveTabFireEvent)
                        {
                            wxBookCtrlEvent event(wxEVT_BOOK_PAGE_CLOSE_BUTTON);
                            event.SetEventObject(GetParent());
                            event.SetSelection(mLastCloseIdx);
                            GetParent()->GetEventHandler()->AddPendingEvent(event);
                        }
                        else
                        {
                            CallAfter(&DefaultTabCtrl::DoDeletePage, mLastCloseIdx);
                        }
                    }
                }
                UpdateVisibleTabs(false);
                mLastCloseIdx = wxNOT_FOUND;
            }
            else

                mShowTabList = false;
        }
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::OnMotion(wxMouseEvent& event)
    {
        event.Skip();
        for (size_t i = 0; i < mVisibleList.size(); ++i)
        {
            TabInfoPtr_t tab = mVisibleList.at(i);
            tab->removeXButtonMark(Button::State::BST_Visable | Button::State::BST_Press | Button::State::BST_Hover);
        }
        if (mDDBRect.Contains(event.GetPosition()))
        {
            mDDBHover = true;
        }
        else
        {
            mDDBHover = false;

            wxString curtip = GetToolTipText();
            eDirection align;
            TestPoint(event.GetPosition(), mTabIdx, mVisibleIdx, align);
            if (mTabIdx == wxNOT_FOUND)
            {
                if (!curtip.IsEmpty()) { SetToolTip(""); }
            }
            else
            {
                wxString pagetip = m_tabs.at(mTabIdx)->GetTooltip();
                if (pagetip != curtip) { SetToolTip(pagetip); }
            }

            if (mTabDragStartTime.IsValid()) { // If we're tugging on the tab, consider starting D'n'D
                wxTimeSpan diff = wxDateTime::UNow() - mTabDragStartTime;
                if (diff.GetMilliseconds() > 100 && // We need to check both x and y distances as tabs may be vertical
                    ((abs(mTabDragStartPos.x - event.GetX()) > 10) || (abs(mTabDragStartPos.y - event.GetY()) > 10))) {
                    OnBeginDrag();
                }
            }
            //if (mTabIdx != wxNOT_FOUND)
            //{
                //if (!mDDBRect.Contains(event.GetPosition()))
                //{
                //    m_tabs[mTabIdx]->addXButtonMark(Button::State::BST_Visable | Button::State::BST_Normal);
                //}
            //}

            if (mTabIdx != wxNOT_FOUND)
            {
                TabInfoPtr_t t = m_tabs[mTabIdx];
                m_tabs[mTabIdx]->addXButtonMark(Button::State::BST_Visable | Button::State::BST_Normal);
                if ((GetStyle() & kNotebook_CloseButtonOnActiveTab) && t->GetCloseButtonRect().Contains(event.GetPosition()))
                {
                    t->removeXButtonMark(Button::State::BST_Normal);
                    t->addXButtonMark((event.LeftIsDown() ? Button::State::BST_Press : Button::State::BST_Hover) | Button::State::BST_Visable);
                }
                else
                {
                    t->addXButtonMark(Button::State::BST_Normal | Button::State::BST_Visable);
                }
            }
        }
        Refresh();
    }
    //--------------------------------------------------------------------------------
    void DefaultTabCtrl::OnLeaveWindow(wxMouseEvent& event)
    {
        event.Skip();

        for (size_t i = 0; i < mVisibleList.size(); ++i)
        {
            TabInfoPtr_t tab = mVisibleList.at(i);
            tab->removeXButtonMark(Button::State::BST_Visable | Button::State::BST_Press |
                Button::State::BST_Hover);
        }
        mDDBHover = false;
        Refresh();
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::OnMouseMiddleClick(wxMouseEvent& event)
    {
        event.Skip();
        if (GetStyle() & kNotebook_MouseMiddleClickClosesTab)
        {
            if (mTabIdx != wxNOT_FOUND)
            {
                CallAfter(&DefaultTabCtrl::DoDeletePage, mTabIdx);
            }
        }
        else if (GetStyle() & kNotebook_MouseMiddleClickFireEvent)
        {
            if (mTabIdx != wxNOT_FOUND)
            {
                // Just fire an event
                wxBookCtrlEvent e(wxEVT_BOOK_PAGE_CLOSE_BUTTON);
                e.SetEventObject(GetParent());
                e.SetSelection(mTabIdx);
                GetParent()->GetEventHandler()->AddPendingEvent(e);
            }
        }
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::OnContextMenu(wxContextMenuEvent& event)
    {
        event.Skip();

        wxPoint pt = ::wxGetMousePosition();
        pt = ScreenToClient(pt);

        if ((mTabIdx != wxNOT_FOUND))
        {
            if (m_contextMenu && (mTabIdx == GetSelection()))
            {
                PopupMenu(m_contextMenu);
            }
            else
            {
                wxBookCtrlEvent menuEvent(wxEVT_BOOK_TAB_CONTEXT_MENU);
                menuEvent.SetEventObject(GetParent());
                menuEvent.SetSelection(mTabIdx);
                GetParent()->GetEventHandler()->ProcessEvent(menuEvent);
            }
        }
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::OnLeftDClick(wxMouseEvent& event)
    {
        event.Skip();
        if (mVisibleIdx == wxNOT_FOUND) {
            // Fire background d-clicked event
            wxBookCtrlEvent e(wxEVT_BOOK_TABAREA_DCLICKED);
            e.SetEventObject(GetParent());
            GetParent()->GetEventHandler()->AddPendingEvent(e);
        }
        /*else {
            // a tab was hit
            wxBookCtrlEvent e(wxEVT_BOOK_TAB_DCLICKED);
            e.SetEventObject(GetParent());
            e.SetSelection(mTabIdx);
            GetParent()->GetEventHandler()->AddPendingEvent(e);
        }*/
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::DoDrawBottomBox(TabInfoPtr_t tabinfo, const wxRect& clientRect, wxDC& dc,
        const TabColours& colours)
    {
        mView->DrawBottomRect(this, tabinfo, clientRect, dc, colours, GetStyle());
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::OnMouseScroll(wxMouseEvent& event)
    {
        event.Skip();
        if (GetStyle() & kNotebook_MouseScrollSwitchTabs)
        {
            size_t curSelection = GetSelection();
            if (event.GetWheelRotation() > 0)
            {
                if (curSelection > 0) { SetSelection(curSelection - 1); }
            }
            else
            {
                if (curSelection < GetTabs().size()) { SetSelection(curSelection + 1); }
            }
        }
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::OnBeginDrag()
    {
        mTabDragStartTime.Set((time_t)-1); // Reset the saved values
        mTabDragStartPos = wxPoint();

        // We simply drag the active tab index
        s_clTabCtrlDnD_Data.srcTabCtrl = this;
        s_clTabCtrlDnD_Data.tabIndex = GetSelection();

        wxTextDataObject dragContent("DefaultTabCtrl");
        wxDropSource dragSource(this);
        dragSource.SetData(dragContent);
        wxDragResult result = dragSource.DoDragDrop(true);
        wxUnusedVar(result);
    }
    // -------------------------------------------------------------------------------
    void DefaultTabCtrl::OnColoursChanged(Event& event)
    {
        event.Skip();
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        Refresh();
    }
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    // ScrolledTabCtrl
    // -------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    ScrolledTabCtrl::ScrolledTabCtrl(wxWindow* notebook, size_t style, const wxSize & size) :
        TabCtrl(notebook, style),
        ScrolledWindow(notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS | wxTAB_TRAVERSAL),
        mItemSize(size)
    {
        mMe = this;
        SetAutoLayout(false);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        DoSetBestSize();

        if (m_style & kNotebook_DarkTabs)
        {
            m_colours.setDark();
        }
        else
        {
            m_colours.setLight();
        }
        SetStyle(m_style, false);
        m_colours.mArea = gsgs_Style().GetAppBgColour();
        m_colours.mInactiveText = gsgs_Style().GetAppTextColour();

        Bind(wxEVT_PAINT, &ScrolledTabCtrl::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) {});
        //Bind(wxEVT_LEFT_DOWN, &ScrolledTabCtrl::OnLeftDown, this);
        //Bind(wxEVT_LEFT_UP, &ScrolledTabCtrl::OnLeftUp, this);
        //Bind(wxEVT_MOTION, &ScrolledTabCtrl::OnMotion, this);
        Bind(wxEVT_LEAVE_WINDOW, &ScrolledTabCtrl::OnLeaveWindow, this);
        Bind(wxEVT_MOUSEWHEEL, &ScrolledTabCtrl::OnMouseScroll, this);
    }
    // -------------------------------------------------------------------------------
    ScrolledTabCtrl::~ScrolledTabCtrl()
    {
        Unbind(wxEVT_PAINT, &ScrolledTabCtrl::OnPaint, this);
        Unbind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) {});
        //Unbind(wxEVT_LEFT_DOWN, &ScrolledTabCtrl::OnLeftDown, this);
        //Unbind(wxEVT_LEFT_UP, &ScrolledTabCtrl::OnLeftUp, this);
        //Unbind(wxEVT_MOTION, &ScrolledTabCtrl::OnMotion, this);
        Unbind(wxEVT_LEAVE_WINDOW, &ScrolledTabCtrl::OnLeaveWindow, this);
        Unbind(wxEVT_MOUSEWHEEL, &ScrolledTabCtrl::OnMouseScroll, this);
    }
    // -------------------------------------------------------------------------------
    void ScrolledTabCtrl::DoSetBestSize()
    {
        wxWindowUpdateLocker ttlock(this);
        int bmpHeight = GetDefaultBitmapHeight(mView->ySpacer);

        mHeight = bmpHeight + mView->ySpacer * 2 + SB_WIDTH;
        mWidth = bmpHeight + (mView->xSpacer + mView->majorCurveWidth + mView->smallCurveWidth) * 2 + SB_WIDTH;

        if (IsVerticalStyle())
        {
            SetSizeHints(mWidth, -1);
            SetSize(mWidth, -1);
        }
        else
        {
            SetSizeHints(-1, mHeight);
            SetSize(-1, mHeight);
        }
    }
    //--------------------------------------------------------------------------------
    bool ScrolledTabCtrl::ShiftRight(TabInfoVec_t& tabs, const wxRect & crect, int gab)
    {
        // Move the first tab from the list and adjust the remainder
        // of the tabs x coordiate
        if (tabs.size())
        {
            SetHThumbPosition(gab, true);
            return true;
        }
        return false;
    }
    //--------------------------------------------------------------------------------
    bool ScrolledTabCtrl::ShiftBottom(TabInfoVec_t& tabs, const wxRect & crect, int gab)
    {
        // Move the first tab from the list and adjust the remainder
        // of the tabs y coordiate
        if (tabs.size())
        {
            SetVThumbPosition(gab, true);
            return true;
        }
        return false;
    }
    //--------------------------------------------------------------------------------
    int ScrolledTabCtrl::GetDefaultBitmapHeight(int Y_spacer)
    {
        int bmpHeight = 0;
        if (mItemSize.x == -1 || mItemSize.y == -1)
        {
            wxBitmap dummyBmp = gsgs_Image().LoadBitmap("scrollTab");
            if (dummyBmp.IsOk())
            {
                bmpHeight = dummyBmp.GetScaledHeight();
                if (mItemSize.x == -1)
                    mItemSize.x = dummyBmp.GetSize().x;
                if (mItemSize.y == -1)
                    mItemSize.y = dummyBmp.GetSize().y;
            }
            else
            {
                bmpHeight = 20;
                if (mItemSize.x == -1)
                    mItemSize.x = 20;
                if (mItemSize.y == -1)
                    mItemSize.y = 50;
            }
        }
        else
        {
            bmpHeight = mItemSize.y;
        }
        return bmpHeight;
    }
    // -------------------------------------------------------------------------------
    bool ScrolledTabCtrl::IsActiveTabVisible(const TabInfoVec_t& tabs, const wxRect & crect, int * gap) const
    {
        wxRect clientRect(crect);
        if (gap)
            *gap = 0;
        clientRect.Offset(m_hsb->GetThumbPosition(), m_vsb->GetThumbPosition());

        if (mActiveTab)
        {
            if (IsVerticalStyle())
            {
                const wxRect & tabRect = mActiveTab->GetRect();
                if (clientRect.Contains(tabRect))
                {
                    return true;
                }
                else
                {
                    if (gap)
                    {
                        *gap = tabRect.y - clientRect.y - clientRect.height + tabRect.height + m_vsb->GetThumbPosition();
                    }
                }
            }
            else
            {
                const wxRect & tabRect = mActiveTab->GetRect();
                //tabRect.SetWidth(tabRect.GetWidth() * 0.7);
                if (clientRect.Contains(tabRect))
                {
                    return true;
                }
                else
                {
                    if (gap)
                    {
                        *gap = tabRect.x - clientRect.x - clientRect.width + tabRect.width + m_hsb->GetThumbPosition();
                    }
                }
            }
        }
        return false;
    }
    // -------------------------------------------------------------------------------
    void ScrolledTabCtrl::OnSize(wxSizeEvent& event)
    {
        //ScrolledWindow::OnSize(event);
        wxSize csize = event.GetSize();
        wxSize tsize = event.GetSize();
        if (IsVerticalStyle())
        {
            wxSize bsize = wxSize(mItemSize.GetWidth(), mItemSize.GetHeight() * m_tabs.size());
            if (tsize.x < bsize.x)
            {
                csize.y = csize.y - SB_WIDTH;

                if (csize.y < bsize.y)
                    csize.x = csize.x - SB_WIDTH;
            }
            else if (tsize.y < bsize.y)
            {
                csize.x = csize.x - SB_WIDTH;

                if (csize.x < bsize.x)
                    csize.y = csize.y - SB_WIDTH;
            }

            if (csize.y < bsize.y)
            {
                wxWindowUpdateLocker templ(m_vsb);
                m_vsb->SetSize(tsize.GetWidth() - SB_WIDTH, 0, SB_WIDTH, csize.y);
            }
            if (csize.x < bsize.x)
            {
                wxWindowUpdateLocker templ(m_hsb);
                m_hsb->SetSize(0, tsize.GetHeight() - SB_WIDTH, csize.x, SB_WIDTH);
            }

            UpdateVScrollBar(GetVThumbPosition(), csize.y, bsize.GetHeight(), csize.y, false, true);
            UpdateHScrollBar(GetHThumbPosition(), csize.x, bsize.GetWidth(), csize.x, false, true);
        }
        else
        {
            wxSize bsize = wxSize(mItemSize.GetHeight() * m_tabs.size(), mItemSize.GetWidth());
            if (tsize.x < bsize.x)
            {
                csize.y = csize.y - SB_WIDTH;

                if (csize.y < bsize.y)
                    csize.x = csize.x - SB_WIDTH;
            }
            else if (tsize.y < bsize.y)
            {
                csize.x = csize.x - SB_WIDTH;

                if (csize.x < bsize.x)
                    csize.y = csize.y - SB_WIDTH;
            }

            if (csize.y < bsize.y)
            {
                wxWindowUpdateLocker templ(m_vsb);
                m_vsb->SetSize(tsize.GetWidth() - SB_WIDTH, 0, SB_WIDTH, csize.y);
            }
            if (csize.x < bsize.x)
            {
                wxWindowUpdateLocker templ(m_hsb);
                m_hsb->SetSize(0, tsize.GetHeight() - SB_WIDTH, csize.x, SB_WIDTH);
            }

            UpdateVScrollBar(GetVThumbPosition(), csize.y, bsize.GetHeight(), csize.y, false, true);
            UpdateHScrollBar(GetHThumbPosition(), csize.x, bsize.GetWidth(), csize.x, false, true);
        }

        UpdateVisibleTabs(true);
        Refresh();
    }
    //--------------------------------------------------------------------------------
    void ScrolledTabCtrl::OnMouseWheel(wxMouseEvent& event)
    {
        if (IsVerticalStyle())
        {
            event.m_linesPerAction *= mItemSize.GetHeight();
            event.m_wheelAxis = wxMOUSE_WHEEL_VERTICAL;
        }
        else
        {
            event.m_linesPerAction *= mItemSize.GetWidth();
            event.m_wheelAxis = wxMOUSE_WHEEL_HORIZONTAL;
        }
        ScrolledWindow::OnMouseWheel(event);
    }
    // -------------------------------------------------------------------------------
    void ScrolledTabCtrl::OnPaint(wxPaintEvent & e)
    {
        wxAutoBufferedPaintDC gcdc(this);
        wxPoint pt = gcdc.GetDeviceOrigin();
#ifdef __WXGTK__
        // It may actually be correct to always query
        // the m_sign from the DC here, but I leave the
        // #ifdef GTK for now.
        if (gcdc.GetLayoutDirection() == wxLayout_RightToLeft)
            gcdc.SetDeviceOrigin(pt.x + GetHThumbPosition(), pt.y - GetVThumbPosition());
        else
#endif
            gcdc.SetDeviceOrigin(pt.x - GetHThumbPosition(), pt.y - GetVThumbPosition());

        wxRect clientRect(GetClientRect());
        if (clientRect.width <= 3 || clientRect.height <= 3)
            return;

        if (m_tabs.empty())
        {
            gcdc.SetPen(m_colours.mArea);
            gcdc.SetBrush(m_colours.mArea);
            gcdc.DrawRectangle(GetClientRect());
            return;
        }

#ifdef __WXOSX__
        clientRect.Inflate(1);
#endif

        mView->DrawBackground(this, gcdc, clientRect, m_colours, m_style);

        gcdc.SetDeviceClippingRegion(GetClientArea());
        for (int i = (mVisibleList.size() - 1); i >= 0; --i)
        {
            TabInfoPtr_t tab = mVisibleList.at(i);
            mView->Draw(this, gcdc, gcdc, *tab.get(), m_colours, m_style);
        }
        gcdc.DestroyClippingRegion();

        mView->FinaliseBackground(this, gcdc, clientRect, m_colours, m_style);
    }
    // -------------------------------------------------------------------------------
    void ScrolledTabCtrl::OnLeftDown(wxMouseEvent& event)
    {
        event.Skip();

        if (GetSelection() != mTabIdx) 
        { 
            SetSelection(mTabIdx); 
        }
    }
    // -------------------------------------------------------------------------------
    void ScrolledTabCtrl::OnLeftUp(wxMouseEvent& event)
    {
        event.Skip();
    }
    // -------------------------------------------------------------------------------
    void ScrolledTabCtrl::OnMotion(wxMouseEvent& event)
    {
        event.Skip();
        for (size_t i = 0; i < mVisibleList.size(); ++i)
        {
            TabInfoPtr_t tab = mVisibleList.at(i);
            tab->removeXButtonMark(Button::State::BST_Visable | Button::State::BST_Press | Button::State::BST_Hover);
        }
        wxString curtip = GetToolTipText();
        eDirection align;
        TestPoint(event.GetPosition() + wxPoint(m_hsb->GetThumbPosition(), m_vsb->GetThumbPosition()), mTabIdx, mVisibleIdx, align);
        if (mTabIdx == wxNOT_FOUND)
        {
            if (!curtip.IsEmpty()) 
            { 
                SetToolTip(""); 
            }
        }
        else
        {
            wxString pagetip = m_tabs.at(mTabIdx)->GetTooltip();
            if (pagetip != curtip) 
            { 
                SetToolTip(pagetip); 
            }
        }
        
        if (mTabIdx != wxNOT_FOUND)
        {
            m_tabs[mTabIdx]->addXButtonMark(Button::State::BST_Normal | Button::State::BST_Visable);
        }

        Refresh();
    }
    //--------------------------------------------------------------------------------
    void ScrolledTabCtrl::OnLeaveWindow(wxMouseEvent& event)
    {
        event.Skip();

        for (size_t i = 0; i < mVisibleList.size(); ++i)
        {
            TabInfoPtr_t tab = mVisibleList.at(i);
            tab->removeXButtonMark(Button::State::BST_Visable | Button::State::BST_Press |
                Button::State::BST_Hover);
        }
        Refresh();
    }
    //--------------------------------------------------------------------------------
    void ScrolledTabCtrl::CalculateOffsets(TabInfo * info, size_t style, const wxRect & rect)
    {
        wxGCDC dc;
        int Y_spacer = mView->ySpacer;
        int X_spacer = mView->xSpacer;
        int M_spacer = mView->majorCurveWidth;
        int S_spacer = mView->smallCurveWidth;

        //wxFont font = TabView::GetTabFont(true);
        wxFont font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxT("ËÎÌו"));
        dc.SetFont(font);

        wxSize sz = dc.GetTextExtent(info->m_label);

        //info->m_width = X_spacer + M_spacer + S_spacer;
        bool bVerticalTabs = IS_VERTICAL_TABS(style);
        if (bVerticalTabs)
        {
            info->m_width = rect.GetWidth();
            info->m_height = mItemSize.y ? mItemSize.y : sz.GetHeight();
        }
        else
        {
            info->m_width = mItemSize.x ? mItemSize.x : sz.GetWidth();
            info->m_height = rect.GetHeight();
        }

        // Text
        info->m_textY = ((info->m_height - sz.y) / 2);
        //Bitmap
        if (info->m_bitmap.IsOk()) {
            info->m_bmpX = ((info->m_width - info->m_bitmap.GetScaledWidth()) / 2);
            //info->m_width += info->m_bitmap.GetScaledWidth();
            info->m_bmpY = ((info->m_height - info->m_bitmap.GetScaledHeight()) / 2);
        }
        else
        {
            info->m_bmpX = wxNOT_FOUND;
            info->m_bmpY = wxNOT_FOUND;
        }
        info->m_textWidth = sz.x;
        info->m_textHeight = sz.y;
        // x button
        /*wxRect xrect;
        if ((style & kNotebook_CloseButtonOnActiveTab)) {
            info->m_width += X_spacer;
            xrect = wxRect(info->m_width, info->m_height, X_BUTTON_SIZE, X_BUTTON_SIZE);
            info->m_bmpCloseX = xrect.GetX();
            //m_bmpCloseY = 0; // we will fix this later
            info->m_bmpCloseY = ((info->m_height - X_BUTTON_SIZE) / 2);
            info->m_width += xrect.GetWidth();
        }*/
        if(info->m_width - sz.x < 0)
            info->m_textX = ((sz.x - info->m_width) / 2);
        else
            info->m_textX = ((info->m_width - sz.x) / 2);
        //info->m_width += X_spacer + M_spacer + S_spacer;
        if ((style & kNotebook_UnderlineActiveTab) && bVerticalTabs) 
        { 
            info->m_width += 8; 
        }

        info->m_rect.SetWidth(info->m_width);
        info->m_rect.SetHeight(info->m_height);
    }
    //--------------------------------------------------------------------------------
    wxRect ScrolledTabCtrl::GetContentArea() const
    {
        return GetClientArea();
    }
    //--------------------------------------------------------------------------------
    void ScrolledTabCtrl::ScrollToRow(int firstLine)
    {
        //m_vsb->SetThumbPosition(firstLine);
        Refresh();
    }
    //--------------------------------------------------------------------------------
    void ScrolledTabCtrl::ScollToColumn(int firstColumn)
    {
        //m_hsb->SetThumbPosition(firstColumn);
        Refresh();
    }
    // -------------------------------------------------------------------------------
    void ScrolledTabCtrl::DoDrawBottomBox(TabInfoPtr_t tabinfo, const wxRect& clientRect, wxDC& dc,
        const TabColours& colours)
    {
        mView->DrawBottomRect(this, tabinfo, clientRect, dc, colours, GetStyle());
    }
    // -------------------------------------------------------------------------------
    void ScrolledTabCtrl::OnMouseScroll(wxMouseEvent& event)
    {
        event.Skip();
        if (GetStyle() & kNotebook_MouseScrollSwitchTabs)
        {
            size_t curSelection = GetSelection();
            if (event.GetWheelRotation() > 0)
            {
                if (curSelection > 0) 
                { 
                    SetSelection(curSelection - 1); 
                }
            }
            else
            {
                if (curSelection < GetTabs().size()) 
                { 
                    SetSelection(curSelection + 1); 
                }
            }
        }
    }
    // -------------------------------------------------------------------------------
    void ScrolledTabCtrl::OnColoursChanged(Event& event)
    {
        event.Skip();
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        Refresh();
    }
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    // Notebook
    //--------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    Notebook::Notebook(wxWindow* parent, wxWindowID id, const wxSize & itemsize, const wxPoint& pos,
        const wxSize & size, long tabstyle, long style, TabType tabtype, const wxString& name):
        wxPanel(parent, id, pos, size, wxWANTS_CHARS | style, name)
    {
        wxWindowUpdateLocker nnl(this);
        SetAutoLayout(false);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());

        //static bool once = false;
        /*if(!once) 
        {
            // Add PNG and Bitmap handler
            wxImage::AddHandler(new wxPNGHandler);
            wxXmlResource::Get()->AddHandler(new wxBitmapXmlHandler);
            Notebook_Init_Bitmaps();
            once = true;
        }*/
        //tabstyle = (tabstyle & ~wxWINDOW_STYLE_MASK); // filter out wxWindow styles
        if (tabtype == TT_Mark)
        {
            mTab = new DefaultTabCtrl(this, tabstyle, itemsize);
        }
        else if (tabtype == TT_Scroll)
        {
            mTab = new ScrolledTabCtrl(this, tabstyle, itemsize);
        }
        else
        {
            mTab = new DefaultTabCtrl(this, tabstyle, itemsize);
        }
        mWinList = new WindowStack(this);
        PositionControls();
    }
    // -------------------------------------------------------------------------------
    Notebook::~Notebook() 
    {
    }
    // -------------------------------------------------------------------------------
    void Notebook::PositionControls()
    {
        size_t style = GetStyle();

        // Detach the controls from the main sizer
        if(GetSizer()) {
            GetSizer()->Detach(mWinList);
            GetSizer()->Detach(mTab->getMe());
        }

        // Set new sizer
        if(style & kNotebook_BottomTabs) 
        {
            // Tabs are placed under the pages
            SetSizer(new wxBoxSizer(wxVERTICAL));
            GetSizer()->Add(mWinList, 1, wxEXPAND);
            GetSizer()->Add(mTab->getMe(), 0, wxEXPAND);
        } 
        else if(style & kNotebook_LeftTabs) 
        {
            // Tabs are placed to the left of the pages
            SetSizer(new wxBoxSizer(wxHORIZONTAL));
            GetSizer()->Add(mTab->getMe(), 0, wxEXPAND);
            GetSizer()->Add(mWinList, 1, wxEXPAND);
        } 
        else if(style & kNotebook_RightTabs) 
        {
            // Tabs are placed to the right of the pages
            SetSizer(new wxBoxSizer(wxHORIZONTAL));
            GetSizer()->Add(mWinList, 1, wxEXPAND);
            GetSizer()->Add(mTab->getMe(), 0, wxEXPAND);
        } else {
            // Tabs are placed on top of the pages
            SetSizer(new wxBoxSizer(wxVERTICAL));
            GetSizer()->Add(mTab->getMe(), 0, wxEXPAND);
            GetSizer()->Add(mWinList, 1, wxEXPAND);
        }
        //wxWindowUpdateLocker ll(this);
        //Layout();
    }
    // -------------------------------------------------------------------------------
    void Notebook::AddPage(wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp)
    {
        TabInfoPtr_t tab(new TabInfo(mTab, GetStyle(), page, label, bmp));
        tab->SetActive(selected, GetStyle());
        mTab->AddPage(tab);
    }
    // -------------------------------------------------------------------------------
    bool Notebook::InsertPage(size_t index, wxWindow * page, const wxString& label, bool selected, const wxBitmap& bmp)
    {
        TabInfoPtr_t tab(new TabInfo(mTab, GetStyle(), page, label, bmp));
        tab->SetActive(selected, GetStyle());
        return mTab->InsertPage(index, tab);
    }
    // -------------------------------------------------------------------------------
    wxWindow * Notebook::GetCurrentPage() const
    {
        if(mTab->GetSelection() == wxNOT_FOUND) 
            return NULL;
        return mTab->GetPage(mTab->GetSelection());
    }
    // -------------------------------------------------------------------------------
    void Notebook::EnableStyle(NotebookStyle style, bool enable)
    {
        size_t flags = GetStyle();
        if(enable) {
            flags |= style;
        } else {
            flags &= ~style;
        }
        SetStyle(flags);
    }
    // -------------------------------------------------------------------------------
    void Notebook::SetTabDirection(wxDirection d)
    {
        size_t flags = GetStyle();
        // Clear all direction styles
        flags &= ~kNotebook_BottomTabs;
        flags &= ~kNotebook_LeftTabs;
        flags &= ~kNotebook_RightTabs;
    #if 0
        if(d == wxBOTTOM || d == wxRIGHT) 
        {
            flags |= kNotebook_BottomTabs;
        }
    #else
        if(d == wxBOTTOM) {
            flags |= kNotebook_BottomTabs;
        } else if(d == wxRIGHT) {
            flags |= kNotebook_RightTabs;
        } else if(d == wxLEFT) {
            flags |= kNotebook_LeftTabs;
        }
    #endif
        SetStyle(flags);
    }
    // -------------------------------------------------------------------------------
}