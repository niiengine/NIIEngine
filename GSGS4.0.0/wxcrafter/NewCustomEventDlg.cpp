#include "NewCustomEventDlg.h"
#include <gsgsWindowManager.h>

NewCustomEventDlg::NewCustomEventDlg(wxWindow* parent)
    : NewCustomEventBaseDlg(parent)
{
    SetName("NewCustomEventDlg");
    gsgs::WindowManager::Load(this);
}

NewCustomEventDlg::~NewCustomEventDlg() {}
