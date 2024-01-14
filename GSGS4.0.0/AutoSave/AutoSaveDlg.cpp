#include "AutoSaveDlg.h"

AutoSaveSettings::AutoSaveSettings()
    : ObjectData("auto-save")
    , m_flags(0)
    , m_checkInterval(5)
{
}

AutoSaveSettings::~AutoSaveSettings() {}

void AutoSaveSettings::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("m_flags").getValue(m_flags, m_flags);
    json.getObject("m_checkInterval").getValue(m_checkInterval, m_checkInterval);
}

gsgs::JSONObject AutoSaveSettings::ToJSON() const
{
    gsgs::JSONObject json = gsgs::JSONObject::createObject(GetName());
    json.addProperty("m_flags", m_flags);
    json.addProperty("m_checkInterval", m_checkInterval);
    return json;
}

AutoSaveSettings AutoSaveSettings::Load()
{
    AutoSaveSettings settings;
    gsgs::Config config("auto-save.conf");
    config.ReadItem(&settings);
    return settings;
}

void AutoSaveSettings::Save(const AutoSaveSettings& settings)
{
    gsgs::Config config("auto-save.conf");
    config.WriteItem(&settings);
}

AutoSaveDlg::AutoSaveDlg(wxWindow* parent)
    : AutoSaveDlgBase(parent)
{
    AutoSaveSettings conf = AutoSaveSettings::Load();
    m_checkBoxEnabled->SetValue(conf.HasFlag(AutoSaveSettings::kEnabled));
    m_spinCtrlInterval->SetValue(conf.GetCheckInterval());
}

AutoSaveDlg::~AutoSaveDlg() {}

void AutoSaveDlg::OnEnabledUI(wxUpdateUIEvent& event) { event.Enable(m_checkBoxEnabled->IsChecked()); }

void AutoSaveDlg::OnOK(wxCommandEvent& event)
{
    AutoSaveSettings conf;
    conf.EnableFlag(AutoSaveSettings::kEnabled, m_checkBoxEnabled->IsChecked());
    conf.SetCheckInterval(m_spinCtrlInterval->GetValue());
    AutoSaveSettings::Save(conf);
    
    EndModal(wxID_OK);
}
