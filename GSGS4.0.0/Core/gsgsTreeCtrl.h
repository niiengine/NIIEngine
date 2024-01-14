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
#ifndef gsgsTREECTRL_H
#define gsgsTREECTRL_H

#include "gsgsPreInclude.h"
#include "gsgsItemPanel.h"
#include "gsgsScrolledPanel.h"
#include "gsgsCommonHandler.h"
#include "gsgsPluginCommon.h"
#include "gsgsRowEntry.h"
#include <wx/datetime.h>
#include <wx/dc.h>
#include <wx/treectrl.h>
#include <wx/bitmap.h>
#include <wx/scrolwin.h>
#include <wx/headercol.h>
#include <wx/colour.h>
#include <wx/treebase.h>
#include <wx/dataview.h>
#include <functional>

#define wxTR_ENABLE_SEARCH 0x4000
// Sorting is applied for top level items (i.e. items whom their direct parent is the root item)
#define wxTR_SORT_TOP_LEVEL 0x0100

// Extra styles supported by this class
#define wxDV_ENABLE_SEARCH wxTR_ENABLE_SEARCH

// Search flags
// See wxTR_SEARCH* for more info
#define wxDV_SEARCH_METHOD_EXACT wxTR_SEARCH_METHOD_EXACT
#define wxDV_SEARCH_METHOD_CONTAINS wxTR_SEARCH_METHOD_CONTAINS
#define wxDV_SEARCH_VISIBLE_ITEMS wxTR_SEARCH_VISIBLE_ITEMS
#define wxDV_SEARCH_ICASE wxTR_SEARCH_ICASE
#define wxDV_SEARCH_INCLUDE_CURRENT_ITEM wxTR_SEARCH_INCLUDE_CURRENT_ITEM
#define wxDV_SEARCH_DEFAULT wxTR_SEARCH_DEFAULT

#define DV_ITEM(tree_item) wxDataViewItem(tree_item.GetID())
#define TREE_ITEM(dv_item) wxTreeItemId(dv_item.GetID())

wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TREE_ITEM_VALUE_CHANGED, wxTreeEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TREE_CHOICE, wxTreeEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DATAVIEW_SEARCH_TEXT, wxDataViewEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DATAVIEW_CLEAR_SEARCH, wxDataViewEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DATAVIEW_CHOICE_BUTTON, wxDataViewEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_DATAVIEW_CHOICE, wxDataViewEvent);

namespace gsgs
{
    static const int wxTREE_HITTEST_ONDROPDOWNARROW  = 0x2000;
    typedef std::function<bool(RowEntry*, RowEntry*)> TreeSortFunc;
    
    class _gsgsAPI TreeCtrlModel
    {
    public:
        TreeCtrlModel(TreeCtrl* tree);
        ~TreeCtrlModel();

        inline void SetFirstItemOnScreen(RowEntry* first)               { m_firstItemOnScreen = first; }
        inline RowEntry* GetFirstItemOnScreen() const                   { return m_firstItemOnScreen; }

        inline void SetSortFunction(const TreeSortFunc& CompareFunc)    { mSortFunc = CompareFunc; }
        void ExpandAllChildren(const wxTreeItemId& item)                { DoExpandAllChildren(item, true); }
        void CollapseAllChildren(const wxTreeItemId& item)              { DoExpandAllChildren(item, false); }

        // Notifications from the node
        void NodeDeleted(RowEntry* node);
        void NodeExpanded(RowEntry* node, bool expanded);
        bool NodeExpanding(RowEntry* node, bool expanding);

        void GetNextItems(RowEntry* from, int count, RowEntry::Vec_t& items, bool selfIncluded = true) const;
        void GetPrevItems(RowEntry* from, int count, RowEntry::Vec_t& items, bool selfIncluded = true) const;
        wxTreeItemId AddRoot(const wxString& text, int image, int selImage, wxTreeItemData* data);
        wxTreeItemId AppendItem(const wxTreeItemId& parent, const wxString& text, int image, int selImage, wxTreeItemData* data);
        wxTreeItemId InsertItem(const wxTreeItemId& parent, const wxTreeItemId& previous, const wxString& text, int image,
                                int selImage, wxTreeItemData* data);
        wxTreeItemId GetRootItem() const;

        inline void SetIndentSize(int indentSize)                       { m_indentSize = indentSize; }
        inline int GetIndentSize() const                                { return m_indentSize; }

        /**
         * @brief clear the selection from all the items
         */
        void UnselectAll();

        wxTreeItemId GetItemBefore(const wxTreeItemId& item, bool visibleItem) const;
        wxTreeItemId GetItemAfter(const wxTreeItemId& item, bool visibleItem) const;
        RowEntry* GetRowBefore(RowEntry* item, bool visibleItem) const;
        RowEntry* GetRowAfter(RowEntry* item, bool visibleItem) const;
        inline RowEntry* ToPtr(const wxTreeItemId& item) const
        {
            if(!m_root || !item.IsOk()) { return nullptr; }
            return reinterpret_cast<RowEntry*>(item.GetID());
        }

        /**
         * @brief select a given item
         */
        void SelectItem(const wxTreeItemId& item, bool select = true, bool addSelection = false, bool clear_old_selection = false);
        
        /**
         * @brief add an item to the selection list
         * this function fires an event
         */
        void AddSelection(const wxTreeItemId& item);

        /**
         * @brief clear all selections, return true on sucess, this function fires the changing event
         */
        bool ClearSelections(bool notify);

