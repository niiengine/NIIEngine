#ifndef LANGUAGESERVERENTRY_H
#define LANGUAGESERVERENTRY_H

#include "gsgsConfig.h"
#include <gsgsStringUtil.h>
#include "LSPNetwork.h"

class LanguageServerEntry
{
    bool m_enabled = true;
    wxString m_name;
    wxString m_exepath;
    wxString m_args;
    wxString m_workingDirectory;
    wxArrayString m_languages;
    wxString m_connectionString;
    int m_priority = 50;
    gsgsStringSet m_unimplementedMethods;
    bool m_disaplayDiagnostics = true;

public:
    // use 'map' to keep the items sorted by name
    typedef std::map<wxString, LanguageServerEntry> Map_t;

    /**
     * @brief try to validate the LSP by checking that all paths do exists
     * @return
     */
    bool IsValid() const;

public:
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON() const;
    LanguageServerEntry();
    virtual ~LanguageServerEntry();

    const gsgsStringSet& GetUnimplementedMethods() const { return m_unimplementedMethods; }

    /**
     * @brief add unimplemented method to this LSP
     * @param methodName
     */
    void AddUnImplementedMethod(const wxString& methodName);

    LanguageServerEntry& SetDisaplayDiagnostics(bool disaplayDiagnostics)
    {
        this->m_disaplayDiagnostics = disaplayDiagnostics;
        return *this;
    }
    bool IsDisaplayDiagnostics() const { return m_disaplayDiagnostics; }
    LanguageServerEntry& SetArgs(const wxString& args)
    {
        this->m_args = args;
        return *this;
    }
    LanguageServerEntry& SetExepath(const wxString& exepath)
    {
        this->m_exepath = exepath;
        return *this;
    }
    LanguageServerEntry& SetConnectionString(const wxString& connectionString)
    {
        this->m_connectionString = connectionString;
        return *this;
    }
    const wxString& GetConnectionString() const { return m_connectionString; }
    const wxString& GetArgs() const { return m_args; }
    const wxString& GetExepath() const { return m_exepath; }
    LanguageServerEntry& SetPriority(int priority)
    {
        this->m_priority = priority;
        return *this;
    }
    int GetPriority() const { return m_priority; }
    LanguageServerEntry& SetEnabled(bool enabled)
    {
        this->m_enabled = enabled;
        return *this;
    }
    bool IsEnabled() const { return m_enabled; }
    LanguageServerEntry& SetLanguages(const wxArrayString& languages)
    {
        this->m_languages = languages;
        return *this;
    }
    const wxArrayString& GetLanguages() const { return m_languages; }
    LanguageServerEntry& SetWorkingDirectory(const wxString& workingDirectory)
    {
        this->m_workingDirectory = workingDirectory;
        return *this;
    }
    const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
    LanguageServerEntry& SetName(const wxString& name)
    {
        this->m_name = name;
        return *this;
    }
    const wxString& GetName() const { return m_name; }
    eNetworkType GetNetType() const;
};

#endif // LANGUAGESERVERENTRY_H
