
#include "zn_config_item.h"

znConfigItem::znConfigItem()
    : gsgs::ObjectData("ZoomNavigator")
    , m_highlightColour("LIGHT GREY")
    , m_enabled(false)
    , m_zoomFactor(-10)
    , m_useScrollbar(true)
{
}

znConfigItem::~znConfigItem() {}

void znConfigItem::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("m_highlightColour").getValue(m_highlightColour);
    json.getObject("m_enabled").getValue(m_enabled, m_enabled);
    json.getObject("m_zoomFactor").getValue(m_zoomFactor, m_zoomFactor);
    json.getObject("m_useScrollbar").getValue(m_useScrollbar, m_useScrollbar);
}

gsgs::JSONObject znConfigItem::ToJSON() const
{
    gsgs::JSONObject element = gsgs::JSONObject::createObject(GetName());
    element.addProperty("m_highlightColour", m_highlightColour);
    element.addProperty("m_enabled", m_enabled);
    element.addProperty("m_zoomFactor", m_zoomFactor);
    element.addProperty("m_useScrollbar", m_useScrollbar);
    return element;
}
