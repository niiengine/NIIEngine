#include "VimSettings.h"

VimSettings::VimSettings()
    : ObjectData("vim")
    , m_enabled(false)
{
}

VimSettings::~VimSettings() {}

void VimSettings::FromJSON(const gsgs::JSONObject & json) 
{ 
    json.getObject("enabled").getValue(m_enabled, m_enabled);
}

gsgs::JSONObject VimSettings::ToJSON() const
{
    gsgs::JSONObject element = gsgs::JSONObject::createObject(GetName());
    element.addProperty("enabled", m_enabled);
    return element;
}

VimSettings& VimSettings::Load()
{
    gsgs::Config config("vim.conf");
    config.ReadItem(this);
    return *this;
}

VimSettings& VimSettings::Save()
{
    gsgs::Config config("vim.conf");
    config.WriteItem(this);
    return *this;
}
