
#include "gsgsTreeCtrl.h"
#include "gsgsStyleManager.h"
#include "gsgsConfig.h"

#include <wx/app.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include <wx/dcmemory.h>
#include <wx/dnd.h>
#include <wx/renderer.h>
#include <wx/settings.h>
#include <wx/wupdlock.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include <cmath>
#include <cstdlib>
#include <wx/imaglist.h>

#ifdef __WXMSW__
    #include <uxtheme.h>
#endif

#define TREE_STYLE wxTR_ENABLE_SEARCH | wxBORDER_NONE

#undef CHECK_PTR_RET
#define CHECK_PTR_RET(p) if(!p) { return; }
#undef CHECK_ITEM_RET
#define CHECK_ITEM_RET(item) if(!item.IsOk()) { return; }
#undef CHECK_ITEM_RET_INVALID_ITEM
#define CHECK_ITEM_RET_INVALID_ITEM(item) if(!item.IsOk()) { return wxTreeItemId(); }
#undef CHECK_ITEM_RET_FALSE
#define CHECK_ITEM_RET_FALSE(item) if(!item.IsOk()) { return false; }
#define CHECK_ROOT_RET() if(!m_model.GetRoot()) { return; }

#ifdef __WXMSW__
#define LIST_STYLE wxDV_ROW_LINES | wxDV_ENABLE_SEARCH | wxBORDER_NONE
#else
#define LIST_STYLE wxDV_ROW_LINES | wxDV_ENABLE_SEARCH | wxBORDER_NONE
#endif

wxDEFINE_EVENT(wxEVT_TREE_ITEM_VALUE_CHANGED, wxTreeEvent);
wxDEFINE_EVENT(wxEVT_TREE_CHOICE, wxTreeEvent);

wxDEFINE_EVENT(wxEVT_DATAVIEW_SEARCH_TEXT, wxDataViewEvent);
wxDEFINE_EVENT(wxEVT_DATAVIEW_CLEAR_SEARCH, wxDataViewEvent);
wxDEFINE_EVENT(wxEVT_DATAVIEW_CHOICE_BUTTON, wxDataViewEvent);
wxDEFINE_EVENT(wxEVT_DATAVIEW_CHOICE, wxDataViewEvent);

