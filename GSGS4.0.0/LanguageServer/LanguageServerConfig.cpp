#include "LanguageServerConfig.h"
#include <algorithm>
#include <NodeJSLocator.h>

LanguageServerConfig::LanguageServerConfig()
    : ObjectData("LSPConfig")
{
    NodeJSLocator locator;
    locator.Locate();

    m_nodejs = locator.GetNodejs();
}

LanguageServerConfig::~LanguageServerConfig() {}

LanguageServerConfig& LanguageServerConfig::Load()
{
    gsgs::Config conf("LanguageServer.conf");
    conf.ReadItem(this);
    return *this;
}

LanguageServerConfig& LanguageServerConfig::Save()
{
    gsgs::Config conf("LanguageServer.conf");
    conf.WriteItem(this);
    return *this;
}

LanguageServerConfig& LanguageServerConfig::Get()
{
    static LanguageServerConfig config;
    return config;
}

void LanguageServerConfig::FromJSON(const JSONObject& json)
{
    m_servers.clear();
    json.getObject("flags").getValue(m_flags, m_flags);
    json.getObject("nodejs").getValue(m_nodejs, m_nodejs);
    if(json.isObjectExist("servers")) {
        JSONObject servers = json.getObject("servers");
        size_t count = servers.getArraySize();
        for(size_t i = 0; i < count; ++i) {
            JSONObject server = servers.getArray(i);
            LanguageServerEntry entry;
            entry.FromJSON(server);
            m_servers.insert({ entry.GetName(), entry });
        }
    }
}

JSONObject LanguageServerConfig::ToJSON() const
{
    JSONObject json = JSONObject::createObject(GetName());
    json.addProperty("flags", m_flags);
    json.addProperty("nodejs", m_nodejs);
    JSONObject servers = JSONObject::createArray("servers");
    std::for_each(m_servers.begin(), m_servers.end(),
                  [&](const LanguageServerEntry::Map_t::value_type& vt) { servers.arrayAppend(vt.second.ToJSON()); });
    json.arrayAppend(servers);
    return json;
}

void LanguageServerConfig::AddServer(const LanguageServerEntry& server)
{
    RemoveServer(server.GetName());
    m_servers.insert({ server.GetName(), server });
}

void LanguageServerConfig::RemoveServer(const wxString& name)
{
    if(m_servers.count(name)) { m_servers.erase(name); }
}

const LanguageServerEntry& LanguageServerConfig::GetServer(const wxString& name) const
{
    static LanguageServerEntry NullEntry;
    if(m_servers.count(name) == 0) { return NullEntry; }
    return m_servers.find(name)->second;
}

LanguageServerEntry& LanguageServerConfig::GetServer(const wxString& name)
{
    static LanguageServerEntry NullEntry;
    if(m_servers.count(name) == 0) { return NullEntry; }
    return m_servers[name];
}
