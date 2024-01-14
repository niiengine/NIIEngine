#ifndef __EditorConfigPlugin__
#define __EditorConfigPlugin__

#include "plugin.h"
#include "gsgsEvent.h"
#include "EditorConfigCache.h"
#include "gsgsOptionsConfig.h"
#include "EditorConfigSettings.h"

class EditorConfigPlugin : public Plugin
{
    EditorConfigCache m_cache;
    EditorConfigSettings m_settings;

protected:
    bool DoGetEditorConfigForFile(const wxFileName& filename, gsgs::EditorConfigSection& section);

public:
    EditorConfigPlugin(IManager* manager);
    ~EditorConfigPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(gsgs::ToolBar * toolbar);
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    virtual void CreatePluginMenu(wxMenu * pluginsMenu);

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    virtual void UnPlug();

    void OnEditorConfigLoading(gsgs::EditorConfigEvent& event);
    void OnActiveEditorChanged(wxCommandEvent& event);
    void OnSettings(wxCommandEvent& event);
};

#endif // EditorConfigPlugin
