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
#ifndef gsgs_TREECTRLNODE_H
#define gsgs_TREECTRLNODE_H

#include "gsgsPreInclude.h"
#include "gsgsStyleManager.h"
#include <array>
#include <wx/colour.h>
#include <wx/gdicmn.h>
#include <wx/treebase.h>
#include <wx/font.h>

namespace gsgs
{
    class SearchText;

    enum TreeCtrlNodeFlags {
        kNF_FontBold = (1 << 0),
        kNF_FontItalic = (1 << 1),
        kNF_SortItems = (1 << 2),
        kNF_Expanded = (1 << 3),
        kNF_Selected = (1 << 4),
        kNF_Hovered = (1 << 5),
        kNF_Hidden = (1 << 6),
        kNF_LisItem = (1 << 7),
        kNF_HighlightText = (1 << 8),
    };

    typedef std::array<wxString, 3> Str3Arr_t;
    struct _gsgsAPI MatchResult {
        std::unordered_map<size_t, Str3Arr_t> matches;

        bool Get(size_t col, Str3Arr_t& arr) const
        {
            if(matches.count(col) == 0) { return false; }
            arr = matches.find(col)->second;
            return true;
        }
        void Add(size_t col, const Str3Arr_t& arr)
        {
            matches.erase(col);
            matches[col] = arr;
        }

        void Clear() { matches.clear(); }
    };

    class _gsgsAPI CellValue
    {
    public:
        enum eType {
            kTypeNull = 0,
            kTypeString = 1,
            kTypeBool = 2,
            kTypeChoice = 3,
        };
    public:
        CellValue();
        CellValue(const wxString& text, int bmpIndex = wxNOT_FOUND, int bmpOpenIndex = wxNOT_FOUND);
        CellValue(const char* ptext, int bmpIndex = wxNOT_FOUND, int bmpOpenIndex = wxNOT_FOUND);
        CellValue(bool bValue, const wxString& label, int bmpIndex = wxNOT_FOUND, int bmpOpenIndex = wxNOT_FOUND);
        virtual ~CellValue();
        inline bool IsOk() const                                { return m_type != kTypeNull; }
        inline bool IsString() const                            { return m_type == kTypeString; }
        inline bool IsBool() const                              { return m_type == kTypeBool; }
        inline void SetType(eType type)                         { m_type = type; }
        inline void SetValue(const wxString& text)              { this->m_stringValue = text; }
        inline void SetValue(bool b)                            { this->m_boolValue = b; }
        inline const wxString& GetValueString() const           { return m_stringValue; }
        inline bool GetValueBool() const                        { return m_boolValue; }
        inline void SetTooltip(const wxString & text)           { mTooltip = text; }
        inline const wxString & GetTooltip() const              { return mTooltip; }
        inline bool IsChoice() const                            { return m_type == kTypeChoice; }

        inline int GetBitmapIndex() const                       { return m_bitmapIndex; }
        inline int GetBitmapSelectedIndex() const               { return m_bitmapSelectedIndex; }
        inline void SetBgColour(const wxColour& bgColour)       { this->m_bgColour = bgColour; }
        inline void SetBitmapIndex(int bitmapIndex)             { this->m_bitmapIndex = bitmapIndex; }
        inline void SetBitmapSelectedIndex(int bitmapSelectedIndex) { this->m_bitmapSelectedIndex = bitmapSelectedIndex; }
        inline void SetFont(const wxFont& font)                 { this->m_font = font; }
        inline void SetTextColour(const wxColour& textColour)   { this->m_textColour = textColour; }
        inline const wxColour& GetBgColour() const              { return m_bgColour; }
        inline const wxFont& GetFont() const                    { return m_font; }
        inline const wxColour& GetTextColour() const            { return m_textColour; }
        inline void SetCheckboxRect(const wxRect& rect)         { this->m_checkboxRect = rect; }
        inline const wxRect& GetCheckboxRect() const            { return m_checkboxRect; }
        inline void SetDropDownRect(const wxRect& rect)         { this->m_dropDownRect = rect; }
        inline const wxRect& GetDropDownRect() const            { return m_dropDownRect; }
        typedef std::vector<CellValue> Vect_t;
        const static CellValue gNull;
    protected:
        wxString m_stringValue;
        wxString mTooltip;
        eType m_type = kTypeNull;
        int m_bitmapIndex = wxNOT_FOUND;
        int m_bitmapSelectedIndex = wxNOT_FOUND;
        wxFont m_font;
        wxColour m_textColour;
        wxColour m_bgColour;
        wxRect m_checkboxRect;
        wxRect m_dropDownRect;
        bool m_boolValue = false;
    };