        inline bool IsItemSelected(const wxTreeItemId& item) const      { return IsItemSelected(ToPtr(item)); }
        bool IsItemSelected(const RowEntry* item) const;

        bool IsVisible(const wxTreeItemId& item) const;

        /**
         * @brief select the children of 'item' this functin fires the changing and changed events
         */
        void SelectChildren(const wxTreeItemId& item);

        void Clear();
        
        void ClearScreenItems();

        void SetOnScreenItems(const RowEntry::Vec_t& items);

        inline const RowEntry::Vec_t& GetOnScreenItems() const          { return m_onScreenItems; }
        inline RowEntry::Vec_t& GetOnScreenItems()                      { return m_onScreenItems; }
        inline const RowEntry::Vec_t& GetSelections() const             { return m_selectedItems; }
        bool ExpandToItem(const wxTreeItemId& item);
        wxTreeItemId GetSingleSelection() const;
        inline size_t GetSelectionsCount() const                        { return m_selectedItems.size(); }

        /**
         * @brief do we have items in this tree? (root included)
         */
        inline bool IsEmpty() const                                     { return m_root == nullptr; }

        inline RowEntry* GetRoot() const                                { return m_root; }

        /**
         * @brief delete subtree starting from 'item', including item
         * fires event wxEVT_TREE_DELETE_ITEM
         * @param item
         */
        void DeleteItem(const wxTreeItemId& item);
        int GetItemIndex(RowEntry* item) const;
        RowEntry* GetItemFromIndex(int index) const;

        /**
         * @brief get range of items from -> to
         * Or from: to->from (incase 'to' has a lower index)
         */
        bool GetRange(RowEntry* from, RowEntry* to, RowEntry::Vec_t& items) const;

        size_t GetExpandedLines() const;

        RowEntry* GetNextSibling(RowEntry* item) const;
        RowEntry* GetPrevSibling(RowEntry* item) const;
        
        void EnableEvents(bool enable) { m_shutdown = !enable; }
    protected:
        TreeCtrlModel();
        void DoExpandAllChildren(const wxTreeItemId& item, bool expand);
        bool IsSingleSelection() const;
        bool IsMultiSelection() const;
        bool SendEvent(wxEvent& event);
    private:
        TreeCtrl* m_tree = nullptr;
        RowEntry* m_root = nullptr;
        RowEntry::Vec_t m_selectedItems;
        RowEntry::Vec_t m_onScreenItems;
        RowEntry* m_firstItemOnScreen = nullptr;
        TreeSortFunc mSortFunc = nullptr;
        int m_indentSize = 16;
        bool m_shutdown = false;
    };

