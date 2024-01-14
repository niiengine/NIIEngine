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
#ifndef gsgsCommonHandler_H
#define gsgsCommonHandler_H

#include "gsgsPreInclude.h"
#include "gsgsProcess.h"
#include "gsgsNotebook.h"
#include <wx/pen.h>
#include <wx/bitmap.h>
#include <wx/aui/auibar.h>
#include <wx/aui/framemanager.h>
#include <wx/simplebook.h>
#include <wx/timer.h>
#include <wx/stc/stc.h>
#include <wx/statbmp.h>
#include "imanager.h"
#include <tuple>

namespace gsgs
{
    class _gsgsAPI ThemeHandlerHelper : public wxEvtHandler
    {
    public:
        typedef wxSharedPtr<ThemeHandlerHelper> Ptr;
    public:
        ThemeHandlerHelper(wxWindow* win);
        virtual ~ThemeHandlerHelper();
        void OnTheme(wxCommandEvent& e);
        void OnPreferencesUpdated(wxCommandEvent& e);
        void OnColoursUpdated(Event& e);
        void UpdateColours(wxWindow* topWindow);
        void UpdateNotebookColours(wxWindow* topWindow);
    private:
        void DoUpdateNotebookStyle(wxWindow* win);
    private:
        wxWindow* m_window;
    };

    class _gsgsAPI GenericSTCStyler : public wxEvtHandler
    {
    public:
        enum eStyles { kDefault = 0, kInfo, kWarning, kError, kLastStyle };
    public:
        GenericSTCStyler(wxStyledTextCtrl* stc);
        virtual ~GenericSTCStyler();

        typedef wxSharedPtr<GenericSTCStyler> Ptr_t;
        /**
         * @brief register keywords for a known style
         */
        void AddStyle(const wxArrayString& words, GenericSTCStyler::eStyles style);
        /**
         * @brief register a user style for a given phrases
         */
        void AddUserStyle(const wxArrayString& words, const wxColour& fgColour, const wxColour& bgColour = wxColour());

        /**
         * @brief apply the user provided styles
         */
        void ApplyStyles();

        /**
         * @brief clear the current styles
         */
        void ClearAllStyles();
    protected:
        void OnStyleNeeded(wxStyledTextEvent& event);
        void OnThemChanged(wxCommandEvent& event);
        bool GetNextLine(const wxString& inText, wxString& lineText) const;
        int GetStyleForLine(const wxString& lineText) const;
        void ResetStyles();
        void InitDefaultStyles();
    private:
        wxStyledTextCtrl* m_ctrl;
        std::vector<std::pair<wxString, int> > m_words;
        std::vector<std::tuple<int, wxColour, wxColour> > m_styleInfo;
        int m_nextAvailStyle;
    };

    class _gsgsAPI AuiCaptionEnabler : public wxEvtHandler
    {
    public:
        AuiCaptionEnabler();
        virtual ~AuiCaptionEnabler();
        void Initialize(wxWindow* window, const wxString& windowCaption, wxAuiManager* aui);
    protected:
        void OnTimer(wxTimerEvent& event);
        void OnEditorSettingsChanged(wxCommandEvent& event);
    private:
        wxAuiManager* m_aui;
        wxWindow* m_window;
        wxTimer* m_timer;
        wxString m_caption;
        bool m_ensureCaptionsVisible;
        bool m_captionAdded;
    };

    class _gsgsAPI FindResultsStyler : public wxEvtHandler
    {
    public:
        enum eState {
            kHeader,
            kStartOfLine,
            kFile,
            kLineNumber,
            kScope,
            kMatch,
        };

        enum {
            // Custom styles
            LEX_FIF_DEFAULT = 0,
            LEX_FIF_FILE,
            LEX_FIF_MATCH,
            LEX_FIF_LINE_NUMBER,
            LEX_FIF_HEADER,
            LEX_FIF_SCOPE,
            LEX_FIF_MATCH_COMMENT,
        };
        typedef SharedPtr<FindResultsStyler> Ptr_t;
    public:
        FindResultsStyler();
        FindResultsStyler(wxStyledTextCtrl* stc);
        virtual ~FindResultsStyler();

        /**
         * @brief reset the styler
         */
        virtual void Reset();
        
        /**
         * @brief style the control text
         */
        virtual void StyleText(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, bool hasSope);

        /**
         * @brief initialize the control styles and colours
         */
        virtual void SetStyles(wxStyledTextCtrl* ctrl);

        /**
         * @brief user clicked on the editor, return the matching style (LEX_FIF_*)
         * @return return the clicked line + the style on that line
         */
        virtual int HitTest(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e, int& line);
        
