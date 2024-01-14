#include "EditorConfigSettings.h"
#include "EditorConfigSettingsDlg.h"
#include "gsgsConfig.h"
#include "gsgsEvent.h"
#include "editorconfigplugin.h"
#include "gsgsLogManager.h"
#include <wx/filename.h>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
//-----------------------------------------------------------------------------------
static EditorConfigPlugin* thePlugin = NULL;
//-----------------------------------------------------------------------------------
CL_PLUGIN_API Plugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) 
    { 
        thePlugin = new EditorConfigPlugin(manager); 
    }
    return thePlugin;
}
//-----------------------------------------------------------------------------------
CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("Eran Ifrah"));
    info.SetName(wxT("gsgs::Config"));
    info.SetDescription(_("Support for .editorconfig files in CodeLite"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}
//-----------------------------------------------------------------------------------
CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }
//-----------------------------------------------------------------------------------
EditorConfigPlugin::EditorConfigPlugin(IManager* manager)
    : Plugin(manager)
{
    m_longName = _("Support for .editorconfig files in CodeLite");
    m_shortName = wxT("gsgs::Config");

    // Load the settings
    m_settings.Load();

    // Bind events
    gsgs_Event().Bind(wxEVT_EDITOR_CONFIG_LOADING, &EditorConfigPlugin::OnEditorConfigLoading, this);
    gsgs_Event().Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &EditorConfigPlugin::OnActiveEditorChanged, this);
}
//-----------------------------------------------------------------------------------
EditorConfigPlugin::~EditorConfigPlugin() {}
//-----------------------------------------------------------------------------------
void EditorConfigPlugin::CreateToolBar(gsgs::ToolBar* toolbar) { wxUnusedVar(toolbar); }
//-----------------------------------------------------------------------------------
void EditorConfigPlugin::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    menu->Append(new wxMenuItem(menu, XRCID("editor_config_settings"), _("Settings...")));
    pluginsMenu->Append(wxID_ANY, "gsgs::Config", menu);
    menu->Bind(wxEVT_MENU, &EditorConfigPlugin::OnSettings, this, XRCID("editor_config_settings"));
}
//-----------------------------------------------------------------------------------
void EditorConfigPlugin::UnPlug()
{
    gsgs_Event().Unbind(wxEVT_EDITOR_CONFIG_LOADING, &EditorConfigPlugin::OnEditorConfigLoading, this);
    gsgs_Event().Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &EditorConfigPlugin::OnActiveEditorChanged, this);
}
//-----------------------------------------------------------------------------------
void EditorConfigPlugin::OnEditorConfigLoading(gsgs::EditorConfigEvent& event)
{
    event.Skip();
    if(!m_settings.IsEnabled()) 
    {
        gsgsDEBUG() << "gsgs::Config is disabled." << clEndl;
        return;
    }

    gsgs::EditorConfigSection section;
    wxFileName fn(event.GetFileName());
    if(!DoGetEditorConfigForFile(fn, section)) { return; }

    event.Skip(false);
    event.SetEditorConfig(section);
}
//-----------------------------------------------------------------------------------
void EditorConfigPlugin::OnActiveEditorChanged(wxCommandEvent& event)
{
    event.Skip();
    if(!m_settings.IsEnabled()) 
    {
        gsgsDEBUG1() << "gsgs::Config is disabled." << clEndl;
        return;
    }

    IEditor* editor = m_mgr->GetActiveEditor();
    CHECK_PTR_RET(editor);

    OptionsConfig * conf = editor->GetOptions();
    CHECK_PTR_RET(conf);

    gsgs::EditorConfigSection section;
    if(!DoGetEditorConfigForFile(editor->GetFileName(), section)) return;
    conf->UpdateFromEditorConfig(section);
    editor->ApplyEditorConfig();
}
//-----------------------------------------------------------------------------------
bool EditorConfigPlugin::DoGetEditorConfigForFile(const wxFileName& filename, gsgs::EditorConfigSection& section)
{
    // Try the cache first
    if(m_cache.Get(filename, section)) 
    {
        gsgsDEBUG1() << "Using gsgs::Config settings for file:" << filename << clEndl;
        section.PrintToLog();
        return true;
    }

    // Sanity
    if(!filename.IsOk() || !filename.FileExists()) 
    {
        gsgsDEBUG1() << "No gsgs::Config file found for file:" << filename << clEndl;
        return false;
    }

    gsgs::EditorConfigData conf;
    if(!conf.GetSectionForFile(filename, section)) 
    {
        // Update the cache
        gsgsDEBUG1() << "No gsgs::Config file found for file:" << filename << clEndl;
        return false;
    }

    m_cache.Add(filename, section);
    return true;
}
//-----------------------------------------------------------------------------------
void EditorConfigPlugin::OnSettings(wxCommandEvent& event)
{
    EditorConfigSettingsDlg dlg(wxTheApp->GetTopWindow());
    if(dlg.ShowModal() == wxID_OK) {
        // Store the settings
        m_settings.SetEnabled(dlg.IsEnabled());
        m_settings.Save();
    }
}
//-----------------------------------------------------------------------------------