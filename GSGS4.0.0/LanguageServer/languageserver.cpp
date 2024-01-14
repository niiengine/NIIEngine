#include "languageserver.h"
#include <wx/xrc/xmlres.h>
#include "ieditor.h"
#include <wx/stc/stc.h>
#include "gsgsEvent.h"
#include <gsgsPreInclude.h>
#include "globals.h"
#include "LanguageServerConfig.h"
#include "LanguageServerSettingsDlg.h"
#include "gsgsPathManager.h"

//-----------------------------------------------------------------------------------
static LanguageServerPlugin* thePlugin = NULL;

//-----------------------------------------------------------------------------------
CL_PLUGIN_API Plugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) { thePlugin = new LanguageServerPlugin(manager); }
    return thePlugin;
}
//-----------------------------------------------------------------------------------
CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran"));
    info.SetName(wxT("LanguageServerPlugin"));
    info.SetDescription(_("Support for Language Server Protocol (LSP)"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}
//-----------------------------------------------------------------------------------
CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }
//-----------------------------------------------------------------------------------
LanguageServerPlugin::LanguageServerPlugin(IManager* manager)
    : Plugin(manager)
{
    m_longName = _("Support for Language Server Protocol (LSP)");
    m_shortName = wxT("LanguageServerPlugin");

    // Load the configuration
    LanguageServerConfig::Get().Load();
    m_servers.reset(new LanguageServerCluster());
}
//-----------------------------------------------------------------------------------
LanguageServerPlugin::~LanguageServerPlugin() {}
//-----------------------------------------------------------------------------------
void LanguageServerPlugin::CreateToolBar(gsgs::ToolBar* toolbar)
{
    // You can add items to the main toolbar here
    wxUnusedVar(toolbar);
}
//-----------------------------------------------------------------------------------
void LanguageServerPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    menu->Append(XRCID("language-server-settings"), _("Settings"));
    menu->Bind(wxEVT_MENU, &LanguageServerPlugin::OnSettings, this, XRCID("language-server-settings"));
    pluginsMenu->Append(wxID_ANY, _("Language Server"), menu);
}
//-----------------------------------------------------------------------------------
void LanguageServerPlugin::UnPlug()
{
    LanguageServerConfig::Get().Save();
    m_servers.reset(nullptr);
}
//-----------------------------------------------------------------------------------
void LanguageServerPlugin::OnSettings(wxCommandEvent& e)
{
    LanguageServerSettingsDlg dlg(gsgs_Event().TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        // restart all language servers
        dlg.Save();
        m_servers->Reload();
    }
}
//-----------------------------------------------------------------------------------