namespace gsgs
{
    //-------------------------------------------------------------------------------------
    #define INDENT_SIZE 16
    static void MSWSetNativeTheme(wxWindow* win)
    {
    #ifdef __WXMSW__
        SetWindowTheme((HWND)win->GetHWND(), wxT("Resource"), NULL);
    #endif
    }
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // TreeNodeVisitor
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    class TreeNodeVisitor
    {
    public:
        TreeNodeVisitor(){}
        virtual ~TreeNodeVisitor(){}

        /**
         * @brief Visit the tree in order
         * @param item item to start from
         * @param expandedItemsOnly set to false to visit all items, regardless of their state
         * @param VisitFunc a callback to be called per item. The CB signature is (RowEntry* child)
         * the callback returns true to continue, false to stop
         */
        void Visit(RowEntry* item, bool expandedItemsOnly, std::function<bool(RowEntry*, bool)>& func)
        {
            RowEntry* current = item;
            while(current) {
                bool isVisible = current->IsVisible();
                if(expandedItemsOnly && isVisible) {
                    if(!func(current, isVisible)) { return; }
                } else if(!expandedItemsOnly) {
                    if(!func(current, isVisible)) { return; }
                }
                current = current->GetNext();
            }
        }
    };
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // TreeCtrlModel
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    TreeCtrlModel::TreeCtrlModel() :
        mSortFunc(0),
        m_tree(0)
    {
    }
    //-------------------------------------------------------------------------------------
    TreeCtrlModel::TreeCtrlModel(TreeCtrl* tree)
        : m_tree(tree)
    {
        // Setup a default compare function
        mSortFunc = [&](RowEntry* a, RowEntry* b) {
            return a->GetLabel(0).CmpNoCase(b->GetLabel(0)) < 0;
        };
    }
    //-------------------------------------------------------------------------------------
    TreeCtrlModel::~TreeCtrlModel()
    {
        m_shutdown = true; // Disable events
        wxDELETE(m_root);
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::GetNextItems(RowEntry* from, int count, RowEntry::Vec_t& items, bool selfIncluded) const
    {
        if (count > 0)
        {
            //items.reserve(count);
            from->GetNextItems(count, items, selfIncluded);
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::GetPrevItems(RowEntry* from, int count, RowEntry::Vec_t& items, bool selfIncluded) const
    {
        return from->GetPrevItems(count, items, selfIncluded);
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrlModel::AddRoot(const wxString& text, int image, int selImage, wxTreeItemData* data)
    {
        if (!m_root)
        {
            m_root = new RowEntry(m_tree, text, image, selImage);
            m_root->SetClientObject(data);
            if (m_tree->GetTreeStyle() & wxTR_HIDE_ROOT) {
                m_root->SetHidden(true);
                m_root->SetExpanded(true);
            }
        }
        return wxTreeItemId(m_root);
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrlModel::GetRootItem() const
    {
        if(m_root)
            return wxTreeItemId(const_cast<RowEntry*>(m_root));
        return wxTreeItemId();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::UnselectAll()
    {
        for(size_t i = 0; i < m_selectedItems.size(); ++i) 
        {
            m_selectedItems[i]->SetSelected(false);
        }
        m_selectedItems.clear();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::SelectItem(const wxTreeItemId& item, bool select_it, bool addSelection, bool clear_old_selection)
    {
        RowEntry* child = ToPtr(item);
        if (child || !child->IsHidden())
        {
            if (clear_old_selection && !ClearSelections(item != GetSingleSelection())) { return; }

            if (select_it) {
                RowEntry::Vec_t::iterator iter =
                    std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](RowEntry* p) { return (p == child); });
                // If the item is already selected, don't select it again
                if (iter != m_selectedItems.end()) { return; }
            }

            // Fire an event only if the was no selection prior to this item
            bool fire_event = (m_selectedItems.empty() && select_it);

            if (IsMultiSelection() && addSelection) {
                // If we are unselecting it, remove it from the array
                RowEntry::Vec_t::iterator iter =
                    std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](RowEntry* p) { return (p == child); });
                if (iter != m_selectedItems.end() && !select_it) { m_selectedItems.erase(iter); }
            }
            else {
                if (!ClearSelections(item != GetSingleSelection())) { return; }
            }
            child->SetSelected(select_it);
            m_selectedItems.push_back(child);
            if (fire_event) {
                wxTreeEvent evt(wxEVT_TREE_SEL_CHANGED);
                evt.SetEventObject(m_tree);
                evt.SetItem(wxTreeItemId(child));
                evt.SetLabel(child->GetLabel());
                SendEvent(evt);
            }
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::Clear()
    {
        m_selectedItems.clear();
        for(size_t i = 0; i < m_onScreenItems.size(); ++i) {
            m_onScreenItems[i]->ClearRects();
        }
        m_onScreenItems.clear();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::ClearScreenItems()
    {
        for(size_t i = 0; i < m_onScreenItems.size(); ++i) {
            m_onScreenItems[i]->ClearRects();
        }
        m_onScreenItems.clear();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::SetOnScreenItems(const RowEntry::Vec_t& items)
    {
        // Clear the old visible items. But only, if the item does not appear in both lists
        /*for(size_t i = 0; i < m_onScreenItems.size(); ++i) {
            RowEntry* visibleItem = m_onScreenItems[i];
            RowEntry::Vec_t::const_iterator iter =
                std::find_if(items.begin(), items.end(), [&](RowEntry* item) { return item == visibleItem; });
            if(iter == items.end()) { m_onScreenItems[i]->ClearRects(); }
        }*/
        m_onScreenItems = items;
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrlModel::ExpandToItem(const wxTreeItemId& item)
    {
        RowEntry* child = ToPtr(item);
        if (child)
        {
            RowEntry* parent = child->GetParent();
            while (parent) {
                if (!parent->SetExpanded(true)) { return false; }
                parent = parent->GetParent();
            }
            return true;
        }
        return false;
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrlModel::AppendItem(const wxTreeItemId& parent, const wxString& text, int image, int selImage,
                                             wxTreeItemData* data)
    {
        if (parent.IsOk())
        {
            RowEntry* parentNode = ToPtr(parent);

            RowEntry* child = new RowEntry(m_tree, text, image, selImage);
            child->SetClientObject(data);

            if (!parentNode->IsRoot() && (m_tree->GetTreeStyle() & wxTR_SORT_TOP_LEVEL)) {
                // We have been requested to sort top level items only
                parentNode->AddChild(child);
            }
            else if (mSortFunc != nullptr && m_tree->IsSortEnable()) {
                // Find the best insertion point
                RowEntry* prevItem = nullptr;
                const RowEntry::Vec_t& children = parentNode->GetChildren();
                // Loop over the parent's children and add execute the compare function
                for (int i = ((int)children.size() - 1); i >= 0; --i) {
                    if (!mSortFunc(child, children[i])) {
                        // Our item should be placed _after_ children[i]s
                        prevItem = ToPtr(children[i]);
                        break;
                    }
                }
                parentNode->InsertChild(child, prevItem);
            }
            else {
                parentNode->AddChild(child);
            }
            return wxTreeItemId(child);
        }
        return wxTreeItemId();
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrlModel::InsertItem(const wxTreeItemId& parent, const wxTreeItemId& previous, const wxString& text,
                                             int image, int selImage, wxTreeItemData* data)
    {
        if (parent.IsOk() && previous.IsOk())
        {
            RowEntry* pPrev = ToPtr(previous);
            RowEntry* parentNode = ToPtr(parent);
            if (pPrev->GetParent() != parentNode) { return wxTreeItemId(); }

            RowEntry* child = new RowEntry(m_tree, text, image, selImage);
            child->SetClientObject(data);
            parentNode->InsertChild(child, pPrev);
            return wxTreeItemId(child);
        }
        return wxTreeItemId();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::DoExpandAllChildren(const wxTreeItemId& item, bool expand)
    {
        RowEntry* p = ToPtr(item);
        if (p)
        {
            while (p) {
                if (p->HasChildren()) {
                    if (expand && !p->IsExpanded()) {
                        p->SetExpanded(true);
                    }
                    else if (!expand && p->IsExpanded()) {
                        p->SetExpanded(false);
                    }
                }
                p = p->GetNext();
            }
        }
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrlModel::GetItemBefore(const wxTreeItemId & item, bool visibleItem) const
    {
        return wxTreeItemId(GetRowBefore(ToPtr(item), visibleItem));
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrlModel::GetItemAfter(const wxTreeItemId & item, bool visibleItem) const
    {
        return wxTreeItemId(GetRowAfter(ToPtr(item), visibleItem));
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::DeleteItem(const wxTreeItemId & item)
    {
        RowEntry * node = ToPtr(item);
        if(node)
        {
            node->DeleteAllChildren();

            // Send the delete event
            wxTreeEvent event(wxEVT_TREE_DELETE_ITEM);
            event.SetEventObject(m_tree);
            event.SetItem(item);
            SendEvent(event);

            // Delete the item itself
            if(node->GetParent()) {
                node->GetParent()->DeleteChild(node);
            } else {
                // The root item
                wxDELETE(m_root);
            }
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::NodeDeleted(RowEntry * node)
    {
        // Clear the various caches
        {
            RowEntry::Vec_t::iterator iter =
                std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](RowEntry* n) { return n == node; });
            if(iter != m_selectedItems.end()) {
                m_selectedItems.erase(iter);
                if(m_selectedItems.empty()) {
                    // Dont leave the tree without a selected item
                    if(node->GetNext()) { SelectItem(wxTreeItemId(node->GetNext())); }
                }
            }
        }

        {
            RowEntry::Vec_t::iterator iter =
                std::find_if(m_onScreenItems.begin(), m_onScreenItems.end(), [&](RowEntry* n) { return n == node; });
            if(iter != m_onScreenItems.end()) { m_onScreenItems.erase(iter); }
        }
        {
            if(m_firstItemOnScreen == node) { m_firstItemOnScreen = nullptr; }
        }
        {
            if(m_root == node) { m_root = nullptr; }
        }
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrlModel::NodeExpanding(RowEntry* node, bool expanding)
    {
        wxTreeEvent before(expanding ? wxEVT_TREE_ITEM_EXPANDING : wxEVT_TREE_ITEM_COLLAPSING);
        before.SetItem(wxTreeItemId(node));
        before.SetEventObject(m_tree);
        SendEvent(before);
        return before.IsAllowed();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::NodeExpanded(RowEntry* node, bool expanded)
    {
        wxTreeEvent after(expanded ? wxEVT_TREE_ITEM_EXPANDED : wxEVT_TREE_ITEM_COLLAPSED);
        after.SetItem(wxTreeItemId(node));
        after.SetEventObject(m_tree);
        SendEvent(after);
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrlModel::IsSingleSelection() const 
    { 
        return m_tree && !(m_tree->GetTreeStyle() & wxTR_MULTIPLE); 
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrlModel::IsMultiSelection() const 
    { 
        return m_tree && (m_tree->GetTreeStyle() & wxTR_MULTIPLE); 
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrlModel::SendEvent(wxEvent& event)
    {
        if(m_shutdown) { return false; }
        return m_tree->GetEventHandler()->ProcessEvent(event);
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrlModel::GetSingleSelection() const
    {
        if(m_selectedItems.empty()) { return wxTreeItemId(); }
        return wxTreeItemId(m_selectedItems.back());
    }
    //-------------------------------------------------------------------------------------
    int TreeCtrlModel::GetItemIndex(RowEntry* item) const
    {
        if (item)
        {
            if (m_root)
            {
                int counter = 0;
                RowEntry* current = m_root;
                while (current) {
                    if (current == item) { return counter; }
                    if (current->IsVisible()) { ++counter; }
                    current = current->GetNext();
                }
            }
        }
        return wxNOT_FOUND;
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrlModel::GetRange(RowEntry* from, RowEntry* to, RowEntry::Vec_t& items) const
    {
        items.clear();
        if(from == nullptr || to == nullptr) { return false; }
        if(from == nullptr) {
            items.push_back(to);
            return true;
        }
        if(to == nullptr) {
            items.push_back(from);
            return true;
        }
        if(from == to) {
            items.push_back(from);
            return true;
        }

        int index1 = GetItemIndex(from);
        int index2 = GetItemIndex(to);

        RowEntry* start_item = index1 > index2 ? to : from;
        RowEntry* end_item = index1 > index2 ? from : to;
        RowEntry* current = start_item;
        while(current) {
            if(current == end_item) {
                items.push_back(current);
                break;
            }
            if(current->IsVisible()) { items.push_back(current); }
            current = current->GetNext();
        }
        return true;
    }
    //-------------------------------------------------------------------------------------
    size_t TreeCtrlModel::GetExpandedLines() const
    {
        if (GetRoot())
        {
            return m_root->GetExpandedLines();
        }
        return 0;
    }
    //-------------------------------------------------------------------------------------
    RowEntry* TreeCtrlModel::GetItemFromIndex(int index) const
    {
        if(index < 0) { return nullptr; }
        if (m_root)
        {
            int curIndex = -1;
            RowEntry* current = m_root;
            while (current) {
                if (current->IsVisible()) { ++curIndex; }
                if (curIndex == index) { return current; }
                current = current->GetNext();
            }
        }
        return nullptr;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::SelectChildren(const wxTreeItemId& item)
    {
        RowEntry* parent = ToPtr(item);
        if (parent)
        {
            if (ClearSelections(true))
            {
                std::for_each(parent->GetChildren().begin(), parent->GetChildren().end(),
                    [&](RowEntry* child) { AddSelection(wxTreeItemId(child)); });
            }
        }
    }
    //-------------------------------------------------------------------------------------
    RowEntry* TreeCtrlModel::GetNextSibling(RowEntry* item) const
    {
        if(!item->GetParent()) { return nullptr; }
        const RowEntry::Vec_t& children = item->GetParent()->GetChildren();
        if(children.empty()) { return nullptr; }
        size_t where = -1;
        for(size_t i = 0; i < children.size(); ++i) {
            if(item == children[i]) {
                where = i;
                break;
            }
        }

        // if we couldnt find 'item' in the children list or if it's the last child
        // return nullptr
        if((where == (size_t)-1) || (where == (children.size() - 1))) { return nullptr; }
        ++where;
        return children[where];
    }
    //-------------------------------------------------------------------------------------
    RowEntry* TreeCtrlModel::GetPrevSibling(RowEntry* item) const
    {
        if(!item->GetParent()) { return nullptr; }
        const RowEntry::Vec_t& children = item->GetParent()->GetChildren();
        if(children.empty()) { return nullptr; }
        size_t where = -1;
        for(size_t i = 0; i < children.size(); ++i) {
            if(item == children[i]) {
                where = i;
                break;
            }
        }

        // if we couldnt find item in the children list or if it's the first child
        // we return nullptr
        if((where == (size_t)-1) || (where == (size_t)0)) { return nullptr; }

        --where;
        return children[where];
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlModel::AddSelection(const wxTreeItemId& item)
    {
        RowEntry* child = ToPtr(item);
        if(child)
        {
            if(child->IsHidden()) { return; }

            RowEntry::Vec_t::iterator iter =
                std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](RowEntry* p) { return (p == child); });
            // If the item is already selected, don't select it again
            if(iter != m_selectedItems.end()) { return; }

            // if we already got selections, notify about the change
            //    if(!m_selectedItems.empty()) {
            //        wxTreeEvent evt(wxEVT_TREE_SEL_CHANGING);
            //        evt.SetEventObject(m_tree);
            //        evt.SetOldItem(GetSingleSelection());
            //        SendEvent(evt);
            //        if(!evt.IsAllowed()) { return; }
            //    }

            child->SetSelected(true);
            // Send 'SEL_CHANGED' event
            m_selectedItems.push_back(child);
            if(m_selectedItems.size() == 1) {
                wxTreeEvent evt(wxEVT_TREE_SEL_CHANGED);
                evt.SetEventObject(m_tree);
                evt.SetItem(wxTreeItemId(child));
                evt.SetLabel("");
                SendEvent(evt);
            }
        }
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrlModel::ClearSelections(bool notify)
    {
        if(m_selectedItems.empty()) { return true; }

        if(notify) {
            // Check if we can proceed with this operation
            wxTreeEvent evt(wxEVT_TREE_SEL_CHANGING);
            evt.SetEventObject(m_tree);
            evt.SetOldItem(GetSingleSelection());
            SendEvent(evt);
            if(!evt.IsAllowed()) { return false; }
        }
        UnselectAll();
        return true;
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrlModel::IsItemSelected(const RowEntry* item) const
    {
        if(item == nullptr) { return false; }
        if(m_selectedItems.empty()) { return false; }
        RowEntry::Vec_t::const_iterator iter =
            std::find_if(m_selectedItems.begin(), m_selectedItems.end(), [&](RowEntry* p) { return (p == item); });
        return (iter != m_selectedItems.end());
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrlModel::IsVisible(const wxTreeItemId& item) const
    {
        if (item.IsOk())
        {
            RowEntry* entry = ToPtr(item);
            RowEntry::Vec_t::const_iterator iter =
                std::find_if(m_onScreenItems.begin(), m_onScreenItems.end(), [&](RowEntry* p) { return (p == entry); });
            return (iter != m_onScreenItems.end());
        }
        return false;
    }
    //-------------------------------------------------------------------------------------
    RowEntry* TreeCtrlModel::GetRowBefore(RowEntry* item, bool visibleItem) const
    {
        RowEntry* curp = item;
        if (curp)
        {
            curp = curp->GetPrev();
            while (curp) {
                if (visibleItem && !curp->IsVisible()) {
                    curp = curp->GetPrev();
                    continue;
                }
                break;
            }
            return curp;
        }
        return nullptr;
    }
    //-------------------------------------------------------------------------------------
    RowEntry* TreeCtrlModel::GetRowAfter(RowEntry* item, bool visibleItem) const
    {
        RowEntry* curp = item;
        if (curp)
        {
            curp = curp->GetNext();
            while (curp) {
                if (visibleItem && !curp->IsVisible()) {
                    curp = curp->GetNext();
                    continue;
                }
                break;
            }
            return curp;
        }
        return nullptr;
    }
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // TreeCtrl
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    TreeCtrl::TreeCtrl()
        : m_model(this)
    {
    }
    //-------------------------------------------------------------------------------------
    TreeCtrl::TreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : ItemPanel(parent, id, pos, size, style | wxWANTS_CHARS)
        , m_model(this)
    {
        m_treeStyle = style & ~wxWINDOW_STYLE_MASK; // remove the non window style
        DoInitialize();
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    {
        if (ItemPanel::Create(parent, id, pos, size, style | wxWANTS_CHARS))
        {
            m_treeStyle = style & ~wxWINDOW_STYLE_MASK;
            DoInitialize();
            return true;
        }
        return false;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::DoInitialize()
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        wxSize textSize = GetTextSize("Tp");
        SetLineHeight(RowEntry::Y_SPACER + textSize.GetHeight() + RowEntry::Y_SPACER);
        SetIndent(GetLineHeight());

        Bind(wxEVT_PAINT, &TreeCtrl::OnPaint, this);
        Bind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) { });
        Bind(wxEVT_LEFT_DOWN, &TreeCtrl::OnMouseLeftDown, this);
        Bind(wxEVT_LEFT_UP, &TreeCtrl::OnMouseLeftUp, this);
        Bind(wxEVT_LEFT_DCLICK, &TreeCtrl::OnMouseLeftDClick, this);
        Bind(wxEVT_LEAVE_WINDOW, &TreeCtrl::OnLeaveWindow, this);
        //Bind(wxEVT_ENTER_WINDOW, &TreeCtrl::OnEnterWindow, this);
        Bind(wxEVT_CONTEXT_MENU, &TreeCtrl::OnContextMenu, this);
        Bind(wxEVT_RIGHT_DOWN, &TreeCtrl::OnRightDown, this);

        // Initialise default colours
        GetColours().setDefault();

        // There is always a header
        GetHeader()->Add("");
        //SetShowHeader(false);
        MSWSetNativeTheme(this);
    }
    //-------------------------------------------------------------------------------------
    TreeCtrl::~TreeCtrl()
    {
        Unbind(wxEVT_PAINT, &TreeCtrl::OnPaint, this);
        Unbind(wxEVT_ERASE_BACKGROUND, [&](wxEraseEvent&) { });
        Unbind(wxEVT_LEFT_DOWN, &TreeCtrl::OnMouseLeftDown, this);
        Unbind(wxEVT_LEFT_UP, &TreeCtrl::OnMouseLeftUp, this);
        Unbind(wxEVT_LEFT_DCLICK, &TreeCtrl::OnMouseLeftDClick, this);
        Unbind(wxEVT_LEAVE_WINDOW, &TreeCtrl::OnLeaveWindow, this);
        //Unbind(wxEVT_ENTER_WINDOW, &TreeCtrl::OnEnterWindow, this);
        Unbind(wxEVT_CONTEXT_MENU, &TreeCtrl::OnContextMenu, this);
        Unbind(wxEVT_RIGHT_DOWN, &TreeCtrl::OnRightDown, this);
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::OnPaint(wxPaintEvent&)
    {
        wxAutoBufferedPaintDC pdc(this);
        PrepareDC(pdc);
        wxGCDC dc(pdc);

        // Call the parent's Render method
        Render(dc);

        if(!m_model.GetRoot()) {
            // Reset the various items
            SetFirstItemOnScreen(nullptr);
            //RowEntry::Vec_t items;
            //m_model.SetOnScreenItems(items);
            m_model.ClearScreenItems();
            return;
        }

        //m_needToUpdateScrollbar = false;
        if(!GetFirstItemOnScreen()) {
            SetFirstItemOnScreen(m_model.GetRoot());
            //m_needToUpdateScrollbar = true;
        }
        RowEntry* firstItem = GetFirstItemOnScreen();
        if(!firstItem) { return; }

        // Get list of items to draw
        RowEntry::Vec_t items;
        size_t maxItems = GetNumLineCanFitOnScreen();
        if(maxItems == 0) { return; }
        m_model.GetNextItems(firstItem, maxItems, items);

        // Always try to get maximum entries to draw on the canvas
        if(items.empty()) { return; }
        bool canScrollDown = GetVScrollBar()->CanScollDown();

        // An action took that requires us to try to maximimze the list
        if(m_maxList) {
            while(
                (canScrollDown &&
                 (items.size() < maxItems)) || // While can move the scroll thumb a bit further down, increase the list size
                (!canScrollDown &&
                 (items.size() < (maxItems - 1)))) { // the scroll thumb cant be moved further down, so it
                                                     // makes no sense on hiding the last item (we wont be
                                                     // able to reach it), so make sure we extend the list
                                                     // up to max-items -1, this means that the last item is
                                                     // always fully visible
                firstItem = m_model.GetRowBefore(firstItem, true);
                if(!firstItem) { break; }
                items.insert(items.begin(), firstItem);
                //m_needToUpdateScrollbar = true;
            }
        }
        m_maxList = false;
        // So we increased the list to display as much as items as we can.
        // However, if the last item in the view is not visible, make it so
        RowEntry* lastRow = items.back();
        if(lastRow && lastRow->IsSelected()) {
            AssignRects(items);
            if(!IsItemFullyVisible(lastRow)) 
            {
                // Remove the first item from the list and append new item at the bottom
                // basically what we are doing here is sliding the window (visible items) 1 row up
                items.erase(items.begin());
                RowEntry* new_last_item = m_model.GetRowAfter(lastRow, true);
                if(new_last_item) { items.push_back(new_last_item); }
            }
        }
        m_model.ClearScreenItems();
        // Update the first item on screen
        SetFirstItemOnScreen(firstItem);

        // Draw the items
        wxRect clientRect = GetItemsRect();
        // Set the width of the clipping region to match the header's width
        clientRect.SetWidth(clientRect.GetWidth() + m_firstColumn + 1);
        dc.SetClippingRegion(clientRect);
        RenderItems(dc, items);
        dc.DestroyClippingRegion();

        // Keep the visible items
        m_model.SetOnScreenItems(items); // Keep track of the visible items
        //if(m_needToUpdateScrollbar) 
        //{ 
            //CallAfter(&TreeCtrl::UpdateScrollBar); 
            //m_needToUpdateScrollbar = false;
        //}
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::InsertItem(const wxTreeItemId& parent, const wxTreeItemId& previous, const wxString& text,
                                        int image, int selImage, wxTreeItemData* data)
    {
        wxTreeItemId item = m_model.InsertItem(parent, previous, text, image, selImage, data);
        DoUpdateHeader(item);
        if(IsExpanded(parent)) { UpdateScrollBar(); }
        return item;
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::AppendItem(const wxString& text, int image, int selImage,
        wxTreeItemData* data)
    {
        wxTreeItemId item = m_model.AppendItem(GetRootItem(), text, image, selImage, data);
        DoUpdateHeader(item);
        if(IsExpanded(GetRootItem())) { UpdateScrollBar(); }
        return item;
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::AppendItem(const wxTreeItemId& parent, const wxString& text, int image, int selImage,
                                        wxTreeItemData* data)
    {
        wxTreeItemId item = m_model.AppendItem(parent, text, image, selImage, data);
        DoUpdateHeader(item);
        if(IsExpanded(parent)) { UpdateScrollBar(); }
        return item;
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::AddRoot(const wxString& text, int image, int selImage, wxTreeItemData* data)
    {
        wxTreeItemId root = m_model.AddRoot(text, image, selImage, data);
        DoUpdateHeader(root);
        return root;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::Expand(const wxTreeItemId& item)
    {
        CHECK_ITEM_RET(item);
        RowEntry* child = m_model.ToPtr(item);
        if (child)
        {
            child->SetExpanded(true);
            m_maxList = true;
            DoUpdateHeader(item);
            //m_needToUpdateScrollbar = false;
            UpdateScrollBar();
            Refresh();
            //if (GetVScrollBar()->IsShown()) { GetVScrollBar()->CallAfter(&wxWindow::Update); }
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::Collapse(const wxTreeItemId& item)
    {
        CHECK_ITEM_RET(item);
        RowEntry* child = m_model.ToPtr(item);
        if (child)
        {
            child->SetExpanded(false);
            m_maxList = true;
            //m_needToUpdateScrollbar = false;
            UpdateScrollBar();
            DoUpdateHeader(item);
            Refresh();
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SelectItem(const wxTreeItemId& item, bool select)
    {
        CHECK_ITEM_RET(item);
        if((select && m_model.IsItemSelected(item)) || (!select && !m_model.IsItemSelected(item))) { return; }
        m_model.SelectItem(item, select, false, true);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::OnMouseLeftDown(wxMouseEvent& event)
    {
        event.Skip();
        CHECK_ROOT_RET();
        int flags = 0;
        int column = wxNOT_FOUND;
        wxPoint pt = DoFixPoint(event.GetPosition());
        wxTreeItemId where = HitTest(pt, flags, column);
        if(where.IsOk()) {
            if(flags & wxTREE_HITTEST_ONITEMBUTTON) {
                if(IsExpanded(where)) {
                    Collapse(where);
                } else {
                    Expand(where);
                }
            } else {
                RowEntry* pNode = m_model.ToPtr(where);
                if(flags & wxTREE_HITTEST_ONITEMSTATEICON) {
                    // Change the state
                    Check(where, !IsChecked(where, column), column);
                }
                bool has_multiple_selection = (m_model.GetSelectionsCount() > 1);
                if(HasStyle(wxTR_MULTIPLE)) {
                    if(event.ControlDown()) {
                        // Toggle the selection
                        m_model.SelectItem(where, !pNode->IsSelected(), true);
                    } else if(event.ShiftDown()) {
                        // Range selection
                        RowEntry::Vec_t range;
                        m_model.GetRange(pNode, m_model.ToPtr(m_model.GetSingleSelection()), range);
                        std::for_each(range.begin(), range.end(),
                                      [&](RowEntry* p) { m_model.AddSelection(wxTreeItemId(p)); });
                    } else {
                        // The default, single selection
                        if(!has_multiple_selection && pNode->IsSelected()) {
                            // Nothing to be done here
                        } else if(!has_multiple_selection && !pNode->IsSelected()) {
                            // We got other selection (1), but not this one, select it
                            m_model.SelectItem(where, true, false, true);
                            EnsureVisible(where);
                        } else if(has_multiple_selection && !pNode->IsSelected()) {
                            // we got multiple selections and we want to select a new item, what we do
                            // is we clear the current selections and select the new item
                            m_model.SelectItem(where, true, false, true);
                            EnsureVisible(where);
                        }
                    }

                } else {
                    if(GetSelection() != wxTreeItemId(pNode)) {
                        // Select it
                        SelectItem(wxTreeItemId(pNode));
                        EnsureVisible(where);
                    }
                }

                if((flags & wxTREE_HITTEST_ONDROPDOWNARROW) && !has_multiple_selection) {
                    wxTreeEvent evt(wxEVT_TREE_CHOICE);
                    evt.SetInt(column);
                    evt.SetEventObject(this);
                    evt.SetItem(where);
                    GetEventHandler()->ProcessEvent(evt);
                }
            }
            Refresh();
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::OnMouseLeftUp(wxMouseEvent& event)
    {
        event.Skip();
        int flags = 0;
        wxPoint pt = DoFixPoint(event.GetPosition());
        int column = wxNOT_FOUND;
        wxTreeItemId where = HitTest(pt, flags, column);
        if(where.IsOk() && (flags & wxTREE_HITTEST_ONITEM)) {
            bool has_multiple_selection = (m_model.GetSelectionsCount() > 1);
            RowEntry* pNode = m_model.ToPtr(where);
            if(has_multiple_selection && pNode->IsSelected() && !event.HasAnyModifiers()) {
                // Select this item while clearing the others
                m_model.SelectItem(where, true, false, true);
                Refresh();
            }
        }
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::HitTest(const wxPoint& point, int& flags, int& column) const
    {
        column = wxNOT_FOUND;
        flags = 0;
        if (m_model.GetRoot())
        {
            for (size_t i = 0; i < m_model.GetOnScreenItems().size(); ++i) {
                const RowEntry* item = m_model.GetOnScreenItems()[i];

                {
                    wxRect buttonRect = item->GetButtonRect();
                    // Adjust the coordiantes incase we got h-scroll
                    buttonRect.SetX(buttonRect.GetX() - GetFirstColumn());
                    if (buttonRect.Contains(point)) {
                        flags = wxTREE_HITTEST_ONITEMBUTTON;
                        // The button is always on column 0
                        column = 0;
                        return wxTreeItemId(const_cast<RowEntry*>(item));
                    }
                }
                if (item->GetItemRect().Contains(point)) {
                    flags = wxTREE_HITTEST_ONITEM;
                    if (!GetHeader()->empty()) {
                        for (size_t col = 0; col < GetHeader()->size(); ++col) {
                            // Check which column was clicked
                            wxRect cellRect = item->GetCellRect(col);
                            // We need to fix the x-axis to reflect any horizontal scrollbar
                            cellRect.SetX(cellRect.GetX() - GetFirstColumn());
                            if (cellRect.Contains(point)) {
                                // Check if click was made on the checkbox ("state icon")
                                wxRect checkboxRect = item->GetCheckboxRect(col);
                                wxRect dropDownRect = item->GetChoiceRect(col);
                                if (!checkboxRect.IsEmpty()) {
                                    // Adjust the coordiantes incase we got h-scroll
                                    checkboxRect.SetX(checkboxRect.GetX() - GetFirstColumn());
                                    if (checkboxRect.Contains(point)) { flags |= wxTREE_HITTEST_ONITEMSTATEICON; }
                                }
                                else if (!dropDownRect.IsEmpty()) {
                                    dropDownRect.SetX(dropDownRect.GetX() - GetFirstColumn());
                                    if (dropDownRect.Contains(point)) { flags |= wxTREE_HITTEST_ONDROPDOWNARROW; }
                                }
                                column = col;
                                break;
                            }
                        }
                        if (column == wxNOT_FOUND) {
                            // assume its the last column (the last column expands on the remainder of the row
                            column = (GetHeader()->size() - 1);
                        }
                    }
                    return wxTreeItemId(const_cast<RowEntry*>(item));
                }
            }
        }
        return wxTreeItemId();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::UnselectAll()
    {
        if (m_model.GetRoot())
        {
            m_model.UnselectAll();
            Refresh();
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::DeleteSelection()
    {
        wxTreeItemId item = m_model.GetSingleSelection();
        CHECK_ITEM_RET(item);
        // delete the item + its children
        // fires event
        m_model.DeleteItem(item);
        UpdateScrollBar();
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    wxPoint TreeCtrl::DoFixPoint(const wxPoint& pt)
    {
        //wxPoint point = pt;
        return pt;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::EnsureVisible(const wxTreeItemId& item)
    {
        if (item.IsOk())
        {
            // Make sure that all parents of ítem are expanded
            if (m_model.ExpandToItem(item))
            {

                // We need to paint to take place now so all the items will be assigned with the updated geometry
                //Update();
                // scroll to the item
                RowEntry* pNode = m_model.ToPtr(item);
                if (IsItemVisible(pNode) && IsItemFullyVisible(pNode)) { return; }
                EnsureItemVisible(pNode, false); // make it visible at the bottom
                UpdateScrollBar();               // Make sure that the scrollbar fits the view
                Refresh();
            }
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::OnMouseLeftDClick(wxMouseEvent& event)
    {
        event.Skip();
        CHECK_ROOT_RET();

        int flags = 0;
        int column = wxNOT_FOUND;

        wxPoint pt = DoFixPoint(event.GetPosition());
        wxTreeItemId where = HitTest(pt, flags, column);
        if(where.IsOk()) {
            SelectItem(where, true);

            // Let sublclasses handle this first
            wxTreeEvent evt(wxEVT_TREE_ITEM_ACTIVATED);
            evt.SetEventObject(this);
            evt.SetItem(where);
            evt.SetInt(column);
            if(GetEventHandler()->ProcessEvent(evt)) { return; }

            // Process the default action
            if(ItemHasChildren(where)) {
                if(IsExpanded(where)) {
                    Collapse(where);
                } else {
                    Expand(where);
                }
            }
        }
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrl::IsExpanded(const wxTreeItemId& item) const
    {
        if (item.GetID())
        {
            RowEntry* child = m_model.ToPtr(item);
            if (child)
                return child->IsExpanded();
        }
        return false;
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrl::ItemHasChildren(const wxTreeItemId& item) const
    {
        if (item.GetID())
        {
            RowEntry* child = m_model.ToPtr(item);
            if (child)
                return child->HasChildren();
        }
        return false;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetIndent(int size)
    {
        ItemPanel::SetIndent(size);
        m_model.SetIndentSize(size);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    int TreeCtrl::GetIndent() const { return m_model.GetIndentSize(); }
    //-------------------------------------------------------------------------------------
    bool TreeCtrl::IsEmpty() const { return m_model.IsEmpty(); }
    //-------------------------------------------------------------------------------------
    size_t TreeCtrl::GetChildrenCount(const wxTreeItemId& item, bool recursively) const
    {
        if (item.GetID())
        {
            RowEntry* node = m_model.ToPtr(item);
            return node->GetChildrenCount(recursively);
        }
        return 0;
    }
    //-------------------------------------------------------------------------------------
    size_t TreeCtrl::GetItemCount(bool recurse) const
    {
        RowEntry* root = m_model.GetRoot();
        if (root)
        {
            return root->GetChildrenCount(recurse);
        }
        return 0;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::DeleteChildren(const wxTreeItemId& item)
    {
        if (item.GetID())
        {
            RowEntry* node = m_model.ToPtr(item);
            node->DeleteAllChildren();
            UpdateScrollBar();
            Refresh();
        }
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetFirstChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const
    {
        if (item.GetID())
        {
            RowEntry* node = m_model.ToPtr(item);
            const RowEntry::Vec_t& children = node->GetChildren();
            if (children.empty())
                return wxTreeItemId(); // No children
            int* pidx = (int*)&cookie;
            int& idx = (*pidx);
            idx = 1; // the next item
            return wxTreeItemId(children[0]);
        }
        return wxTreeItemId();
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetNextChild(const wxTreeItemId& item, wxTreeItemIdValue& cookie) const
    {
        if (item.GetID())
        {
            int* pidx = (int*)&cookie;
            int& idx = (*pidx);
            RowEntry* node = m_model.ToPtr(item);
            const RowEntry::Vec_t& children = node->GetChildren();
            if (idx >= (int)children.size())
                return wxTreeItemId();
            wxTreeItemId child(children[idx]);
            idx++;
            return child;
        }
        return wxTreeItemId();
    }
    //-------------------------------------------------------------------------------------
    const wxString & TreeCtrl::GetItemText(const wxTreeItemId& item, size_t col) const
    {
        if (item.GetID())
        {
            RowEntry* node = m_model.ToPtr(item);
            return node->GetLabel(col);
        }
        return gsgsEmptyString;
    }
    //-------------------------------------------------------------------------------------
    const wxString & TreeCtrl::GetItemTooltip(const wxTreeItemId& item, size_t col) const
    {
        if (item.GetID())
        {
            RowEntry* node = m_model.ToPtr(item);
            return node->GetTooltip(col);
        }
        return gsgsEmptyString;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::DoMouseScroll(const wxMouseEvent& event)
    {
        CHECK_ROOT_RET();
        if(!GetFirstItemOnScreen()) { return; }

        const RowEntry::Vec_t& onScreenItems = m_model.GetOnScreenItems();
        if(onScreenItems.empty()) { return; }
        RowEntry* lastItem = onScreenItems.back();
        RowEntry* firstItem = onScreenItems.front();

        // Can we scroll any further?
        wxTreeItemId nextItem;
        bool scrolling_down = !(event.GetWheelRotation() > 0);
        if(scrolling_down) { // Scrolling up
            nextItem = m_model.GetItemAfter(lastItem, true);
        } else {
            nextItem = m_model.GetItemAfter(firstItem, true);
        }

        if(!nextItem.IsOk() && (scrolling_down && !IsItemFullyVisible(lastItem))) {
            nextItem = wxTreeItemId(lastItem);
        } else if(!nextItem.IsOk()) {
            // No more items to draw
            m_scrollLines = 0;
            return;
        }

        RowEntry::Vec_t items;
        m_scrollLines += event.GetWheelRotation();
        int lines = m_scrollLines / event.GetWheelDelta();
        int remainder = m_scrollLines % event.GetWheelDelta();

        if(lines != 0) {
            m_scrollLines = remainder;
        } else {
            return;
        }
        if(event.GetWheelRotation() > 0) { // Scrolling up
            m_model.GetPrevItems(GetFirstItemOnScreen(), std::abs((double)lines), items, false);
            if(items.empty()) { return; }
            SetFirstItemOnScreen(items.front()); // first item
        } else {
            m_model.GetNextItems(GetFirstItemOnScreen(), std::abs((double)lines), items, false);
            if(items.empty()) { return; }
            SetFirstItemOnScreen(items.back()); // the last item
        }
        UpdateScrollBar();
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::DoBitmapAdded()
    {
        // Also, we adjust the indent size
        if (GetBitmaps())
        {
            int heighestBitmap = 0;
            for (size_t i = 0; i < GetBitmaps()->size(); ++i) {
                const wxBitmap& bmp = GetBitmaps()->at(i);
                if (bmp.IsOk()) { heighestBitmap = wxMax(heighestBitmap, bmp.GetScaledHeight()); }
            }
            heighestBitmap += 2 * RowEntry::Y_SPACER;
            SetLineHeight(wxMax(heighestBitmap, GetLineHeight()));
            SetIndent(GetLineHeight());
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetBitmaps(BitmapList* bitmaps)
    {
        ItemPanel::SetBitmaps(bitmaps);
        DoBitmapAdded();
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::ProcessIdle()
    {
        if(HasStyle(wxTR_FULL_ROW_HIGHLIGHT)) {
            CHECK_ROOT_RET();
            int flags = 0;
            wxPoint pt = ScreenToClient(::wxGetMousePosition());
            int column = wxNOT_FOUND;
            wxTreeItemId item = HitTest(pt, flags, column);
            if(item.IsOk()) {
                RowEntry::Vec_t& items = m_model.GetOnScreenItems();
                RowEntry* hoveredNode = m_model.ToPtr(item);
                bool refreshNeeded = false;
                for(size_t i = 0; i < items.size(); ++i) {
                    bool new_state = hoveredNode == items[i];
                    bool old_state = items[i]->IsHovered();
                    if(!refreshNeeded) { refreshNeeded = (new_state != old_state); }
                    items[i]->SetHovered(hoveredNode == items[i]);
                }
                if(refreshNeeded) { Refresh(); }
            }
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::OnLeaveWindow(wxMouseEvent& event)
    {
        event.Skip();
        CHECK_ROOT_RET();
        RowEntry::Vec_t& items = m_model.GetOnScreenItems();
        for(size_t i = 0; i < items.size(); ++i) {
            items[i]->SetHovered(false);
        }
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::ExpandAllChildren(const wxTreeItemId& item)
    {
        wxBusyCursor bc;
        m_model.ExpandAllChildren(item);
        UpdateScrollBar();
        DoUpdateHeader(item);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::CollapseAllChildren(const wxTreeItemId& item)
    {
        wxBusyCursor bc;
        m_model.CollapseAllChildren(item);
        SetFirstItemOnScreen(m_model.ToPtr(item));
        SelectItem(item);
        UpdateScrollBar();
        DoUpdateHeader(item);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetFirstVisibleItem() const
    {
        const RowEntry::Vec_t& items = m_model.GetOnScreenItems();
        if(items.empty()) { return wxTreeItemId(); }
        return wxTreeItemId(items[0]);
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetNextVisible(const wxTreeItemId& item) const { return DoGetSiblingVisibleItem(item, true); }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetPrevVisible(const wxTreeItemId& item) const { return DoGetSiblingVisibleItem(item, false); }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::DoGetSiblingVisibleItem(const wxTreeItemId& item, bool next) const
    {
        const RowEntry::Vec_t& items = m_model.GetOnScreenItems();
        if (item.IsOk() && items.size())
        {
            RowEntry* from = m_model.ToPtr(item);
            RowEntry::Vec_t::const_iterator iter =
                std::find_if(items.begin(), items.end(), [&](RowEntry* p) { return p == from; });
            if (next && (iter == items.end())) { return wxTreeItemId(); }
            if (!next && (iter == items.begin())) { return wxTreeItemId(); }
            if (next) {
                ++iter;
                if (iter == items.end()) { return wxTreeItemId(); }
            }
            else {
                --iter;
                if (iter == items.begin()) { return wxTreeItemId(); }
            }
            return wxTreeItemId(*iter);
        }
        return wxTreeItemId();
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetSelection() const 
    { 
        return m_model.GetSingleSelection(); 
    }
    //-------------------------------------------------------------------------------------
    const wxString & TreeCtrl::GetSelectionText(size_t col) const
    {
        return GetItemText(m_model.GetSingleSelection(), col);
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetFocusedItem() const 
    { 
        return GetSelection(); 
    }
    //-------------------------------------------------------------------------------------
    size_t TreeCtrl::GetSelections(wxArrayTreeItemIds& selections) const
    {
        const RowEntry::Vec_t& items = m_model.GetSelections();
        if (items.size())
        {
            std::for_each(items.begin(), items.end(), [&](RowEntry* item) { selections.Add(wxTreeItemId(item)); });
            return selections.size();
        }
        return 0;
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrl::DoKeyDown(const wxKeyEvent& event)
    {
        // Let the user chance to process this first
        wxTreeEvent evt(wxEVT_TREE_KEY_DOWN);
        evt.SetEventObject(this);
        evt.SetKeyEvent(event);
        evt.SetItem(GetSelection()); // can be an invalid item
        if(GetEventHandler()->ProcessEvent(evt)) { return true; }

        // Let the parent process this
        if(ItemPanel::DoKeyDown(event)) { return false; }

        if(!m_model.GetRoot()) {
            // we didnt process this event, carry on
            return true;
        }
        wxTreeItemId selectedItem = GetSelection();
        if(!selectedItem.IsOk()) { return true; }

        if(event.GetKeyCode() == WXK_LEFT) {
            if(m_model.ToPtr(selectedItem)->IsExpanded()) {
                Collapse(selectedItem);
                return true;
            }
        } else if(event.GetKeyCode() == WXK_RIGHT) {
            if(!m_model.ToPtr(selectedItem)->IsExpanded()) {
                Expand(selectedItem);
                return true;
            }
        } else if(event.GetKeyCode() == WXK_RETURN || event.GetKeyCode() == WXK_NUMPAD_ENTER) {
            wxTreeEvent evt(wxEVT_TREE_ITEM_ACTIVATED);
            evt.SetEventObject(this);
            evt.SetItem(selectedItem);
            GetEventHandler()->ProcessEvent(evt);
            return true;
        }
        // We didnt process this event, continue on processing it
        return false;
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrl::IsItemVisible(RowEntry* item) const
    {
        const RowEntry::Vec_t& onScreenItems = m_model.GetOnScreenItems();
        RowEntry::Vec_t::const_iterator iter =
            std::find_if(onScreenItems.begin(), onScreenItems.end(), [&](RowEntry* p) { return p == item; });
        return (iter != onScreenItems.end());
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrl::IsItemFullyVisible(RowEntry* item) const
    {
        const wxRect& itemRect = item->GetItemRect();
        const wxRect clientRect = GetItemsRect();
        return clientRect.Contains(itemRect);
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::EnsureItemVisible(RowEntry* item, bool fromTop)
    {
        CHECK_PTR_RET(item)
        if(m_model.GetOnScreenItems().empty()) {
            // requesting to ensure item visibility before we drawn anyting on the screen
            // to reduce any strange behaviours (e.g. 1/2 screen is displayed while we can display more items)
            // turn the m_maxList flag to ON
            m_maxList = true;
        }

        if(IsItemFullyVisible(item)) { return; }
        if(fromTop) {
            SetFirstItemOnScreen(item);
        } else {
            //int max_lines_on_screen = GetNumLineCanFitOnScreen();
            RowEntry::Vec_t items;
            m_model.GetPrevItems(item, GetNumLineCanFitOnScreen(), items);
            if(items.empty()) { return; }
            //if(!IsItemFullyVisible(item) && (items.size() > 1)) { items.erase(items.begin()); }
            SetFirstItemOnScreen(items[0]);
        }
    }
    //-------------------------------------------------------------------------------------
    size_t TreeCtrl::GetExpandCount() const
    {
        RowEntry* root = m_model.GetRoot();
        if (root)
        {
            if (root->IsHidden())
                return root->GetExpandedChildrenCount();
            else
                return root->GetExpandedChildrenCount() + 1;
        }
        return 0;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::Delete(const wxTreeItemId& item)
    {
        CHECK_ITEM_RET(item);
        // delete the item + its children
        // fires event
        m_model.DeleteItem(item);
        UpdateScrollBar();
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetItemData(const wxTreeItemId& item, wxTreeItemData* data)
    {
        RowEntry * node = m_model.ToPtr(item);
        CHECK_PTR_RET(node);
        node->SetClientObject(data);
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemData* TreeCtrl::GetItemData(const wxTreeItemId& item) const
    {
        if (item.GetID())
        {
            RowEntry* node = m_model.ToPtr(item);
            return node->GetClientObject();
        }
        return nullptr;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetItemIntData(const wxTreeItemId& item, wxIntPtr data)
    {
        RowEntry * node = m_model.ToPtr(item);
        CHECK_PTR_RET(node);
        node->SetData(data);
    }
    //-------------------------------------------------------------------------------------
    wxIntPtr TreeCtrl::GetItemIntData(const wxTreeItemId& item) const
    {
        if (item.GetID())
        {
            RowEntry* node = m_model.ToPtr(item);
            return node->GetData();
        }
        return -1;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetItemBackgroundColour(const wxTreeItemId& item, const wxColour& colour, size_t col)
    {
        RowEntry* node = m_model.ToPtr(item);
        CHECK_PTR_RET(node);
        node->SetBgColour(colour, col);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    wxColour TreeCtrl::GetItemBackgroundColour(const wxTreeItemId& item, size_t col) const
    {
        RowEntry* node = m_model.ToPtr(item);
        if(!node) { return wxNullColour; }
        return node->GetBgColour(col);
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetItemTextColour(const wxTreeItemId& item, const wxColour& colour, size_t col)
    {
        RowEntry* node = m_model.ToPtr(item);
        CHECK_PTR_RET(node);
        node->SetTextColour(colour, col);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    wxColour TreeCtrl::GetItemTextColour(const wxTreeItemId& item, size_t col) const
    {
        RowEntry* node = m_model.ToPtr(item);
        if(!node) { return wxNullColour; }
        return node->GetTextColour(col);
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetItemText(const wxTreeItemId& item, const wxString& text, size_t col)
    {
        RowEntry* node = m_model.ToPtr(item);
        CHECK_PTR_RET(node);
        node->SetLabel(text, col);
        DoUpdateHeader(item);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetItemTooltip(const wxTreeItemId& item, const wxString& text, size_t col)
    {
        RowEntry* node = m_model.ToPtr(item);
        CHECK_PTR_RET(node);
        node->SetTooltip(text, col);
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetItemBold(const wxTreeItemId& item, bool bold, size_t col)
    {
        RowEntry* node = m_model.ToPtr(item);
        CHECK_PTR_RET(node);
        wxFont f = node->GetFont(col);
        if(!f.IsOk()) { f = GetDefaultFont(); }
        f.SetWeight(bold ? wxFONTWEIGHT_BOLD : wxFONTWEIGHT_NORMAL);
        node->SetFont(f, col);

        // Changing font can change the width of the text, so update the header if needed
        DoUpdateHeader(item);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetItemFont(const wxTreeItemId& item, const wxFont& font, size_t col)
    {
        RowEntry* node = m_model.ToPtr(item);
        CHECK_PTR_RET(node);
        node->SetFont(font, col);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    wxFont TreeCtrl::GetItemFont(const wxTreeItemId& item, size_t col) const
    {
        RowEntry* node = m_model.ToPtr(item);
        if(!node) { return wxNullFont; }
        return node->GetFont(col);
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::OnContextMenu(wxContextMenuEvent& event)
    {
        event.Skip();
        CHECK_ROOT_RET();
        int flags = 0;
        int column = wxNOT_FOUND;
        wxPoint pt = ScreenToClient(::wxGetMousePosition());
        wxTreeItemId item = HitTest(pt, flags, column);
        if(item.IsOk()) {
            SelectItem(item, true);
            wxTreeEvent evt(wxEVT_TREE_ITEM_MENU);
            evt.SetItem(item);
            evt.SetInt(column); // pass the column
            evt.SetEventObject(this);
            GetEventHandler()->ProcessEvent(evt);
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::OnRightDown(wxMouseEvent& event)
    {
        event.Skip();
        CHECK_ROOT_RET();
        int flags = 0;
        int column = wxNOT_FOUND;
        wxPoint pt = DoFixPoint(event.GetPosition());
        wxTreeItemId where = HitTest(pt, flags, column);
        if(where.IsOk()) {
            wxTreeEvent evt(wxEVT_TREE_ITEM_RIGHT_CLICK);
            evt.SetEventObject(this);
            evt.SetItem(where);
            evt.SetInt(column);
            event.Skip(false);
            if(GetEventHandler()->ProcessEvent(evt)) { return; }
            event.Skip();
        }
    }
    //-------------------------------------------------------------------------------------
    RowEntry* TreeCtrl::GetFirstItemOnScreen() 
    { 
        return m_model.GetFirstItemOnScreen(); 
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetFirstItemOnScreen(RowEntry* item) 
    { 
        m_model.SetFirstItemOnScreen(item); 
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetSortFunction(const TreeSortFunc& CompareFunc) 
    { 
        m_model.SetSortFunction(CompareFunc); 
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::ScrollToRow(int firstLine)
    {
        RowEntry* newTopLine = nullptr;
        // Thumbtrack sends the top-line index in event.GetPosition()
        newTopLine = m_model.GetItemFromIndex(firstLine);
        if(newTopLine) {
            if(newTopLine->IsHidden()) { newTopLine = newTopLine->GetFirstChild(); }
            SetFirstItemOnScreen(newTopLine);
            if(!GetVScrollBar()->CanScollDown()) {
                // we cant scroll down anymore
                // Get list of items to draw
                RowEntry::Vec_t items;
                size_t maxItems = GetNumLineCanFitOnScreen();
                m_model.GetNextItems(newTopLine, maxItems, items);
                AssignRects(items);
                if(!items.empty() && !IsItemFullyVisible(items.back()) && (firstLine != 0)) {
                    newTopLine = m_model.GetRowAfter(newTopLine, true);
                    if(newTopLine) { SetFirstItemOnScreen(newTopLine); }
                }
            }
        }
    #if USE_NATIVE_SCROLLBAR
        UpdateScrollBar();
    #endif
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::ScrollRows(int steps, wxDirection direction)
    {
        // Process the rest of the scrolling events here
        wxTreeItemId nextSelection;
        bool fromTop = false;
        if(steps == 0) {
            // Top or Bottom
            if(direction == wxUP) {
                if(IsRootHide()) {
                    nextSelection = wxTreeItemId(m_model.ToPtr(GetRootItem())->GetFirstChild());
                } else {
                    nextSelection = GetRootItem();
                }
                fromTop = true;
            } else {
                // Find the last item, it does not matter if the root is hidden
                RowEntry* node = m_model.ToPtr(GetRootItem());
                while(node->GetLastChild()) {
                    node = node->GetLastChild();
                }
                nextSelection = wxTreeItemId(node);
            }
        } else {
            nextSelection = DoScrollLines(steps, direction == wxUP, GetFocusedItem(), false);
            fromTop = (direction == wxUP);
        }

        if(::wxGetKeyState(WXK_SHIFT) && HasStyle(wxTR_MULTIPLE)) {
            m_model.AddSelection(nextSelection);
        } else {
            SelectItem(nextSelection);
        }
        EnsureItemVisible(m_model.ToPtr(nextSelection), fromTop);
        UpdateScrollBar();
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SelectChildren(const wxTreeItemId& item)
    {
        CHECK_ITEM_RET(item);
        if(!(GetTreeStyle() & wxTR_MULTIPLE)) {
            // Can only be used with multiple selection trees
            return;
        }
        m_model.SelectChildren(item);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetNextSibling(const wxTreeItemId& item) const
    {
        CHECK_ITEM_RET_INVALID_ITEM(item);
        return m_model.GetNextSibling(m_model.ToPtr(item));
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetPrevSibling(const wxTreeItemId& item) const
    {
        CHECK_ITEM_RET_INVALID_ITEM(item);
        return m_model.GetPrevSibling(m_model.ToPtr(item));
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::DoScrollLines(int numLines, bool up, wxTreeItemId from, bool selectIt)
    {
        wxTreeItemId selectedItem = from;
        if (selectedItem.IsOk())
        {
            int counter = 0;
            wxTreeItemId nextItem = selectedItem;
            while (nextItem.IsOk() && (counter < numLines)) {
                if (up) {
                    nextItem = m_model.GetItemBefore(selectedItem, true);
                }
                else {
                    nextItem = m_model.GetItemAfter(selectedItem, true);
                }
                if (nextItem.IsOk()) { selectedItem = nextItem; }
                counter++;
            }
            if (selectIt) { SelectItem(selectedItem); }
            return selectedItem;
        }
        return wxTreeItemId();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::EnableStyle(int style, bool enable, bool refresh)
    {
        if(enable) {
            m_treeStyle |= style;
        } else {
            m_treeStyle &= ~style;
        }

        if(style == wxTR_ENABLE_SEARCH) { GetSearch().SetEnabled(enable); }

        // From this point on, we require a root item
        if(!m_model.GetRoot()) { return; }

        // When changing the wxTR_HIDE_ROOT style
        // we need to fix the indentation for each node in the tree
        if(style == wxTR_HIDE_ROOT) {
            m_model.GetRoot()->SetHidden(IsRootHide());
            std::function<bool(RowEntry*, bool)> UpdateIndentsFunc = [=](RowEntry* node, bool) {
                //wxUnusedVar(visibleItem);
                if(node->GetParent()) { node->SetIndentsCount(node->GetParent()->GetIndentsCount() + 1); }
                return true;
            };
            TreeNodeVisitor V;
            V.Visit(m_model.GetRoot(), false, UpdateIndentsFunc);
            wxTreeItemId newRoot(m_model.GetRoot()->GetFirstChild());
            if(newRoot) { DoUpdateHeader(newRoot); }
        }
        if(refresh) 
        { 
            Refresh(); 
        }
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetItemParent(const wxTreeItemId & item) const
    {
        CHECK_ITEM_RET_INVALID_ITEM(item);
        return wxTreeItemId(m_model.ToPtr(item)->GetParent());
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetItemImage(const wxTreeItemId & item, int imageId, int openImageId, size_t col)
    {
        RowEntry * node = m_model.ToPtr(item);
        CHECK_PTR_RET(node);
        node->SetBitmapIndex(imageId, col);
        node->SetBitmapSelectedIndex(openImageId, col);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    int TreeCtrl::GetItemImage(const wxTreeItemId & item, bool selectedImage, size_t col) const
    {
        if (item.GetID())
        {
            RowEntry * node = m_model.ToPtr(item);
            return selectedImage ? node->GetBitmapSelectedIndex(col) : node->GetBitmapIndex(col);
        }
        return wxNOT_FOUND;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::OnEnterWindow(wxMouseEvent& event) { event.Skip(); }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::DoUpdateHeader(const wxTreeItemId& item) { ItemPanel::DoUpdateHeader(m_model.ToPtr(item)); }
    //-------------------------------------------------------------------------------------
    bool TreeCtrl::IsVisible(const wxTreeItemId& item) const { return m_model.IsVisible(item); }
    //-------------------------------------------------------------------------------------
    bool TreeCtrl::IsSelected(const wxTreeItemId& item) const { return m_model.IsItemSelected(item); }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetRow(const wxPoint& pt) const
    {
        int flags = 0;
        int column = wxNOT_FOUND;
        wxTreeItemId item = HitTest(pt, flags, column);
        if(item.IsOk() && (flags & wxTREE_HITTEST_ONITEM)) { return item; }
        return wxTreeItemId();
    }
    //-------------------------------------------------------------------------------------
    int TreeCtrl::GetFirstItemPosition() const 
    { 
        return m_model.GetItemIndex(m_model.GetFirstItemOnScreen()); 
    }
    //-------------------------------------------------------------------------------------
    int TreeCtrl::GetRange() const 
    { 
        //return m_model.GetExpandedLines();
        return GetExpandCount();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::DeleteAllItems()
    {
        m_model.EnableEvents(false);
        Delete(GetRootItem());
        m_model.EnableEvents(true);
        DoUpdateHeader(nullptr);
        m_scrollLines = 0;
        SetFirstColumn(0);
        UpdateScrollBar();
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetNextItem(const wxTreeItemId& item) const { return m_model.GetItemAfter(item, true); }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::GetPrevItem(const wxTreeItemId& item) const { return m_model.GetItemBefore(item, true); }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::FindNext(const wxTreeItemId& from, const wxString& what, size_t col, size_t searchFlags)
    {
        return wxTreeItemId(DoFind(m_model.ToPtr(from), what, col, searchFlags, true));
    }
    //-------------------------------------------------------------------------------------
    wxTreeItemId TreeCtrl::FindPrev(const wxTreeItemId& from, const wxString& what, size_t col, size_t searchFlags)
    {
        return wxTreeItemId(DoFind(m_model.ToPtr(from), what, col, searchFlags, false));
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::HighlightText(const wxTreeItemId& item, bool b)
    {
        if (item.IsOk())
        {
            m_model.ToPtr(item)->SetHighlight(b);
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::ClearHighlight(const wxTreeItemId& item)
    {
        if (item.IsOk())
        {
            RowEntry* row = m_model.ToPtr(item);
            row->SetHighlight(false);
            row->SetHighlightInfo({});
            Refresh();
        }
    }
    //-------------------------------------------------------------------------------------
    RowEntry* TreeCtrl::DoFind(RowEntry* from, const wxString& what, size_t col, size_t searchFlags, bool next)
    {
        RowEntry* curp = nullptr;
        if(from) {
            if(searchFlags & wxTR_SEARCH_INCLUDE_CURRENT_ITEM) {
                curp = from;
            } else {
                curp = next ? m_model.GetRowAfter(m_model.ToPtr(from), searchFlags & wxTR_SEARCH_VISIBLE_ITEMS)
                            : m_model.GetRowBefore(m_model.ToPtr(from), searchFlags & wxTR_SEARCH_VISIBLE_ITEMS);
            }
        }
        else
        {
            curp = m_model.GetRoot();
        }
        while(curp) {
            const wxString& haystack = curp->GetLabel(col);
            MatchResult res;
            if(SearchText::Matches(what, col, haystack, searchFlags, &res)) {
                curp->SetHighlightInfo(res);
                curp->SetHighlight(true);
                return curp;
            }
            curp = next ? m_model.GetRowAfter(curp, searchFlags & wxTR_SEARCH_VISIBLE_ITEMS)
                        : m_model.GetRowBefore(curp, searchFlags & wxTR_SEARCH_VISIBLE_ITEMS);
        }
        return nullptr;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::ClearAllHighlights()
    {
        TreeNodeVisitor V;
        std::function<bool(RowEntry*, bool)> Foo = [&](RowEntry* r, bool visible) {
            wxUnusedVar(visible);
            r->SetHighlightInfo({});
            r->SetHighlight(false);
            return true;
        };
        V.Visit(m_model.GetRoot(), false, Foo);
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::UpdateScrollBar()
    {
        ItemPanel::UpdateScrollBar();
        m_scrollLines = 0;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::DoAddHeader(const wxString& label, const wxBitmap& bmp, int width)
    {
        wxUnusedVar(bmp);
        if(m_needToClearDefaultHeader) {
            m_needToClearDefaultHeader = false;
            GetHeader()->Clear();
        }
        HeaderItem& col = GetHeader()->Add(label);
        if(width > 0) { col.SetWidthValue(width); }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::Check(const wxTreeItemId& item, bool check, size_t col)
    {
        RowEntry* row = m_model.ToPtr(item);
        if (row)
        {
            row->SetChecked(check, row->GetBitmapIndex(col), row->GetLabel(col), col);

            // Fire value changed event
            wxTreeEvent evt(wxEVT_TREE_ITEM_VALUE_CHANGED);
            evt.SetInt(col);
            evt.SetEventObject(this);
            evt.SetItem(item);
            GetEventHandler()->ProcessEvent(evt);
            Refresh();
        }
    }
    //-------------------------------------------------------------------------------------
    bool TreeCtrl::IsChecked(const wxTreeItemId& item, size_t col) const
    {
        RowEntry* row = m_model.ToPtr(item);
        if (row)
        {
            return row->IsChecked(col);
        }
        return false;
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrl::SetImageList(wxImageList* images)
    {
        ItemPanel::SetImageList(images);
        DoBitmapAdded();
        Refresh();
    }
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // TreeCtrlColourHelper
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    TreeCtrlColourHelper::TreeCtrlColourHelper(DefaultTreeCtrl* tree) : m_tree(tree){}
    //-------------------------------------------------------------------------------------
    TreeCtrlColourHelper::~TreeCtrlColourHelper() {}
    //-------------------------------------------------------------------------------------
    void TreeCtrlColourHelper::DoSetBgColour(const wxTreeItemId & item, const wxColour & currentBgColour, const FolderColour::Map_t & coloursMap) const
    {
        CHECK_ITEM_RET(item);
        wxString path = GetItemPath(item);
        wxColour bgColour = currentBgColour;
        FolderColour::Map_t::const_iterator iter = coloursMap.find(path);
        if(iter != coloursMap.end()) { bgColour = iter->second.GetColour(); }
        
        // It's OK for the colour to be "invalid", it will get reset to the tree's default
        // colouring
        m_tree->SetItemBackgroundColour(item, bgColour);

        if(m_tree->ItemHasChildren(item)) {
            wxTreeItemIdValue cookie;
            wxTreeItemId child = m_tree->GetFirstChild(item, cookie);
            while(child.IsOk()) {
                DoSetBgColour(child, bgColour, coloursMap);
                child = m_tree->GetNextChild(item, cookie);
            }
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlColourHelper::DoClearBgColour(const wxTreeItemId & item, const wxColour& colourToSet, FolderColour::Map_t & coloursMap) const
    {
        CHECK_ITEM_RET(item);
        wxString path = GetItemPath(item);
        FolderColour::Map_t::const_iterator iter = coloursMap.find(path);
        if(iter != coloursMap.end()) { coloursMap.erase(iter); }
        m_tree->SetItemBackgroundColour(item, colourToSet);
        if(m_tree->ItemHasChildren(item)) {
            wxTreeItemIdValue cookie;
            wxTreeItemId child = m_tree->GetFirstChild(item, cookie);
            while(child.IsOk()) {
                DoClearBgColour(child, colourToSet, coloursMap);
                child = m_tree->GetNextChild(item, cookie);
            }
        }
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlColourHelper::ResetBgColour(const wxTreeItemId & item, FolderColour::Map_t & coloursMap) const
    {
        CHECK_ITEM_RET(item);
        wxTreeItemId itemParent = m_tree->GetItemParent(item);
        wxColour col = (itemParent.IsOk() && m_tree->GetItemBackgroundColour(itemParent).IsOk())
            ? m_tree->GetItemBackgroundColour(itemParent) : wxNullColour;

        DoClearBgColour(item, col, coloursMap);
    }
    //-------------------------------------------------------------------------------------
    void TreeCtrlColourHelper::SetBgColour(const wxTreeItemId & item, const wxColour& currentBgColour, FolderColour::Map_t & coloursMap) const
    {
        wxString path = GetItemPath(item);
        FolderColour vdc(path, currentBgColour);
        if(coloursMap.count(path)) 
        { 
            coloursMap.erase(path); 
        }
        coloursMap.insert(std::make_pair(path, vdc));
        DoSetBgColour(item, currentBgColour, coloursMap);
    }
    //-------------------------------------------------------------------------------------
    wxString TreeCtrlColourHelper::GetItemPath(DefaultTreeCtrl * tree, const wxTreeItemId & item)
    {
        std::deque<wxString> queue;
        wxString text = tree->GetItemText(item);
        queue.push_front(text);

        wxTreeItemId p = tree->GetItemParent(item);
        while(p.IsOk()) {
            text = tree->GetItemText(p);
            queue.push_front(text);
            p = tree->GetItemParent(p);
        }

        wxString path;
        while(!queue.empty()) {
            path << queue.front() << "/";
            queue.pop_front();
        }

        if(!path.IsEmpty()) { path.RemoveLast(); }
        return path;
    }
    //-------------------------------------------------------------------------------------
    wxString TreeCtrlColourHelper::GetItemPath(const wxTreeItemId& item) const 
    { 
        return GetItemPath(m_tree, item); 
    }
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // DefaultTreeCtrl
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    DefaultTreeCtrl::DefaultTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : TreeCtrl(parent, id, pos, size, style /*| TREE_STYLE*/)
    {
        gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &DefaultTreeCtrl::OnTheme, this);
        ApplyTheme();
        m_keyboard.reset(new TreeKeyboardInput(this));
    }
    //-------------------------------------------------------------------------------------
    bool DefaultTreeCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    {
        if (TreeCtrl::Create(parent, id, pos, size, style /*| TREE_STYLE*/))
        {
            gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &DefaultTreeCtrl::OnTheme, this);
            ApplyTheme();
            m_keyboard.reset(new TreeKeyboardInput(this));
            return true;
        }
        return false;
    }
    //-------------------------------------------------------------------------------------
    DefaultTreeCtrl::DefaultTreeCtrl() {}
    //-------------------------------------------------------------------------------------
    DefaultTreeCtrl::~DefaultTreeCtrl()
    {
        m_keyboard.reset(nullptr);
        gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &DefaultTreeCtrl::OnTheme, this);
    }
    //-------------------------------------------------------------------------------------
    void DefaultTreeCtrl::OnTheme(wxCommandEvent& event)
    {
        event.Skip();
        ApplyTheme();
    }
    //-------------------------------------------------------------------------------------
    void DefaultTreeCtrl::ApplyTheme()
    {
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
        gsgs::Colour colours;
        if(lexer->IsDark()) 
        {
            colours.setBase(gsgs_Style().GetAppBgColour());
        } 
        else 
        {
            colours.setDefault();
        }
        
        // Use alternate line colours for light trees only
        //EnableStyle(wxTR_ROW_LINES, !gsgs::Colour::IsDark(baseColour));
        
        // Set the built-in search colours
        //wxColour highlightColur = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT);
        //wxColour textColour = wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
        //colours.SetMatchedItemBgText(highlightColur);
        //colours.SetMatchedItemText(textColour);

        colours.SetMatchedItemBgText(gsgs_Style().GetAppActiveTextColour());
        colours.SetMatchedItemText(gsgs_Style().GetAppActiveTextColour());
        colours.SetButtonColour(gsgs_Style().GetActiveMarkerColour());
        colours.SetSelbuttonColour(gsgs_Style().GetHoverMarkerColour());

        // When not using custom colours, use system defaults
        if(!gsgs_Style().IsCustomColour()) {
            //colours.SetSelItemBgColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
            colours.SetSelItemBgColour(gsgs_Style().GetActiveMarkerColour());
    #ifdef __WXOSX__
            colours.SetSelItemBgColourNoFocus(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
    #else
            colours.SetSelItemBgColourNoFocus(colours.GetSelItemBgColour().ChangeLightness(110));
    #endif
        }
        // When using custom bg colour, don't use native drawings
        this->SetColours(colours);
        this->SetNativeTheme(false);
    }
    //-------------------------------------------------------------------------------------
    wxIMPLEMENT_DYNAMIC_CLASS(DataViewTextBitmap,           wxObject);
    IMPLEMENT_VARIANT_OBJECT_EXPORTED(DataViewTextBitmap,   _gsgsAPI);

    wxIMPLEMENT_DYNAMIC_CLASS(DataViewCheckbox,             wxObject);
    IMPLEMENT_VARIANT_OBJECT_EXPORTED(DataViewCheckbox,     _gsgsAPI);

    wxIMPLEMENT_DYNAMIC_CLASS(DataViewChoice,               wxObject);
    IMPLEMENT_VARIANT_OBJECT_EXPORTED(DataViewChoice,       _gsgsAPI);

    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // DataViewListCtrl
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    std::unordered_map<int, int> DataViewListCtrl::m_stylesMap;
    //-------------------------------------------------------------------------------------
    DataViewListCtrl::DataViewListCtrl() 
    {
    }
    //-------------------------------------------------------------------------------------
    DataViewListCtrl::DataViewListCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, 
        const wxSize& size, long style) :
            TreeCtrl(parent, id, pos, size, style)
    {
        DoInitialize(style);
    }
    //-------------------------------------------------------------------------------------
    DataViewListCtrl::~DataViewListCtrl()
    {
        Unbind(wxEVT_TREE_ITEM_EXPANDING, [](wxTreeEvent&) {});
        Unbind(wxEVT_TREE_ITEM_EXPANDED, [](wxTreeEvent&) {});
        Unbind(wxEVT_TREE_ITEM_COLLAPSING, [](wxTreeEvent&) {});
        Unbind(wxEVT_TREE_ITEM_COLLAPSED, [](wxTreeEvent&) {});
        Unbind(wxEVT_TREE_DELETE_ITEM, [](wxTreeEvent&) {});
        Unbind(wxEVT_TREE_ITEM_RIGHT_CLICK, [](wxTreeEvent& e) { e.Skip(); });

        Unbind(wxEVT_TREE_BEGIN_DRAG, &DataViewListCtrl::OnConvertEvent, this);
        Unbind(wxEVT_TREE_CHOICE, &DataViewListCtrl::OnConvertEvent, this);
        Unbind(wxEVT_TREE_END_DRAG, &DataViewListCtrl::OnConvertEvent, this);
        Unbind(wxEVT_TREE_SEL_CHANGED, &DataViewListCtrl::OnConvertEvent, this);
        Unbind(wxEVT_TREE_ITEM_ACTIVATED, &DataViewListCtrl::OnConvertEvent, this);
        Unbind(wxEVT_TREE_ITEM_MENU, &DataViewListCtrl::OnConvertEvent, this);
        Unbind(wxEVT_TREE_ITEM_VALUE_CHANGED, &DataViewListCtrl::OnConvertEvent, this);
    }
    //-------------------------------------------------------------------------------------
    bool DataViewListCtrl::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    {
        if (TreeCtrl::Create(parent, id, pos, size, style))
        {
            DoInitialize(style);
            return true;
        }
        return false;
    }
    //-------------------------------------------------------------------------------------
    /*int DataViewListCtrl::GetRange() const 
    { 
        return GetExpandCount(); 
    }*/
    //-------------------------------------------------------------------------------------
    wxDataViewItem DataViewListCtrl::AppendItem(const wxVector<wxVariant>& values, wxUIntPtr data)
    {
        wxTreeItemId item = TreeCtrl::AppendItem(GetRootItem(), "", -1, -1, nullptr);
        RowEntry* child = m_model.ToPtr(item);
        // mark this row as a "list-view" row (i.e. it can't have children)
        child->SetListItem(false);
        wxDataViewItem dvItem = DV_ITEM(item);
        child->SetData(data);

        for (size_t i = 0; i < values.size(); ++i) {
            const wxVariant& v = values[i];
            DoSetCellValue(child, i, v);
        }
        UpdateScrollBar();
        return dvItem;
    }
    //-------------------------------------------------------------------------------------
    wxDataViewItem DataViewListCtrl::AppendItem(const wxDataViewItem & pid, const wxVector<wxVariant>& values, wxUIntPtr data)
    {
        wxTreeItemId item = TreeCtrl::AppendItem(TREE_ITEM(pid), "", -1, -1, nullptr);
        RowEntry* child = m_model.ToPtr(item);
        // mark this row as a "list-view" row (i.e. it can't have children)
        child->SetListItem(false);
        wxDataViewItem dvItem = DV_ITEM(item);
        child->SetData(data);
        for (size_t i = 0; i < values.size(); ++i) {
            const wxVariant& v = values[i];
            DoSetCellValue(child, i, v);
        }
        UpdateScrollBar();
        return dvItem;
    }
    //-------------------------------------------------------------------------------------
    wxDataViewColumn* DataViewListCtrl::AppendIconTextColumn(const wxString& label, wxDataViewCellMode mode, int width,
        wxAlignment align, int flags)
    {
        wxUnusedVar(mode);
        wxUnusedVar(align);
        wxUnusedVar(flags);
        AddHeader(label, wxNullBitmap, width);
        return nullptr;
    }
    //-------------------------------------------------------------------------------------
    wxDataViewColumn* DataViewListCtrl::AppendProgressColumn(const wxString& label, wxDataViewCellMode mode, int width,
        wxAlignment align, int flags)
    {
        wxUnusedVar(mode);
        wxUnusedVar(align);
        wxUnusedVar(flags);
        AddHeader(label, wxNullBitmap, width);
        return nullptr;
    }
    //-------------------------------------------------------------------------------------
    wxDataViewColumn* DataViewListCtrl::AppendTextColumn(const wxString& label, wxDataViewCellMode mode, int width,
        wxAlignment align, int flags)
    {
        wxUnusedVar(mode);
        wxUnusedVar(align);
        wxUnusedVar(flags);
        AddHeader(label, wxNullBitmap, width);
        return nullptr;
    }
    //-------------------------------------------------------------------------------------
    int DataViewListCtrl::GetSelections(wxDataViewItemArray & sel) const
    {
        wxArrayTreeItemIds items;
        TreeCtrl::GetSelections(items);
        for (size_t i = 0; i < items.size(); ++i)
        {
            sel.Add(DV_ITEM(items[i]));
        }
        return sel.size();
    }
    //-------------------------------------------------------------------------------------
    int DataViewListCtrl::GetSelectedItemsCount() const
    {
        return m_model.GetSelectionsCount();
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::OnConvertEvent(wxTreeEvent& event)
    {
        wxEventType type = wxEVT_ANY;
        wxString eventText;

        if (event.GetEventType() == wxEVT_TREE_BEGIN_DRAG) {
            type = wxEVT_DATAVIEW_ITEM_BEGIN_DRAG;
        }
        else if (event.GetEventType() == wxEVT_TREE_END_DRAG) {
            type = wxEVT_DATAVIEW_ITEM_DROP;
        }
        else if (event.GetEventType() == wxEVT_TREE_SEL_CHANGED) {
            type = wxEVT_DATAVIEW_SELECTION_CHANGED;
        }
        else if (event.GetEventType() == wxEVT_TREE_ITEM_ACTIVATED) {
            type = wxEVT_DATAVIEW_ITEM_ACTIVATED;
        }
        else if (event.GetEventType() == wxEVT_TREE_ITEM_MENU) {
            type = wxEVT_DATAVIEW_ITEM_CONTEXT_MENU;
        }
        else if (event.GetEventType() == wxEVT_TREE_SEARCH_TEXT) {
            type = wxEVT_DATAVIEW_SEARCH_TEXT;
            eventText = event.GetString();
        }
        else if (event.GetEventType() == wxEVT_TREE_CLEAR_SEARCH) {
            type = wxEVT_DATAVIEW_CLEAR_SEARCH;
        }
        else if (event.GetEventType() == wxEVT_TREE_ITEM_VALUE_CHANGED) {
            type = wxEVT_DATAVIEW_ITEM_VALUE_CHANGED;
        }
        else if (event.GetEventType() == wxEVT_TREE_CHOICE) {
            type = wxEVT_DATAVIEW_CHOICE_BUTTON;
        }
        if (type != wxEVT_ANY)
        {
#if wxCHECK_VERSION(3, 1, 0)
            wxDataViewEvent e(type, &m_dummy, DV_ITEM(event.GetItem()));
#else
            wxDataViewEvent e(type);
            e.SetItem(DV_ITEM(event.GetItem()));
#endif
            e.SetEventObject(this);
            e.SetColumn(event.GetInt());
            e.SetString(eventText);
            if (!GetEventHandler()->ProcessEvent(e)) {
                event.Skip();
                //return false;
            }
            //return true;
        }
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::DeleteAllItems(const std::function<void(wxUIntPtr)>& deleterFunc)
    {
        // If a deleter was provided, call it per user's item data
        if (deleterFunc && m_model.GetRoot())
        {
            RowEntry::Vec_t & children = m_model.GetRoot()->GetChildren();
            for (size_t i = 0; i < children.size(); ++i)
            {
                wxUIntPtr userData = children[i]->GetData();
                if (userData)
                {
                    deleterFunc(userData);
                }
                children[i]->SetData(0);
            }
        }
        TreeCtrl::DeleteAllItems();
        // DVC must always have the hidden root
        AddRoot("Hidden Root", -1, -1, nullptr);
    }
    //-------------------------------------------------------------------------------------
    wxDataViewItem DataViewListCtrl::AppendItem(const wxString& text, int image, int selImage, wxUIntPtr data)
    {
        wxTreeItemId child = TreeCtrl::AppendItem(GetRootItem(), text, image, selImage, nullptr);
        // mark this row as a "list-view" row (i.e. it can't have children)
        m_model.ToPtr(child)->SetListItem(false);
        wxDataViewItem dvItem = DV_ITEM(child);
        SetItemData(dvItem, data);
        UpdateScrollBar();
        return dvItem;
    }
    //-------------------------------------------------------------------------------------
    wxDataViewItem DataViewListCtrl::AppendItem(const wxDataViewItem& parent, const wxString& text, int image,
        int selImage, wxUIntPtr data)
    {
        wxTreeItemId child = TreeCtrl::AppendItem(TREE_ITEM(parent), text, image, selImage, nullptr);
        // mark this row as a "list-view" row (i.e. it can't have children)
        m_model.ToPtr(child)->SetListItem(false);
        wxDataViewItem dvItem = DV_ITEM(child);
        SetItemData(dvItem, data);
        UpdateScrollBar();
        return dvItem;
    }
    //-------------------------------------------------------------------------------------
    wxDataViewItem DataViewListCtrl::AppendItem(const wxString& text, bool set, int image, int selImage, wxUIntPtr data)
    {
        wxTreeItemId child = TreeCtrl::AppendItem(GetRootItem(), text, image, selImage, nullptr);
        // mark this row as a "list-view" row (i.e. it can't have children)
        RowEntry * re = m_model.ToPtr(child);
        re->SetListItem(false);
        re->SetChecked(set);
        wxDataViewItem dvItem = DV_ITEM(child);
        SetItemData(dvItem, data);
        UpdateScrollBar();
        return dvItem;
    }
    //-------------------------------------------------------------------------------------
    wxDataViewItem DataViewListCtrl::AppendItem(const wxDataViewItem& parent, const wxString& text, bool set,
        int image, int selImage, wxUIntPtr data)
    {
        wxTreeItemId child = TreeCtrl::AppendItem(TREE_ITEM(parent), text, image, selImage, nullptr);
        // mark this row as a "list-view" row (i.e. it can't have children)
        RowEntry * re = m_model.ToPtr(child);
        re->SetListItem(false);
        re->SetChecked(set);
        wxDataViewItem dvItem = DV_ITEM(child);
        SetItemData(dvItem, data);
        UpdateScrollBar();
        return dvItem;
    }
    //-------------------------------------------------------------------------------------
    wxDataViewItem DataViewListCtrl::InsertItem(const wxDataViewItem& previous, const wxString& text, int image,
        int selImage, wxUIntPtr data)
    {
        wxTreeItemId child = TreeCtrl::InsertItem(GetRootItem(), wxTreeItemId(previous.GetID()), text, image, selImage, nullptr);
        // mark this row as a "list-view" row (i.e. it can't have children)
        m_model.ToPtr(child)->SetListItem(false);
        wxDataViewItem dvItem = DV_ITEM(child);
        SetItemData(dvItem, data);
        return dvItem;
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::SetItemData(const wxDataViewItem& item, wxUIntPtr data)
    {
        RowEntry* r = m_model.ToPtr(TREE_ITEM(item));
        r->SetData(data);
    }
    //-------------------------------------------------------------------------------------
    wxUIntPtr DataViewListCtrl::GetItemData(const wxDataViewItem& item) const
    {
        RowEntry* r = m_model.ToPtr(TREE_ITEM(item));
        return r->GetData();
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::SetItemBackgroundColour(const wxDataViewItem& item, const wxColour& colour, size_t col)
    {
        TreeCtrl::SetItemBackgroundColour(TREE_ITEM(item), colour, col);
    }
    //-------------------------------------------------------------------------------------
    wxColour DataViewListCtrl::GetItemBackgroundColour(const wxDataViewItem& item, size_t col) const
    {
        return TreeCtrl::GetItemBackgroundColour(TREE_ITEM(item), col);
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::SetItemTextColour(const wxDataViewItem& item, const wxColour& colour, size_t col)
    {
        TreeCtrl::SetItemTextColour(TREE_ITEM(item), colour, col);
    }
    //-------------------------------------------------------------------------------------
    wxColour DataViewListCtrl::GetItemTextColour(const wxDataViewItem& item, size_t col) const
    {
        return TreeCtrl::GetItemTextColour(TREE_ITEM(item), col);
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::SetItemText(const wxDataViewItem& item, const wxString& text, size_t col)
    {
        TreeCtrl::SetItemText(TREE_ITEM(item), text, col);
    }
    //-------------------------------------------------------------------------------------
    const wxString & DataViewListCtrl::GetItemText(const wxDataViewItem& item, size_t col) const
    {
        return TreeCtrl::GetItemText(TREE_ITEM(item), col);
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::SetItemBold(const wxDataViewItem& item, bool bold, size_t col)
    {
        TreeCtrl::SetItemBold(TREE_ITEM(item), bold, col);
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::SetItemFont(const wxDataViewItem& item, const wxFont& font, size_t col)
    {
        TreeCtrl::SetItemFont(TREE_ITEM(item), font, col);
    }
    //-------------------------------------------------------------------------------------
    wxFont DataViewListCtrl::GetItemFont(const wxDataViewItem& item, size_t col) const
    {
        return TreeCtrl::GetItemFont(TREE_ITEM(item), col);
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::EnableStyle(int style, bool enable, bool refresh)
    {
        if (m_stylesMap.count(style))
            TreeCtrl::EnableStyle(m_stylesMap[style], enable, refresh);
    }
    //-------------------------------------------------------------------------------------
    HeaderItem* DataViewListCtrl::GetColumn(size_t index)
    {
        if (index < GetHeader()->size())
            return &GetHeader()->Item(index);
        return nullptr;
    }
    //-------------------------------------------------------------------------------------
    wxDataViewItem DataViewListCtrl::RowToItem(size_t row) const
    {
        // Since a DataViewListCtrl is basically a tree with a single hidden node (the root)
        // A row is simply a child at a given index
        RowEntry* root = m_model.GetRoot();
        if (root)
        {
            if (row < root->GetChildren().size())
                return wxDataViewItem(root->GetChildren()[row]);
        }
        return wxDataViewItem();
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::DeleteItem(size_t row)
    {
        wxDataViewItem item = RowToItem(row);
        if (item.IsOk())
        {
            Delete(TREE_ITEM(item));
        }
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::SetValue(const wxVariant& value, size_t row, size_t col)
    {
        wxDataViewItem item = RowToItem(row);
        if (item.IsOk())
        {
            RowEntry* r = m_model.ToPtr(TREE_ITEM(item));
            DoSetCellValue(r, col, value);
        }
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::DoSetCellValue(RowEntry* row, size_t col, const wxVariant& value)
    {
        wxString variantType = value.GetType();
        if (variantType == "bool") {
            row->SetChecked(value.GetBool(), wxNOT_FOUND, wxString(), col);
        }
        else if (variantType == "string") {
            row->SetLabel(value.GetString(), col);
        }
        else if (variantType == "DataViewCheckbox") {
            DataViewCheckbox check;
            check << value;
            row->SetChecked(check.IsChecked(), check.GetBitmapIndex(), check.GetText(), col);
        }
        else if (variantType == "wxDataViewIconText") {
            // Extract the iamge + text from the wxDataViewIconText class
            wxDataViewIconText iconText;
            iconText << value;
            //  update the row with the icon + text
            row->SetLabel(iconText.GetText(), col);
            //row->SetBitmapIndex(iconText.GetIcon(), col);
        }
        else if (variantType == "DataViewTextBitmap") {
            // Extract the iamge + text from the wxDataViewIconText class
            DataViewTextBitmap iconText;
            iconText << value;
            //  update the row with the icon + text
            row->SetLabel(iconText.GetText(), col);
            row->SetBitmapIndex(iconText.GetBitmapIndex(), col);
        }
        else if (variantType == "DataViewChoice") {
            DataViewChoice choice;
            choice << value;
            row->SetChoice(true, col);
            row->SetBitmapIndex(choice.GetBitmapIndex(), col);
            row->SetLabel(choice.GetLabel(), col);
        }
        else if (variantType == "double") {
            row->SetLabel(wxString() << value.GetDouble(), col);
        }
        else if (variantType == "datetime") {
            row->SetLabel(value.GetDateTime().FormatDate(), col);
        }
        // Call this to update the view + update the header bar
        TreeCtrl::SetItemText(wxTreeItemId(row), row->GetLabel(col), col);
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::SetSortFunction(const TreeSortFunc & CompareFunc)
    {
        RowEntry* root = m_model.GetRoot();
        if (root)
        {
            // Disconnect the current function, if any
            m_model.SetSortFunction(nullptr);
            if (!CompareFunc) {
                // we are done
                return;
            }

            // This list ctrl is composed of a hidden root + its children
            // Step 1:
            RowEntry::Vec_t& children = root->GetChildren();
            for (size_t i = 0; i < children.size(); ++i) {
                RowEntry* child = children[i];
                child->SetNext(nullptr);
                child->SetPrev(nullptr);
            }

            // Step 2: disconect the root
            root->SetNext(nullptr);

            // Step 3: sort the children
            std::sort(children.begin(), children.end(), CompareFunc);

            // Now, reconnect the children, starting with the root
            RowEntry* prev = root;
            for (size_t i = 0; i < children.size(); ++i) {
                RowEntry* child = children[i];
                prev->SetNext(child);
                child->SetPrev(prev);
                prev = child;
            }

            // and store the new sorting method
            m_model.SetSortFunction(CompareFunc);

            Refresh();
        }
    }
    //-------------------------------------------------------------------------------------
    int DataViewListCtrl::ItemToRow(const wxDataViewItem& item) const
    {
        RowEntry* pItem = m_model.ToPtr(TREE_ITEM(item));
        if (pItem)
        {
            RowEntry* root = m_model.GetRoot();
            if (root)
            {
                const RowEntry::Vec_t& children = root->GetChildren();
                for (size_t i = 0; i < children.size(); ++i) {
                    if (children[i] == pItem) { return i; }
                }
            }
        }
        return wxNOT_FOUND;
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::Select(const wxDataViewItem& item)
    {
        if (HasStyle(wxTR_MULTIPLE))
        {
            m_model.SelectItem(TREE_ITEM(item), true, true, false);
        }
        else {
            TreeCtrl::SelectItem(TREE_ITEM(item), true);
        }
    }
    //-------------------------------------------------------------------------------------
    wxDataViewItem DataViewListCtrl::FindNext(const wxDataViewItem& from, const wxString& what, size_t col,
        size_t searchFlags)
    {
        return DV_ITEM(TreeCtrl::FindNext(TREE_ITEM(from), what, col, searchFlags));
    }
    //-------------------------------------------------------------------------------------
    wxDataViewItem DataViewListCtrl::FindPrev(const wxDataViewItem& from, const wxString& what, size_t col,
        size_t searchFlags)
    {
        return DV_ITEM(TreeCtrl::FindPrev(TREE_ITEM(from), what, col, searchFlags));
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::HighlightText(const wxDataViewItem& item, bool b)
    {
        TreeCtrl::HighlightText(TREE_ITEM(item), b);
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::ClearHighlight(const wxDataViewItem& item)
    {
        TreeCtrl::ClearHighlight(TREE_ITEM(item));
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::EnsureVisible(const wxDataViewItem& item)
    {
        TreeCtrl::EnsureVisible(TREE_ITEM(item));
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::ClearColumns()
    {
        GetHeader()->Clear();
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::SetItemChecked(const wxDataViewItem& item, bool checked, size_t col)
    {
        TreeCtrl::Check(TREE_ITEM(item), checked, col);
    }
    //-------------------------------------------------------------------------------------
    bool DataViewListCtrl::IsItemChecked(const wxDataViewItem& item, size_t col) const
    {
        return TreeCtrl::IsChecked(TREE_ITEM(item), col);
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::ShowMenuForItem(const wxDataViewItem& item, wxMenu& menu, size_t col)
    {
        RowEntry* row = m_model.ToPtr(TREE_ITEM(item));
        if (row)
        {
            wxRect r = row->GetCellRect(col);
            PopupMenu(&menu, r.GetBottomLeft());
        }
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::ShowStringSelectionMenu(const wxDataViewItem& item, const wxArrayString& choices, size_t col)
    {
        RowEntry* row = m_model.ToPtr(TREE_ITEM(item));
        if (!row) { return; }
        const wxString& currentSelection = row->GetLabel(col);
        wxMenu menu;
        wxString selectedString;
        std::unordered_map<int, wxString> M;
        for (const wxString& str : choices) {
            int id = wxXmlResource::GetXRCID(str);
            wxMenuItem* item = menu.Append(id, str, str, wxITEM_CHECK);
            item->Check(currentSelection == str);
            M.insert({ id, str });
        }
        menu.Bind(wxEVT_MENU,
            [&](wxCommandEvent& event) {
            if (M.count(event.GetId())) { selectedString = M[event.GetId()]; }
        },
            wxID_ANY);
        wxRect r = row->GetCellRect(col);
        PopupMenu(&menu, r.GetBottomLeft());
        if (!selectedString.IsEmpty()) {
            // fire selection made event
#if wxCHECK_VERSION(3, 1, 0)
            wxDataViewEvent e(wxEVT_DATAVIEW_CHOICE, &m_dummy, item);
#else
            wxDataViewEvent e(wxEVT_DATAVIEW_CHOICE);
            e.SetItem(item);
#endif
            e.SetEventObject(this);
            e.SetColumn(col);
            e.SetString(selectedString);
            e.Allow(); // by default allow
            GetEventHandler()->ProcessEvent(e);
            if (e.IsAllowed()) {
                SetItemText(item, selectedString, col);
            }
        }
    }
    //-------------------------------------------------------------------------------------
    void DataViewListCtrl::DoInitialize(long style)
    {
        // Map DataViewListCtrl to TreeCtrl style
        if (m_stylesMap.empty()) {
            m_stylesMap.insert({ wxDV_ROW_LINES, wxTR_ROW_LINES });
            m_stylesMap.insert({ wxDV_MULTIPLE, wxTR_MULTIPLE });
            m_stylesMap.insert({ wxDV_ENABLE_SEARCH, wxTR_ENABLE_SEARCH });
        }

        m_treeStyle = wxTR_HIDE_ROOT;
        if (style & wxDV_ROW_LINES) { m_treeStyle |= wxTR_ROW_LINES; }
        if (style & wxDV_MULTIPLE) { m_treeStyle |= wxTR_MULTIPLE; }

        if (style & wxDV_NO_HEADER)
        {
            SetShowHeader(false);
        }
        else
        {
            SetShowHeader(true);
        }
        //my_style |= wxTR_HIDE_ROOT;
        //m_treeStyle = my_style;

        // Ignore these events
        Bind(wxEVT_TREE_ITEM_EXPANDING, [](wxTreeEvent&) {});
        Bind(wxEVT_TREE_ITEM_EXPANDED, [](wxTreeEvent&) {});
        Bind(wxEVT_TREE_ITEM_COLLAPSING, [](wxTreeEvent&) {});
        Bind(wxEVT_TREE_ITEM_COLLAPSED, [](wxTreeEvent&) {});
        Bind(wxEVT_TREE_DELETE_ITEM, [](wxTreeEvent&) {});
        Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, [](wxTreeEvent& e) { e.Skip(); });

        // Translate the following events to wxDVC events
        Bind(wxEVT_TREE_ITEM_VALUE_CHANGED, &DataViewListCtrl::OnConvertEvent, this);
        Bind(wxEVT_TREE_CHOICE, &DataViewListCtrl::OnConvertEvent, this);
        Bind(wxEVT_TREE_BEGIN_DRAG, &DataViewListCtrl::OnConvertEvent, this);
        Bind(wxEVT_TREE_END_DRAG, &DataViewListCtrl::OnConvertEvent, this);
        Bind(wxEVT_TREE_SEL_CHANGED, &DataViewListCtrl::OnConvertEvent, this);
        Bind(wxEVT_TREE_ITEM_ACTIVATED, &DataViewListCtrl::OnConvertEvent, this);
        Bind(wxEVT_TREE_ITEM_MENU, &DataViewListCtrl::OnConvertEvent, this);
        Bind(wxEVT_TREE_SEARCH_TEXT, &DataViewListCtrl::OnConvertEvent, this);
        Bind(wxEVT_TREE_CLEAR_SEARCH, &DataViewListCtrl::OnConvertEvent, this);

        AddRoot("Hidden Root", -1, -1, nullptr);
    }
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    // ThemedListCtrl
    //-------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------
    ThemedListCtrl::ThemedListCtrl()
    {
        m_keyboard.reset(nullptr);
    }
    //-------------------------------------------------------------------------------------
    ThemedListCtrl::ThemedListCtrl(wxWindow * parent, wxWindowID id, const wxPoint & pos, const wxSize & size, long style):
        DataViewListCtrl(parent, id, pos, size, style | LIST_STYLE)
    {
        gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &ThemedListCtrl::OnTheme, this);
        SetSortFunction(nullptr);
        
        ApplyTheme();

        // Enable keyboard search
        m_keyboard.reset(new TreeKeyboardInput(this));
    }
    //-------------------------------------------------------------------------------------
    ThemedListCtrl::~ThemedListCtrl()
    {
        m_keyboard.reset(nullptr);
        gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &ThemedListCtrl::OnTheme, this);
    }
    //-------------------------------------------------------------------------------------
    void ThemedListCtrl::OnTheme(wxCommandEvent& event)
    {
        event.Skip();
        ApplyTheme();
    }
    //-------------------------------------------------------------------------------------
    void ThemedListCtrl::ApplyTheme()
    {
        LexerStyle::Ptr_t lexer = gsgs_Style().GetLexer("text");
        Colour colours;
        if (lexer->IsDark()) {
            colours.setBase(gsgs_Style().GetAppBgColour());
        }
        else {
            colours.setDefault();
        }

        // Set the built-in search colours
        const wxColour & highlightColur = gsgs_Style().GetColour(wxSYS_COLOUR_HIGHLIGHT);
        const wxColour & textColour = gsgs_Style().GetColour(wxSYS_COLOUR_HIGHLIGHTTEXT);
        colours.SetMatchedItemBgText(highlightColur);
        colours.SetMatchedItemText(textColour);
        colours.SetButtonColour(gsgs_Style().GetActiveMarkerColour());
        colours.SetSelbuttonColour(gsgs_Style().GetHoverMarkerColour());
        // When not using custom colours, use system defaults
        if (!gsgs_Style().IsCustomColour()) {
            colours.SetSelItemBgColour(gsgs_Style().GetActiveMarkerColour());
#ifdef __WXOSX__
            colours.SetSelItemBgColourNoFocus(gsgs_Style().GetColour(wxSYS_COLOUR_HIGHLIGHT));
#else
            colours.SetSelItemBgColourNoFocus(colours.GetSelItemBgColour().ChangeLightness(110));
#endif
        }
        // When using custom bg colour, don't use native drawings
        //this->SetNativeTheme(!useCustomColour);
        this->SetColours(colours);
        this->SetNativeTheme(false);
    }
    //-------------------------------------------------------------------------------------
    class WindowTreeItemData : public wxTreeItemData
    {
    public:
        WindowTreeItemData() : mWindow(0) {}

        WindowTreeItemData(wxWindow* win) : mWindow(win) {}

        wxWindow* GetSelectWindow() const { return mWindow; }
    protected:
        wxWindow* mWindow;
    };
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    // Treebook
    //--------------------------------------------------------------------------------
    // -------------------------------------------------------------------------------
    Treebook::Treebook(wxWindow* parent, wxWindowID id, const wxPoint& pos,
        const wxSize & size, long style, const wxString& name) :
        wxPanel(parent, id, pos, size, wxWANTS_CHARS | style, name)
    {
        wxWindowUpdateLocker nnl(this);
        SetAutoLayout(false);
        SetBackgroundColour(gsgs_Style().GetAppBgColour());

        mTreeCtrl = new DefaultTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(150, -1),
            wxTR_ROW_LINES | wxTR_HIDE_ROOT | wxTR_ENABLE_SEARCH | wxBORDER_SIMPLE);
        mTreeCtrl->AddTreeStyle(wxTR_SORT_TOP_LEVEL);
        mTreeCtrl->SetShowHeader(FALSE);
        mTreeCtrl->SetSortEnable(false);

        mTreeCtrl->AddHeader(_T("Ctrl"));
        mTreeCtrl->AddRoot(_T("Ctrl"));
        mTreeCtrl->Bind(wxEVT_TREE_SEL_CHANGED, &Treebook::OnSelected, this);
        mWinList = new WindowStack(this);
        //PositionControls();

        SetSizer(new wxBoxSizer(wxHORIZONTAL));
        GetSizer()->Add(mTreeCtrl, 0, wxALL | wxEXPAND, 5);
        GetSizer()->Add(mWinList, 1, wxUP | wxDOWN | wxRIGHT | wxEXPAND, 5);
    }
    // -------------------------------------------------------------------------------
    Treebook::~Treebook()
    {
        mTreeCtrl->Unbind(wxEVT_TREE_SEL_CHANGED, &Treebook::OnSelected, this);
    }
    //--------------------------------------------------------------------------------
    void Treebook::OnSelected(wxTreeEvent& event)
    {
        wxTreeItemId selection = mTreeCtrl->GetSelection();
        if (selection.IsOk())
        {
            WindowTreeItemData * temp = static_cast<WindowTreeItemData *>(mTreeCtrl->GetItemData(selection));
            mWinList->Select(temp->GetSelectWindow());
        }
    }
    // -------------------------------------------------------------------------------
    /*void Treebook::PositionControls()
    {
        size_t style = GetStyle();

        // Detach the controls from the main sizer
        if (GetSizer()) {
            GetSizer()->Detach(mWinList);
            GetSizer()->Detach(mTab->getMe());
        }

        // Set new sizer
        if (style & kNotebook_BottomTabs)
        {
            // Tabs are placed under the pages
            SetSizer(new wxBoxSizer(wxVERTICAL));
            GetSizer()->Add(mWinList, 1, wxEXPAND);
            GetSizer()->Add(mTab->getMe(), 0, wxEXPAND);
        }
        else if (style & kNotebook_LeftTabs)
        {
            // Tabs are placed to the left of the pages
            SetSizer(new wxBoxSizer(wxHORIZONTAL));
            GetSizer()->Add(mTab->getMe(), 0, wxEXPAND);
            GetSizer()->Add(mWinList, 1, wxEXPAND);
        }
        else if (style & kNotebook_RightTabs)
        {
            // Tabs are placed to the right of the pages
            SetSizer(new wxBoxSizer(wxHORIZONTAL));
            GetSizer()->Add(mWinList, 1, wxEXPAND);
            GetSizer()->Add(mTab->getMe(), 0, wxEXPAND);
        }
        else {
            // Tabs are placed on top of the pages
            SetSizer(new wxBoxSizer(wxVERTICAL));
            GetSizer()->Add(mTab->getMe(), 0, wxEXPAND);
            GetSizer()->Add(mWinList, 1, wxEXPAND);
        }
        //wxWindowUpdateLocker ll(this);
        //Layout();
    }*/
    //--------------------------------------------------------------------------------
    /*int Treebook::SetSelection(size_t selection) 
    { 
        return mTab->SetSelection(selection); 
    }
    //--------------------------------------------------------------------------------
    int Treebook::GetSelection() const 
    { 
        return mTab->GetSelection(); 
    }*/
    // -------------------------------------------------------------------------------
    void Treebook::AddPage(wxWindow* page, const wxString& label, bool selected, const wxBitmap& bmp)
    {
        mWinList->Add(page, selected);
        wxTreeItemId curidx = mTreeCtrl->AppendItem(label);
        mTreeCtrl->SetItemData(curidx, new WindowTreeItemData(page));
        mTreeCtrl->SelectItem(curidx, selected);
    }
    // -------------------------------------------------------------------------------
    bool Treebook::InsertPage(const wxTreeItemId & parent, wxWindow * page, const wxString& label, bool selected, const wxBitmap& bmp)
    {
        mWinList->Add(page, selected);
        wxTreeItemId curidx = mTreeCtrl->AppendItem(parent, label);
        mTreeCtrl->SetItemData(curidx, new WindowTreeItemData(page));
        mTreeCtrl->SelectItem(curidx, selected);
        return true;
    }
    // -------------------------------------------------------------------------------
    wxWindow * Treebook::GetCurrentPage() const
    {
        return mWinList->GetSelected();
    }
    // -------------------------------------------------------------------------------
    void Treebook::EnableStyle(NotebookStyle style, bool enable)
    {
        //size_t flags = GetStyle();
        /*if (enable) {
            flags |= style;
        }
        else {
            flags &= ~style;
        }*/
        //SetStyle(flags);
    }
    // -------------------------------------------------------------------------------
    /*void Treebook::SetTabDirection(wxDirection d)
    {
        size_t flags = GetStyle();
        // Clear all direction styles
        flags &= ~kNotebook_BottomTabs;
        flags &= ~kNotebook_LeftTabs;
        flags &= ~kNotebook_RightTabs;
#if 0
        if (d == wxBOTTOM || d == wxRIGHT)
        {
            flags |= kNotebook_BottomTabs;
        }
#else
        if (d == wxBOTTOM) {
            flags |= kNotebook_BottomTabs;
        }
        else if (d == wxRIGHT) {
            flags |= kNotebook_RightTabs;
        }
        else if (d == wxLEFT) {
            flags |= kNotebook_LeftTabs;
        }
#endif
        SetStyle(flags);
    }*/
    // -------------------------------------------------------------------------------

// Images used by the checktreectrl

/* XPM */
    static const char *Checkbox_off_xpm[] = {
        /* columns rows colors chars-per-pixel */
        "16 16 11 1",
        "$ c #F7F7F7",
        "% c #FFFFFF",
        ". c #E7E7DE",
        "  c #185284",
        "X c #DEDEDE",
        "o c #E7E7DE",
        "@ c #E7E7E7",
        "O c #EFEFE7",
        "+ c #EFEFEF",
        "# c #F7F7EF",
        "& c None",
        /* pixels */
        "&&&&&&&&&&&&&&&&",
        "                ",
        " ......XooOO+.. ",
        " ....XXo@O++#.. ",
        " ...XXo@O+++#.. ",
        " ..Xoo@@++#$$.. ",
        " ..Xo@O++#+$$.. ",
        " .oo@@+++$$$$.. ",
        " .o@O++#+$$%%.. ",
        " .oO+++$$$$%%.. ",
        " .O++#$#$$%%%.. ",
        " .O++$$$$%%%%.. ",
        " .+#$#$$%%%%%.. ",
        " .............. ",
        "                ",
        "&&&&&&&&&&&&&&&&"
    };


    /* XPM */
    static const char *Checkbox_on_xpm[] = {
        /* columns rows colors chars-per-pixel */
        "16 16 10 1",
        "# c #F7F7F7",
        "$ c #FFFFFF",
        "@ c #21A521",
        "  c #185284",
        ". c #E7E7DE",
        "X c #E7E7E7",
        "o c #EFEFE7",
        "O c #EFEFEF",
        "+ c #F7F7EF",
        "& c None",
        /* pixels */
        "&&&&&&&&&&&&&&&&",
        "                ",
        " .......XooO+.. ",
        " .....XXooO+O.. ",
        " ....XXXOO@+#.. ",
        " ...XXXoO@@##.. ",
        " ...@XOO@@@##.. ",
        " ..X@@O@@@##$.. ",
        " .Xo@@@@@##$#.. ",
        " .ooO@@@##$$$.. ",
        " .oO+O@##$#$$.. ",
        " .O+O###$$$$$.. ",
        " .++###$$#$$$.. ",
        " .............. ",
        "                ",
        "&&&&&&&&&&&&&&&&"
    };


    //-----------------------------------------------------------
    // wxCheckTreeCtrl
    //-----------------------------------------------------------

    DEFINE_EVENT_TYPE(wxEVT_CKTR_ITEM_SELECTED)
        DEFINE_EVENT_TYPE(wxEVT_CKTR_ITEM_UNSELECTED)

        BEGIN_EVENT_TABLE(wxCheckTreeCtrl, wxTreeCtrl)
        EVT_LEFT_DOWN(wxCheckTreeCtrl::OnLeftDown)
        EVT_LEFT_DCLICK(wxCheckTreeCtrl::OnLeftDown)
        END_EVENT_TABLE()

        wxCheckTreeCtrl::wxCheckTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name)
        : m_checkedBmp(16, 16)
        , m_uncheckedBmp(16, 16)
    {
        Create(parent, id, pos, size, style | wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT, validator, name);
        //create images
        m_checkedBmp = wxBitmap(Checkbox_on_xpm);
        m_uncheckedBmp = wxBitmap(Checkbox_off_xpm);

        //create an image list and assign it
        wxImageList *il = new wxImageList(16, 16, true);
        il->Add(m_checkedBmp);		//0
        il->Add(m_uncheckedBmp);	//1

        //will be owned by the control
        AssignImageList(il);
    }

    wxCheckTreeCtrl::~wxCheckTreeCtrl()
    {
    }

    wxTreeItemId wxCheckTreeCtrl::AddRoot(const wxString& text, bool checked, wxTreeItemData* data)
    {
        return wxTreeCtrl::AddRoot(text, checked ? 0 : 1, checked ? 0 : 1, data);
    }

    wxTreeItemId wxCheckTreeCtrl::AppendItem(const wxTreeItemId& parent, const wxString& text, bool checked, wxTreeItemData* data)
    {
        return wxTreeCtrl::AppendItem(parent, text, checked ? 0 : 1, checked ? 0 : 1, data);
    }

    void wxCheckTreeCtrl::OnLeftDown(wxMouseEvent &event)
    {
        int flags;
        wxTreeItemId item = wxTreeCtrl::HitTest(event.GetPosition(), flags);
        if (item.IsOk() && flags & wxTREE_HITTEST_ONITEMICON) {
            if (IsChecked(item)) {
                //fire unselect event
                wxCheckTreeCtrlEvent e(wxEVT_CKTR_ITEM_UNSELECTED, GetId());
                e.SetItem(item);
                e.SetEventObject(this);
                GetEventHandler()->ProcessEvent(e);

                //Veto?
                if (!e.IsAllowed()) {
                    return;
                }
                Check(item, false);

            }
            else {
                //fire select event
                wxCheckTreeCtrlEvent e(wxEVT_CKTR_ITEM_SELECTED, GetId());
                e.SetItem(item);
                e.SetEventObject(this);
                GetEventHandler()->ProcessEvent(e);

                //Veto?
                if (!e.IsAllowed()) {
                    return;
                }
                Check(item, true);
            }
            return;
        }
        event.Skip();
    }

    bool wxCheckTreeCtrl::IsChecked(const wxTreeItemId &item) const
    {
        int imgId = GetItemImage(item);
        return imgId == 0;
    }

    void wxCheckTreeCtrl::Check(const wxTreeItemId &item, bool check)
    {
        if (check) {
            SetItemImage(item, 0);
            SetItemImage(item, 0, wxTreeItemIcon_Selected);
        }
        else {
            SetItemImage(item, 1);
            SetItemImage(item, 1, wxTreeItemIcon_Selected);
        }
    }

    void wxCheckTreeCtrl::GetItemChildrenRecursive(const wxTreeItemId &parent, std::list<wxTreeItemId> &children)
    {
        //delete the item's children
        wxTreeItemIdValue cookie;
        children.push_back(parent);
        wxTreeItemId child = GetFirstChild(parent, cookie);
        while (child.IsOk())
        {
            if (ItemHasChildren(child)) {
                GetItemChildrenRecursive(child, children);
            }
            else {
                children.push_back(child);
            }
            child = GetNextChild(parent, cookie);
        }
    }

    void wxCheckTreeCtrl::RecursiveCheck(const wxTreeItemId &item, bool check)
    {
        std::list<wxTreeItemId> children;
        GetItemChildrenRecursive(item, children);

        std::list<wxTreeItemId>::iterator iter = children.begin();
        for (; iter != children.end(); iter++)
        {
            Check((*iter), check);
        }
    }
}