
#include "PHPEntityFunction.h"
#include "PHPEntityVariable.h"
#include "PHPOutlineTree.h"
#include "PHPSourceFile.h"
#include "gsgsStyleManager.h"
#include "gsgsFileUtil.h"
#include "globals.h"
#include "gsgsNavigationManager.h"
#include <algorithm>
#include <globals.h>
#include <ieditor.h>
#include <imanager.h>
#include <wx/wupdlock.h>

class QItemData : public wxTreeItemData
{
public:
    PHPEntityBase::Ptr_t m_entry;

public:
    QItemData(PHPEntityBase::Ptr_t entry)
        : m_entry(entry)
    {
    }
    virtual ~QItemData() {}
};

PHPOutlineTree::PHPOutlineTree(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : gsgs::DefaultTreeCtrl(parent, id, pos, size, style)
{
    SetBitmaps(&gsgs_Image().GetBitmaps());
}

PHPOutlineTree::~PHPOutlineTree() {}

int PHPOutlineTree::GetImageId(PHPEntityBase::Ptr_t entry)
{
    gsgs::ImageManager* bmpLoader = &gsgs_Image();
    if(entry->Is(kEntityTypeFunction)) {
        PHPEntityFunction* func = entry->Cast<PHPEntityFunction>();

        if(func->HasFlag(kFunc_Private))
            return bmpLoader->GetMimeImageId(gsgs::ImageManager::kFunctionPrivate);
        else if(func->HasFlag(kFunc_Protected))
            return bmpLoader->GetMimeImageId(gsgs::ImageManager::kFunctionProtected);
        else
            // public
            return bmpLoader->GetMimeImageId(gsgs::ImageManager::kFunctionPublic);

    } else if(entry->Is(kEntityTypeVariable)) {
        PHPEntityVariable* var = entry->Cast<PHPEntityVariable>();
        if(!var->IsMember() && !var->IsConst()) {
            // A global variale
            return bmpLoader->GetMimeImageId(gsgs::ImageManager::kMemberPublic);

        } else if(var->IsMember()) {
            if(var->HasFlag(kVar_Const)) return bmpLoader->GetMimeImageId(gsgs::ImageManager::kConstant); // constant
            // Member
            if(var->HasFlag(kVar_Private))
                return bmpLoader->GetMimeImageId(gsgs::ImageManager::kMemberPrivate);
            else if(var->HasFlag(kVar_Protected))
                return bmpLoader->GetMimeImageId(gsgs::ImageManager::kMemberProtected);
            else
                return bmpLoader->GetMimeImageId(gsgs::ImageManager::kMemberPublic);

        } else if(var->IsConst()) {
            // Constant
            return bmpLoader->GetMimeImageId(gsgs::ImageManager::kConstant);
        } else {
            return bmpLoader->GetMimeImageId(gsgs::ImageManager::kMemberPublic);
        }

    } else if(entry->Is(kEntityTypeNamespace)) {
        // Namespace
        return bmpLoader->GetMimeImageId(gsgs::ImageManager::kNamespace);
    } else if(entry->Is(kEntityTypeClass)) {
        return bmpLoader->GetMimeImageId(gsgs::ImageManager::kClass);
    }
    return wxNOT_FOUND; // Unknown
}

void PHPOutlineTree::BuildTree(const wxFileName& filename)
{
    m_filename = filename;
    PHPSourceFile sourceFile(filename, NULL);
    sourceFile.SetParseFunctionBody(false);
    sourceFile.Parse();
    wxWindowUpdateLocker locker(this);
    DeleteAllItems();

    wxTreeItemId root = AddRoot(wxT("Root"));

    // Build the tree view
    BuildTree(root, sourceFile.Namespace());

    if(HasChildren(GetRootItem())) { ExpandAll(); }
}

void PHPOutlineTree::Clear()
{
    wxWindowUpdateLocker locker(this);
    DeleteAllItems();
    m_filename.Clear();
}

void PHPOutlineTree::BuildTree(wxTreeItemId parentTreeItem, PHPEntityBase::Ptr_t entity)
{
    int imgID = GetImageId(entity);
    wxTreeItemId parent = AppendItem(parentTreeItem, entity->GetDisplayName(), imgID, imgID, new QItemData(entity));
    // dont add the children of the function (i.e. function arguments)
    if(entity->Is(kEntityTypeFunction)) return;
    const PHPEntityBase::List_t& children = entity->GetChildren();
    if(!children.empty()) {
        PHPEntityBase::List_t::const_iterator iter = children.begin();
        for(; iter != children.end(); ++iter) {
            BuildTree(parent, *iter);
        }
    }
}

void PHPOutlineTree::ItemSelected(const wxTreeItemId& item, bool focusEditor)
{
    QItemData* itemData = dynamic_cast<QItemData*>(GetItemData(item));
    CHECK_PTR_RET(itemData);

    IEditor* editor = m_manager->GetActiveEditor();
    CHECK_PTR_RET(editor);

    // Define the pattern to search

    editor->FindAndSelect(itemData->m_entry->GetShortName(), itemData->m_entry->GetShortName(),
                          editor->PosFromLine(itemData->m_entry->GetLine()), NavMgr::Get());
    // set the focus to the editor
    if(focusEditor) { CallAfter(&PHPOutlineTree::SetEditorActive, editor); }
}

void PHPOutlineTree::SetEditorActive(IEditor* editor) { editor->SetActive(); }

bool PHPOutlineTree::Select(const wxString& pattern)
{
    wxTreeItemId item = DoFind(pattern, GetRootItem());
    if(item.IsOk()) {
        // select this item
        EnsureVisible(item);
        SelectItem(item);
        return true;
    }
    return false;
}

wxTreeItemId PHPOutlineTree::DoFind(const wxString& pattern, const wxTreeItemId& parent)
{
    if((GetRootItem() != parent) && FileUtil::FuzzyMatch(pattern, GetItemText(parent))) { return parent; }
    if(ItemHasChildren(parent)) {
        wxTreeItemIdValue cookie;
        wxTreeItemId child = GetFirstChild(parent, cookie);
        while(child.IsOk()) {
            wxTreeItemId match = DoFind(pattern, child);
            if(match.IsOk()) { return match; }
            child = GetNextChild(parent, cookie);
        }
    }
    return wxTreeItemId();
}
