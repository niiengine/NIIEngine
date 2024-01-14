#include "WebToolsConfig.h"
#include "gsgsJSON.h"
#include "NodeJSLocator.h"
#include <set>
#include <algorithm>
#include <vector>

WebToolsConfig::WebToolsConfig()
    : ObjectData("WebTools")
    , m_jsFlags(kJSEnableCC | kJSLibraryBrowser | kJSLibraryEcma5 | kJSLibraryEcma6 | kJSPluginNode | kJSNodeExpress)
    , m_xmlFlags(kXmlEnableCC)
    , m_htmlFlags(kHtmlEnableCC)
    , m_nodeOptions(0)
{
}

WebToolsConfig::~WebToolsConfig() {}

WebToolsConfig& WebToolsConfig::Load()
{
    gsgs::Config conf("WebTools.conf");
    conf.ReadItem(this);
    return *this;
}

WebToolsConfig& WebToolsConfig::SaveConfig()
{
    gsgs::Config conf("WebTools.conf");
    conf.WriteItem(this);
    return *this;
}

void WebToolsConfig::FromJSON(const gsgs::JSONObject& json)
{
    json.getObject("m_jsFlags").getValue(m_jsFlags, m_jsFlags);
    json.getObject("m_xmlFlags").getValue(m_xmlFlags, m_xmlFlags);
    json.getObject("m_htmlFlags").getValue(m_htmlFlags, m_htmlFlags);
    json.getObject("m_nodeOptions").getValue(m_nodeOptions, m_nodeOptions);
    json.getObject("m_portNumber").getValue(m_portNumber, m_portNumber);

    wxString v;
    json.getObject("m_nodejs").getValue(v);
    if(!v.IsEmpty() && wxFileName::FileExists(v)) { m_nodejs = v; }
    json.getObject("m_npm").getValue(v);
    if(!v.IsEmpty() && wxFileName::FileExists(v)) { m_npm = v; }
}

gsgs::JSONObject WebToolsConfig::ToJSON() const
{
    gsgs::JSONObject element = gsgs::JSONObject::createObject(GetName());
    element.addProperty("m_jsFlags", m_jsFlags);
    element.addProperty("m_xmlFlags", m_xmlFlags);
    element.addProperty("m_htmlFlags", m_htmlFlags);
    element.addProperty("m_nodejs", m_nodejs);
    element.addProperty("m_npm", m_npm);
    element.addProperty("m_portNumber", m_portNumber);
    return element;
}

wxString WebToolsConfig::GetTernProjectFile() const
{
    gsgs::JSON root(cJSON_Object);
    gsgs::JSONObject libs = gsgs::JSONObject::createArray("libs");
    root.getObject().arrayAppend(libs);

    if(m_jsFlags & kJSLibraryBrowser) libs.arrayAppend("browser");
    if(m_jsFlags & kJSLibraryChai) libs.arrayAppend("chai");
    if(m_jsFlags & kJSLibraryEcma5) libs.arrayAppend("ecma5");
    if(m_jsFlags & kJSLibraryEcma6) libs.arrayAppend("ecma6");
    if(m_jsFlags & kJSLibraryJQuery) libs.arrayAppend("jquery");
    if(m_jsFlags & kJSLibraryUnderscore) libs.arrayAppend("underscore");
    if(m_jsFlags & kJSPluginQML) libs.arrayAppend("qml");

    gsgs::JSONObject plugins = gsgs::JSONObject::createObject("plugins");
    root.getObject().arrayAppend(plugins);

    std::vector<wxString> pluginsToLoad;

    // basic plugins that should always get loaded
    pluginsToLoad.push_back("commonjs");
    pluginsToLoad.push_back("modules");

    if(m_jsFlags & kJSPluginNode) {
        pluginsToLoad.push_back("node_resolve");
        pluginsToLoad.push_back("node");
    }

    if(m_jsFlags & kJSPluginRequireJS) { pluginsToLoad.push_back("requirejs"); }

    if(m_jsFlags & kJSPluginStrings) { pluginsToLoad.push_back("complete_strings"); }

    if(m_jsFlags & kJSPluginAngular) { pluginsToLoad.push_back("angular"); }

    if(m_jsFlags & kJSWebPack) { pluginsToLoad.push_back("webpack"); }

    if(m_jsFlags & kJSNodeExpress) {
        pluginsToLoad.push_back("node_resolve");
        pluginsToLoad.push_back("node");
        pluginsToLoad.push_back("node-express");
    }

    std::set<wxString> uniquePlugins;
    std::for_each(pluginsToLoad.begin(), pluginsToLoad.end(), [&](const wxString& name) {
        if(uniquePlugins.count(name) == 0) {
            uniquePlugins.insert(name);
            gsgs::JSONObject node = gsgs::JSONObject::createObject(name);
            plugins.arrayAppend(node);
        }
    });
    return root.getObject().getString();
}

WebToolsConfig& WebToolsConfig::Get()
{
    static WebToolsConfig webtoolsConfig;
    return webtoolsConfig;
}

bool WebToolsConfig::IsNodeInstalled() const
{
    wxFileName fn(GetNodejs());
    return fn.IsOk() && fn.FileExists();
}

bool WebToolsConfig::IsNpmInstalled() const
{
    wxFileName fn(GetNpm());
    return fn.IsOk() && fn.FileExists();
}

wxFileName WebToolsConfig::GetTernScript() const
{
    wxFileName fn(GetTempFolder(false), "tern");
    fn.AppendDir("node_modules");
    fn.AppendDir("tern");
    fn.AppendDir("bin");
    return fn;
}

bool WebToolsConfig::IsTernInstalled() const { return GetTernScript().FileExists(); }

wxString WebToolsConfig::GetTempFolder(bool create) const
{
    wxFileName fn(gsgs_Path().GetUserDataDir(), "");
    fn.AppendDir("webtools");
    if(create) { fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL); }
    return fn.GetPath();
}
