#ifndef LANGUAGESERVERCONFIG_H
#define LANGUAGESERVERCONFIG_H

#include "gsgsConfig.h"
#include "LanguageServerEntry.h"

class LanguageServerConfig : public ObjectData
{
public:
    enum eLSPFlags {
        kEnabaled = (1 << 0),
    };
public:
    virtual ~LanguageServerConfig();
    LanguageServerConfig& Load();
    LanguageServerConfig& Save();
    static LanguageServerConfig& Get();
    virtual void FromJSON(const JSONObject& json);
    virtual JSONObject ToJSON() const;
    LanguageServerConfig& SetFlags(size_t flags)
    {
        m_flags = flags;
        return *this;
    }
    size_t GetFlags() const { return m_flags; }
    bool IsEnabled() const { return HasFlag(kEnabaled); }
    void SetEnabled(bool b) { EnableFlag(kEnabaled, b); }
    LanguageServerConfig& SetServers(const LanguageServerEntry::Map_t& servers)
    {
        m_servers = servers;
        return *this;
    }
    const LanguageServerEntry::Map_t & GetServers() const { return m_servers; }
    LanguageServerEntry::Map_t& GetServers() { return m_servers; }
    
    LanguageServerEntry & GetServer(const wxString& name);
    const LanguageServerEntry & GetServer(const wxString& name) const;
    /**
     * @brief add server. erase an existing one with the same name
     */
    void AddServer(const LanguageServerEntry & server);
    
    /**
     * @brief delete an existing server by name
     */
    void RemoveServer(const wxString & name);
    LanguageServerConfig & SetNodejs(const wxString & nodejs)
    {
        m_nodejs = nodejs;
        return *this;
    }
    const wxString & GetNodejs() const { return m_nodejs; }
protected:
    LanguageServerConfig();

    bool HasFlag(eLSPFlags flag) const { return m_flags & flag; }
    void EnableFlag(eLSPFlags flag, bool b)
    {
        if(b) {
            m_flags |= flag;
        } else {
            m_flags &= ~flag;
        }
    }
protected:
    size_t m_flags = 0;
    LanguageServerEntry::Map_t m_servers;
    wxString m_nodejs;
};

#endif // LANGUAGESERVERCONFIG_H
