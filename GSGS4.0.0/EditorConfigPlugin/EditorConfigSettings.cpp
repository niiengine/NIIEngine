#include "EditorConfigSettings.h"
#include "gsgsConfig.h"

EditorConfigSettings::EditorConfigSettings()
    : m_flags(kEnabled)
{
}

EditorConfigSettings::~EditorConfigSettings() {}

EditorConfigSettings& EditorConfigSettings::Load()
{
    gsgs_Config().Read("gsgs::Config/Flags", m_flags, m_flags);
    return *this;
}

EditorConfigSettings& EditorConfigSettings::Save()
{
    gsgs_Config().Write("gsgs::Config/Flags", m_flags);
    return *this;
}
