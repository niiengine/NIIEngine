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
#ifndef gsgs_BOOK_H
#define gsgs_BOOK_H

#include "gsgsPreInclude.h"
#include "gsgsMultiBook.h"
#include "gsgsNavigationManager.h"
#include "gsgsCommonDialog.h"
#include "CommonController.h"
#include "gsgsFileUtil.h"
#include "gsgsQuickFindBar.h"
#include "gsgsEditor.h"
#include "gsgsSessionManager.h"

class FilesModifiedDlg;
namespace gsgs
{
    class Present;
    class EditorBar;
    class Book : public wxPanel
    {
    public:
        enum
        {
            kGetAll_Default = 0x00000000,         // booked editors only
            kGetAll_RetainOrder = 0x00000001,     // Order must keep
            kGetAll_IncludeDetached = 0x00000002, // return both booked editors and detached
            kGetAll_DetachedOnly = 0x00000004,    // return detached editors only
        };
    public:
        Book(wxWindow* parent, wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL | wxSIMPLE_BORDER,
            const wxString& name = wxPanelNameStr);
        virtual ~Book();

        void SetFindBar(QuickFindBar* findBar);

        QuickFindBar* GetFindBar() const                        { return m_findBar; }

        void createPresent();

        //static bool AskUserToSave(Editor* editor);
        /**
         * @brief show the navigation dialog
         */
        void ShowNavigationDialog();

        /**
         * @brief move the active page right or left
         */
        void MovePage(bool movePageRight);

        const EditorFrameList_t& GetDetachedEditors() const     { return m_detachedEditors; }
        void DetachActiveEditor();
        void ClearFileHistory();
        FileHistory& GetRecentlyOpenedFilesClass()              { return m_recentFiles; }
        void ShowQuickBarForPlugins();
        void ShowQuickBar(bool s);
        void ShowQuickBar(const wxString& findWhat, bool showReplace);
        void ShowTabBar(bool b);
        void ShowNavBar(bool s = true);
        bool IsNavBarShown() const                              { return m_navBar && m_navBar->IsShown(); }
        EditorBar* GetEditorBar() const                         { return m_navBar; }
        void SetEditorBar(EditorBar* bar)                       { m_navBar = bar; }

        void SaveSession(SessionEntry & session, wxArrayInt * excludeArr = NULL);
        void RestoreSession(SessionEntry & session);
        /**
         * @brief create session from current IDE state
         */
        void CreateSession(SessionEntry & session, wxArrayInt * excludeArr = NULL);

        Editor * GetActiveEditor(bool includeDetachedEditors = false);

        /**
         * @brief return vector of all editors in the notebook. This function only returns instances of type Editor
         * @param editors [output]
         * @param flags kGetAll_*
         */
        void GetAllEditors(EditorVec_t& editors, size_t flags);
        /**
         * @brief return vector of all tabs in the notebook
         * @param tabs [output]
         */
        void GetAllTabs(TabVec_t& tabs);

        /**
         * @brief return a list of the detached tabs
         */
        void GetDetachedTabs(TabVec_t& tabs);

        Editor* FindEditor(const wxString& fileName);

        bool CloseEditor(const wxString& fileName) { return ClosePage(FindEditor(fileName)); }

        Editor * NewEditor();

        Editor * OpenFile(const wxString& file_name, const wxString& projectName = wxEmptyString, int lineno = wxNOT_FOUND,
            long position = wxNOT_FOUND, OF_extra extra = OF_AddJump, bool preserveSelection = true,
                const wxBitmap& bmp = wxNullBitmap, const wxString& tooltip = wxEmptyString);
        /**
         * @brief open file based on a browsing record
         */
        Editor* OpenFile(const BrowseRecord& rec);

        /**
         * @brief a simpler version: open a file with a given tooltip and bitmap
         */
        Editor* OpenFile(const wxString& file_name, const wxBitmap& bmp, const wxString& tooltip = wxEmptyString)
        {
            return OpenFile(file_name, "", wxNOT_FOUND, wxNOT_FOUND, OF_AddJump, false, bmp, tooltip);
        }

        bool AddPage(wxWindow* win, const wxString& text, const wxString& tooltip = wxEmptyString,
            const wxBitmap& bmp = wxNullBitmap, bool selected = false, int insert_at_index = wxNOT_FOUND);
        bool SelectPage(wxWindow* win);
        wxWindow* FindPage(const wxString& text) const;

        wxWindow * GetCurrentPage() const;

        int GetCurrentPageIndex() const;

        wxWindow * GetPage(size_t page);

