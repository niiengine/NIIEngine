

#ifndef WORDCOMPLETIONSETTINGS_H
#define WORDCOMPLETIONSETTINGS_H

#include "gsgsConfig.h"

class WordCompletionSettings : public gsgs::ObjectData
{
public:
    enum {
        kComparisonStartsWith = 0,
        kComparisonContains = 1,
    };
public:
    WordCompletionSettings();
    virtual ~WordCompletionSettings();

    virtual void FromJSON(const gsgs::JSONObject& json);
    virtual gsgs::JSONObject ToJSON() const;

    void SetComparisonMethod(int comparisonMethod) { this->m_comparisonMethod = comparisonMethod; }
    int GetComparisonMethod() const { return m_comparisonMethod; }

    void SetEnabled(bool enabled) { this->m_enabled = enabled; }
    bool IsEnabled() const { return m_enabled; }
    
    WordCompletionSettings& Load();
    WordCompletionSettings& Save();
private:
    int m_comparisonMethod;
    bool m_enabled;
};

#endif // WORDCOMPLETIONSETTINGS_H
