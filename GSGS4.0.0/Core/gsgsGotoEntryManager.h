/*
-----------------------------------------------------------------------------
A
     __      _   _   _   ______
    |   \   | | | | | | |  ____)                    _
    | |\ \  | | | | | | | |         ___      ___   (_)   ___
    | | \ \ | | | | | | | |____    / _ \   / ___ \  _   / _ \   ___
    | |  \ \| | | | | | |  ____)  | / \ | | |  | | | | | / \ | / _ )
    | |   \ | | | | | | | |_____  | | | | | |__| | | | | | | | | __/
    |_|    \ _| |_| |_| |_______) |_| |_|  \___| | |_| |_| |_| |___|
                                             __/ |                 
                                            \___/   
                                                
                                                
                                                                 F i l e


Copyright: NIIEngine Team Group

Home page: www.niiengine.com 

Email: niiengine@gmail.com OR niiengine@163.com

Licence: commerce(www.niiengine.com/license)(Three kinds)
------------------------------------------------------------------------------
*/
#ifndef gsgsGotoEntryManager_H
#define gsgsGotoEntryManager_H

#include "gsgsPreInclude.h"
#include "gsgsEvent.h"
#include <wx/bitmap.h>

namespace gsgs
{
    class _gsgsAPI GotoEntry
    {
    public:
        enum eFlags {
            kItemNormal = (1 << 0),
            kItemRadio = (1 << 1),
            kItemCheck = (1 << 2),
            kItemStateChecked = (1 << 3),
        };
    public:
        GotoEntry(const wxString& desc, const wxString& shortcut, int id);
        GotoEntry();

        void SetDesc(const wxString& desc) { this->m_desc = desc; }
        void SetKeyboardShortcut(const wxString& keyboardShortcut) { this->m_keyboardShortcut = keyboardShortcut; }
        void SetResourceID(int resourceID) { this->m_resourceID = resourceID; }
        const wxString& GetDesc() const { return m_desc; }
        const wxString& GetKeyboardShortcut() const { return m_keyboardShortcut; }
        int GetResourceID() const { return m_resourceID; }
    #if wxUSE_GUI
        const wxBitmap& GetBitmap() const { return m_bitmap; }
        void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }
    #endif
        void SetFlags(size_t flags) { this->m_flags = flags; }
        size_t GetFlags() const { return m_flags; }
        bool IsChecked() const { return m_flags & kItemStateChecked; }
        void SetChecked(bool b) { b ? m_flags |= kItemStateChecked : m_flags &= ~kItemStateChecked; }
        bool IsCheckable() const { return m_flags & kItemCheck; }
        void SetCheckable(bool b) { b ? m_flags |= kItemCheck : m_flags &= ~kItemCheck; }
    private:
        wxString m_desc;
        wxString m_keyboardShortcut;
        int m_resourceID;
    #if wxUSE_GUI
        wxBitmap m_bitmap;
    #endif
        size_t m_flags;
    };
    typedef std::vector<GotoEntry> GotoEntryList;

    class _gsgsAPI GotoEvent : public Event
    {
    public:
        GotoEvent(wxEventType commandType = wxEVT_NULL, int winid = 0);
        GotoEvent(const GotoEvent& src);
        GotoEvent& operator=(const GotoEvent& src);
        virtual ~GotoEvent();
        virtual wxEvent* Clone() const { return new GotoEvent(*this); }
        const GotoEntryList& GetEntries() const { return m_entries; }
        GotoEntryList& GetEntries() { return m_entries; }
        void SetEntries(const GotoEntryList& entries) { m_entries = entries; }
        void SetEntry(const GotoEntry& entry) { m_entry = entry; }
        const GotoEntry& GetEntry() const { return m_entry; }
    private:
        GotoEntryList m_entries;
        GotoEntry m_entry;
    };

    typedef void (wxEvtHandler::*clGotoEventFunction)(GotoEvent&);
    #define clGotoEventHandler(func) wxEVENT_HANDLER_CAST(clGotoEventFunction, func)

    // Goto Anything item was selected
    // Use: Event::GetString to get the selected item
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GOTO_ANYTHING_SELECTED, GotoEvent);

    // Goto Anything dialog is about to be shown
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GOTO_ANYTHING_SHOWING, GotoEvent);

    // This event is sent to allow plugins and other modules to sort the entries before
    // they are displayed to the user
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_GOTO_ANYTHING_SORT_NEEDED, GotoEvent);

    class _gsgsAPI GotoEntryManager : public wxEvtHandler
    {
    public:
        static GotoEntryManager& Get();

        /**
         * @brief fill the gotomanager with all the menu entries
         */
        void Initialise();

        /**
         * @brief show the "Goto Anything" dialog
         */
        void ShowDialog();

        /**
         * @brief return list of all available actions
         */
        std::vector<GotoEntry> GetActions();
    protected:
        GotoEntryManager();
        virtual ~GotoEntryManager();

        void OnActionSelected(GotoEvent& e);
        void OnShowing(GotoEvent& e);

        void DoAddCurrentTabActions(GotoEntryList& V);
    private:
        std::unordered_map<wxString, GotoEntry> m_actions;
    };
}
#endif
