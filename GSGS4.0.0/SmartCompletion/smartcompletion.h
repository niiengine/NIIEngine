#ifndef __SmartCompletion__
#define __SmartCompletion__

#include "SmartCompletionsConfig.h"
#include "gsgsEvent.h"
#include "gsgsTag.h"
#include "plugin.h"
#include "gsgsStringUtil.h"
#include <unordered_map>

class SmartCompletion : public Plugin
{
    typedef std::unordered_map<wxString, int> WeightTable_t;
    WeightTable_t* m_pCCWeight;
    WeightTable_t* m_pGTAWeight;
    typedef std::pair<TagEntryPtr, int> QueueElement_t;
    SmartCompletionsConfig m_config;

protected:
    void OnCodeCompletionSelectionMade(CodeCompleteEvent& event);
    void OnCodeCompletionShowing(CodeCompleteEvent& event);
    void OnGotoAnythingSort(gsgs::GotoEvent& event);
    void OnGotoAnythingSelectionMade(gsgs::GotoEvent& event);
    void OnSettings(wxCommandEvent& e);

public:
    SmartCompletion(IManager* manager);
    ~SmartCompletion();

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

#endif // SmartCompletion
