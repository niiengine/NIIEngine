#include "VimSettings.h"
#include "VimSettingsDlg.h"
#include "codelite_vim.h"
#include "gsgsEvent.h"
#include "gsgsPreInclude.h"
#include "vim_manager.h"
#include <iostream>
#include <wx/app.h>
#include <wx/dialog.h>
#include <wx/menu.h>
#include <wx/settings.h>
#include <wx/xrc/xmlres.h>

//-----------------------------------------------------------------------------------
static CodeliteVim* thePlugin = NULL;
//-----------------------------------------------------------------------------------
CL_PLUGIN_API Plugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) { thePlugin = new CodeliteVim(manager); }
    return thePlugin;
}
//-----------------------------------------------------------------------------------
CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("bau"));
    info.SetName(wxT("gsgs Vim"));
    info.SetDescription(_("vim bindings for gsgs"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}
//-----------------------------------------------------------------------------------
CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }
//-----------------------------------------------------------------------------------
CodeliteVim::CodeliteVim(IManager* manager)
    : Plugin(manager)
{
    m_longName = _("vim bindings for gsgs");
    m_shortName = wxT("gsgs Vim");

    wxTheApp->Bind(wxEVT_MENU, &CodeliteVim::onVimSetting, this, XRCID("vim_settings"));

    // Load the settings from the file system
    m_settings.Load();
    m_vimM = new VimManager(manager, m_settings);
}
//-----------------------------------------------------------------------------------
CodeliteVim::~CodeliteVim() {}
//-----------------------------------------------------------------------------------
void CodeliteVim::CreateToolBar(gsgs::ToolBar* toolbar) { wxUnusedVar(toolbar); }
//-----------------------------------------------------------------------------------
void CodeliteVim::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    menu->Append(new wxMenuItem(menu, XRCID("vim_settings"), _("Settings...")));
    pluginsMenu->Append(wxID_ANY, GetShortName(), menu);
    wxTheApp->Bind(wxEVT_MENU, &CodeliteVim::onVimSetting, this, XRCID("vim_settings"));
}
//-----------------------------------------------------------------------------------
void CodeliteVim::UnPlug()
{
    wxTheApp->Unbind(wxEVT_MENU, &CodeliteVim::onVimSetting, this, XRCID("vim_settings"));
    wxDELETE(m_vimM);
}
//-----------------------------------------------------------------------------------
void CodeliteVim::onVimSetting(wxCommandEvent& event)
{
    VimSettingsDlg dlg(gsgs_Event().TopFrame());
    if(dlg.ShowModal() == wxID_OK) {
        // Store the settings
        m_settings.SetEnabled(dlg.GetCheckBoxEnabled()->IsChecked()).Save();
        m_vimM->SettingsUpdated();
    }
}
//-----------------------------------------------------------------------------------