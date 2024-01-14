#include "gsgsGotoEntryManager.h"
#include "NewProjectDialog.h"
#include "gsgsImageManager.h"
#include "gsgsWindowManager.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsLogManager.h"
#include "globals.h"
#include "imanager.h"
#include <wx/menu.h>
#include <wx/xrc/xmlres.h>
#include <wx/stc/stc.h>

namespace gsgs
{
    //-----------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_GOTO_ANYTHING_SELECTED, GotoEvent);
    wxDEFINE_EVENT(wxEVT_GOTO_ANYTHING_SHOWING, GotoEvent);
    wxDEFINE_EVENT(wxEVT_GOTO_ANYTHING_SORT_NEEDED, GotoEvent);
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GotoEvent
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GotoEvent::GotoEvent(wxEventType commandType, int winid)
        : gsgs::Event(commandType, winid)
    {
    }

    GotoEvent::GotoEvent(const GotoEvent& src) { *this = src; }

    GotoEvent::~GotoEvent() {}

    GotoEvent& GotoEvent::operator=(const GotoEvent& src)
    {
        if (this == &src) { return *this; }
        gsgs::Event::operator=(src);
        m_entries = src.m_entries;
        m_entry = src.m_entry;
        return *this;
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GotoEntry
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GotoEntry::GotoEntry(const wxString& desc, const wxString& shortcut, int id)
        : m_desc(desc)
        , m_keyboardShortcut(shortcut)
        , m_resourceID(id)
        , m_flags(0)
    {
    }
    //-----------------------------------------------------------------------------
    GotoEntry::GotoEntry()
        : m_resourceID(wxNOT_FOUND)
        , m_flags(0)
    {
    }
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    // GotoEntryManager
    //-----------------------------------------------------------------------------
    //-----------------------------------------------------------------------------
    GotoEntryManager::GotoEntryManager()
    {
        gsgs_Event().Bind(wxEVT_GOTO_ANYTHING_SELECTED, &GotoEntryManager::OnActionSelected, this);
        gsgs_Event().Bind(wxEVT_GOTO_ANYTHING_SHOWING, &GotoEntryManager::OnShowing, this);
    }
    //-----------------------------------------------------------------------------
    GotoEntryManager::~GotoEntryManager()
    {
        gsgs_Event().Unbind(wxEVT_GOTO_ANYTHING_SELECTED, &GotoEntryManager::OnActionSelected, this);
        gsgs_Event().Unbind(wxEVT_GOTO_ANYTHING_SHOWING, &GotoEntryManager::OnShowing, this);
    }
    //-----------------------------------------------------------------------------
    GotoEntryManager& GotoEntryManager::Get()
    {
        static GotoEntryManager manager;
        return manager;
    }
    //-----------------------------------------------------------------------------
    void GotoEntryManager::OnActionSelected(GotoEvent& e)
    {
        e.Skip();
        // Trigger the action
        const GotoEntry& entry = e.GetEntry();
        if(entry.GetResourceID() != wxID_ANY) {
            wxCommandEvent evtAction(wxEVT_MENU, entry.GetResourceID());
            if(entry.IsCheckable()) {
                evtAction.SetInt(entry.IsChecked() ? 0 : 1); // Set the opposite value
            }
            gsgs_Event().TopFrame()->GetEventHandler()->AddPendingEvent(evtAction);
        }
    }
    //-----------------------------------------------------------------------------
    void GotoEntryManager::ShowDialog()
    {
        // Let the plugins know that we are about to display the actions
        GotoEvent evtShowing(wxEVT_GOTO_ANYTHING_SHOWING);
        evtShowing.SetEntries(GetActions());
        gsgs_Event().ProcessEvent(evtShowing);

        // Let the plugins sort the content
        GotoEvent evtSort(wxEVT_GOTO_ANYTHING_SORT_NEEDED);
        evtSort.GetEntries().swap(evtShowing.GetEntries());
        gsgs_Event().ProcessEvent(evtSort);

        std::vector<GotoEntry> entries = evtSort.GetEntries();
        GotoAnythingDlg dlg(gsgs_Event().TopFrame(), entries);
        dlg.ShowModal();
    }
    //-----------------------------------------------------------------------------
    std::vector<GotoEntry> GotoEntryManager::GetActions()
    {
        Initialise();
        std::vector<GotoEntry> actions;
        std::for_each(
            m_actions.begin(), m_actions.end(),
            [&](const std::unordered_map<wxString, GotoEntry>::value_type& vt) { actions.push_back(vt.second); });
        std::sort(actions.begin(), actions.end(),
                  [&](const GotoEntry& a, const GotoEntry& b) { return a.GetDesc() < b.GetDesc(); });
        return actions;
    }
    //-----------------------------------------------------------------------------
    void GotoEntryManager::Initialise()
    {
        // Register the core actions
        m_actions.clear();

        wxFrame* mainFrame = gsgs_Event().TopFrame();
        wxMenuBar* mb = mainFrame->GetMenuBar();
        if(!mb) 
            return;
        // Get list of menu entries
        std::queue<std::pair<wxString, wxMenu*> > q;
        for(size_t i = 0; i < mb->GetMenuCount(); ++i) {
            q.push(std::make_pair("", mb->GetMenu(i)));
        }

        static wxBitmap defaultBitmap = gsgs_Image().LoadBitmap("placeholder");
        while(!q.empty()) {
            wxMenu* menu = q.front().second;
            wxString prefix = q.front().first;
            q.pop();

            // Call this to ensure that any checkable items are marked as "checked" if needed
            menu->UpdateUI(mainFrame->GetEventHandler());

            const wxMenuItemList& L = menu->GetMenuItems();
            wxMenuItemList::const_iterator iter = L.begin();
            for(; iter != L.end(); ++iter) {
                wxMenuItem* menuItem = *iter;
                if(menuItem->GetSubMenu()) {
                    wxString labelText = menuItem->GetItemLabelText();
                    if((labelText == "Recent Files") || (labelText == "Recent Workspaces")) { continue; }
                    q.push(std::make_pair(menuItem->GetItemLabelText() + " > ", menuItem->GetSubMenu()));
                } else if((menuItem->GetId() != wxNOT_FOUND) && (menuItem->GetId() != wxID_SEPARATOR)) {
                    GotoEntry entry;
                    wxString desc = menuItem->GetItemLabelText();
                    entry.SetDesc(prefix + desc);
                    if(menuItem->IsCheck()) {
                        entry.SetFlags(GotoEntry::kItemCheck);
                        entry.SetChecked(menuItem->IsChecked());
                    }
                    if(menuItem->GetAccel()) { entry.SetKeyboardShortcut(menuItem->GetAccel()->ToString()); }
                    entry.SetResourceID(menuItem->GetId());
                    entry.SetBitmap(menuItem->GetBitmap().IsOk() ? menuItem->GetBitmap() : defaultBitmap);
                    if(!entry.GetDesc().IsEmpty()) {
                        // Dont add empty entries
                        m_actions[entry.GetDesc()] = entry;
                    }
                }
            }
        }
    }
    //-----------------------------------------------------------------------------
    void GotoEntryManager::DoAddCurrentTabActions(GotoEntryList& V)
    {
        IEditor* editor = GetManager()->GetActiveEditor();
        if(editor) {
            {
                GotoEntry entry(_("Current Tab > Close Tabs To The Right"), "", XRCID("close_tabs_to_the_right"));
                V.push_back(entry);
            }
            {
                GotoEntry entry(_("Current Tab > Close Other Tabs"), "", XRCID("close_other_tabs"));
                V.push_back(entry);
            }
            {
                GotoEntry entry(_("Current Tab > Duplicate Tab"), "", XRCID("duplicate_tab"));
                V.push_back(entry);
            }
            {
                GotoEntry entry(_("Current Tab > Make it Read Only"), "", XRCID("mark_readonly"));
                // This a checkable item
                entry.SetCheckable(true);
                entry.SetChecked(editor->GetCtrl()->GetReadOnly());
                V.push_back(entry);
            }
            {
                GotoEntry entry(_("Current Tab > Detach"), "", XRCID("ID_DETACH_EDITOR"));
                V.push_back(entry);
            }
            {
                GotoEntry entry(_("Current Tab > Open Shell Here"), "", XRCID("open_shell_from_filepath"));
                V.push_back(entry);
            }
            {
                GotoEntry entry(_("Current Tab > Open in File Explorer"), "", XRCID("open_file_explorer"));
                V.push_back(entry);
            }
            {
                GotoEntry entry(_("Current Tab > Copy file name"), "", XRCID("copy_file_name_only"));
                V.push_back(entry);
            }
            {
                GotoEntry entry(_("Current Tab > Copy fullpath (name, extension and directory)"), "",
                                  XRCID("copy_file_name"));
                V.push_back(entry);
            }
            {
                GotoEntry entry(_("Current Tab > Copy Path only (directory part only)"), "", XRCID("copy_file_path"));
                V.push_back(entry);
            }
            if(gsgs_Workspace().IsValid()) {
                GotoEntry entry(_("Current Tab > Copy full path relative to workspace"), "",
                                  XRCID("copy_file_relative_path_to_workspace"));
                V.push_back(entry);
            }
        }
    }
    //-----------------------------------------------------------------------------
    void GotoEntryManager::OnShowing(GotoEvent & e)
    {
        e.Skip();
        // Add core actions which are not part of the menu bar
        GotoEntryList& V = e.GetEntries();
        DoAddCurrentTabActions(V);
    }
    //-----------------------------------------------------------------------------
}