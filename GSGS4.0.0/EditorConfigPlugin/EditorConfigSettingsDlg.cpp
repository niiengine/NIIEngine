#include "EditorConfigSettingsDlg.h"
#include "gsgsConfig.h"
#include "EditorConfigSettings.h"

EditorConfigSettingsDlg::EditorConfigSettingsDlg(wxWindow* parent)
    : EditorConfigSettingsDlgBase(parent)
{
    EditorConfigSettings settings;
    settings.Load();
    m_checkBoxEnable->SetValue(settings.IsEnabled());
}

EditorConfigSettingsDlg::~EditorConfigSettingsDlg() {}
