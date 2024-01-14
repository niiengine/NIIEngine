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
#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include "gsgsPreInclude.h"
#include "debugger.h"
#include "wx/xml/xml.h"

namespace
{
const wxChar defaultSessionName[] = wxT("Default");
const wxChar sessionTag[] = wxT("Session");
const wxChar tabgroupTag[] = wxT("Tabgroup");
}

namespace gsgs
{
    class _gsgsAPI SessionEntry : public ObjectData
    {
    public:
        SessionEntry();
        virtual ~SessionEntry();

        inline void SetSelectedTab(int tab)                                 { m_selectedTab = tab; }
        inline int GetSelectedTab() const                                   { return m_selectedTab; }
        inline void SetWorkspaceName(const wxString & name)                 { m_workspaceName = name; }
        inline const wxString& GetWorkspaceName() const                     { return m_workspaceName; }
        inline void SetTabInfoArr(const std::vector<TabInfoData> & ilist)   { m_vTabInfoArr = ilist; }
        inline const std::vector<TabInfoData>& GetTabInfoArr()              { return m_vTabInfoArr; }
        inline void SetBreakpoints(const std::vector<BreakpointInfo> & breakpoints) { m_breakpoints.SetBreakpoints(breakpoints); }
        inline const std::vector<BreakpointInfo>& GetBreakpoints() const    { return m_breakpoints.GetBreakpoints(); }
        // inline void SetTabs(const wxArrayString & tabs)                  {this->m_tabs = tabs;}
        // inline const wxArrayString& GetTabs() const                      {return m_tabs;}
        inline void SetFindInFilesMask(const wxString& mask)                { m_findInFilesMask = mask; }
        inline const wxString & GetFindInFilesMask() const                  { return m_findInFilesMask; }

        void Serialize(Serializer& arch);
        void DeSerialize(Serializer& arch);
    protected:
        int m_selectedTab;
        wxArrayString m_tabs;
        wxString m_workspaceName;
        std::vector<TabInfoData> m_vTabInfoArr;
        BreakpointInfoArray m_breakpoints;
        wxString m_findInFilesMask;
    };

    /**
     * \class TabGroupEntry
     * \brief TabGroupEntry serialises a named group of tabs
     */
    class _gsgsAPI TabGroupEntry : public SessionEntry
    {
    public:
        TabGroupEntry() {}
        virtual ~TabGroupEntry() {}

        void Serialize(Serializer& arch);
        void DeSerialize(Serializer& arch);
        inline void SetTabgroupName(const wxString & name)                  { m_tabgroupName = name; }
        inline const wxString& GetTabgroupName() const                      { return m_tabgroupName; }
    private:
        wxString m_tabgroupName;
    };

    class _gsgsAPI SessionManager
    {
    public:
        static SessionManager& singleton();

        bool Load(const wxString& fileName);

        bool Save(const wxString& name, SessionEntry& session, 
            const wxString& suffix = wxT(""), const wxChar* Tag = sessionTag);

        bool GetSession(const wxString& workspaceFile, SessionEntry& session, 
            const wxString& suffix = wxT(""), const wxChar* Tag = sessionTag);

        void SetLastSession(const wxString & name);

        wxString GetLastSession();

        void UpdateFindInFilesMaskForCurrentWorkspace(const wxString & mask);

        wxString GetFindInFilesMaskForCurrentWorkspace();
    protected:
        SessionManager();
        ~SessionManager();
        wxFileName GetSessionFileName(const wxString & name, const wxString & suffix = wxT("")) const;
    private:
        wxXmlDocument m_doc;
        wxFileName m_fileName;
    };
}
#define gsgs_Session() gsgs::SessionManager::singleton()
#endif
