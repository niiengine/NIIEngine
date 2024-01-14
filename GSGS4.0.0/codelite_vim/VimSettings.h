#ifndef VIMSETTINGS_H
#define VIMSETTINGS_H

#include "gsgsConfig.h"

class VimSettings : public gsgs::ObjectData
{
    bool m_enabled;

public:
    VimSettings();
    virtual ~VimSettings();

public:
    virtual void FromJSON(const gsgs::JSONObject& json);
    virtual gsgs::JSONObject ToJSON() const;

    VimSettings& Load();
    VimSettings& Save();
    VimSettings& SetEnabled(bool enabled)
    {
        this->m_enabled = enabled;
        return *this;
    }
    bool IsEnabled() const { return m_enabled; }
};

#endif // VIMSETTINGS_H
