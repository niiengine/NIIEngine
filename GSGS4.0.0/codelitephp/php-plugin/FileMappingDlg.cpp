#include "FileMappingDlg.h"
#include <gsgsWindowManager.h>

FileMappingDlg::FileMappingDlg(wxWindow* parent)
    : FileMappingDlgBase(parent)
{
    SetName("FileMappingDlg");
    gsgs::WindowManager::Load(this);
}

FileMappingDlg::~FileMappingDlg() {}

void FileMappingDlg::OnOkUI(wxUpdateUIEvent& event)
{
    event.Enable(!m_dirPickerSource->GetPath().IsEmpty() && !m_textCtrlRemote->IsEmpty());
}
