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
#ifndef gsgsItemPanel_H
#define gsgsItemPanel_H

#include "gsgsPreInclude.h"
#include "gsgsStyleManager.h"
#include "gsgsRowEntry.h"
#include "gsgsScrolledPanel.h"
#include <wx/bitmap.h>
#include <wx/gdicmn.h>
#include <wx/imaglist.h>

#ifdef __WXOSX__
#define SCROLL_TICK 2
#else
#define SCROLL_TICK 2
#endif

namespace gsgs
{
    // Search flags
    #define wxTR_SEARCH_METHOD_EXACT (1 << 0)    // Use an exact string comparison method
    #define wxTR_SEARCH_METHOD_CONTAINS (1 << 1) // Compare using wxString::Contains method
    #define wxTR_SEARCH_VISIBLE_ITEMS (1 << 2)   // Search reachable items (i.e. they can be visible if scrolled enough)
    #define wxTR_SEARCH_ICASE (1 << 3)           // Search incase-sensitive
    #define wxTR_SEARCH_INCLUDE_CURRENT_ITEM \
        (1 << 4) // When calling the search API, FindNext/FindPrev include the 'starting' item
    #define wxTR_SEARCH_DEFAULT \
        (wxTR_SEARCH_METHOD_CONTAINS | wxTR_SEARCH_VISIBLE_ITEMS | wxTR_SEARCH_ICASE | wxTR_SEARCH_INCLUDE_CURRENT_ITEM)

    class SearchControl;
    class _gsgsAPI SearchText
    {
    public:
        SearchText();
        virtual ~SearchText();
        inline void SetEnabled(bool enabled)    { this->m_enabled = enabled; }
        inline bool IsEnabled() const           { return m_enabled; }
        static bool Matches(const wxString& findWhat, size_t col, const wxString& text,
            size_t searchFlags = wxTR_SEARCH_DEFAULT, MatchResult* matches = nullptr);
    private:
        bool m_enabled = false;
    };

    enum HeaderFlags {
        kHeaderNative = (1 << 0),
        kHeaderColWidthFitData = (1 << 1),
        kHeaderColWidthFitHeader = (1 << 2),
        kHeaderColWidthUser = (1 << 3),
        kHeaderColWidthMask = (kHeaderColWidthUser | kHeaderColWidthFitHeader | kHeaderColWidthFitData),
    };

    class _gsgsAPI HeaderItem
    {
    public:
        HeaderItem();
        HeaderItem(wxWindow* parent, const wxString& label, const wxBitmap& bmp = wxNullBitmap);
        virtual ~HeaderItem();

        void Render(wxDC& dc, const Colour& colours, int flags);
        void SetBitmap(const wxBitmap& bitmap)  { this->m_bitmap = bitmap; }
        void SetLabel(const wxString& label)    { this->m_label = label; }
        void SetRect(const wxRect& rect)        { this->m_rect = rect; }
        const wxRect& GetRect() const           { return m_rect; }
        const wxBitmap& GetBitmap() const       { return m_bitmap; }
        const wxString& GetLabel() const        { return m_label; }
        void SetWidthValue(int width);
        void UpdateWidth(int width);
        size_t GetWidth() const                 { return m_rect.GetWidth(); }
        void SetX(size_t x)                     { m_rect.SetX(x); }
        bool IsAutoResize() const               { return m_flags & kHeaderColWidthFitData; }
    protected:
        void EnableFlag(int flag, bool b)       { b ? m_flags |= flag : m_flags &= ~flag; }
    public:
        typedef std::vector<HeaderItem> Vect_t;
        static const int X_SPACER = 10;
        static const int X_HeaderSPACER = 5;
        static const int Y_SPACER = 2;
        static const int Y_HeaderSPACER = 2;
    private:
        wxString m_label;
        wxBitmap m_bitmap;
        wxRect m_rect;
        wxWindow* m_parent = nullptr;
        size_t m_flags = kHeaderColWidthFitData;
    };

    class _gsgsAPI HeaderBar : public wxPanel
    {
    public:
        HeaderBar(ItemPanel* parent, const Colour * colours);
        virtual ~HeaderBar();

