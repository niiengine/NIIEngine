#include "WordCompletionSettings.h"

WordCompletionSettings::WordCompletionSettings()
    : gsgs::ObjectData("WordCompletionSettings")
    , m_comparisonMethod(kComparisonStartsWith)
    , m_enabled(true)
{
}

WordCompletionSettings::~WordCompletionSettings() {}

void WordCompletionSettings::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("m_comparisonMethod").getValue(m_comparisonMethod, m_comparisonMethod);
    json.getObject("m_enabled").getValue(m_enabled, m_enabled);
}

gsgs::JSONObject WordCompletionSettings::ToJSON() const
{
    gsgs::JSONObject element = gsgs::JSONObject::createObject(GetName());
    element.addProperty("m_comparisonMethod", m_comparisonMethod);
    element.addProperty("m_enabled", m_enabled);
    return element;
}

WordCompletionSettings& WordCompletionSettings::Load()
{
    gsgs::Config config("word-completion.conf");
    config.ReadItem(this);
    return *this;
}

WordCompletionSettings& WordCompletionSettings::Save()
{
    gsgs::Config config("word-completion.conf");
    config.WriteItem(this);
    return *this;
}
