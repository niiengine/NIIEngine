

#ifndef ZNCONFIGITEM_H
#define ZNCONFIGITEM_H

#include "gsgsConfig.h"

class znConfigItem : public gsgs::ObjectData
{
public:
    znConfigItem(); 
    virtual ~znConfigItem();

    virtual void FromJSON(const gsgs::JSONObject& json);
    virtual gsgs::JSONObject ToJSON() const;

    void SetEnabled(bool enabled) { m_enabled = enabled; }
    void SetHighlightColour(const wxString& highlightColour) { m_highlightColour = highlightColour; }
    bool IsEnabled() const { return m_enabled; }
    const wxString& GetHighlightColour() const { return m_highlightColour; }
    void SetZoomFactor(int zoomFactor) { this->m_zoomFactor = zoomFactor; }
    int GetZoomFactor() const { return m_zoomFactor; }
    void SetUseScrollbar(bool useScrollbar) { this->m_useScrollbar = useScrollbar; }
    bool IsUseScrollbar() const { return m_useScrollbar; }
private:
    wxString m_highlightColour;
    bool m_enabled;
    int m_zoomFactor;
    bool m_useScrollbar;
};

#endif // ZNCONFIGITEM_H
