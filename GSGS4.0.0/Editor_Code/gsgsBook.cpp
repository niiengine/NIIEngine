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

#include "gsgsBook.h"
#include "NotebookNavigationDlg.h"
#include "gsgsTagManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsLogManager.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsPluginManager.h"
#include "gsgsQuickFindBar.h"
#include "gsgsImageManager.h"
#include "gsgsPluginCommon.h"
#include "CommonController1.h"
#include "CommonController.h"
#include "findreplacedlg.h"
#include "gsgsFrame.h"
#include "gsgsPresent.h"
#include "gsgsManager.h"
#include "globals.h"
#include "ieditor.h"
#include "imanager.h"
#include "wxcrafter_plugin.h"
#include <wx/clntdata.h>
#include <wx/aui/framemanager.h>
#include <wx/regex.h>
#include <wx/wupdlock.h>
#include <wx/xrc/xmlres.h>
#include <wx/dcbuffer.h>
#include <wx/settings.h>

namespace gsgs
{
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // ImageViewer
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    class ImageViewer : public wxPanel
    {
    public:
        ImageViewer::ImageViewer(wxWindow* parent, const wxFileName& filename) :
            m_filename(filename)
        {
            Hide();
            if (!wxPanel::Create(parent, wxID_ANY, wxDefaultPosition, wxSize(500, 300), wxTAB_TRAVERSAL, wxT("ImageViewer")))
                return;
            wxBoxSizer* boxSizer194 = new wxBoxSizer(wxVERTICAL);
            this->SetSizer(boxSizer194);

            m_scrollWin196 = new DefaultScrolledWindow(this, wxID_ANY, wxDefaultPosition, wxDLG_UNIT(this, wxSize(-1, -1)),
                wxBORDER_NONE);
            m_scrollWin196->SetScrollRate(5, 5);

            boxSizer194->Add(m_scrollWin196, 1, wxEXPAND, 0);
            wxPanel * temp = new wxPanel(m_scrollWin196, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxCLIP_CHILDREN);
            m_scrollWin196->setClient(temp);
            wxBoxSizer* boxSizer198 = new wxBoxSizer(wxVERTICAL);
            temp->SetSizer(boxSizer198);

            m_staticBitmap = new wxStaticBitmap(temp, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0);

            boxSizer198->Add(m_staticBitmap, 0, wxALL | wxALIGN_CENTER_HORIZONTAL, 0);

            wxBitmap bmp;
            if(bmp.LoadFile(filename.GetFullPath(), wxBITMAP_TYPE_ANY)) {
                m_bitmap = bmp;
                m_staticBitmap->SetBitmap(m_bitmap);
            }
        }

        wxStaticBitmap* GetStaticBitmap() { return m_staticBitmap; }
        DefaultScrolledWindow* GetScrollWin196() { return m_scrollWin196; }
    protected:
        DefaultScrolledWindow* m_scrollWin196;
        wxStaticBitmap* m_staticBitmap;
        wxFileName m_filename;
        wxBitmap m_bitmap;
    };
    //----------------------------------------------------------------------------
    bool BookAskUserToSave(Editor* editor)
    {
        if (!editor || !editor->GetModify())
            return true;

        // unsaved changes
        wxString msg;
        msg << _("Save changes to '") << editor->GetFileName().GetFullName() << wxT("' ?");
        long style = wxYES_NO;
        if (!gsgs_Manager()->IsShutdownInProgress()) { style |= wxCANCEL; }

        int answer = wxMessageBox(msg, _("Confirm"), style, gsgs_Frame());
        switch (answer)
        {
        case wxYES:
            return editor->SaveFile();
        case wxNO:
            editor->SetSavePoint();
            return true;
        case wxCANCEL:
            return false;
        }

        return true;
    }
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    // Book
    //----------------------------------------------------------------------------
    //----------------------------------------------------------------------------
    Book::Book(wxWindow* parent, wxWindowID winid, const wxPoint& pos,
        const wxSize& size, long style, const wxString& name) :
        wxPanel(parent, winid, pos, size, wxTAB_TRAVERSAL | wxNO_BORDER, name),
        m_navBar(NULL), 
        m_useBuffereLimit(true), 
        m_isWorkspaceReloading(false), 
        m_reloadingDoRaise(true), 
        mWelcome(NULL), 
        m_findBar(NULL)
    {
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        gsgs_Style().RegisterWindow(this);
        m_filesModifiedDlg = new FilesModifiedDlg(gsgs_Frame());
        wxBoxSizer* sz = new wxBoxSizer(wxVERTICAL);
        SetSizer(sz);
        long ostyle = kNotebook_AllowDnD |                  // Allow tabs to move
            kNotebook_MouseMiddleClickClosesTab | // Handle mouse middle button when clicked on a tab
            kNotebook_MouseMiddleClickFireEvent | // instead of closing the tab, fire an event
            kNotebook_ShowFileListButton |        // show drop down list of all open tabs
            kNotebook_EnableNavigationEvent |     // Notify when user hit Ctrl-TAB or Ctrl-PGDN/UP
            kNotebook_UnderlineActiveTab |        // Mark active tab with dedicated coloured line
            kNotebook_DynamicColours |             // The tabs colour adjust to the editor's theme
            kNotebook_AllowForeignDnD;
        if(gsgs_EditorConfig().GetOptions()->IsTabHasXButton())
            ostyle |= (kNotebook_CloseButtonOnActiveTabFireEvent | kNotebook_CloseButtonOnActiveTab);

        if(gsgs_EditorConfig().GetOptions()->IsMouseScrollSwitchTabs()) 
            ostyle |= kNotebook_MouseScrollSwitchTabs;

        m_book = new Notebook(this, wxID_ANY, wxDefaultSize, wxDefaultPosition, wxDefaultSize, ostyle, style);
        sz->Add(m_book, 1, wxEXPAND);

        m_book->Bind(wxEVT_BOOK_PAGE_CLOSING, &Book::OnPageClosing, this);
        m_book->Bind(wxEVT_BOOK_PAGE_CLOSED, &Book::OnPageClosed, this);
        m_book->Bind(wxEVT_BOOK_PAGE_CHANGED, &Book::OnPageChanged, this);
        m_book->Bind(wxEVT_BOOK_PAGE_CHANGING, &Book::OnPageChanging, this);
        m_book->Bind(wxEVT_BOOK_PAGE_CLOSE_BUTTON, &Book::OnClosePage, this);
        m_book->Bind(wxEVT_BOOK_TABAREA_DCLICKED, &Book::OnMouseDClick, this);
        m_book->Bind(wxEVT_BOOK_TAB_DCLICKED, &Book::OnTabDClicked, this);
        m_book->Bind(wxEVT_BOOK_TAB_CONTEXT_MENU, &Book::OnTabLabelContextMenu, this);

        gsgs_Event().Connect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(Book::OnWorkspaceLoaded), NULL, this);
        gsgs_Event().Connect(wxEVT_PROJ_FILE_ADDED, gsgsEventHandler(Book::OnProjectFileAdded), NULL, this);
        gsgs_Event().Connect(wxEVT_PROJ_FILE_REMOVED, gsgsEventHandler(Book::OnProjectFileRemoved), NULL, this);
        gsgs_Event().Connect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(Book::OnWorkspaceClosed), NULL, this);
        //gsgs_Event().Connect(wxEVT_INIT_DONE, wxCommandEventHandler(Book::OnInitDone), NULL, this);
        gsgs_Event().Bind(wxEVT_DEBUG_ENDED, &Book::OnDebugEnded, this);
        gsgs_Event().Bind(wxEVT_DETACHED_EDITOR_CLOSED, &Book::OnDetachedEditorClosed, this);
        gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &Book::OnTheme, this);
        gsgs_Event().Bind(wxEVT_EDITOR_CONFIG_CHANGED, &Book::OnEditorSettingsChanged, this);
        gsgs_Event().Bind(wxEVT_CXX_SYMBOLS_CACHE_UPDATED, &Book::OnCacheUpdated, this);
        gsgs_Event().Bind(wxEVT_CC_UPDATE_NAVBAR, &Book::OnUpdateNavigationBar, this);
        gsgs_Event().Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &Book::OnColoursAndFontsChanged, this);
        gsgs_Event().Bind(wxEVT_NAVBAR_SCOPE_MENU_SHOWING, &Book::OnNavigationBarMenuShowing, this);
        gsgs_Event().Bind(wxEVT_NAVBAR_SCOPE_MENU_SELECTION_MADE, &Book::OnNavigationBarMenuSelectionMade, this);
        gsgs_Event().Bind(wxEVT_EDITOR_SETTINGS_CHANGED, &Book::OnSettingsChanged, this);
    }
    //----------------------------------------------------------------------------
    Book::~Book()
    {
        wxDELETE(m_filesModifiedDlg);
        m_book->Unbind(wxEVT_BOOK_PAGE_CLOSING, &Book::OnPageClosing, this);
        m_book->Unbind(wxEVT_BOOK_PAGE_CLOSED, &Book::OnPageClosed, this);
        m_book->Unbind(wxEVT_BOOK_PAGE_CHANGED, &Book::OnPageChanged, this);
        m_book->Unbind(wxEVT_BOOK_PAGE_CHANGING, &Book::OnPageChanging, this);
        m_book->Unbind(wxEVT_BOOK_PAGE_CLOSE_BUTTON, &Book::OnClosePage, this);
        m_book->Unbind(wxEVT_BOOK_TABAREA_DCLICKED, &Book::OnMouseDClick, this);
        m_book->Unbind(wxEVT_BOOK_TAB_DCLICKED, &Book::OnTabDClicked, this);
        m_book->Unbind(wxEVT_BOOK_TAB_CONTEXT_MENU, &Book::OnTabLabelContextMenu, this);

        gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &Book::OnTheme, this);
        gsgs_Event().Disconnect(wxEVT_WORKSPACE_LOADED, wxCommandEventHandler(Book::OnWorkspaceLoaded), NULL, this);
        gsgs_Event().Disconnect(wxEVT_PROJ_FILE_ADDED, gsgsEventHandler(Book::OnProjectFileAdded), NULL, this);
        gsgs_Event().Disconnect(wxEVT_PROJ_FILE_REMOVED, gsgsEventHandler(Book::OnProjectFileRemoved), NULL, this);
        gsgs_Event().Disconnect(wxEVT_WORKSPACE_CLOSED, wxCommandEventHandler(Book::OnWorkspaceClosed), NULL, this);
        //gsgs_Event().Disconnect(wxEVT_INIT_DONE, wxCommandEventHandler(Book::OnInitDone), NULL, this);
        gsgs_Event().Unbind(wxEVT_DEBUG_ENDED, &Book::OnDebugEnded, this);
        gsgs_Event().Unbind(wxEVT_DETACHED_EDITOR_CLOSED, &Book::OnDetachedEditorClosed, this);
        gsgs_Event().Unbind(wxEVT_EDITOR_CONFIG_CHANGED, &Book::OnEditorSettingsChanged, this);
        gsgs_Event().Unbind(wxEVT_CXX_SYMBOLS_CACHE_UPDATED, &Book::OnCacheUpdated, this);
        gsgs_Event().Unbind(wxEVT_CC_UPDATE_NAVBAR, &Book::OnUpdateNavigationBar, this);
        gsgs_Event().Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &Book::OnColoursAndFontsChanged, this);
        gsgs_Event().Unbind(wxEVT_NAVBAR_SCOPE_MENU_SHOWING, &Book::OnNavigationBarMenuShowing, this);
        gsgs_Event().Unbind(wxEVT_NAVBAR_SCOPE_MENU_SELECTION_MADE, &Book::OnNavigationBarMenuSelectionMade, this);
        gsgs_Event().Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, &Book::OnSettingsChanged, this);
        if(m_findBar) 
        {
            gsgs_Event().Unbind(wxEVT_ALL_EDITORS_CLOSED, &Book::OnAllEditorClosed, this);
            gsgs_Event().Unbind(wxEVT_ACTIVE_EDITOR_CHANGED, &Book::OnEditorChanged, this);
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnMouseDClick(wxBookCtrlEvent&)
    {
        //wxUnusedVar(e);
        NewEditor();
    }
    //----------------------------------------------------------------------------
    void Book::OnPageClosing(wxBookCtrlEvent& e)
    {
        e.Skip();

        //Editor * editor = dynamic_cast<Editor *>(m_book->GetPage(e.GetSelection()));
        EditorScrolledPanel * esp = dynamic_cast<EditorScrolledPanel *>(m_book->GetPage(e.GetSelection()));
        if(esp)
        {
            Editor * editor = esp->getEditor();
            if(BookAskUserToSave(editor))
            {
                SendCmdEvent(wxEVT_EDITOR_CLOSING, (IEditor*)editor);
            } 
            else 
            {
                e.Veto();
            }
        } 
        else 
        {
            // Unknown type, ask the plugins - maybe they know about this type
            wxNotifyEvent closeEvent(wxEVT_NOTIFY_PAGE_CLOSING);
            closeEvent.SetClientData(m_book->GetPage(e.GetSelection()));
            gsgs_Event().ProcessEvent(closeEvent);
            if(!closeEvent.IsAllowed()) 
                e.Veto();
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnPageClosed(wxBookCtrlEvent& e)
    {
        e.Skip();

        if(m_book->GetPageCount() == 0)
        {
            if (!gsgs_Manager()->IsWorkspaceOpen())
            {
                SendCmdEvent(wxEVT_ALL_EDITORS_CLOSED);
                ShowQuickBar(false);
                showPresent(true);
            }
            else
            {
                gsgs_Frame()->SetFrameTitle(0);
            }
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnProjectFileAdded(Event& e)
    {
        e.Skip();
        const wxArrayString & files = e.GetStrings();
        for(size_t i = 0; i < files.GetCount(); ++i) 
        {
            Editor * editor = FindEditor(files.Item(i));
            if(editor) 
            {
                wxString fileName = editor->GetFileName().GetFullPath();
                if(files.Index(fileName) != wxNOT_FOUND) 
                {
                    editor->SetProject(gsgs_Manager()->GetProjectNameByFile(fileName));
                }
            }
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnProjectFileRemoved(Event& e)
    {
        e.Skip();
        const wxArrayString& files = e.GetStrings();
        for(size_t i = 0; i < files.GetCount(); ++i) 
        {
            Editor* editor = FindEditor(files.Item(i));
            if(editor && files.Index(editor->GetFileName().GetFullPath()) != wxNOT_FOUND) 
            {
                editor->SetProject(wxEmptyString);
            }
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnWorkspaceLoaded(wxCommandEvent& e)
    {
        e.Skip();
        CloseAll(false, false); // get ready for session to be restored by clearing out existing pages
        showPresent(false);
    }
    //----------------------------------------------------------------------------
    void Book::OnWorkspaceClosed(wxCommandEvent& e)
    {
        e.Skip();
        CloseAll(false);
        StatusBar* sb = GetManager()->GetStatusBar();
        if(sb) 
        { 
            sb->SetSourceControlBitmap(wxNullBitmap, "", ""); 
        }
    }
    //----------------------------------------------------------------------------
    void Book::ClearFileHistory()
    {
        size_t count = m_recentFiles.GetCount();
        for(size_t i = 0; i < count; i++) 
        {
            m_recentFiles.RemoveFileFromHistory(0);
        }
        gsgs_Config().ClearRecentOpen();
    }
    //----------------------------------------------------------------------------
    void Book::UpdateNavBar(Editor* editor)
    {
        TagEntryPtr tag = NULL;
        if(m_navBar && m_navBar->IsShown()) {
            if(editor && !editor->GetProject().IsEmpty()) {
                TagEntryPtrList tags;
                if(TagsManagerST::Get()->GetFileCache()->Find(editor->GetFileName(), tags,
                                                              CxxFileCacheSymbols::kFunctions)) {
                    // the tags are already sorted by line
                    // find the entry that is closest to the current line

                    // lower_bound: find the first entry that !(entry < val)
                    TagEntryPtr dummy(new TagEntry());
                    dummy->SetLine(editor->GetCurrentLine() + 2);
                    TagEntryPtrList::iterator iter =
                        std::lower_bound(tags.begin(), tags.end(), dummy,
                                         [&](TagEntryPtr a, TagEntryPtr b) { return a->GetLine() < b->GetLine(); });
                    if(iter != tags.end()) {
                        if(iter != tags.begin()) {
                            std::advance(iter, -1); // we want the previous match
                        }
                        // we got a match
                        tag = (*iter);
                    } else if(!tags.empty()) {
                        // take the last element
                        tag = tags.back();
                    }
                } else {
                    TagsManagerST::Get()->GetFileCache()->RequestSymbols(editor->GetFileName());
                }
            }
            if(tag) {
                m_navBar->SetMessage(tag->GetScope(), tag->GetName());
            } else {
                m_navBar->SetMessage("", "");
            }
        }
    }
    //----------------------------------------------------------------------------
    void Book::ShowNavBar(bool s)
    {
        if(m_navBar) 
        {
            m_navBar->DoShow(s);
            Editor* editor = GetActiveEditor();
            CHECK_PTR_RET(editor);

            // Update the navigation bar
            CodeCompleteEvent evtUpdateNavBar(wxEVT_CC_UPDATE_NAVBAR);
            evtUpdateNavBar.SetEditor(editor);
            evtUpdateNavBar.SetLineNumber(editor->GetCtrl()->GetCurrentLine());
            gsgs_Event().AddPendingEvent(evtUpdateNavBar);
        }
    }
    //----------------------------------------------------------------------------
    void Book::SaveSession(SessionEntry& session, wxArrayInt* excludeArr) 
    { 
        CreateSession(session, excludeArr); 
    }
    //----------------------------------------------------------------------------
    void Book::RestoreSession(SessionEntry& session)
    {
        if(session.GetTabInfoArr().empty()) return; // nothing to restore

        CloseAll(false);
        size_t sel = session.GetSelectedTab();
        const std::vector<TabInfoData> & vTabInfoArr = session.GetTabInfoArr();
        for(size_t i = 0; i < vTabInfoArr.size(); i++) 
        {
            const TabInfoData & ti = vTabInfoArr[i];
            m_reloadingDoRaise = (i == vTabInfoArr.size() - 1); // Raise() when opening only the last editor
            Editor * editor = OpenFile(ti.GetFileName());
            if(!editor) 
            {
                if(i < sel) 
                {
                    // have to adjust selected tab number because couldn't open tab
                    sel--;
                }
                continue;
            }

            editor->SetFirstVisibleLine(ti.GetFirstVisibleLine());
            editor->SetEnsureCaretIsVisible(editor->PositionFromLine(ti.GetCurrentLine()));
            editor->LoadMarkersFromArray(ti.GetBookmarks());
            editor->LoadCollapsedFoldsFromArray(ti.GetCollapsedFolds());
        }
        m_book->SetSelection(sel);
    }
    //----------------------------------------------------------------------------
    Editor* Book::GetActiveEditor(bool includeDetachedEditors)
    {
        if(includeDetachedEditors) 
        {
            EditorFrameList_t::iterator iter = m_detachedEditors.begin();
            for(; iter != m_detachedEditors.end(); ++iter) 
            {
                Editor * re = (*iter)->GetEditor()->getEditor();
                if(re->IsFocused())
                    return re;
            }
        }

        EditorScrolledPanel * esp = dynamic_cast<EditorScrolledPanel*>(GetCurrentPage());
        if (esp)
            return esp->getEditor();
        return NULL;
    }
    //----------------------------------------------------------------------------
    void Book::GetAllTabs(TabVec_t& tabs)
    {
        tabs.clear();
    #if USE_AUI_NOTEBOOK
        m_book->GetAllTabs(tabs);
    #else
        TabInfoVec_t tabsInfo;
        m_book->GetAllTabs(tabsInfo);

        // Convert into "Tab" array
        std::for_each(tabsInfo.begin(), tabsInfo.end(), [&](TabInfoPtr_t tabInfo) 
        {
            Tab t;
            t.bitmap = tabInfo->GetBitmap();
            t.text = tabInfo->GetLabel();
            t.window = tabInfo->GetWindow();

            //Editor* editor = dynamic_cast<Editor*>(t.window);
            EditorScrolledPanel * esp = dynamic_cast<EditorScrolledPanel*>(t.window);
            if(esp)
            {
                Editor* editor = esp->getEditor();
                t.isFile = true;
                t.isModified = editor->IsModified();
                t.filename = editor->GetFileName();
            }
            tabs.push_back(t);
        });
    #endif
    }
    //----------------------------------------------------------------------------
    void Book::GetAllEditors(EditorVec_t& editors, size_t flags)
    {
        editors.clear();
        if(!(flags & kGetAll_DetachedOnly)) 
        {
            // Collect booked editors
            if(!(flags & kGetAll_RetainOrder)) 
            {
                // Most of the time we don't care about the order the tabs are stored in
                for(size_t i = 0; i < m_book->GetPageCount(); i++) 
                {
                    //Editor* editor = dynamic_cast<Editor*>(m_book->GetPage(i));
                    EditorScrolledPanel * esp = dynamic_cast<EditorScrolledPanel*>(m_book->GetPage(i));
                    if(esp) { editors.push_back(esp->getEditor()); }
                }
            } else {
                for(size_t i = 0; i < m_book->GetPageCount(); i++) 
                {
                    //Editor* editor = dynamic_cast<Editor*>(m_book->GetPage(i));
                    EditorScrolledPanel * esp = dynamic_cast<EditorScrolledPanel*>(m_book->GetPage(i));
                    if(esp) { editors.push_back(esp->getEditor()); }
                }
            }
        }
        if((flags & kGetAll_IncludeDetached) || (flags & kGetAll_DetachedOnly)) {
            // Add the detached editors
            EditorFrameList_t::iterator iter = m_detachedEditors.begin();
            for(; iter != m_detachedEditors.end(); ++iter) {
                editors.push_back((*iter)->GetEditor()->getEditor());
            }
        }
    }
    //----------------------------------------------------------------------------
    Editor* Book::FindEditor(const wxString& fileName)
    {
        wxString unixStyleFile(fileName);
    #ifdef __WXMSW__
        unixStyleFile.Replace(wxT("\\"), wxT("/"));
    #endif
        // On gtk either fileName or the editor filepath (or both) may be (or their paths contain) symlinks
        wxString fileNameDest = RealPath(fileName);

        for(size_t i = 0; i < m_book->GetPageCount(); i++) {
            EditorScrolledPanel* esp = dynamic_cast<EditorScrolledPanel*>(m_book->GetPage(i));
            if(esp) {
                Editor * editor = esp->getEditor();
                wxString unixStyleFile(editor->GetFileName().GetFullPath());
                wxString nativeFile(unixStyleFile);
    #ifdef __WXMSW__
                unixStyleFile.Replace(wxT("\\"), wxT("/"));
    #endif

    #ifndef __WXMSW__
                // On Unix files are case sensitive
                if(nativeFile.Cmp(fileName) == 0 || unixStyleFile.Cmp(fileName) == 0 ||
                   unixStyleFile.Cmp(fileNameDest) == 0)
    #else
                // Compare in no case sensitive manner
                if(nativeFile.CmpNoCase(fileName) == 0 || unixStyleFile.CmpNoCase(fileName) == 0 ||
                   unixStyleFile.CmpNoCase(fileNameDest) == 0)
    #endif
                {
                    return editor;
                }

    #if defined(__WXGTK__)
                // Try again, dereferencing the editor fpath
                wxString editorDest = RealPath(unixStyleFile);
                if(editorDest.Cmp(fileName) == 0 || editorDest.Cmp(fileNameDest) == 0) { return editor; }
    #endif
            }
        }

        // try the detached editors
        EditorFrameList_t::iterator iter = m_detachedEditors.begin();
        for(; iter != m_detachedEditors.end(); ++iter) 
        {
            Editor * re = (*iter)->GetEditor()->getEditor();
            if (re->GetFileName().GetFullPath() == fileName)
                return re;
        }
        return NULL;
    }
    //----------------------------------------------------------------------------
    wxWindow* Book::FindPage(const wxString& text) const
    {
        for(size_t i = 0; i < m_book->GetPageCount(); i++) 
        {
            EditorScrolledPanel* esp = dynamic_cast<EditorScrolledPanel*>(m_book->GetPage(i));
            if (esp)
            {
                Editor* editor = esp->getEditor();
                if (editor && editor->GetFileName().GetFullPath().CmpNoCase(text) == 0)
                    return esp;
            }
            if(m_book->GetPageText(i) == text) 
                return m_book->GetPage(i);
        }
        return NULL;
    }
    //----------------------------------------------------------------------------
    Editor* Book::NewEditor()
    {
        static unsigned int fileCounter = 0;

        wxString fileNameStr(_("new "));
        fileNameStr << ++fileCounter;
        wxFileName fileName(fileNameStr);

        // A Nice trick: hide the notebook, open the editor
        // and then show it
        //bool hidden = false;
        //(m_book->GetPageCount() == 0) 
        //    hidden = GetSizer()->Hide(m_book);

        //Editor* editor = new Editor(m_book);
        EditorScrolledPanel * esp = new EditorScrolledPanel(m_book);
        Editor* editor = esp->getEditor();
        editor->SetFileName(fileName);
        editor->SetUseHorizontalScrollBar(false);
        editor->SetSyntaxHighlight(wxString("c++"));
        //AddPage(editor, fileName.GetFullName(), fileName.GetFullPath(), wxNullBitmap, true);
        AddPage(esp, fileName.GetFullName(), fileName.GetFullPath(), wxNullBitmap, true);

        //if(hidden) 
        //    GetSizer()->Show(m_book);

        return editor;
    }
    //----------------------------------------------------------------------------
    static bool IsFileExists(const wxFileName& filename)
    {
    //#ifdef __WXMSW__
        /*    wxString drive  = filename.GetVolume();
            if(drive.Length()>1)
                return false;*/

        return filename.FileExists();
    //#else
    //    return filename.FileExists();
    //#endif
    }
    //----------------------------------------------------------------------------
    Editor* Book::OpenFile(const wxString& file_name, const wxString& projectName, int lineno, long position,
                                 OF_extra extra /*=OF_AddJump*/, bool preserveSelection /*=true*/,
                                 const wxBitmap& bmp /*= wxNullBitmap*/, const wxString& tooltip /* wxEmptyString */)
    {
        wxFileName fileName(file_name);
        fileName.MakeAbsolute();

    #ifdef __WXMSW__
        // Handle cygwin paths
        wxString curpath = fileName.GetFullPath();
        static wxRegEx reCygdrive("/cygdrive/([A-Za-z])");
        if(reCygdrive.Matches(curpath)) {
            // Replace the /cygdrive/c with volume C:
            wxString volume = reCygdrive.GetMatch(curpath, 1);
            volume << ":";
            reCygdrive.Replace(&curpath, volume);
            fileName = curpath;
        }
    #endif

        if(!IsFileExists(fileName)) {
            gsgsDEBUG() << "Failed to open:" << fileName << ". No such file or directory";
            return NULL;
        }

        if(FileUtil::GetType(fileName.GetFullPath()) == FileUtil::TypeBmp) {
            // a bitmap file, open it using an image viewer
            DoOpenImageViewer(fileName);
            return NULL;
        }

        wxString projName = projectName;
        if(projName.IsEmpty()) {
            wxString filePath(fileName.GetFullPath());
            // try to match a project name to the file. otherwise, CC may not work
            projName = gsgs_Manager()->GetProjectNameByFile(filePath);
            // If (in Linux) the project contains a *symlink* this filepath, not the realpath,
            // filePath will have been changed by GetProjectNameByFile() to that symlink.
            // So update fileName in case
            fileName = wxFileName(filePath);
        }

        Editor* editor = GetActiveEditor(true);
        BrowseRecord jumpfrom = editor ? editor->CreateBrowseRecord() : BrowseRecord();

        editor = FindEditor(fileName.GetFullPath());
        if(editor) {
            editor->SetProject(projName);
        } else if(fileName.IsOk() == false) {
            gsgsLogMessage(wxT("Invalid file name: ") + fileName.GetFullPath());
            return NULL;

        } else if(!fileName.FileExists()) {
            gsgsLogMessage(wxT("File: ") + fileName.GetFullPath() + wxT(" does not exist!"));
            return NULL;

        } else {

            // A Nice trick: hide the notebook, open the editor
            // and then show it
            // hidden(false);
            //if(m_book->GetPageCount() == 0) 
            //    hidden = GetSizer()->Hide(m_book);

            //editor = new Editor(m_book);
            EditorScrolledPanel * esp = new EditorScrolledPanel(m_book);
            editor = esp->getEditor();
            editor->SetEditorBitmap(bmp);
            editor->Create(projName, fileName);

            int sel = m_book->GetSelection();
            if((extra & OF_PlaceNextToCurrent) && (sel != wxNOT_FOUND)) 
            {
                //AddPage(editor, fileName.GetFullName(), tooltip.IsEmpty() ? fileName.GetFullPath() : tooltip, bmp, false, sel + 1);
                AddPage(esp, fileName.GetFullName(), tooltip.IsEmpty() ? fileName.GetFullPath() : tooltip, bmp, false, sel + 1);
            } 
            else 
            {
                //AddPage(editor, fileName.GetFullName(), tooltip.IsEmpty() ? fileName.GetFullPath() : tooltip, bmp);
                AddPage(esp, fileName.GetFullName(), tooltip.IsEmpty() ? fileName.GetFullPath() : tooltip, bmp);
            }
            editor->SetSyntaxHighlight();
            gsgs_Manager()->GetBreakpointsMgr()->RefreshBreakpointsForEditor(editor);

            // mark the editor as read only if needed
            MarkEditorReadOnly(editor, false);

            // Show the notebook
            //if(hidden) GetSizer()->Show(m_book);

            /*if(position == wxNOT_FOUND && lineno == wxNOT_FOUND && editor->GetContext()->GetName() == wxT("C++")) {
                // try to find something interesting in the file to put the caret at
                // for now, just skip past initial blank lines and comments
                for(lineno = 0; lineno < editor->GetLineCount(); lineno++) {
                    switch(editor->GetStyleAt(editor->PositionFromLine(lineno))) {
                    case wxSTC_C_DEFAULT:
                    case wxSTC_C_COMMENT:
                    case wxSTC_C_COMMENTDOC:
                    case wxSTC_C_COMMENTLINE:
                    case wxSTC_C_COMMENTLINEDOC:
                        continue;
                    }
                    // if we got here, it's a line to stop on
                    break;
                }
                if(lineno == editor->GetLineCount()) {
                    lineno = 1; // makes sure a navigation record gets saved
                }
            }*/
        }

        if(position != wxNOT_FOUND) {
            editor->SetEnsureCaretIsVisible(position, preserveSelection);
            editor->SetLineVisible(editor->LineFromPosition(position));

        } else if(lineno != wxNOT_FOUND) {
            editor->CenterLine(lineno);
        }

        if(m_reloadingDoRaise) {
            if(GetActiveEditor() == editor) {
                editor->SetActive();
            } else {
                SelectPage(editor->GetParent());
            }
        }

        // Add this file to the history. Don't check for uniqueness:
        // if it's already on the list, wxFileHistory will move it to the top
        // Also, sync between the history object and the configuration file
        m_recentFiles.AddFileToHistory(fileName.GetFullPath());
        gsgs_Config().AddRecentOpen(fileName.GetFullPath(), false);

        if(extra & OF_AddJump) {
            BrowseRecord jumpto = editor->CreateBrowseRecord();
            NavMgr::Get()->AddJump(jumpfrom, jumpto);
        }
        return editor;
    }
    //----------------------------------------------------------------------------
    bool Book::AddPage(wxWindow* win, const wxString& text, const wxString& tooltip, const wxBitmap& bmp, bool selected,
                           int insert_at_index /*=wxNOT_FOUND*/)
    {
        showPresent(false);
        if(m_book->GetPageIndex(win) != wxNOT_FOUND) 
            return false;
        if(insert_at_index == wxNOT_FOUND) 
        {
            m_book->AddPage(win, text, selected, bmp);
        } 
        else 
        {
            if(!m_book->InsertPage(insert_at_index, win, text, selected, bmp)) 
            {
                // failed to insert, append it
                m_book->AddPage(win, text, selected, bmp);
            }
        }

        if(tooltip.size()) 
            m_book->SetPageToolTip(m_book->GetPageIndex(win), tooltip);
        return true;
    }
    //----------------------------------------------------------------------------
    bool Book::SelectPage(wxWindow* win)
    {
        int index = m_book->GetPageIndex(win);
        if(index != wxNOT_FOUND && m_book->GetSelection() != index) 
        {
    #if USE_AUI_NOTEBOOK
            m_book->ChangeSelection(index);
    #else
            m_book->SetSelection(index);
    #endif
        }
        return DoSelectPage(win);
    }
    //----------------------------------------------------------------------------
    bool Book::UserSelectFiles(std::vector<std::pair<wxFileName, bool> >& files, const wxString& title,
                                   const wxString& caption, bool cancellable)
    {
        if(files.empty()) 
            return true;

        FileCheckList dlg(gsgs_Frame(), wxID_ANY, title);
        dlg.SetCaption(caption);
        dlg.SetFiles(files);
        dlg.SetCancellable(cancellable);
        bool res = dlg.ShowModal() == wxID_OK;
        files = dlg.GetFiles();
        return res;
    }
    //----------------------------------------------------------------------------
    bool Book::SaveAll(bool askUser, bool includeUntitled)
    {
        // turn the 'saving all' flag on so we could 'Veto' all focus events
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);

        std::vector<std::pair<wxFileName, bool> > files;
        size_t n = 0;
        for(size_t i = 0; i < editors.size(); i++) {
            if(!editors[i]->GetModify()) continue;

            if(!includeUntitled && !editors[i]->GetFileName().FileExists())
                continue; // don't save new documents that have not been saved to disk yet

            files.push_back(std::make_pair(editors[i]->GetFileName(), true));
            editors[n++] = editors[i];
        }
        editors.resize(n);

        bool res = !askUser || UserSelectFiles(files, _("Save Modified Files"),
                                               _("Some files are modified.\nChoose the files you would like to save."));
        if(res) {
            for(size_t i = 0; i < files.size(); i++) {
                if(files[i].second) { editors[i]->SaveFile(); }
            }
        }
        // And notify the plugins to save their tabs (this function only cover editors)
        Event saveAllEvent(wxEVT_SAVE_ALL_EDITORS);
        gsgs_Event().AddPendingEvent(saveAllEvent);
        return res;
    }
    //----------------------------------------------------------------------------
    void Book::ReloadExternallyModified(bool prompt)
    {
        if(m_isWorkspaceReloading) 
            return;
        static int depth = wxNOT_FOUND;
        ++depth;

        // Protect against recursion
        if(depth == 2) 
        {
            depth = wxNOT_FOUND;
            return;
        }

        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);

        time_t workspaceModifiedTimeBefore = gsgs_CppWorker().GetFileLastModifiedTime();

        // filter list of editors for any whose files have been modified
        std::vector<std::pair<wxFileName, bool> > files;
        size_t n = 0;
        for(size_t i = 0; i < editors.size(); i++) {
            time_t diskTime = editors[i]->GetFileLastModifiedTime();
            time_t editTime = editors[i]->GetEditorLastModifiedTime();
            if(diskTime != editTime) {
                // update editor last mod time so that we don't keep bugging the user over the same file,
                // unless it gets changed again
                editors[i]->SetEditorLastModifiedTime(diskTime);

                // A last check: see if the content of the file has actually changed. This avoids unnecessary reload offers
                // after e.g. git stash
                if(!CompareFileWithString(editors[i]->GetFileName().GetFullPath(), editors[i]->GetText())) {
                    files.push_back(std::make_pair(editors[i]->GetFileName(), !editors[i]->GetModify()));
                    editors[n++] = editors[i];
                }
            }
        }
        editors.resize(n);
        if(n == 0) return;

        if(prompt) {

            int res = gsgs_Config().GetAnnoyingDlgAnswer("FilesModifiedDlg", wxNOT_FOUND);
            if(res == wxID_CANCEL) {
                return; // User had previous ticked the 'Remember my answer' checkbox after he'd just chosen Ignore
            }

            if(res == wxNOT_FOUND) {
                // User hasn't previously ticked the 'Remember my answer' checkbox
                // Show the dialog
                res = GetFilesModifiedDlg()->ShowModal();

                if(GetFilesModifiedDlg()->GetRememberMyAnswer()) {
                    gsgs_Config().SetAnnoyingDlgAnswer("FilesModifiedDlg", res, false);
                }

                if(res == FilesModifiedDlg::kID_BUTTON_IGNORE) { return; }
            }

            if(res == FilesModifiedDlg::kID_BUTTON_CHOOSE) {
                UserSelectFiles(
                    files, _("Reload Modified Files"),
                    _("Files have been modified outside the editor.\nChoose which files you would like to reload."), false);
            }
        }

        time_t workspaceModifiedTimeAfter = gsgs_CppWorker().GetFileLastModifiedTime();
        if(workspaceModifiedTimeBefore != workspaceModifiedTimeAfter) {
            // a workspace reload occurred between the "Reload Modified Files" and
            // the "Reload WOrkspace" dialog, cancel this it's not needed anymore
            return;
        }

        EditorVec_t editorsAgain;
        GetAllEditors(editorsAgain, Book::kGetAll_IncludeDetached);

        // Make sure that the tabs that we have opened
        // are still available in the main book
        EditorVec_t realEditorsList;
        std::sort(editors.begin(), editors.end());
        std::sort(editorsAgain.begin(), editorsAgain.end());
        std::set_intersection(editorsAgain.begin(), editorsAgain.end(), editors.begin(), editors.end(),
                              std::back_inserter(realEditorsList));

        // Update the "files" list
        if(editors.size() != realEditorsList.size()) {
            // something went wrong here...
            CallAfter(&Book::ReloadExternallyModified, prompt);
            return;
        }

        // reset the recursive protector
        depth = wxNOT_FOUND;

        std::vector<wxFileName> filesToRetag;
        for(size_t i = 0; i < files.size(); i++) {
            if(files[i].second) {
                editors[i]->ReloadFromDisk(true);
                filesToRetag.push_back(files[i].first);
            }
        }
        if(filesToRetag.size() > 1) {
            TagsManagerST::Get()->RetagFiles(filesToRetag, TagsManager::Retag_Quick);
            SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&filesToRetag);

        } else if(filesToRetag.size() == 1) {
            gsgs_Manager()->RetagFile(filesToRetag.at(0).GetFullPath());
            SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&filesToRetag);
        }
    }
    //----------------------------------------------------------------------------
    bool Book::ClosePage(wxWindow* page)
    {
        int pos = m_book->GetPageIndex(page);
        return pos != wxNOT_FOUND && m_book->DeletePage(pos, true);
    }
    //----------------------------------------------------------------------------
    bool Book::CloseAllButThis(wxWindow* page)
    {
        wxString text;

        WindowUpdateLocker locker(this);

        int pos = m_book->GetPageIndex(page);
        if(pos != wxNOT_FOUND) 
        {
            text = m_book->GetPageText(pos);
            m_book->RemovePage(pos);
        }

        bool res = CloseAll(true);
        if(pos != wxNOT_FOUND) 
        { 
            m_book->AddPage(page, text, true); 
        }
        return res;
    }
    //----------------------------------------------------------------------------
    bool Book::CloseAll(bool cancellable, bool showpresent)
    {
        EditorVec_t editors;
        GetAllEditors(editors, kGetAll_IncludeDetached);

        // filter list of editors for any that need to be saved
        std::vector<std::pair<wxFileName, bool> > files;
        size_t n = 0;
        for(size_t i = 0; i < editors.size(); i++) {
            if(editors[i]->GetModify()) {
                files.push_back(std::make_pair(editors[i]->GetFileName(), true));
                editors[n++] = editors[i];
            }
        }
        editors.resize(n);

        if(!UserSelectFiles(files, _("Save Modified Files"),
                            _("Some files are modified.\nChoose the files you would like to save."), cancellable))
            return false;

        for(size_t i = 0; i < files.size(); i++) {
            if(files[i].second) {
                editors[i]->SaveFile();
            } else {
                editors[i]->SetSavePoint();
            }
        }

        // Delete the files without notifications (it will be faster)
        WindowUpdateLocker locker(this);
        SendCmdEvent(wxEVT_ALL_EDITORS_CLOSING);

        m_reloadingDoRaise = false;
        m_book->DeleteAllPages();
        m_reloadingDoRaise = true;

        // Delete all detached editors
        EditorFrameList_t::iterator iter = m_detachedEditors.begin();
        for(; iter != m_detachedEditors.end(); ++iter) {
            (*iter)->Destroy(); // Destroying the frame will release the editor
        }

        // Since we got no more editors opened,
        // send a wxEVT_ALL_EDITORS_CLOSED event
        SendCmdEvent(wxEVT_ALL_EDITORS_CLOSED);

        // Update the quick-find-bar
        GetManager()->SetStatusMessage("");

        // Update the frame's title
        gsgs_Frame()->SetFrameTitle(NULL);

        DoHandleFrameMenu(NULL);

        // OutputTabWindow::OnEditUI will crash on >=wxGTK-2.9.3 if we don't set the focus somewhere that still exists
        // This workaround doesn't seem to work if applied earlier in the function :/
        m_book->SetFocus();

        if(showpresent && m_book->GetPageCount() == 0)
        {        
            showPresent(true);
        }

        return true;
    }
    //----------------------------------------------------------------------------
    wxString Book::GetPageTitle(wxWindow* page) const
    {
        int selection = m_book->GetPageIndex(page);
        if(selection != wxNOT_FOUND) return m_book->GetPageText(selection);
        return wxEmptyString;
    }
    //----------------------------------------------------------------------------
    void Book::SetPageTitle(wxWindow* page, const wxString& name)
    {
        int selection = m_book->GetPageIndex(page);
        if(selection != wxNOT_FOUND) {
            m_book->SetPageText(selection, name);
        }
    }
    //----------------------------------------------------------------------------
    void Book::ApplySettings()
    {
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);
        for(size_t i = 0; i < editors.size(); i++) {
            editors[i]->SetSyntaxHighlight(editors[i]->GetContext()->GetName());
        }

        gsgs_Frame()->UpdateAUI();
        gsgs_Frame()->ShowOrHideCaptions();
    }
    //----------------------------------------------------------------------------
    void Book::UnHighlightAll()
    {
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);
        for(size_t i = 0; i < editors.size(); i++) {
            editors[i]->UnHighlightAll();
        }
    }
    //----------------------------------------------------------------------------
    void Book::DelAllBreakpointMarkers()
    {
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);
        for(size_t i = 0; i < editors.size(); i++) {
            editors[i]->DelAllBreakpointMarkers();
        }
    }
    //----------------------------------------------------------------------------
    void Book::SetViewEOL(bool visible)
    {
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);
        for(size_t i = 0; i < editors.size(); i++) {
            editors[i]->SetViewEOL(visible);
        }
    }
    //----------------------------------------------------------------------------
    void Book::HighlightWord(bool hl)
    {
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);
        for(size_t i = 0; i < editors.size(); i++) {
            editors[i]->HighlightWord(hl);
        }
    }
    //----------------------------------------------------------------------------
    void Book::ShowWhitespace(int ws)
    {
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);
        for(size_t i = 0; i < editors.size(); i++) {
            editors[i]->SetViewWhiteSpace(ws);
        }
    }
    //----------------------------------------------------------------------------
    void Book::UpdateColours()
    {
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);
        for(size_t i = 0; i < editors.size(); i++) {
            editors[i]->UpdateColours();
        }
    }
    //----------------------------------------------------------------------------
    void Book::UpdateBreakpoints()
    {
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);
        for(size_t i = 0; i < editors.size(); i++) {
            editors[i]->UpdateBreakpoints();
        }
        gsgs_Manager()->GetBreakpointsMgr()->RefreshBreakpointMarkers();
    }
    //----------------------------------------------------------------------------
    void Book::MarkEditorReadOnly(Editor* editor, bool refresh)
    {
        if (editor)
        {
            bool readOnly = (!editor->IsEditable()) || ::IsFileReadOnly(editor->GetFileName());
            if (readOnly && editor->GetModify()) {
                // an attempt to mark a modified file as read-only
                // ask the user to save his changes before
                ::wxMessageBox(_("Please save your changes before marking the file as read only"), "gsgs",
                    wxOK | wxCENTER | wxICON_WARNING, this);
                return;
            }

            wxBitmap lockBmp = ::gsgs_Image().LoadBitmap("lock");
            for (size_t i = 0; i < m_book->GetPageCount(); i++) {
                wxBitmap orig_bmp = editor->GetEditorBitmap();
                if (editor->GetParent() == m_book->GetPage(i)) {
                    wxWindowUpdateLocker templ(m_book);
                    m_book->SetPageBitmap(i, readOnly ? lockBmp : orig_bmp, refresh);
                    break;
                }
            }
        }
    }
    //----------------------------------------------------------------------------
    long Book::GetBookStyle() { return 0; }
    //----------------------------------------------------------------------------
    bool Book::DoSelectPage(wxWindow* win)
    {
        EditorScrolledPanel* esp = dynamic_cast<EditorScrolledPanel*>(win);
        Editor* editor = 0;
        if(esp)
            editor = esp->getEditor();
        if (editor)
        {
            editor->SetActive();

            wxStyledTextEvent dummy(wxEVT_STC_UPDATEUI);
            editor->GetEventHandler()->AddPendingEvent(dummy);
        }

        // Remove context menu if needed
        DoHandleFrameMenu(editor);

        if (editor)
        {
            wxCommandEvent event(wxEVT_ACTIVE_EDITOR_CHANGED);
            gsgs_Event().AddPendingEvent(event);
            UpdateNavBar(editor);
        }
        else
        {
            gsgs_Frame()->SetFrameTitle(NULL);
            gsgs_Frame()->GetMainStatusBar()->SetLinePosColumn(wxEmptyString);
            UpdateNavBar(NULL);
            SendCmdEvent(wxEVT_CMD_PAGE_CHANGED, win);
        } 

        return true;
    }
    //----------------------------------------------------------------------------
    void Book::OnPageChanged(wxBookCtrlEvent& e)
    {
        e.Skip();

        int newSel = e.GetSelection();
        if(newSel != wxNOT_FOUND && m_reloadingDoRaise)
        {
            wxWindow* win = m_book->GetPage((size_t)newSel);
            if(win) 
            { 
                SelectPage(win); 
            }
        }

        // Cancel any tooltip
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);
        for(size_t i = 0; i < editors.size(); ++i) 
        {
            // Cancel any calltip when switching from the editor
            editors.at(i)->DoCancelCalltip();
        }
    }
    //----------------------------------------------------------------------------
    void Book::DoUpdateNotebookTheme()
    {
        size_t initialStyle = m_book->GetStyle();
        size_t style = initialStyle;
        if(gsgs_EditorConfig().GetOptions()->IsTabColourMatchesTheme()) {
            // Update theme
            IEditor* editor = GetActiveEditor();
            if(editor) {
                wxColour bgColour = editor->GetCtrl()->StyleGetBackground(0);
                if(Colour::IsDark(bgColour) && !(m_book->GetStyle() & kNotebook_DarkTabs)) {
                    style &= ~kNotebook_LightTabs;
                    style |= kNotebook_DarkTabs;
                } else if(!Colour::IsDark(bgColour) && !(m_book->GetStyle() & kNotebook_LightTabs)) {
                    style &= ~kNotebook_DarkTabs;
                    style |= kNotebook_LightTabs;
                }
            }
        } else if(gsgs_EditorConfig().GetOptions()->IsTabColourDark()) {
            style &= ~kNotebook_LightTabs;
            style |= kNotebook_DarkTabs;
        } else {
            style &= ~kNotebook_DarkTabs;
            style |= kNotebook_LightTabs;
        }

        if(!gsgs_EditorConfig().GetOptions()->IsTabHasXButton()) {
            style &= ~(kNotebook_CloseButtonOnActiveTab | kNotebook_CloseButtonOnActiveTabFireEvent);
        } else {
            style |= (kNotebook_CloseButtonOnActiveTab | kNotebook_CloseButtonOnActiveTabFireEvent);
        }

        if(initialStyle != style) 
        {
            m_book->SetStyle(style); 
        }
    }
    //----------------------------------------------------------------------------
    wxWindow* Book::GetCurrentPage() const { return m_book->GetCurrentPage(); }
    //----------------------------------------------------------------------------
    int Book::GetCurrentPageIndex() const { return m_book->GetSelection(); }
    //----------------------------------------------------------------------------
    void Book::OnClosePage(wxBookCtrlEvent& e)
    {
        WindowUpdateLocker locker(this);
        int where = e.GetSelection();
        if(where == wxNOT_FOUND) 
        { 
            return; 
        }
        wxWindow* page = m_book->GetPage((size_t)where);
        if(page) 
        { 
            ClosePage(page); 
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnDebugEnded(DebugEvent& e) { e.Skip(); }
    //----------------------------------------------------------------------------
    void Book::DoHandleFrameMenu(Editor* editor) { wxUnusedVar(editor); }
    //----------------------------------------------------------------------------
    void Book::OnPageChanging(wxBookCtrlEvent& e)
    {
        Editor* editor = GetActiveEditor();
        if(editor) 
        { 
            editor->CallTipCancel();
        }
        e.Skip();
    }
    //----------------------------------------------------------------------------
    void Book::SetViewWordWrap(bool b)
    {
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_Default);
        for(size_t i = 0; i < editors.size(); i++) {
            editors[i]->SetWrapMode(b ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE);
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnInitDone(wxCommandEvent& e)
    {
        e.Skip();
        if(m_book->GetPageCount() == 0) 
        {
            showPresent(true);
        }
    }
    //----------------------------------------------------------------------------
    wxWindow* Book::GetPage(size_t page) 
    { 
        return m_book->GetPage(page); 
    }
    //----------------------------------------------------------------------------
    bool Book::ClosePage(const wxString& text)
    {
        int numPageClosed(0);
        bool closed = ClosePage(FindPage(text));
        while(closed) {
            ++numPageClosed;
            closed = ClosePage(FindPage(text));
        }
        return numPageClosed > 0;
    }
    //----------------------------------------------------------------------------
    size_t Book::GetPageCount() const { return m_book->GetPageCount(); }
    //----------------------------------------------------------------------------
    void Book::DetachActiveEditor()
    {
        Editor* editor = GetActiveEditor();
        if(editor) {
            m_book->RemovePage(m_book->GetSelection(), true);
            EditorFrame* frame = new EditorFrame(gsgs_Frame(), 
                static_cast<EditorScrolledPanel*>(editor->GetParent()), m_book->GetStyle());
            // Move it to the same position as the main frame
            frame->Move(gsgs_Event().TopFrame()->GetPosition());
            // And show it
            frame->Show();
            // frame->Raise();
            m_detachedEditors.push_back(frame);
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnDetachedEditorClosed(Event& e)
    {
        e.Skip();
        IEditor* editor = reinterpret_cast<IEditor*>(static_cast<EditorScrolledPanel *>(e.GetClientData())->getEditor());
        DoEraseDetachedEditor(editor);

        wxString alternateText = (editor && editor->IsModified()) ? editor->GetCtrl()->GetText() : "";
        // Open the file again in the main book
        CallAfter(&Book::DoOpenFile, e.GetFileName(), alternateText);
    }
    //----------------------------------------------------------------------------
    void Book::DoEraseDetachedEditor(IEditor* editor)
    {
        EditorFrameList_t::iterator iter = m_detachedEditors.begin();
        for(; iter != m_detachedEditors.end(); ++iter) {
            if((*iter)->GetEditor()->getEditor() == editor) {
                m_detachedEditors.erase(iter);
                break;
            }
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnWorkspaceReloadEnded(Event& e)
    {
        e.Skip();
        m_isWorkspaceReloading = false;
    }
    //----------------------------------------------------------------------------
    void Book::OnWorkspaceReloadStarted(Event& e)
    {
        e.Skip();
        m_isWorkspaceReloading = true;
    }
    //----------------------------------------------------------------------------
    void Book::ClosePageVoid(wxWindow* win) { ClosePage(win); }
    //----------------------------------------------------------------------------
    void Book::CloseAllButThisVoid(wxWindow* win) { CloseAllButThis(win); }
    //----------------------------------------------------------------------------
    void Book::CloseAllVoid(bool cancellable) { CloseAll(cancellable); }
    //----------------------------------------------------------------------------
    FilesModifiedDlg* Book::GetFilesModifiedDlg()
    {
        return m_filesModifiedDlg;
    }
    //----------------------------------------------------------------------------
    void Book::CreateSession(SessionEntry& session, wxArrayInt* excludeArr)
    {
        EditorVec_t editors;
        GetAllEditors(editors, kGetAll_RetainOrder);

        // Remove editors which belong to the SFTP
        EditorVec_t editorsTmp;
        std::for_each(editors.begin(), editors.end(), [&](Editor* editor) {
            IEditor* ieditor = dynamic_cast<IEditor*>(editor);
            if(ieditor->GetClientData("sftp") == NULL) { editorsTmp.push_back(editor); }
        });

        editors.swap(editorsTmp);

        session.SetSelectedTab(0);
        std::vector<TabInfoData> vTabInfoArr;
        for(size_t i = 0; i < editors.size(); i++) {

            if(excludeArr && (excludeArr->GetCount() > i) && (!excludeArr->Item(i))) {
                // If we're saving only selected editors, and this isn't one of them...
                continue;
            }

            if(editors[i] == GetActiveEditor()) { session.SetSelectedTab(vTabInfoArr.size()); }
            TabInfoData oTabInfo;
            oTabInfo.SetFileName(editors[i]->GetFileName().GetFullPath());
            oTabInfo.SetFirstVisibleLine(editors[i]->GetFirstVisibleLine());
            oTabInfo.SetCurrentLine(editors[i]->GetCurrentLine());

            wxArrayString astrBookmarks;
            editors[i]->StoreMarkersToArray(astrBookmarks);
            oTabInfo.SetBookmarks(astrBookmarks);

            std::vector<int> folds;
            editors[i]->StoreCollapsedFoldsToArray(folds);
            oTabInfo.SetCollapsedFolds(folds);

            vTabInfoArr.push_back(oTabInfo);
        }
        session.SetTabInfoArr(vTabInfoArr);

        // Set the "Find In Files" file mask for this workspace
        FindReplaceData frd;
        frd.SetName("FindInFilesData");
        gsgs_Config().ReadItem(&frd);
        session.SetFindInFilesMask(frd.GetSelectedMask());
    }
    //----------------------------------------------------------------------------
    void Book::ShowTabBar(bool b) { wxUnusedVar(b); }
    //----------------------------------------------------------------------------
    void Book::CloseTabsToTheRight(wxWindow* win)
    {
        wxString text;

        // WindowUpdateLocker locker(this);

        // Get list of tabs to close
        WindowList windows;
        bool currentWinFound(false);
        for(size_t i = 0; i < m_book->GetPageCount(); ++i) {
            if(currentWinFound) {
                windows.push_back(m_book->GetPage(i));
            } else {
                if(m_book->GetPage(i) == win) { currentWinFound = true; }
            }
        }

        // start from right to left
        if(windows.empty()) return;

        WindowList tabsToClose;
        for(int i = (int)(windows.size() - 1); i >= 0; --i) {
            if(windows.at(i) == win) { break; }
            tabsToClose.push_back(windows.at(i));
        }

        if(tabsToClose.empty()) return;

        for(size_t i = 0; i < tabsToClose.size(); ++i) {
            ClosePage(tabsToClose.at(i));
        }
        if(m_book->GetSizer())
            m_book->GetSizer()->Layout();
    }
    //----------------------------------------------------------------------------
    void Book::ShowNavigationDialog()
    {
        if(!gsgs_EditorConfig().GetOptions()->IsCtrlTabEnabled())
            return; 

        if(m_book->GetPageCount() == 0)
            return;

        NotebookNavigationDlg dlg(gsgs_Event().TopFrame(), m_book);
        if(dlg.ShowModal() == wxID_OK && dlg.GetSelection() != wxNOT_FOUND) 
        { 
            m_book->SetSelection(dlg.GetSelection()); 
        }
    }
    //----------------------------------------------------------------------------
    void Book::MovePage(bool movePageRight)
    {
        // Determine the new index
        int newIndex = wxNOT_FOUND;

        // Sanity
        if(m_book->GetPageCount() == 0)
            return;

        int pageCount = static_cast<int>(m_book->GetPageCount());
        if(movePageRight && ((m_book->GetSelection() + 1) < pageCount)) 
        {
            newIndex = m_book->GetSelection() + 1;
        } 
        else if(!movePageRight && (m_book->GetSelection() > 0)) 
        {
            newIndex = m_book->GetSelection() - 1;
        }
        if(newIndex != wxNOT_FOUND) { m_book->MoveActivePage(newIndex); }
    }
    //----------------------------------------------------------------------------
    void Book::OnTheme(wxCommandEvent& e)
    {
        e.Skip();
        DoUpdateNotebookTheme();
        // force a redrawing of the main notebook
        //m_book->SetStyle(m_book->GetStyle());
    }
    //----------------------------------------------------------------------------
    void Book::OnEditorSettingsChanged(wxCommandEvent& e)
    {
        e.Skip();
        DoUpdateNotebookTheme();
    }
    //----------------------------------------------------------------------------
    void Book::OnTabDClicked(wxBookCtrlEvent& e)
    {
        e.Skip();
        gsgs_Manager()->TogglePanes();
    }
    //----------------------------------------------------------------------------
    void Book::DoOpenImageViewer(const wxFileName& filename)
    {
        ImageViewer* imageViewer = new ImageViewer(m_book, filename);
        AddPage(imageViewer, filename.GetFullName(), filename.GetFullPath(), wxNullBitmap, true);
        //size_t pos = m_book->GetPageCount();
        //m_book->AddPage(imageViewer, filename.GetFullName(), true);
        //m_book->SetPageToolTip(pos, filename.GetFullPath());
    }
    //----------------------------------------------------------------------------
    void Book::OnTabLabelContextMenu(wxBookCtrlEvent& e)
    {
        e.Skip();
        wxWindow* book = static_cast<wxWindow*>(e.GetEventObject());
        if(book == m_book) {
            e.Skip(false);
            if(e.GetSelection() == m_book->GetSelection()) {
                // The tab requested for context menu is the active one
                DoShowTabLabelContextMenu();
            } else {
                // Make this tab the active one and requeue the context menu event
                m_book->SetSelection(e.GetSelection());
                DoShowTabLabelContextMenu();
            }
        }
    }
    //----------------------------------------------------------------------------
    bool Book::ClosePage(IEditor* editor, bool notify)
    {
        wxWindow* page = editor->GetCtrl();
        if (page)
        {
            int pos = m_book->GetPageIndex(page->GetParent());
            return (pos != wxNOT_FOUND) && (m_book->DeletePage(pos, notify));
        }
        return false;
    }
    //----------------------------------------------------------------------------
    void Book::GetDetachedTabs(TabVec_t& tabs)
    {
        // Make sure that modified detached editors are also enabling the "Save" and "Save All" button
        const EditorFrameList_t& detachedEditors = GetDetachedEditors();
        std::for_each(detachedEditors.begin(), detachedEditors.end(), [&](EditorFrame* fr) {
            Tab tabInfo;
            Editor * ed = fr->GetEditor()->getEditor();
            tabInfo.bitmap = wxNullBitmap;
            tabInfo.filename = ed->GetFileName();
            tabInfo.isFile = true;
            tabInfo.isModified = ed->IsModified();
            tabInfo.text = ed->GetFileName().GetFullPath();
            tabInfo.window = fr->GetEditor();
            tabs.push_back(tabInfo);
        });
    }
    //----------------------------------------------------------------------------
    void Book::DoOpenFile(const wxString& filename, const wxString& content)
    {
        Editor* editor = OpenFile(filename);
        if(editor && !content.IsEmpty()) { editor->SetText(content); }
    }
    //----------------------------------------------------------------------------
    void Book::OnCacheUpdated(Event& e)
    {
        e.Skip();
        UpdateNavBar(GetActiveEditor());
    }
    //----------------------------------------------------------------------------
    void Book::OnUpdateNavigationBar(CodeCompleteEvent& e)
    {
        e.Skip();
        IEditor* editor = dynamic_cast<IEditor*>(e.GetEditor());
        CHECK_PTR_RET(editor);

        Editor* activeEditor = GetActiveEditor();
        if(editor != activeEditor) return;

        // This event is no longer valid
        if(activeEditor->GetCurrentLine() != e.GetLineNumber()) return;

        FileUtil::FileType ft = FileUtil::GetTypeFromExtension(editor->GetFileName());
        if((ft != FileUtil::TypeSourceC) && (ft != FileUtil::TypeSourceCpp) &&
           (ft != FileUtil::TypeHeader))
            return;

        // A C++ file :)
        e.Skip(false);
        UpdateNavBar(activeEditor);
    }
    //----------------------------------------------------------------------------
    void Book::OnColoursAndFontsChanged(Event& e)
    {
        e.Skip();
        
        EditorVec_t editors;
        GetAllEditors(editors, Book::kGetAll_IncludeDetached);
        for(size_t i = 0; i < editors.size(); i++) {
            editors[i]->SetSyntaxHighlight(editors[i]->GetContext()->GetName());
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnNavigationBarMenuShowing(ContextMenuEvent& e)
    {
        e.Skip();
        m_currentNavBarTags.clear();
        IEditor* editor = GetActiveEditor(true);
        if(m_navBar && m_navBar->IsShown() && editor) {
            TagEntryPtrList tags;
            if(!TagsManagerST::Get()->GetFileCache()->Find(editor->GetFileName(), tags,
                                                           CxxFileCacheSymbols::kFunctions) ||
               tags.empty()) {
                return;
            }

            if(gsgs_EditorConfig().GetOptions()->IsSortNavBarDropdown()) {
                std::sort(tags.begin(), tags.end(), [](TagEntryPtr pLhs, TagEntryPtr pRhs) {
                    return pLhs->GetFullDisplayName() < pRhs->GetFullDisplayName();
                });
            }

            wxMenu* menu = e.GetMenu();
            std::for_each(tags.begin(), tags.end(), [&](TagEntryPtr tag) {
                wxString fullname = tag->GetFullDisplayName();
                menu->Append(wxID_ANY, fullname);
                m_currentNavBarTags.insert({ fullname, tag });
            });
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnNavigationBarMenuSelectionMade(Event& e)
    {
        e.Skip();
        const wxString& selection = e.GetString();
        if(m_currentNavBarTags.count(selection) == 0)
            return;

        TagEntryPtr tag = m_currentNavBarTags[selection];
        // Ours to handle
        e.Skip(false);

        IEditor* editor = GetActiveEditor(true);
        if(editor)
            editor->FindAndSelect(tag->GetPattern(), tag->GetName(), editor->PosFromLine(tag->GetLine() - 1), nullptr);
    }
    //----------------------------------------------------------------------------
    void Book::OnSettingsChanged(wxCommandEvent& e)
    {
        e.Skip();
    }
    //----------------------------------------------------------------------------
    Editor* Book::OpenFile(const BrowseRecord& rec)
    {
        Editor* editor = OpenFile(rec.filename, rec.project, wxNOT_FOUND, wxNOT_FOUND, OF_None, true);
        if(editor) 
        {
            if(rec.firstLineInView != wxNOT_FOUND) { editor->GetCtrl()->SetFirstVisibleLine(rec.firstLineInView); }
            // Determine the best position for the caret
            int pos = rec.position;
            if((pos == wxNOT_FOUND) && (rec.lineno != wxNOT_FOUND)) { pos = editor->PositionFromLine(rec.lineno); }
            if(pos != wxNOT_FOUND) {
                editor->SetCurrentPos(rec.position);
                editor->SetSelectionStart(rec.position);
                editor->SetSelectionEnd(rec.position);
                editor->EnsureCaretVisible();
            }
        }
        return editor;
    }
    //----------------------------------------------------------------------------
    void Book::DoShowTabLabelContextMenu()
    {
        wxMenu* contextMenu = wxXmlResource::Get()->LoadMenu(wxT("editor_tab_right_click"));

        // Notify the plugins about the tab label context menu
        ContextMenuEvent event(wxEVT_CONTEXT_MENU_TAB_LABEL);
        event.SetMenu(contextMenu);
        gsgs_Event().ProcessEvent(event);

        contextMenu = event.GetMenu();
        m_book->PopupMenu(contextMenu);
        wxDELETE(contextMenu);
    }
    //----------------------------------------------------------------------------
    void Book::createPresent()
    {
        wxWindowUpdateLocker locker(this);
        if(mWelcome == 0) 
        { 
            mWelcome = new Present(this);
            GetSizer()->Add(mWelcome, 1, wxEXPAND);
        }

        DoShowWindow(mWelcome, true);
        DoShowWindow(m_book, false);
    }
    //----------------------------------------------------------------------------
    void Book::DoShowWindow(wxWindow* win, bool show)
    {
        if (show)
        {
            if (GetSizer()->GetItem(win) && !win->IsShown())
            {
                GetSizer()->Show(win);
            }
        }
        else
        {
            if (GetSizer()->GetItem(win) && win->IsShown())
            {
                GetSizer()->Hide(win);
            }
        }
    }
    //----------------------------------------------------------------------------
    void Book::showPresent(bool show)
    {
        wxWindowUpdateLocker locker(this);
        if (show)
        {
            mWelcome->RefreshRecentList();
            DoShowWindow(mWelcome, true);
            DoShowWindow(m_book, false);
            StatusBar* sb = GetManager()->GetStatusBar();
            if (sb)
            {
                sb->SetLinePosColumn(_T("Welcome to ¹âÕ¤¹ÊÊÂ"));
            }
            gsgs_Frame()->SetFrameTitle(0);
        }
        else
        {
            DoShowWindow(m_book, true);
            DoShowWindow(mWelcome, false);
        }
        GetSizer()->Layout();
    }
    //----------------------------------------------------------------------------
    void Book::ShowQuickBarForPlugins()
    {
        if(m_findBar) 
        { 
            m_findBar->ShowForPlugins(); 
        }
    }
    //----------------------------------------------------------------------------
    void Book::ShowQuickBar(bool s)
    {
        if(m_findBar) 
        {
            m_findBar->Show(s);
            GetParent()->GetSizer()->Layout();
        }
    }
    //----------------------------------------------------------------------------
    void Book::ShowQuickBar(const wxString& findWhat, bool showReplace)
    {
        if(m_findBar) 
        {
            m_findBar->Show(findWhat, showReplace);
            GetParent()->GetSizer()->Layout();
        }
    }
    //----------------------------------------------------------------------------
    void Book::OnEditorChanged(wxCommandEvent& event)
    {
        event.Skip();
        IEditor* editor = GetActiveEditor();
        if(editor) 
        {
            m_findBar->SetEditor(editor->GetCtrl());
            return;
        }
        m_findBar->SetEditor(NULL);
    }
    //----------------------------------------------------------------------------
    void Book::OnAllEditorClosed(wxCommandEvent& event)
    {
        event.Skip();
        m_findBar->SetEditor(NULL);
    }
    //----------------------------------------------------------------------------
    void Book::SetFindBar(QuickFindBar* bar)
    {
        if(m_findBar) 
            return;
        m_findBar = bar;
        gsgs_Event().Bind(wxEVT_ALL_EDITORS_CLOSED, &Book::OnAllEditorClosed, this);
        gsgs_Event().Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &Book::OnEditorChanged, this);
    }
    //----------------------------------------------------------------------------
}