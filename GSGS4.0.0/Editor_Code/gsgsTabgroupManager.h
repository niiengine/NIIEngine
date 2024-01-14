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
#ifndef gsgsTABGROUP_MANAGER_H
#define gsgsTABGROUP_MANAGER_H

#include "gsgsPreInclude.h"
#include "gsgsSingleton.h"
#include <wx/xml/xml.h>
#include <utility>

/**
 * Each pair consists of the tabgroup name, and an array of the names of the constituent tabs
 */
typedef std::pair<wxString, wxArrayString> spTabGrp;
typedef std::vector<spTabGrp> vTabGrps;

namespace gsgs
{
    class TabgroupManager : public wxEvtHandler
    {
        friend class Singleton<TabgroupManager>;
    public:
        const wxString & GetTabgroupDirectory()         {return m_tabgroupdir; }
        vTabGrps& GetTabgroups(bool isGlobal = false);

        /*!
         * \brief Finds the spTabGrp that matches tabgroupname, returning its TabgroupItems in items
         * \param tabgroupname
         * \param items
         * \return true if tabgroup was found
         */
        bool FindTabgroup(bool isGlobal, const wxString & tabgroupname, wxArrayString & items);

        void LoadTabgroupData(bool isGlobal, const wxString & tabgroup);

        wxXmlNode * FindTabgroupItem(wxXmlDocument & doc, const wxString & filepath, const wxString & itemfilepath);

        bool DoAddItemToTabgroup(wxXmlDocument & doc, wxXmlNode* node, const wxString& filepath, const wxString & nextitemfilepath);

        /*!
         * \brief Remove this item from the tabgroup on disc, optionally returning its data in an xml node to be stored for
         * Cut
         * \param doc
         * \param filepath
         * \param itemfilepath
         * \return
         */
        wxXmlNode * DoDeleteTabgroupItem(wxXmlDocument& doc, const wxString& filepath, const wxString& itemfilepath);
    protected:
        TabgroupManager();

        ~TabgroupManager();

        void OnWorkspaceLoaded(wxCommandEvent & evt);

        void OnWorkspaceClosed(wxCommandEvent & evt);

        void LoadKnownTabgroups(bool isGlobal = false);

        void SetTabgroupDirectory();
    protected:
        wxString m_tabgroupdir;
        vTabGrps m_tabgroups;
        vTabGrps m_globalTabgroups;
    };

    typedef Singleton<TabgroupManager> TabGroupsManager;
}
#endif