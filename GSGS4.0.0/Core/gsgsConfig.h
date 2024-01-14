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
#ifndef _gsgs_CONFIG_H
#define _gsgs_CONFIG_H

#include "gsgsPreInclude.h"
#include "gsgsSerializer.h"
#include <wx/colour.h>
#include <wx/xml/xml.h>

#define kConfigLLDBTooltipW                 "LLDBTooltipW"
#define kConfigLLDBTooltipH                 "LLDBTooltipH"
#define kConfigBuildAutoScroll              "build-auto-scroll"
#define kConfigCreateVirtualFoldersOnDisk   "CreateVirtualFoldersOnDisk"
#define kConfigLogVerbosity                 "LogVerbosity"
#define kConfigRedirectLogOutput            "RedirectLogOutput"
#define kConfigSingleInstance               "SingleInstance"
#define kConfigCheckForNewVersion           "CheckForNewVersion"
#define kConfigMaxItemsInFindReplaceDialog  "MaxItemsInFindReplaceDialog"
#define kConfigMaxOpenedTabs                "MaxOpenedTabs"
#define kConfigRestoreLastSession           "RestoreLastSession"
#define kConfigFrameTitlePattern            "FrameTitlePattern"
#define kConfigStatusbarShowLine            "StatusbarShowLine"
#define kConfigStatusbarShowColumn          "StatusbarShowColumn"
#define kConfigStatusbarShowPosition        "StatusbarShowPosition"
#define kConfigStatusbarShowLength          "StatusbarShowLength"
#define kConfigStatusbarShowSelectedChars   "StatusbarShowSelChars"
#define kConfigToolbarGroupSpacing          "ToolbarGroupSpacing"
#define kConfigAutoDetectCompilerOnStartup  "AutoDetectCompilerOnStartup"
#define kConfigBootstrapCompleted           "BootstrapCompleted"
#define kConfigUpdateParserPaths            "updateParserPaths"
#define kConfigShowToolBar                  "ShowToolBar"
#define kConfigShowTabBar                   "ShowTabBar"
#define kConfigShowMenuBar                  "ShowMenuBar"
#define kConfigShowStatusBar                "ShowStatusBar"
#define kConfigWorkspaceTabSashPosition     "WorkspaceTabSashPosition"
#define kConfigTabsPaneSortAlphabetically   "TabsPaneSortAlphabetically"
#define kConfigFileExplorerBookmarks        "FileExplorerBookmarks"

namespace gsgs
{
    class _gsgsAPI Config
    {
    public:
        Config(const wxString & filename = "GsGs.cfg");
        virtual ~Config();

        static Config & singleton();

        static void Release();

        void setAutoPermanent(bool set)         { mAutoPermanent = set; }

        bool isAutoPermanent() const            { return mAutoPermanent; }

        bool Load(const wxString & basename, const wxString & version);

        void Reload();

        void Save(const wxFileName& fn);
        void Save();

        bool ReadItem(ObjectData* out, const wxString & destitem = wxEmptyString);
        void WriteItem(const ObjectData* in, const wxString & destitem = wxEmptyString);

        bool Read(const wxString & name, bool & out, bool _default);
        bool Read(const wxString & name, int & out, int _default);
        bool Read(const wxString & name, wxString & out, const wxString & _default);
        bool Read(const wxString & name, wxArrayString & out, const wxArrayString & _default);
        bool Read(const wxString & name, wxFont & out, const wxFont & _default);
        bool Read(const wxString & name, wxColour & out, const wxColour & _default);
        bool Read(const wxString & name, ObjectData * out);

        void Write(const wxString & name, bool in);
        void Write(const wxString & name, int in);
        void Write(const wxString & name, const wxString & in);
        void Write(const wxString & name, const wxArrayString & in);
        void Write(const wxString & name, const wxFont & in);
        void Write(const wxString & name, const wxColour & in);
        bool Write(const wxString & name, ObjectData * in);

        JSONObject GetRootObject();
        JSONObject AddObject(const wxString & name);
        JSONObject GetObject(const wxString & name);
        bool IsObjectExist(const wxString & name);

        inline void AddRecentOpen(const wxString & filename, bool permanent)    { DoAddRecentItem("RecentOpen", filename, permanent); }
        inline void GetRecentOpen(wxArrayString & out) const                    { DoGetRecentItems("RecentOpen", out); }
        inline void ClearRecentOpen()                                           { DoClearRecentItems("RecentOpen"); }

        void SetWorkspaceTabOrder(const wxArrayString & tabs, int selected, bool permanent);
        bool GetWorkspaceTabOrder(wxArrayString & tabs, int & selected);

        void SetOutputTabOrder(const wxArrayString & tabs, int selected, bool permanent);
        bool GetOutputTabOrder(wxArrayString & tabs, int & selected);
        

        void AddQuickFindSearchItem(const wxString & str, bool permanent);
        void AddQuickFindReplaceItem(const wxString & str, bool permanent);

        wxArrayString GetQuickFindSearchItems() const;
        wxArrayString GetQuickFindReplaceItems() const;

        void SetQuickFindSearchItems(const wxArrayString & items, bool permanent);
        void SetQuickFindReplaceItems(const wxArrayString & items, bool permanent);

        // standard IDs for annoying dialogs
        int GetAnnoyingDlgAnswer(const wxString & name, int _default = wxNOT_FOUND);

        void SetAnnoyingDlgAnswer(const wxString & name, int value, bool permanent);

        void ClearAnnoyingDlgAnswers();
    protected:
        Config(const Config & o) { }
        Config & operator=(const Config & o) { return *this; }

        void DoDeleteProperty(const wxString & property);
        JSONObject GetGeneralSetting();

        void DoAddRecentItem(const wxString & propName, const wxString & filename, bool permanent);
        void DoGetRecentItems(const wxString & propName, wxArrayString & out) const;
        void DoClearRecentItems(const wxString & propName);
        void DoReadRecentItem();
    protected:
        static Config* gInstance;
        JSON * m_root;
        wxFileName m_filename;
        wxXmlDocument m_doc;
        wxString m_fileName;
        bool mAutoPermanent;
        static std::map<wxString, wxArrayString> m_cacheRecentItems;
    };
}
#define gsgs_Config() gsgs::Config::singleton()
#endif // CLCONFIG_H
