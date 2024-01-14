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

#ifndef PERSPECTIVEMANAGER_H
#define PERSPECTIVEMANAGER_H

#include "gsgsPreInclude.h"
#include <wx/aui/framemanager.h>

extern wxString DEBUG_LAYOUT ;
extern wxString NORMAL_LAYOUT;

#define PERSPECTIVE_FIRST_MENU_ID 17000
#define PERSPECTIVE_LAST_MENU_ID  17020
namespace gsgs
{
    class PerspectiveManager : public wxEvtHandler
    {
    public:
        PerspectiveManager();
        virtual ~PerspectiveManager();
        
        void ConnectEvents(wxAuiManager* mgr);
        void DisconnectEvents();
        
        void ToggleOutputPane(bool hide, bool commit = true);
        void SetActive(const wxString & active) 
        {
            this->m_active = active;
        }
        const wxString & GetActive() const 
        {
            return m_active;
        }

        bool IsDefaultActive() const;
        void LoadPerspective(const wxString &name = wxT(""));
        void LoadPerspectiveByMenuId(int id);
        void SavePerspective(const wxString &name = wxT(""), bool notify = true);
        void SavePerspectiveIfNotExists(const wxString &name);
        
        wxArrayString GetAllPerspectives();
        void DeleteAllPerspectives();
        void ClearIds();
        int MenuIdFromName(const wxString &name);
        wxString NameFromMenuId(int id);
        void Rename(const wxString &old, const wxString &new_name);
        void Delete(const wxString &name);

        int FirstMenuId() const 
        {
            return PERSPECTIVE_FIRST_MENU_ID;
        }
        int LastMenuId() const 
        {
            return PERSPECTIVE_LAST_MENU_ID;
        }
    protected:
        wxString DoGetPathFromName(const wxString &name);
        void DoEnsureDebuggerPanesAreVisible();
        void DoShowPane(const wxString &panename, bool show, bool &needUpdate);
        // Event handlers
        void OnPaneClosing(wxAuiManagerEvent &event);
    protected:
        std::map<wxString, int> m_menuIdToName;
        int                     m_nextId;
        wxString                m_active;
        wxString                m_buildPerspective;
        wxAuiManager *          m_aui;
    };
}
#endif
