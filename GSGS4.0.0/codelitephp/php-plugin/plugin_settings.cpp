#include "plugin_settings.h"

PluginSettings::PluginSettings()
    : gsgs::ObjectData("php-plugin")
{
}

PluginSettings::~PluginSettings()
{
}

void PluginSettings::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("m_openWorkspaceFilter").getValue(m_openWorkspaceFilter);
}

gsgs::JSONObject PluginSettings::ToJSON() const
{
    gsgs::JSONObject e = gsgs::JSONObject::createObject(GetName());
    e.addProperty("m_openWorkspaceFilter", m_openWorkspaceFilter);
    return e;
}

void PluginSettings::Load(PluginSettings& settings)
{
    gsgs_Config().ReadItem( &settings );
}

void PluginSettings::Save(const PluginSettings& settings)
{
    gsgs_Config().WriteItem( &settings );
}
