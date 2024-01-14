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

#ifndef GSGSNOTEBOOK_H
#define GSGSNOTEBOOK_H

#include "gsgsPreInclude.h"
#include "gsgsStyleManager.h"
#include "gsgsScrolledPanel.h"
#include <wx/menu.h>
#include <wx/notebook.h>
#include <wx/dcmemory.h>
#include <wx/dnd.h>
#include <wx/dc.h>
#include <wx/aui/auibook.h>
#include <wx/colour.h>
#include <wx/pen.h>

wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BOOK_PAGE_CHANGING, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BOOK_PAGE_CHANGED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BOOK_PAGE_CLOSING, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BOOK_PAGE_CLOSED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BOOK_TAB_CONTEXT_MENU, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BOOK_PAGE_CLOSE_BUTTON, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BOOK_TAB_DCLICKED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BOOK_TABAREA_DCLICKED, wxBookCtrlEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_BOOK_FILELIST_BUTTON_CLICKED, ContextMenuEvent);

namespace gsgs
{
    class Notebook;
    class TabCtrl;
    class TabView;

    class _gsgsAPI NotebookTabArt : public wxAuiDefaultTabArt
    {
    public:
        NotebookTabArt();
        virtual ~NotebookTabArt();

        virtual wxAuiTabArt* Clone() { return new NotebookTabArt(*this); }
        virtual void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);
        virtual void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& in_rect,
            int close_button_state, wxRect* out_tab_rect, wxRect* out_button_rect, int* x_extent);
        virtual wxSize GetTabSize(wxDC& dc, wxWindow* wnd, const wxString& caption, const wxBitmap& bitmap, bool active,
            int closeButtonState, int* xExtent);

        int GetAdditionalBorderSpace(wxWindow* wnd) { return 0; }
        void DrawButton(wxDC& dc, wxWindow* wnd, const wxRect& in_rect, int bitmap_id, int button_state, int orientation, wxRect* out_rect);
    public:
        void RefreshColours(long style);
    protected:
        // The tab area background colour
        wxColour m_bgColour;
        wxColour m_markerColour;

        // The tab area pen colour (used to mark the tab area borders)
        wxColour m_penColour;
        wxColour m_activeTabPenColour;

        // The text colour
        wxColour m_activeTabTextColour;
        wxColour m_tabTextColour;

        // A singe tab background colour
        wxColour m_activeTabBgColour;
        wxColour m_tabBgColour;

        double m_tabRadius;

        wxBitmap m_bmpClose;
        wxBitmap m_bmpCloseHover;
        wxBitmap m_bmpClosePressed;
        wxRect m_windowListButtonRect;
    };

    enum NotebookStyle {
        /// Use the built-in light tab colours
        kNotebook_LightTabs = (1 << 0),
        /// Use the built-in dark tab colours
        kNotebook_DarkTabs = (1 << 1),
        /// Allow tabs to move using DnD
        kNotebook_AllowDnD = (1 << 2),
        /// Draw X button on the active tab
        kNotebook_CloseButtonOnActiveTab = (1 << 3),
        /// Mouse middle click closes tab
        kNotebook_MouseMiddleClickClosesTab = (1 << 4),
        /// Show a drop down button for displaying all tabs list
        kNotebook_ShowFileListButton = (1 << 5),
        /// Mouse middle click on a tab fires an event
        kNotebook_MouseMiddleClickFireEvent = (1 << 6),
        /// Clicking the X button on the active button fires an event
        /// instead of closing the tab (i.e. let the container a complete control)
        kNotebook_CloseButtonOnActiveTabFireEvent = (1 << 7),
        /// Fire navigation event for Ctrl-TAB et al
        kNotebook_EnableNavigationEvent = (1 << 8),
        /// Place tabs at the bottom
        kNotebook_BottomTabs = (1 << 9),

        /// Allow DnD between different book controls
        kNotebook_AllowForeignDnD = (1 << 10),

        /// Place the tabs on the right
        kNotebook_RightTabs = (1 << 11),

        /// Place th tabs on the left
        kNotebook_LeftTabs = (1 << 12),

        /// The notebook colours are changing based on the current editor theme
        kNotebook_DynamicColours = (1 << 13),

        /// Underline the active tab with a 2 pixel line
        kNotebook_UnderlineActiveTab = (1 << 14),

        /// When scrolling with the mouse button when hovering the tab control, switch between tabs
        kNotebook_MouseScrollSwitchTabs = (1 << 15),

        kNotebook_ScrollTabs            = (1 << 16),

        /// Default notebook
        kNotebook_Default = kNotebook_LightTabs | kNotebook_ShowFileListButton,

    };