        /**
         * @brief set drawing native header
         */
        void SetNative(bool b)
        {
            b ? m_flags |= kHeaderNative : m_flags &= ~kHeaderNative;
            Refresh();
        }

        /**
         * @brief update the column width, but only if the new width is greater than the current one, unless 'force' is set
         * to 'true'
         * @param col
         * @param width
         * @param force set the new width, no matter what...
         */
        void UpdateColWidthIfNeeded(size_t col, int width, bool force = false);
        HeaderItem& Item(int index)             { return m_columns[index]; }
        const HeaderItem& Item(int index) const { return m_columns[index]; }
        HeaderItem& operator[](int index)       { return Item(index); }
        const HeaderItem& operator[](int index) const { return Item(index); }

        size_t size() const                     { return m_columns.size(); }
        size_t GetCount() const                 { return size(); }

        /**
         * @brief Return the header bar height, taking into consideration all columns
         */
        size_t GetHeight() const;

        /**
         * @brief do we have any columns?
         */
        bool empty() const                      { return m_columns.empty(); }
        /**
         * @brief do we have any columns?
         */
        bool IsEmpty() const                    { return empty(); }

        /**
         * @brief add new column to the header bar
         */
        void push_back(const HeaderItem& item);
        /**
         * @brief add new column to the header bar
         */
        HeaderItem& Add(const HeaderItem& item)
        {
            push_back(item);
            return Last();
        }
        /**
         * @brief add new column to the header bar
         */
        HeaderItem& Add(const wxString& label, const wxBitmap& bitmap = wxNullBitmap)
        {
            push_back(HeaderItem(m_parent, label, bitmap));
            return Last();
        }

        void Clear();
        const HeaderItem& Last() const;
        HeaderItem& Last();
        /**
         * @brief draw the header bar using dc and colours
         */
        void Render(wxDC& dc, const Colour* colours);
        size_t GetWidth() const;

        /**
         * @brief are we dragging a column?
         */
        bool IsDragging() { return m_isDragging; };

        /**
         * @brief process a left-down event
         */
        void OnMouseLeftDown(wxMouseEvent& event);
        /**
         * @brief process motion events
         */
        void OnMotion(wxMouseEvent& event);
        /**
         * @brief process a left-up event
         */
        void OnMouseLeftUp(wxMouseEvent& event);

        void OnMouseCaptureLost(wxMouseCaptureLostEvent & e);
    protected:
        void DoUpdateSize();
        wxSize GetTextSize(const wxString& label) const;
        int HitBorder(int x) const;
        void OnPaint(wxPaintEvent& event);
        void DoCancelDrag();
    private:
        HeaderItem::Vect_t m_columns;
        const Colour * m_colours;
        int m_flags = 0;
        bool m_isDragging = false;
        int m_draggedCol = wxNOT_FOUND;
        wxCursor m_previousCursor;
    };

    class _gsgsAPI ItemPanel : public ScrolledWindow
    {
    public:
        ItemPanel();
        ItemPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                           const wxSize& size = wxDefaultSize, long style = 0);
        virtual ~ItemPanel();