    class _gsgsAPI RowEntry
    {
    public:
        typedef std::vector<RowEntry*> Vec_t;
        static int Y_SPACER;
        static int X_SPACER;
    public:
        RowEntry* GetLastChild() const;
        RowEntry* GetFirstChild() const;

        RowEntry(TreeCtrl* tree, const wxString& label, int bitmapIndex = wxNOT_FOUND,
                   int bitmapSelectedIndex = wxNOT_FOUND);
        RowEntry(TreeCtrl* tree, bool checked, const wxString& label, int bitmapIndex = wxNOT_FOUND,
                   int bitmapSelectedIndex = wxNOT_FOUND);
        ~RowEntry();
        /**
         * @brief return the item rectangle
         * @return
         */
        wxRect GetCellRect(size_t col = 0) const;
        inline RowEntry* GetNext() const                        { return m_next; }
        inline RowEntry* GetPrev() const                        { return m_prev; }
        inline void SetNext(RowEntry* next)                     { this->m_next = next; }
        inline void SetPrev(RowEntry* prev)                     { this->m_prev = prev; }
        inline void SetHighlightInfo(const MatchResult& info) { m_higlightInfo = info; }
        inline const MatchResult& GetHighlightInfo() const    { return m_higlightInfo; }
        void SetHidden(bool b);
        inline bool IsHidden() const                            { return HasFlag(kNF_Hidden); }
        inline void SetHighlight(bool b)                        { SetFlag(kNF_HighlightText, b); }
        inline bool IsHighlight() const                         { return HasFlag(kNF_HighlightText); }
        inline void SetData(wxIntPtr data)                     { this->m_data = data; }
        inline wxIntPtr GetData()                              { return m_data; }

        /**
         * @brief using wxDC, calculate the item's width
         */
        int CalcItemWidth(wxDC& dc, int rowHeight, size_t col = 0);
        inline void SetListItem(bool b)                         { SetFlag(kNF_LisItem, b); }
        inline bool IsListItem() const                          { return m_flags & kNF_LisItem; }
        bool IsVisible() const;
        void SetBgColour(const wxColour& bgColour, size_t col = 0);
        void SetFont(const wxFont& font, size_t col = 0);
        void SetTextColour(const wxColour& textColour, size_t col = 0);
        const wxColour& GetBgColour(size_t col = 0) const;
        const wxFont& GetFont(size_t col = 0) const;
        const wxColour& GetTextColour(size_t col = 0) const;
        /**
         * @brief remove and delete a single child
         * @param child
         */
        void DeleteChild(RowEntry* child);
        /**
         * @brief remove all children items
         */
        void DeleteAllChildren();
        void Render(wxWindow* win, wxDC& dc, const Colour& colours, int row_index, SearchText* searcher);
        inline void SetHovered(bool b)                      { SetFlag(kNF_Hovered, b); }
        inline bool IsHovered() const                       { return m_flags & kNF_Hovered; }

        void ClearRects();
        inline void SetRects(const wxRect& rect, const wxRect& buttonRect) { m_rowRect = rect; m_buttonRect = buttonRect; }
        inline const wxRect& GetItemRect() const            { return m_rowRect; }
        inline const wxRect& GetButtonRect() const          { return m_buttonRect; }
        const wxRect& GetCheckboxRect(size_t col = 0) const;
        const wxRect& GetChoiceRect(size_t col = 0) const;

        void AddChild(RowEntry* child);

        /**
         * @brief insert item at 'where'. The new item is placed after 'prev'
         */
        void InsertChild(RowEntry* child, RowEntry* prev);

        /**
         * @brief insert this node between first and second
         */
        void ConnectNodes(RowEntry* first, RowEntry* second);

        inline bool IsBold() const              { return HasFlag(kNF_FontBold); }
        inline void SetBold(bool b)             { SetFlag(kNF_FontBold, b); }

        inline bool IsItalic() const            { return HasFlag(kNF_FontItalic); }
        inline void SetItalic(bool b)           { SetFlag(kNF_FontItalic, b); }

