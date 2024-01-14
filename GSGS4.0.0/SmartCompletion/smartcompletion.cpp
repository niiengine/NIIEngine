#include "SmartCompletionsSettingsDlg.h"
#include "gsgsEvent.h"
#include "gsgsEvent.h"
#include "gsgsPreInclude.h"
#include "smartcompletion.h"
#include <algorithm>
#include <queue>
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>

static SmartCompletion* thePlugin = NULL;

// Define the plugin entry point
CL_PLUGIN_API Plugin* CreatePlugin(IManager* manager)
{
    if(thePlugin == NULL) { thePlugin = new SmartCompletion(manager); }
    return thePlugin;
}

CL_PLUGIN_API PluginInfo* GetPluginInfo()
{
    static PluginInfo info;
    info.SetAuthor(wxT("PC"));
    info.SetName(wxT("SmartCompletion"));
    info.SetDescription(_("Make the default Code Completion smarter and better"));
    info.SetVersion(wxT("v1.0"));
    return &info;
}

CL_PLUGIN_API int GetPluginInterfaceVersion() { return PLUGIN_INTERFACE_VERSION; }

SmartCompletion::SmartCompletion(IManager* manager)
    : Plugin(manager)
{
    m_longName = _("Make the default Code Completion smarter and better");
    m_shortName = wxT("SmartCompletion");
    gsgs_Event().Bind(wxEVT_CCBOX_SELECTION_MADE, &SmartCompletion::OnCodeCompletionSelectionMade, this);
    gsgs_Event().Bind(wxEVT_CCBOX_SHOWING, &SmartCompletion::OnCodeCompletionShowing, this);
    gsgs_Event().Bind(wxEVT_GOTO_ANYTHING_SORT_NEEDED, &SmartCompletion::OnGotoAnythingSort, this);
    gsgs_Event().Bind(wxEVT_GOTO_ANYTHING_SELECTED, &SmartCompletion::OnGotoAnythingSelectionMade, this);
    m_config.Load();
    m_pCCWeight = &m_config.GetCCWeightTable();
    m_pGTAWeight = &m_config.GetGTAWeightTable();
}

SmartCompletion::~SmartCompletion() {}

void SmartCompletion::CreateToolBar(gsgs::ToolBar* toolbar) { wxUnusedVar(toolbar); }

void SmartCompletion::CreatePluginMenu(wxMenu* pluginsMenu)
{
    wxMenu* menu = new wxMenu();
    wxMenuItem* item(NULL);

    item = new wxMenuItem(menu, XRCID("smart_completion_settings"), _("Settings..."), _("Settings..."), wxITEM_NORMAL);
    menu->Append(item);

    pluginsMenu->Append(wxID_ANY, wxT("SmartCompletions"), menu);

    m_mgr->GetTheApp()->Bind(wxEVT_MENU, &SmartCompletion::OnSettings, this, XRCID("smart_completion_settings"));
}

void SmartCompletion::UnPlug()
{
    m_config.Save();
    gsgs_Event().Unbind(wxEVT_CCBOX_SELECTION_MADE, &SmartCompletion::OnCodeCompletionSelectionMade, this);
    gsgs_Event().Unbind(wxEVT_CCBOX_SHOWING, &SmartCompletion::OnCodeCompletionShowing, this);
    gsgs_Event().Unbind(wxEVT_GOTO_ANYTHING_SORT_NEEDED, &SmartCompletion::OnGotoAnythingSort, this);
    gsgs_Event().Unbind(wxEVT_GOTO_ANYTHING_SELECTED, &SmartCompletion::OnGotoAnythingSelectionMade, this);
    m_mgr->GetTheApp()->Unbind(wxEVT_MENU, &SmartCompletion::OnSettings, this, XRCID("smart_completion_settings"));
}

void SmartCompletion::OnCodeCompletionSelectionMade(CodeCompleteEvent& event)
{
    event.Skip();
    if(!m_config.IsEnabled()) return;

    CHECK_PTR_RET(event.GetEntry());

    // Collect info about this match
    TagEntryPtr tag = event.GetEntry()->GetTag();
    if(tag) {
        WeightTable_t& T = *m_pCCWeight;
        // we have an associated tag
        wxString k = tag->GetScope() + "::" + tag->GetName();
        if(T.count(k) == 0) {
            T[k] = 1;
        } else {
            T[k]++;
        }
        m_config.GetUsageDb().StoreCCUsage(k, T[k]);
    }
}

