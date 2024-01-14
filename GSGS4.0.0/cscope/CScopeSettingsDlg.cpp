#include "CScopeSettingsDlg.h"
#include "gsgsWindowManager.h"
#include "csscopeconfdata.h"
#include "gsgsEditorConfig.h"

CScopeSettingsDlg::CScopeSettingsDlg(wxWindow* parent)
    : CScopeSettingsDlgBase(parent)
{
    CScopeConfData settings;
    gsgs_EditorConfig().ReadObject("CscopeSettings", &settings);

    m_filePickerCScopeExe->SetPath(settings.GetCscopeExe());
    SetName("CScopeSettingsDlg");
    gsgs::WindowManager::Load(this);
}

CScopeSettingsDlg::~CScopeSettingsDlg() {}