    class _gsgsAPI TreeCtrl : public ItemPanel
    {
    public:
        TreeCtrl();
        TreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize, long style = 0);
        virtual ~TreeCtrl();
        bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0);

        virtual int GetFirstItemPosition() const;

        virtual int GetRange() const;

        //===--------------------
        // Search support
        //===--------------------
        /**
         * @brief search for an item from pos that matches the "what" string.
         * Pass an invalid item to start the search from the root
         */
        wxTreeItemId FindNext(const wxTreeItemId& from, const wxString& what, size_t col = 0,
                              size_t searchFlags = wxTR_SEARCH_DEFAULT);
        /**
         * @brief search for an item from pos that matches the "what" string.
         * Pass an invalid item to start the search from the root
         */
        wxTreeItemId FindPrev(const wxTreeItemId& from, const wxString& what, size_t col = 0,
                              size_t searchFlags = wxTR_SEARCH_DEFAULT);

        /**
         * @brief highlight matched string of an item. This call should be called after a successfull call to
         * FindNext or FindPrev
         */
        void HighlightText(const wxTreeItemId& item, bool b);

        /**
         * @brief clear highlighted text
         */
        void ClearHighlight(const wxTreeItemId& item);

        /**
         * @brief clear all highlighted text from all the items
         */
        void ClearAllHighlights();
        
        //===--------------------
        // table view support
        //===--------------------
        
        void AddHeader(const wxString& label, const wxBitmap& bmp = wxNullBitmap, int width = wxCOL_WIDTH_AUTOSIZE)
        {
            DoAddHeader(label, bmp, width);
        }
        
        // For internal use, dont use these two methods
        const TreeCtrlModel& GetModel() const                           { return m_model; }
        TreeCtrlModel& GetModel()                                       { return m_model; }
        
        void SetSortEnable(bool b)                                      { mSortEnable = b;}
        
        bool IsSortEnable() const                                       { return mSortEnable;}

        /**
         * @brief set a sorting function for this tree. The function returns true if the first element should be placed
         * before the second element
         */
        virtual void SetSortFunction(const TreeSortFunc& CompareFunc);

        /**
         * @brief associate bitmap vector with this tree. The bitmaps array must exists as long as this control exists
         */
        virtual void SetBitmaps(BitmapList* bitmaps);
        
        /**
         * @brief set image list. The control does not take ownership on the input image list
         * Instead, it creates a copy of the images. It is up to the user to free any resources allocated
         */
        virtual void SetImageList(wxImageList* images);

        /**
         * @brief return the tree style
         */
        long GetTreeStyle() const { return m_treeStyle; }

        /**
         * @brief is the root hidden?
         */
        void SetRootHide(bool set) { EnableStyle(wxTR_HIDE_ROOT, set, false); }

        /**
         * @brief is the root hidden?
         */
        bool IsRootHide() const { return m_treeStyle & wxTR_HIDE_ROOT; }

        /**
         * @brief enable style on the tree
         */
        virtual void EnableStyle(int style, bool enable, bool refresh = true);

        /**
         * @brief does the tree has 'style' enabled?
         */
        bool HasStyle(int style) const { return m_treeStyle & style; }
        
        /**
         * @brief add style to the current tree style
         */
        void AddTreeStyle(int style) { m_treeStyle |= style; }
        
        /**
         * @brief set style to the tree
         */
        void SetTreeStyle(int style) { m_treeStyle = style; }
        
        /**
         * @brief Calculates which (if any) item is under the given point, returning the tree item id at this point plus
         *  extra information flags.
         *  flags is a bitlist of the following (the bits are output returned by this function):
         *  wxTREE_HITTEST_NOWHERE: In the client area but below the last item.
         *  wxTREE_HITTEST_ONITEMBUTTON: On the button associated with an item.
         *  wxTREE_HITTEST_ONITEM
         */
        wxTreeItemId HitTest(const wxPoint& point, int& flags, int& column) const;

        /**
         * @brief ppends an item to the end of the branch identified by parent, return a new item id.
         */
        wxTreeItemId AppendItem(const wxString& text, int image = -1, int selImage = -1, wxTreeItemData* data = NULL);

        /**
         * @brief ppends an item to the end of the branch identified by parent, return a new item id.
         */
        wxTreeItemId AppendItem(const wxTreeItemId& parent, const wxString& text, int image = -1, int selImage = -1,
                                wxTreeItemData* data = NULL);
        /**
         * @brief Adds the root node to the tree, returning the new item.
         */
        wxTreeItemId AddRoot(const wxString& text, int image = -1, int selImage = -1, wxTreeItemData* data = NULL);

        /**
         * @brief insert item after 'previous'
         */
        wxTreeItemId InsertItem(const wxTreeItemId& parent, const wxTreeItemId& previous, const wxString& text,
                                int image = -1, int selImage = -1, wxTreeItemData* data = NULL);
        /**
         * @brief return the root item
         */
        wxTreeItemId GetRootItem() const        { return m_model.GetRootItem(); }

        /**
         * @brief return the item's parent
         */
        wxTreeItemId GetItemParent(const wxTreeItemId& item) const;

        /**
         * @brief Expands the given item
         */
        void Expand(const wxTreeItemId& item);
        
        /**
         * @brief check a given item (if it has checkbox)
         * This method fires 'wxEVT_TREE_ITEM_VALUE_CHANGED' event
         */
        void Check(const wxTreeItemId& item, bool check, size_t col = 0);
        
        /**
         * @brief is the state of an item is checked?
         */
        bool IsChecked(const wxTreeItemId& item, size_t col = 0) const;
        
        /**
         * @brief Collapses the given item
         */
        void Collapse(const wxTreeItemId& item);

        /**
         * @brief Selects the given item
         */
        void SelectItem(const wxTreeItemId& item, bool select = true);

        // Selection
        wxTreeItemId GetSelection() const;
        const wxString & GetSelectionText(size_t col = 0) const;
        wxTreeItemId GetFocusedItem() const;
        size_t GetSelections(wxArrayTreeItemIds& selections) const;

        /**
         * @brief unselect all items
         */
        void UnselectAll();

        void DeleteSelection();

        /**
         * @brief Scrolls and/or expands items to ensure that the given item is visible.
         */
        void EnsureVisible(const wxTreeItemId& item);

        /**
         * @brief is item expanded?
         */
        bool IsExpanded(const wxTreeItemId& item) const;

        /**
         * @brief Returns true if the item has children.
         */
        bool ItemHasChildren(const wxTreeItemId& item) const;
        bool HasChildren(const wxTreeItemId& item) const                { return ItemHasChildren(item); }

        /**
         * @brief set the item's indent size
         */
        void SetIndent(int size);

        /**
         * @brief return the current indent size
         */
        virtual int GetIndent() const;

        /**
         * @brief is the tree has items? (root included)
         */
        bool IsEmpty() const;

        /**
         * @brief return the children count of this item
         */
        size_t GetChildrenCount(const wxTreeItemId& item, bool recursively = true) const;

        size_t GetItemCount(bool recurse = false) const;

        /**
         * @brief Delete all the item's children. No events are fired
         */
        void DeleteChildren(const wxTreeItemId& item);
        /**
         * @brief Returns the first child; call GetNextChild() for the next child.
         */
        wxTreeItemId GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;
        wxTreeItemId GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const;

        /**
         * @brief get the next item in tree (as if you were clicking DOWN in the keyboard)
         */
        wxTreeItemId GetNextItem(const wxTreeItemId& item) const;

        /**
         * @brief get the prev item in tree (as if you were clicking DOWN in the keyboard)
         */
        wxTreeItemId GetPrevItem(const wxTreeItemId& item) const;

        /**
         * @brief for compatibility, we dont really need to call this method manually
         */
        void SortChildren(const wxTreeItemId& item) { wxUnusedVar(item); }

        /**
         * @brief set item's image index
         */
        void SetItemImage(const wxTreeItemId& item, int imageId, int openImageId = wxNOT_FOUND, size_t col = 0);

        /**
         * @brief return the associated image id with this item
         */
        int GetItemImage(const wxTreeItemId& item, bool selectedImage = false, size_t col = 0) const;
        /**
         * @brief Returns the first visible item
         */
        wxTreeItemId GetFirstVisibleItem() const;

        /**
         * @brief Returns the next visible item or an invalid item if this item is the last visible one
         */
        wxTreeItemId GetNextVisible(const wxTreeItemId& item) const;

        /**
         * @brief Returns the prev visible item or an invalid item if this item is the first visible one
         */
        wxTreeItemId GetPrevVisible(const wxTreeItemId& item) const;

        //===--------------------------------
        // Item properties
        //===--------------------------------
        wxTreeItemData* GetItemData(const wxTreeItemId& item) const;
        void SetItemData(const wxTreeItemId& item, wxTreeItemData* data);

        wxIntPtr GetItemIntData(const wxTreeItemId& item) const;
        void SetItemIntData(const wxTreeItemId& item, wxIntPtr data);

        void SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& colour, size_t col = 0);
        wxColour GetItemBackgroundColour(const wxTreeItemId& item, size_t col = 0) const;

        void SetItemTextColour(const wxTreeItemId& item, const wxColour& colour, size_t col = 0);
        wxColour GetItemTextColour(const wxTreeItemId& item, size_t col = 0) const;

        void SetItemText(const wxTreeItemId& item, const wxString& text, size_t col = 0);
        const wxString & GetItemText(const wxTreeItemId& item, size_t col = 0) const;

        void SetItemTooltip(const wxTreeItemId& item, const wxString& text, size_t col = 0);
        const wxString & GetItemTooltip(const wxTreeItemId& item, size_t col = 0) const;

        void SetItemBold(const wxTreeItemId& item, bool bold, size_t col = 0);

        void SetItemFont(const wxTreeItemId& item, const wxFont& font, size_t col = 0);
        wxFont GetItemFont(const wxTreeItemId& item, size_t col = 0) const;

        /**
         * @brief expand this item and all its children
         */
        void ExpandAllChildren(const wxTreeItemId& item);

        /**
         * @brief expand the entire tree
         */
        void ExpandAll() { ExpandAllChildren(GetRootItem()); }

        /**
         * @brief expand this item and all its children
         */
        void CollapseAllChildren(const wxTreeItemId& item);

        /**
         * @brief expand the entire tree
         */
        void CollapseAll() { CollapseAllChildren(GetRootItem()); }

        size_t GetExpandCount() const;

        /**
         * @brief Deletes the specified item.
         * A EVT_TREE_DELETE_ITEM event will be generated.
         */
        void Delete(const wxTreeItemId& item);

        /**
         * @brief Select all the immediate children of the given parent
         */
        void SelectChildren(const wxTreeItemId& item);

        /**
         * @brief Returns the next sibling of the specified item; call GetPrevSibling() for the previous sibling
         */
        wxTreeItemId GetNextSibling(const wxTreeItemId& item) const;

        /**
         * @brief Returns the previous sibling of the specified item; call GetNextSibling() for the next sibling
         */
        wxTreeItemId GetPrevSibling(const wxTreeItemId& item) const;

        /**
         * @brief delete all items in tree
         */
        virtual void DeleteAllItems();

        /**
         * @brief is this item visible?
         */
        bool IsVisible(const wxTreeItemId& item) const;
        /**
         * @brief is item selected?
         */
        bool IsSelected(const wxTreeItemId& item) const;
    protected:
        void UpdateScrollBar();
        void DoAddHeader(const wxString& label, const wxBitmap& bmp, int width = wxCOL_WIDTH_AUTOSIZE);
    protected:
        virtual bool DoKeyDown(const wxKeyEvent& event);
        void OnPaint(wxPaintEvent& event);
        void OnMouseLeftDown(wxMouseEvent& event);
        void OnMouseLeftUp(wxMouseEvent& event);
        void OnRightDown(wxMouseEvent& event);
        void OnMouseLeftDClick(wxMouseEvent& event);
        void DoMouseScroll(const wxMouseEvent& event);
        void ProcessIdle();
        void OnLeaveWindow(wxMouseEvent& event);
        void OnEnterWindow(wxMouseEvent& event);
        void OnContextMenu(wxContextMenuEvent& event);

        void ScrollRows(int steps, wxDirection direction);
        void ScrollToRow(int firstLine);

        wxTreeItemId GetRow(const wxPoint& pt) const;
    private:
        wxPoint DoFixPoint(const wxPoint& pt);
        wxTreeItemId DoGetSiblingVisibleItem(const wxTreeItemId& item, bool next) const;
        bool IsItemVisible(RowEntry* item) const;
        bool IsItemFullyVisible(RowEntry* item) const;
        void EnsureItemVisible(RowEntry* item, bool fromTop);
        RowEntry* GetFirstItemOnScreen();
        void SetFirstItemOnScreen(RowEntry* item);
        wxTreeItemId DoScrollLines(int numLines, bool up, wxTreeItemId from, bool selectIt);

        /**
         * @brief bitmap file was added, re-calculate the line heights
         */
        void DoBitmapAdded();

        /**
         * @brief update the header size
         */
        void DoUpdateHeader(const wxTreeItemId& item);

        void DoInitialize();
        RowEntry* DoFind(RowEntry* from, const wxString& what, size_t col, size_t searchFlags, bool next);
    protected:
        TreeCtrlModel m_model;
        bool mSortEnable = false;
        bool m_needToClearDefaultHeader = true;
        bool m_needToUpdateScrollbar = false;
        long m_treeStyle = 0;
        int m_scrollLines = 0;
    };

    class _gsgsAPI DefaultTreeCtrl : public TreeCtrl
    {
    public:
        DefaultTreeCtrl();

        DefaultTreeCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize, long style = 0);

        virtual ~DefaultTreeCtrl();
        bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize, long style = 0);
    protected:
        void OnTheme(wxCommandEvent& event);
        void ApplyTheme();
    private:
        TreeKeyboardInput::Ptr_t m_keyboard;
    };

    class _gsgsAPI TreeCtrlColourHelper
    {
    public:
        typedef SharedPtr<TreeCtrlColourHelper> Ptr_t;
    public:
        TreeCtrlColourHelper(DefaultTreeCtrl* tree);
        virtual ~TreeCtrlColourHelper();

        void SetBgColour(const wxTreeItemId& item, const wxColour& currentBgColour, FolderColour::Map_t& coloursMap) const;

        void ResetBgColour(const wxTreeItemId& item, FolderColour::Map_t& coloursMap) const;

        /**
         * @brief return a text path representing a tree item
         */
        static wxString GetItemPath(DefaultTreeCtrl* tree, const wxTreeItemId& item);
    protected:
        wxString GetItemPath(const wxTreeItemId& item) const;

        void DoSetBgColour(const wxTreeItemId& item, const wxColour& currentBgColour, const FolderColour::Map_t& coloursMap) const;

        void DoClearBgColour(const wxTreeItemId& item, const wxColour& colourToSet, FolderColour::Map_t& coloursMap) const;
    private:
        DefaultTreeCtrl* m_tree;
    };

    /**
     * @brief a thin wrapper around TreeCtrl which provides basic compatiblity API (such as adding columns)
     * This is mainly for code generators like wxCrafter
     */
    class _gsgsAPI DataViewListCtrl : public TreeCtrl
    {
    public:
        DataViewListCtrl();
        DataViewListCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0);
        virtual ~DataViewListCtrl();

        bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0);

        /**
         * @brief how many items can scroll in the view?
         */
        //virtual int GetRange() const;
        bool IsEmpty() const { return GetItemCount() == 0; }

        ///===--------------------
        /// Search support
        ///===--------------------
        /**
         * @brief search for an item from pos that matches the "what" string.
         * Pass an invalid item to start the search from the root
         */
        wxDataViewItem FindNext(const wxDataViewItem& from, const wxString& what, size_t col = 0,
            size_t searchFlags = wxDV_SEARCH_DEFAULT);
        /**
         * @brief search for an item from pos that matches the "what" string.
         * Pass an invalid item to start the search from the root
         */
        wxDataViewItem FindPrev(const wxDataViewItem& from, const wxString& what, size_t col = 0,
            size_t searchFlags = wxDV_SEARCH_DEFAULT);

        /**
         * @brief highlight matched string of an item. This call should be called after a successfull call to
         * FindNext or FindPrev
         */
        void HighlightText(const wxDataViewItem& item, bool b);

        /**
         * @brief clear highlighted text
         */
        void ClearHighlight(const wxDataViewItem& item);

        ///===--------------------
        /// wxDV compatilibty API
        ///===--------------------

        /**
         * @brief Scrolls to ensure that the given item is visible.
         */
        void EnsureVisible(const wxDataViewItem& item);

        /**
         * @brief appends an item to the end of the list
         */
        wxDataViewItem AppendItem(const wxString& text, int image = -1, int selImage = -1, wxUIntPtr data = 0);

        /**
         * @brief insert item after 'previous'
         */
        wxDataViewItem AppendItem(const wxDataViewItem& parent, const wxString& text, int image = -1, int selImage = -1,
            wxUIntPtr data = 0);

        /**
         * @brief appends an item to the end of the list
         */
        wxDataViewItem AppendItem(const wxString& text, bool set, int image, int selImage, wxUIntPtr data = 0);

        /**
         * @brief insert item after 'previous'
         */
        wxDataViewItem AppendItem(const wxDataViewItem& parent, const wxString& text, bool set, int image, int selImage,
            wxUIntPtr data = 0);

        /**
         * @brief insert item after 'previous'
         */
        wxDataViewItem InsertItem(const wxDataViewItem& previous, const wxString& text, int image = -1, int selImage = -1,
            wxUIntPtr data = 0);

        wxDataViewItem AppendItem(const wxVector<wxVariant>& values, wxUIntPtr data = 0);

        wxDataViewItem AppendItem(const wxDataViewItem & parent, const wxVector<wxVariant>& values, wxUIntPtr data = 0);

        wxDataViewColumn* AppendIconTextColumn(const wxString& label, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
            int width = -1, wxAlignment align = wxALIGN_LEFT,
            int flags = wxDATAVIEW_COL_RESIZABLE);

        wxDataViewColumn* AppendProgressColumn(const wxString& label, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
            int width = -1, wxAlignment align = wxALIGN_LEFT,
            int flags = wxDATAVIEW_COL_RESIZABLE);

        wxDataViewColumn* AppendTextColumn(const wxString& label, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT,
            int width = -1, wxAlignment align = wxALIGN_LEFT,
            int flags = wxDATAVIEW_COL_RESIZABLE);

        wxDataViewItem GetSelection() const { return wxDataViewItem(TreeCtrl::GetSelection().GetID()); }
        wxDataViewItem GetCurrentItem() const { return wxDataViewItem(GetFocusedItem().GetID()); }
        void Select(const wxDataViewItem& item);
        int GetSelections(wxDataViewItemArray& sel) const;
        int GetSelectedItemsCount() const;
        /**
         * @brief delete all items in the view. If a "deleterFunc" is provided, it will be called per item data
         */
        void DeleteAllItems(const std::function<void(wxUIntPtr)>& deleterFunc = nullptr);

        wxUIntPtr GetItemData(const wxDataViewItem& item) const;
        void SetItemData(const wxDataViewItem& item, wxUIntPtr data);

        void SetItemBackgroundColour(const wxDataViewItem& item, const wxColour& colour, size_t col = 0);
        wxColour GetItemBackgroundColour(const wxDataViewItem& item, size_t col = 0) const;

        void SetItemTextColour(const wxDataViewItem& item, const wxColour& colour, size_t col = 0);
        wxColour GetItemTextColour(const wxDataViewItem& item, size_t col = 0) const;

        void SetItemText(const wxDataViewItem& item, const wxString& text, size_t col = 0);
        const wxString & GetItemText(const wxDataViewItem& item, size_t col = 0) const;

        void SetItemChecked(const wxDataViewItem& item, bool checked, size_t col = 0);
        bool IsItemChecked(const wxDataViewItem& item, size_t col = 0) const;

        void SetItemBold(const wxDataViewItem& item, bool bold, size_t col = 0);

        void SetItemFont(const wxDataViewItem& item, const wxFont& font, size_t col = 0);
        wxFont GetItemFont(const wxDataViewItem& item, size_t col = 0) const;

        virtual void EnableStyle(int style, bool enable, bool refresh = true);

        HeaderItem* GetColumn(size_t index);

        /**
         * @brief return item at a given row. This function is executed in O(1)
         */
        wxDataViewItem RowToItem(size_t row) const;

        /**
         * @brief return row number from item. This function is executed in O(N)
         */
        int ItemToRow(const wxDataViewItem& item) const;

        /**
         * @brief Delete the row at position row.
         */
        void DeleteItem(size_t row);
        /**
         * @brief Sets the value of a given row/col (i.e. cell)
         */
        void SetValue(const wxVariant& value, size_t row, size_t col);

        /**
         * @brief set sorting function AND apply it
         */
        void SetSortFunction(const TreeSortFunc& CompareFunc);

        /**
         * @brief remove all columns from the control
         */
        void ClearColumns();

        /**
         * @brief display a menu for a given item & column ("cell")
         */
        void ShowMenuForItem(const wxDataViewItem& item, wxMenu& menu, size_t col = 0);

        /**
         * @brief display a string selection menu for a given cell. The selection string is set as the cell text
         */
        void ShowStringSelectionMenu(const wxDataViewItem& item, const wxArrayString& choices, size_t col = 0);
    protected:
        void DoInitialize(long style);
        void OnConvertEvent(wxTreeEvent& event);
        void DoSetCellValue(RowEntry* row, size_t col, const wxVariant& value);
    private:
        wxDataViewListCtrl m_dummy; // Needed for generating wxDV compatible events

        static std::unordered_map<int, int> m_stylesMap;
    };

    // Helper class passing bitmap index + text
    class _gsgsAPI DataViewTextBitmap : public wxObject
    {
    public:
        DataViewTextBitmap(const wxString& text = wxEmptyString, int bitmapIndex = wxNOT_FOUND)
            : m_text(text)
            , m_bitmapIndex(bitmapIndex)
        {
        }

        DataViewTextBitmap(const DataViewTextBitmap& other)
            : wxObject()
            , m_text(other.m_text)
            , m_bitmapIndex(other.m_bitmapIndex)
        {
        }

        virtual ~DataViewTextBitmap() {}

        void SetText(const wxString& text)          { m_text = text; }
        const wxString & GetText() const            { return m_text; }
        void SetBitmapIndex(int index)              { m_bitmapIndex = index; }
        int GetBitmapIndex() const                  { return m_bitmapIndex; }

        bool IsSameAs(const DataViewTextBitmap& other) const
        {
            return m_text == other.m_text && m_bitmapIndex == other.m_bitmapIndex;
        }

        bool operator==(const DataViewTextBitmap& other) const { return IsSameAs(other); }

        bool operator!=(const DataViewTextBitmap& other) const { return !IsSameAs(other); }

        wxDECLARE_DYNAMIC_CLASS(DataViewTextBitmap);
    private:
        wxString m_text;
        int m_bitmapIndex;
    };

    DECLARE_VARIANT_OBJECT_EXPORTED(DataViewTextBitmap, _gsgsAPI)

    // Helper class passing bitmap bool + label with possible bitmap index
    class _gsgsAPI DataViewCheckbox : public wxObject
    {
    public:
        DataViewCheckbox(bool checked, int bitmapIndex = wxNOT_FOUND, const wxString& label = wxEmptyString)
            : m_checked(checked)
            , m_label(label)
            , m_bitmapIndex(bitmapIndex)
        {
        }

        DataViewCheckbox(const DataViewCheckbox& other)
            : wxObject()
            , m_checked(other.m_checked)
            , m_label(other.m_label)
            , m_bitmapIndex(other.m_bitmapIndex)
        {
        }

        DataViewCheckbox() {}

        virtual ~DataViewCheckbox() {}

        void SetChecked(bool checked)                       { this->m_checked = checked; }
        bool IsChecked() const                              { return m_checked; }
        void SetText(const wxString& text)                  { m_label = text; }
        wxString GetText() const                            { return m_label; }
        void SetBitmapIndex(int index)                      { m_bitmapIndex = index; }
        int GetBitmapIndex() const                          { return m_bitmapIndex; }

        bool IsSameAs(const DataViewCheckbox& other) const
        {
            return m_label == other.m_label && m_bitmapIndex == other.m_bitmapIndex && m_checked == other.m_checked;
        }

        bool operator==(const DataViewCheckbox& other) const { return IsSameAs(other); }

        bool operator!=(const DataViewCheckbox& other) const { return !IsSameAs(other); }

        wxDECLARE_DYNAMIC_CLASS(DataViewCheckbox);
    private:
        bool m_checked = false;
        wxString m_label;
        int m_bitmapIndex = wxNOT_FOUND;
    };

    DECLARE_VARIANT_OBJECT_EXPORTED(DataViewCheckbox, _gsgsAPI)

    // Helper class passing bitmap bool + label with possible bitmap index
    class _gsgsAPI DataViewChoice : public wxObject
    {
    public:
        DataViewChoice(const wxString& label, int bitmapIndex = wxNOT_FOUND)
            : m_label(label)
            , m_bitmapIndex(bitmapIndex)
        {
        }

        DataViewChoice(const DataViewChoice& other)
            : wxObject()
            , m_label(other.m_label)
            , m_bitmapIndex(other.m_bitmapIndex)
        {
        }

        DataViewChoice() {}

        virtual ~DataViewChoice() {}

        void SetBitmapIndex(int index)                      { m_bitmapIndex = index; }
        int GetBitmapIndex() const                          { return m_bitmapIndex; }

        void SetLabel(const wxString& label)                { this->m_label = label; }
        const wxString& GetLabel() const                    { return m_label; }

        bool IsSameAs(const DataViewChoice& other) const
        {
            return m_label == other.m_label && m_bitmapIndex == other.m_bitmapIndex;
        }

        bool operator==(const DataViewChoice& other) const { return IsSameAs(other); }
        bool operator!=(const DataViewChoice& other) const { return !IsSameAs(other); }

        wxDECLARE_DYNAMIC_CLASS(DataViewChoice);
    private:
        wxString m_label;
        int m_bitmapIndex = wxNOT_FOUND;
    };

    DECLARE_VARIANT_OBJECT_EXPORTED(DataViewChoice, _gsgsAPI)

    class _gsgsAPI ThemedListCtrl : public DataViewListCtrl
    {
    public:
        ThemedListCtrl();
        ThemedListCtrl(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = 0);
        virtual ~ThemedListCtrl();
    private:
        void OnTheme(wxCommandEvent& event);
        void ApplyTheme();
    private:
        TreeKeyboardInput::Ptr_t m_keyboard;
    };

    class _gsgsAPI Treebook : public wxPanel
    {
        friend class TabCtrl;
    public:
        Treebook(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxEmptyString);
        virtual ~Treebook();

        /**
         * @brief update the notebook art class and refresh
         */
        //void SetArt(TabViewPtr_t art) { mTab->SetArt(art); }

        /**
         * @brief set the notebook style. The style bits are kNotebook_* (you can set several
         * styles OR-ed)
         */
        //void SetStyle(size_t style) { PositionControls(); mTab->SetStyle(style, false); }

        /**
         * @brief set the tab direction
         */
        //void SetTabDirection(wxDirection d);

        /**
         * @brief return the book style
         */
        //size_t GetStyle() const { return mTab->GetStyle(); }

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
        bool InsertPage(const wxTreeItemId & parent, wxWindow * page, const wxString & label, bool selected = false, const wxBitmap & bmp = wxNullBitmap);

        /**
         * @brief return the currently selected page or null
         */
        wxWindow* GetCurrentPage() const;

        /**
         * @brief Returns the index of the specified tab window or wxNOT_FOUND if not found
         */
        //inline int FindPage(wxWindow* page) const { return mTab->FindPage(page); }

        /**
         * @brief Deletes the specified page, without deleting the associated window
         */
        //inline bool RemovePage(size_t page, bool notify = false) { return mTab->RemovePage(page, notify, false); }

        /**
         * @brief Deletes the specified page and the associated window
         */
        //inline bool DeletePage(size_t page, bool notify = true) { return mTab->RemovePage(page, notify, true); }

        /**
         * @brief Deletes all pages
         */
        //inline bool DeleteAllPages() { return mTab->DeleteAllPages(); }

        /**
         * @brief set a new selection. This function fires an event that can be vetoed
         */
        //int SetSelection(size_t selection);
        /**
         * @brief set new selection. No events are fired
         */
        //inline int ChangeSelection(size_t selection) { return mTab->ChangeSelection(selection); }

        /**
         * @brief return the currently selected page, return wxNOT_FOUND if non found
         */
        //int GetSelection() const;

        /**
         * @brief Sets the text for the given page.
         */
        //inline bool SetPageText(size_t page, const wxString& text) { return mTab->SetPageText(page, text); }

        /**
         * @brief Returns the string for the given page
         */
        //inline wxString GetPageText(size_t page) const { return mTab->GetPageText(page); }

        /**
         * @brief set the image for the given page
         */
        //void SetPageBitmap(size_t index, const wxBitmap& bmp, bool refresh = true) { mTab->SetPageBitmap(index, bmp, refresh); }

        /**
         * @brief return bitmap for a given page. Return wxNullBitmap if invalid page
         */
        //wxBitmap GetPageBitmap(size_t index) const { return mTab->GetPageBitmap(index); }

        // Base class members...
        //virtual bool SetPageImage(size_t, int) { return false; }
        //virtual int GetPageImage(size_t n) const { return wxNOT_FOUND; }

        /**
         * @brief return the index of a given window in the tab control
         * @param window
         * @return return window index, or wxNOT_FOUND
         */
        //inline int GetPageIndex(wxWindow* window) const { return mTab->DoGetPageIndex(window); }

        /**
         * @brief return the index of a given window by its title
         */
        //inline int GetPageIndex(const wxString& label) const { return mTab->DoGetPageIndex(label); }

        /**
         * @brief Returns the number of pages in the control
         */
        //inline size_t GetPageCount() const { return mTab->GetTabs().size(); }

        /**
         * @brief Returns the window at the given page position.
         */
        //inline wxWindow* GetPage(size_t index) const { return mTab->GetPage(index); }

        /**
         * @brief return an array of all the windows managed by this notebook
         */
        //inline void GetAllPages(WindowList& pages) { mTab->GetAllPages(pages); }

        /**
         * @brief return all tabs info
         * @param tabs [output]
         */
        /*inline size_t GetAllTabs(TabInfoVec_t& tabs)
        {
            tabs = mTab->GetTabs();
            return tabs.size();
        }*/

        /**
         * @brief set a context menu to be shown whe context menu is requested
         * on a tab label
         */
        //void SetMenu(wxMenu* menu) { mTab->SetMenu(menu); }

        /**
         * @brief Sets the tool tip displayed when hovering over the tab label of the page
         * @return true if tool tip was updated, false if it failed, e.g. because the page index is invalid.
         */
        //inline bool SetPageToolTip(size_t page, const wxString& tooltip) { return mTab->SetPageToolTip(page, tooltip); }

        /**
         * @brief return the tabbing history
         * @return
         */
        //inline TabHistoryPtr_t GetHistory() const { return mTab->GetHistory(); }

        /**
         * @brief move the active page and place it in the new nexIndex
         */
        //inline bool MoveActivePage(int newIndex) { return mTab->MoveActiveToIndex(newIndex, GetSelection() > newIndex ? eDirection::kLeft : eDirection::kRight); }
    protected:
        virtual void OnSelected(wxTreeEvent& event);
        //inline bool IsVerticalStyle() const { return mTab->IsVerticalStyle(); }
        //void OnSize(wxSizeEvent& event);
        //void PositionControls();
    private:
        WindowStack * mWinList;
        TreeCtrl * mTreeCtrl;
    };

    class wxCheckTreeCtrl : public wxTreeCtrl
    {
        wxBitmap m_checkedBmp;
        wxBitmap m_uncheckedBmp;

    protected:
        void GetItemChildrenRecursive(const wxTreeItemId &parent, std::list<wxTreeItemId> &children);

    public:
        wxCheckTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT("treeCtrl"));
        virtual ~wxCheckTreeCtrl();

        //override item managements functions
        virtual wxTreeItemId AddRoot(const wxString& text, bool checked, wxTreeItemData* data = NULL);
        virtual wxTreeItemId AppendItem(const wxTreeItemId& parent, const wxString& text, bool checked, wxTreeItemData* data = NULL);

        bool IsChecked(const wxTreeItemId& item) const;
        virtual void Check(const wxTreeItemId& item, bool check = true);
        void RecursiveCheck(const wxTreeItemId &item, bool check = true);

        DECLARE_EVENT_TABLE();
        virtual void OnLeftDown(wxMouseEvent &event);
    };

    /**
     * \brief Holds information about events associated with wxCheckTreeCtrl objects
     * This event is fired when an item is (un)selected. use the:
     * \code
     * EVT_CHECKTREE_ITEM_SELECTED() //wxEVT_CKTR_ITEM_SELECTED
     * EVT_CHECKTREE_ITEM_UNSELECTED() //wxEVT_CKTR_ITEM_UNSELECTED
     * \code
     * macros to handle them.
     */
    class wxCheckTreeCtrlEvent : public wxNotifyEvent
    {
        wxTreeItemId m_item;

    public:
        /**
         * \param commandType - event type
         * \param winid - window ID
         */
        wxCheckTreeCtrlEvent(wxEventType commandType = wxEVT_NULL, int winid = 0) : wxNotifyEvent(commandType, winid) {}
        virtual ~wxCheckTreeCtrlEvent() {}

        void SetItem(const wxTreeItemId &item) { m_item = item; }
        const wxTreeItemId& GetItem() const { return m_item; }
    };

    extern const wxEventType wxEVT_CKTR_ITEM_UNSELECTED;
    extern const wxEventType wxEVT_CKTR_ITEM_SELECTED;

    typedef void (wxEvtHandler::*wxCheckTreeCtrlEventFunction)(wxCheckTreeCtrlEvent&);

#define wxCheckTreeCtrlEventHandler(func) \
	(wxObjectEventFunction)(wxEventFunction)wxStaticCastEvent(wxCheckTreeCtrlEventFunction, &func)

#define EVT_CHECKTREE_ITEM_SELECTED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_CKTR_ITEM_SELECTED, winid, wxCheckTreeCtrlEventHandler(fn))

#define EVT_CHECKTREE_ITEM_UNSELECTED(winid, fn) \
	wx__DECLARE_EVT1(wxEVT_CKTR_ITEM_UNSELECTED, winid, wxCheckTreeCtrlEventHandler(fn))

}
#endif 