        void SetNativeTheme(bool nativeTheme);
        inline bool IsNativeTheme() const               { return m_nativeTheme; }
        bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0);
        virtual int GetIndent() const                   { return m_indent; }

        virtual void SetFirstColumn(int firstColumn)    { this->m_firstColumn = firstColumn; }
        virtual int GetFirstColumn() const              { return m_firstColumn; }

        virtual void SetLineHeight(int lineHeight)      { this->m_lineHeight = lineHeight; }
        virtual int GetLineHeight() const               { return m_lineHeight; }

        virtual void SetBitmaps(BitmapList * bitmaps)   { this->m_bitmaps = bitmaps; }
        virtual void SetImageList(wxImageList* images);
        virtual const BitmapList* GetBitmaps() const    { return m_bitmaps; }
        virtual BitmapList* GetBitmaps()                { return m_bitmaps; }

        void SetScrollTick(int scrollTick)              { this->m_scrollTick = scrollTick; }
        int GetScrollTick() const                       { return m_scrollTick; }

        /**
         * @brief return bitmap at a given index
         */
        const wxBitmap& GetBitmap(size_t index) const;

        /**
         * @brief set the item's indent size
         */
        virtual void SetIndent(int size)                { m_indent = size; }

        /**
         * @brief return the items rect area, excluding header
         */
        wxRect GetItemsRect() const;

        /**
         * @brief draw the header + items
         */
        void Render(wxDC& dc);

        /**
         * @brief Get a pointer to the header, create one if needed
         */
        HeaderBar* GetHeader() const                    { return m_viewHeader;  }

        /**
         * @brief sets a column width. If the width is less than 0, this function does nothing. If the column is out of
         * bound this function does nothing. Two sepcial values are allowed here: wxCOL_WIDTH_DEFAULT and
         * wxCOL_WIDTH_AUTOSIZE
         * @param col the column index
         * @param width the width. Can contain one of the special values: wxCOL_WIDTH_DEFAULT and wxCOL_WIDTH_AUTOSIZE
         */
        void SetColumnWidth(size_t col, int width);

        /**
         * @brief should we show the header bar?
         */
        void SetShowHeader(bool b);
        /**
         * @brief is the heaer bar visible?
         */
        bool IsHeaderVisible() const                    { return GetHeader()->IsShown(); }

        /**
         * @brief update the scrollbar with the current view status
         * subclass should call this method whenver the view changes (re-sized, items are expanding, collapsing etc)
         */
        virtual void UpdateScrollBar();

        void SetColours(const Colour& colours)
        {
            m_colours = colours;
            GetVScrollBar()->SetColours(m_colours);
            GetHScrollBar()->SetColours(m_colours);
            Refresh();
        }

        inline const Colour& GetColours() const         { return m_colours; }
        inline Colour& GetColours()                     { return m_colours; }

        /// @copydetails ScrolledWindow::ScollToColumn
        void ScollToColumn(int firstColumn);
        
        /// @copydetails ScrolledWindow::ScrollColumns
        void ScrollColumns(int steps, wxDirection direction);

        /**
         * @brief override this
         */
        virtual bool IsEmpty() const = 0;
        /**
         * @brief return the total of numbers of items we can scroll
         * If the view has collpased items, the range _excludes_ them
         */
        virtual int GetRange() const = 0;

        /**
         * @brief return the row number of the first visible item in the view
         */
        virtual int GetFirstItemPosition() const = 0;

        void SearchControlDismissed();
    protected:
        void DoInitialize();
        int GetNumLineCanFitOnScreen() const;
        virtual RowEntry* GetFirstItemOnScreen();
        virtual void SetFirstItemOnScreen(RowEntry* item);
        void RenderItems(wxDC& dc, const RowEntry::Vec_t& items);
        void AssignRects(const RowEntry::Vec_t& items);
        void OnSize(wxSizeEvent& event);
        void DoUpdateHeader(RowEntry* row);
        wxSize GetTextSize(const wxString& label) const;
        virtual void OnMouseScroll(wxMouseEvent& event);
        virtual bool DoKeyDown(const wxKeyEvent& event);
        virtual void DoMouseScroll(const wxMouseEvent& event);
        SearchText& GetSearch()                         { return m_search; }
        const SearchText& GetSearch() const             { return m_search; }

        void DoPositionVScrollbar();
    protected:
        HeaderBar* m_viewHeader = nullptr;
        RowEntry* m_firstItemOnScreen = nullptr;
        Colour m_colours;
        int m_firstColumn = 0;
        int m_lineHeight = 0;
        int m_indent = 0;
        int m_scrollTick = SCROLL_TICK;
        BitmapList* m_bitmaps = nullptr;
        BitmapList* m_bitmapsInternal = nullptr;
        SearchText m_search;
        SearchControl* m_searchControl = nullptr;
        bool m_maxList = false;
        bool m_nativeTheme = false;
    };

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TREE_SEARCH_TEXT, wxTreeEvent);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TREE_CLEAR_SEARCH, wxTreeEvent);
}
#endif
