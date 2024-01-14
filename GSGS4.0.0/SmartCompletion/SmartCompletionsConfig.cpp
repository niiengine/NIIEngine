#include "SmartCompletionsConfig.h"

SmartCompletionsConfig::SmartCompletionsConfig()
    : ObjectData("settings")
    , m_flags(kEnabled)
{
}

SmartCompletionsConfig::~SmartCompletionsConfig() {}

SmartCompletionsConfig& SmartCompletionsConfig::Load()
{
    gsgs::Config conf("SmartCompletions.conf");
    conf.ReadItem(this);
    m_db.Open();
    m_db.LoadCCUsageTable(m_CCweight);
    m_db.LoadGTAUsageTable(m_GTAweight);
    return *this;
}

SmartCompletionsConfig& SmartCompletionsConfig::Save()
{
    gsgs::Config conf("SmartCompletions.conf");
    conf.WriteItem(this);
    return *this;
}

void SmartCompletionsConfig::FromJSON(const JSONObject& json)
{
    JSONObject e = json.getObject(GetName());
    e.getObject("m_flags").getValue(m_flags, m_flags);
}

JSONObject SmartCompletionsConfig::ToJSON() const
{
    JSONObject json = JSONObject::createObject(GetName());
    json.addProperty("m_flags", m_flags);
    return json;
}
