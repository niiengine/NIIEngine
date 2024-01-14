#include "LanguageServerEntry.h"
#include "LanguageServerProtocol.h"

LanguageServerEntry::LanguageServerEntry()
    : m_connectionString("stdio")
{
}

LanguageServerEntry::~LanguageServerEntry() {}

void LanguageServerEntry::FromJSON(const JSONObject& json)
{
    json.getObject("name").getValue(m_name);
    json.getObject("exepath").getValue(m_exepath);
    json.getObject("args").getValue(m_args);
    json.getObject("workingDirectory").getValue(m_workingDirectory);
    json.getObject("languages").getValue(m_languages);
    json.getObject("enabled").getValue(m_enabled, m_enabled);
    json.getObject("connectionString").getValue(m_connectionString, "stdio");
    json.getObject("priority").getValue(m_priority, m_priority);
    json.getObject("displayDiagnostics").getValue(m_disaplayDiagnostics, m_disaplayDiagnostics); // defaults to true
    m_unimplementedMethods.clear();
    wxArrayString methods;
    json.getObject("unimplementedMethods").getValue(methods);
    for(const wxString& methodName : methods) {
        m_unimplementedMethods.insert(methodName);
    }
}

JSONObject LanguageServerEntry::ToJSON() const
{
    JSONObject json = JSONObject::createObject();
    json.addProperty("name", m_name);
    json.addProperty("exepath", m_exepath);
    json.addProperty("args", m_args);
    json.addProperty("languages", m_languages);
    json.addProperty("enabled", m_enabled);
    json.addProperty("workingDirectory", m_workingDirectory);
    json.addProperty("connectionString", m_connectionString);
    json.addProperty("priority", m_priority);
    json.addProperty("displayDiagnostics", m_disaplayDiagnostics);
    wxArrayString methods;
    methods.Alloc(m_unimplementedMethods.size());
    for(const wxString& methodName : m_unimplementedMethods) {
        methods.Add(methodName);
    }

    json.addProperty("unimplementedMethods", methods);
    return json;
}

eNetworkType LanguageServerEntry::GetNetType() const
{
    wxString connectionString = GetConnectionString();
    connectionString.Trim().Trim(false);
    if(connectionString.CmpNoCase("stdio") == 0) {
        return eNetworkType::kStdio;
    } else {
        return eNetworkType::kTcpIP;
    }
}

bool LanguageServerEntry::IsValid() const
{
    bool is_valid = true;
    if(m_name.IsEmpty()) { return false; }

    wxFileName exePath(m_exepath);
    if(exePath.IsAbsolute() && !exePath.FileExists()) { return false; }
    wxFileName wd(m_workingDirectory, "");
    if(wd.IsAbsolute() && !wd.DirExists()) { return false; }
    return true;
}

void LanguageServerEntry::AddUnImplementedMethod(const wxString& methodName)
{
    m_unimplementedMethods.insert(methodName);
}
