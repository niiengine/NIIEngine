#include "gsgsTreeCtrl.h"
#include "my_tree_view.h"
#include "tree_item_data.h"
#include <wx/settings.h>

MyTreeView::MyTreeView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
    : gsgs::DefaultTreeCtrl(parent, id, pos, size, 0)
{
    TreeSortFunc SortFunc = [&](gsgs::RowEntry* itemA, gsgs::RowEntry* itemB) {
        ItemData* a = dynamic_cast<ItemData*>(itemA->GetClientObject());
        ItemData* b = dynamic_cast<ItemData*>(itemB->GetClientObject());
        if(a->IsFolder() && b->IsFile())
            return true;
        else if(b->IsFolder() && a->IsFile())
            return false;
        return (a->GetDisplayName().CmpNoCase(b->GetDisplayName()) < 0);
    };
    SetSortFunction(SortFunc);
}

MyTreeView::~MyTreeView() {}
