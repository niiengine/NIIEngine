#ifndef SMARTCOMPLETIONSCONFIG_H
#define SMARTCOMPLETIONSCONFIG_H

#include "SmartCompletionUsageDB.h"
#include "gsgsConfig.h"
#include "gsgsStringUtil.h"

using namespace gsgs;
class SmartCompletionsConfig : public ObjectData
{
public:
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON() const;

    enum eOptions {
        kEnabled = (1 << 0),
    };
public:
    SmartCompletionsConfig();
    virtual ~SmartCompletionsConfig();

    SmartCompletionsConfig& Load();
    SmartCompletionsConfig& Save();

    bool IsEnabled() const { return m_flags & kEnabled; }
    void SetEnabled(bool b) { b ? m_flags |= kEnabled : m_flags &= ~kEnabled; }
    std::unordered_map<wxString, int>& GetCCWeightTable() { return m_CCweight; }
    std::unordered_map<wxString, int>& GetGTAWeightTable() { return m_GTAweight; }
    SmartCompletionUsageDB& GetUsageDb() { return m_db; }
protected:
    size_t m_flags;
    std::unordered_map<wxString, int> m_CCweight;
    std::unordered_map<wxString, int> m_GTAweight;
    SmartCompletionUsageDB m_db;
};

#endif // SMARTCOMPLETIONSCONFIG_H
