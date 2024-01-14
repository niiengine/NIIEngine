#include "gsgsItemPanel.h"
#include "gsgsTreeCtrl.h"
#include "gsgsScrolledPanel.h"
#include <wx/dc.h>
#include <wx/headercol.h>
#include <wx/cursor.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/font.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/minifram.h>
#include <wx/textctrl.h>
#include <cmath>

#if defined(__WXGTK__) || defined(__WXOSX__)
#define USE_PANEL_PARENT 1
#else
#define USE_PANEL_PARENT 0
#endif

namespace gsgs
{
    wxDEFINE_EVENT(wxEVT_TREE_SEARCH_TEXT, wxTreeEvent);
    wxDEFINE_EVENT(wxEVT_TREE_CLEAR_SEARCH, wxTreeEvent);

    //===------------------------
    // Helper class
    //===------------------------
    class SearchControl :
    #if USE_PANEL_PARENT
        public wxPanel
    #else
        public wxMiniFrame
    #endif
    {
        wxTextCtrl* m_textCtrl = nullptr;

    private:
        void DoSelect(bool next)
        {
            TreeCtrl* tree = dynamic_cast<TreeCtrl*>(GetParent());
            if(!tree || m_textCtrl->IsEmpty()) { return; }
            wxTreeItemId where = next ? tree->FindNext(tree->GetSelection(), m_textCtrl->GetValue(), 0,
                                                       wxTR_SEARCH_DEFAULT & ~wxTR_SEARCH_INCLUDE_CURRENT_ITEM)
                                      : tree->FindPrev(tree->GetSelection(), m_textCtrl->GetValue(), 0,
                                                       wxTR_SEARCH_DEFAULT & ~wxTR_SEARCH_INCLUDE_CURRENT_ITEM);
            if(where.IsOk()) {
                RowEntry* row = reinterpret_cast<RowEntry*>(where.GetID());
                MatchResult res = row->GetHighlightInfo();

                // This will remove all the matched info, including the last call to FindNext/Prev
                tree->ClearAllHighlights();

                // Set back the match info
                row->SetHighlightInfo(res);

                // Select the item
                tree->SelectItem(where);

                // Make sure its visible
                tree->EnsureVisible(where);

                // Highlight the result
                tree->HighlightText(where, true);
            }
        }

    public:
        SearchControl(ItemPanel* parent)
    #if USE_PANEL_PARENT
            : wxPanel(parent)
    #else
            : wxMiniFrame(parent, wxID_ANY, "Find", wxDefaultPosition, wxDefaultSize,
                          wxFRAME_FLOAT_ON_PARENT | wxBORDER_SIMPLE)
    #endif
        {
            Hide();
            SetSizer(new wxBoxSizer(wxVERTICAL));
            wxPanel* mainPanel = new wxPanel(this);
            GetSizer()->Add(mainPanel, 1, wxEXPAND);
            mainPanel->SetSizer(new wxBoxSizer(wxVERTICAL));
            int scrollBarWidth = wxSystemSettings::GetMetric(wxSYS_VSCROLL_X, parent);
            wxSize searchControlSize(GetParent()->GetSize().GetWidth() / 2 - scrollBarWidth, -1);
            m_textCtrl = gsgs_Style().createTextCtrl(mainPanel, wxID_ANY, "", wxDefaultPosition, searchControlSize,
                                        wxTE_RICH | wxTE_PROCESS_ENTER);
            mainPanel->GetSizer()->Add(m_textCtrl, 0, wxEXPAND);
            m_textCtrl->CallAfter(&wxTextCtrl::SetFocus);

            m_textCtrl->Bind(wxEVT_TEXT, &SearchControl::OnTextUpdated, this);
            m_textCtrl->Bind(wxEVT_KEY_DOWN, &SearchControl::OnKeyDown, this);
        }

        virtual ~SearchControl()
        {
            m_textCtrl->Unbind(wxEVT_TEXT, &SearchControl::OnTextUpdated, this);
            m_textCtrl->Unbind(wxEVT_KEY_DOWN, &SearchControl::OnKeyDown, this);

            // Let the parent know that we were dismissed
            ItemPanel* parent = dynamic_cast<ItemPanel*>(GetParent());
            parent->SearchControlDismissed();
        }

        void PositionControl()
        {
    #if USE_PANEL_PARENT
            int scrollBarHeight = wxSystemSettings::GetMetric(wxSYS_HSCROLL_Y, GetParent());
            int x = GetParent()->GetSize().GetWidth() / 2;
            int y = GetParent()->GetSize().GetHeight() - GetSize().GetHeight() - scrollBarHeight;
            SetPosition(wxPoint(x, y));
    #else
            wxPoint parentPt = GetParent()->GetScreenPosition();
            CenterOnParent();
            SetPosition(wxPoint(GetPosition().x, parentPt.y - m_textCtrl->GetSize().GetHeight()));
    #endif
        }
        void DoSelectNone() { m_textCtrl->SelectNone(); }

        void InitSearch(const wxChar& ch)
        {
            m_textCtrl->SetFocus();
            m_textCtrl->ChangeValue(wxString() << ch);
            m_textCtrl->SetInsertionPointEnd();
            CallAfter(&SearchControl::DoSelectNone);
        }

        void ShowControl(const wxChar& ch)
        {
            Show();
            m_textCtrl->ChangeValue("");
            PositionControl();
            CallAfter(&SearchControl::InitSearch, ch);
        }

        void SelectNext() { DoSelect(true); }

        void SelectPrev() { DoSelect(false); }

        void Dismiss()
        {
            GetParent()->CallAfter(&wxWindow::SetFocus);
            // Clear the search
            wxTreeEvent e(wxEVT_TREE_CLEAR_SEARCH);
            e.SetEventObject(GetParent());
            GetParent()->GetEventHandler()->QueueEvent(e.Clone());
            Hide();
        }

        void OnTextUpdated(wxCommandEvent& event)
        {
            event.Skip();
            wxTreeEvent e(wxEVT_TREE_SEARCH_TEXT);
            e.SetString(m_textCtrl->GetValue());
            e.SetEventObject(GetParent());
            GetParent()->GetEventHandler()->QueueEvent(e.Clone());
        }

        void OnKeyDown(wxKeyEvent& event)
        {
            if(event.GetKeyCode() == WXK_ESCAPE) {
                Dismiss();
                return;
            } else if(event.GetKeyCode() == WXK_DOWN) {
                SelectNext();
            } else if(event.GetKeyCode() == WXK_UP) {
                SelectPrev();
            } else if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
                // Activate the item
                TreeCtrl* tree = dynamic_cast<TreeCtrl*>(GetParent());
                wxTreeEvent evt(wxEVT_TREE_ITEM_ACTIVATED);
                evt.SetEventObject(tree);
                evt.SetItem(tree->GetSelection());
                tree->GetEventHandler()->AddPendingEvent(evt);
                Dismiss();
            } else {
                event.Skip();
            }
        }
    };
    //---------------------------------------------------------------------------------
    #ifdef __WXMSW__
    #define PEN_STYLE wxPENSTYLE_SHORT_DASH
    #else
    #define PEN_STYLE wxPENSTYLE_DOT
    #endif

    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    // SearchText
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    SearchText::SearchText() {}
    //---------------------------------------------------------------------------------
    SearchText::~SearchText() {}
    //---------------------------------------------------------------------------------
    bool SearchText::Matches(const wxString& findWhat, size_t col, const wxString& text, size_t searchFlags,
        MatchResult* matches)
    {
        wxString haystack = searchFlags & wxTR_SEARCH_ICASE ? text.Lower() : text;
        wxString needle = searchFlags & wxTR_SEARCH_ICASE ? findWhat.Lower() : findWhat;
        if (!matches) {
            if (searchFlags & wxTR_SEARCH_METHOD_CONTAINS) {
                return haystack.Contains(needle);
            }
            else {
                return (haystack == needle);
            }
        }
        else {
            if (searchFlags & wxTR_SEARCH_METHOD_CONTAINS) {
                int where = haystack.Find(needle);
                if (where == wxNOT_FOUND) { return false; }
                Str3Arr_t arr;
                arr[0] = text.Mid(0, where);
                arr[1] = text.Mid(where, needle.length());
                arr[2] = text.Mid(where + needle.length());
                matches->Add(col, arr);
                return true;
            }
            else {
                if (haystack == needle) {
                    Str3Arr_t arr;
                    arr[0] = "";
                    arr[1] = text;
                    arr[2] = "";
                    matches->Add(col, arr);
                    return true;
                }
                else {
                    return false;
                }
            }
        }
        return false;
    }
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    // HeaderItem
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    HeaderItem::HeaderItem(wxWindow* parent, const wxString& label, const wxBitmap& bmp)
        : m_label(label)
        , m_bitmap(bmp)
        , m_parent(parent)
    {
    }
    //---------------------------------------------------------------------------------
    HeaderItem::HeaderItem() {}
    //---------------------------------------------------------------------------------
    HeaderItem::~HeaderItem() {}
    //---------------------------------------------------------------------------------
    void HeaderItem::Render(wxDC& dc, const Colour& colours, int flags)
    {
        dc.SetFont(ScrolledWindow::GetDefaultFont());
        wxSize textSize = dc.GetTextExtent(GetLabel());
        int textY = m_rect.GetY() + (m_rect.GetHeight() - textSize.GetHeight()) / 2;

        if (flags & kHeaderNative) {
            wxRendererNative::Get().DrawHeaderButton(m_parent, dc, m_rect, 0);

        }
        else {
            dc.SetBrush(colours.GetHeaderBgColour());
            dc.SetPen(colours.GetHeaderBgColour());
            dc.DrawRectangle(m_rect);
        }

        dc.SetTextForeground(flags & kHeaderNative ? wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT)
            : colours.GetItemTextColour());
        //dc.DrawText(GetLabel(), wxPoint(m_rect.GetX() + X_SPACER, textY));
        dc.DrawText(GetLabel(), wxPoint(m_rect.GetX() + X_HeaderSPACER, textY));
    }
    //---------------------------------------------------------------------------------
    void HeaderItem::SetWidthValue(int width)
    {
        switch (width) {
        case wxCOL_WIDTH_AUTOSIZE:
            EnableFlag(kHeaderColWidthMask, false);
            EnableFlag(kHeaderColWidthFitData, true);
            return;
        case wxCOL_WIDTH_DEFAULT:
            EnableFlag(kHeaderColWidthMask, false);
            EnableFlag(kHeaderColWidthFitHeader, true);
            return;
        default:
            if (width < 0) { return; }
            EnableFlag(kHeaderColWidthMask, false);
            EnableFlag(kHeaderColWidthUser, true);
            break;
        }
        UpdateWidth(width);
    }
    //---------------------------------------------------------------------------------
    void HeaderItem::UpdateWidth(int width)
    {
        if (width >= 0) { m_rect.SetWidth(width); }
    }
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    // HeaderBar
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    HeaderBar::HeaderBar(ItemPanel* parent, const Colour * colours)
        : m_colours(colours)
    {
        Hide();
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        if (wxPanel::Create(parent))
        {
            Bind(wxEVT_PAINT, &HeaderBar::OnPaint, this);
            Bind(wxEVT_LEFT_DOWN, &HeaderBar::OnMouseLeftDown, this);
            Bind(wxEVT_MOTION, &HeaderBar::OnMotion, this);
            Bind(wxEVT_LEFT_UP, &HeaderBar::OnMouseLeftUp, this);
            Bind(wxEVT_MOUSE_CAPTURE_LOST, &HeaderBar::OnMouseCaptureLost, this);
            Bind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) { });
        }
    }
    //---------------------------------------------------------------------------------
    HeaderBar::~HeaderBar() 
    {
        Unbind(wxEVT_PAINT, &HeaderBar::OnPaint, this);
        Unbind(wxEVT_LEFT_DOWN, &HeaderBar::OnMouseLeftDown, this);
        Unbind(wxEVT_MOTION, &HeaderBar::OnMotion, this);
        Unbind(wxEVT_LEFT_UP, &HeaderBar::OnMouseLeftUp, this);
        Unbind(wxEVT_MOUSE_CAPTURE_LOST, &HeaderBar::OnMouseCaptureLost, this);
        Unbind(wxEVT_ERASE_BACKGROUND, [](wxEraseEvent&) {  });
    }
    //---------------------------------------------------------------------------------
    size_t HeaderBar::GetHeight() const
    {
        if (m_columns.empty()) { return 0; }
        return m_columns[0].GetRect().GetHeight();
    }
    //---------------------------------------------------------------------------------
    void HeaderBar::push_back(const HeaderItem & item)
    {
        m_columns.push_back(item);
        DoUpdateSize();
    }
    //---------------------------------------------------------------------------------
    void HeaderBar::DoUpdateSize()
    {
        wxSize fixedText = GetTextSize("Tp");
        int xx = 0;
        for (size_t i = 0; i < m_columns.size(); ++i) {
            HeaderItem & item = m_columns[i];
            wxSize textSize = GetTextSize(item.GetLabel());
            wxRect headerRect(xx, 0, textSize.GetWidth() + (2 * HeaderItem::X_HeaderSPACER),
                fixedText.GetHeight() + (2 * HeaderItem::Y_SPACER));
            item.SetRect(headerRect);
            xx += item.GetRect().GetWidth();
        }
    }
    //---------------------------------------------------------------------------------
    wxSize HeaderBar::GetTextSize(const wxString& label) const
    {
        wxBitmap bmp(1, 1);
        wxMemoryDC memDC(bmp);
        wxGCDC gcdc(memDC);
        wxFont font = ScrolledWindow::GetDefaultFont();
        gcdc.SetFont(font);
        wxSize textSize = gcdc.GetTextExtent(label);
        return textSize;
    }
    //---------------------------------------------------------------------------------
    void HeaderBar::Render(wxDC& dc, const Colour* colours)
    {
        const wxRect rect = GetClientRect();
        dc.SetPen(colours->GetHeaderBgColour());
        dc.SetBrush(colours->GetHeaderBgColour());
        dc.DrawRectangle(rect);

        const Colour & _colours = *colours;
        //_colours.SetBgColour(_colours.GetHeaderBgColour());
        bool useNativeHeader = (m_flags & kHeaderNative);
        if (useNativeHeader) 
        { 
            wxRendererNative::Get().DrawHeaderButton(this, dc, rect, 0); 
        }

        // Set the DC origin to reflect the h-scrollbar
        ItemPanel* parent = dynamic_cast<ItemPanel*>(GetParent());
        dc.SetDeviceOrigin(-parent->GetFirstColumn(), 0);
        for (size_t i = 0; i < size(); ++i) 
        {
            bool is_last = (i == (size() - 1));
            Item(i).Render(dc, _colours, m_flags);
            if (!is_last && !useNativeHeader) {
                dc.SetPen(wxPen(_colours.GetHeaderVBorderColour(), 1, PEN_STYLE));
                dc.DrawLine(Item(i).GetRect().GetTopRight(), Item(i).GetRect().GetBottomRight());
            }
        }

        // Restore the DC origin
        dc.SetDeviceOrigin(0, 0);
        if (!useNativeHeader) {
            dc.SetPen(_colours.GetHeaderHBorderColour());
            dc.DrawLine(rect.GetBottomLeft(), rect.GetBottomRight());
        }
    }
    //---------------------------------------------------------------------------------
    void HeaderBar::UpdateColWidthIfNeeded(size_t col, int width, bool force)
    {
        if (col < m_columns.size())
        {
            HeaderItem& column = m_columns[col];
            column.UpdateWidth(force ? width : wxMax(column.GetWidth(), width));

            // Update the offsets
            int xx = 0;
            for (size_t i = 0; i < m_columns.size(); ++i) {
                HeaderItem & column = m_columns[i];
                column.SetX(xx);
                xx += column.GetWidth();
            }
        }
    }
    //---------------------------------------------------------------------------------
    void HeaderBar::OnMouseLeftDown(wxMouseEvent& event)
    {
        event.Skip();

        ItemPanel* parent = dynamic_cast<ItemPanel*>(GetParent());
        int x = event.GetX() + parent->GetFirstColumn();
        m_draggedCol = HitBorder(x);
        if (m_draggedCol > wxNOT_FOUND) {
            // Get ready to drag
            m_previousCursor = GetCursor();
            SetCursor(wxCursor(wxCURSOR_SIZEWE));
            m_isDragging = true;
            CaptureMouse();
        }
    }
    //---------------------------------------------------------------------------------
    void HeaderBar::OnMotion(wxMouseEvent& event)
    {
        event.Skip();
        if (m_isDragging) {
            ItemPanel* parent = dynamic_cast<ItemPanel*>(GetParent());
            int x = event.GetX() + parent->GetFirstColumn();
            wxCHECK_RET(m_draggedCol > -1 && m_draggedCol < (int)m_columns.size(), "Dragging but the column is invalid");
            int delta = x - Item(m_draggedCol).GetRect().GetRight();
            // Compare with COL_MIN_SIZE as we don't want to shrink the col to nothing (or beyond!)
            const static int COL_MIN_SIZE = 7;
            if ((int)(Item(m_draggedCol).GetWidth()) + delta > COL_MIN_SIZE) {
                parent->SetColumnWidth(m_draggedCol, Item(m_draggedCol).GetWidth() + delta);
            }
            if (IsShown()) { Update(); }
        }
    }
    //---------------------------------------------------------------------------------
    void HeaderBar::OnMouseLeftUp(wxMouseEvent& event)
    {
        event.Skip();
        DoCancelDrag();
    }
    //---------------------------------------------------------------------------------
    void HeaderBar::OnMouseCaptureLost(wxMouseCaptureLostEvent & e)
    {
        DoCancelDrag();
    }
    //---------------------------------------------------------------------------------
    int HeaderBar::HitBorder(int x) const // Returns the column whose *right*-hand edge contains the cursor
    {
        int xpos(0);
        for (size_t i = 0; i < size(); ++i) {
            xpos += Item(i).GetWidth();
            if (abs(x - xpos) < 5) { return i; }
        }

        return wxNOT_FOUND;
    }
    //---------------------------------------------------------------------------------
    size_t HeaderBar::GetWidth() const
    {
        size_t w = 0;
        for (size_t i = 0; i < m_columns.size(); ++i) {
            w += m_columns[i].GetWidth();
        }
        return w;
    }
    //---------------------------------------------------------------------------------
    const HeaderItem& HeaderBar::Last() const
    {
        static HeaderItem emptyItem;
        if (m_columns.empty()) {
            return emptyItem;
        }
        return m_columns.back();
    }
    //---------------------------------------------------------------------------------
    HeaderItem& HeaderBar::Last()
    {
        if (m_columns.empty()) {
            static HeaderItem emptyItem;
            return emptyItem;
        }
        return m_columns.back();
    }
    //---------------------------------------------------------------------------------
    void HeaderBar::Clear() { m_columns.clear(); }
    //---------------------------------------------------------------------------------
    void HeaderBar::OnPaint(wxPaintEvent&)
    {
        wxAutoBufferedPaintDC dc(this);
        //PrepareDC(dc);
        wxGCDC gcdc(dc);
        Render(gcdc, m_colours);
    }
    //---------------------------------------------------------------------------------
    void HeaderBar::DoCancelDrag()
    {
        m_isDragging = false;
        m_draggedCol = -1;
        SetCursor(m_previousCursor);
        m_previousCursor = wxCursor();
        if (HasCapture()) { ReleaseMouse(); }
    }
    //---------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------
    // ItemPanel
    //---------------------------------------------------------------------------------
    ItemPanel::ItemPanel() :
        m_viewHeader(0),
        m_bitmaps(0)
    {
        m_colours.setBase(gsgs_Style().GetAppBgColour());
        //m_viewHeader = new HeaderBar(this, &m_colours);
    }
    //---------------------------------------------------------------------------------
    ItemPanel::ItemPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                           long style):
        ScrolledWindow(parent, id, pos, size, style)
    {
        m_colours.setBase(gsgs_Style().GetAppBgColour());
        DoInitialize();
    }
    //---------------------------------------------------------------------------------
    ItemPanel::~ItemPanel()
    {
        m_searchControl = nullptr;
        Unbind(wxEVT_MOUSEWHEEL, &ItemPanel::OnMouseScroll, this);
        wxDELETE(m_bitmapsInternal);
    }
    //---------------------------------------------------------------------------------
    bool ItemPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    {
        if (ScrolledWindow::Create(parent, id, pos, size, style))
        {
            DoInitialize();
            return true;
        }
        return false;
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::DoInitialize()
    {
        //SetBackgroundStyle(wxBG_STYLE_PAINT);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());
        m_bitmaps = 0;
        m_viewHeader = new HeaderBar(this, &m_colours);
        Bind(wxEVT_MOUSEWHEEL, &ItemPanel::OnMouseScroll, this);
        Bind(wxEVT_SET_FOCUS, [&](wxFocusEvent& e) {
            e.Skip();
            if(m_searchControl && m_searchControl->IsShown()) 
            { 
                m_searchControl->Dismiss(); 
            }
        });
        wxSize textSize = GetTextSize("Tp");
        SetLineHeight(RowEntry::Y_SPACER + textSize.GetHeight() + RowEntry::Y_SPACER);
        SetIndent(0);
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::SetShowHeader(bool b)
    {
        wxWindowUpdateLocker llt(this);
        m_viewHeader->Show(b);
        DoPositionVScrollbar(); // Adjust the vertical scrollbar if needed
        Refresh();
    }
    //---------------------------------------------------------------------------------
    wxRect ItemPanel::GetItemsRect() const
    {
        // Return the rectangle taking header into consideration
        int yOffset = 0;
        if(m_viewHeader->IsShown()) 
        { 
            yOffset = m_viewHeader->GetHeight(); 
        }
        wxRect clientRect = GetClientArea();
        clientRect.SetY(yOffset);
        clientRect.SetHeight(clientRect.GetHeight() - yOffset);
        return clientRect;
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::RenderItems(wxDC & dc, const RowEntry::Vec_t & items)
    {
        AssignRects(items);
        for(size_t i = 0; i < items.size(); ++i) 
        {
            RowEntry* it = items[i];
            if(it->IsHidden())
            { 
                continue; 
            }
            else
            {
                it->Render(this, dc, m_colours, i, &GetSearch());
            }
        }
    }
    //---------------------------------------------------------------------------------
    int ItemPanel::GetNumLineCanFitOnScreen() const
    {
        wxRect clientRect = GetItemsRect();
        int max_lines_on_screen = std::ceilf(clientRect.GetHeight() / m_lineHeight);
        return max_lines_on_screen;
    }
    //---------------------------------------------------------------------------------
    RowEntry * ItemPanel::GetFirstItemOnScreen() 
    { 
        return m_firstItemOnScreen; 
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::SetFirstItemOnScreen(RowEntry* item) 
    { 
        m_firstItemOnScreen = item; 
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::UpdateScrollBar()
    {

        //{
            // V-scrollbar
            int vthumbSize = GetNumLineCanFitOnScreen(); // Number of lines can be drawn
            int vpageSize = (vthumbSize);
            int vrangeSize = GetRange();
            int vposition = GetFirstItemPosition();
        //    UpdateVScrollBar(vposition, vthumbSize, vrangeSize, vpageSize, false, false);
        //}
        //{
            // H-scrollbar
            int hthumbSize = GetClientArea().GetWidth();
            int hrangeSize = IsEmpty() ? 0 : m_viewHeader->GetWidth();
            if((m_firstColumn + hthumbSize) > hrangeSize) { m_firstColumn = (hrangeSize - hthumbSize); }
            int hpageSize = (hthumbSize - 1);
            int hposition = m_firstColumn;

            int pixels_after = hrangeSize - m_firstColumn - hthumbSize;
            if((pixels_after < 0) && (hrangeSize > hthumbSize)) {
                // m_firstColumn += pixels_after; // reduce it from the left side
                HeaderItem& column = GetHeader()->Item(GetHeader()->size() - 1);
                column.UpdateWidth(column.GetWidth() - pixels_after);
            }
            if(m_firstColumn < 0) { m_firstColumn = 0; }
            hposition = m_firstColumn;

            wxSize csize = GetClientSize();
            wxSize tsize = wxSize(hthumbSize, vthumbSize);
            wxSize bsize = wxSize(hrangeSize, vrangeSize);
            if (tsize.x < bsize.x)
            {
                csize.y = csize.y - SB_WIDTH;

                if (tsize.y < bsize.y)
                    csize.x = csize.x - SB_WIDTH;
            }
            else if (tsize.y < bsize.y)
            {
                csize.x = csize.x - SB_WIDTH;

                if (tsize.x < bsize.x)
                    csize.y = csize.y - SB_WIDTH;
            }
            if (tsize.y < bsize.y)
            {
                if (IsHeaderVisible()) {
                    csize.y -= (GetHeader() ? GetHeader()->GetHeight() : 0);
                    if (csize.y < 0) { csize.y = 0; }
                    wxWindowUpdateLocker templ(m_vsb);
                    m_vsb->SetSize(csize.x, GetHeader() ? GetHeader()->GetHeight() : 0, SB_WIDTH, csize.y);
                }
                else {
                    wxWindowUpdateLocker templ(m_vsb);
                    m_vsb->SetSize(csize.x, 0, SB_WIDTH, csize.y);
                }
            }

            if (tsize.x < bsize.x)
            {
                wxWindowUpdateLocker templ(m_hsb);
                m_hsb->SetSize(0, csize.y, csize.x, SB_WIDTH);
            }

            UpdateVScrollBar(vposition, vthumbSize, vrangeSize, vpageSize, false, false);
            UpdateHScrollBar(hposition, hthumbSize, hrangeSize, hpageSize, false, false);
        //}
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::Render(wxDC& dc)
    {
        // draw the background on the entire client Rect
        dc.SetPen(GetColours().GetBgColour());
        dc.SetBrush(GetColours().GetBgColour());
        dc.DrawRectangle(GetClientRect());
        // draw the background on the entire client contentArea
        //dc.SetPen(GetColours().GetBgColour());
        //dc.SetBrush(GetColours().GetBgColour());
        //dc.DrawRectangle(GetClientArea());

        // Set the device origin to the X-offset
        dc.SetDeviceOrigin(-m_firstColumn, 0);
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::OnSize(wxSizeEvent& event)
    {
        //event.Skip();
        m_firstColumn = 0;
        // since the control size was resized, we turn the "m_maxList" flag to ON
        // and in turn, in the OnPaint() we will try to maximize the list displayed
        // to fit
        m_maxList = true;
        //DoPositionHScrollbar();
        //DoPositionVScrollbar();
        UpdateScrollBar();
        {
            WindowUpdateLocker ttl(m_viewHeader);
            m_viewHeader->SetSize(0, 0, event.GetSize().GetWidth(), m_viewHeader->GetHeight());
            if(m_viewHeader->IsShown())m_viewHeader->Update();
        }
        Refresh();
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::ScollToColumn(int firstColumn)
    {
        m_firstColumn = firstColumn;
        Refresh();
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::ScrollColumns(int steps, wxDirection direction)
    {
        if((steps == 0) && (direction == wxLEFT)) {
            m_firstColumn = 0;
        } else if((steps == 0) && (direction == wxRIGHT)) {
            m_firstColumn = GetHeader()->GetWidth();
        } else {
            int max_width = GetHeader()->GetWidth();
            int firstColumn = m_firstColumn + ((direction == wxRIGHT) ? steps : -steps);
            if(firstColumn < 0) { firstColumn = 0; }
            int pageSize = GetClientArea().GetWidth();
            if((firstColumn + pageSize) > max_width) { firstColumn = max_width - pageSize; }
            m_firstColumn = firstColumn;
        }
        Refresh();
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::DoUpdateHeader(RowEntry* row)
    {
        // do we have header?
        if(GetHeader()->empty()) { return; }
        if(row && row->IsHidden()) { return; }
        wxDC& dc = GetTempDC();

        // Null row means: set the header bar to fit the column's label
        bool forceUpdate = (row == nullptr);
        int row_width = 0;
        // Use bold font, to get the maximum width needed
        for(size_t i = 0; i < GetHeader()->size(); ++i) 
        {
            if(row) 
            {
                row_width = row->CalcItemWidth(dc, m_lineHeight, i);
            }
            else 
            {
                int colWidth = dc.GetTextExtent(GetHeader()->Item(i).GetLabel()).GetWidth();
                colWidth += 3 * RowEntry::X_SPACER;
                row_width = colWidth;
            }
            if(forceUpdate || GetHeader()->Item(i).IsAutoResize()) {
                GetHeader()->UpdateColWidthIfNeeded(i, row_width, forceUpdate);
            }
        }
    }
    //---------------------------------------------------------------------------------
    wxSize ItemPanel::GetTextSize(const wxString& label) const
    {
        wxDC& dc = GetTempDC();
        wxFont font = GetDefaultFont();
        dc.SetFont(font);
        wxSize textSize = dc.GetTextExtent(label);
        return textSize;
    }
    //---------------------------------------------------------------------------------
    const wxBitmap& ItemPanel::GetBitmap(size_t index) const
    {
        if (GetBitmaps() && (index < GetBitmaps()->size())) {
            return GetBitmaps()->at(index);
        }
        return gsgs_Image().GetBitmap(index);
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::OnMouseScroll(wxMouseEvent& event)
    {
        event.Skip();
        DoMouseScroll(event);
    }
    //---------------------------------------------------------------------------------
    bool ItemPanel::DoKeyDown(const wxKeyEvent& event)
    {
        if(m_searchControl && m_searchControl->IsShown()) { return true; }
        if(m_search.IsEnabled() && wxIsprint(event.GetUnicodeKey()) &&
           (event.GetModifiers() == wxMOD_NONE || event.GetModifiers() == wxMOD_SHIFT)) {
            if(!m_searchControl) 
            { 
                m_searchControl = new SearchControl(this); 
            }
            m_searchControl->ShowControl(event.GetUnicodeKey());
            return true;
        }
        return false;
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::SearchControlDismissed() {}
    //---------------------------------------------------------------------------------
    void ItemPanel::AssignRects(const RowEntry::Vec_t& items)
    {
        wxRect clientRect = GetItemsRect();
        int y = clientRect.GetY();
        for(size_t i = 0; i < items.size(); ++i) {
            RowEntry* it = items[i];
            if(it->IsHidden()) {
                // Set the item's rects into something non visible
                it->SetRects(wxRect(-100, -100, 0, 0), wxRect());
                continue;
            }
            wxRect itemRect = wxRect(0, y, clientRect.GetWidth(), m_lineHeight);
            wxRect buttonRect;
            if(it->HasChildren()) {
                buttonRect = wxRect(it->GetIndentsCount() * GetIndent(), y, m_lineHeight, m_lineHeight);
            }
            else
            {
                buttonRect = wxRect(it->GetIndentsCount() * GetIndent(), y, 0, 0);
            }
            it->SetRects(itemRect, buttonRect);
            y += m_lineHeight;
        }
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::DoMouseScroll(const wxMouseEvent& event)
    {
        int range = GetRange();
        bool going_up = (event.GetWheelRotation() > 0);
        int new_row = GetFirstItemPosition() + (going_up ? -GetScrollTick() : GetScrollTick());
        if(new_row < 0) { new_row = 0; }
        if(new_row >= range) { new_row = range - 1; }
        ScrollToRow(new_row);
        Refresh();
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::DoPositionVScrollbar()
    {
        if(IsHeaderVisible()) {
            // When the heaer is visible place the vertical scrollbar under it
            wxRect clientRect = GetClientRect();
            wxSize vsbSize = GetVScrollBar()->GetSize();

            int height = clientRect.GetHeight();
            if(mHsbNeedShow) { height -= GetHScrollBar()->GetSize().GetHeight(); }
            int width = vsbSize.GetWidth();
            int x = clientRect.GetWidth() - vsbSize.GetWidth();
            int y = (GetHeader() ? GetHeader()->GetHeight() : 0);
            height -= (GetHeader() ? GetHeader()->GetHeight() : 0);
            if(height < 0) { height = 0; }
            wxWindowUpdateLocker tttl(this);
            GetVScrollBar()->SetSize(x, y, width, height);
        } else {
            ScrolledWindow::DoPositionVScrollbar();
        }
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::SetColumnWidth(size_t col, int width)
    {
        if (col < GetHeader()->size())
        {
            // Handle special values
            if (width == wxCOL_WIDTH_AUTOSIZE || width == wxCOL_WIDTH_DEFAULT) {
                GetHeader()->Item(col).SetWidthValue(width);
                GetHeader()->UpdateColWidthIfNeeded(col, width, true);
            }
            else if (width >= 0) {
                GetHeader()->Item(col).SetWidthValue(width);
                GetHeader()->UpdateColWidthIfNeeded(col, width, true);
            }
            UpdateScrollBar();
            Refresh();
        }
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::SetNativeTheme(bool nativeTheme)
    {
        GetHeader()->SetNative(nativeTheme);
        m_nativeTheme = nativeTheme;
        Refresh();
    }
    //---------------------------------------------------------------------------------
    void ItemPanel::SetImageList(wxImageList* images)
    {
        wxDELETE(m_bitmapsInternal);
        if(!images || images->GetImageCount() <= 0) { return; }

        m_bitmapsInternal = new BitmapList();
        m_bitmapsInternal->reserve(images->GetImageCount());
        for(size_t i = 0; i < (size_t)images->GetImageCount(); ++i) {
            m_bitmapsInternal->push_back(images->GetBitmap(i));
        }
        SetBitmaps(m_bitmapsInternal);
    }
    //---------------------------------------------------------------------------------
}