        inline bool IsExpanded() const          { return HasFlag(kNF_Expanded) || HasFlag(kNF_Hidden); }
        bool SetExpanded(bool b);
        inline bool IsRoot() const              { return GetParent() == nullptr; }
        
        // Cell accessors
        void SetBitmapIndex(int bitmapIndex, size_t col = 0);
        void SetBitmapSelectedIndex(int bitmapIndex, size_t col = 0);
        void SetLabel(const wxString& label, size_t col = 0);
        void SetTooltip(const wxString& label, size_t col = 0);
        /**
         * @brief make this specific cell as "choice" (dropdown will drawn to the right)
         */
        void SetChoice(bool b, size_t col = 0);
        bool IsChoice(size_t col) const;
        
        // Set this cell as "checkable" cell with possible label
        void SetChecked(bool checked, int bitmapIndex, const wxString& label, size_t col = 0);
        void SetChecked(bool checked, size_t col = 0);
        bool IsChecked(size_t col = 0) const;

        int GetBitmapIndex(size_t col = 0) const;
        int GetBitmapSelectedIndex(size_t col = 0) const;
        const wxString& GetLabel(size_t col = 0) const;
        const wxString& GetTooltip(size_t col = 0) const;

        inline const std::vector<RowEntry*>& GetChildren() const    { return m_children; }
        inline std::vector<RowEntry*>& GetChildren()                { return m_children; }
        inline wxTreeItemData* GetClientObject() const              { return m_clientObject; }
        void SetParent(RowEntry* parent);
        inline RowEntry* GetParent() const                          { return m_parent; }
        inline bool HasChildren() const                             { return m_children.size(); }
        inline void SetClientObject(wxTreeItemData* data)
        {
            wxDELETE(m_clientObject);
            this->m_clientObject = data;
        }
        size_t GetChildrenCount(bool recurse) const;
        size_t GetExpandedChildrenCount() const;
        int GetExpandedLines() const;
        void GetNextItems(int count, RowEntry::Vec_t& items, bool selfIncluded = true);
        void GetPrevItems(int count, RowEntry::Vec_t& items, bool selfIncluded = true);
        inline void SetIndentsCount(int count)              { this->m_indentsCount = count; }
        inline int GetIndentsCount() const                  { return m_indentsCount; }

        inline bool IsSelected() const                      { return HasFlag(kNF_Selected); }
        inline void SetSelected(bool b)                     { SetFlag(kNF_Selected, b); }
        inline void UnselectAll();
        inline bool IsSorted() const                        { return HasFlag(kNF_SortItems); }
    protected:
        void SetFlag(TreeCtrlNodeFlags flag, bool b)
        {
            if(b) {
                m_flags |= flag;
            } else {
                m_flags &= ~flag;
            }
        }

        inline bool HasFlag(TreeCtrlNodeFlags flag) const { return m_flags & flag; }

        /**
         * @brief return the nth visible item
         */
        RowEntry* GetVisibleItem(int index);
        CellValue& GetColumn(size_t col = 0);
        const CellValue& GetColumn(size_t col = 0) const;
        void DrawSimpleSelection(wxWindow* win, wxDC& dc, const wxRect& rect, const Colour& colours);
        void RenderText(wxWindow* win, wxDC& dc, const Colour& colours, const wxString& text, int x, int y, size_t col);
        void RenderTextSimple(wxWindow* win, wxDC& dc, const Colour& colours, const wxString& text, int x, int y, size_t col);
        void RenderCheckBox(wxWindow* win, wxDC& dc, const Colour& colours, const wxRect& rect, bool checked);
        int GetCheckBoxWidth(wxWindow* win);
        
    protected:
        TreeCtrl* m_tree = nullptr;
        TreeCtrlModel* m_model = nullptr;
        CellValue::Vect_t m_cells;
        size_t m_flags = 0;
        wxTreeItemData* m_clientObject = nullptr;
        wxIntPtr m_data = -1;
        RowEntry* m_parent = nullptr;
        RowEntry::Vec_t m_children;
        RowEntry* m_next = nullptr;
        RowEntry* m_prev = nullptr;
        int m_indentsCount = 0;
        wxRect m_rowRect;
        wxRect m_buttonRect;
        MatchResult m_higlightInfo;
    };
}
#endif