        size_t GetPageCount() const;

        bool UserSelectFiles(std::vector<std::pair<wxFileName, bool> > & files, const wxString & title,
            const wxString & caption, bool cancellable = true);

        bool SaveAll(bool askUser, bool includeUntitled);

        void ReloadExternallyModified(bool prompt);

        bool ClosePage(const wxString& text);

        bool ClosePage(wxWindow* win);
        bool ClosePage(IEditor* editor, bool prompt);
        bool CloseAllButThis(wxWindow* win);
        bool CloseAll(bool cancellable, bool showpresent = true);

        // These 3 functions are meant to be used with CallAfter
        void ClosePageVoid(wxWindow* win);
        void CloseAllButThisVoid(wxWindow* win);
        void CloseTabsToTheRight(wxWindow* win);
        void CloseAllVoid(bool cancellable);

        wxString GetPageTitle(wxWindow* win) const;
        void SetPageTitle(wxWindow* page, const wxString& name);
        long GetBookStyle();

        void ApplySettings();
        void UnHighlightAll();
        void DelAllBreakpointMarkers();
        void SetViewEOL(bool visible);
        void SetViewWordWrap(bool b);
        void HighlightWord(bool hl);
        void ShowWhitespace(int ws);
        void UpdateColours();
        void UpdateBreakpoints();
        void MarkEditorReadOnly(Editor* editor, bool refresh = true);

        void SetUseBuffereLimit(bool useBuffereLimit) { m_useBuffereLimit = useBuffereLimit; }
        bool GetUseBuffereLimit() const { return m_useBuffereLimit; }
    private:
        FilesModifiedDlg* GetFilesModifiedDlg();
        void DoShowTabLabelContextMenu();
        void DoUpdateNotebookTheme();
        void DoOpenImageViewer(const wxFileName& filename);

        void OnMouseDClick(wxBookCtrlEvent& e);
        void OnTabDClicked(wxBookCtrlEvent& e);
        void OnTabLabelContextMenu(wxBookCtrlEvent& e);
        void OnPageClosing(wxBookCtrlEvent& e);
        void OnPageClosed(wxBookCtrlEvent& e);
        void OnPageChanged(wxBookCtrlEvent& e);
        void OnClosePage(wxBookCtrlEvent& e);
        void OnPageChanging(wxBookCtrlEvent& e);
        void OnProjectFileAdded(Event& e);
        void OnProjectFileRemoved(Event& e);
        void OnWorkspaceLoaded(wxCommandEvent& e);
        void OnWorkspaceClosed(wxCommandEvent& e);
        void OnDebugEnded(DebugEvent& e);
        void OnInitDone(wxCommandEvent& e);
        void OnDetachedEditorClosed(Event& e);
        void OnTheme(wxCommandEvent& e);
        void OnColoursAndFontsChanged(Event& e);
        bool DoSelectPage(wxWindow* win);
        void DoHandleFrameMenu(Editor* editor);
        void DoEraseDetachedEditor(IEditor* editor);
        void OnWorkspaceReloadStarted(Event& e);
        void OnWorkspaceReloadEnded(Event& e);
        void OnEditorSettingsChanged(wxCommandEvent& e);
        void OnCacheUpdated(Event& e);
        void OnUpdateNavigationBar(CodeCompleteEvent& e);
        void OnNavigationBarMenuShowing(ContextMenuEvent& e);
        void OnNavigationBarMenuSelectionMade(Event& e);
        void OnSettingsChanged(wxCommandEvent& e);

        /**
         * @brief open file and set an alternate content
         */
        void DoOpenFile(const wxString& filename, const wxString& content = "");

        /**
         * @brief update the navigation bar (C++)
         * @param editor
         */
        void UpdateNavBar(Editor* editor);

        /**
         * @brief display the welcome page
         */
        void showPresent(bool show);
        void DoShowWindow(wxWindow * win, bool show);

        void OnEditorChanged(wxCommandEvent& event);
        void OnAllEditorClosed(wxCommandEvent& event);
    private:
        Notebook * m_book;
        Present * mWelcome;
        EditorBar * m_navBar;
        QuickFindBar * m_findBar;
        FilesModifiedDlg * m_filesModifiedDlg;
        EditorFrameList_t m_detachedEditors;
        FileHistory m_recentFiles;
        std::vector<unsigned int> mFreeMark;
        std::unordered_map<wxString, TagEntryPtr> m_currentNavBarTags;
        bool m_isWorkspaceReloading;
        bool m_reloadingDoRaise;
        bool m_useBuffereLimit;
    };
}
#endif
