#include "php_configuration_data.h"
#include "php_utils.h"
#include "gsgsConfig.h"
#include "PHPSetterGetterEntry.h"
#include "globals.h"
#include "gsgsPathManager.h"

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
// PHPConfigurationData
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
PHPConfigurationData::PHPConfigurationData()
    : ObjectData("PHPConfigurationData")
    , m_xdebugPort(9000)
    , m_xdebugIdeKey("codeliteide")
    , m_xdebugHost("127.0.0.1")
    , m_findInFilesMask("*.php;*.inc;*.phtml;*.js;*.html;*.css;*.scss;*.json;*.xml;*.ini;*.md;*.txt;*.text;.htaccess;*."
                        "sql;.gitignore")
    , m_workspaceType(0)
    , m_settersGettersFlags(kSG_None)
{
    m_phpOptions.Load();
}
//------------------------------------------------------------------------------------------
PHPConfigurationData::~PHPConfigurationData() {}
//------------------------------------------------------------------------------------------
void PHPConfigurationData::FromJSON(const JSONObject& json)
{
    json.getObject("m_findInFilesMask").getValue(m_findInFilesMask, m_findInFilesMask);
    json.getObject("m_xdebugPort").getValue(m_xdebugPort, m_xdebugPort);
    json.getObject("m_xdebugHost").getValue(m_xdebugHost, m_xdebugHost);
    json.getObject("m_flags").getValue(m_flags, m_flags);
    m_settersGettersFlags = json.getObject("m_settersGettersFlags").getValue(m_settersGettersFlags);
    json.getObject("m_xdebugIdeKey").getValue(m_xdebugIdeKey, m_xdebugIdeKey);
    json.getObject("m_workspaceType").getValue(m_workspaceType, m_workspaceType);
    m_xdebugIdeKey.Trim().Trim(false);

    // xdebug IDE can not be an empty string, or else debugging in command line
    // will not work
    if(m_xdebugIdeKey.IsEmpty()) { m_xdebugIdeKey = "gsgside"; }
    json.getObject("m_ccIncludePath").getValue(m_ccIncludePath);
}
//------------------------------------------------------------------------------------------
JSONObject PHPConfigurationData::ToJSON() const
{
    JSONObject e = JSONObject::createObject(GetName());
    e.addProperty("m_xdebugPort", m_xdebugPort);
    e.addProperty("m_xdebugHost", m_xdebugHost);
    e.addProperty("m_ccIncludePath", m_ccIncludePath);
    e.addProperty("m_flags", m_flags);
    e.addProperty("m_xdebugIdeKey", m_xdebugIdeKey);
    e.addProperty("m_findInFilesMask", m_findInFilesMask);
    e.addProperty("m_workspaceType", m_workspaceType);
    e.addProperty("m_settersGettersFlags", m_settersGettersFlags);
    return e;
}
//------------------------------------------------------------------------------------------
wxString PHPConfigurationData::GetIncludePathsAsString() const
{
    wxString str;
    for(size_t i = 0; i < GetIncludePaths().GetCount(); i++) {
        str << GetIncludePaths().Item(i) << wxT("\n");
    }
    if(str.IsEmpty() == false) { str.RemoveLast(); }
    return str;
}
//------------------------------------------------------------------------------------------
wxString PHPConfigurationData::GetCCIncludePathsAsString() const
{
    wxString str;
    for(size_t i = 0; i < m_ccIncludePath.GetCount(); i++) {
        str << m_ccIncludePath.Item(i) << wxT("\n");
    }
    if(str.IsEmpty() == false) { str.RemoveLast(); }
    return str;
}
//------------------------------------------------------------------------------------------
PHPConfigurationData& PHPConfigurationData::Load()
{
    gsgs::Config conf("php.conf");
    conf.ReadItem(this);

    m_phpOptions.Load();
    return *this;
}
//------------------------------------------------------------------------------------------
void PHPConfigurationData::Save()
{
    gsgs::Config conf("php.conf");
    conf.WriteItem(this);

    m_phpOptions.Save();
}
//------------------------------------------------------------------------------------------