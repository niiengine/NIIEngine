#include "gsgsFileViwerTreeCtrl.h"

namespace gsgs
{
    FileViewerTreeCtrl::FileViewerTreeCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size,
                                               long style)
        : gsgs::DefaultTreeCtrl(parent, id, pos, size, (style & ~wxTR_FULL_ROW_HIGHLIGHT))
    {
        std::function<bool(const wxTreeItemId&, const wxTreeItemId&)> SortFunc = [&](const wxTreeItemId& itemA,
                                                                                     const wxTreeItemId& itemB) {
            TreeCtrlData* a = static_cast<TreeCtrlData*>(GetItemData(itemA));
            TreeCtrlData* b = static_cast<TreeCtrlData*>(GetItemData(itemB));
            if(a->IsFolder() && b->IsFile())
                return true;
            else if(b->IsFolder() && a->IsFile())
                return false;
            // same kind
            return (a->GetName().CmpNoCase(b->GetName()) < 0);
        };
        SetSortFunction(SortFunc);
    }

    FileViewerTreeCtrl::~FileViewerTreeCtrl() {}
}