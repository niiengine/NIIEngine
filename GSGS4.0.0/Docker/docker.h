#ifndef __Docker__
#define __Docker__

#include "clDockerDriver.h"
#include "plugin.h"

class DockerOutputPane;
class Docker : public Plugin
{
    DockerOutputPane* m_outputView;
    gsgs::TabTogglerHelper::Ptr_t m_tabToggler;
    clDockerDriver::Ptr_t m_driver;

public:
    Docker(IManager* manager);
    virtual ~Docker();

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

    DockerOutputPane* GetTerminal() { return m_outputView; }
    clDockerDriver::Ptr_t GetDriver() { return m_driver; }
};

#endif // Docker