        /**
         * @brief same as above, but uses the wxSTC passed in the Ctor
         */
        virtual int HitTest(wxStyledTextEvent& e, int& line);

        /**
         * @brief check whether a "Togglable" style was clicked
         * @return return the line number that was clicked if the style on that line is a togglable style
         * otherwise return wxNOT_FOUND
         */
        virtual int TestToggle(wxStyledTextCtrl* ctrl, wxStyledTextEvent& e);
    protected:
        void OnStyleNeeded(wxStyledTextEvent& e);
    protected:
        wxStyledTextCtrl* m_stc;
        eState m_curstate;
    };

    class _gsgsAPI WorkspaceView : public wxEvtHandler
    {
    public:
        WorkspaceView(wxSimplebook* book);
        virtual ~WorkspaceView();
        
        /**
         * @brief return the default page to show
         */
        inline const wxString& GetDefaultPage() const { return m_defaultPage; }
        
        /**
         * @brief set the default page to show
         */
        inline void SetDefaultPage(const wxString& page) { m_defaultPage = page; }
        
        /**
         * @brief add custom page to the inner notebook
         * @note adding a page does not select it
         */
        void AddPage(wxWindow* page, const wxString& name, bool book = true);

        /**
         * @brief return page with given name
         */
        wxWindow* GetPage(const wxString& name) const;

        /**
         * @brief select the current page to display
         * @param name
         */
        void SelectPage(const wxString& name);

        /**
         * @brief remove page from the inner notebook
         * @note if page with name does not exist, do nothing
         */
        void RemovePage(const wxString& name);
        
        /**
         * @brief return the underlying book control mananged by the view
         */
        inline wxSimplebook* GetBook() { return m_simpleBook; }
        
        /**
         * @brief return all pages 
         */
        void GetAllPages(std::map<wxString, wxWindow*> & outlist) const;
    protected:
        /**
         * @brief return the index of a give page by its name.
         * @param name the page name
         * @return page index or wxString::npos on failure
         */
        size_t GetPageIndex(const wxString& name) const;
        
        /**
         * @brief a workspace was closed
         */
        void OnWorkspaceClosed(wxCommandEvent &event);
    private:
        wxSimplebook* m_simpleBook;
        wxString m_defaultPage;
        std::map<wxString, wxWindow*> m_windows;
    };

    class _gsgsAPI ProfileHandler : public wxEvtHandler
    {
    public:
        static ProfileHandler& Get();
    protected:
        ProfileHandler();
        virtual ~ProfileHandler();
            
        void OnWorkspaceClosed(wxCommandEvent& e);
        void OnWorkspaceLoaded(wxCommandEvent& e);
        void OnGoingDown(Event& e);
        void HandleWorkspaceTabs(bool show);
        void HandleOutputTabs(bool show);

        bool IsPageExistsInBook(Notebook* book, const wxString& label) const;
        void RestoreTabs(gsgsStringSet& tabs, wxEventType eventType);
        void HideTabs(const gsgsStringSet& candidates, Notebook* book, wxEventType eventType, gsgsStringSet& tabsHidden);

    private:
        gsgsStringSet m_cxxOutputTabs;
        gsgsStringSet m_cxxWorkspaceTabs;
        gsgsStringSet m_cxxOutputTabsToRestore;
        gsgsStringSet m_cxxWorkspaceTabsToRestore;
    };

    class _gsgsAPI JSCTags : public wxEvtHandler
    {
    public:
        typedef SharedPtr<JSCTags> Ptr_t;
    public:
        JSCTags();
        virtual ~JSCTags();

        void ZipExtractCompleted();
        void OnEditorSaved(Event& event);
        void OnEditorChanged(wxCommandEvent& event);
        void OnInitDone(wxCommandEvent& event);
        void OnEditorClosing(wxCommandEvent& e);
    private:
        bool m_zipExtracted;
    };

    class _gsgsAPI TabTogglerHelper : public wxEvtHandler
    {
    public:
        typedef wxSharedPtr<TabTogglerHelper> Ptr_t;
    public:
        TabTogglerHelper(const wxString& outputTabName, wxWindow* outputTab, const wxString& workspaceTabName, wxWindow* workspaceTab);
        virtual ~TabTogglerHelper();

