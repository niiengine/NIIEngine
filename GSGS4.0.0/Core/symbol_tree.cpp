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
#include "gsgsFileUtil.h"
#include "gsgsPreProcess.h"

#include "gsgsImageManager.h"
#include "gsgsTagManager.h"
#include "globals.h"
#include "symbol_tree.h"
#include <functional>
#include <imanager.h>
#include <wx/wupdlock.h>

namespace gsgs
{
    //-----------------------------------------------------------------
    SymbolTree::SymbolTree()
        : m_sortByLineNumber(true)
    {
        InitialiseSymbolMap();
        TreeSortFunc func = [&](RowEntry* a, RowEntry* b) {
            // Line number compare
            MyTreeItemData* cd1 = dynamic_cast<MyTreeItemData*>(a->GetClientObject());
            MyTreeItemData* cd2 = dynamic_cast<MyTreeItemData*>(b->GetClientObject());
            if (m_sortByLineNumber && cd1 && cd2) {
                return cd1->GetLine() < cd2->GetLine();
            }
            else {
                // ABC compare
                int img1, img2;
                img1 = a->GetBitmapIndex();
                img2 = b->GetBitmapIndex();
                if (img1 < img2)
                    return true;
                else if (img1 < img2)
                    return false;
                else {
                    // Items  has the same icons, compare text
                    return (a->GetLabel().CmpNoCase(b->GetLabel()) < 0);
                }
            }
        };
        SetSortFunction(func);
    }
    //-----------------------------------------------------------------
    SymbolTree::SymbolTree(wxWindow* parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
        : m_sortByLineNumber(true)
    {
        InitialiseSymbolMap();
        DefaultTreeCtrl::Create(parent, id, pos, size, style | wxTR_HIDE_ROOT);
        TreeSortFunc func = [&](RowEntry* a, RowEntry* b) {
            // Line number compare
            MyTreeItemData* cd1 = dynamic_cast<MyTreeItemData*>(a->GetClientObject());
            MyTreeItemData* cd2 = dynamic_cast<MyTreeItemData*>(b->GetClientObject());
            if (m_sortByLineNumber && cd1 && cd2) {
                return cd1->GetLine() < cd2->GetLine();
            }
            else {
                // ABC compare
                int img1, img2;
                img1 = a->GetBitmapIndex();
                img2 = b->GetBitmapIndex();
                if (img1 < img2)
                    return true;
                else if (img1 < img2)
                    return false;
                else {
                    // Items  has the same icons, compare text
                    return (a->GetLabel().CmpNoCase(b->GetLabel()) < 0);
                }
            }
        };
        SetSortFunction(func);
    }
    //-----------------------------------------------------------------
    SymbolTree::~SymbolTree() {}
    //-----------------------------------------------------------------
    void SymbolTree::InitialiseSymbolMap()
    {
        //--------------------------------------------------------
        // Initialise the images map (kind:icon_index)
        //--------------------------------------------------------
        ImageManager* loader = &gsgs_Image();
        m_imagesMap["class_view"] = loader->GetMimeImageId(ImageManager::kClass);
        m_imagesMap["project"] = loader->GetMimeImageId(FileUtil::TypeProject);
        m_imagesMap["namespace"] = loader->GetMimeImageId(ImageManager::kNamespace);
        m_imagesMap["globals"] = loader->GetMimeImageId(ImageManager::kAngleBrackets);
        m_imagesMap["class"] = loader->GetMimeImageId(ImageManager::kClass);
        m_imagesMap["interface"] = loader->GetMimeImageId(ImageManager::kClass);
        m_imagesMap["interface_private"] = loader->GetMimeImageId(ImageManager::kClass);
        m_imagesMap["interface_protected"] = loader->GetMimeImageId(ImageManager::kClass);
        m_imagesMap["class_private"] = loader->GetMimeImageId(ImageManager::kClass);
        m_imagesMap["class_public"] = loader->GetMimeImageId(ImageManager::kClass);
        m_imagesMap["class_protected"] = loader->GetMimeImageId(ImageManager::kClass);
        m_imagesMap["struct"] = loader->GetMimeImageId(ImageManager::kStruct);
        m_imagesMap["struct_private"] = loader->GetMimeImageId(ImageManager::kStruct);
        m_imagesMap["struct_public"] = loader->GetMimeImageId(ImageManager::kStruct);
        m_imagesMap["struct_protected"] = loader->GetMimeImageId(ImageManager::kStruct);
        m_imagesMap["function"] = loader->GetMimeImageId(ImageManager::kFunctionPublic);
        m_imagesMap["prototype"] = loader->GetMimeImageId(ImageManager::kFunctionPublic);
        m_imagesMap["function_public"] = loader->GetMimeImageId(ImageManager::kFunctionPublic);
        m_imagesMap["prototype_public"] = loader->GetMimeImageId(ImageManager::kFunctionPublic);
        m_imagesMap["function_protected"] = loader->GetMimeImageId(ImageManager::kFunctionProtected);
        m_imagesMap["prototype_protected"] = loader->GetMimeImageId(ImageManager::kFunctionProtected);
        m_imagesMap["function_private"] = loader->GetMimeImageId(ImageManager::kFunctionPrivate);
        m_imagesMap["prototype_private"] = loader->GetMimeImageId(ImageManager::kFunctionPrivate);
        m_imagesMap["variable"] = loader->GetMimeImageId(ImageManager::kMemberPublic);
        m_imagesMap["member"] = loader->GetMimeImageId(ImageManager::kMemberPublic);
        m_imagesMap["member_public"] = loader->GetMimeImageId(ImageManager::kMemberPublic);
        m_imagesMap["member_protected"] = loader->GetMimeImageId(ImageManager::kMemberProtected);
        m_imagesMap["member_private"] = loader->GetMimeImageId(ImageManager::kMemberPrivate);
        m_imagesMap["typedef"] = loader->GetMimeImageId(ImageManager::kTypedef);
        m_imagesMap["typedef_public"] = loader->GetMimeImageId(ImageManager::kTypedef);
        m_imagesMap["typedef_private"] = loader->GetMimeImageId(ImageManager::kTypedef);
        m_imagesMap["typedef_protected"] = loader->GetMimeImageId(ImageManager::kTypedef);
        m_imagesMap["macro"] = loader->GetMimeImageId(ImageManager::kMacro);
        m_imagesMap["macro_private"] = loader->GetMimeImageId(ImageManager::kMacro);
        m_imagesMap["macro_protected"] = loader->GetMimeImageId(ImageManager::kMacro);
        m_imagesMap["macro_public"] = loader->GetMimeImageId(ImageManager::kMacro);
        m_imagesMap["enum"] = loader->GetMimeImageId(ImageManager::kEnum);
        m_imagesMap["cenum"] = loader->GetMimeImageId(ImageManager::kEnum);
        m_imagesMap["enum_private"] = loader->GetMimeImageId(ImageManager::kEnum);
        m_imagesMap["enum_public"] = loader->GetMimeImageId(ImageManager::kEnum);
        m_imagesMap["enum_protected"] = loader->GetMimeImageId(ImageManager::kEnum);
        m_imagesMap["enumerator"] = loader->GetMimeImageId(ImageManager::kEnumerator);
        m_imagesMap["method"] = loader->GetMimeImageId(ImageManager::kFunctionPublic);
        m_imagesMap["method_public"] = loader->GetMimeImageId(ImageManager::kFunctionPublic);
        m_imagesMap["method_protected"] = loader->GetMimeImageId(ImageManager::kFunctionProtected);
        m_imagesMap["method_private"] = loader->GetMimeImageId(ImageManager::kFunctionProtected);

        //-----------------------------------------------------------
        // Populate globals kind
        //-----------------------------------------------------------
        m_globalsKind[wxT("<global>")] = true;
        m_globalsKind[wxT("function")] = true;
        m_globalsKind[wxT("prototype")] = true;
        m_globalsKind[wxT("variable")] = true;
        m_globalsKind[wxT("enumerator")] = true;
    }
    //-----------------------------------------------------------------
    /*void SymbolTree::Create(wxWindow* parent, const wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    {
        DefaultTreeCtrl::Create(parent, id, pos, size, style);
        //BuildTree(wxFileName(), TagEntryPtrList());
    }*/
    //-----------------------------------------------------------------
    void SymbolTree::BuildTree(const wxFileName& fileName, const TagEntryPtrList& tags, bool forceBuild)
    {
        TagEntryPtrList newTags;
        if (tags.empty()) {

            // Get the current database
            ITagsStoragePtr db = TagsManagerST::Get()->GetDatabase();
            if (!db) {
                wxWindowUpdateLocker locker(this);
                Clear();
                return;
            }
            // Load the new tags from the database
            db->SelectTagsByFile(fileName.GetFullPath(), newTags);

            // Compare the new tags with the old ones
            if (!forceBuild && TagsManagerST::Get()->AreTheSame(newTags, m_currentTags)) return;

            m_currentTags.clear();
            m_currentTags.insert(m_currentTags.end(), newTags.begin(), newTags.end());

        }
        else {

            m_currentTags.clear();
            m_currentTags.insert(m_currentTags.end(), tags.begin(), tags.end());
        }

        wxWindowUpdateLocker locker(this);
        Clear();
        m_fileName = fileName;

        // Convert them into tree
        m_tree = TagsManagerST::Get()->Load(m_fileName, &m_currentTags);
        if (!m_tree) { return; }

        // Add invisible root node
        wxTreeItemId root;
        root = AddRoot(fileName.GetFullName(), 15, 15);

        TreeWalker<wxString, TagEntry> walker(m_tree->GetRoot());

        // add three items here:
        // the globals node, the mcros and the prototype node
        int nodeImgIdx = gsgs_Image().GetMimeImageId(ImageManager::kAngleBrackets);
        m_globalsNode = AppendItem(root, wxT("Global Functions and Variables"), nodeImgIdx, nodeImgIdx,
            new MyTreeItemData(wxT("Global Functions and Variables"), wxEmptyString));
        m_prototypesNode = AppendItem(root, wxT("Functions Prototypes"), nodeImgIdx, nodeImgIdx,
            new MyTreeItemData(wxT("Functions Prototypes"), wxEmptyString));
        m_macrosNode =
            AppendItem(root, wxT("Macros"), nodeImgIdx, nodeImgIdx, new MyTreeItemData(wxT("Macros"), wxEmptyString));

        for (; !walker.End(); walker++) {
            // Add the item to the tree
            TagNode* node = walker.GetNode();

            // Skip root node
            if (node->IsRoot()) continue;

            // Add the node
            AddItem(node);
        }

        if (ItemHasChildren(m_globalsNode) == false) { Delete(m_globalsNode); }
        if (ItemHasChildren(m_prototypesNode) == false) { Delete(m_prototypesNode); }
        if (ItemHasChildren(m_macrosNode) == false) { Delete(m_macrosNode); }
    }
    //-----------------------------------------------------------------
    void SymbolTree::AddItem(TagNode* node)
    {
        // Get node icon index
        TagEntry nodeData = node->GetData();

        int iconIndex = GetItemIconIndex(nodeData.GetKind(), nodeData.GetAccess());
        wxString displayName(nodeData.GetDisplayName());

        wxTreeItemId parentHti;
        if (nodeData.GetName().IsEmpty()) return;

        wxFont font = ScrolledWindow::GetDefaultFont();
        if (nodeData.GetKind() == wxT("prototype")) { font.SetStyle(wxFONTSTYLE_ITALIC); }
        if (nodeData.GetAccess() == wxT("public")) { font.SetWeight(wxFONTWEIGHT_BOLD); }

        //-------------------------------------------------------------------------------
        // We gather globals together under special node
        //-------------------------------------------------------------------------------
        if ((nodeData.GetParent() == wxT("<global>")) && // parent is global scope
            m_globalsKind.find(nodeData.GetKind()) !=
            m_globalsKind.end()) { // the node kind is one of function, prototype or variable
            if (nodeData.GetKind() == wxT("prototype"))
                parentHti = m_prototypesNode;
            else
                parentHti = m_globalsNode;
        }
        else
            parentHti = node->GetParent()->GetData().GetTreeItemId();

        //---------------------------------------------------------------------------------
        // Macros are gathered under the 'Macros' node
        //---------------------------------------------------------------------------------
        if (nodeData.GetKind() == wxT("macro")) { parentHti = m_macrosNode; }

        // only if parent is valid, we add item to the tree
        wxTreeItemId hti;

        if (parentHti.IsOk() == false) { parentHti = GetRootItem(); }

        if (parentHti.IsOk()) {
            hti = AppendItem(
                parentHti,   // parent
                displayName, // display name
                iconIndex,   // item image index
                iconIndex,   // selected item image
                new MyTreeItemData(node->GetData().GetFile(), node->GetData().GetPattern(), node->GetData().GetLine()));
            SetItemFont(hti, font);
            node->GetData().SetTreeItemId(hti);
            m_items[nodeData.Key()] = hti.m_pItem;
        }
    }
    //-----------------------------------------------------------------
    void SymbolTree::SelectItemByName(const wxString& name)
    {
        if (!Matches(GetRootItem(), name)) {
            // wxTreeItemId currentSelection = GetSelection();
            // if(currentSelection.IsOk()) {
            //     SelectItem(currentSelection, false);
            // }
        }
    }
    //-----------------------------------------------------------------
    int SymbolTree::GetItemIconIndex(const wxString& kind, const wxString& access)
    {
        wxString key(kind);
        int index(m_imagesMap["struct"]); // structs will be the default icon index

        if (!access.IsEmpty()) key += wxT("_") + access;

        key.Trim();

        std::map<wxString, int>::iterator iter = m_imagesMap.find(key);
        if (iter != m_imagesMap.end()) { index = iter->second; }
        return index;
    }
    //-----------------------------------------------------------------
    void SymbolTree::UpdateSymbols(const std::vector<std::pair<wxString, TagEntry> >& items)
    {
        if (m_tree)
        {
            Freeze();
            for (size_t i = 0; i < items.size(); i++) {
                wxString key = items[i].first;
                TagEntry data = items[i].second;

                UpdateGuiItem(data, key);
            }
            Thaw();
        }
    }
    //-----------------------------------------------------------------
    void SymbolTree::UpdateGuiItem(TagEntry& data, const wxString& key)
    {
        if (!m_tree) return;

        TagNode* node = m_tree->Find(key);
        if (node) {
            // Update the new data with the gui tree item id
            data.SetTreeItemId(node->GetData().GetTreeItemId());
            node->SetData(data);

            // Update Icon if needed
            int iconIndex = GetItemIconIndex(data.GetKind(), data.GetAccess());
            int curIconIndex = -1;
            wxTreeItemId itemId = node->GetData().GetTreeItemId();
            if (itemId.IsOk()) {
                curIconIndex = GetItemImage(itemId);
                if (curIconIndex != iconIndex) {
                    // Need to update the image as well
                    SetItemImage(node->GetData().GetTreeItemId(), iconIndex);
                    SetItemImage(node->GetData().GetTreeItemId(), iconIndex, wxTreeItemIcon_Selected);

                } // if(curIconIndex != iconIndex )
                // update the linenumber and file
                MyTreeItemData* item_data = new MyTreeItemData(data.GetFile(), data.GetPattern());
                wxTreeItemData* old_data = GetItemData(itemId);
                if (old_data) delete old_data;
                SetItemData(itemId, item_data);
            }
        }
    }
    //-----------------------------------------------------------------
    void SymbolTree::DeleteSymbols(const std::vector<std::pair<wxString, TagEntry> >& items)
    {
        if (!m_tree) return;

        std::map<void*, bool> deletedMap;

        Freeze();
        for (size_t i = 0; i < items.size(); i++) {
            wxString key = items.at(i).first;

            std::map<wxString, void*>::iterator iter = m_items.find(key);
            if (iter != m_items.end() && iter->second) {
                wxTreeItemId hti = iter->second;
                // if this note was already deleted, dont delete it again
                if (deletedMap.find(hti.m_pItem) == deletedMap.end()) {
                    GetItemChildrenRecursive(hti, deletedMap);
                    // remove just the parent
                    Delete(hti);
                }
                m_items.erase(iter);
            }
        }
        Thaw();
    }
    //-----------------------------------------------------------------
    void SymbolTree::GetItemChildrenRecursive(wxTreeItemId& parent, std::map<void*, bool>& deletedMap)
    {
        // delete the item's children
        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(parent, cookie);
        while (child.IsOk()) {
            if (ItemHasChildren(child)) {
                GetItemChildrenRecursive(child, deletedMap);
            }
            else {
                deletedMap[child.m_pItem] = child;
            }
            child = GetNextChild(parent, cookie);
        }
        deletedMap[parent.m_pItem] = true;
    }
    //-----------------------------------------------------------------
    void SymbolTree::AddSymbols(const std::vector<std::pair<wxString, TagEntry> >& items)
    {
        if (m_tree)
        {
            for (size_t i = 0; i < items.size(); i++) {
                TagEntry data = items.at(i).second;
                if (m_tree) {
                    TagNode* node = m_tree->AddEntry(data);
                    if (node) { AddItem(node); }
                }
            }
        }
    }
    //-----------------------------------------------------------------
    void SymbolTree::Clear()
    {
        // Clear the tree
        //Freeze();
        DeleteAllItems();
        m_items.clear();
        m_globalsNode = wxTreeItemId();
        m_prototypesNode = wxTreeItemId();
        m_macrosNode = wxTreeItemId();
        m_fileName.Clear();
        //Thaw();
    }
    //-----------------------------------------------------------------
    bool SymbolTree::Matches(const wxTreeItemId& item, const wxString& patter)
    {
        if (!item.IsOk()) return false;
        wxString displayName = GetItemText(item);
        wxString path = displayName.BeforeFirst(wxT('('));
        // Get the name from the path
        path = path.AfterLast(wxT(':'));
        if (FileUtil::FuzzyMatch(patter, path)) {
            SelectItem(item);
            EnsureVisible(item);
            return true;
        }

        // Check the item's children
        if (ItemHasChildren(item)) {
            wxTreeItemIdValue cookie;
            wxTreeItemId child = GetFirstChild(item, cookie);
            while (child.IsOk()) {
                if (Matches(child, patter)) { return true; }
                child = GetNextChild(item, cookie);
            }
        }
        return false;
    }
    //-----------------------------------------------------------------
    void SymbolTree::SelectFirstItem() 
    { 
        SelectItem(GetFirstVisibleItem()); 
    }
    //-----------------------------------------------------------------
}