#define IS_VERTICAL_TABS(style) ((style & kNotebook_RightTabs) || (style & kNotebook_LeftTabs))
    class _gsgsAPI TabHistory
    {
    public:
        TabHistory() {}
        virtual ~TabHistory() {}

        void Push(wxWindow* page)
        {
            if (page == NULL) 
                return;
            Pop(page);
            m_history.insert(m_history.begin(), page);
        }

        void Pop(wxWindow* page)
        {
            if (!page) 
                return;
            WindowList::iterator iter =
                std::find_if(m_history.begin(), m_history.end(), [&](wxWindow* w) { return w == page; });
            if (iter != m_history.end())
            {
                m_history.erase(iter);
            }
        }

        wxWindow* PrevPage()
        {
            if (m_history.empty())
            {
                return NULL;
            }
            return m_history[0];
        }
        void Clear() { m_history.clear(); }

        inline const WindowList& GetHistory() const { return m_history; }
    private:
        WindowList m_history;
    };
    typedef wxSharedPtr<TabHistory> TabHistoryPtr_t;

    struct _gsgsAPI Tab
    {
        wxString text;
        wxWindow* window;
        wxBitmap bitmap;
        wxFileName filename;
        bool isModified;
        bool isFile;
        Tab() : window(NULL) , isFile(false) , isModified(false) {}
    };
    typedef std::vector<Tab> TabVec_t;

    enum class eDirection 
    {
        kInvalid = -1,
        kRight = 0,
        kLeft = 1,
        kUp = 2,
        kDown = 3,
    };

    class _gsgsAPI WindowStack : public wxWindow
    {
    public:
        WindowStack(wxWindow* parent, wxWindowID id = wxID_ANY);
        virtual ~WindowStack();

        bool Add(wxWindow* win, bool select);
        bool Remove(wxWindow* win);
        void Clear();
        bool Contains(wxWindow* win);
        inline bool IsEmpty() const             { return mWinList.empty(); }

        void Select(wxWindow* win);
        inline wxWindow* GetSelected() const    { return m_activeWin;  }
    protected:
        int FindPage(wxWindow* page) const;
        void ChangeSelection(size_t index);
        void DoSelect(wxWindow* win);
        void OnSize(wxSizeEvent& e);
        void DoHideNoActiveWindows();
    private:
        WindowList mWinList;
        wxWindow* m_activeWin;
    };

    class _gsgsAPI TabColours
    {
    public:
        TabColours();
        ~TabColours() {}

        bool isDark() const;

        void set(const wxColour& base, const wxColour& text);

        void setDark();

        void setLight();
    public:
        wxColour mInactiveBg;
        wxColour mInactivePen;
        wxColour mInactiveText;
        wxColour mInactiveInnerPen;
        wxColour mActiveText;
        wxColour mActiveBg;
        wxColour mActivePen;
        wxColour mActiveInnerPen;

        wxColour mArea;
        wxColour mMarker;
    };

    class _gsgsAPI TabInfo
    {
    public:
        TabInfo(TabCtrl * ctrl);
        TabInfo(TabCtrl * ctrl, size_t style, wxWindow* page, const wxString& text, const wxBitmap & bmp = wxNullBitmap);
        virtual ~TabInfo() {}

        inline bool IsValid() const             { return m_window != NULL; }
        void SetBitmap(const wxBitmap& bitmap, size_t style);
        void SetLabel(const wxString& label, size_t style);
        void SetActive(bool active, size_t style);
        void SetRect(const wxRect& rect)        { this->m_rect = rect; }
        inline void addXButtonMark(int i)       { mXButtonMark |= i; }
        inline void removeXButtonMark(int i)    { mXButtonMark &= ~i; }
        inline void setXButtonMark(int i)       { mXButtonMark = i; }
        inline int getXButtonMark()const        { return mXButtonMark; }
        inline const wxBitmap& GetBitmap() const{ return m_bitmap; }
        inline const wxString& GetLabel() const { return m_label; }
        inline const wxRect& GetRect() const    { return m_rect; }
        inline wxRect& GetRect()                { return m_rect; }
        inline wxRect GetCloseButtonRect() const;
        inline wxWindow* GetWindow()            { return m_window; }
        inline wxWindow* GetWindow() const      { return m_window; }
        inline void SetWindow(wxWindow* window) { this->m_window = window; }
        inline bool IsActive() const            { return mXButtonMark & Button::BST_Active; }
        inline bool IsHover() const             { return mXButtonMark & Button::BST_Hover; }
        inline int GetBmpCloseX() const         { return m_bmpCloseX; }
        inline int GetBmpCloseY() const         { return m_bmpCloseY; }
        inline int GetHeight() const            { return m_height; }
        inline int GetWidth() const             { return m_width; }
        inline void SetTooltip(const wxString& tooltip) { this->m_tooltip = tooltip; }
        inline const wxString& GetTooltip() const { return m_tooltip; }
        inline const wxBitmap& GetDisabledBitmp() const { return m_disabledBitmp; }
    public:
        TabCtrl* mTab;
        wxString m_label;
        wxString m_tooltip;
        wxWindow* m_window;
        wxRect m_rect;
        int m_textX;
        int m_textY;
        int m_bmpX;
        int m_bmpY;
        int m_bmpCloseX;
        int m_bmpCloseY;
        int m_width;
        int m_height;
        int m_vTabsWidth;
        int m_textWidth;
        int m_textHeight;
        int mXButtonMark;

        wxBitmap m_bitmap;
        wxBitmap m_disabledBitmp;
    };
    typedef wxSharedPtr<TabInfo> TabInfoPtr_t;
    typedef std::vector<TabInfoPtr_t> TabInfoVec_t;
    typedef wxSharedPtr<TabView> TabViewPtr_t;
    class _gsgsAPI TabView
    {
    public:
        TabView(const wxString& name);
        virtual ~TabView() {}
        virtual void Draw(wxWindow * parent, wxDC& dc, wxDC& fontDC, const TabInfo& tabInfo, const TabColours& colours, size_t style) = 0;
        virtual void DrawBottomRect(wxWindow * parent, TabInfoPtr_t activeTab, const wxRect& clientRect, wxDC& dc, const TabColours& colours, size_t style) = 0;
        virtual void DrawBackground(wxWindow * parent, wxDC& dc, const wxRect& clientRect, const TabColours& colours, size_t style);

        /**
         * @brief finalise the background after all elements have been drawn on the tab area colour. Default is
         * to do nothing
         */
        virtual void FinaliseBackground(wxWindow * parent, wxDC& dc, const wxRect& clientRect, const TabColours& colours, size_t style);
        /**
         * @brief reutrn font suitable for drawing the tab label
         */
        static wxFont GetTabFont(bool bold);

        /**
         * @brief draw a button in a given state at a give location
         */
        static void DrawCloseButton(wxWindow * win, wxDC & dc, const TabInfo & tabInfo, const TabColours& colours);

        /**
         * @brief draw cheveron button
         */
        static void DrawChevron(wxWindow * win, wxDC & dc, const wxRect & rect, const TabColours & colours, bool hover);

        static int GetXButtonSize();
        /**
         * @brief Adjust colours per renderer
         * @param colours [in/out]
         * @param style the notebook style
         */
        virtual void AdjustColours(TabColours& colours, size_t style);

        /**
         * @brief allocate new renderer based on gsgs's settings
         */
        static TabViewPtr_t CreateRenderer(size_t tabStyle);
        /**
         * @brief return list of availale renderers
         */
        static wxArrayString GetRenderers();

        /**
         * @brief return the marker pen width
         * @return
         */
        static int GetMarkerWidth();

        void SetName(const wxString& name)      { this->m_name = name; }

        const wxString& GetName() const         { return m_name; }
    protected:
        void ClearActiveTabExtraLine(TabInfoPtr_t activeTab, wxDC& dc, const TabColours& colours, size_t style);
        void DrawMarker(wxDC& dc, const TabInfo& tabInfo, const TabColours& colours, size_t style);
        void DrawMarkerLine(wxDC& dc, const wxPoint& p1, const wxPoint& p2, wxDirection direction);
    public:
        int bottomAreaHeight;     // BOTTOM_AREA_HEIGHT = 3;
        int majorCurveWidth;      // MAJOR_CURVE_WIDTH = 15;
        int smallCurveWidth;      // SMALL_CURVE_WIDTH = 4;
        int overlapWidth;         // OVERLAP_WIDTH = 20;
        int verticalOverlapWidth; // V_OVERLAP_WIDTH = 3;
        int xSpacer;
        int ySpacer;
        wxString m_name;
    };

    class _gsgsAPI TabCtrl
    {
        friend class Notebook;
    public:
        TabCtrl(wxWindow * notebook, size_t style, const wxSize & size = wxSize(-1, -1));
        virtual ~TabCtrl();

        wxWindow * getMe() const                            { return mMe; }

        bool IsVerticalStyle() const;

        void SetArt(TabViewPtr_t art);

        inline TabViewPtr_t GetArt()                        { return mView; }

        inline void SetColours(const TabColours& colurs)    { m_colours = colurs; }

        inline const TabColours& GetColours() const         { return m_colours; }

        /**
         * @brief test if pt is on one of the visible tabs return its index
         * @param pt mouse click position
         * @param realPosition [output] the index position in the m_tabs array
         * @param tabHit [output] the index position in the mVisibleList array
         * @param leftSide [output] if the point is on the LEFT side of the tab's rect, then return wxALIGN_LEFT, otherwise
         * return wxALIGN_RIGHT. Another possible value is wxALIGN_INVALID
         */
        void TestPoint(const wxPoint& pt, int& realPosition, int& tabHit, eDirection& align);

        /**
         * @brief Move the active tab to a new position
         * @param newIndex the new position. 0-based index in the m_tabs array
         */
        bool MoveActiveToIndex(int newIndex, eDirection direction);

        /**
         * @brief return true if index is in the tabs vector range
         */
        bool IsIndexValid(size_t index) const;

        void SetStyle(size_t style, bool refresh = true);

        size_t GetStyle() const                             { return m_style; }

        /**
         * @brief update the selected tab. This function also fires an event
         */
        virtual int SetSelection(size_t tabIdx);

        /**
         * @brief update the selected tab. This function does not fire an event
         */
        virtual int ChangeSelection(size_t tabIdx, bool refresh = true);

        /**
         * @brief return the current selection. return wxNOT_FOUND if non is selected
         */
        int GetSelection() const;

        /**
         * @brief Sets the text for the given page.
         */
        bool SetPageText(size_t page, const wxString& text);

        wxString GetPageText(size_t page) const;

        bool InsertPage(size_t index, TabInfoPtr_t tab);

        void AddPage(TabInfoPtr_t tab)              { InsertPage(m_tabs.size(), tab); }

        void SetPageBitmap(size_t index, const wxBitmap& bmp, bool refresh = true);

        wxBitmap GetPageBitmap(size_t index) const;
        wxWindow* GetPage(size_t index) const;
        void GetAllPages(WindowList& pages);
        int FindPage(wxWindow* page) const;
        bool RemovePage(size_t page, bool notify, bool deletePage);
        bool DeleteAllPages();

        TabInfoPtr_t GetTabInfo(size_t index);
        TabInfoPtr_t GetTabInfo(wxWindow* page);
        TabInfoPtr_t GetActiveTabInfo();

        virtual int GetDefaultBitmapHeight(int Y_spacer);

        virtual void CalculateOffsets(TabInfo * info, size_t style, const wxRect & rect);

        virtual wxRect GetContentArea() const;

        void SetMenu(wxMenu* menu);
        bool SetPageToolTip(size_t page, const wxString & tooltip);
        inline const TabInfoVec_t& GetTabs() const          { return m_tabs; }
        inline TabHistoryPtr_t GetHistory() const           { return m_history; }
    protected:
        int DoGetPageIndex(wxWindow* win) const;
        int DoGetPageIndex(const wxString& label) const;
        virtual bool ShiftRight(TabInfoVec_t& tabs, const wxRect & crect, int gab);
        virtual bool ShiftBottom(TabInfoVec_t& tabs, const wxRect & crect, int gab);
        bool IsActiveTabInList(const TabInfoVec_t& tabs) const;
        virtual bool IsActiveTabVisible(const TabInfoVec_t& tabs, const wxRect & crect, int * gap = 0) const;

        /**
         * @brief loop over the tabs and set their coordiantes
         */
        void DoUpdateCoordiantes(TabInfoVec_t& tabs, const wxRect & crect);
        /**
         * @brief get list of tabs to draw. This call always returns the active tab as part of the list
         * It also ensures that we draw as much tabs as we can.
         * @param offset reset the 0 based index from m_tabs
         */
        void UpdateVisibleTabs(bool forceReshuffle = false);

        /**
         * @brief calculate and set the tab ctrl size
         */
        virtual void DoSetBestSize();

        WindowStack* GetStack();

        void DoDeletePage(size_t page) { RemovePage(page, true, true); }
        void DoUpdateXCoordFromPage(wxWindow* page, int diff);
    protected:
        wxWindow * mMe;
        int mHeight;
        int mWidth;
        int mVisibleIdx;
        int mTabIdx;
        int mLastCloseIdx;
        size_t m_style;
        TabColours m_colours;
        TabInfoPtr_t mActiveTab;
        TabInfoVec_t m_tabs;
        TabInfoVec_t mVisibleList;
        wxMenu* m_contextMenu;
        wxRect mDDBRect;
        TabHistoryPtr_t m_history;
        TabViewPtr_t mView;

        wxDateTime mTabDragStartTime;
        wxPoint mTabDragStartPos;
        bool mDDBHover;
        bool mShowTabList;
        bool mScrollMode;
    };

    class _gsgsAPI DefaultTabCtrl : public TabCtrl, public wxPanel
    {
        friend class Notebook;
    public:
        DefaultTabCtrl(wxWindow * notebook, size_t style, const wxSize & size = wxSize(-1, -1));
        virtual ~DefaultTabCtrl();
    protected:
        void OnColoursChanged(Event& event);
        void OnPaint(wxPaintEvent& e);
        void OnSize(wxSizeEvent& event);
        void OnLeftDown(wxMouseEvent& event);
        void OnLeftUp(wxMouseEvent& event);
        void OnLeftDClick(wxMouseEvent& event);
        void OnMotion(wxMouseEvent& event);
        void OnLeaveWindow(wxMouseEvent& event);
        void OnMouseMiddleClick(wxMouseEvent& event);
        void OnMouseScroll(wxMouseEvent& event);
        void OnContextMenu(wxContextMenuEvent& event);
        void DoDrawBottomBox(TabInfoPtr_t activeTab, const wxRect& clientRect, wxDC& dc, const TabColours& colours);
        bool IsActiveTabVisible(const TabInfoVec_t& tabs, const wxRect & crect, int * gap = 0) const;

        /**
         * @brief calculate and set the tab ctrl size
         */
        void DoSetBestSize();

        void DoShowTabList();

        void DoDeletePage(size_t page) { RemovePage(page, true, true); }

        void OnBeginDrag();
    private:
    };

    class _gsgsAPI ScrolledTabCtrl : public TabCtrl, public ScrolledWindow
    {
        friend class Notebook;
    public:
        ScrolledTabCtrl(wxWindow * notebook, size_t style, const wxSize & size = wxSize(-1, -1));
        virtual ~ScrolledTabCtrl();

        size_t GetStyle() const { return m_style; }

        /// copydetails TabCtrl::CalculateOffsets
        virtual void CalculateOffsets(TabInfo * info, size_t style, const wxRect & rect);

        /// copydetails TabCtrl::GetContentArea
        virtual wxRect GetContentArea() const;

        /// copydetails ScrolledWindow::ScrollToRow
        virtual void ScrollToRow(int firstLine);

        /// @copydetails ScrolledWindow::ScollToColumn
        virtual void ScollToColumn(int firstColumn);
    protected:
        void OnColoursChanged(Event& event);
        void OnPaint(wxPaintEvent& e);
        void OnSize(wxSizeEvent& event);
        void OnMouseWheel(wxMouseEvent& event);
        void OnLeftDown(wxMouseEvent& event);
        void OnLeftUp(wxMouseEvent& event);
        void OnMotion(wxMouseEvent& event);
        void OnMouseScroll(wxMouseEvent& event);
        void OnLeaveWindow(wxMouseEvent& event);
        void DoDrawBottomBox(TabInfoPtr_t activeTab, const wxRect& clientRect, wxDC& dc, const TabColours& colours);
        bool IsActiveTabVisible(const TabInfoVec_t& tabs, const wxRect & crect, int * gap = 0) const;

        /**
         * @brief calculate and set the tab ctrl size
         */
        void DoSetBestSize();

        virtual bool ShiftRight(TabInfoVec_t& tabs, const wxRect & crect, int gab);
        virtual bool ShiftBottom(TabInfoVec_t& tabs, const wxRect & crect, int gab);
        virtual int GetDefaultBitmapHeight(int Y_spacer);

        void DoDeletePage(size_t page) { RemovePage(page, true, true); }
    private:
        wxSize mItemSize;
    };

    class _gsgsAPI Notebook : public wxPanel
    {
        friend class TabCtrl;
        friend class DefaultTabCtrl;
    public:
        enum TabType
        {
            TT_Mark,
            TT_Scroll
        };
    public:
        Notebook(wxWindow* parent, wxWindowID id = wxID_ANY, const wxSize & itemsize = wxDefaultSize, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long tabstyle = 0, long style = wxTAB_TRAVERSAL | wxSIMPLE_BORDER, 
            TabType tabtype = TT_Mark, const wxString& name = wxEmptyString);
        virtual ~Notebook();

        /**
         * @brief update the notebook art class and refresh
         */
        void SetArt(TabViewPtr_t art)       { mTab->SetArt(art); }

        /**
         * @brief set the notebook style. The style bits are kNotebook_* (you can set several
         * styles OR-ed)
         */
        void SetStyle(size_t style)         { PositionControls(); mTab->SetStyle(style, false); }

        /**
         * @brief set the tab direction
         */
        void SetTabDirection(wxDirection d);

        /**
         * @brief return the book style
         */
        size_t GetStyle() const         { return mTab->GetStyle(); }

        /**
         * @brief enable a specific style in the notebook
         */
        void EnableStyle(NotebookStyle style, bool enable);

        /**
         * @brief append page to the notebook
         */
        void AddPage(wxWindow * page, const wxString & label, bool selected = false, const wxBitmap & bmp = wxNullBitmap);

        /**
         * @brief insert page at a specified position
         */
        bool InsertPage(size_t index, wxWindow * page, const wxString & label, bool selected = false, const wxBitmap & bmp = wxNullBitmap);

        /**
         * @brief return the currently selected page or null
         */
        wxWindow* GetCurrentPage() const;

        /**
         * @brief Returns the index of the specified tab window or wxNOT_FOUND if not found
         */
        inline int FindPage(wxWindow* page) const                  { return mTab->FindPage(page);  }

        /**
         * @brief Deletes the specified page, without deleting the associated window
         */
        inline bool RemovePage(size_t page, bool notify = false)   { return mTab->RemovePage(page, notify, false); }

        /**
         * @brief Deletes the specified page and the associated window
         */
        inline bool DeletePage(size_t page, bool notify = true)     { return mTab->RemovePage(page, notify, true); }

        /**
         * @brief Deletes all pages
         */
        inline bool DeleteAllPages()                                { return mTab->DeleteAllPages(); }

        /**
         * @brief set a new selection. This function fires an event that can be vetoed
         */
        inline int SetSelection(size_t selection)                   { return mTab->SetSelection(selection); }
        /**
         * @brief set new selection. No events are fired
         */
        inline int ChangeSelection(size_t selection)                { return mTab->ChangeSelection(selection); }

        /**
         * @brief return the currently selected page, return wxNOT_FOUND if non found
         */
        inline int GetSelection() const                             { return mTab->GetSelection(); }

        /**
         * @brief Sets the text for the given page.
         */
        inline bool SetPageText(size_t page, const wxString& text)     { return mTab->SetPageText(page, text); }

        /**
         * @brief Returns the string for the given page
         */
        inline wxString GetPageText(size_t page) const                 { return mTab->GetPageText(page); }

        /**
         * @brief set the image for the given page
         */
        void SetPageBitmap(size_t index, const wxBitmap& bmp, bool refresh = true){ mTab->SetPageBitmap(index, bmp, refresh); }

        /**
         * @brief return bitmap for a given page. Return wxNullBitmap if invalid page
         */
        wxBitmap GetPageBitmap(size_t index) const                      { return mTab->GetPageBitmap(index); }

        // Base class members...
        virtual bool SetPageImage(size_t, int)                          { return false; }
        virtual int GetPageImage(size_t n) const                        { return wxNOT_FOUND; }

        /**
         * @brief return the index of a given window in the tab control
         * @param window
         * @return return window index, or wxNOT_FOUND
         */
        inline int GetPageIndex(wxWindow* window) const        { return mTab->DoGetPageIndex(window); }

        /**
         * @brief return the index of a given window by its title
         */
        inline int GetPageIndex(const wxString& label) const   { return mTab->DoGetPageIndex(label); }

        /**
         * @brief Returns the number of pages in the control
         */
        inline size_t GetPageCount() const                     { return mTab->GetTabs().size(); }

        /**
         * @brief Returns the window at the given page position.
         */
        inline wxWindow* GetPage(size_t index) const           { return mTab->GetPage(index); }

        /**
         * @brief return an array of all the windows managed by this notebook
         */
        inline void GetAllPages(WindowList& pages) { mTab->GetAllPages(pages); }

        /**
         * @brief return all tabs info
         * @param tabs [output]
         */
        inline size_t GetAllTabs(TabInfoVec_t& tabs)
        {
            tabs = mTab->GetTabs();
            return tabs.size();
        }

        /**
         * @brief set a context menu to be shown whe context menu is requested
         * on a tab label
         */
        void SetMenu(wxMenu* menu) { mTab->SetMenu(menu); }

        /**
         * @brief Sets the tool tip displayed when hovering over the tab label of the page
         * @return true if tool tip was updated, false if it failed, e.g. because the page index is invalid.
         */
        inline bool SetPageToolTip(size_t page, const wxString& tooltip) { return mTab->SetPageToolTip(page, tooltip); }

        /**
         * @brief return the tabbing history
         * @return
         */
        inline TabHistoryPtr_t GetHistory() const { return mTab->GetHistory(); }

        /**
         * @brief move the active page and place it in the new nexIndex
         */
        inline bool MoveActivePage(int newIndex)   { return mTab->MoveActiveToIndex(newIndex, GetSelection() > newIndex ? eDirection::kLeft : eDirection::kRight); }
    protected:
        inline bool IsVerticalStyle() const { return mTab->IsVerticalStyle(); }
        //void OnSize(wxSizeEvent& event);
        void PositionControls();
    private:
        WindowStack * mWinList;
        TabCtrl * mTab;
    };
}
#endif // NOTEBOOK_H
