#ifndef __LanguageServerPlugin__
#define __LanguageServerPlugin__

#include "plugin.h"
#include "gsgsEvent.h"
#include "LanguageServerCluster.h"
#include "gsgsEvent.h"

class LanguageServerPlugin : public Plugin
{
    LanguageServerCluster::Ptr_t m_servers;
    IProcess* m_process = nullptr;
    
protected:
    void OnSettings(wxCommandEvent& e);

public:
    LanguageServerPlugin(IManager* manager);
    virtual ~LanguageServerPlugin();

    //--------------------------------------------
    // Abstract methods
    //--------------------------------------------
    virtual void CreateToolBar(gsgs::ToolBar* toolbar);
    /**
     * @brief Add plugin menu to the "Plugins" menu item in the menu bar
     */
    virtual void CreatePluginMenu(wxMenu* pluginsMenu);

    /**
     * @brief Unplug the plugin. Perform here any cleanup needed (e.g. unbind events, destroy allocated windows)
     */
    virtual void UnPlug();
};

#endif // LanguageServerPlugin