void SmartCompletion::OnCodeCompletionShowing(CodeCompleteEvent& event)
{
    event.Skip();
    if(!m_config.IsEnabled()) return;

    // Sort the entries by their weight
    wxCodeCompletionBoxEntry::Vec_t& entries = event.GetEntries();

    // We dont want to mess with the default sorting. We just want to place the onse with weight at the top
    // so we split the list into 2: entries with weight geater than 0 and 0
    wxCodeCompletionBoxEntry::Vec_t importantEntries;
    wxCodeCompletionBoxEntry::Vec_t normalEntries;
    wxCodeCompletionBoxEntry::Vec_t::iterator iter = entries.begin();
    for(; iter != entries.end(); ++iter) {
        wxCodeCompletionBoxEntry::Ptr_t entry = (*iter);
        if(entry->GetTag()) {
            wxString k = entry->GetTag()->GetScope() + "::" + entry->GetTag()->GetName();
            if(m_pCCWeight->count(k)) {
                entry->SetWeight((*m_pCCWeight)[k]);
                importantEntries.push_back(entry);
            } else {
                normalEntries.push_back(entry);
            }
        } else {
            normalEntries.push_back(entry);
        }
    }

    entries.swap(normalEntries);
    // Step 2: sort the important entries, based on their weight
    std::sort(importantEntries.begin(), importantEntries.end(),
              [&](wxCodeCompletionBoxEntry::Ptr_t a, wxCodeCompletionBoxEntry::Ptr_t b) {
                  // Sort in desecnding order
                  return a->GetWeight() > b->GetWeight();
              });

    // Step 3: prepend the important entries
    entries.insert(entries.begin(), importantEntries.begin(), importantEntries.end());
}

void SmartCompletion::OnSettings(wxCommandEvent& e)
{
    SmartCompletionsSettingsDlg dlg(gsgs_Event().TopFrame(), m_config);
    dlg.ShowModal();
}

void SmartCompletion::OnGotoAnythingSort(gsgs::GotoEvent& event)
{
    event.Skip();
    if(!m_config.IsEnabled()) return;

    // Sort the entries by their weight
    GotoEntryList& entries = event.GetEntries();
    WeightTable_t& T = *m_pGTAWeight;
    // We dont want to mess with the default sorting. We just want to place the ones with weight at the top
    // so we split the list into 2: entries with weight geater than 0 and 0
    std::vector<std::pair<int, gsgs::GotoEntry> > importantEntries;
    GotoEntryList normalEntries;
    GotoEntryList::iterator iter = entries.begin();
    std::for_each(entries.begin(), entries.end(), [&](const gsgs::GotoEntry& entry) {
        if(T.count(entry.GetDesc())) {
            // This item has weight
            int weight = T[entry.GetDesc()];
            importantEntries.push_back({ weight, entry });
        } else {
            normalEntries.push_back(entry);
        }
    });

    // the list should now contains all the list *wihtout* weight
    entries.swap(normalEntries);

    // Step 2: sort the important entries - the sorting is DESC (lower first)
    std::sort(
        importantEntries.begin(), importantEntries.end(),
        [&](const std::pair<int, gsgs::GotoEntry>& a, const std::pair<int, gsgs::GotoEntry>& b) { return a.first < b.first; });

    // Step 3: prepend the important entries (it actually reverse the sorting)
    std::for_each(importantEntries.begin(), importantEntries.end(),
                  [&](const std::pair<int, gsgs::GotoEntry>& p) { entries.insert(entries.begin(), p.second); });
}

void SmartCompletion::OnGotoAnythingSelectionMade(gsgs::GotoEvent& event)
{
    event.Skip();
    if(!m_config.IsEnabled()) return;

    // Collect info about this match
    WeightTable_t& T = *m_pGTAWeight;

    const wxString& key = event.GetEntry().GetDesc();
    if(T.count(key) == 0) {
        T[key] = 1;
    } else {
        T[key]++;
    }
    m_config.GetUsageDb().StoreGTAUsage(key, T[key]);
}