        inline void SetOutputTab(wxWindow* outputTab)
        {
            m_outputTab = outputTab;
        }
        inline void SetOutputTabBmp(const wxBitmap& outputTabBmp)
        {
            m_outputTabBmp = outputTabBmp;
        }
        inline void SetOutputTabName(const wxString& outputTabName)
        {
            m_outputTabName = outputTabName;
        }
        inline void SetWorkspaceTab(wxWindow* workspaceTab)
        {
            m_workspaceTab = workspaceTab;
        }
        inline void SetWorkspaceTabBmp(const wxBitmap& workspaceTabBmp)
        {
            m_workspaceTabBmp = workspaceTabBmp;
        }
        inline void SetWorkspaceTabName(const wxString& workspaceTabName)
        {
            m_workspaceTabName = workspaceTabName;
        }

        inline wxWindow * GetOutputTab() { return m_outputTab; }
        
        inline const wxBitmap& GetOutputTabBmp() const { return m_outputTabBmp; }
        
        inline const wxString& GetOutputTabName() const { return m_outputTabName; }
        
        inline wxWindow* GetWorkspaceTab() { return m_workspaceTab; }
        
        inline const wxBitmap& GetWorkspaceTabBmp() const { return m_workspaceTabBmp; }
        
        inline const wxString& GetWorkspaceTabName() const { return m_workspaceTabName; }
        
        /**
         * @brief return true if "tabname" is in the notebook (does not matter if it is focused or not)
         * @return wxNOT_FOUND if the tab does not exists, otherwise, return the tab index
         */
        static int IsTabInNotebook(Notebook* book, const wxString& tabname);
    protected:
        void OnToggleOutputTab(Event& event);
        void OnToggleWorkspaceTab(Event& event);
    private:
        wxString m_outputTabName;
        wxString m_workspaceTabName;
        wxWindow * m_outputTab;
        wxWindow * m_workspaceTab;
        wxBitmap m_outputTabBmp;
        wxBitmap m_workspaceTabBmp;
    };

    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_COMPILE_COMMANDS_JSON_GENERATED, Event);

    class _gsgsAPI CompileCommandsGenerator : public wxEvtHandler
    {
    public:
        typedef wxSharedPtr<CompileCommandsGenerator> Ptr_t;
        CompileCommandsGenerator();
        virtual ~CompileCommandsGenerator();

        void GenerateCompileCommands();
    protected:
        void OnProcessTeraminated(gsgs::ProcessEvent& event);
    private:
        IProcess* m_process = nullptr;
        wxFileName m_outputFile;
    };

    class _gsgsAPI TreeKeyboardInput : public wxEvtHandler
    {
    public:
        typedef wxSharedPtr<TreeKeyboardInput> Ptr_t;
    public:
        TreeKeyboardInput(TreeCtrl* tree);
        virtual ~TreeKeyboardInput();
    protected:
        void OnSearch(wxTreeEvent& event);
        void OnClearSearch(wxTreeEvent& event);
    protected:
        TreeCtrl* m_tree = nullptr;
    };

    class _gsgsAPI MainAuiTBArt : public wxAuiDefaultToolBarArt, public wxEvtHandler
    {
    public:
        MainAuiTBArt(bool isMainBook = false);
        virtual ~MainAuiTBArt();
        wxAuiToolBarArt* Clone() { return new MainAuiTBArt(); }
        virtual void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);
        virtual void DrawPlainBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect);
        virtual void DrawDropDownButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect);
        virtual void DrawButton(wxDC& dc, wxWindow* wnd, const wxAuiToolBarItem& item, const wxRect& rect);
        virtual void DrawGripper(wxDC& dc, wxWindow* wnd, const wxRect& rect);
        void OnTheme(wxCommandEvent& event);
    private:
        bool m_isMainBook;
    };

    class WindowManager;

    class _gsgsAPI BitmapOverlayCtrl : public wxEvtHandler
    {
    public:
        typedef SharedPtr<BitmapOverlayCtrl> Ptr_t;
    public:
        BitmapOverlayCtrl(wxWindow* win, const wxString& bitmapname, size_t flags);
        ~BitmapOverlayCtrl();
    protected:
        void DoPosition();
        void OnSize(wxSizeEvent& event);
    private:
        wxWindow* m_win;
        wxString m_name;
        size_t m_flags;
        wxStaticBitmap* m_bmpWindow;
        wxBitmap m_bmp;
    };

    class _gsgsAPI IncludePathLocator : public wxEvtHandler {
    public:
        IncludePathLocator(IManager *mgr);
        ~IncludePathLocator();

        void Locate(wxArrayString &paths, wxArrayString &excludePaths, bool thirdPartyLibs = true, const wxString &tool = wxString());
    private:
        IManager *m_mgr;
    };
}
#endif