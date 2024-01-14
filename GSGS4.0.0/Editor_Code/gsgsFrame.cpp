
#include "gsgsPreProcess.h"
#include "gsgsFrame.h"
#include "GsGsVersion.h"
#include "gsgsBookmarkManager.h"
#include "NewProjectDialog.h"
#include "gsgsCommonHandler.h"
#include "gsgsCommonDialog.h"
#include "CommonController1.h"
#include "CommonController.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsBuildConfig.h"
#include "gsgsTabgroupManager.h"
#include "gsgsPathManager.h"
#include "gsgsCodeCompletionManager.h"
#include "gsgsLogManager.h"
#include "gsgsToolBar.h"
#include "gsgsQuickFindBar.h"
#include "gsgsCodeBoxManager.h"
#include "gsgsStatusBar.h"
#include "gsgsAbout.h"
#include "gsgsNotebook.h"
#include "gsgsProcess.h"
#include "gsgsDebuggerManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsPluginCommon.h"
#include "gsgsJob.h"
#include "gsgsLanguage.h"
#include "gsgsMenuManager.h"
#include "gsgsNavigationManager.h"
#include "gsgsBuildTab.h"
#include "gsgsPluginManager.h"
#include "quickoutlinedlg.h"
#include "gsgsSearchWorker.h"
#include "gsgsSessionManager.h"
#include "gsgsCreateWorkspace.h"
#include "gsgsPreferences.h"
#include "symbol_tree.h"
#include "gsgsPresent.h"
#include "SocketAPI/gsgsSocketServer.h"
#include "SocketAPI/gsgsSocketClientAsync.h"
#include "context_cpp.h"
#include "gsgsFileView.h"
#include "gsgsCompilerSettings.h"
#include "globals.h"
#include "imanager.h"
#include "cpptoken.h"
#include "gsgsManager.h"
#include "app.h"

#include <wx/bookctrl.h>
#include <wx/busyinfo.h>
#include <wx/richmsgdlg.h>
#include <wx/wupdlock.h>
#include <wx/image.h>
#include <wx/listbox.h>
#include <wx/listctrl.h>
#include <wx/thread.h>
#include <wx/url.h>
#include <wx/dnd.h>

#ifdef __WXMAC__
#include <wx/srchctrl.h>
#endif

#define CHECK_POINTER(p) if(!p) return;

#ifdef __WXGTK20__
// We need this ugly hack to workaround a gtk2-wxGTK name-clash
// See http://trac.wxwidgets.org/ticket/10883
#define GSocket GlibGSocket
#include <gtk/gtk.h>
#undef GSocket
#endif

namespace gsgs
{
    class FileDropTarget : public wxFileDropTarget {
    public:
        FileDropTarget();
        virtual ~FileDropTarget();
        virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
    };
    FileDropTarget::FileDropTarget()
        : wxFileDropTarget()
    {
    }

    FileDropTarget::~FileDropTarget()
    {
    }

    bool FileDropTarget::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString &filenames)
    {
        wxUnusedVar(x);
        wxUnusedVar(y);
        for(size_t i=0; i<filenames.GetCount(); i++){
            gsgs_Frame()->GetMainBook()->OpenFile(filenames.Item(i));
        }
        return true;
    }
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    // MainFrameHelper
    //---------------------------------------------------------------------------------------
    //---------------------------------------------------------------------------------------
    MainFrameHelper::MainFrameHelper(Frame* frame, WindowManager* mgr):
        m_mainFrame(frame), 
        m_mgr(mgr), 
        m_debuggerFeatures(DebugEvent::kAllFeatures)
    {
    }
    //---------------------------------------------------------------------------------------
    MainFrameHelper::~MainFrameHelper() 
    {
    }
    //---------------------------------------------------------------------------------------
    bool MainFrameHelper::IsToolbarShown() const 
    { 
        return m_mainFrame->GetMainToolBar()->IsShown(); 
    }
    //---------------------------------------------------------------------------------------
    bool MainFrameHelper::IsCaptionsVisible() const
    {
        wxAuiPaneInfoArray & panes = m_mgr->GetAllPanes();
        for(size_t i = 0; i < panes.GetCount(); ++i) 
        {
            if(panes.Item(i).IsOk() && !panes.Item(i).IsToolbar()) 
            {
                if(panes.Item(i).HasCaption()) 
                {
                    return true;
                }
            }
        }
        return false;
    }
    //---------------------------------------------------------------------------------------
    bool MainFrameHelper::IsStatusBarVisible() const
    { 
        return m_mainFrame->GetStatusBar()->IsShown(); 
    }
    //---------------------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_CMD_NEW_VERSION_AVAILABLE, wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_VERSION_UPTODATE, wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_VERSION_CHECK_ERROR, wxCommandEvent);
    wxDEFINE_EVENT(wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES, Event);
    wxDEFINE_EVENT(wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP, Event);
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // WebUpdateJob
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    class WebUpdateJob : public wxEvtHandler
    {
    public:
        WebUpdateJob(wxEvtHandler * parent, bool userRequest, bool onlyRelease)
            : m_parent(parent)
            , m_userRequest(userRequest)
            , m_onlyRelease(onlyRelease){}

        ~WebUpdateJob() 
        {
            Clear(); 
        }
        void ParseFile();
        bool IsUserRequest() const                  { return m_userRequest; }
        void Check();
    protected:
        /**
         * @brief get the current platform details
         */
        void GetPlatformDetails(wxString& os, wxString & codename, wxString& arch) const;
        void OnConnected(Event& e)
        {
            wxString message;
            message << "GET /packages.json HTTP/1.1\r\n"
                    << "Host: www.niiengine.com/\r\n"
                    << "\r\n";
            m_socket->Send(message);
        }
        void OnConnectionLost(Event& e)
        {
            gsgsDEBUG() << "WebUpdateJob: Connection lost:" << e.GetString() << clEndl;
            m_socket.reset(nullptr);
            NotifyError("Connection lost:" + e.GetString());
        }
        void OnConnectionError(Event& e)
        {
            gsgsDEBUG() << "WebUpdateJob: Connection error:" << e.GetString() << clEndl;
            m_socket.reset(nullptr);
            NotifyError("Connection error:" + e.GetString());
        }
        void OnSocketError(Event& e)
        {
            gsgsDEBUG() << "WebUpdateJob: socket error:" << e.GetString() << clEndl;
            m_socket.reset(nullptr);
            NotifyError("Socker error:" + e.GetString());
        }
        void OnSocketInput(Event& e)
        {
            m_dataRead << e.GetString();
            int where = m_dataRead.Find("\r\n\r\n");
            if(where != wxNOT_FOUND) {
                wxString headers = m_dataRead.Mid(0, where);
                m_dataRead = m_dataRead.Mid(where + 4);
                ParseFile();
                Clear();
            }
        }
        inline void Clear()                         { m_socket.reset(nullptr); }
        void NotifyError(const wxString& errmsg)
        {
            wxCommandEvent event(wxEVT_CMD_VERSION_CHECK_ERROR);
            event.SetString(errmsg);
            m_parent->AddPendingEvent(event);
        }
    private:
        wxEvtHandler * m_parent;
        AsyncSocket::Ptr_t m_socket;
        wxString m_dataRead;
        bool m_userRequest;
        bool m_onlyRelease;
    };
    
    static const size_t DLBUFSIZE = 4096;
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // GSGSVersion
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    struct GSGSVersion 
    {
        GSGSVersion(const JSONObject& json)
            : m_version(wxNOT_FOUND)
            , m_isReleaseVersion(false)
        {
            json.getObject("os").getValue(m_os);
            json.getObject("codename").getValue(m_codename);
            json.getObject("arch").getValue(m_arch);
            json.getObject("url").getValue(m_url);
            json.getObject("version").getValue(m_version);
            json.getObject("isRelease").getValue(m_isReleaseVersion, m_isReleaseVersion);
        }

        void Print() { gsgsDEBUG() << "--->" << m_os << "," << m_codename << "," << m_arch << "," << m_version << clEndl; }

        /**
         * @brief return true of this gsgs version object is newer than the provided input
         */
        bool IsNewer(const wxString& os, const wxString& codename, const wxString& arch) const
        {
            wxString strVersionNumer = GSGS_VERSION;
            strVersionNumer.Replace(".", "");
            long nVersionNumber = -1;
            strVersionNumer.ToCLong(&nVersionNumber);

            if((m_os == os) && (m_arch == arch) && (m_codename == codename)) {
                bool res = (m_version > nVersionNumber);
                if(res) { gsgsDEBUG() << "Found new version!" << clEndl; }
                return res;
            }
            return false;
        }

        bool IsReleaseVersion() const           { return m_isReleaseVersion; }
        const wxString& GetArch() const         { return m_arch; }
        const wxString& GetCodename() const     { return m_codename; }
        const wxString& GetOs() const           { return m_os; }
        const wxString& GetUrl() const          { return m_url; }
        int GetVersion() const                  { return m_version; }
        
        wxString m_os;
        wxString m_codename;
        wxString m_arch;
        wxString m_url;
        int m_version;
        bool m_isReleaseVersion;
    };
    //-----------------------------------------------------------------
    void WebUpdateJob::ParseFile()
    {
        wxString os, arch, codename;
        GetPlatformDetails(os, codename, arch);

        gsgsDEBUG() << "Current platform details:" << os << "," << codename << "," << arch << "," << GSGS_VERSION
                  << clEndl;
        JSON root(m_dataRead);
        JSONObject platforms = root.getObject().getObject("platforms");

        int count = platforms.getArraySize();
        for(int i = 0; i < count; ++i) 
        {
            GSGSVersion v(platforms.getArray(i));
            v.Print();
            if(!v.IsReleaseVersion() && m_onlyRelease) 
            {
                // User wishes to be prompted for new releases only
                // skip weekly builds
                continue;
            }

            if(v.IsNewer(os, codename, arch)) 
            {
                gsgsDEBUG() << "A new version of gsgs found" << clEndl;
                wxCommandEvent event(wxEVT_CMD_NEW_VERSION_AVAILABLE);
                event.SetClientData(new WebUpdateJobData("http://www.niiengine.com/forum/", v.GetUrl(), GSGS_VERSION, "", false, true));
                m_parent->AddPendingEvent(event);
                return;
            }
        }

        // If we got here, then the version is up to date
        wxCommandEvent event(wxEVT_CMD_VERSION_UPTODATE);
        m_parent->AddPendingEvent(event);
    }
    //-----------------------------------------------------------------
    void WebUpdateJob::GetPlatformDetails(wxString& os, wxString& codename, wxString& arch) const
    {
        static gsgsStringMap linuxVariants;

    #ifdef __WXMSW__
        os = "msw";
        codename = "Windows";
    #ifndef NDEBUG
        os << "-dbg";
    #endif

    #ifdef _WIN64
        arch = "x86_64";
    #else
        arch = "i386";
    #endif
    #elif defined(__WXOSX__)
        os = "osx";
        arch = "x86_64";
        codename = "10.8";
    #else
        os = "linux";
        wxFFile fp("/etc/issue", "rb");
        wxString content;
        if(fp.IsOpened()) {
            fp.ReadAll(&content, wxConvUTF8);
            fp.Close();
        }
        // Test for common code names that we support on Linux
        if(content.Contains("Ubuntu 14.04")) {
            codename = "Ubuntu 14.04";
        } else if(content.Contains("Ubuntu 16.04")) {
            codename = "Ubuntu 16.04";
        } else if(content.Contains("Ubuntu 18.04")) {
            codename = "Ubuntu 18.04";
        } else if(content.Contains("Debian GNU/Linux 8")) {
            codename = "Debian GNU/Linux 8";
        } else {
            codename = "others";
        }
    #if __LP64__
        arch = "x86_64";
    #else
        arch = "i386";
    #endif
    #endif
    }
    //-----------------------------------------------------------------
    void WebUpdateJob::Check()
    {
        m_socket.reset(
            new AsyncSocket("tcp://198.167.139.57:80", kAsyncSocketBuffer | kAsyncSocketClient | kAsyncSocketNonBlocking));
        m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECTED, &WebUpdateJob::OnConnected, this);
        m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECTION_LOST, &WebUpdateJob::OnConnectionLost, this);
        m_socket->Bind(wxEVT_ASYNC_SOCKET_CONNECT_ERROR, &WebUpdateJob::OnConnectionError, this);
        m_socket->Bind(wxEVT_ASYNC_SOCKET_ERROR, &WebUpdateJob::OnSocketError, this);
        m_socket->Bind(wxEVT_ASYNC_SOCKET_INPUT, &WebUpdateJob::OnSocketInput, this);
        m_socket->Start();
    }
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // SInstanceThread
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    class SInstanceThread : public wxThread
    {
    public:
        SInstanceThread() : wxThread(wxTHREAD_JOINABLE) {}
        
        ~SInstanceThread() { Stop(); }
        /**
         * @brief the thread main loop
         */
        virtual void* Entry()
        {
            try {
                wxString connectionString;
                connectionString << "tcp://127.0.0.1:" << SINGLE_INSTANCE_PORT;
                m_serverSocket.Start(connectionString);
                gsgsDEBUG() << "SInstanceThread: starting on:" << connectionString << clEndl;

                while(!TestDestroy()) {
                    // wait for a new connection
                    SocketBase::Ptr_t client = m_serverSocket.WaitForNewConnection(1);
                    if(!client) 
                        continue;

                    wxString message;
                    if(client->ReadMessage(message, 3) == SocketBase::kTimeout) continue;
                    gsgs_Debug("SInstanceThread: received new message: %s", message);

                    JSON root(message);
                    wxArrayString args;
                    root.getObject().getObject("args").getValue(args);

                    if(args.IsEmpty()) {
                        // just raise gsgs
                        Event event(wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP);
                        gsgs_Event().AddPendingEvent(event);
                    } else {
                        Event event(wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES);
                        event.SetStrings(args);
                        gsgs_Event().AddPendingEvent(event);
                    }
                }
            } catch(SocketException& e) {
                gsgs_Error("Failed to create single instance socket: %s", e.what());
            }
            return NULL;
        }
        
        /**
         * @brief start the single instance thread
         */
        void Start()
        {
            Create();
            Run();
        }

        /**
         * @brief stop the thread
         */
        void Stop()
        {
            // Notify the thread to exit and
            // wait for it
            if(IsAlive()) {
                Delete(NULL, wxTHREAD_WAIT_BLOCK);

            } else {
                Wait(wxTHREAD_WAIT_BLOCK);
            }
        }
    private:
        SocketServer m_serverSocket;
    };
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // GeneralInfo
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    class GeneralInfo : public ObjectData
    {
    public:
        GeneralInfo() : m_frameSize(800, 600) , m_framePos(30, 30)
            , m_flags(GSGS_SHOW_WELCOME_PAGE | GSGS_USE_EOL_LF | GSGS_SHOW_SPLASH | GSGS_MAXIMIZE_FRAME) {}
        virtual ~GeneralInfo() {}

        inline void SetFrameSize(const wxSize &sz) { m_frameSize = sz;}

        inline const wxSize& GetFrameSize() const { return m_frameSize;}

        inline void SetFramePosition(const wxPoint &pt) { m_framePos = pt; }

        inline const wxPoint& GetFramePosition() const { return m_framePos; }

        inline void SetFlags(const size_t& flags) { m_flags = flags; }

        inline const size_t& GetFlags() const { return m_flags; }

        void Serialize(Serializer &arch)
        {
            arch.Write(wxT("m_frameSize"), m_frameSize);
            arch.Write(wxT("m_framePos"), m_framePos);
            arch.Write(wxT("m_flags"), m_flags);
        }

        void DeSerialize(Serializer &arch)
        {
            arch.Read(wxT("m_frameSize"), m_frameSize);
            arch.Read(wxT("m_framePos"), m_framePos);
            arch.Read(wxT("m_flags"), m_flags);
        }
    private:
        wxSize m_frameSize;
        wxPoint m_framePos;
        size_t m_flags;
    };
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // ThemeHandler
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    ThemeHandler::ThemeHandler()
    {
        //gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED, &ThemeHandler::OnEditorThemeChanged, this);
        gsgs_Event().Bind(wxEVT_INIT_DONE, &ThemeHandler::OnInitDone, this);
        gsgs_Event().Bind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &ThemeHandler::OnColoursChanged, this);
    }
    //-----------------------------------------------------------------
    ThemeHandler::~ThemeHandler()
    {
        //gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED, &ThemeHandler::OnEditorThemeChanged, this);
        gsgs_Event().Unbind(wxEVT_INIT_DONE, &ThemeHandler::OnInitDone, this);
        gsgs_Event().Unbind(wxEVT_CMD_COLOURS_FONTS_UPDATED, &ThemeHandler::OnColoursChanged, this);
    }
    //-----------------------------------------------------------------
    void ThemeHandler::OnEditorThemeChanged(wxCommandEvent& e) 
    { 
        e.Skip(); 
    }
    //-----------------------------------------------------------------
    void ThemeHandler::OnInitDone(wxCommandEvent& e)
    {
        e.Skip();
        m_helper.reset(new ThemeHandlerHelper(gsgs_Frame()));
        //m_helper->UpdateColours(gsgs_Frame());
        // Fire "wxEVT_EDITOR_SETTINGS_CHANGED" to ensure that the notebook appearance is in sync with the settings
        PostCmdEvent(wxEVT_EDITOR_SETTINGS_CHANGED);
    }
    //-----------------------------------------------------------------
    void ThemeHandler::OnColoursChanged(Event& e)
    {
        e.Skip();
        CallAfter(&ThemeHandler::UpdateColours);
    }
    //-----------------------------------------------------------------
    #define IS_TYPEOF(Type, Win) (dynamic_cast<Type*>(Win))
    //-----------------------------------------------------------------
    void ThemeHandler::UpdateColours()
    {
        // Collect all toolbars
        std::queue<wxWindow*> q;
        q.push(gsgs_Frame());

        const wxColour & bgColour = gsgs_Style().GetAppBgColour();
        const wxColour & fgColour = gsgs_Style().GetAppTextColour();

        while(!q.empty()) {
            wxWindow* w = q.front();
            q.pop();
            if(IS_TYPEOF(wxPanel, w) || IS_TYPEOF(wxStaticText, w) || IS_TYPEOF(wxCheckBox, w)) {
                w->SetBackgroundStyle(wxBG_STYLE_PAINT);
                w->SetBackgroundColour(bgColour);
                w->SetForegroundColour(fgColour);
            }
            wxWindowList::compatibility_iterator iter = w->GetChildren().GetFirst();
            while(iter) {
                q.push(iter->GetData());
                iter = iter->GetNext();
            }
        }
        gsgs_Frame()->Refresh();
    }
    //-----------------------------------------------------------------
    void ThemeHandler::UpdateNotebookColours(wxWindow* parent) 
    {
        m_helper->UpdateNotebookColours(parent); 
    }
    //-----------------------------------------------------------------
    // from auto-generated file svninfo.cpp:
    static wxStopWatch gStopWatch;

    // from iconsextra.cpp:
    extern char* cubes_xpm[];
    extern unsigned char cubes_alpha[];

    static int FrameTimerId = wxNewId();

    const wxEventType wxEVT_LOAD_PERSPECTIVE = XRCID("load_perspective");
    const wxEventType wxEVT_REFRESH_PERSPECTIVE_MENU = XRCID("refresh_perspective_menu");
    const wxEventType wxEVT_ACTIVATE_EDITOR = XRCID("activate_editor");
    //const wxEventType wxEVT_LOAD_SESSION = ::wxNewEventType();

    #define CHECK_SHUTDOWN() { if(gsgs_Manager()->IsShutdownInProgress()) { return; } }

    #ifdef __WXGTK__
    #define FACTOR_1 0.0
    #define FACTOR_2 0.0
    #else
    #define FACTOR_1 2.0
    #define FACTOR_2 2.0
    #endif

    static bool IsDebuggerRunning()
    {
        DebugEvent eventIsRunning(wxEVT_DBG_IS_RUNNING);
        gsgs_Event().ProcessEvent(eventIsRunning);
        IDebugger * dbgr = DebuggerMgr::Get().GetActiveDebugger();
        return (dbgr && dbgr->IsRunning()) || eventIsRunning.IsAnswer();
    }

    struct StartPageData
    {
        wxString name;
        wxString file_path;
        wxString action;
    };

    //----------------------------------------------------------------
    // Our main frame
    //----------------------------------------------------------------
    BEGIN_EVENT_TABLE(Frame, wxFrame)
    // ATTN: Any item that appears in more than one menu is
    //       only listed once here, the first time it occurs
    //---------------------------------------------------
    // System events
    //---------------------------------------------------
    // EVT_IDLE(Frame::OnIdle)
    EVT_CLOSE(Frame::OnClose)
    EVT_TIMER(FrameTimerId, Frame::OnTimer)
    //  EVT_AUI_RENDER(Frame::OnAuiManagerRender)
    //  EVT_AUI_PANE_CLOSE(Frame::OnDockablePaneClosed)

    //---------------------------------------------------
    // File menu
    //---------------------------------------------------
    EVT_MENU(XRCID("new_file"),                             Frame::OnFileNew)
    EVT_MENU(XRCID("open_file"),                            Frame::OnFileOpen)
    EVT_MENU(XRCID("open_folder"),                          Frame::OnFileOpenFolder)
    EVT_MENU(XRCID("refresh_file"),                         Frame::OnFileReload)
    EVT_MENU(XRCID("load_tab_group"),                       Frame::OnFileLoadTabGroup)
    EVT_MENU(XRCID("save_file"),                            Frame::OnSave)
    EVT_MENU(XRCID("duplicate_tab"),                        Frame::OnDuplicateTab)
    EVT_MENU(XRCID("save_file_as"),                         Frame::OnSaveAs)
    EVT_MENU(XRCID("save_all"),                             Frame::OnFileSaveAll)
    EVT_MENU(XRCID("save_tab_group"),                       Frame::OnFileSaveTabGroup)
    EVT_MENU(XRCID("close_file"),                           Frame::OnFileClose)
    EVT_MENU(wxID_CLOSE_ALL,                                Frame::OnFileCloseAll)
    EVT_MENU_RANGE(RecentFilesSubMenuID, RecentFilesSubMenuID + 10, Frame::OnRecentFile)
    EVT_MENU_RANGE(RecentWorkspaceSubMenuID, RecentWorkspaceSubMenuID + 10, Frame::OnRecentWorkspace)
    EVT_MENU_RANGE(ID_MENU_CUSTOM_TARGET_FIRST, ID_MENU_CUSTOM_TARGET_MAX, Frame::OnBuildCustomTarget)
    EVT_MENU(wxID_EXIT,                                     Frame::OnQuit)

    EVT_MENU(wxID_PRINT,                                    Frame::OnPrint)
    EVT_UPDATE_UI(wxID_PRINT,                               Frame::OnFileExistUpdateUI)
    EVT_MENU(wxID_PAGE_SETUP,                               Frame::OnPageSetup)

    EVT_UPDATE_UI(XRCID("refresh_file"),                    Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("save_file"),                       Frame::OnFileSaveUI)
    EVT_UPDATE_UI(XRCID("duplicate_tab"),                   Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("save_file_as"),                    Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("save_all"),                        Frame::OnFileSaveAllUI)
    EVT_UPDATE_UI(XRCID("save_tab_group"),                  Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("close_file"),                      Frame::OnFileCloseUI)
    EVT_UPDATE_UI(XRCID("recent_workspaces"),               Frame::OnRecentWorkspaceUI)

    //--------------------------------------------------
    // Edit menu
    //--------------------------------------------------
    EVT_MENU(wxID_UNDO,                                     Frame::DispatchCommandEvent)
    EVT_MENU(wxID_REDO,                                     Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("label_current_state"),                  Frame::DispatchCommandEvent)
    EVT_MENU(wxID_DUPLICATE,                                Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("delete_line"),                          Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("delete_line_end"),                      Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("delete_line_start"),                    Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("copy_line"),                            Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("cut_line"),                             Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("transpose_lines"),                      Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("trim_trailing"),                        Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("to_upper"),                             Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("to_lower"),                             Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("match_brace"),                          Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("select_to_brace"),                      Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("complete_word"),                        Frame::OnCodeComplete)
    EVT_MENU(XRCID("simple_word_completion"),               Frame::OnWordComplete)
    EVT_MENU(XRCID("complete_word_refresh_list"),           Frame::OnCompleteWordRefreshList)
    EVT_MENU(XRCID("function_call_tip"),                    Frame::OnFunctionCalltip)
    EVT_MENU(XRCID("convert_eol_win"),                      Frame::OnConvertEol)
    EVT_MENU(XRCID("convert_eol_unix"),                     Frame::OnConvertEol)
    EVT_MENU(XRCID("convert_eol_mac"),                      Frame::OnConvertEol)
    EVT_MENU(XRCID("move_line_down"),                       Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("move_line_up"),                         Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("center_line"),                          Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("center_line_roll"),                     Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("selection_to_multi_caret"),             Frame::OnSplitSelection)
    EVT_MENU(XRCID("convert_indent_to_tabs"),               Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("convert_indent_to_spaces"),             Frame::DispatchCommandEvent)
    EVT_UPDATE_UI(XRCID("selection_to_multi_caret"),        Frame::OnSplitSelectionUI)

    EVT_UPDATE_UI(wxID_UNDO,                                Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(wxID_REDO,                                Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(wxID_DUPLICATE,                           Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("delete_line"),                     Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("delete_line_end"),                 Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("delete_line_start"),               Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("copy_line"),                       Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("cut_line"),                        Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("transpose_lines"),                 Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("trim_trailing"),                   Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("to_upper"),                        Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("to_lower"),                        Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("match_brace"),                     Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("select_to_brace"),                 Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("complete_word"),                   Frame::OnCompleteWordUpdateUI)
    EVT_UPDATE_UI(XRCID("simple_word_completion"),          Frame::OnCompleteWordUpdateUI)
    EVT_UPDATE_UI(XRCID("function_call_tip"),               Frame::OnFunctionCalltipUI)
    EVT_UPDATE_UI(XRCID("convert_eol_win"),                 Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("convert_eol_unix"),                Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("convert_eol_mac"),                 Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("move_line_down"),                  Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("move_line_up"),                    Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("center_line_roll"),                Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("convert_indent_to_tabs"),          Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("convert_indent_to_spaces"),        Frame::OnFileExistUpdateUI)

    //-------------------------------------------------------
    // View menu
    //-------------------------------------------------------
    EVT_MENU(XRCID("restore_layout"),                       Frame::OnRestoreDefaultLayout)
    EVT_MENU(XRCID("word_wrap"),                            Frame::OnViewWordWrap)
    EVT_MENU(XRCID("toggle_fold"),                          Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("fold_all"),                             Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("fold_all_in_selection"),                Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("fold_topmost_in_selection"),            Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("display_eol"),                          Frame::OnViewDisplayEOL)
    EVT_MENU(XRCID("whitepsace_invisible"),                 Frame::OnShowWhitespace)
    EVT_MENU(XRCID("whitepsace_always"),                    Frame::OnShowWhitespace)
    EVT_MENU(XRCID("whitespace_visiable_after_indent"),     Frame::OnShowWhitespace)
    EVT_MENU(XRCID("whitespace_indent_only"),               Frame::OnShowWhitespace)
    EVT_MENU(XRCID("next_tab"),                             Frame::OnNextTab)
    EVT_MENU(XRCID("prev_tab"),                             Frame::OnPrevTab)
    EVT_MENU(XRCID("full_screen"),                          Frame::OnShowFullScreen)
    EVT_MENU(XRCID("show_nav_toolbar"),                     Frame::OnShowNavBar)
    EVT_MENU(XRCID("toogle_main_toolbars"),                 Frame::OnToggleMainTBars)
    EVT_MENU(XRCID("toogle_plugin_toolbars"),               Frame::OnTogglePluginTBars)
    EVT_MENU(XRCID("toggle_panes"),                         Frame::OnTogglePanes)
    EVT_MENU(XRCID("distraction_free_mode"),                Frame::OnToggleMinimalView)
    EVT_UPDATE_UI(XRCID("distraction_free_mode"),           Frame::OnToggleMinimalViewUI)
    EVT_MENU(XRCID("hide_status_bar"),                      Frame::OnShowStatusBar)
    EVT_UPDATE_UI(XRCID("hide_status_bar"),                 Frame::OnShowStatusBarUI)
    EVT_MENU(XRCID("hide_tool_bar"),                        Frame::OnShowToolbar)
    EVT_UPDATE_UI(XRCID("hide_tool_bar"),                   Frame::OnShowToolbarUI)
    EVT_MENU(XRCID("show_menu_bar"),                        Frame::OnShowMenuBar)
    EVT_UPDATE_UI(XRCID("show_menu_bar"),                   Frame::OnShowMenuBarUI)
    EVT_MENU(XRCID("show_tab_bar"),                         Frame::OnShowTabBar)
    EVT_UPDATE_UI(XRCID("show_tab_bar"),                    Frame::OnShowTabBarUI)
    EVT_MENU_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID,   Frame::DispatchCommandEvent)
    EVT_MENU(wxID_ZOOM_FIT,                                 Frame::DispatchCommandEvent)
    EVT_MENU(wxID_ZOOM_IN,                                  Frame::DispatchCommandEvent)
    EVT_MENU(wxID_ZOOM_OUT,                                 Frame::DispatchCommandEvent)
    EVT_UPDATE_UI(wxID_ZOOM_OUT,                            Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(wxID_ZOOM_IN,                             Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(wxID_ZOOM_OUT,                            Frame::OnFileExistUpdateUI)

    EVT_UPDATE_UI(XRCID("word_wrap"),                       Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("toggle_fold"),                     Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("fold_all"),                        Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("fold_all_in_selection"),           Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("fold_topmost_in_selection"),       Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("display_eol"),                     Frame::OnViewDisplayEOL_UI)
    EVT_UPDATE_UI(XRCID("next_tab"),                        Frame::OnNextPrevTab_UI)
    EVT_UPDATE_UI(XRCID("prev_tab"),                        Frame::OnNextPrevTab_UI)
    EVT_UPDATE_UI(XRCID("whitepsace_invisible"),            Frame::OnShowWhitespaceUI)
    EVT_UPDATE_UI(XRCID("whitepsace_always"),               Frame::OnShowWhitespaceUI)
    EVT_UPDATE_UI(XRCID("whitespace_visiable_after_indent"),Frame::OnShowWhitespaceUI)
    EVT_UPDATE_UI(XRCID("whitespace_indent_only"),          Frame::OnShowWhitespaceUI)
    EVT_UPDATE_UI(XRCID("show_nav_toolbar"),                Frame::OnShowNavBarUI)
    EVT_UPDATE_UI(viewAsSubMenuID,                          Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI_RANGE(viewAsMenuItemID, viewAsMenuItemMaxID, Frame::DispatchUpdateUIEvent)

    //-------------------------------------------------------
    // Search menu
    //-------------------------------------------------------
    EVT_MENU(wxID_FIND,                                     Frame::DispatchCommandEvent)
    EVT_MENU(wxID_REPLACE,                                  Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("find_resource"),                        Frame::OnFindResourceXXX)
    EVT_MENU(XRCID("incremental_search"),                   Frame::OnIncrementalSearch)
    EVT_MENU(XRCID("incremental_replace"),                  Frame::OnIncrementalReplace)
    EVT_MENU(XRCID("find_symbol"),                          Frame::OnQuickOutline)
    EVT_MENU(XRCID("goto_definition"),                      Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("goto_previous_definition"),             Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("goto_linenumber"),                      Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("toggle_bookmark"),                      Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("next_bookmark"),                        Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("previous_bookmark"),                    Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("removeall_current_bookmarks"),          Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("removeall_bookmarks"),                  Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("next_fif_match"),                       Frame::OnNextFiFMatch)
    EVT_MENU(XRCID("previous_fif_match"),                   Frame::OnPreviousFiFMatch)
    EVT_MENU(XRCID("grep_current_file"),                    Frame::OnGrepWord)
    EVT_MENU(XRCID("grep_current_workspace"),               Frame::OnGrepWord)
    EVT_MENU(XRCID("web_search_selection"),                 Frame::OnWebSearchSelection)
    EVT_MENU(XRCID("ID_QUICK_ADD_NEXT"),                    Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("ID_QUICK_FIND_ALL"),                    Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("ID_GOTO_ANYTHING"),                     Frame::OnGotoAnything)
    EVT_MENU(XRCID("wxEVT_BOOK_NAV_NEXT"),                  Frame::OnMainBookNavigating)
    EVT_MENU(XRCID("wxEVT_BOOK_NAV_PREV"),                  Frame::OnMainBookNavigating)
    EVT_MENU(XRCID("wxEVT_BOOK_MOVE_TAB_LEFT"),             Frame::OnMainBookMovePage)
    EVT_MENU(XRCID("wxEVT_BOOK_MOVE_TAB_RIGHT"),            Frame::OnMainBookMovePage)
    EVT_UPDATE_UI(XRCID("ID_QUICK_ADD_NEXT"),               Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("ID_QUICK_FIND_ALL"),               Frame::OnFileExistUpdateUI)

    EVT_UPDATE_UI(wxID_FIND,                                Frame::OnIncrementalSearchUI)
    EVT_UPDATE_UI(wxID_REPLACE,                             Frame::OnIncrementalSearchUI)
    EVT_UPDATE_UI(XRCID("select_previous"),                 Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("select_next"),                     Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("find_next"),                       Frame::OnFileExistUpdateUI)
    EVT_MENU(XRCID("find_next"),                            Frame::OnFindSelection)
    EVT_UPDATE_UI(XRCID("find_previous"),                   Frame::OnFileExistUpdateUI)
    EVT_MENU(XRCID("find_previous"),                        Frame::OnFindSelectionPrev)
    EVT_UPDATE_UI(XRCID("find_next_at_caret"),              Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("find_previous_at_caret"),          Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("incremental_search"),              Frame::OnIncrementalSearchUI)
    EVT_UPDATE_UI(XRCID("find_resource"),                   Frame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("find_type"),                       Frame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("find_function"),                   Frame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("find_macro"),                      Frame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("find_typedef"),                    Frame::OnWorkspaceOpen)

    EVT_UPDATE_UI(XRCID("find_symbol"),                     Frame::OnCompleteWordUpdateUI)
    EVT_UPDATE_UI(XRCID("goto_definition"),                 Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("goto_previous_definition"),        Frame::DispatchUpdateUIEvent)
    EVT_UPDATE_UI(XRCID("goto_linenumber"),                 Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("toggle_bookmark"),                 Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("next_bookmark"),                   Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("previous_bookmark"),               Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("removeall_bookmarks"),             Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("next_fif_match"),                  Frame::OnNextFiFMatchUI)
    EVT_UPDATE_UI(XRCID("previous_fif_match"),              Frame::OnPreviousFiFMatchUI)
    EVT_UPDATE_UI(XRCID("grep_current_file"),               Frame::OnGrepWordUI)
    EVT_UPDATE_UI(XRCID("grep_current_workspace"),          Frame::OnGrepWordUI)
    EVT_UPDATE_UI(XRCID("web_search_selection"),            Frame::OnWebSearchSelectionUI)

    //-------------------------------------------------------
    // Project menu
    //-------------------------------------------------------
    EVT_MENU(XRCID("local_workspace_prefs"),                Frame::OnWorkspaceEditorPreferences)
    EVT_MENU(XRCID("local_workspace_settings"),             Frame::OnWorkspaceSettings)
    EVT_MENU(XRCID("new_workspace"),                        Frame::OnProjectNewWorkspace)
    EVT_MENU(XRCID("file_new_workspace"),                   Frame::OnProjectNewWorkspace)
    EVT_UPDATE_UI(XRCID("file_new_workspace"),              Frame::OnNewWorkspaceUI)
    EVT_UPDATE_UI(XRCID("new_workspace"),                   Frame::OnNewWorkspaceUI)
    EVT_UPDATE_UI(XRCID("import_from_msvs"),                Frame::OnNewWorkspaceUI)
    EVT_MENU(XRCID("switch_to_workspace"),                  Frame::OnSwitchWorkspace)
    EVT_MENU(XRCID("file_switch_to_workspace"),             Frame::OnSwitchWorkspace)
    EVT_UPDATE_UI(XRCID("switch_to_workspace"),             Frame::OnSwitchWorkspaceUI)
    EVT_UPDATE_UI(XRCID("file_switch_to_workspace"),        Frame::OnSwitchWorkspaceUI)
    EVT_MENU(XRCID("close_workspace"),                      Frame::OnCloseWorkspace)
    EVT_MENU(XRCID("file_close_workspace"),                 Frame::OnCloseWorkspace)
    EVT_MENU(XRCID("reload_workspace"),                     Frame::OnReloadWorkspace)
    EVT_MENU(XRCID("import_from_msvs"),                     Frame::OnImportMSVS)
    EVT_MENU(XRCID("new_project"),                          Frame::OnProjectNewProject)
    EVT_MENU(XRCID("file_new_project"),                     Frame::OnProjectNewProject)
    EVT_MENU(XRCID("add_project"),                          Frame::OnProjectAddProject)
    EVT_MENU(XRCID("reconcile_project"),                    Frame::OnReconcileProject)
    EVT_MENU(XRCID("retag_workspace"),                      Frame::OnRetagWorkspace)
    EVT_MENU(XRCID("full_retag_workspace"),                 Frame::OnRetagWorkspace)
    EVT_MENU(XRCID("project_properties"),                   Frame::OnShowActiveProjectSettings)

    EVT_UPDATE_UI(XRCID("local_workspace_prefs"),           Frame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("local_workspace_settings"),        Frame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("close_workspace"),                 Frame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("file_close_workspace"),            Frame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("reload_workspace"),                Frame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("add_project"),                     Frame::OnWorkspaceMenuUI)
    EVT_UPDATE_UI(XRCID("file_new_project"),                Frame::OnWorkspaceOpen)
    EVT_UPDATE_UI(XRCID("new_project"),                     Frame::OnNewProjectUI)
    EVT_UPDATE_UI(XRCID("reconcile_project"),               Frame::OnShowActiveProjectSettingsUI)
    EVT_UPDATE_UI(XRCID("retag_workspace"),                 Frame::OnRetagWorkspaceUI)
    EVT_UPDATE_UI(XRCID("full_retag_workspace"),            Frame::OnRetagWorkspaceUI)
    EVT_UPDATE_UI(XRCID("project_properties"),              Frame::OnShowActiveProjectSettingsUI)

    //-------------------------------------------------------
    // Build menu
    //-------------------------------------------------------
    EVT_MENU(XRCID("execute_no_debug"),                     Frame::OnExecuteNoDebug)
    EVT_MENU(XRCID("stop_executed_program"),                Frame::OnStopExecutedProgram)
    EVT_MENU(XRCID("build_active_project"),                 Frame::OnBuildProject)
    EVT_MENU(XRCID("build_active_project_only"),            Frame::OnBuildProjectOnly)
    EVT_TOOL_DROPDOWN(XRCID("build_active_project"),        Frame::OnShowBuildMenu)
    EVT_MENU(XRCID("compile_active_file"),                  Frame::OnCompileFile)
    EVT_MENU(XRCID("clean_active_project"),                 Frame::OnCleanProject)
    EVT_MENU(XRCID("clean_active_project_only"),            Frame::OnCleanProjectOnly)
    EVT_MENU(XRCID("stop_active_project_build"),            Frame::OnStopBuild)
    EVT_MENU(XRCID("rebuild_active_project"),               Frame::OnRebuildProject)
    EVT_MENU(XRCID("build_n_run_active_project"),           Frame::OnBuildAndRunProject)
    EVT_MENU(XRCID("build_workspace"),                      Frame::OnBuildWorkspace)
    EVT_MENU(XRCID("clean_workspace"),                      Frame::OnCleanWorkspace)
    EVT_MENU(XRCID("rebuild_workspace"),                    Frame::OnReBuildWorkspace)
    EVT_MENU(XRCID("batch_build"),                          Frame::OnBatchBuild)

    EVT_UPDATE_UI(XRCID("execute_no_debug"),                Frame::OnExecuteNoDebugUI)
    EVT_UPDATE_UI(XRCID("stop_executed_program"),           Frame::OnStopExecutedProgramUI)
    EVT_UPDATE_UI(XRCID("build_active_project"),            Frame::OnBuildProjectUI)
    EVT_UPDATE_UI(XRCID("compile_active_file"),             Frame::OnCompileFileUI)
    EVT_UPDATE_UI(XRCID("clean_active_project"),            Frame::OnCleanProjectUI)
    EVT_UPDATE_UI(XRCID("stop_active_project_build"),       Frame::OnStopBuildUI)
    EVT_UPDATE_UI(XRCID("rebuild_active_project"),          Frame::OnBuildProjectUI)
    EVT_UPDATE_UI(XRCID("build_n_run_active_project"),      Frame::OnBuildProjectUI)
    EVT_UPDATE_UI(XRCID("build_workspace"),                 Frame::OnBuildWorkspaceUI)
    EVT_UPDATE_UI(XRCID("clean_workspace"),                 Frame::OnCleanWorkspaceUI)
    EVT_UPDATE_UI(XRCID("rebuild_workspace"),               Frame::OnReBuildWorkspaceUI)
    EVT_UPDATE_UI(XRCID("batch_build"),                     Frame::OnBatchBuildUI)

    //-------------------------------------------------------
    // Debug menu
    //-------------------------------------------------------
    EVT_MENU(XRCID("debugger_win_locals"),                  Frame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_watches"),                 Frame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_output"),                  Frame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_threads"),                 Frame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_callstack"),               Frame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_memory"),                  Frame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_breakpoints"),             Frame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_asciiview"),               Frame::OnShowDebuggerWindow)
    EVT_MENU(XRCID("debugger_win_disassemble"),             Frame::OnShowDebuggerWindow)
    EVT_UPDATE_UI(XRCID("debugger_win_locals"),             Frame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_watches"),            Frame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_output"),             Frame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_threads"),            Frame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_callstack"),          Frame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_memory"),             Frame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_breakpoints"),        Frame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_asciiview"),          Frame::OnShowDebuggerWindowUI)
    EVT_UPDATE_UI(XRCID("debugger_win_disassemble"),        Frame::OnShowDebuggerWindowUI)
    EVT_MENU(XRCID("start_debugger"),                       Frame::OnDebug)
    EVT_MENU(XRCID("restart_debugger"),                     Frame::OnDebugRestart)
    EVT_MENU(XRCID("attach_debugger"),                      Frame::OnDebugAttach)
    EVT_MENU(XRCID("pause_debugger"),                       Frame::OnDebugCmd)
    EVT_MENU(XRCID("stop_debugger"),                        Frame::OnDebugStop)
    EVT_MENU(XRCID("dbg_run_to_cursor"),                    Frame::OnDebugRunToCursor)
    EVT_MENU(XRCID("dbg_jump_cursor"),                      Frame::OnDebugJumpToCursor)
    EVT_MENU(XRCID("dbg_stepin"),                           Frame::OnDebugCmd)
    EVT_MENU(XRCID("dbg_stepi"),                            Frame::OnDebugCmd)
    EVT_MENU(XRCID("dbg_stepout"),                          Frame::OnDebugCmd)
    EVT_MENU(XRCID("dbg_enable_reverse_debug"),             Frame::OnToggleReverseDebugging)
    EVT_MENU(XRCID("dbg_start_recording"),                  Frame::OnToggleReverseDebuggingRecording)
    EVT_MENU(XRCID("dbg_next"),                             Frame::OnDebugCmd)
    EVT_MENU(XRCID("dbg_nexti"),                            Frame::OnDebugCmd)
    EVT_MENU(XRCID("show_cursor"),                          Frame::OnDebugCmd)
    EVT_MENU(XRCID("add_breakpoint"),                       Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("insert_breakpoint"),                    Frame::DispatchCommandEvent) // Toggles
    EVT_MENU(XRCID("disable_all_breakpoints"),              Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("enable_all_breakpoints"),               Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("delete_all_breakpoints"),               Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("insert_temp_breakpoint"),               Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("insert_disabled_breakpoint"),           Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("insert_cond_breakpoint"),               Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("edit_breakpoint"),                      Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("show_breakpoint_dlg"),                  Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("insert_watchpoint"),                    Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("toggle_breakpoint_enabled_status"),     Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("ignore_breakpoint"),                    Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("delete_breakpoint"),                    Frame::DispatchCommandEvent)
    EVT_MENU(XRCID("quick_debug"),                          Frame::OnQuickDebug)
    EVT_MENU(XRCID("debug_core_dump"),                      Frame::OnDebugCoreDump)

    EVT_UPDATE_UI(XRCID("start_debugger"),                  Frame::OnDebugUI)
    EVT_UPDATE_UI(XRCID("restart_debugger"),                Frame::OnDebugRestartUI)
    EVT_UPDATE_UI(XRCID("stop_debugger"),                   Frame::OnDebugStopUI)
    EVT_UPDATE_UI(XRCID("dbg_stepin"),                      Frame::OnDebugCmdUI)
    EVT_UPDATE_UI(XRCID("dbg_stepi"),                       Frame::OnDebugCmdUI)
    EVT_UPDATE_UI(XRCID("dbg_stepout"),                     Frame::OnDebugCmdUI)
    EVT_UPDATE_UI(XRCID("dbg_next"),                        Frame::OnDebugCmdUI)

    EVT_UPDATE_UI(XRCID("dbg_enable_reverse_debug"),        Frame::OnToggleReverseDebuggingUI)
    EVT_UPDATE_UI(XRCID("dbg_start_recording"),             Frame::OnToggleReverseDebuggingRecordingUI)
    EVT_UPDATE_UI(XRCID("pause_debugger"),                  Frame::OnDebugInterruptUI)
    EVT_UPDATE_UI(XRCID("dbg_nexti"),                       Frame::OnDebugStepInstUI)
    EVT_UPDATE_UI(XRCID("show_cursor"),                     Frame::OnDebugShowCursorUI)
    EVT_UPDATE_UI(XRCID("dbg_run_to_cursor"),               Frame::OnDebugRunToCursorUI)
    EVT_UPDATE_UI(XRCID("dbg_jump_cursor"),                 Frame::OnDebugJumpToCursorUI)

    EVT_UPDATE_UI(XRCID("insert_breakpoint"),               Frame::OnDebugManageBreakpointsUI)
    EVT_UPDATE_UI(XRCID("disable_all_breakpoints"),         Frame::OnDebugManageBreakpointsUI)
    EVT_UPDATE_UI(XRCID("enable_all_breakpoints"),          Frame::OnDebugManageBreakpointsUI)
    EVT_UPDATE_UI(XRCID("delete_all_breakpoints"),          Frame::OnDebugManageBreakpointsUI)
    EVT_UPDATE_UI(XRCID("quick_debug"),                     Frame::OnQuickDebugUI)
    EVT_UPDATE_UI(XRCID("debug_core_dump"),                 Frame::OnQuickDebugUI)

    //-------------------------------------------------------
    // Plugins menu
    //-------------------------------------------------------
    EVT_MENU(XRCID("manage_plugins"),                       Frame::OnManagePlugins)

    //-------------------------------------------------------
    // Settings menu
    //-------------------------------------------------------
    EVT_MENU(wxID_PREFERENCES,                              Frame::OnViewOptions)
    EVT_MENU(XRCID("add_envvar"),                           Frame::OnAddEnvironmentVariable)
    EVT_MENU(XRCID("advance_settings"),                     Frame::OnAdvanceSettings)
    EVT_MENU(XRCID("tags_options"),                         Frame::OnCtagsOptions)

    //-------------------------------------------------------
    // Help menu
    //-------------------------------------------------------
    EVT_MENU(wxID_ABOUT,                                    Frame::OnAbout)
    EVT_MENU(XRCID("wxID_REPORT_BUG"),                      Frame::OnReportIssue)
    //EVT_MENU(XRCID("check_for_update"),                     Frame::OnCheckForUpdate)
    //EVT_MENU(XRCID("run_setup_wizard"),                     Frame::OnRunSetupWizard)

    //-------------------------------------------------------
    // Perspective menu
    //-------------------------------------------------------
    EVT_MENU_RANGE(PERSPECTIVE_FIRST_MENU_ID, PERSPECTIVE_LAST_MENU_ID, Frame::OnChangePerspective)
    EVT_UPDATE_UI_RANGE(PERSPECTIVE_FIRST_MENU_ID, PERSPECTIVE_LAST_MENU_ID, Frame::OnChangePerspectiveUI)
    EVT_MENU(XRCID("manage_perspectives"),                  Frame::OnManagePerspectives)
    EVT_MENU(XRCID("save_current_layout"),                  Frame::OnSaveLayoutAsPerspective)

    //-----------------------------------------------------------------
    // Toolbar
    //-----------------------------------------------------------------
    EVT_MENU(wxID_FORWARD,                                  Frame::OnBackwardForward)
    EVT_MENU(wxID_BACKWARD,                                 Frame::OnBackwardForward)
    EVT_MENU(XRCID("highlight_word"),                       Frame::OnHighlightWord)

    EVT_UPDATE_UI(wxID_FORWARD,                             Frame::OnBackwardForwardUI)
    EVT_UPDATE_UI(wxID_BACKWARD,                            Frame::OnBackwardForwardUI)

    //-------------------------------------------------------
    // Workspace Pane tab context menu
    //-------------------------------------------------------
    EVT_MENU(XRCID("detach_wv_tab"),                        Frame::OnDetachWorkspaceViewTab)
    EVT_MENU(XRCID("hide_wv_tab"),                          Frame::OnHideWorkspaceViewTab)
    EVT_MENU(XRCID("hide_ov_tab"),                          Frame::OnHideOutputViewTab)

    //-------------------------------------------------------
    // Debugger Pane tab context menu
    //-------------------------------------------------------
    EVT_MENU(XRCID("detach_debugger_tab"),                  Frame::OnDetachDebuggerViewTab)

    //-------------------------------------------------------
    // Editor tab context menu
    //-------------------------------------------------------
    EVT_MENU(XRCID("close_other_tabs"),                     Frame::OnCloseAllButThis)
    EVT_MENU(XRCID("close_tabs_to_the_right"),              Frame::OnCloseTabsToTheRight)
    EVT_MENU(XRCID("copy_file_relative_path_to_workspace"), Frame::OnCopyFilePathRelativeToWorkspace)
    EVT_MENU(XRCID("copy_file_name"),                       Frame::OnCopyFilePath)
    EVT_MENU(XRCID("copy_file_path"),                       Frame::OnCopyFilePathOnly)
    EVT_MENU(XRCID("copy_file_name_only"),                  Frame::OnCopyFileName)
    EVT_MENU(XRCID("open_shell_from_filepath"),             Frame::OnOpenShellFromFilePath)
    EVT_MENU(XRCID("open_file_explorer"),                   Frame::OnOpenFileExplorerFromFilePath)
    EVT_MENU(XRCID("ID_DETACH_EDITOR"),                     Frame::OnDetachEditor)
    EVT_MENU(XRCID("mark_readonly"),                        Frame::OnMarkEditorReadonly)

    EVT_UPDATE_UI(XRCID("mark_readonly"),                   Frame::OnMarkEditorReadonlyUI)
    EVT_UPDATE_UI(XRCID("copy_file_relative_path_to_workspace"), Frame::OnCopyFilePathRelativeToWorkspaceUI)
    EVT_UPDATE_UI(XRCID("copy_file_name"),                  Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("copy_file_path"),                  Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("open_shell_from_filepath"),        Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("open_file_explorer"),              Frame::OnFileExistUpdateUI)
    EVT_UPDATE_UI(XRCID("ID_DETACH_EDITOR"),                Frame::OnDetachEditorUI)

    //-----------------------------------------------------------------
    // Default editor context menu
    //-----------------------------------------------------------------
    EVT_MENU(wxID_DELETE,                                   Frame::DispatchCommandEvent)
    EVT_UPDATE_UI(wxID_DELETE,                              Frame::DispatchUpdateUIEvent)

    //-----------------------------------------------------------------
    // C++ editor context menu
    //-----------------------------------------------------------------
    EVT_MENU(XRCID("open_include_file"),                    Frame::OnCppContextMenu)
    EVT_MENU(XRCID("add_include_file"),                     Frame::OnCppContextMenu)
    EVT_MENU(XRCID("add_forward_decl"),                     Frame::OnCppContextMenu)
    EVT_MENU(XRCID("swap_files"),                           Frame::OnCppContextMenu)
    EVT_MENU(XRCID("find_decl"),                            Frame::OnCppContextMenu)
    EVT_MENU(XRCID("find_impl"),                            Frame::OnCppContextMenu)
    EVT_MENU(XRCID("go_to_function_start"),                 Frame::OnCppContextMenu)
    EVT_MENU(XRCID("go_to_next_function"),                  Frame::OnCppContextMenu)
    EVT_MENU(XRCID("insert_doxy_comment"),                  Frame::OnCppContextMenu)
    EVT_MENU(XRCID("setters_getters"),                      Frame::OnCppContextMenu)
    EVT_MENU(XRCID("move_impl"),                            Frame::OnCppContextMenu)
    EVT_MENU(XRCID("add_impl"),                             Frame::OnCppContextMenu)
    EVT_MENU(XRCID("sync_signatures"),                      Frame::OnCppContextMenu)
    EVT_MENU(XRCID("add_multi_impl"),                       Frame::OnCppContextMenu)
    EVT_MENU(XRCID("add_virtual_impl"),                     Frame::OnCppContextMenu)
    EVT_MENU(XRCID("add_pure_virtual_impl"),                Frame::OnCppContextMenu)
    EVT_MENU(XRCID("rename_symbol"),                        Frame::OnCppContextMenu)
    EVT_MENU(XRCID("find_references"),                      Frame::OnCppContextMenu)
    EVT_MENU(XRCID("comment_selection"),                    Frame::OnCppContextMenu)
    EVT_MENU(XRCID("comment_line"),                         Frame::OnCppContextMenu)
    EVT_MENU(XRCID("retag_file"),                           Frame::OnCppContextMenu)

    //-----------------------------------------------------------------
    // Hyperlinks
    //-----------------------------------------------------------------
    EVT_HTML_LINK_CLICKED(wxID_ANY,                         Frame::OnLinkClicked)
    EVT_MENU(XRCID("link_action"),                          Frame::OnStartPageEvent)

    //-----------------------------------------------------------------
    // gsgs-specific events
    //-----------------------------------------------------------------
    EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_MESSAGE,               Frame::OnParsingThreadMessage)
    EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_CLEAR_TAGS_CACHE,      Frame::OnClearTagsCache)
    EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_RETAGGING_COMPLETED,   Frame::OnRetaggingCompelted)
    EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_RETAGGING_PROGRESS,    Frame::OnRetaggingProgress)
    EVT_COMMAND(wxID_ANY, wxEVT_PARSE_THREAD_READY,                 Frame::OnParserThreadReady)

    EVT_COMMAND(wxID_ANY, wxEVT_ACTIVATE_EDITOR,                    Frame::OnActivateEditor)

    EVT_COMMAND(wxID_ANY, wxEVT_TAGS_DB_UPGRADE,                    Frame::OnDatabaseUpgrade)
    EVT_COMMAND(wxID_ANY, wxEVT_TAGS_DB_UPGRADE_INTER,              Frame::OnDatabaseUpgradeInternally)
    EVT_COMMAND(wxID_ANY, wxEVT_REFRESH_PERSPECTIVE_MENU,           Frame::OnRefreshPerspectiveMenu)
    EVT_MENU(XRCID("update_num_builders_count"),                    Frame::OnUpdateNumberOfBuildProcesses)
    EVT_MENU(XRCID("goto_gsgs_download_url"),                       Frame::OnGotoDownloadPage)

    EVT_COMMAND(wxID_ANY, wxEVT_CMD_NEW_VERSION_AVAILABLE,          Frame::OnNewVersionAvailable)
    EVT_COMMAND(wxID_ANY, wxEVT_CMD_VERSION_UPTODATE,               Frame::OnNewVersionAvailable)
    EVT_COMMAND(wxID_ANY, wxEVT_CMD_VERSION_CHECK_ERROR,            Frame::OnVersionCheckError)

    EVT_COMMAND(wxID_ANY, wxEVT_CMD_NEW_DOCKPANE,                   Frame::OnNewDetachedPane)
    EVT_COMMAND(wxID_ANY, wxEVT_LOAD_PERSPECTIVE,                   Frame::OnLoadPerspective)
    EVT_COMMAND(wxID_ANY, wxEVT_CMD_DELETE_DOCKPANE,                Frame::OnDestroyDetachedPane)
    END_EVENT_TABLE()
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    // Frame
    //-----------------------------------------------------------------
    //-----------------------------------------------------------------
    Frame * Frame::mMainFrame = NULL;
    bool Frame::m_initCompleted = false;
    //-----------------------------------------------------------------
    Frame::Frame(wxWindow * pParent, wxWindowID id, bool recover, long style)
        :
        mRecover(recover)
        ,m_workspaceRetagIsRequired(false)
        , m_noSavePerspectivePrompt(false)
        , m_highlightWord(false)
        , m_buildAndRun(false)
        , m_bookmarksDropDownMenu(NULL)
        , m_singleInstanceThread(NULL)
        , mInfoBar(NULL)

    #ifdef __WXGTK__
        , m_isWaylandSession(false)
    #endif
        , m_webUpdate(NULL)
        , mToolBar(NULL)
    {
        wxWindowUpdateLocker llt(this);
        SetBackgroundStyle(wxBG_STYLE_PAINT);
        mMainFrame = this;
        mWinMag = &gsgs_Window();
        mWinMag->SetMainFrame(this);

        mFrameInfo = new GeneralInfo();
        gsgs_EditorConfig().ReadObject(wxT("GeneralInfo"), mFrameInfo);
        int screenW = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
        int screenH = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
        if(mFrameInfo->GetFramePosition().x < 0 || mFrameInfo->GetFramePosition().x > screenW) {
            mFrameInfo->SetFramePosition(wxPoint(30, 3));
        }
        if(mFrameInfo->GetFramePosition().y < 0 || mFrameInfo->GetFramePosition().y > screenH) {
            mFrameInfo->SetFramePosition(wxPoint(30, 3));
        }
        wxSize frameSize(mFrameInfo->GetFrameSize());
        if(mFrameInfo->GetFrameSize().x < 600 || mFrameInfo->GetFrameSize().x > screenW) {
            frameSize.SetWidth(600);
        }
        if(mFrameInfo->GetFrameSize().y < 400 || mFrameInfo->GetFrameSize().y > screenH) {
            frameSize.SetHeight(400);
        }
        mFrameInfo->SetFrameSize(frameSize);

    #ifdef __WXOSX__
        mFrameInfo->SetFramePosition(wxPoint(30, 100));
        mFrameInfo->SetFrameSize(wxSize(600, 400));
    #endif
        if(!wxFrame::Create(pParent, id, _("光栅故事 ") + GSGS_VERSION, mFrameInfo->GetFramePosition(), mFrameInfo->GetFrameSize(), style))
            return;

        gsgs_Style().RegisterWindow(this);

    #if defined(__WXGTK20__)
        // A rather ugly hack here.  GTK V2 insists that F10 should be the
        // accelerator for the menu bar.  We don't want that.  There is
        // no sane way to turn this off, but we *can* get the same effect
        // by setting the "menu bar accelerator" property to the name of a
        // function key that is apparently legal, but doesn't really exist.
        // (Or if it does, it certainly isn't a key we use.)
        gtk_settings_set_string_property(gtk_settings_get_default(), "gtk-menu-bar-accel", "F15", "foo");

    #endif
        gsgs_Config().Load(wxT("config/debuggers.xml"), wxT("5.4"));

        PluginManager::Get()->SetDockingManager(mWinMag);

        m_highlightWord = (bool)gsgs_EditorConfig().GetInteger(wxT("highlight_word"), 0);

        m_frameHelper.Reset(new MainFrameHelper(this, mWinMag));

        gsgs_Workspace();
        gsgs_Manager();
        RefactoringEngine::Instance();
        CodeCompletionManager::Get();

        SearchThreadST::Get()->SetNotifyWindow(this);
        SearchThreadST::Get()->Start(WXTHREAD_MIN_PRIORITY);

        JobQueueSingleton::Instance()->Start(6);

        m_singleInstanceThread = new SInstanceThread();
        m_singleInstanceThread->Start();

        // start the editor creator thread
        m_timer = new wxTimer(this, FrameTimerId);

        CreateViewImpl();

        // plugins must be loaded before the file explorer
        CompleteInitialization();

        // time to create the file explorer
        wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("go_home"));
        GetFileExplorer()->GetEventHandler()->ProcessEvent(e);
#ifndef __WXMSW__
        SendSizeEvent(wxSEND_EVENT_POST);
#endif
        // Keep the current layout before loading the perspective from the disk
        m_defaultLayout = mWinMag->SavePerspective();
        mWinMag->Update();
        // Save the current layout as the "Default" layout (unless we already got one ...)
        gsgs_Manager()->GetPerspectiveManager().SavePerspectiveIfNotExists(NORMAL_LAYOUT);

        // connect common edit events
        wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &Frame::DispatchCommandEvent, this, wxID_COPY);
        wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &Frame::DispatchCommandEvent, this, wxID_PASTE);
        wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &Frame::DispatchCommandEvent, this, wxID_SELECTALL);
        wxTheApp->Bind(wxEVT_COMMAND_MENU_SELECTED, &Frame::DispatchCommandEvent, this, wxID_CUT);
        wxTheApp->Bind(wxEVT_UPDATE_UI,             &Frame::DispatchUpdateUIEvent, this, wxID_COPY);
        wxTheApp->Bind(wxEVT_UPDATE_UI,             &Frame::DispatchUpdateUIEvent, this, wxID_PASTE);
        wxTheApp->Bind(wxEVT_UPDATE_UI,             &Frame::DispatchUpdateUIEvent, this, wxID_SELECTALL);
        wxTheApp->Bind(wxEVT_UPDATE_UI,             &Frame::DispatchUpdateUIEvent, this, wxID_CUT);
        wxTheApp->Bind(wxEVT_ACTIVATE_APP,          &Frame::OnAppActivated, this);
        gsgs_Event().Bind(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED, &Frame::OnEnvironmentVariablesModified, this);
        gsgs_Event().Bind(wxEVT_LOAD_SESSION,       &Frame::OnLoadSession,  this);
        gsgs_Event().Bind(wxEVT_SHELL_COMMAND_PROCESS_ENDED, gsgsEventHandler(Frame::OnBuildEnded), this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_LOADED,   &Frame::OnWorkspaceLoaded, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_CLOSED,   &Frame::OnWorkspaceClosed, this);
        gsgs_Event().Bind(wxEVT_GSGS_THEME_CHANGED,   &Frame::OnTheme, this);
        gsgs_Event().Bind(wxEVT_ACTIVE_EDITOR_CHANGED, &Frame::OnActiveEditorChanged, this);
        gsgs_Event().Bind(wxEVT_EDITOR_SETTINGS_CHANGED, &Frame::OnSettingsChanged, this);
        gsgs_Event().Bind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT, &Frame::OnReloadExternallModifiedNoPrompt, this);
        gsgs_Event().Bind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED, &Frame::OnReloadExternallModified, this);
        gsgs_Event().Bind(wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES, &Frame::OnSingleInstanceOpenFiles, this);
        gsgs_Event().Bind(wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP, &Frame::OnSingleInstanceRaise, this);
        gsgs_Event().Bind(wxEVT_PROJ_RENAMED,       &Frame::OnProjectRenamed, this);
        gsgs_Event().Bind(wxEVT_REFACTOR_ENGINE_REFERENCES, &Frame::OnFindReferences, this);
        gsgs_Event().Bind(wxEVT_REFACTOR_ENGINE_RENAME_SYMBOL, &Frame::OnRenameSymbol, this);
        gsgs_Event().Bind(wxEVT_WORKSPACE_Recently, &Frame::OnRecentlyWorkspace, this);
        gsgs_Event().Bind(GSGS_ProgramClose,        &Frame::OnQuit, this);
        gsgs_Event().Bind(GSGS_ProgramMinimize,     &Frame::OnMinimize, this);
        gsgs_Event().Bind(GSGS_ProgramMaximize,     &Frame::OnMaximize, this);
        mToolBar->Bind(wxEVT_TOOL_DROPDOWN,        &Frame::OnTBUnRedoMenu, this, wxID_UNDO);
        mToolBar->Bind(wxEVT_TOOL_DROPDOWN,        &Frame::OnTBUnRedoMenu, this, wxID_REDO);
        mToolBar->Bind(wxEVT_TOOL,                 &Frame::OnTBUnRedo,     this, wxID_UNDO);
        mToolBar->Bind(wxEVT_TOOL,                 &Frame::OnTBUnRedo,     this, wxID_REDO);
        mInfoBar->Bind(wxEVT_BUTTON,               &Frame::OnInfobarButton, this);

        gsgs_Window().addHotKey("wxEVT_BOOK_NAV_NEXT", "Ctrl-Shift-TAB", _("Switch to Next Tab"));
        gsgs_Window().addHotKey("wxEVT_BOOK_NAV_PREV", "Ctrl-TAB", _("Switch to Previous Tab"));
        gsgs_Window().addHotKey("wxEVT_BOOK_MOVE_TAB_LEFT", "Ctrl-Shift-PGUP", _("Move Tab Left"));
        gsgs_Window().addHotKey("wxEVT_BOOK_MOVE_TAB_RIGHT", "Ctrl-Shift-PGDN", _("Move Tab Right"));
        gsgs_Window().addHotKey("selection_to_multi_caret", "Ctrl-Shift-L", _("Edit::Split selection into multiple carets"));
        gsgs_Window().addHotKey("incremental_replace", "", _("Search::Toggle the Quick-Replace Bar"));
        gsgs_Window().addHotKey("copy_file_relative_path_to_workspace", "Ctrl-Alt-Shift-C", "Edit::Copy Path Relative to Clipboard");
        gsgs_Window().addHotKey("copy_file_name", "", "Edit::Copy Path to Clipboard");
        gsgs_Window().addHotKey("copy_file_path", "", "Edit::Copy Full Path to Clipboard");
        gsgs_Window().addHotKey("copy_file_name_only", "", "Edit::Copy File Name to Clipboard");
        gsgs_Window().addHotKey("open_shell_from_filepath", "Ctrl-Shift-T", "Search::Open Shell From File Path");
        gsgs_Window().addHotKey("open_file_explorer", "Ctrl-Alt-Shift-T", "Search::Open Containing Folder");
    #ifdef __WXGTK__
        // Try to detect if this is a Wayland session; we have some Wayland-workaround code
        wxString sesstype("XDG_SESSION_TYPE"), session_type;
        wxGetEnv(sesstype, &session_type);
        m_isWaylandSession = session_type.Lower().Contains("wayland");
    #endif
        m_timer->Start(500, true);
    }
    //-----------------------------------------------------------------
    Frame::~Frame(void)
    {
        delete mFrameInfo;
        wxDELETE(m_singleInstanceThread);
        wxDELETE(m_webUpdate);
        wxDELETE(m_timer);

    #ifndef __WXMSW__
        m_zombieReaper.Stop();
    #endif
        CodeCompletionManager::Release();
        RefactoringEngine::Shutdown();

        wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED,   &Frame::DispatchCommandEvent, this, wxID_COPY);
        wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED,   &Frame::DispatchCommandEvent, this, wxID_PASTE);
        wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED,   &Frame::DispatchCommandEvent, this, wxID_SELECTALL);
        wxTheApp->Unbind(wxEVT_COMMAND_MENU_SELECTED,   &Frame::DispatchCommandEvent, this, wxID_CUT);
        wxTheApp->Unbind(wxEVT_UPDATE_UI,               &Frame::DispatchUpdateUIEvent, this, wxID_COPY);
        wxTheApp->Unbind(wxEVT_UPDATE_UI,               &Frame::DispatchUpdateUIEvent, this, wxID_PASTE);
        wxTheApp->Unbind(wxEVT_UPDATE_UI,               &Frame::DispatchUpdateUIEvent, this, wxID_SELECTALL);
        wxTheApp->Unbind(wxEVT_UPDATE_UI,               &Frame::DispatchUpdateUIEvent, this, wxID_CUT);
        wxTheApp->Unbind(wxEVT_ACTIVATE_APP,            &Frame::OnAppActivated, this);
        gsgs_Event().Unbind(wxEVT_REFACTOR_ENGINE_REFERENCES, &Frame::OnFindReferences, this);
        gsgs_Event().Unbind(wxEVT_REFACTOR_ENGINE_RENAME_SYMBOL, &Frame::OnRenameSymbol, this);
        gsgs_Event().Unbind(wxEVT_ENVIRONMENT_VARIABLES_MODIFIED, &Frame::OnEnvironmentVariablesModified, this);
        gsgs_Event().Unbind(wxEVT_SHELL_COMMAND_PROCESS_ENDED, gsgsEventHandler(Frame::OnBuildEnded), this);
        gsgs_Event().Unbind(wxEVT_LOAD_SESSION,         &Frame::OnLoadSession, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_LOADED,     &Frame::OnWorkspaceLoaded, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_CLOSED,     &Frame::OnWorkspaceClosed,  this);
        gsgs_Event().Unbind(wxEVT_GSGS_THEME_CHANGED,   &Frame::OnTheme, this);
        gsgs_Event().Unbind(wxEVT_ACTIVE_EDITOR_CHANGED,&Frame::OnActiveEditorChanged, this);
        gsgs_Event().Unbind(wxEVT_EDITOR_SETTINGS_CHANGED, &Frame::OnSettingsChanged, this);
        gsgs_Event().Unbind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED_NOPROMPT, &Frame::OnReloadExternallModifiedNoPrompt, this);
        gsgs_Event().Unbind(wxEVT_CMD_SINGLE_INSTANCE_THREAD_OPEN_FILES, &Frame::OnSingleInstanceOpenFiles, this);
        gsgs_Event().Unbind(wxEVT_CMD_SINGLE_INSTANCE_THREAD_RAISE_APP, &Frame::OnSingleInstanceRaise, this);
        gsgs_Event().Unbind(wxEVT_CMD_RELOAD_EXTERNALLY_MODIFIED, &Frame::OnReloadExternallModified, this);
        gsgs_Event().Unbind(wxEVT_PROJ_RENAMED,         &Frame::OnProjectRenamed, this);
        gsgs_Event().Unbind(wxEVT_WORKSPACE_Recently,   &Frame::OnRecentlyWorkspace, this);
        gsgs_Event().Unbind(GSGS_ProgramClose,          &Frame::OnQuit, this);
        gsgs_Event().Unbind(GSGS_ProgramMinimize,       &Frame::OnMinimize, this);
        gsgs_Event().Unbind(GSGS_ProgramMaximize,       &Frame::OnMaximize, this);
        mToolBar->Unbind(wxEVT_TOOL,                   &Frame::OnTBUnRedo,     this, wxID_UNDO);
        mToolBar->Unbind(wxEVT_TOOL,                   &Frame::OnTBUnRedo,     this, wxID_REDO);
        mToolBar->Unbind(wxEVT_TOOL_DROPDOWN,          &Frame::OnTBUnRedoMenu, this, wxID_UNDO);
        mToolBar->Unbind(wxEVT_TOOL_DROPDOWN,          &Frame::OnTBUnRedoMenu, this, wxID_REDO);
        mInfoBar->Unbind(wxEVT_BUTTON,                 &Frame::OnInfobarButton, this);

        gsgs_Manager()->GetPerspectiveManager().DisconnectEvents();
        mWinMag->UnInit();
        ManagerST::Free();
        gsgs_Path().RemoveTempDir();
        //SetBackgroundStyle(wxBG_STYLE_PAINT);
    }
    //-----------------------------------------------------------------
    Frame * Frame::setup(bool recover)
    {
        if (!mMainFrame)
            //return new Frame(NULL, wxID_ANY, recover, wxDEFAULT_FRAME_STYLE | wxNO_FULL_REPAINT_ON_RESIZE);

            return new Frame(NULL, wxID_ANY, recover, wxNO_FULL_REPAINT_ON_RESIZE |/* wxFRAME_NO_TASKBAR |*/ wxNO_BORDER);
        return mMainFrame;
    }
    //-----------------------------------------------------------------
    Frame * Frame::singleton()
    { 
        return mMainFrame; 
    }
    //-----------------------------------------------------------------
    void Frame::CreateViewImpl()
    {
        Maximize(mFrameInfo->GetFlags() & GSGS_MAXIMIZE_FRAME ? true : false);
        SetSizer(new wxBoxSizer(wxVERTICAL));
        SetAutoLayout(false);
        wxMenuBar * mb = wxXmlResource::Get()->LoadMenuBar(wxT("main_menu"));
#ifdef __WXOSX__
        wxMenu* view = NULL;
        wxMenuItem* item = mb->FindItem(XRCID("show_tabs_tab"), &view);
        if (item && view) 
        { 
            view->Remove(item); 
        }
    #if wxCHECK_VERSION(3, 1, 0)
        EnableFullScreenView();
    #endif
#endif
        mMenu = new MenuBar(this, mb);
#ifdef __WXGTK__
        GetMenuBar()->Show(gsgs_Config().Read(kConfigShowMenuBar, true));
#endif
        mInfoBar = new InfoBar(this);
        mPanel = new wxPanel(this);

        GetSizer()->Add(mMenu, 0, wxEXPAND);
        GetSizer()->Add(mInfoBar, 0, wxEXPAND);
        GetSizer()->Add(mPanel, 1, wxEXPAND);

        loadLogo();

        mWinMag->SetManagedWindow(mPanel);
        mWinMag->SetArtProvider(new DockArt(PluginManager::Get()));
        mWinMag->SetDockSizeConstraint(0.5, 0.5);
        UpdateAUI();

        int captionSize = 22;
        {
            wxMemoryDC memDC;
            wxBitmap bmp(1, 1);
            memDC.SelectObject(bmp);
            memDC.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));
            wxSize textSize = memDC.GetTextExtent("Tp");
            captionSize = textSize.y + 6;
        }

        mWinMag->GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, wxAUI_GRADIENT_HORIZONTAL);
        mWinMag->GetArtProvider()->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, captionSize);
        mWinMag->GetArtProvider()->SetColor(wxAUI_DOCKART_SASH_COLOUR, gsgs_Style().GetAppActiveTextColour());
        mWinMag->GetArtProvider()->SetColor(wxAUI_DOCKART_BACKGROUND_COLOUR, gsgs_Style().GetAppActiveTextColour());
        mWinMag->GetArtProvider()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);
        mWinMag->GetArtProvider()->SetMetric(wxAUI_DOCKART_SASH_SIZE, 1);

        OnRecentlyWorkspace(wxCommandEvent());
        OnRefreshPerspectiveMenu(wxCommandEvent());

        // Connect to Edit menu, so that its labelled-state submenu can be added on the fly when necessary
        wxMenu* editmenu = NULL;
        wxMenuItem* menuitem = GetMenuBar()->FindItem(wxID_UNDO, &editmenu);
        if(menuitem && editmenu) 
        {
            editmenu->Bind(wxEVT_MENU_OPEN, wxMenuEventHandler(Frame::OnEditMenuOpened), this);
        }

        gsgs_EditorConfig().GetOptions()->SetWorkspaceTabsDirection(wxBOTTOM);
        gsgs_EditorConfig().GetOptions()->SetOutputTabsDirection(wxBOTTOM);

        mConfigPanel = new ConfigurationPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
        mWorkBench = new WorkBench(mPanel, mWinMag);

        wxArrayString files; 
        gsgs_Config().GetRecentOpen(files);
        FileHistory & hs = GetMainBook()->GetRecentlyOpenedFilesClass();

        wxMenu* menu = NULL;
        wxMenuItem* item = GetMenuBar()->FindItem(XRCID("recent_files"), &menu);
        if (item && menu) {
            wxMenu* submenu = item->GetSubMenu();
            if (submenu) {
                for (size_t i = files.GetCount(); i > 0; --i) {
                    hs.AddFileToHistory(files.Item(i - 1));
                }
                // set this menu as the recent file menu
                hs.SetBaseId(RecentFilesSubMenuID + 1);
                hs.UseMenu(submenu);
                hs.AddFilesToMenu();
            }
        }

        mStatusBar = new StatusBar(this, PluginManager::Get());
        SetStatusBar(mStatusBar);

        if(!gsgs_Settings().Load(wxT("2.1"))) 
        {
            gsgs_Error("Can't locate configuration");
        }

        gsgs_Config().ReadItem(&m_tagsOptionsData);
        TagsOptionsData tmp;
        m_tagsOptionsData.Merge(tmp);
        gsgs_Config().WriteItem(&m_tagsOptionsData);

        // start ctags process
        gsgs_Path().SetLauncherPath(gsgs_Path().GetBinaryFullPath("gsgs_launcher"));
        TagsManagerST::Get()->SetIndexerPath(gsgs_Path().GetBinaryFullPath("gsgs_indexer"));
        TagsManagerST::Get()->StartIndexer();

        //--------------------------------------------------------------------------------------
        // Start the parsing thread, the parsing thread and the SymbolTree (or its derived)
        // Are connected. The constructor of SymbolTree, calls ParseThreadST::Get()->SetNotifyWindow(this)
        // to allows it to receive events for gui changes.
        //
        // If you wish to connect another object for it, simply call ParseThreadST::Get()->SetNotifyWindow(this)
        // with another object as 'this'
        //--------------------------------------------------------------------------------------

        // Update the parser thread search paths
        ParseThreadST::Get()->SetCrawlerEnabeld(m_tagsOptionsData.GetParserEnabled());
        ParseThreadST::Get()->SetSearchPaths(m_tagsOptionsData.GetParserSearchPaths(), m_tagsOptionsData.GetParserExcludePaths());

        ParseThreadST::Get()->Start();

        // Connect this tree to the parse thread
        ParseThreadST::Get()->SetNotifyWindow(this);

        // update ctags options
        TagsManagerST::Get()->SetCtagsOptions(m_tagsOptionsData);

        // set this frame as the event handler for any events
        TagsManagerST::Get()->SetEvtHandler(this);

        // We must do Layout() before loading the toolbars, otherwise they're broken in >=wxGTK-2.9
        Layout();
        // Create the toolbars
        // If we requested to create a single toolbar, create a native toolbar
        // otherwise, we create a multiple toolbars using wxAUI toolbar if possible
        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        if(options) 
        {
            CreateToolBar(options->GetIconsSize());
            Bind(wxEVT_TOOL_DROPDOWN, &Frame::OnNativeTBUnRedoDropdown, this, wxID_UNDO, wxID_REDO);
        } 
        else 
        {
            CreateToolBar(16);
        }

        // Connect the custom build target events range: !USE_AUI_TOOLBAR only
        if(GetMainToolBar()) 
        {
            GetMainToolBar()->Connect(ID_MENU_CUSTOM_TARGET_FIRST, ID_MENU_CUSTOM_TARGET_MAX, wxEVT_COMMAND_MENU_SELECTED,
                wxCommandEventHandler(Frame::OnBuildCustomTarget), NULL, this);
        }

        Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(Frame::OnChangeActiveBookmarkType), this,
             XRCID("BookmarkTypes[start]"), XRCID("BookmarkTypes[end]"));

        GetWorkspacePane()->GetNotebook()->SetMenu(wxXmlResource::Get()->LoadMenu(wxT("workspace_view_rmenu")));
        GetDebuggerPane()->GetNotebook()->SetMenu(wxXmlResource::Get()->LoadMenu(wxT("debugger_view_rmenu")));
        GetOutputPane()->GetNotebook()->SetMenu(wxXmlResource::Get()->LoadMenu(wxT("outputview_view_rmenu")));
        GetPerspectivePane()->GetNotebook()->SetMenu(wxXmlResource::Get()->LoadMenu(wxT("workspace_view_rmenu")));
        mWinMag->UpdateAccels();

        // try to locate the build tools
        if(gsgs_EditorConfig().GetInteger(wxT("FixBuildToolOnStartup"), 1))
            UpdateBuildTools();

#ifdef __WXGTK__
        //SendSizeEvent(wxSEND_EVENT_POST);
#endif
        // Ditto the workspace pane auinotebook
        //GetWorkspacePane()->SendSizeEvent(wxSEND_EVENT_POST);

        SetDropTarget(new FileDropTarget());

        GetMainBook()->createPresent();
    }
    //-----------------------------------------------------------------
    void Frame::DoShowToolbars(bool show, bool)
    {
    #ifdef __WXMSW__
        wxWindowUpdateLocker locker(this);
    #endif
        mToolBar->Show(show);
        Layout();
        mWinMag->Update();
    }
    //-----------------------------------------------------------------
    void Frame::DoShowStatusBar(bool show, bool)
    {
#ifdef __WXMSW__
        wxWindowUpdateLocker locker(this);
#endif
        mStatusBar->Show(show);
        SendSizeEvent();
        Layout();
    }
    //-----------------------------------------------------------------
    void Frame::OnEditMenuOpened(wxMenuEvent& event)
    {
        event.Skip();
        Editor * editor = GetMainBook()->GetActiveEditor(true);
        wxMenuItem* labelCurrentState = event.GetMenu()->FindChildItem(XRCID("label_current_state"));
        if(labelCurrentState) { // Here seems to be the only reliable place to do 'updateui' for this; a real UpdateUI
                                // handler is only hit when there's no editor :/
            labelCurrentState->Enable(editor != NULL);
        } else {
            // In wx3.1 Bind()ing wxEVT_MENU_OPEN for the Edit menu catches its submenu opens too, so we arrive here
            // multiple times
            return;
        }

        if(editor) 
        {
            editor->GetCommandsProcessor().PrepareLabelledStatesMenu(event.GetMenu());
        } 
        else 
        {
            // There's no active editor, so remove any stale submenu; otherwise it'll display but the contents won't work
            wxMenuItem* menuitem = event.GetMenu()->FindChildItem(XRCID("goto_labelled_state"));
            if(menuitem) { event.GetMenu()->Delete(menuitem); }
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnNativeTBUnRedoDropdown(wxCommandEvent& event)
    {
        Editor* editor = GetMainBook()->GetActiveEditor(true);
        if(editor && GetMainToolBar()) 
        {
            bool undoing = event.GetId() == wxID_UNDO;
            wxMenu* menu = new wxMenu;
            editor->GetCommandsProcessor().DoPopulateUnRedoMenu(*menu, undoing);
            if(!menu->GetMenuItemCount()) 
            {
                delete menu;
                return;
            }

            if(undoing) 
            {
                menu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnUndoDropdownItem),
                           &editor->GetCommandsProcessor());
            } 
            else 
            {
                menu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(CommandProcessorBase::OnRedoDropdownItem),
                           &editor->GetCommandsProcessor());
            }
            GetMainToolBar()->SetDropdownMenu(event.GetId(), menu);

            event.Skip();
        }
        // Don't skip if there's no active editor/toolbar, otherwise a stale menu will show
    }
    //-----------------------------------------------------------------
    void Frame::CreateToolBar(int toolSize)
    {
        if(mToolBar) 
        {
            GetSizer()->Detach(mToolBar);
            wxDELETE(mToolBar);
        }
        int temp;
        gsgs_Config().Read(kConfigToolbarGroupSpacing, temp, 50);
        mToolBar = new ToolBar(this, wxID_ANY);
        mToolBar->SetGroupSpacing(temp);
        mToolBar->SetMiniToolBar(true); // We want main toolbar
        mToolBar->EnableCustomisation(true);
        ImageManager * bmpLoader = &gsgs_Image();

        mToolBar->AddToolInter(wxID_BACKWARD, _("Backward"),        bmpLoader->LoadBitmap(wxT("back"), toolSize), _("Backward"));
        mToolBar->AddToolInter(wxID_FORWARD, _("Forward"),          bmpLoader->LoadBitmap(wxT("forward"), toolSize), _("Forward"));
        mToolBar->AddSpacer();
        mToolBar->AddToolInter(XRCID("new_file"),   _("New"),       bmpLoader->LoadBitmap(wxT("file_new"), toolSize), _("New File"));
        mToolBar->AddToolInter(XRCID("open_file"),  _("Open"),      bmpLoader->LoadBitmap(wxT("file_open"), toolSize), _("Open File"));
        /*mToolBar->AddToolInter(XRCID("refresh_file"), _("Reload"),bmpLoader->LoadBitmap(wxT("file_reload"), toolSize), _("Reload File"));*/
        mToolBar->AddToolInter(XRCID("save_file"),  _("Save"),      bmpLoader->LoadBitmap(wxT("file_save"), toolSize), _("Save"));
        mToolBar->AddToolInter(XRCID("save_all"),   _("Save All"),  bmpLoader->LoadBitmap(wxT("file_save_all"), toolSize), _("Save All"));
        /*mToolBar->AddToolInter(XRCID("close_file"), _("Close"),   bmpLoader->LoadBitmap(wxT("execute_stop"), toolSize), _("Close File"));*/
        mToolBar->AddSpacer();
        mToolBar->AddToolInter(wxID_CUT,    _("Cut"),               bmpLoader->LoadBitmap(wxT("cut"), toolSize), _("Cut"));
        mToolBar->AddToolInter(wxID_COPY,   _("Copy"),              bmpLoader->LoadBitmap(wxT("copy"), toolSize), _("Copy"));
        mToolBar->AddToolInter(wxID_PASTE,  _("Paste"),             bmpLoader->LoadBitmap(wxT("paste"), toolSize), _("Paste"));
        mToolBar->AddToolInter(wxID_UNDO,   _("Undo"),              bmpLoader->LoadBitmap(wxT("undo"), toolSize), _("Undo"), wxITEM_DROPDOWN);
        mToolBar->AddToolInter(wxID_REDO,   _("Redo"),              bmpLoader->LoadBitmap(wxT("redo"), toolSize), _("Redo"), wxITEM_DROPDOWN);
        mToolBar->AddSpacer();

        //----------------------------------------------
        // create the search toolbar
        //----------------------------------------------
        mToolBar->AddToolInter(wxID_FIND, _("Find"), bmpLoader->LoadBitmap(wxT("find"), toolSize), _("Find"));
        mToolBar->AddToolInter(wxID_REPLACE, _("Replace"), bmpLoader->LoadBitmap(wxT("find_and_replace"), toolSize), _("Replace"));
        /*mToolBar->AddToolInter(XRCID("find_in_files"), _("Find In Files"), bmpLoader->LoadBitmap(wxT("find_in_files"), toolSize), _("Find In Files"));
        mToolBar->AddToolInter(XRCID("find_resource"), _("Find Resource In Workspace"),
                           bmpLoader->LoadBitmap(wxT("open_resource"), toolSize), _("Find Resource In Workspace"));
        mToolBar->AddToolInter(XRCID("highlight_word"), _("Highlight Word"), bmpLoader->LoadBitmap(wxT("mark_word"), toolSize),
                           _("Highlight Matching Words"), wxITEM_CHECK);
        mToolBar->ToggleTool(XRCID("highlight_word"), m_highlightWord);
        mToolBar->AddToolInter(XRCID("toggle_bookmark"), _("Toggle Bookmark"), bmpLoader->LoadBitmap(wxT("bookmark"), toolSize),
            _("Toggle Bookmark"), wxITEM_DROPDOWN);
        */
        mToolBar->AddToolInter(XRCID("toggle_bookmark"), _("Toggle Bookmark"), bmpLoader->LoadBitmap(wxT("mark_word"), toolSize),
            _("Toggle Bookmark"), wxITEM_DROPDOWN);
        mToolBar->SetDropdownMenu(XRCID("toggle_bookmark"), gsgs_Bookmark().CreateBookmarksSubmenu(NULL));
        mToolBar->AddSpacer();

        //----------------------------------------------
        // create the build toolbar
        //----------------------------------------------
        
        mToolBar->AddToolInter(XRCID("build_active_project"), _("Build Active Project"),
                           bmpLoader->LoadBitmap(wxT("build"), toolSize), _("Build Active Project"), wxITEM_DROPDOWN);
        mToolBar->AddToolInter(XRCID("stop_active_project_build"), _("Stop Current Build"),
                           bmpLoader->LoadBitmap(wxT("execute_stop"), toolSize), _("Stop Current Build"));
        mToolBar->AddToolInter(XRCID("clean_active_project"), _("Clean Active Project"),
                           bmpLoader->LoadBitmap(wxT("clear"), toolSize), _("Clean Active Project"));
        mToolBar->AddSeparator();

        mToolBar->AddControl(mConfigPanel);
        mToolBar->AddToolInter(XRCID("execute_no_debug"), _("Run Active Project"),
                           bmpLoader->LoadBitmap(wxT("execute"), toolSize), _("Run Active Project"));
        mToolBar->AddToolInter(XRCID("start_debugger"), _("Start or Continue debugger"),
            bmpLoader->LoadBitmap(wxT("start-debugger"), toolSize), _("Start or Continue debugger"));
        mToolBar->AddToolInter(XRCID("stop_executed_program"), _("Stop Running Program"),
                           bmpLoader->LoadBitmap(wxT("execute_stop"), toolSize), _("Stop Running Program"));

        GetSizer()->Insert(1, mToolBar, 0, wxEXPAND);
        mToolBar->Realize();
        mToolBar->Bind(wxEVT_TOOLBAR_CUSTOMISE, &Frame::OnCustomiseToolbar, this);
    }
    //-----------------------------------------------------------------
    bool Frame::StartSetupWizard(int page)
    {
        BootstrapWizard wiz(this, page);
        if(wiz.RunWizard(wiz.GetPage(page)))
        {
            {
                wxString message;
                if(wiz.IsRestartRequired()) 
                {
                    message << _("Applying and restarting gsgs");
                } 
                else 
                {
                    message << _("Applying, Take a few seconds");
                }

                BusyInfo bi(message);

                BootstrapData data = wiz.GetData();

                // update the compilers if not empty
                if(!data.compilers.empty()) 
                {
                    gsgs_Settings().SetCompilers(data.compilers);
                    CallAfter(&Frame::UpdateParserSearchPathsFromDefaultCompiler);
                }
                OptionsConfig * options = gsgs_EditorConfig().GetOptions();
                options->SetIndentUsesTabs(data.useTabs);
                options->SetShowWhitspaces(data.whitespaceVisibility);
                gsgs_EditorConfig().SetOptions(options);

                // Update the theme
                //gsgs_Style().SetTheme(data.selectedTheme);
                gsgs_Style().Save();
            }

            if(wiz.IsRestartRequired()) 
            {
                // Don't annoy the user by showing the 'Save Perspective' dialog,
                // especially as he hasn't yet had a chance to set it!
                // Also, the dialog would probably get hidden behind the new CL instance
                SetNoSavePerspectivePrompt(true);

                Event restartEvent(wxEVT_RESTART_GSGS);
                gsgs_Manager()->AddPendingEvent(restartEvent);
                return true;
            }
        }
        return false;
    }
    //-----------------------------------------------------------------
    void Frame::Bootstrap()
    {
        bool tempb;
        gsgs_Config().Read(kConfigBootstrapCompleted, tempb, false);
        if(!tempb)
        {
            gsgs_Config().Write(kConfigBootstrapCompleted, true);
            if(StartSetupWizard()) 
            {
                gsgs_Event().PostCommandEvent(wxEVT_INIT_DONE, NULL);
                return;
            }
        }

        // Load the session manager
        wxString sessConfFile;
        sessConfFile << gsgs_Path().GetUserDataDir() << wxT("/config/sessions.xml");
        gsgs_Session().Load(sessConfFile);

        gsgs_Event().PostCommandEvent(wxEVT_INIT_DONE, NULL);

        // restore last session if needed
        /*gsgs_Config().Read(kConfigRestoreLastSession, tempb, true);
        if(tempb && mRecover) 
        {
            wxCommandEvent loadSessionEvent(wxEVT_LOAD_SESSION);
            gsgs_Event().AddPendingEvent(loadSessionEvent);
        }*/
    }
    //-----------------------------------------------------------------
    void Frame::UpdateBuildTools() {}
    //-----------------------------------------------------------------
    void Frame::OnQuit(wxCommandEvent&) 
    { 
        Close(); 
    }
    //-----------------------------------------------------------------
    void Frame::OnMaximize(wxCommandEvent&)
    {
        static bool temp = true;
        if (temp)
            temp = false;
        else
            temp = true;
        Maximize(temp);
        wxWindowUpdateLocker lltk(this);
        Layout();
    }
    //-----------------------------------------------------------------
    void Frame::OnMinimize(wxCommandEvent&) 
    { 
        Iconize();
    }
    //-----------------------------------------------------------------
    void Frame::OnTBUnRedoMenu(wxCommandEvent& event)
    {
        Editor* editor = GetMainBook()->GetActiveEditor(true);
        if(editor) {
            editor->GetCommandsProcessor().ProcessEvent(event);
        } else if(GetMainBook()->GetCurrentPage()) {
            event.StopPropagation(); // Otherwise we'll infinitely loop if the active page doesn't handle this event
            GetMainBook()->GetCurrentPage()->GetEventHandler()->ProcessEvent(event); // Let the active plugin have a go
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnTBUnRedo(wxCommandEvent& event)
    {
        DispatchCommandEvent(event); // Revert to standard processing
    }
    //-----------------------------------------------------------------
    bool Frame::IsEditorEvent(wxEvent& event)
    {
        // If the event came from the toolbar, return true
        // if(dynamic_cast<ToolBar*>(event.GetEventObject())) { return true; }

    #ifdef __WXGTK__
        Book* mainBook = GetMainBook();
        if(!mainBook || !mainBook->GetActiveEditor(true)) {
            if(event.GetId() == wxID_FIND)
                return true;
            else
                return false;
        }

        switch(event.GetId()) {
        case wxID_CUT:
        case wxID_SELECTALL:
        case wxID_COPY:
        case wxID_PASTE:
        case wxID_UNDO:
        case wxID_REDO: {
            bool isFocused;
            //Editor* editor = dynamic_cast<Editor*>(event.GetEventObject());
            EditorScrolledPanel* esp = dynamic_cast<EditorScrolledPanel*>(event.GetEventObject());
            if(esp) {
                isFocused = true;

            } else {
                isFocused = mainBook->GetActiveEditor(true)->IsFocused();
            }
            return isFocused;
        }
        default:
            break;
        }

    #else

        // Handle common edit events
        // if the focused window is *not* Editor,
        // and the focused windows is of type
        // wxTextCtrl or wxScintilla, let the focused
        // Window handle the event
        wxWindow* focusWin = wxWindow::FindFocus();
        if(focusWin) 
        {
            switch(event.GetId()) 
            {
            case wxID_CUT:
            case wxID_SELECTALL:
            case wxID_COPY:
            case wxID_PASTE:
            case wxID_UNDO:
            case wxID_REDO: 
            {
                //Editor* ed = dynamic_cast<Editor*>(focusWin);
                EditorScrolledPanel * esp = dynamic_cast<EditorScrolledPanel*>(focusWin);
                if(!esp) {
                    // let other controls handle it
                    return false;
                }
                break;
            }
            default:
                break;
            }
        }
    #endif
        return true;
    }
    //-----------------------------------------------------------------
    void Frame::DispatchCommandEvent(wxCommandEvent& event)
    {
        if (IsEditorEvent(event)) {
            Editor* editor = GetMainBook()->GetActiveEditor(true);
            if (editor) {
                editor->OnMenuCommand(event);
            }
            else if (event.GetId() == wxID_FIND) {
                // Let the plugins handle this
                GetMainBook()->ShowQuickBarForPlugins();
            }
            return;
        }
        event.Skip();
    }
    //-----------------------------------------------------------------
    void Frame::DispatchUpdateUIEvent(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();

        if (IsEditorEvent(event)) 
        {
            Editor* editor = GetMainBook()->GetActiveEditor(true);
            if (editor) {

                if (event.GetId() >= viewAsMenuItemID && event.GetId() <= viewAsMenuItemMaxID) {
                    // keep the old id as int and override the value set in the event object
                    // to trick the event system
                    event.SetInt(event.GetId());
                    event.SetId(viewAsMenuItemID);
                }
                editor->OnUpdateUI(event);
                
            }
            else
            {
                event.Enable(false);
            }
        }
        else
        {
            event.Enable(false);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnFileExistUpdateUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        event.Enable(GetMainBook()->GetActiveEditor(true) != NULL);
    }
    //-----------------------------------------------------------------
    void Frame::OnAbout(wxCommandEvent&)
    {
    /*#ifdef __WXGTK__
        // Build the about info
        wxAboutDialogInfo info;
        // Misc
        info.SetWebSite("http://www.niiengine.com", _("光栅故事 Home"));
        info.SetVersion(GSGS_VERSION);
        info.SetCopyright("Shenzhen Mindeng Technology Co., Ltd. 2018-2021");

        // Load the license file
        wxFileName license(gsgs_Path().GetDataDir(), "LICENSE");
        wxString fileContent;
        FileUtil::ReadFileContent(license, fileContent);
        info.SetLicence(fileContent);
        info.SetName(_("光栅故事"));
        info.SetDescription(_("三维创意商业平台,工业互联网软件"));

        const wxBitmap & iconBmp = gsgs_Image().LoadBitmap("gsgs-logo", 64);
        if(iconBmp.IsOk()) {
            wxIcon icn;
            icn.CopyFromBitmap(iconBmp);
            info.SetIcon(icn);
        }
        wxAboutBox(info, this);
    #else*/
        AboutDialog dialog(this, GSGS_VERSION);
        dialog.ShowModal();
    //#endif
    }
    //-----------------------------------------------------------------
    void Frame::OnClose(wxCloseEvent& event)
    {
        // Prompt before exit, but not if we're coming from the Setup Wizard
        if(!GetAndResetNoSavePerspectivePrompt()) 
        {
            wxStandardID ans = PromptMessageDialog(_("Closing 光栅故事\n\nSave workbench and exit?"), 
                "SaveAndExit", "Save&Exit", "Exit", "Cancel");
            if(ans == wxID_CANCEL) 
            {
                event.Veto();
                event.Skip(false);
                return;
            }

            if(ans == wxID_YES) 
            {
                if(!SaveLayoutAndSession()) 
                {
                    event.Veto();
                    event.Skip(false);
                    return;
                }
            }
        }
        Show(false);
        SaveGeneralSettings();

        event.Skip();

        wxString msg;
        gsgs_Manager()->SetShutdownInProgress(true);

        // Notify the plugins that we are going down
        Event eventGoingDown(wxEVT_GOING_DOWN);
        gsgs_Event().ProcessEvent(eventGoingDown);

        // Stop the retag thread
        ParseThreadST::Get()->Stop();

        // Stop the search thread
        gsgs_Manager()->KillProgram();
        SearchThreadST::Get()->StopSearch();

        // Stop any debugging session if any
        IDebugger* debugger = DebuggerMgr::Get().GetActiveDebugger();
        if(debugger && debugger->IsRunning()) 
            gsgs_Manager()->DbgStop();

        // In case we got some data in the clipboard, flush it so it will be available
        // after our process exits
        wxTheClipboard->Flush();
    }
    //-----------------------------------------------------------------
    void Frame::LoadSession(const wxString& sessionName)
    {
        SessionEntry session;
        if(gsgs_Session().GetSession(sessionName, session)) 
        {
            wxString wspFile = session.GetWorkspaceName();
            if(wspFile.IsEmpty() == false && wspFile != wxT("Default")) 
            {

                // Load the workspace only if it exists
                wxFileName fnWorkspace(wspFile);

                if(fnWorkspace.Exists()) {
                    wxCommandEvent eventOpenWorkspace;
                    eventOpenWorkspace.SetString(fnWorkspace.GetFullPath());
                    OnSwitchWorkspace(eventOpenWorkspace);
                }
            } else {
                // no workspace to open, so just restore any previously open editors
                GetMainBook()->RestoreSession(session);
            }
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnSave(wxCommandEvent&)
    {
        Editor* editor = GetMainBook()->GetActiveEditor(true);
        if(editor) {
            editor->SaveFile();
        } else {
            // delegate it to the plugins
            wxCommandEvent saveEvent(XRCID("save_file"));
            gsgs_Event().ProcessEvent(saveEvent);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnSaveAs(wxCommandEvent&)
    {
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor) { editor->SaveFileAs(); }
    }
    //-----------------------------------------------------------------
    void Frame::OnFileLoadTabGroup(wxCommandEvent&)
    {
        wxArrayString previousgroups;
        gsgs_EditorConfig().GetRecentItems(previousgroups, wxT("RecentTabgroups"));

        // Check the previous items still exist
        for(int n = (int)previousgroups.GetCount() - 1; n >= 0; --n) 
        {
            if(!wxFileName::FileExists(previousgroups.Item(n))) { previousgroups.RemoveAt(n); }
        }
        gsgs_EditorConfig().SetRecentItems(previousgroups, wxT("RecentTabgroups")); // In case any were deleted

        wxString path = gsgs_Workspace().IsOpen() ? gsgs_CppWorker().GetFileName().GetPath()
                                                            : wxGetHomeDir();
        LoadTabGroupDlg dlg(this, path, previousgroups);

        // Disable the 'Replace' checkbox if there aren't any editors to replace
        std::vector<Editor*> editors;
        GetMainBook()->GetAllEditors(editors, Book::kGetAll_Default);
        dlg.EnableReplaceCheck(editors.size());

        if(dlg.ShowModal() != wxID_OK) 
        { 
            return; 
        }

        wxString filepath = dlg.GetListBox()->GetStringSelection();
        wxString sessionFilepath = filepath.BeforeLast(wxT('.'));

        WindowUpdateLocker locker(this);
        TabGroupEntry session;
        if(gsgs_Session().GetSession(sessionFilepath, session, "tabgroup", tabgroupTag)) 
        {
            // We've 'loaded' the requested tabs. If required, delete any current ones
            if(dlg.GetReplaceCheck()) { GetMainBook()->CloseAll(true); }
            GetMainBook()->RestoreSession(session);

            // Remove any previous instance of this group from the history, then prepend it and save
            int index = previousgroups.Index(filepath);
            if(index != wxNOT_FOUND) { previousgroups.RemoveAt(index); }
            previousgroups.Insert(filepath, 0);
            gsgs_EditorConfig().SetRecentItems(previousgroups, wxT("RecentTabgroups"));
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnFileReload(wxCommandEvent&)
    {
        Editor* editor = GetMainBook()->GetActiveEditor(true);
        if(editor) {
            if(editor->GetModify()) {
                // Ask user if he really wants to lose all changes
                wxString msg;
                msg << editor->GetFileName().GetFullName() << _(" has been modified, reload file anyway?");
                wxRichMessageDialog dlg(::wxGetTopLevelParent(editor), msg, _("Reload File"),
                                        wxYES_NO | wxCANCEL | wxNO_DEFAULT | wxICON_WARNING);
                if(dlg.ShowModal() != wxID_YES) { return; }
            }
            editor->ReloadFromDisk(true);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnCloseWorkspace(wxCommandEvent&)
    {
        wxBusyCursor bc;
        // let the plugins close any custom workspace
        Event e(wxEVT_CMD_CLOSE_WORKSPACE, GetId());
        e.SetEventObject(this);
        gsgs_Event().ProcessEvent(e);

        // In any case, make sure that we dont have a workspace opened
        if(gsgs_Workspace().IsOpen())
            gsgs_Manager()->CloseWorkspace();
    }
    //-----------------------------------------------------------------
    void Frame::OnSwitchWorkspace(wxCommandEvent& event)
    {
        wxBusyCursor bc;
        wxString wspFile;
        // Is it an internal command? (usually the workspace file name to load is set in the
        if(event.GetString().IsEmpty()) 
        {
            // now it is time to prompt user for new workspace to open
            const wxString ALL(wxT("GSGS Workspace files (*.gsgsws)|*.gsgsws|") wxT("All Files (*)|*"));
            wxFileDialog dlg(this, _("Open Workspace"), wxEmptyString, wxEmptyString, ALL,
                             wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE, wxDefaultPosition);
            if(dlg.ShowModal() == wxID_OK) { wspFile = dlg.GetPath(); }
        } 
        else 
        {
            wspFile = event.GetString();
        }

        if(wspFile.IsEmpty()) 
            return;

        /*// Let the plugins a chance of handling this workspace first
        Event e(wxEVT_CMD_OPEN_WORKSPACE, GetId());
        e.SetEventObject(this);
        e.SetFileName(wspFile);
        if(gsgs_Event().ProcessEvent(e)) 
        {
            // the plugin processed it by itself
            return;
        }

        // Make sure that the 'Workspace' tab is visible
        wxCommandEvent showTabEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("show_workspace_tab"));
        showTabEvent.SetEventObject(this);
        showTabEvent.SetInt(1);
        GetEventHandler()->ProcessEvent(showTabEvent);*/

        // Open the workspace
        gsgs_Manager()->OpenWorkspace(wspFile);
    }
    //-----------------------------------------------------------------
    void Frame::OnCompleteWordRefreshList(wxCommandEvent&) 
    { 
    }
    //-----------------------------------------------------------------
    void Frame::OnCodeComplete(wxCommandEvent&)
    {
        Editor* editor = GetMainBook()->GetActiveEditor(true);
        if(editor) 
        { 
            editor->CompleteWord(LSP::CompletionItem::kTriggerUser); 
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnFunctionCalltip(wxCommandEvent&)
    {
        Editor* editor = GetMainBook()->GetActiveEditor(true);
        if(editor) 
        { 
            editor->ShowFunctionTipFromCurrentPos(); 
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnFileNew(wxCommandEvent&)
    {
        GetMainBook()->NewEditor();
    }
    //-----------------------------------------------------------------
    void Frame::OnFileOpen(wxCommandEvent&)
    {
        const wxString ALL(wxT("All Files (*)|*"));

        // Open a file using the current editor's path
        Editor* editor = GetMainBook()->GetActiveEditor();
        wxString open_path;

        if(editor) 
        {
            open_path = editor->GetFileName().GetPath();
        } 
        else 
        {
            // Could not locate the active editor, use the project
            ProjectPtr project = gsgs_Manager()->GetProject(gsgs_Manager()->GetActiveProjectName());
            if(project) { open_path = project->GetFileName().GetPath(); }
        }

        wxFileDialog* dlg = new wxFileDialog(this, _("Open File"), open_path, wxEmptyString, ALL,
                                             wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_MULTIPLE, wxDefaultPosition);
        if(dlg->ShowModal() == wxID_OK) 
        {
            wxArrayString paths;
            dlg->GetPaths(paths);
            for(size_t i = 0; i < paths.GetCount(); i++) 
            {
                GetMainBook()->OpenFile(paths.Item(i));
            }
        }
        dlg->Destroy();
    }
    //-----------------------------------------------------------------
    void Frame::OnFileClose(wxCommandEvent&)
    {
        wxWindow* winToClose = GetMainBook()->GetCurrentPage();
        if(winToClose)
        {
            GetMainBook()->CallAfter(&Book::ClosePageVoid, winToClose);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnFileSaveAll(wxCommandEvent& )
    {
        GetMainBook()->SaveAll(false, true);
    }
    //-----------------------------------------------------------------
    void Frame::OnFileSaveTabGroup(wxCommandEvent&)
    {
        wxArrayString previousgroups;
        gsgs_EditorConfig().GetRecentItems(previousgroups, wxT("RecentTabgroups"));

        SaveTabGroupDlg dlg(this, previousgroups);

        std::vector<Editor*> editors;
        wxArrayString filepaths;
        GetMainBook()->GetAllEditors(editors, Book::kGetAll_RetainOrder |
                                                  Book::kGetAll_IncludeDetached); // We'll want the order of intArr
                                                                                      // to match the order in
                                                                                      // Book::SaveSession
        for(size_t i = 0; i < editors.size(); ++i) 
        {
            filepaths.Add(editors[i]->GetFileName().GetFullPath());
        }
        dlg.SetListTabs(filepaths);

        while(true) 
        {
            if(dlg.ShowModal() != wxID_OK) { return; }
            wxString sessionName = dlg.GetTabgroupName();
            if(sessionName.IsEmpty()) {
                if(wxMessageBox(_("Please enter a name for the tab group"), _("光栅故事"), wxICON_ERROR | wxOK | wxCANCEL, this) != wxOK) 
                {
                    return;
                } else {
                    continue;
                }
            }

            wxString path = dlg.GetSaveInWorkspace() ? TabGroupsManager::Get()->GetTabgroupDirectory()
                                                     : gsgs_Path().GetUserDataDir() + "/tabgroups";

            if(path.Right(1) != gsgsPathSep) { path << gsgsPathSep; }
            wxString filepath(path + sessionName + wxT(".tabgroup"));
            if(wxFileName::FileExists(filepath)) {
                if(wxMessageBox(_("There is already a file with this name. Do you want to overwrite it?"),
                                _("Are you sure?"), wxICON_EXCLAMATION | wxOK | wxCANCEL, this) != wxOK) {
                    return;
                }
            }

            wxArrayInt intArr;
            if(dlg.GetChoices(intArr)) { // Don't bother to save if no tabs were selected
                TabGroupEntry session;
                session.SetTabgroupName(path + sessionName);
                GetMainBook()->SaveSession(session, &intArr);
                gsgs_Session().Save(session.GetTabgroupName(), session, "tabgroup", tabgroupTag);
                // Add the new tabgroup to the tabgroup manager and pane
                GetWorkspacePane()->GetServiceTab()->AddNewTabgroupToTree(!dlg.GetSaveInWorkspace(), filepath);

                // Remove any previous instance of this group from the history, then prepend it and save
                int index = previousgroups.Index(filepath);
                if(index != wxNOT_FOUND) { previousgroups.RemoveAt(index); }
                previousgroups.Insert(filepath, 0);
                gsgs_EditorConfig().SetRecentItems(previousgroups, wxT("RecentTabgroups"));
                GetMainStatusBar()->SetMessage(_("Tab group saved"));
            }

            return;
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnCompleteWordUpdateUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        // This menu item is enabled only if the current editor belongs to a project
        event.Enable(GetMainBook()->GetActiveEditor(true));
    }
    //-----------------------------------------------------------------
    void Frame::OnWorkspaceOpen(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        event.Enable(gsgs_Workspace().GetWorkspace());
    #if 0
        Event e(wxEVT_CMD_IS_WORKSPACE_OPEN, GetId());
        e.SetEventObject(this);
        e.SetAnswer(false);
        gsgs_Event().ProcessEvent(e);

        event.Enable(gsgs_Manager()->IsOpen() || e.IsAnswer());
    #endif
    }
    //-----------------------------------------------------------------
    // Project->New Workspace
    void Frame::OnProjectNewWorkspace(wxCommandEvent & event)
    {
        // let the plugins handle this event first
        SelectIndexLinkButton * eobj = dynamic_cast<SelectIndexLinkButton *>(event.GetEventObject());
        int ssi = 0;
        if (eobj)
        {
            ssi = eobj->GetSelectIndex();
        }
        CreateWorkspaceDialog dlg(this);
        dlg.SetNotebookSelection(ssi);
        dlg.SetLabel(_("Create workspace:"));
        dlg.ShowModal();

        if (dlg.isUnChoice())
        {
            // a pluing workspace, pass it to the plugins
            Event e(wxEVT_CMD_CREATE_NEW_WORKSPACE);
            e.SetEventObject(this);
            //e.SetString(selection);
            gsgs_Event().AddPendingEvent(e);
        }
    }
    //-----------------------------------------------------------------
    // Project->New Project
    void Frame::OnProjectNewProject(wxCommandEvent&)
    {
        // Let the plugin process this request first
        gsgs_Manager()->ShowNewProjectWizard();
    }
    //-----------------------------------------------------------------
    void Frame::OnProjectAddProject(wxCommandEvent&)
    {
        // Prompt user for project path
        const wxString ALL(wxT("gsgs Projects (*.project)|*.project|") wxT("All Files (*)|*"));
        wxFileDialog* dlg = new wxFileDialog(this, _("Open Project"), wxEmptyString, wxEmptyString, ALL,
                                             wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);
        if(dlg->ShowModal() == wxID_OK) {
            // Open it
            gsgs_Manager()->AddProject(dlg->GetPath());
        }
        dlg->Destroy();
    }
    //-----------------------------------------------------------------
    void Frame::OnReconcileProject(wxCommandEvent&)
    {
        gsgs_Manager()->ReconcileProject();
    }
    //-----------------------------------------------------------------
    void Frame::OnCtagsOptions(wxCommandEvent&)
    {
        bool colVars(false);
        bool newColVars(false);
        bool caseSensitive(false);

        size_t colourTypes(0);

        colVars = (m_tagsOptionsData.GetFlags() & CC_COLOUR_VARS ? true : false);
        colourTypes = m_tagsOptionsData.GetCcColourFlags();

        wxArrayString pathsBefore = m_tagsOptionsData.GetParserSearchPaths();
        CodeCompletionSettingsDialog dlg(this, m_tagsOptionsData);
        if(dlg.ShowModal() == wxID_OK) {
            m_tagsOptionsData = dlg.GetData();

            // writes the content into the ctags.replacements file (used by
            // gsgs_indexer)
            m_tagsOptionsData.SyncData();

            wxArrayString pathsAfter = m_tagsOptionsData.GetParserSearchPaths();
            wxArrayString removedPaths;

            // Compare the paths
            for(size_t i = 0; i < pathsBefore.GetCount(); i++) {
                int where = pathsAfter.Index(pathsBefore.Item(i));
                if(where == wxNOT_FOUND) {
                    removedPaths.Add(pathsBefore.Item(i));
                } else {
                    pathsAfter.RemoveAt((size_t)where);
                }
            }

            if(removedPaths.IsEmpty() == false) {
                wxWindowDisabler disableAll;
                wxBusyInfo info(_T("Updating tags database, please wait..."), this);
                wxTheApp->Yield();

                // Remove all tags from the database which starts with the paths which were
                // removed from the parser include path
                ITagsStoragePtr db = TagsManagerST::Get()->GetDatabase();
                db->Begin();
                for(size_t i = 0; i < removedPaths.GetCount(); i++) {
                    db->DeleteByFilePrefix(wxFileName(), removedPaths.Item(i));
                    db->DeleteFromFilesByPrefix(wxFileName(), removedPaths.Item(i));
                    wxTheApp->Yield();
                }
                db->Commit();
            }

            newColVars = (m_tagsOptionsData.GetFlags() & CC_COLOUR_VARS ? true : false);
            caseSensitive = (m_tagsOptionsData.GetFlags() & CC_IS_CASE_SENSITIVE);

            TagsManagerST::Get()->SetCtagsOptions(m_tagsOptionsData);
            TagsManagerST::Get()->GetDatabase()->SetEnableCaseInsensitive(!caseSensitive);

            gsgs_Config().WriteItem(&m_tagsOptionsData);

            // We use this method 'UpdateParserPaths' since it will also update the parser
            // thread with any workspace search/exclude paths related
            gsgs_Manager()->UpdateParserPaths(false);

            TagsManagerST::Get()->GetDatabase()->SetMaxWorkspaceTagToColour(m_tagsOptionsData.GetMaxItemToColour());

            // do we need to colourise?
            if((newColVars != colVars) || (colourTypes != m_tagsOptionsData.GetCcColourFlags())) {
                GetMainBook()->UpdateColours();
            }

            if(pathsAfter.IsEmpty() == false) 
            {
                // a retagg is needed
                wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
                AddPendingEvent(e);
            }
            // Update the pre-processor dimming feature
            CodeCompletionManager::Get().RefreshPreProcessorColouring();
        }
    }
    //-----------------------------------------------------------------
    int Frame::getFrameMark() const 
    { 
        return mFrameInfo->GetFlags(); 
    }
    //-----------------------------------------------------------------
    void Frame::OnToggleMainTBars(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        ToggleToolBars(true);
    }
    //-----------------------------------------------------------------
    void Frame::OnTogglePluginTBars(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        ToggleToolBars(false);
    }
    //-----------------------------------------------------------------
    void Frame::ToggleToolBars(bool std)
    {
        gsgsStringSet toolbars;
        {
            wxAuiPaneInfoArray& allPanes = mWinMag->GetAllPanes();
            for(size_t i = 0; i < allPanes.GetCount(); ++i) 
            {
                wxAuiPaneInfo& pane = allPanes.Item(i);
                if(!pane.IsOk() || !pane.IsToolbar()) 
                    continue;

                if(std) {
                    // collect core toolbars
                    if(m_coreToolbars.count(pane.name)) toolbars.insert(pane.name);
                } else {
                    // collect plugins toolbars
                    if(m_coreToolbars.count(pane.name) == 0) toolbars.insert(pane.name);
                }
            }
        }

        if(toolbars.empty()) return;

        // determine that state based on the first toolbar
        bool currentStateVisible = mWinMag->GetPane((*toolbars.begin())).IsShown();

        gsgsStringSet::iterator iter = toolbars.begin();
        for(; iter != toolbars.end(); ++iter) 
        {
            wxString name = *iter;
            wxAuiPaneInfo& pane = mWinMag->GetPane(name);
            pane.Show(!currentStateVisible);
        }
        mWinMag->UpdateAccels();
    }
    //-----------------------------------------------------------------
    void Frame::OnViewOptions(wxCommandEvent&)
    {
        PreferencesDialog dlg(this);
        dlg.ShowModal();

        if(dlg.isRestartRquired())
        { 
            mInfoBar->DisplayMessage(_("GSGS suggestion: restart, do it now?"), wxICON_QUESTION,
                { { XRCID("restart-gsgs"), _("Yes") }, { wxID_NO, _("No") } });
        }
        m_themeHandler.CallAfter(&ThemeHandler::UpdateNotebookColours, this);
    }
    //-----------------------------------------------------------------
    void Frame::OnTogglePanes(wxCommandEvent&)
    {
        gsgs_Manager()->TogglePanes();
    }
    //-----------------------------------------------------------------
    void Frame::OnAddEnvironmentVariable(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        EnvironmentVariablesDlg dlg(this);
        dlg.ShowModal();
    }
    //-----------------------------------------------------------------
    void Frame::OnAdvanceSettings(wxCommandEvent& event)
    {
        BuildSettingsDialog * dlg = new BuildSettingsDialog(this, 0);
        if(dlg->ShowModal() == wxID_OK) {
            // mark the whole workspace as dirty so makefile generation will take place
            // force makefile generation upon configuration change
            if(gsgs_Workspace().IsOpen()) {
                wxArrayString projs;
                gsgs_Manager()->GetProjectList(projs);
                for(size_t i = 0; i < projs.GetCount(); i++) {
                    ProjectPtr proj = gsgs_Manager()->GetProject(projs.Item(i));
                    if(proj) { proj->SetModified(true); }
                }
            }
        }
        SelectBestEnvSet();
        dlg->Destroy();
    }
    //-----------------------------------------------------------------
    void Frame::OnBuildEnded(Event& event)
    {
        event.Skip();

        if(m_buildAndRun) {
            // If the build process was part of a 'Build and Run' command, check whether an erros
            // occurred during build process, if non, launch the output
            m_buildAndRun = false;
            if(gsgs_Manager()->IsBuildEndedSuccessfully() ||
               wxMessageBox(_("Build ended with errors. Continue?"), _("Confirm"), wxYES_NO | wxICON_QUESTION, this) == wxYES) 
            {
                gsgs_Manager()->ExecuteNoDebug(gsgs_Manager()->GetActiveProjectName());
            }
        }
        // Process next command from the queue
        gsgs_Manager()->ProcessCommandQueue();
    }
    //-----------------------------------------------------------------
    // Build operations
    void Frame::OnBuildProject(wxCommandEvent& event)
    {
        wxUnusedVar(event);

        // Let the plugins handle this first
        BuildEvent buildEvent(wxEVT_BUILD_STARTING);
        buildEvent.SetKind("build");
        if(gsgs_Event().ProcessEvent(buildEvent)) { return; }

        bool enable = !gsgs_Manager()->IsBuildInProgress() && !gsgs_Manager()->GetActiveProjectName().IsEmpty();
        if(enable) {

            // Make sure that the working folder is set to the correct path
            wxString workspacePath = gsgs_CppWorker().GetFileName().GetPath();
            ::wxSetWorkingDirectory(workspacePath);
            gsgsDEBUG() << "Setting working directory to" << workspacePath;
            GetMainStatusBar()->SetMessage(_("Build starting..."));

            wxString conf, projectName;
            projectName = gsgs_Manager()->GetActiveProjectName();

            // get the selected configuration to be built
            BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
            if(bldConf) { conf = bldConf->GetName(); }

            QueueCommand info(projectName, conf, false, QueueCommand::kBuild);
            if(bldConf && bldConf->IsCustomBuild()) {
                info.SetKind(QueueCommand::kCustomBuild);
                info.SetCustomBuildTarget(wxT("Build"));
            }
            gsgs_Manager()->PushQueueCommand(info);
            gsgs_Manager()->ProcessCommandQueue();

            GetMainStatusBar()->SetMessage("");
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnBuildCustomTarget(wxCommandEvent& event)
    {
        bool enable = !gsgs_Manager()->IsBuildInProgress() && !gsgs_Manager()->GetActiveProjectName().IsEmpty();
        if(enable) {

            // get the selected configuration to be built
            BuildConfigPtr bldConf =
                gsgs_CppWorker().GetProjBuildConf(BuildSettingsConfig::singleton().GetProjectName(), wxEmptyString);
            if(bldConf) {
                BuildSettingsConfig::Pair_t target = BuildSettingsConfig::singleton().GetTarget(event.GetId());
                if(target.second.IsEmpty()) {
                    gsgsDEBUG() << "Failed to find Custom Build Target for event ID:" << event.GetId();
                    return;
                }
                QueueCommand info(BuildSettingsConfig::singleton().GetProjectName(), bldConf->GetName(), false,
                                  QueueCommand::kCustomBuild);
                info.SetCustomBuildTarget(target.first);

                gsgs_Manager()->PushQueueCommand(info);
                gsgs_Manager()->ProcessCommandQueue();
            }
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnBuildAndRunProject(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        bool enable = !gsgs_Manager()->IsBuildInProgress() && !gsgs_Manager()->GetActiveProjectName().IsEmpty();
        if(enable) {
            m_buildAndRun = true;

            wxString projectName = gsgs_Manager()->GetActiveProjectName();
            wxString conf;
            // get the selected configuration to be built
            BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
            if(bldConf) { conf = bldConf->GetName(); }

            QueueCommand info(projectName, conf, false, QueueCommand::kBuild);

            if(bldConf && bldConf->IsCustomBuild()) {
                info.SetKind(QueueCommand::kCustomBuild);
                info.SetCustomBuildTarget(wxT("Build"));
            }

            gsgs_Manager()->PushQueueCommand(info);
            gsgs_Manager()->ProcessCommandQueue();
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnRebuildProject(wxCommandEvent&)
    {
        RebuildProject(gsgs_Manager()->GetActiveProjectName());
    }
    //-----------------------------------------------------------------
    void Frame::OnBuildProjectUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        if(!gsgs_Workspace().GetWorkspace() || !gsgs_Workspace().GetWorkspace()->IsBuildSupported()) {
            event.Enable(false);
        } 
        else 
        {
            event.Enable(!gsgs_Manager()->IsBuildInProgress());
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnStopExecutedProgramUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        ExecuteEvent e(wxEVT_CMD_IS_PROGRAM_RUNNING, GetId());
        e.SetEventObject(this);
        e.SetAnswer(false);
        gsgs_Event().ProcessEvent(e);

        Manager* mgr = gsgs_Manager();
        bool enable = mgr->IsProgramRunning();
        event.Enable(enable || e.IsAnswer());
    }
    //-----------------------------------------------------------------
    void Frame::OnStopBuildUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        if(!gsgs_Workspace().GetWorkspace() || !gsgs_Workspace().GetWorkspace()->IsBuildSupported()) {
            event.Enable(false);
        } else {
            Manager* mgr = gsgs_Manager();
            bool enable = mgr->IsBuildInProgress();
            event.Enable(enable);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnStopBuild(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        Manager* mgr = gsgs_Manager();
        if(mgr->IsBuildInProgress()) { mgr->StopBuild(); }
    }
    //-----------------------------------------------------------------
    void Frame::OnStopExecutedProgram(wxCommandEvent& event)
    {
        ExecuteEvent evtExecute(wxEVT_CMD_STOP_EXECUTED_PROGRAM);
        if(gsgs_Event().ProcessEvent(evtExecute)) return;

        wxUnusedVar(event);
        Manager * mgr = gsgs_Manager();
        if(mgr->IsProgramRunning()) 
        { 
            mgr->KillProgram(); 
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnCleanProject(wxCommandEvent &)
    {
        // Let the plugins handle this first
        BuildEvent buildEvent(wxEVT_BUILD_STARTING);
        buildEvent.SetKind("clean");
        if(gsgs_Event().ProcessEvent(buildEvent)) { return; }

        wxString conf, projectName;
        projectName = gsgs_Manager()->GetActiveProjectName();

        QueueCommand buildInfo(QueueCommand::kClean);
        gsgs_Manager()->PushQueueCommand(buildInfo);
        gsgs_Manager()->ProcessCommandQueue();
    }
    //-----------------------------------------------------------------
    void Frame::OnCleanProjectUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        if(!gsgs_Workspace().GetWorkspace() || !gsgs_Workspace().GetWorkspace()->IsBuildSupported()) 
        {
            event.Enable(false);
            return;
        }
        else
        {
            event.Enable(!gsgs_Manager()->IsBuildInProgress());
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnExecuteNoDebug(wxCommandEvent& event)
    {
        // Sanity
        if(gsgs_CppWorker().IsOpen() && !gsgs_CppWorker().GetActiveProject()) { return; }

        // Let the plugin process this first
        ExecuteEvent evtExecute(wxEVT_CMD_EXECUTE_ACTIVE_PROJECT);
        if(gsgs_CppWorker().IsOpen()) {
            // set the project name
            evtExecute.SetTargetName(gsgs_CppWorker().GetActiveProject()->GetName());
        }
        if(gsgs_Event().ProcessEvent(evtExecute)) { return; }

        // Hereon, C++ workspace only

        // Again, sanity
        if(!gsgs_CppWorker().IsOpen()) { return; }

        // Prepare the commands to execute
        QueueCommand commandExecute(QueueCommand::kExecuteNoDebug);
        wxStandardID res =
            ::PromptMessageDialogWithoutCancel(_("Would you like to build the active project\nbefore executing it?"),
                                               "PromptForBuildBeforeExecute", _("Build and Execute"), _("Execute"));
        // Don't do anything if "X" is pressed
        if(res != wxID_CANCEL) {
            if(res == wxID_YES) {
                QueueCommand buildCommand(QueueCommand::kBuild);
                gsgs_Manager()->PushQueueCommand(buildCommand);
                commandExecute.SetCheckBuildSuccess(true); // execute only if build was successfull
            }

            gsgs_Manager()->PushQueueCommand(commandExecute);
            gsgs_Manager()->ProcessCommandQueue();
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnExecuteNoDebugUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        if(gsgs_Workspace().GetWorkspace()) 
        {
            ExecuteEvent e(wxEVT_CMD_IS_PROGRAM_RUNNING, GetId());
            e.SetEventObject(this);
            e.SetAnswer(false);
            gsgs_Event().ProcessEvent(e);
            bool somethingIsRunning = gsgs_Manager()->IsProgramRunning() || e.IsAnswer();
            event.Enable(!somethingIsRunning);
        }
        else
        {
            event.Enable(false);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnTimer(wxTimerEvent& event)
    {
        //#ifdef __WXMSW__
        //    wxWindowUpdateLocker locker(this);
        //#endif

        gsgsLogMessage(wxString::Format(wxT("Install path: %s"), gsgs_Path().GetInstallDir().c_str()));
        gsgsLogMessage(wxString::Format(wxT("Startup Path: %s"), gsgs_Path().GetStartupDirectory().c_str()));
        gsgsLogMessage("Using " + wxStyledTextCtrl::GetLibraryVersionInfo().ToString());
        if(::IsCygwinEnv()) 
        { 
            gsgsLogMessage("Running under Cygwin environment"); 
        }

        bool temprrr;
        gsgs_Config().Read(kConfigCheckForNewVersion, temprrr, true);
        if(temprrr)
        {
            m_webUpdate = new WebUpdateJob(this, false, gsgs_Config().Read("PromptForNewReleaseOnly", false));
            m_webUpdate->Check();
        }

        // enable/disable plugins toolbar functionality
        PluginManager::Get()->EnableToolbars();

        // Do we need to update the parser paths?
        bool updateParserPaths;
        gsgs_Config().Read(kConfigUpdateParserPaths, updateParserPaths, true);
        if(updateParserPaths) 
        {
            UpdateParserSearchPathsFromDefaultCompiler();
            // Now that we have updated them, mark it as done, so next
            // startups we won't do this again
            gsgs_Config().Write(kConfigUpdateParserPaths, false);
        }

        /////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////
        // clear navigation queue
        if(GetMainBook()->GetCurrentPage() == 0) 
        { 
            NavMgr::Get()->Clear(); 
        }

        // ReTag workspace database if needed (this can happen due to schema version changes)
        // It is important to place the retag code here since the retag workspace should take place after
        // the parser search patha have been updated (if needed)
        if(m_workspaceRetagIsRequired) {
            m_workspaceRetagIsRequired = false;
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("full_retag_workspace"));
            this->AddPendingEvent(evt);
            mInfoBar->DisplayMessage(_("A workspace reparse is needed"), wxICON_INFORMATION);
        }

        // For some reason, under Linux we need to force the menu accelerator again
        // otherwise some shortcuts are getting lose (e.g. Ctrl-/ to comment line)
        gsgs_Manager()->UpdateMenuAccelerators();

        wxModule::InitializeModules();

        // Send initialization end event
        //CallAfter(&Frame::Bootstrap);
        Frame::Bootstrap();
        event.Skip();
    }
    //-----------------------------------------------------------------
    void Frame::UpdateParserSearchPathsFromDefaultCompiler()
    {
        gsgs_Config().ReadItem(&m_tagsOptionsData);
        TagsOptionsData tmp;
        m_tagsOptionsData.Merge(tmp);

        // Try to locate the paths automatically
        CompilerPtr pCompiler = gsgs_Settings().GetDefaultCompiler(wxEmptyString);
        if(!pCompiler) return;

        wxArrayString paths;
        paths = pCompiler->GetDefaultIncludePaths();

        wxArrayString curExcludePaths = m_tagsOptionsData.GetParserExcludePaths();
        wxArrayString curIncluePaths = m_tagsOptionsData.GetParserSearchPaths();

        wxArrayString mergedPaths;
        StringUtils::MergeArrays(mergedPaths, curIncluePaths, paths);
        m_tagsOptionsData.SetParserExcludePaths(curExcludePaths);
        m_tagsOptionsData.SetParserSearchPaths(mergedPaths);
        m_tagsOptionsData.SetVersion(TagsOptionsData::CURRENT_VERSION);

        //-----------------------
        // clang
        //-----------------------

        wxArrayString clangSearchPaths = m_tagsOptionsData.GetClangSearchPathsArray();
        mergedPaths.Empty();
        StringUtils::MergeArrays(mergedPaths, paths, clangSearchPaths);
        m_tagsOptionsData.SetClangSearchPathsArray(mergedPaths);
        gsgs_Config().WriteItem(&m_tagsOptionsData);
    }
    //-----------------------------------------------------------------
    void Frame::OnFileCloseAll(wxCommandEvent&)
    {
        GetMainBook()->CallAfter(&Book::CloseAllVoid, true);
    }
    //-----------------------------------------------------------------
    void Frame::OnQuickOutline(wxCommandEvent &)
    {
        // Sanity
        Editor* editor = GetMainBook()->GetActiveEditor(true);
        CHECK_PTR_RET(editor);

        // let the plugins process this first
        CodeCompleteEvent evt(wxEVT_CC_SHOW_QUICK_OUTLINE, GetId());
        evt.SetEventObject(this);
        evt.SetEditor(editor);

        if(gsgs_Event().ProcessEvent(evt)) return;

        if (gsgs_Workspace().IsOpen())
        {
            if (editor->GetProject().IsEmpty()) return;

            QuickOutlineDlg dlg(this, editor->GetFileName().GetFullPath());

            dlg.ShowModal();
            editor->SetActive();
        }
    }
    //-----------------------------------------------------------------
    wxString Frame::CreateWorkspaceTable()
    {
        wxString html;
        wxArrayString files; gsgs_Config().GetRecentOpen(files);
        //wxColour bgclr = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        wxColour bgclr = gsgs_Style().GetAppBgColour();
        gsgs_Style().LightColour(bgclr, bgclr, 7);

        html << wxT("<table width=100% border=0 bgcolor=\"") << bgclr.GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">");
        if(files.GetCount() == 0) {
            html << wxT("<tr><td><font size=2 face=\"Verdana\">");
            html << _("No workspaces found.") << wxT("</font></td></tr>");
        } else {
            wxColour baseColour(232, 166, 101);
            wxColour firstColour;
            wxColour secondColour;
            gsgs_Style().LightColour(baseColour, firstColour, 6);
            gsgs_Style().LightColour(baseColour, secondColour, 8);
            for(int i = (int)files.GetCount(); i > 0; --i) {
                wxFileName fn(files.Item(i - 1));

                wxColour lineCol;
                if(i % 2) {
                    lineCol = firstColour;
                } else {
                    lineCol = secondColour;
                }

                html << wxT("<tr bgcolor=\"") << lineCol.GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">")
                     << wxT("<td><font size=2 face=\"Verdana\">") << wxT("<a href=\"action:open-file:") << fn.GetFullPath()
                     << wxT("\" >") << fn.GetName() << wxT("</a></font></td>") << wxT("<td><font size=2 face=\"Verdana\">")
                     << fn.GetFullPath() << wxT("</font></td>") << wxT("</tr>");
            }
        }

        html << wxT("</table>");
        return html;
    }
    //-----------------------------------------------------------------
    wxString Frame::CreateFilesTable()
    {
        wxString html;
        wxArrayString files; gsgs_Config().GetRecentOpen(files);

        //wxColour bgclr = wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE);
        wxColour bgclr = gsgs_Style().GetAppBgColour();
        gsgs_Style().LightColour(bgclr, bgclr, 10.0);
        html << wxT("<table width=100% border=0 bgcolor=\"") << bgclr.GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">");
        if(files.GetCount() == 0) {
            html << wxT("<tr><td><font size=2 face=\"Verdana\">");
            html << _("No files found.") << wxT("</font></td></tr>");
        } else {
            wxColour baseColour(232, 166, 101);
            wxColour firstColour;
            wxColour secondColour;
            gsgs_Style().LightColour(baseColour, firstColour, 6);
            gsgs_Style().LightColour(baseColour, secondColour, 8);
            for(int i = (int)files.GetCount(); i > 0; --i) {

                wxFileName fn(files.Item(i - 1));
                wxColour lineCol;
                if(i % 2) {
                    lineCol = firstColour;
                } else {
                    lineCol = secondColour;
                }
                html << wxT("<tr bgcolor=\"") << lineCol.GetAsString(wxC2S_HTML_SYNTAX) << wxT("\">")
                     << wxT("<td><font size=2 face=\"Verdana\">") << wxT("<a href=\"action:open-file:") << fn.GetFullPath()
                     << wxT("\" >") << fn.GetFullName() << wxT("</a></font></td>")
                     << wxT("<td><font size=2 face=\"Verdana\">") << fn.GetFullPath() << wxT("</font></td>")
                     << wxT("</tr>");
            }
        }
        html << wxT("</table>");
        return html;
    }
    //-----------------------------------------------------------------
    void Frame::OnRecentlyWorkspace(wxCommandEvent&)
    {
        wxArrayString files; gsgs_Config().GetRecentOpen(files);
        FileHistory & hs = gsgs_Workspace().GetRecentlyData();

        wxMenu * menu = NULL;
        wxMenuItem * item = GetMenuBar()->FindItem(XRCID("recent_workspaces"), &menu);
        if(item && menu) {
            wxMenu* submenu = item->GetSubMenu();
            if(submenu) {
                for(size_t i = files.GetCount(); i > 0; --i) {
                    hs.AddFileToHistory(files.Item(i - 1));
                }
                // set this menu as the recent file menu
                hs.SetBaseId(RecentWorkspaceSubMenuID + 1);
                hs.UseMenu(submenu);
                // Clear any stale items
                wxMenuItemList items = submenu->GetMenuItems();
                wxMenuItemList::reverse_iterator lriter = items.rbegin();
                for(; lriter != items.rend(); ++lriter)
                    submenu->Delete(*lriter);

                // Add entries without their .workspace extension
                hs.AddFilesToMenuWithoutExt();
            }
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnRecentFile(wxCommandEvent& event)
    {
        size_t idx = event.GetId() - (RecentFilesSubMenuID + 1);
        FileHistory& fh = GetMainBook()->GetRecentlyOpenedFilesClass();

        wxArrayString files;
        fh.GetFiles(files);
        if(idx < files.GetCount()) {
            wxString projectName = gsgs_Manager()->GetProjectNameByFile(files.Item(idx));
            gsgs_Frame()->GetMainBook()->OpenFile(files.Item(idx), projectName);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnRecentWorkspace(wxCommandEvent& event)
    {
        size_t idx = event.GetId() - (RecentWorkspaceSubMenuID + 1);
        FileHistory & fh = gsgs_Workspace().GetRecentlyData();

        wxArrayString files;
        fh.GetFiles(files);

        if(idx < files.GetCount()) {
            wxString file_name(files.Item(idx));

            wxCommandEvent open_workspace_event(wxEVT_COMMAND_MENU_SELECTED, XRCID("switch_to_workspace"));
            open_workspace_event.SetEventObject(this);
            open_workspace_event.SetString(file_name);
            GetEventHandler()->AddPendingEvent(open_workspace_event);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnBackwardForward(wxCommandEvent& event)
    {
        switch(event.GetId()) {
        case wxID_FORWARD:
            NavMgr::Get()->NavigateForward(PluginManager::Get());
            break;
        case wxID_BACKWARD:
            NavMgr::Get()->NavigateBackward(PluginManager::Get());
            break;
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnBackwardForwardUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        if(event.GetId() == wxID_FORWARD) {
            event.Enable(NavMgr::Get()->CanNext());
        } else if(event.GetId() == wxID_BACKWARD) {
            event.Enable(NavMgr::Get()->CanPrev());
        } else {
            event.Skip();
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnImportMSVS(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        const wxString ALL(wxT("All Solution File (*.dsw;*.sln;*.dev;*.bpr;*.cbp;*.workspace)|")
                               wxT("*.dsw;*.sln;*.dev;*.bpr;*.cbp;*.workspace|")
                                   wxT("MS Visual Studio Solution File (*.dsw;*.sln)|*.dsw;*.sln|")
                                       wxT("Bloodshed Dev-C++ Solution File (*.dev)|*.dev|")
                                           wxT("Borland C++ Builder Solution File (*.bpr)|*.bpr|")
                                               wxT("Code::Blocks Solution File (*.cbp;*.workspace)|*.cbp;*.workspace"));

        wxFileDialog dlg(this, _("Open IDE Solution/Workspace File"), wxEmptyString, wxEmptyString, ALL,
                         wxFD_OPEN | wxFD_FILE_MUST_EXIST, wxDefaultPosition);
        if(dlg.ShowModal() == wxID_OK) {

            wxArrayString cmps;
            BuildSettingsConfigCookie cookie;
            CompilerPtr cmp = gsgs_Settings().GetFirstCompiler(cookie);
            while(cmp) {
                cmps.Add(cmp->GetName());
                cmp = gsgs_Settings().GetNextCompiler(cookie);
            }

            // Get the prefered compiler type
            wxString compilerName = wxGetSingleChoice(_("Select the compiler to use:"), _("Choose compiler"), cmps);
            gsgs_Manager()->ImportMSVSSolution(dlg.GetPath(), compilerName);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnDebug(wxCommandEvent& e)
    {
        wxUnusedVar(e);

        bool isBuiltinDebuggerRunning =
            DebuggerMgr::Get().GetActiveDebugger() && DebuggerMgr::Get().GetActiveDebugger()->IsRunning();

        if(!gsgs_CppWorker().IsOpen()) {
            // We hae no workspace opened and yet we got here.
            // this can mean one of two:
            // 1. A non C++ workspace is opened - so we initiate the debugger start command
            // 2. User wishes to run QuickDebug// Let the plugin know that we are about to start debugging
            DebugEvent dbgEvent(wxEVT_DBG_UI_START);
            if(gsgs_Event().ProcessEvent(dbgEvent)) {

                // set the debugger features
                m_frameHelper->SetDebuggerFeatures(dbgEvent.GetFeatures());

                // the event was processed by one of the plugins, there is nothing left to
                // be done here
                return;
            }

            // Note:
            // The second scenario (launch QuickDebug) is handled later on this function
        }

        // Enable all features
        m_frameHelper->SetDebuggerFeatures(DebugEvent::kAllFeatures);

        if(!isBuiltinDebuggerRunning) {
            // Let the plugin know that we are about to start debugging
            DebugEvent dbgEvent(wxEVT_DBG_UI_CONTINUE);
            ProjectPtr activeProject = gsgs_CppWorker().GetActiveProject();
            if(activeProject) {
                BuildConfigPtr buildConfig = activeProject->GetBuildConfiguration();
                if(buildConfig) { dbgEvent.SetDebuggerName(buildConfig->GetDebuggerType()); }
            }
            if(gsgs_Event().ProcessEvent(dbgEvent)) return;
        }

        Manager* mgr = gsgs_Manager();
        if(isBuiltinDebuggerRunning) {
            // Debugger is already running -> continue command
            mgr->DbgContinue();

        } else if(gsgs_Workspace().IsOpen()) {

            if(gsgs_CppWorker().GetActiveProjectName().IsEmpty()) {
                gsgsLogMessage(_("Attempting to debug workspace with no active project? Ignoring."));
                return;
            }

            // Debugger is not running, but workspace is opened -> start debug session
            QueueCommand dbgCmd(QueueCommand::kDebug);

            wxStandardID res =
                ::PromptMessageDialogWithoutCancel(_("Would you like to build the project before debugging it?"),
                                                   "BuildBeforeDebug", _("Build and Debug"), _("Debug"));
            // Don't do anything if "X" is pressed
            if(res != wxID_CANCEL) {
                if(res == wxID_YES) {
                    QueueCommand bldCmd(QueueCommand::kBuild);
                    gsgs_Manager()->PushQueueCommand(bldCmd);
                    dbgCmd.SetCheckBuildSuccess(true);
                }

                // place a debug command
                gsgs_Manager()->PushQueueCommand(dbgCmd);

                // trigger the commands queue
                gsgs_Manager()->ProcessCommandQueue();
            }
        } else if(!gsgs_Workspace().IsOpen()) {
            // Run the 'Quick Debug'
            OnQuickDebug(e);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        e.Enable(!gsgs_Manager()->IsBuildInProgress());
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugRestart(wxCommandEvent& e)
    {
        DebugEvent event(wxEVT_DBG_UI_RESTART);
        if(gsgs_Event().ProcessEvent(event)) return;

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning() && gsgs_Manager()->DbgCanInteract()) {
            GetDebuggerPane()->GetLocalsTable()->Clear();
            dbgr->Restart();
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugRestartUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();

        DebugEvent event(wxEVT_DBG_IS_RUNNING);
        gsgs_Event().ProcessEvent(event);

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        e.Enable(event.IsAnswer() || (dbgr && dbgr->IsRunning() && gsgs_Manager()->DbgCanInteract()));
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugStop(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        DebugEvent debugEvent(wxEVT_DBG_UI_STOP);
        if(gsgs_Event().ProcessEvent(debugEvent)) { return; }
        gsgs_Manager()->DbgStop();
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugStopUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();

        DebugEvent eventIsRunning(wxEVT_DBG_IS_RUNNING);
        gsgs_Event().ProcessEvent(eventIsRunning);

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        e.Enable(eventIsRunning.IsAnswer() || (dbgr && dbgr->IsRunning()));
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugManageBreakpointsUI(wxUpdateUIEvent& e)
    {
        if(e.GetId() == XRCID("delete_all_breakpoints")) {
            std::vector<BreakpointInfo> bps;
            gsgs_Manager()->GetBreakpointsMgr()->GetBreakpoints(bps);
            e.Enable(bps.size());
        } else if(e.GetId() == XRCID("disable_all_breakpoints")) {
            e.Enable(gsgs_Manager()->GetBreakpointsMgr()->AreThereEnabledBreakpoints());
        } else if(e.GetId() == XRCID("enable_all_breakpoints")) {
            e.Enable(gsgs_Manager()->GetBreakpointsMgr()->AreThereDisabledBreakpoints());
        } else {

            e.Enable(true);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugCmd(wxCommandEvent& e)
    {
        int cmd(wxNOT_FOUND);
        int eventId(wxNOT_FOUND);

        if(e.GetId() == XRCID("pause_debugger")) 
        {
            cmd = DBG_PAUSE;
            eventId = wxEVT_DBG_UI_INTERRUPT;
        } 
        else if(e.GetId() == XRCID("dbg_stepin")) 
        {
            cmd = DBG_STEPIN;
            eventId = wxEVT_DBG_UI_STEP_IN;
        } 
        else if(e.GetId() == XRCID("dbg_stepi")) 
        {
            cmd = DBG_STEPI;
            eventId = wxEVT_DBG_UI_STEP_I;
        } 
        else if(e.GetId() == XRCID("dbg_stepout")) 
        {
            cmd = DBG_STEPOUT;
            eventId = wxEVT_DBG_UI_STEP_OUT;
        } 
        else if(e.GetId() == XRCID("dbg_next")) 
        {
            cmd = DBG_NEXT;
            eventId = wxEVT_DBG_UI_NEXT;
        } 
        else if(e.GetId() == XRCID("show_cursor")) 
        {
            cmd = DBG_SHOW_CURSOR;
            eventId = wxEVT_DBG_UI_SHOW_CURSOR;

        } 
        else if(e.GetId() == XRCID("dbg_nexti")) 
        {
            cmd = DBG_NEXTI;
            eventId = wxEVT_DBG_UI_NEXT_INST;
        }

        // Allow the plugins to handle this command first
        DebugEvent evnt(eventId);
        if(gsgs_Event().ProcessEvent(evnt)) 
            return;

        if(cmd != wxNOT_FOUND) 
        { 
            gsgs_Manager()->DbgDoSimpleCommand(cmd); 
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugCmdUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();

        DebugEvent eventIsRunning(wxEVT_DBG_IS_RUNNING);
        gsgs_Event().ProcessEvent(eventIsRunning);

        if(e.GetId() == XRCID("pause_debugger") || e.GetId() == XRCID("dbg_stepin") || e.GetId() == XRCID("dbg_stepi") ||
           e.GetId() == XRCID("dbg_stepout") || e.GetId() == XRCID("dbg_next") || e.GetId() == XRCID("dbg_nexti") ||
           e.GetId() == XRCID("show_cursor")) {
            IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
            e.Enable(eventIsRunning.IsAnswer() || (dbgr && dbgr->IsRunning()));
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnIdle(wxIdleEvent& e)
    {
        e.Skip();
        // make sure that we are always set to the working directory of the workspace
        //    if(gsgs_CppWorker().IsOpen()) {
        //        // Check that current working directory is set to the workspace folder
        //        wxString path = gsgs_CppWorker().GetFileName().GetPath();
        //        wxString curdir = ::wxGetCwd();
        //        if(path != curdir) {
        //            // Check that it really *is* different, not just a symlink issue: see bug #942
        //            if(RealPath(path) != RealPath(curdir)) {
        //                gsgsDEBUG1() << "Current working directory is reset to:" << path;
        //                ::wxSetWorkingDirectory(path);
        //            }
        //        }
        //    }
    }
    //-----------------------------------------------------------------
    void Frame::OnLinkClicked(wxHtmlLinkEvent& e)
    {
        wxString action = e.GetLinkInfo().GetHref();
        if(!action.StartsWith(wxT("action:"), &action)) {
            ::wxLaunchDefaultBrowser(e.GetLinkInfo().GetHref());
            return;
        }
        wxString command = action.BeforeFirst(wxT(':'));
        wxString filename = action.AfterFirst(wxT(':'));
        if(command != wxT("switch-workspace") && command != wxT("open-file") && command != wxT("create-workspace") &&
           command != wxT("import-msvs-solution") && command != wxT("open-workspace")) {
            e.Skip();
            return;
        }

        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("link_action"));
        event.SetEventObject(this);
        StartPageData* data = new StartPageData;
        data->action = command;
        data->file_path = filename;
        if(wxFileName(filename).GetExt() == wxT("workspace")) { data->action = wxT("switch-workspace"); }
        event.SetClientData(data);
        wxPostEvent(this, event);
    }
    //-----------------------------------------------------------------
    void Frame::OnStartPageEvent(wxCommandEvent& e)
    {
        StartPageData* data = (StartPageData*)e.GetClientData();
        if(data->action == wxT("switch-workspace")) {
            WindowUpdateLocker locker(this);
            gsgs_Manager()->OpenWorkspace(data->file_path);

        } else if(data->action == wxT("open-file")) {
            gsgs_Frame()->GetMainBook()->OpenFile(data->file_path, wxEmptyString);

        } else if(data->action == wxT("create-workspace")) {
            OnProjectNewWorkspace(e);

        } else if(data->action == wxT("import-msvs-solution")) {
            OnImportMSVS(e);

        } else if(data->action == wxT("open-workspace")) {
            OnSwitchWorkspace(e);
        }
        wxDELETE(data);
    }
    //-----------------------------------------------------------------
    void Frame::SetFrameFlag(bool set, int flag)
    {
        if(set) {
            mFrameInfo->SetFlags(mFrameInfo->GetFlags() | flag);
        } else {
            mFrameInfo->SetFlags(mFrameInfo->GetFlags() & ~(flag));
        }
    }
    //-----------------------------------------------------------------
    void Frame::CompleteInitialization()
    {
    #ifdef __WXMSW__
        wxWindowUpdateLocker locker(this);
    #endif

        // Populate the list of core toolbars before we start loading
        // the plugins
        wxAuiPaneInfoArray& panes = mWinMag->GetAllPanes();
        for(size_t i = 0; i < panes.GetCount(); ++i) {
            if(panes.Item(i).IsToolbar()) { m_coreToolbars.insert(panes.Item(i).name); }
        }

        // Register C++ keyboard shortcuts
        gsgs_Window().addHotKey("swap_files", "F12", "C++::Swap Header/Implementation file");
        gsgs_Window().addHotKey("rename_symbol", "Ctrl-Shift-H", "C++::Rename Symbol");
        gsgs_Window().addHotKey("open_include_file", "", "Open Include File");

        // Load the plugins
        PluginManager::Get()->Load();

    // Load debuggers (*must* be after the plugins)
    #ifdef USE_POSIX_LAYOUT
        wxString plugdir(gsgs_Path().GetPluginsDirectory());
        DebuggerMgr::Get().Initialize(this, EditorConfigST::Get(), plugdir);
    #else
        DebuggerMgr::Get().Initialize(this, &gsgs_EditorConfig(), gsgs_Path().GetInstallDir());
    #endif
        DebuggerMgr::Get().LoadDebuggers();

        // Connect some system events
        mWinMag->Connect(wxEVT_AUI_PANE_CLOSE, wxAuiManagerEventHandler(Frame::OnDockablePaneClosed), NULL, this);
        // mWinMag->Connect(wxEVT_AUI_RENDER,     wxAuiManagerEventHandler(Frame::OnAuiManagerRender),   NULL, this);

        SelectBestEnvSet();

        // Now everything is loaded, set the saved tab-order in the workspace and the output pane
        GetWorkspacePane()->ApplySavedTabOrder();
        GetOutputPane()->ApplySavedTabOrder();
        Layout();
        gsgs_Manager()->GetPerspectiveManager().ConnectEvents(mWinMag);

        wxCommandEvent evt(wxEVT_GSGS_THEME_CHANGED);
        gsgs_Event().AddPendingEvent(evt);

    #ifndef __WXMSW__
        sigset_t child_set;
        sigemptyset(&child_set);
        sigaddset(&child_set, SIGCHLD);

        // make sure SIGCHILD is not blocked
        sigprocmask(SIG_UNBLOCK, &child_set, NULL);

        // Start the Zombie Reaper thread
        m_zombieReaper.Start();
    #endif
        bool tempb;
        // Hide / Show status/tool bar (native)
        gsgs_Config().Read(kConfigShowToolBar, tempb, false);
        DoShowToolbars(tempb);

        gsgs_Config().Read(kConfigShowStatusBar, tempb, true);
        DoShowStatusBar(tempb);

        // Hide / Show tab bar
        gsgs_Config().Read(kConfigShowTabBar, tempb, true);
        wxCommandEvent eventShowTabBar;
        eventShowTabBar.SetInt(tempb);
        OnShowTabBar(eventShowTabBar);
        ShowOrHideCaptions();

        TabGroupsManager::Get(); // Ensure that the events are binded

        //gsgs_Manager()->GetPerspectiveManager().LoadPerspective(NORMAL_LAYOUT);
        m_initCompleted = true;

        if(GetTheApp()->IsStartedInDebuggerMode()) {
            wxCommandEvent quickDebugEvent(wxEVT_MENU, XRCID("quick_debug"));
            quickDebugEvent.SetEventObject(this);
            GetEventHandler()->AddPendingEvent(quickDebugEvent);
        }

        GotoEntryManager::Get().Initialise();

        // Update the toolbar view
        wxArrayString hiddenItems;
        gsgs_Config().Read("ToolBarHiddenItems", hiddenItems, wxArrayString());
        std::vector<ToolBarButton*>& buttons = mToolBar->GetButtons();
        for(size_t i = 0; i < hiddenItems.size(); ++i) 
        {
            const wxString& label = hiddenItems.Item(i);
            std::vector<ToolBarButton*>::iterator iter = std::find_if(
                buttons.begin(), buttons.end(), [&](ToolBarButton* button) { return button->GetLabel() == label; });
            if(iter != buttons.end()) { (*iter)->Show(false); }
        }

        // Prompt the user to adjust his colours
        bool colourAdjusted;
        gsgs_Config().Read("ColoursAdjusted", colourAdjusted, false);
        if(!colourAdjusted) 
        {
            // Adjust the user colour
            GetMessageBar()->DisplayMessage(_("光栅故事 : apply colour theme, now?"),
                wxICON_QUESTION, { { XRCID("adjust-current-theme"), _("Yes") }, { wxID_NO, "" } });

            // regardless of the answer, dont bug the user again
            gsgs_Config().Write("ColoursAdjusted", true);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnAppActivated(wxActivateEvent& e)
    {
        if(mMainFrame && e.GetActive()) {

            // if workspace or project was modified, don't prompt for
            // file(s) reload
            if(!mMainFrame->ReloadExternallyModifiedProjectFiles()) {
                mMainFrame->GetMainBook()->CallAfter(&Book::ReloadExternallyModified, true);
            }

            // Notify plugins that we got the focus.
            // Some plugins want to hide some frames etc
            wxCommandEvent evtGotFocus(wxEVT_GSGS_MAINFRAME_GOT_FOCUS);
            gsgs_Event().AddPendingEvent(evtGotFocus);

    #ifdef __WXOSX__
            // Set the focus back to the active editor
            Editor* activeEditor = GetMainBook()->GetActiveEditor();
            if(activeEditor) { activeEditor->CallAfter(&Editor::SetActive); }
    #endif

        } else if(mMainFrame) {

    #ifndef __WXMAC__
            /// this code causes crash on Mac, since it destorys an active CCBox
            Editor* editor = GetMainBook()->GetActiveEditor();
            if(editor) {
                // we are loosing the focus
                editor->DoCancelCalltip();
            }
    #endif
        }

        e.Skip();
    }
    //-----------------------------------------------------------------
    void Frame::OnCompileFile(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        Manager* mgr = gsgs_Manager();
        if(gsgs_Workspace().IsOpen() && !mgr->IsBuildInProgress()) {
            Editor* editor = GetMainBook()->GetActiveEditor();
            if(editor && !editor->GetProject().IsEmpty()) {
                mgr->CompileFile(editor->GetProject(), editor->GetFileName().GetFullPath());
            }
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnCompileFileUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        e.Enable(false);
        if(!gsgs_Workspace().GetWorkspace() || !gsgs_Workspace().GetWorkspace()->IsBuildSupported()) {
            return;
        }
        Manager* mgr = gsgs_Manager();
        if(gsgs_Workspace().IsOpen() && !mgr->IsBuildInProgress()) {
            Editor* editor = GetMainBook()->GetActiveEditor();
            if(editor && !editor->GetProject().IsEmpty()) { e.Enable(true); }
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugAttach(wxCommandEvent&)
    {
        // Start the debugger
        Manager* mgr = gsgs_Manager();
        mgr->DbgStart(1);
    }
    //-----------------------------------------------------------------
    void Frame::OnCloseAllButThis(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        wxWindow* win = GetMainBook()->GetCurrentPage();
        if(win != NULL) { GetMainBook()->CallAfter(&Book::CloseAllButThisVoid, win); }
    }
    //-----------------------------------------------------------------
    void Frame::OnFileCloseUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        event.Enable(GetMainBook()->GetCurrentPage() != NULL);
    }
    //-----------------------------------------------------------------
    void Frame::OnConvertEol(wxCommandEvent& e)
    {
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor) {
            int eol(wxSTC_EOL_LF);
            if(e.GetId() == XRCID("convert_eol_win")) {
                eol = wxSTC_EOL_CRLF;
            } else if(e.GetId() == XRCID("convert_eol_mac")) {
                eol = wxSTC_EOL_CR;
            }
            editor->ConvertEOLs(eol);
            editor->SetEOLMode(eol);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnViewDisplayEOL(wxCommandEvent& e)
    {
        bool visible;
        size_t frame_flags = mFrameInfo->GetFlags();
        if(e.IsChecked()) {
            frame_flags |= GSGS_SHOW_EOL;
            visible = true;
        } else {
            frame_flags &= ~GSGS_SHOW_EOL;
            visible = false;
        }

        mFrameInfo->SetFlags(frame_flags);
        GetMainBook()->SetViewEOL(visible);
    }
    //-----------------------------------------------------------------
    void Frame::OnViewDisplayEOL_UI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor) {
            e.Enable(true);
            e.Check(mFrameInfo->GetFlags() & GSGS_SHOW_EOL ? true : false);
        }
        e.Enable(false);
    }
    //-----------------------------------------------------------------
    void Frame::OnCopyFileName(wxCommandEvent& event)
    {
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor) {
            wxString fileName = editor->GetFileName().GetFullName();
    #if wxUSE_CLIPBOARD
            if(wxTheClipboard->Open()) {
                wxTheClipboard->UsePrimarySelection(false);
                if(!wxTheClipboard->SetData(new wxTextDataObject(fileName))) {
                    // wxPrintf(wxT("Failed to insert data %s to clipboard"), textToCopy.GetData());
                }
                wxTheClipboard->Close();
            } else {
                wxPrintf(wxT("Failed to open the clipboard"));
            }
    #endif
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnCopyFilePath(wxCommandEvent&)
    {
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor) {
            wxString fileName = editor->GetFileName().GetFullPath();
    #if wxUSE_CLIPBOARD
            if(wxTheClipboard->Open()) {
                wxTheClipboard->UsePrimarySelection(false);
                if(!wxTheClipboard->SetData(new wxTextDataObject(fileName))) {
                    // wxPrintf(wxT("Failed to insert data %s to clipboard"), textToCopy.GetData());
                }
                wxTheClipboard->Close();
            } else {
                wxPrintf(wxT("Failed to open the clipboard"));
            }
    #endif
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnCopyFilePathOnly(wxCommandEvent&)
    {
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor) {
            wxString fileName = editor->GetFileName().GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    #if wxUSE_CLIPBOARD
            if(wxTheClipboard->Open()) {
                wxTheClipboard->UsePrimarySelection(false);
                if(!wxTheClipboard->SetData(new wxTextDataObject(fileName))) {
                    // wxPrintf(wxT("Failed to insert data %s to clipboard"), textToCopy.GetData());
                }
                wxTheClipboard->Close();
            } else {
                wxPrintf(wxT("Failed to open the clipboard"));
            }
    #endif
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnWorkspaceMenuUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        if(gsgs_Workspace().IsOpen() == false) {
            e.Enable(false);
            return;
        }
        if(gsgs_Manager()->IsBuildInProgress()) {
            e.Enable(false);
            return;
        }
        e.Enable(true);
    }
    //-----------------------------------------------------------------
    void Frame::OnManagePlugins(wxCommandEvent&)
    {
        PluginMgrDlg dlg(this);
        if(dlg.ShowModal() == wxID_OK) 
            mInfoBar->DisplayMessage(_("GSGS suggestion: restart, do it now?"), wxICON_QUESTION,
                { { XRCID("restart-gsgs"), _("Yes") }, { wxID_NO, _("No") } });
    }
    //-----------------------------------------------------------------
    void Frame::OnCppContextMenu(wxCommandEvent& e)
    {
        Editor* editor = GetMainBook()->GetActiveEditor(true);
        if(editor) editor->GetContext()->ProcessEvent(e);
    }
    //-----------------------------------------------------------------
    void Frame::OnUpdateBuildRefactorIndexBar(wxCommandEvent&) {  }
    //-----------------------------------------------------------------
    void Frame::OnHighlightWord(wxCommandEvent& event)
    {
        long highlightWord = gsgs_EditorConfig().GetInteger(wxT("highlight_word"), 1);

        // Notify all open editors that word hight is checked
        wxCommandEvent evtEnable(wxCMD_EVENT_ENABLE_WORD_HIGHLIGHT);
        if(!highlightWord) {
            GetMainBook()->HighlightWord(true);
            gsgs_EditorConfig().SetInteger(wxT("highlight_word"), 1);
            evtEnable.SetInt(1);
        } else {
            GetMainBook()->HighlightWord(false);
            gsgs_EditorConfig().SetInteger(wxT("highlight_word"), 0);
            evtEnable.SetInt(0);
        }

        gsgs_Event().AddPendingEvent(evtEnable);
    }
    //-----------------------------------------------------------------
    void Frame::OnShowNavBar(wxCommandEvent& e)
    {
        GetMainBook()->ShowNavBar(e.IsChecked());
        //mWinMag->Update();
    }
    //-----------------------------------------------------------------
    void Frame::OnShowNavBarUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        e.Check(GetMainBook()->IsNavBarShown());
    }
    //-----------------------------------------------------------------
    void Frame::OnSingleInstanceOpenFiles(Event& e)
    {
        const wxArrayString& files = e.GetStrings();
        for(size_t i = 0; i < files.GetCount(); ++i) {
            wxFileName fn(files.Item(i));
            // if file is workspace, load it
            if(fn.GetExt() == wxT("gsgsws")) {
                wxCommandEvent workspaceEvent;
                workspaceEvent.SetString(files.Item(i));
                OnSwitchWorkspace(workspaceEvent);

            } else {
                GetMainBook()->OpenFile(files.Item(i), wxEmptyString);
            }
        }

        CallAfter(&Frame::Raise);
    }
    //-----------------------------------------------------------------
    void Frame::OnSingleInstanceRaise(Event& e)
    {
        wxUnusedVar(e);
        Raise();
    }
    //-----------------------------------------------------------------
    void Frame::OnNewVersionAvailable(wxCommandEvent& e)
    {
        gsgsDEBUG() << "Frame::OnNewVersionAvailable called:"
                  << (e.GetEventType() == wxEVT_CMD_VERSION_UPTODATE ? "up-to-date" : "new version found") << clEndl;
        if((e.GetEventType() == wxEVT_CMD_VERSION_UPTODATE) && m_webUpdate->IsUserRequest()) {
            // All is up to date
            wxMessageBox(_("You already have the latest version of 光栅故事"), "光栅故事", wxOK | wxCENTRE, this);
        } else {
            WebUpdateJobData* data = reinterpret_cast<WebUpdateJobData*>(e.GetClientData());
            if(data) {
                if(data->IsUpToDate() == false) {
                    wxRichMessageDialog dlg(this, _("A new version of 光栅故事 is available for download"), "光栅故事",
                                            wxYES_NO | wxCANCEL | wxYES_DEFAULT | wxCENTRE | wxICON_INFORMATION);
                    dlg.SetYesNoLabels(_("Download"), _("No"));
                    if(dlg.ShowModal() == wxID_YES) { ::wxLaunchDefaultBrowser(data->GetUrl()); }
                }
                wxDELETE(data);
            }
        }
        wxDELETE(m_webUpdate);
    }
    //-----------------------------------------------------------------
    void Frame::OnDetachWorkspaceViewTab(wxCommandEvent& e)
    {
        size_t sel = GetWorkspacePane()->GetNotebook()->GetSelection();
        wxWindow* page = GetWorkspacePane()->GetNotebook()->GetCurrentPage();
        wxString text = GetWorkspacePane()->GetNotebook()->GetPageText(sel);
        wxBitmap bmp = GetWorkspacePane()->GetNotebook()->GetPageBitmap(
            sel); // We might have a bitmap on the tab, make sure we restore it

        DockablePane* pane = new DockablePane(this, GetWorkspacePane()->GetNotebook(), text, true, bmp, wxSize(200, 200));
        page->Reparent(pane);

        // remove the page from the notebook
        GetWorkspacePane()->GetNotebook()->RemovePage(sel);
        pane->SetChildNoReparent(page);
        wxUnusedVar(e);
    }
    //-----------------------------------------------------------------
    void Frame::OnHideOutputViewTab(wxCommandEvent& e)
    {
        size_t sel = GetOutputPane()->GetNotebook()->GetSelection();
        wxString text = GetOutputPane()->GetNotebook()->GetPageText(sel);

        Event eventHide(wxEVT_SHOW_OUTPUT_TAB);
        eventHide.SetSelected(false).SetString(text);
        gsgs_Event().AddPendingEvent(eventHide);
    }
    //-----------------------------------------------------------------
    void Frame::OnHideWorkspaceViewTab(wxCommandEvent& e)
    {
        size_t sel = GetWorkspacePane()->GetNotebook()->GetSelection();
        wxString text = GetWorkspacePane()->GetNotebook()->GetPageText(sel);

        Event eventHide(wxEVT_SHOW_WORKSPACE_TAB);
        eventHide.SetSelected(false).SetString(text);
        gsgs_Event().AddPendingEvent(eventHide);
    }
    //-----------------------------------------------------------------
    void Frame::OnNewDetachedPane(wxCommandEvent& e)
    {
        DockablePane* pane = (DockablePane*)(e.GetClientData());
        if(pane) {
            wxString text = pane->GetName();
            mWinMag->AddMenu(text);

            // keep list of all detached panes
            wxArrayString panes;
            mWinMag->getFloatingList(panes);
            DetachedPanesInfo dpi(panes);
            gsgs_EditorConfig().WriteObject(wxT("DetachedPanesList"), &dpi);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnDestroyDetachedPane(wxCommandEvent& e)
    {
        DockablePane* pane = (DockablePane*)(e.GetClientData());
        if(pane) {
            mWinMag->DetachPane(pane);

            // remove any menu entries for this pane
            mWinMag->RemoveMenu(pane->GetName());

            // keep list of all detached panes
            wxArrayString panes;
            mWinMag->getFloatingList(panes);
            DetachedPanesInfo dpi(panes);
            gsgs_EditorConfig().WriteObject(wxT("DetachedPanesList"), &dpi);

            pane->Destroy();
            mWinMag->UpdateAccels();
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnAuiManagerRender(wxAuiManagerEvent& e)
    {
        wxAuiManager* mgr = e.GetManager();
        wxAuiPaneInfoArray& panes = mgr->GetAllPanes();

        wxAcceleratorTable* accelTable = GetAcceleratorTable();
        if(accelTable != NULL) {
            for(size_t i = 0; i < panes.GetCount(); i++) {
                if(panes[i].frame != NULL) { panes[i].frame->SetAcceleratorTable(*accelTable); }
            }
        }
        e.Skip();
    }
    //-----------------------------------------------------------------
    void Frame::OnDockablePaneClosed(wxAuiManagerEvent& e)
    {
        DockablePane* pane = dynamic_cast<DockablePane*>(e.GetPane()->window);
        wxAuiPaneInfo* pInfo = e.GetPane();
        if(pInfo->IsOk()) { WindowManager::HackHidePane(false, *pInfo, mWinMag); }
        if(pane) {
            wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_pane"));
            pane->GetEventHandler()->ProcessEvent(evt);
        } else {
            e.Skip();
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnFunctionCalltipUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        Editor* editor = GetMainBook()->GetActiveEditor();
        event.Enable(editor ? true : false);
    }
    //-----------------------------------------------------------------
    void Frame::OnReloadWorkspace(wxCommandEvent& event)
    {
        wxUnusedVar(event);

        // let the plugins close any custom workspace
        Event e(wxEVT_CMD_RELOAD_WORKSPACE, GetId());
        e.SetEventObject(this);
        if(gsgs_Event().ProcessEvent(e)) return; // this event was handled by a plugin

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning()) { return; }

        SaveLayoutAndSession();
        gsgs_Manager()->ReloadWorkspace();
    }
    //-----------------------------------------------------------------
    /*void Frame::OnReloadWorkspaceUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        event.Enable(gsgs_Workspace().IsOpen());
    }*/
    //-----------------------------------------------------------------
    void Frame::RebuildProject(const wxString& projectName)
    {
        bool enable = !gsgs_Manager()->IsBuildInProgress() && !gsgs_Manager()->GetActiveProjectName().IsEmpty();
        if(enable) {
            wxString conf;
            // get the selected configuration to be built
            BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
            if(bldConf) { conf = bldConf->GetName(); }

            // first we place a clean command
            QueueCommand buildInfo(projectName, conf, false, QueueCommand::kClean);
            if(bldConf && bldConf->IsCustomBuild()) {
                buildInfo.SetKind(QueueCommand::kCustomBuild);
                buildInfo.SetCustomBuildTarget(wxT("Clean"));
            }
            gsgs_Manager()->PushQueueCommand(buildInfo);

            // now we place a build command
            buildInfo = QueueCommand(projectName, conf, false, QueueCommand::kBuild);

            if(bldConf && bldConf->IsCustomBuild()) {
                buildInfo.SetKind(QueueCommand::kCustomBuild);
                buildInfo.SetCustomBuildTarget(wxT("Build"));
            }
            gsgs_Manager()->PushQueueCommand(buildInfo);

            // process the queue
            gsgs_Manager()->ProcessCommandQueue();
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnBatchBuildUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        if(!gsgs_Workspace().GetWorkspace() || !gsgs_Workspace().GetWorkspace()->IsBuildSupported()) {
            e.Enable(false);
        } else {
            e.Enable(!gsgs_Manager()->IsBuildInProgress() && gsgs_Workspace().IsOpen());
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnBatchBuild(wxCommandEvent& e)
    {
        ProjectBuildDlg* batchBuild = new ProjectBuildDlg(this);
        if(batchBuild->ShowModal() == wxID_OK) {
            // build the projects
            std::list<QueueCommand> buildInfoList;
            batchBuild->GetBuildInfoList(buildInfoList);
            if(buildInfoList.empty() == false) {
                std::list<QueueCommand>::iterator iter = buildInfoList.begin();

                // add all build items to queue
                for(; iter != buildInfoList.end(); iter++) {
                    gsgs_Manager()->PushQueueCommand(*iter);
                }
            }
        }
        batchBuild->Destroy();

        // start the build process
        gsgs_Manager()->ProcessCommandQueue();
    }
    //-----------------------------------------------------------------
    void Frame::SetFrameTitle(Editor* editor)
    {
        wxString title;
        if(editor && editor->GetModify()) { title << wxT("*"); }

        wxString pattern;
        gsgs_Config().Read(kConfigFrameTitlePattern, pattern, wxString("$workspace $fullpath"));
        wxString username = ::wxGetUserId();
        username.Prepend("[ ").Append(" ]");

        wxString workspace = gsgs_CppWorker().GetName();
        if(!workspace.IsEmpty()) { workspace.Prepend("[ ").Append(" ]"); }

        wxString fullname, fullpath;
        if(editor) {
            fullname = editor->GetFileName().GetFullName();
            fullpath = editor->GetFileName().GetFullPath();
        }

        pattern.Replace("$workspace", workspace);
        pattern.Replace("$user", username);
        pattern.Replace("$filename", fullname);
        pattern.Replace("$fullpath", fullpath);

        pattern.Trim().Trim(false);
        if(pattern.IsEmpty()) 
        { 
            pattern << wxT("光栅故事 ") << GSGS_VERSION; 
        } 
        else
        {
            pattern << wxT("-光栅故事 ") << GSGS_VERSION;
        }
        title << pattern;

        Event titleEvent(wxEVT_CL_FRAME_TITLE);
        titleEvent.SetString(title);
        gsgs_Event().ProcessEvent(titleEvent);

        SetTitle(titleEvent.GetString());
    }
    //-----------------------------------------------------------------
    void Frame::OnBuildWorkspace(wxCommandEvent&)
    {
        // start the build process
        gsgs_Manager()->BuildWorkspace();
    }
    //-----------------------------------------------------------------
    void Frame::OnBuildWorkspaceUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        if(!gsgs_Workspace().GetWorkspace() || !gsgs_Workspace().GetWorkspace()->IsBuildSupported()) {
            e.Enable(false);
        } else {
            e.Enable(gsgs_Workspace().IsOpen() && !gsgs_Manager()->IsBuildInProgress());
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnDetachDebuggerViewTab(wxCommandEvent&)
    {
        size_t sel = GetDebuggerPane()->GetNotebook()->GetSelection();
        wxWindow* page = GetDebuggerPane()->GetNotebook()->GetCurrentPage();
        wxString text = GetDebuggerPane()->GetNotebook()->GetPageText(sel);
        wxBitmap bmp = GetDebuggerPane()->GetNotebook()->GetPageBitmap(sel);

        DockablePane* pane = new DockablePane(this, GetDebuggerPane()->GetNotebook(), text, false, bmp, wxSize(200, 200));
        page->Reparent(pane);

        // remove the page from the notebook
        GetDebuggerPane()->GetNotebook()->RemovePage(sel);
        pane->SetChildNoReparent(page);
    }
    //-----------------------------------------------------------------
    void Frame::OnCleanWorkspace(wxCommandEvent&)
    {
        gsgs_Manager()->CleanWorkspace();
    }
    //-----------------------------------------------------------------
    void Frame::OnCleanWorkspaceUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        if(!gsgs_Workspace().GetWorkspace() || !gsgs_Workspace().GetWorkspace()->IsBuildSupported()) {
            e.Enable(false);
        } else {
            e.Enable(gsgs_Workspace().IsOpen() && !gsgs_Manager()->IsBuildInProgress());
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnReBuildWorkspace(wxCommandEvent&)
    {
        gsgs_Manager()->RebuildWorkspace();
    }
    //-----------------------------------------------------------------
    void Frame::OnReBuildWorkspaceUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        if(!gsgs_Workspace().GetWorkspace() || !gsgs_Workspace().GetWorkspace()->IsBuildSupported()) {
            e.Enable(false);
        } else {
            e.Enable(gsgs_Workspace().IsOpen() && !gsgs_Manager()->IsBuildInProgress());
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnOpenShellFromFilePath(wxCommandEvent& e)
    {
        // get the file path
        wxString filepath;
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor) { filepath = editor->GetFileName().GetPath(); }

        if(filepath.IsEmpty()) return;
        DirSaver ds;
        wxSetWorkingDirectory(filepath);

        // Apply the environment variabels before opening the shell
        EnvSetter setter;
        FileUtil::OpenTerminal(filepath);
    }
    //-----------------------------------------------------------------
    void Frame::OnQuickDebug(wxCommandEvent& e)
    {
        // launch the debugger
        QuickDebugDlg dlg(this);
        bool bStartedInDebugMode = GetTheApp()->IsStartedInDebuggerMode();
        if(bStartedInDebugMode || (dlg.ShowModal() == wxID_OK)) {
            // Disable the 'StartedInDebuggerMode' flag - so this will only happen once
            GetTheApp()->SetStartedInDebuggerMode(false);
            DebuggerMgr::Get().SetActiveDebugger(dlg.GetDebuggerName());
            IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();

            if(dbgr && !dbgr->IsRunning()) {

                std::vector<BreakpointInfo> bpList;
                wxString exepath = bStartedInDebugMode ? GetTheApp()->GetExeToDebug() : dlg.GetExe();
                wxString wd = bStartedInDebugMode ? GetTheApp()->GetDebuggerWorkingDirectory() : dlg.GetWorkingDirectory();
                wxArrayString cmds = bStartedInDebugMode ? wxArrayString() : dlg.GetStartupCmds();

                // update the debugger information
                DebuggerInformation dinfo;
                DebuggerMgr::Get().GetDebuggerInformation(dlg.GetDebuggerName(), dinfo);
                dinfo.breakAtWinMain = true;

                // Allow the quick debug to replace the debugger executable
                if(!bStartedInDebugMode && !dlg.GetAlternateDebuggerExe().IsEmpty()) {
                    dinfo.path = dlg.GetAlternateDebuggerExe();
                }

                // read the console command
                dinfo.consoleCommand = gsgs_EditorConfig().GetOptions()->GetProgramConsoleCommand();

                // gsgs_Manager()->GetBreakpointsMgr()->DelAllBreakpoints(); TODO: Reimplement this when
                // UpdateBreakpoints() updates only alterations, rather than delete/re-enter

                wxString dbgname = dinfo.path;
                dbgname = gsgs_EditorConfig().ExpandVariables(dbgname, true);

                // launch the debugger
                dbgr->SetObserver(gsgs_Manager());
                dbgr->SetDebuggerInformation(dinfo);

                // TODO: Reimplement this when UpdateBreakpoints() updates only alterations, rather than delete/re-enter
                // GetMainBook()->UpdateBreakpoints();

                // get an updated list of breakpoints
                gsgs_Manager()->GetBreakpointsMgr()->GetBreakpoints(bpList);

                DebuggerStartupInfo startup_info;
                startup_info.debugger = dbgr;

                // notify plugins that we're about to start debugging
                {
                    DebugEvent eventStarting(wxEVT_DEBUG_STARTING);
                    eventStarting.SetClientData(&startup_info);
                    if(gsgs_Event().ProcessEvent(eventStarting)) return;
                }

                wxString tty;
    #ifndef __WXMSW__
                if(!gsgs_Manager()->StartTTY(
                       DebuggerTerminalPOSIX::MakeExeTitle(
                           exepath, (bStartedInDebugMode ? GetTheApp()->GetDebuggerArgs() : dlg.GetArguments())),
                       tty)) {
                    wxMessageBox(_("Could not start TTY console for debugger!"), _("gsgs"),
                                 wxOK | wxCENTER | wxICON_ERROR);
                }
    #endif

                dbgr->SetIsRemoteDebugging(false);

                // Start the debugger
                DebugSessionInfo si;
                si.debuggerPath = dbgname;
                si.exeName = exepath;
                si.cwd = wd;
                si.cmds = cmds;
                si.bpList = bpList;
                si.ttyName = tty;
                si.enablePrettyPrinting = dinfo.enableGDBPrettyPrinting;

                dbgr->Start(si);

                // notify plugins that the debugger just started
                {
                    DebugEvent eventStarted(wxEVT_DEBUG_STARTED);
                    eventStarted.SetClientData(&startup_info);
                    gsgs_Event().ProcessEvent(eventStarted);
                }

                dbgr->Run(bStartedInDebugMode ? GetTheApp()->GetDebuggerArgs() : dlg.GetArguments(), wxEmptyString);

                // Now the debugger has been fed the breakpoints, re-Initialise the breakpt view,
                // so that it uses debugger_ids instead of internal_ids
                gsgs_Frame()->GetDebuggerPane()->GetBreakpointView()->Initialize();

                // Layout management
                gsgs_Manager()->GetPerspectiveManager().SavePerspective(NORMAL_LAYOUT);
                gsgs_Manager()->GetPerspectiveManager().LoadPerspective(DEBUG_LAYOUT);

            } else if(!dbgr && !bStartedInDebugMode) {

                // Fire an event, maybe a plugin wants to process this
                DebugEvent event(wxEVT_DBG_UI_QUICK_DEBUG);
                event.SetDebuggerName(dlg.GetDebuggerName());
                event.SetExecutableName(dlg.GetExe());
                event.SetWorkingDirectory(dlg.GetWorkingDirectory());
                event.SetStartupCommands(wxJoin(dlg.GetStartupCmds(), '\n'));
                event.SetArguments(dlg.GetArguments());
                gsgs_Event().AddPendingEvent(event);
            }
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugCoreDump(wxCommandEvent& e)
    {
        // launch the debugger
        DebugCoreDumpDlg* dlg = new DebugCoreDumpDlg(this);
        if(dlg->ShowModal() == wxID_OK) {
            DebuggerMgr::Get().SetActiveDebugger(dlg->GetDebuggerName());
            IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();

            if(dbgr && !dbgr->IsRunning()) {

                wxString debuggingcommand;
                debuggingcommand << wxT("-c ") << dlg->GetCore() << wxT(" ") << dlg->GetExe();
                wxString wd = dlg->GetWorkingDirectory();

                // update the debugger information
                DebuggerInformation dinfo;
                DebuggerMgr::Get().GetDebuggerInformation(dlg->GetDebuggerName(), dinfo);
                dinfo.breakAtWinMain = false;

                // read the console command
                dinfo.consoleCommand = gsgs_EditorConfig().GetOptions()->GetProgramConsoleCommand();

                wxString dbgname = dinfo.path;
                dbgname = gsgs_EditorConfig().ExpandVariables(dbgname, true);

                // launch the debugger
                dbgr->SetObserver(gsgs_Manager());
                dbgr->SetDebuggerInformation(dinfo);

                DebuggerStartupInfo startup_info;
                startup_info.debugger = dbgr;

                // notify plugins that we're about to start debugging
                {
                    DebugEvent eventStarting(wxEVT_DEBUG_STARTING);
                    eventStarting.SetClientData(&startup_info);
                    if(gsgs_Event().ProcessEvent(eventStarting)) {
                        dlg->Destroy();
                        return;
                    }
                }

                wxString tty;
    #ifndef __WXMSW__
                if(!gsgs_Manager()->StartTTY(DebuggerTerminalPOSIX::MakeCoreTitle(dlg->GetCore()), tty)) {
                    wxMessageBox(_("Could not start TTY console for debugger!"), _("gsgs"),
                                 wxOK | wxCENTER | wxICON_ERROR);
                }
    #endif
                dbgr->SetIsRemoteDebugging(false);

                // The next two are empty, but are required as parameters
                std::vector<BreakpointInfo> bpList;
                wxArrayString cmds;

                DebugSessionInfo si;
                si.debuggerPath = dbgname;
                si.exeName = debuggingcommand;
                si.cwd = wd;
                si.bpList = bpList;
                si.cmds = cmds;
                si.ttyName = tty;
                si.enablePrettyPrinting = dinfo.enableGDBPrettyPrinting;
                dbgr->Start(si);

                // notify plugins that the debugger just started
                {
                    DebugEvent eventStarted(wxEVT_DEBUG_STARTED);
                    eventStarted.SetClientData(&startup_info);
                    gsgs_Event().ProcessEvent(eventStarted);
                }

                // Coredump debugging doesn't use breakpoints, but probably we should do this here anyway...
                gsgs_Frame()->GetDebuggerPane()->GetBreakpointView()->Initialize();

                gsgs_Manager()->GetPerspectiveManager().SavePerspective(NORMAL_LAYOUT);
                gsgs_Manager()->GetPerspectiveManager().LoadPerspective(DEBUG_LAYOUT);

                // Make sure that the debugger pane is visible, and select the stack trace tab
                wxAuiPaneInfo& info = GetAuiManager()->GetPane(wxT("Debugger"));
                if(info.IsOk() && !info.IsShown()) 
                { 
                    gsgs_Manager()->ShowDebuggerPane(); 
                }

                gsgs_Frame()->GetDebuggerPane()->SelectTab(DebuggerPane::FRAMES);
                gsgs_Manager()->UpdateDebuggerPane();

                // Finally, get the call-stack and 'continue' gdb (which seems to be necessary for things to work...)
                dbgr->ListFrames();
                dbgr->Continue();

            } else if(!dbgr) {
                DebugEvent event(wxEVT_DBG_UI_CORE_FILE);
                event.SetDebuggerName(dlg->GetDebuggerName());
                event.SetExecutableName(dlg->GetExe());
                event.SetCoreFile(dlg->GetCore());
                event.SetWorkingDirectory(dlg->GetWorkingDirectory());
                gsgs_Event().AddPendingEvent(event);
            }
        }
        dlg->Destroy();
    }
    //-----------------------------------------------------------------
    void Frame::OnQuickDebugUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        e.Enable(true);
    }
    //-----------------------------------------------------------------
    void Frame::OnShowWhitespaceUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        if(e.GetId() == XRCID("whitepsace_invisible")) {
            e.Check(options->GetShowWhitspaces() == 0);
        } else if(e.GetId() == XRCID("whitepsace_always")) {
            e.Check(options->GetShowWhitspaces() == 1);
        } else if(e.GetId() == XRCID("whitespace_visiable_after_indent")) {
            e.Check(options->GetShowWhitspaces() == 2);
        } else if(e.GetId() == XRCID("whitespace_indent_only")) {
            e.Check(options->GetShowWhitspaces() == 3);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnShowWhitespace(wxCommandEvent& e)
    {
        OptionsConfig * options = gsgs_EditorConfig().GetOptions();
        if(e.GetId() == XRCID("whitepsace_invisible")) {
            options->SetShowWhitspaces(0);
        } else if(e.GetId() == XRCID("whitepsace_always")) {
            options->SetShowWhitspaces(1);
        } else if(e.GetId() == XRCID("whitespace_visiable_after_indent")) {
            options->SetShowWhitspaces(2);
        } else if(e.GetId() == XRCID("whitespace_indent_only")) {
            options->SetShowWhitspaces(3);
        }

        GetMainBook()->ShowWhitespace(options->GetShowWhitspaces());

        // save the settings
        gsgs_EditorConfig().SetOptions(options);
    }
    //-----------------------------------------------------------------
    void Frame::OnNextTab(wxCommandEvent& e)
    {
        int idx = GetMainBook()->GetCurrentPageIndex();

        if(idx != wxNOT_FOUND) {
            TabVec_t tabs;
            GetMainBook()->GetAllTabs(tabs);

            idx = (idx + 1) % tabs.size();
            GetMainBook()->SelectPage(GetMainBook()->GetPage(idx));
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnPrevTab(wxCommandEvent& e)
    {
        int idx = GetMainBook()->GetCurrentPageIndex();

        if(idx != wxNOT_FOUND) {
            idx--;

            if(idx < 0) {
                TabVec_t tabs;
                GetMainBook()->GetAllTabs(tabs);

                idx = tabs.size() - 1;
            }
            GetMainBook()->SelectPage(GetMainBook()->GetPage(idx));
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnNextPrevTab_UI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor) {
            e.Enable(true);
        }
        e.Enable(false);
    }
    //-----------------------------------------------------------------
    void Frame::OnIncrementalSearch(wxCommandEvent&)
    {
        GetMainBook()->ShowQuickBar(true);
    }
    //-----------------------------------------------------------------
    void Frame::OnIncrementalReplace(wxCommandEvent&)
    {
        GetMainBook()->ShowQuickBar(true);
    }
    //-----------------------------------------------------------------
    void Frame::OnRetagWorkspace(wxCommandEvent& event)
    {
        if(gsgs_Manager()->GetRetagInProgress()) {
            gsgsDEBUG() << "A workspace parsing is already taking place, request is ignored";
            return;
        }

        // See if any of the plugins want to handle this event by itself
        bool fullRetag = !(event.GetId() == XRCID("retag_workspace"));
        wxCommandEvent e(fullRetag ? wxEVT_CMD_RETAG_WORKSPACE_FULL : wxEVT_CMD_RETAG_WORKSPACE, GetId());
        e.SetEventObject(this);
        if(gsgs_Event().ProcessEvent(e)) return;

        // Update the parser paths with the global ones
        gsgs_Manager()->UpdateParserPaths(false);

        TagsManager::RetagType type = TagsManager::Retag_Quick_No_Scan;
        if(event.GetId() == XRCID("retag_workspace"))
            type = TagsManager::Retag_Quick;

        else if(event.GetId() == XRCID("full_retag_workspace"))
            type = TagsManager::Retag_Full;

        else if(event.GetId() == XRCID("retag_workspace_no_includes"))
            type = TagsManager::Retag_Quick_No_Scan;
        gsgs_Manager()->RetagWorkspace(type);
    }
    //-----------------------------------------------------------------
    void Frame::OnShowFullScreen(wxCommandEvent& e)
    {
        wxUnusedVar(e);

        if(IsFullScreen()) {
            ShowFullScreen(false);
        } else {
            ShowFullScreen(true, wxFULLSCREEN_NOCAPTION | wxFULLSCREEN_NOBORDER);
            // Re-apply the menu accelerators
            gsgs_Manager()->UpdateMenuAccelerators();
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnReloadExternallModified(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        ReloadExternallyModifiedProjectFiles();
        GetMainBook()->ReloadExternallyModified(true);
    }
    //-----------------------------------------------------------------
    void Frame::OnReloadExternallModifiedNoPrompt(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        GetMainBook()->ReloadExternallyModified(false);
    }
    //-----------------------------------------------------------------
    bool Frame::ReloadExternallyModifiedProjectFiles()
    {
        if(gsgs_Workspace().IsOpen() == false) { return false; }

        CxxWS* workspace = &gsgs_CppWorker();
        bool workspace_modified = false, project_modified = false;

        // check if the workspace needs reloading and ask the user for confirmation
        // if it does
        if(workspace->GetWorkspaceLastModifiedTime() < workspace->GetFileLastModifiedTime()) {
            // always update last modification time: if the user chooses to reload it
            // will not matter, and it avoids the program prompting the user repeatedly
            // if he chooses not to reload the workspace
            workspace->SetWorkspaceLastModifiedTime(workspace->GetFileLastModifiedTime());
            workspace_modified = true;
        }

        // check if any of the projects in the workspace needs reloading
        wxArrayString projects;
        workspace->GetProjectList(projects);

        for(size_t i = 0; i < projects.GetCount(); ++i) {
            wxString errStr;
            ProjectPtr proj = workspace->FindProjectByName(projects[i], errStr);

            if(proj->GetProjectLastModifiedTime() < proj->GetFileLastModifiedTime()) {
                // always update last modification time: if the user chooses to reload it
                // will not matter, and it avoids the program prompting the user repeatedly
                // if he chooses not to reload some of the projects
                proj->SetProjectLastModifiedTime(proj->GetFileLastModifiedTime());
                project_modified = true;
            }
        }

        if(!project_modified && !workspace_modified) return false;

        // Make sure we don't have the mouse captured in any editor or we might get a crash somewhere
        wxStandardID res =
            ::PromptMessageDialogWithoutCancel(_("Workspace or project settings have been modified outside "
                                                 "of gsgs\nWould you like to reload the workspace?"),
                                               "ReloadWorkspaceWhenAltered", _("Reload workspace"), _("Not now"));
        // Don't do anything if "X" is pressed
        if(res != wxID_CANCEL) {
            if(res == wxID_YES) {
                wxCommandEvent evtReload(wxEVT_COMMAND_MENU_SELECTED, XRCID("reload_workspace"));
                GetEventHandler()->ProcessEvent(evtReload);

            } else {
                // user cancelled the dialog or chosed not to reload the workspace
                if(GetMainBook()->GetActiveEditor()) { GetMainBook()->GetActiveEditor()->CallAfter(&Editor::SetActive); }
            }
        }
        return true;
    }
    //-----------------------------------------------------------------
    bool Frame::SaveLayoutAndSession()
    {
        // save the current session before closing
        // We do this before 'CloseAll' so the session will
        // store the list of tabs

        // Let the plugin process this first
        Event eventSaveSession(wxEVT_SAVE_SESSION_NEEDED);
        if(!gsgs_Event().ProcessEvent(eventSaveSession)) {
            // Do the default session store
            if(gsgs_Workspace().IsOpen()) {
                wxString sessionName = gsgs_CppWorker().GetFileName().GetFullPath();
                SessionEntry session;
                session.SetWorkspaceName(sessionName);
                GetMainBook()->SaveSession(session);
                gsgs_Manager()->GetBreakpointsMgr()->SaveSession(session);
                gsgs_Session().Save(sessionName, session);
                gsgs_Session().SetLastSession(sessionName);
            } else {
                // Create a default session
                wxString sessionName("Default");
                SessionEntry session;
                session.SetWorkspaceName(sessionName);
                GetMainBook()->SaveSession(session);
                gsgs_Manager()->GetBreakpointsMgr()->SaveSession(session);
                gsgs_Session().Save(sessionName, session);
                gsgs_Session().SetLastSession(sessionName);
            }
        }

        // make sure there are no 'unsaved documents'
        if(!GetMainBook()->CloseAll(true)) { return false; }

        // save general information
        if(IsMaximized()) {
            mFrameInfo->SetFrameSize(wxSize(800, 600));
        } else {
            mFrameInfo->SetFrameSize(this->GetSize());
        }
    #ifdef __WXMAC__
        mFrameInfo->SetFramePosition(wxPoint(50, 50));
    #else
        mFrameInfo->SetFramePosition(this->GetScreenPosition());
    #endif

        gsgs_EditorConfig().Begin();

        SetFrameFlag(IsMaximized(), GSGS_MAXIMIZE_FRAME);
        gsgs_EditorConfig().WriteObject(wxT("GeneralInfo"), mFrameInfo);
        gsgs_EditorConfig().SetInteger(wxT("ShowNavBar"), GetMainBook()->IsNavBarShown() ? 1 : 0);
        GetWorkspacePane()->SaveWorkspaceViewTabOrder();
        GetOutputPane()->SaveTabOrder();

        // keep list of all detached panes
        wxArrayString panes;
        mWinMag->getFloatingList(panes);
        DetachedPanesInfo dpi(panes);
        gsgs_EditorConfig().WriteObject(wxT("DetachedPanesList"), &dpi);

        // Update the current perspective as the "NORMAL" one
        gsgs_Manager()->GetPerspectiveManager().SavePerspective(NORMAL_LAYOUT);

        // save the notebooks styles
        gsgs_EditorConfig().SetInteger(wxT("gsgsBook"), GetMainBook()->GetBookStyle());
        gsgs_EditorConfig().Save();
        return true;
    }
    //-----------------------------------------------------------------
    void Frame::SaveGeneralSettings() 
    { 
        gsgs_EditorConfig().WriteObject(wxT("GeneralInfo"), mFrameInfo); 
    }
    //-----------------------------------------------------------------
    void Frame::OnNextFiFMatch(wxCommandEvent &)
    {
        GetOutputPane()->GetFindResultsTab()->NextMatch();
    }
    //-----------------------------------------------------------------
    void Frame::OnPreviousFiFMatch(wxCommandEvent &)
    {
        GetOutputPane()->GetFindResultsTab()->PrevMatch();
    }
    //-----------------------------------------------------------------
    void Frame::OnNextFiFMatchUI(wxUpdateUIEvent& e) { CHECK_SHUTDOWN(); }
    //-----------------------------------------------------------------
    void Frame::OnPreviousFiFMatchUI(wxUpdateUIEvent& e) { CHECK_SHUTDOWN(); }
    //-----------------------------------------------------------------
    void Frame::OnFindResourceXXX(wxCommandEvent& e)
    {
        // Determine the search type
        // Let the plugins a chance before we handle this event
        wxCommandEvent eventOpenResource(wxEVT_CMD_OPEN_RESOURCE, GetId());
        eventOpenResource.SetEventObject(this);
        if(gsgs_Event().ProcessEvent(eventOpenResource)) return;

        wxString initialText;
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor && editor->HasSelection()) {
            int start = editor->GetSelectionNStart(0);
            int end = editor->GetSelectionNEnd(0);
            initialText = editor->GetTextRange(start, end);
        }
        OpenResourceDialog dlg(this, PluginManager::Get(), initialText);

        if(dlg.ShowModal() == wxID_OK && !dlg.GetSelections().empty()) {
            std::vector<OpenResourceDialogItemData*> items = dlg.GetSelections();
            std::for_each(items.begin(), items.end(), [&](OpenResourceDialogItemData* item) {
                OpenResourceDialog::OpenSelection(*item, PluginManager::Get());
            });
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnParsingThreadMessage(wxCommandEvent& e)
    {
        wxString* msg = (wxString*)e.GetClientData();
        if(msg) {
            gsgsLogMessage(*msg);
            delete msg;
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnDatabaseUpgradeInternally(wxCommandEvent& e)
    {
        wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("full_retag_workspace"));
        this->AddPendingEvent(evt);
    }
    //-----------------------------------------------------------------
    // Due to differnet schema versions, the database was truncated by the
    // TagsManager, prompt the user
    void Frame::OnDatabaseUpgrade(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        // turn ON the retag-required flag and perform it in the OnTimer() callback
        // this is to allow gsgs to update paths if needed *before* the retag is taking place
        // (CC paths will be updated if needed in the OnTimer() callback)
        m_workspaceRetagIsRequired = true;
    }
    //-----------------------------------------------------------------
    void Frame::OnCheckForUpdate(wxCommandEvent& e)
    {
        if(!m_webUpdate) {
            m_webUpdate = new WebUpdateJob(this, true, gsgs_Config().Read("PromptForNewReleaseOnly", false));
            m_webUpdate->Check();
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnShowActiveProjectSettings(wxCommandEvent&)
    {
        if (gsgs_CppWorker().IsOpen())
        {
            GetWorkspaceTab()->OpenProjectSettings();
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnShowActiveProjectSettingsUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        wxArrayString projectList;
        gsgs_CppWorker().GetProjectList(projectList);
        e.Enable(gsgs_Workspace().IsOpen() && (projectList.IsEmpty() == false));
    }
    //-----------------------------------------------------------------
    void Frame::OnLoadPerspective(wxCommandEvent& e)
    {
        wxString file;
        file << gsgs_Path().GetUserDataDir() << wxT("/config/gsgs.layout");

        wxFileName oldLayoutFile(file);
        if(oldLayoutFile.FileExists(file)) {
            gsgsRemoveFile(oldLayoutFile.GetFullPath());
            wxCommandEvent eventRestoreLayout(wxEVT_COMMAND_MENU_SELECTED, XRCID("restore_layout"));
            eventRestoreLayout.SetEventObject(this);
            GetEventHandler()->ProcessEvent(eventRestoreLayout);

        } else {
            gsgs_Manager()->GetPerspectiveManager().LoadPerspective(NORMAL_LAYOUT);
            // Update the current perspective
            if(gsgs_Manager()->GetPerspectiveManager().IsDefaultActive()) {
                gsgs_Manager()->GetPerspectiveManager().SavePerspective();
            }
        }
    }
    //-----------------------------------------------------------------
    void Frame::SelectBestEnvSet()
    {
        // Set the workspace's environment variable set to the active one
        wxString projectSetName;
        wxString projectDbgSetName;

        // First, if the project has an environment which is not '<Use Defaults>' use it
        wxString workspaceSetName;
        if(gsgs_Workspace().IsOpen())
        {
            wxString activeProj = gsgs_CppWorker().GetActiveProjectName();
            ProjectPtr p = gsgs_Manager()->GetProject(activeProj);
            if(p) {
                BuildConfigPtr buildConf = gsgs_CppWorker().GetProjBuildConf(activeProj, wxEmptyString);
                if(buildConf) {
                    if(buildConf->GetEnvVarSet() != USE_WORKSPACE_ENV_VAR_SET &&
                       buildConf->GetEnvVarSet() != wxT("<Use Workspace Settings>") /* backward support */) {
                        projectSetName = buildConf->GetEnvVarSet();
                    }

                    if(buildConf->GetDbgEnvSet() != USE_GLOBAL_SETTINGS) { projectDbgSetName = buildConf->GetDbgEnvSet(); }
                }
            }
            workspaceSetName = gsgs_CppWorker().GetLocalWorkspace()->GetActiveEnvironmentSet();
        }

        wxString globalActiveSet = wxT("Default");
        wxString activeSetName;
        EvnVarList vars;
        gsgs_EditorConfig().ReadObject(wxT("Variables"), &vars);

        // By default, use the global one
        activeSetName = globalActiveSet;

        if(!projectSetName.IsEmpty() && vars.IsSetExist(projectSetName)) 
        {
            activeSetName = projectSetName;
        } 
        else if(!workspaceSetName.IsEmpty() && vars.IsSetExist(workspaceSetName)) 
        {
            activeSetName = workspaceSetName;
        }

        vars.SetActiveSet(activeSetName);
        gsgs_EditorConfig().WriteObject(wxT("Variables"), &vars, false);

        ///////////////////////////////////////////////////
        // Select the debugger PreDefined Types settings
        ///////////////////////////////////////////////////
        DebuggerSettingsPreDefMap preDefTypeMap;
        gsgs_Config().Read(wxT("DebuggerCommands"), &preDefTypeMap);

        wxString dbgSetName = wxT("Default");
        if(!projectDbgSetName.IsEmpty() && preDefTypeMap.IsSetExist(projectDbgSetName)) 
        { 
            dbgSetName = projectDbgSetName; 
        }

        preDefTypeMap.SetActive(dbgSetName);
        gsgs_Config().Write(wxT("DebuggerCommands"), &preDefTypeMap);
    }
    //-----------------------------------------------------------------
    void Frame::OnClearTagsCache(wxCommandEvent& e)
    {
        e.Skip();
        TagsManagerST::Get()->ClearTagsCache();
        GetMainStatusBar()->SetMessage(_("Tags cache cleared"));
    }
    //-----------------------------------------------------------------
    void Frame::OnUpdateNumberOfBuildProcesses(wxCommandEvent& e)
    {
        int cpus = wxThread::GetCPUCount();
        BuilderConfigPtr bs = gsgs_Settings().GetBuilderConfig("Default");
        if(bs && cpus != wxNOT_FOUND) {
            wxString jobs;
            jobs << cpus;

            bs->SetToolJobs(jobs);
            gsgs_Settings().SetBuildSystem(bs);
            gsgsLogMessage(wxT("Info: setting number of concurrent builder jobs to ") + jobs);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnWorkspaceEditorPreferences(wxCommandEvent& e)
    {
        GetWorkspaceTab()->GetFileView()->GetEventHandler()->ProcessEvent(e);
    }
    //-----------------------------------------------------------------
    void Frame::OnWorkspaceSettings(wxCommandEvent& e)
    {
        GetWorkspaceTab()->GetFileView()->GetEventHandler()->ProcessEvent(e);
    }
    //-----------------------------------------------------------------
    void Frame::OnRestoreDefaultLayout(wxCommandEvent& e)
    {
        e.Skip();
    #ifndef __WXMAC__
        WindowUpdateLocker locker(this);
    #endif

        gsgsLogMessage(wxT("Restoring layout"));

        // Close all docking panes
        wxAuiPaneInfoArray& panes = mWinMag->GetAllPanes();

        for(size_t i = 0; i < panes.GetCount(); i++) 
        {
            // make sure that the caption is visible
            panes.Item(i).CaptionVisible(true);
            wxAuiPaneInfo& p = panes.Item(i);

            if(p.window) 
            {
                DockablePane* d = dynamic_cast<DockablePane*>(p.window);
                if(d) 
                {
                    wxCommandEvent evt(wxEVT_COMMAND_MENU_SELECTED, XRCID("close_pane"));
                    p.window->GetEventHandler()->AddPendingEvent(evt);
                }
            }
        }

        gsgs_Manager()->GetPerspectiveManager().DeleteAllPerspectives();

        mWinMag->LoadPerspective(m_defaultLayout, false);
        UpdateAUI();
        mWinMag->UpdateAccels();
        // Save the current layout as the 'Default' layout
        gsgs_Manager()->GetPerspectiveManager().SavePerspective(NORMAL_LAYOUT);
    }
    //-----------------------------------------------------------------
    void Frame::UpdateAUI()
    {
        unsigned int aflag = wxAUI_MGR_ALLOW_FLOATING /*| wxAUI_MGR_LIVE_RESIZE*/;
        int dockingStyle = gsgs_EditorConfig().GetOptions()->GetDockingStyle();
        switch(dockingStyle) {
        case 0:
            aflag |= wxAUI_MGR_TRANSPARENT_HINT;
            break;
        case 1:
            aflag |= wxAUI_MGR_RECTANGLE_HINT;
            break;
        case 2:
            aflag |= wxAUI_MGR_VENETIAN_BLINDS_HINT;
            break;
        }
        mWinMag->SetFlags(aflag);
    }
    //-----------------------------------------------------------------
    void Frame::OnRetaggingCompelted(wxCommandEvent& e)
    {
        e.Skip();

        // Generate compile_commands.json file
        gsgs_Manager()->GenerateCompileCommands();

        GetMainStatusBar()->SetMessage(_("Done"));
        GetWorkspacePane()->ClearProgress();

        // Clear all cached tags now that we got our database updated
        TagsManagerST::Get()->ClearAllCaches();

        // Send event notifying parsing completed
        std::vector<std::string>* files = (std::vector<std::string>*)e.GetClientData();
        if(files) {

            // Print the parsing end time
            gsgsDEBUG() << "INFO: Retag workspace completed in" << (gStopWatch.Time() / 1000) << "seconds ("
                      << (unsigned long)files->size() << "files were scanned)";
            std::vector<wxFileName> taggedFiles;
            for(size_t i = 0; i < files->size(); i++) {
                taggedFiles.push_back(wxFileName(wxString(files->at(i).c_str(), wxConvUTF8)));
            }

            SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&taggedFiles);
            delete files;

        } else {
            gsgsLogMessage(_("INFO: Retag workspace completed in 0 seconds (No files were retagged)"));
        }

        wxCommandEvent tagEndEvent(wxEVT_CMD_RETAG_COMPLETED);
        tagEndEvent.SetClientData(e.GetClientData()); // pass the pointer to the original caller
        gsgs_Event().AddPendingEvent(tagEndEvent);
    }
    //-----------------------------------------------------------------
    void Frame::OnRetaggingProgress(wxCommandEvent& e)
    {
        e.Skip();
        if(e.GetInt() == 1) 
        {
            gStopWatch.Start();
        }
        GetWorkspacePane()->UpdateProgress(e.GetInt());
    }
    //-----------------------------------------------------------------
    void Frame::OnRetagWorkspaceUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();

        // See whether we got a custom workspace open in one of the plugins
        Event e(wxEVT_CMD_IS_WORKSPACE_OPEN, GetId());
        e.SetEventObject(this);
        e.SetAnswer(false);
        gsgs_Event().ProcessEvent(e);

        event.Enable((gsgs_Workspace().IsOpen() && !gsgs_Manager()->GetRetagInProgress()) || e.IsAnswer());
    }
    //-----------------------------------------------------------------
    void Frame::OnViewWordWrap(wxCommandEvent& e)
    {
        CHECK_SHUTDOWN();

        OptionsConfig * opts = gsgs_EditorConfig().GetOptions();
        opts->SetWordWrap(e.IsChecked());
        gsgs_EditorConfig().SetOptions(opts);

        GetMainBook()->SetViewWordWrap(e.IsChecked());
    }
    //-----------------------------------------------------------------
    void Frame::OnViewWordWrapUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        bool hasEditor = GetMainBook()->GetActiveEditor() ? true : false;
        if(!hasEditor) {
            e.Enable(false);
            return;
        }

        OptionsConfig * opts = gsgs_EditorConfig().GetOptions();
        e.Enable(true);
        e.Check(opts->GetWordWrap());
    }
    //-----------------------------------------------------------------
    void Frame::OnGrepWord(wxCommandEvent& e)
    {
        CHECK_SHUTDOWN();
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(!editor || editor->GetSelectedText().IsEmpty()) return;

        // Prepare the search data
        bool singleFileSearch(true);
        if(e.GetId() == XRCID("grep_current_workspace")) singleFileSearch = false;

        SearchData data;
        data.SetFindString(editor->GetSelectedText());
        data.SetMatchCase(true);
        data.SetMatchWholeWord(true);
        data.SetRegularExpression(false);
        data.SetDisplayScope(false);
        data.SetEncoding(wxFontMapper::GetEncodingName(editor->GetOptions()->GetFileFontEncoding()));
        data.SetSkipComments(false);
        data.SetSkipStrings(false);
        data.SetColourComments(false);

        wxArrayString files;
        wxArrayString rootDirs;
        wxString mask;
        if(singleFileSearch) {
            rootDirs.Add(wxGetTranslation(SEARCH_IN_CURRENT_FILE));
            files.Add(editor->GetFileName().GetFullPath());
            mask << editor->GetFileName().GetFullName(); // this will ensure that this file is scanned

        } else {
            rootDirs.Add(wxGetTranslation(SEARCH_IN_WORKSPACE_FOLDER));
            gsgs_Manager()->GetWorkspaceFiles(files);
            gsgsStringSet masks;
            // Build a mask that matches the workspace content
            std::for_each(files.begin(), files.end(), [&](const wxString& filename) {
                wxFileName fn(filename);
                wxString curfileMask = fn.GetExt();
                if(fn.GetExt().IsEmpty()) {
                    curfileMask = "*";
                } else {
                    curfileMask = "*." + fn.GetExt();
                }

                if(masks.count(curfileMask) == 0) {
                    masks.insert(curfileMask);
                    mask << curfileMask << ";";
                }
            });
        }

        data.SetRootDirs(rootDirs);
        data.SetFiles(files);
        data.UseNewTab(true);
        data.SetOwner(GetOutputPane()->GetFindResultsTab());
        data.SetExtensions(mask);
        SearchThreadST::Get()->PerformSearch(data);
    }
    //-----------------------------------------------------------------
    void Frame::OnGrepWordUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(e.GetId() == XRCID("grep_current_workspace")) {
            // grep in workspace
            e.Enable(gsgs_Workspace().IsValid() && editor && !editor->GetSelectedText().IsEmpty());
        } else {
            // grep in file
            e.Enable(editor && !editor->GetSelectedText().IsEmpty());
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnWebSearchSelection(wxCommandEvent& e)
    {
        CHECK_SHUTDOWN();

        const auto editor = GetMainBook()->GetActiveEditor();
        if(!editor) { return; }

        const auto text = editor->GetSelectedText();
        if(text.IsEmpty()) { return; }

        const auto options = gsgs_EditorConfig().GetOptions();
        if(options) { wxLaunchDefaultBrowser(wxString(options->GetWebSearchPrefix()) << text, wxBROWSER_NOBUSYCURSOR); }
    }
    //-----------------------------------------------------------------
    void Frame::OnWebSearchSelectionUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();

        const auto editor = GetMainBook()->GetActiveEditor();
        e.Enable(editor && !editor->GetSelectedText().IsEmpty());
    }
    //-----------------------------------------------------------------
    //void Frame::OnPchCacheEnded(wxCommandEvent& e) { e.Skip(); }
    //-----------------------------------------------------------------
    //void Frame::OnPchCacheStarted(wxCommandEvent& e) { e.Skip(); }
    //-----------------------------------------------------------------
    /*void Frame::DoEnableWorkspaceViewFlag(bool enable, int flag)
    {
        long flags = View_Show_Default;
        flags = gsgs_EditorConfig().GetInteger(wxT("view_workspace_view"), flags);
        if(enable) {
            flags |= flag;
        } else {
            flags &= ~flag;
        }
        gsgs_EditorConfig().SetInteger(wxT("view_workspace_view"), flags);
    }*/
    //-----------------------------------------------------------------
    /*bool Frame::IsWorkspaceViewFlagEnabled(int flag)
    {
        long flags = View_Show_Default;
        flags = gsgs_EditorConfig().GetInteger(wxT("view_workspace_view"), flags);
        return (flags & flag);
    }*/
    //-----------------------------------------------------------------
    void Frame::OnChangePerspective(wxCommandEvent& e)
    {
        gsgs_Manager()->GetPerspectiveManager().LoadPerspectiveByMenuId(e.GetId());
    }
    //-----------------------------------------------------------------
    void Frame::OnManagePerspectives(wxCommandEvent& e)
    {
        ManagePerspectivesDlg dlg(this);
        dlg.ShowModal();
        OnRefreshPerspectiveMenu(e);
    }
    //-----------------------------------------------------------------
    void Frame::OnSaveLayoutAsPerspective(wxCommandEvent& e)
    {
        SavePerspectiveDlg dlg(this);
        if(dlg.ShowModal() == wxID_OK && !dlg.GetPerspectiveName().IsEmpty()) {
            gsgs_Manager()->GetPerspectiveManager().SavePerspective(dlg.GetPerspectiveName());
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnRefreshPerspectiveMenu(wxCommandEvent& e) 
    {
        // Locate the "perspective_menu"
        wxMenu* menu = NULL;
        GetMenuBar()->FindItem(XRCID("manage_perspectives"), &menu);
        if(!menu) { return; }

        std::vector<int> menuItemIds;
        const wxMenuItemList& items = menu->GetMenuItems();
        wxMenuItemList::const_iterator iter = items.begin();
        for(; iter != items.end(); iter++) {
            wxMenuItem* menuItem = *iter;
            if(menuItem->GetId() == wxID_SEPARATOR || menuItem->GetId() == XRCID("save_current_layout") ||
               menuItem->GetId() == XRCID("manage_perspectives") || menuItem->GetId() == XRCID("restore_layout"))
                continue;
            menuItemIds.push_back(menuItem->GetId());
        }

        for(size_t i = 0; i < menuItemIds.size(); i++) {
            menu->Delete(menuItemIds.at(i));
        }

        wxArrayString perspectives = gsgs_Manager()->GetPerspectiveManager().GetAllPerspectives();
        for(size_t i = 0; i < perspectives.GetCount(); i++) {
            wxString name = perspectives.Item(i);
            menu->Prepend(gsgs_Manager()->GetPerspectiveManager().MenuIdFromName(name), name, wxT(""), true);
        }        
    }
    //-----------------------------------------------------------------
    void Frame::OnChangePerspectiveUI(wxUpdateUIEvent& e)
    {
        wxString active = gsgs_Manager()->GetPerspectiveManager().GetActive();
        wxString itemName = gsgs_Manager()->GetPerspectiveManager().NameFromMenuId(e.GetId());

        e.Check(active.CmpNoCase(itemName) == 0);
    }
    //-----------------------------------------------------------------
    void Frame::OnParserThreadReady(wxCommandEvent& e)
    {
        e.Skip();
        gsgs_Manager()->SetRetagInProgress(false);

        if(gsgs_Manager()->IsShutdownInProgress()) {
            // we are in shutdown progress, dont do anything
            return;
        }

        GetMainStatusBar()->SetMessage(wxEmptyString);

        if(e.GetInt() == ParseRequest::PR_SUGGEST_HIGHLIGHT_WORDS)
            // no need to trigger another UpdateColour
            return;

        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor) { editor->UpdateColours(); }
    }
    //-----------------------------------------------------------------
    void Frame::OnFileSaveUI(wxUpdateUIEvent & event) 
    { 
        //event.Skip();
        bool hasModifiedEditor = false;
        TabVec_t tabs;
        GetMainBook()->GetAllTabs(tabs);
        GetMainBook()->GetDetachedTabs(tabs);

        if(tabs.size())
            event.Enable(true); 
        else
            event.Enable(false);
    }
    //-----------------------------------------------------------------
    void Frame::OnActivateEditor(wxCommandEvent & e)
    {
        Editor * editor = dynamic_cast<Editor *>(e.GetEventObject());
        if(editor) editor->SetActive();
    }
    //-----------------------------------------------------------------
    void Frame::OnActiveEditorChanged(wxCommandEvent& e)
    {
        e.Skip();
        SelectBestEnvSet(); // Updates the statusbar bookmark display
    }
    //-----------------------------------------------------------------
    void Frame::OnLoadSession(wxCommandEvent& e)
    {
        wxUnusedVar(e);
        LoadSession(gsgs_Session().GetLastSession());
    }
    //-----------------------------------------------------------------
    void Frame::OnShowBuildMenu(wxCommandEvent& e)
    {
        // Show the build menu
        ToolBar* toolbar = dynamic_cast<ToolBar*>(e.GetEventObject());
        CHECK_PTR_RET(toolbar);
        wxMenu menu;
        if (gsgs_CppWorker().IsOpen()) {
            menu.Append(XRCID("build_active_project_only"), wxT("Project Only - Build"));
            menu.Append(XRCID("clean_active_project_only"), wxT("Project Only - Clean"));

            // build the menu and show it
            BuildConfigPtr bldcfg =
                gsgs_CppWorker().GetProjBuildConf(gsgs_CppWorker().GetActiveProjectName(), "");
            if (bldcfg && bldcfg->IsCustomBuild()) {

                // Update teh custom targets
                BuildSettingsConfig::singleton().SetTargets(gsgs_CppWorker().GetActiveProjectName(),
                    bldcfg->GetCustomTargets());

                if (!BuildSettingsConfig::singleton().GetTargets().empty()) { menu.AppendSeparator(); }

                const BuildSettingsConfig::Map_t& targets = BuildSettingsConfig::singleton().GetTargets();
                BuildSettingsConfig::Map_t::const_iterator iter = targets.begin();
                for (; iter != targets.end(); ++iter) {
                    int winid = iter->first; // contains the menu ID
                    menu.Append(winid, iter->second.first);
                }
            }
        }
        toolbar->ShowMenuForButton(XRCID("build_active_project"), &menu);
    }
    //-----------------------------------------------------------------
    void Frame::OnWorkspaceClosed(wxCommandEvent& e)
    {
        e.Skip();
        BuildSettingsConfig::singleton().Clear();

    #ifndef __WXMSW__
    #if wxVERSION_NUMBER >= 2900
        // This is needed in >=wxGTK-2.9, otherwise the current editor sometimes doesn't notice that the output pane has
        // appeared
        // resulting in an area at the bottom that can't be scrolled to
        gsgs_Frame()->SendSizeEvent(wxSEND_EVENT_POST);
    #endif
    #endif
    }
    //-----------------------------------------------------------------
    void Frame::OnIncrementalSearchUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        event.Enable(GetMainBook()->GetCurrentPage() != NULL);
    }
    //-----------------------------------------------------------------
    void Frame::OnBuildProjectOnly(wxCommandEvent& event)
    {
        wxCommandEvent e(wxEVT_CMD_BUILD_PROJECT_ONLY);
        gsgs_Event().AddPendingEvent(e);
    }
    //-----------------------------------------------------------------
    void Frame::OnCleanProjectOnly(wxCommandEvent& event)
    {
        wxCommandEvent e(wxEVT_CMD_CLEAN_PROJECT_ONLY);
        gsgs_Event().AddPendingEvent(e);
    }
    //-----------------------------------------------------------------
    void Frame::OnFileSaveAllUI(wxUpdateUIEvent& event)
    {
        bool hasModifiedEditor = false;
        TabVec_t tabs;
        GetMainBook()->GetAllTabs(tabs);
        GetMainBook()->GetDetachedTabs(tabs);

        for(size_t i = 0; i < tabs.size(); ++i) {
            if(tabs.at(i).isFile && tabs.at(i).isModified) {
                hasModifiedEditor = true;
                break;

            } else if(!tabs.at(i).isFile) {
                // Send an event
                Event modifyEvent(wxEVT_PAGE_MODIFIED_UPDATE_UI);
                modifyEvent.SetClientData(tabs.at(i).window);
                if(gsgs_Event().ProcessEvent(modifyEvent) && modifyEvent.IsAnswer()) {
                    hasModifiedEditor = true;
                    break;
                }
            }
        }

        event.Enable(hasModifiedEditor);
    }
    //-----------------------------------------------------------------
    void Frame::OnShowDebuggerWindow(wxCommandEvent& e)
    {
        // load the debugger configuration
        Config conf("debugger-view.conf");
        DebuggerPaneConfig item;
        conf.ReadItem(&item);

        bool show = e.IsChecked();
        if(e.GetId() == XRCID("debugger_win_locals"))
            item.ShowDebuggerWindow(DebuggerPaneConfig::Locals, show);

        else if(e.GetId() == XRCID("debugger_win_watches"))
            item.ShowDebuggerWindow(DebuggerPaneConfig::Watches, show);

        else if(e.GetId() == XRCID("debugger_win_output"))
            item.ShowDebuggerWindow(DebuggerPaneConfig::Output, show);

        else if(e.GetId() == XRCID("debugger_win_threads"))
            item.ShowDebuggerWindow(DebuggerPaneConfig::Thread, show);

        else if(e.GetId() == XRCID("debugger_win_callstack"))
            item.ShowDebuggerWindow(DebuggerPaneConfig::Callstack, show);

        else if(e.GetId() == XRCID("debugger_win_memory"))
            item.ShowDebuggerWindow(DebuggerPaneConfig::Memory, show);

        else if(e.GetId() == XRCID("debugger_win_breakpoints"))
            item.ShowDebuggerWindow(DebuggerPaneConfig::Breakpoint, show);

        else if(e.GetId() == XRCID("debugger_win_asciiview"))
            item.ShowDebuggerWindow(DebuggerPaneConfig::AsciiViewer, show);

        else if(e.GetId() == XRCID("debugger_win_disassemble"))
            item.ShowDebuggerWindow(DebuggerPaneConfig::Disassemble, show);

        conf.WriteItem(&item);
        // Reload the perspective
        gsgs_Manager()->GetPerspectiveManager().LoadPerspective();
    }
    //-----------------------------------------------------------------
    void Frame::OnShowDebuggerWindowUI(wxUpdateUIEvent& e)
    {
        // load the debugger configuration
        // Config conf("debugger-view.conf");
        DebuggerPaneConfig item;
        // conf.ReadItem( &item );

        DebuggerPaneConfig::eDebuggerWindows winid = DebuggerPaneConfig::None;

        if(e.GetId() == XRCID("debugger_win_locals"))
            winid = DebuggerPaneConfig::Locals;

        else if(e.GetId() == XRCID("debugger_win_watches"))
            winid = DebuggerPaneConfig::Watches;

        else if(e.GetId() == XRCID("debugger_win_output"))
            winid = DebuggerPaneConfig::Output;

        else if(e.GetId() == XRCID("debugger_win_threads"))
            winid = DebuggerPaneConfig::Thread;

        else if(e.GetId() == XRCID("debugger_win_callstack"))
            winid = DebuggerPaneConfig::Callstack;

        else if(e.GetId() == XRCID("debugger_win_memory"))
            winid = DebuggerPaneConfig::Memory;

        else if(e.GetId() == XRCID("debugger_win_breakpoints"))
            winid = DebuggerPaneConfig::Breakpoint;

        else if(e.GetId() == XRCID("debugger_win_asciiview"))
            winid = DebuggerPaneConfig::AsciiViewer;

        if(winid != DebuggerPaneConfig::None) { e.Check(item.IsDebuggerWindowShown(winid)); }
    }
    //-----------------------------------------------------------------
    void Frame::OnRefactoringCacheStatus(wxCommandEvent& e)
    {
        e.Skip();
        if(e.GetInt() == 0) {
            // start
            gsgsLogMessage(wxString() << "Initializing refactoring database for workspace: "
                                    << gsgs_CppWorker().GetName());
        } else {
            gsgsLogMessage(wxString() << "Initializing refactoring database for workspace: "
                                    << gsgs_CppWorker().GetName() << "... done");
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnTheme(wxCommandEvent& e) { e.Skip(); }
    //-----------------------------------------------------------------
    void Frame::OnChangeActiveBookmarkType(wxCommandEvent& e)
    {
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor) editor->OnChangeActiveBookmarkType(e);
    }
    //-----------------------------------------------------------------
    void Frame::OnSettingsChanged(wxCommandEvent& e)
    {
        e.Skip();
        SetFrameTitle(GetMainBook()->GetActiveEditor());
        ShowOrHideCaptions();

        // As the toolbar is showing, refresh in case the group spacing was changed
        mToolBar->Realize();
        EditorVec_t editors;
        GetMainBook()->GetAllEditors(editors, Book::kGetAll_IncludeDetached);

        std::for_each(editors.begin(), editors.end(), [&](Editor* editor) 
        { 
            editor->PreferencesChanged(); 
        });
    }
    //-----------------------------------------------------------------
    void Frame::OnDetachEditor(wxCommandEvent& e) { GetMainBook()->DetachActiveEditor(); }
    //-----------------------------------------------------------------
    void Frame::OnDetachEditorUI(wxUpdateUIEvent& e) { e.Enable(GetMainBook()->GetActiveEditor() != NULL); }
    //-----------------------------------------------------------------
    void Frame::OnShowStatusBar(wxCommandEvent& event)
    {
        DoShowStatusBar(event.IsChecked());
        gsgs_Config().Write(kConfigShowStatusBar, event.IsChecked());
    }
    //-----------------------------------------------------------------
    void Frame::OnShowStatusBarUI(wxUpdateUIEvent& event) { event.Check(m_frameHelper->IsStatusBarVisible()); }
    //-----------------------------------------------------------------
    void Frame::OnShowToolbar(wxCommandEvent& event)
    {
        DoShowToolbars(event.IsChecked());
        gsgs_Config().Write(kConfigShowToolBar, event.IsChecked());
    }
    //-----------------------------------------------------------------
    void Frame::OnShowToolbarUI(wxUpdateUIEvent& event) 
    { 
        event.Check(m_frameHelper->IsToolbarShown()); 
    }
    //-----------------------------------------------------------------
    void Frame::ShowOrHideCaptions()
    {
        DoShowCaptions(gsgs_EditorConfig().GetOptions()->IsShowDockingWindowCaption());
        mWinMag->UpdateAccels();
        PostSizeEvent();
    }
    //-----------------------------------------------------------------
    void Frame::OnOpenFileExplorerFromFilePath(wxCommandEvent& e)
    {
        Editor* editor = GetMainBook()->GetActiveEditor();
        if(editor) { FileUtil::OpenFileExplorerAndSelect(editor->GetFileName()); }
    }
    //-----------------------------------------------------------------
    void Frame::OnSwitchWorkspaceUI(wxUpdateUIEvent& event)
    {
        CHECK_SHUTDOWN();
        event.Enable(!gsgs_Workspace().IsValid());
    }
    //-----------------------------------------------------------------
    void Frame::OnSplitSelection(wxCommandEvent& event)
    {
        Editor* editor = GetMainBook()->GetActiveEditor(true);
        if(editor)
            editor->SplitSelection();
    }
    //-----------------------------------------------------------------
    void Frame::OnSplitSelectionUI(wxUpdateUIEvent& event)
    {
        Editor* editor = GetMainBook()->GetActiveEditor(true);
        event.Enable(editor && editor->HasSelection());
    }
    //-----------------------------------------------------------------
    void Frame::OnProjectRenamed(Event& event)
    {
        event.Skip();
        SetFrameTitle(GetMainBook()->GetActiveEditor());
    }
    //-----------------------------------------------------------------
    void Frame::OnShowTabBar(wxCommandEvent& event)
    {
        gsgs_Config().Write(kConfigShowTabBar, event.IsChecked());
        GetMainBook()->ShowTabBar(event.IsChecked());
    }
    //-----------------------------------------------------------------
    void Frame::OnShowTabBarUI(wxUpdateUIEvent& event) 
    {
        bool temp;
        gsgs_Config().Read(kConfigShowTabBar, temp, true);
        event.Check(temp);
    }
    //-----------------------------------------------------------------
    void Frame::OnRunSetupWizard(wxCommandEvent&)
    {
        if(!StartSetupWizard(1)) 
        { 
            GetMainBook()->ApplySettings(); 
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnCloseTabsToTheRight(wxCommandEvent&)
    {
        wxWindow* win = GetMainBook()->GetCurrentPage();
        if(win) 
        { 
            GetMainBook()->CallAfter(&Book::CloseTabsToTheRight, win); 
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnMarkEditorReadonly(wxCommandEvent& e)
    {
        Editor* editor = GetMainBook()->GetActiveEditor();
        if (editor)
        {
            editor->SetReadOnly(e.IsChecked());
            GetMainBook()->MarkEditorReadOnly(editor);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnMarkEditorReadonlyUI(wxUpdateUIEvent& e)
    {
        Editor* editor = GetMainBook()->GetActiveEditor();
        if (editor)
        {
            e.Check(!editor->IsEditable());
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnWorkspaceLoaded(wxCommandEvent& e)
    {
        e.Skip();
        // If the workspace tab is visible, make it active
        int where = GetWorkspacePane()->GetNotebook()->GetPageIndex(_("Workspace"));
        if(where != wxNOT_FOUND) 
        { 
            GetWorkspacePane()->GetNotebook()->SetSelection(where); 
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnFileOpenFolder(wxCommandEvent& event)
    {
        wxString path = ::wxDirSelector(_("Select Folder"));
        if(path.IsEmpty()) 
            return;
        GetWorkspacePane()->GetFileExplorer()->OpenFolder(path);
        GetWorkspacePane()->SelectTab(GetWorkspacePane()->GetFileExplorer()->GetCaption());
    }
    //-----------------------------------------------------------------
    void Frame::OnNewWorkspaceUI(wxUpdateUIEvent& event)
    {
        event.Enable(!gsgs_Workspace().IsValid());
    }
    //-----------------------------------------------------------------
    void Frame::OnNewProjectUI(wxUpdateUIEvent& event)
    {
        event.Enable(gsgs_Workspace().IsValid() &&
                     gsgs_Workspace().GetWorkspace()->IsProjectSupported());
    }
    //-----------------------------------------------------------------
    void Frame::OnPrint(wxCommandEvent& event)
    {
        if(GetMainBook()->GetActiveEditor(true)) { GetMainBook()->GetActiveEditor(true)->Print(); }
    }
    //-----------------------------------------------------------------
    void Frame::OnPageSetup(wxCommandEvent& event)
    {
        if(GetMainBook()->GetActiveEditor(true)) { GetMainBook()->GetActiveEditor(true)->PageSetup(); }
    }
    //-----------------------------------------------------------------
    void Frame::OnRecentWorkspaceUI(wxUpdateUIEvent& e)
    {
        // We don't allow reloading of recent workspace while another is opened
        e.Enable(!gsgs_Workspace().IsValid());
    }
    //-----------------------------------------------------------------
    void Frame::OnToggleReverseDebugging(wxCommandEvent& e)
    {
        // Currently only supported on GDB
        if(DebuggerMgr::Get().GetActiveDebugger()) {
            DebuggerMgr::Get().GetActiveDebugger()->EnableReverseDebugging(e.IsChecked());
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnToggleReverseDebuggingUI(wxUpdateUIEvent& e)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        bool enable = gsgs_CppWorker().IsOpen() && (dbgr && (dbgr->GetName() == "GNU gdb debugger"));
        if(enable && dbgr->IsRecording()) {
            e.Enable(true);
            e.Check(dbgr->IsReverseDebuggingEnabled());
        } else {
            e.Check(false);
            e.Enable(false);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnToggleReverseDebuggingRecording(wxCommandEvent& e)
    {
        // Currently only supported on GDB
        if(DebuggerMgr::Get().GetActiveDebugger()) {
            DebuggerMgr::Get().GetActiveDebugger()->EnableRecording(e.IsChecked());
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnToggleReverseDebuggingRecordingUI(wxUpdateUIEvent& e)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        bool enable = gsgs_CppWorker().IsOpen() && (dbgr && (dbgr->GetName() == "GNU gdb debugger") &&
                                                            dbgr->IsRunning() && gsgs_Manager()->DbgCanInteract());
        if(enable) {
            e.Enable(true);
            e.Check(DebuggerMgr::Get().GetActiveDebugger()->IsRecording());
        } else {
            e.Enable(false);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnCopyFilePathRelativeToWorkspace(wxCommandEvent&)
    {
        IEditor* editor = GetManager()->GetActiveEditor();
        CHECK_PTR_RET(editor);
        CHECK_COND_RET(gsgs_Workspace().IsValid());

        wxFileName fn(editor->GetFileName());
        fn.MakeRelativeTo(gsgs_Workspace().GetWorkspace()->GetFileName().GetPath());

        ::CopyToClipboard(fn.GetFullPath());
    }
    //-----------------------------------------------------------------
    void Frame::OnCopyFilePathRelativeToWorkspaceUI(wxUpdateUIEvent& event)
    {
        event.Enable(gsgs_Workspace().IsValid() && GetManager()->GetActiveEditor());
    }
    //-----------------------------------------------------------------
    void Frame::loadLogo()
    {
        ImageManager * bmpLoader = &gsgs_Image();

        wxString basename = "-gsgs-logo";
    #ifdef __WXGTK__
        if(getuid() == 0) {
            // ROOT_INFO_LUID
            basename = "-gsgs-root";
        }
    #endif

        wxIconBundle app_icons;
        {
            const wxBitmap & iconBmp = bmpLoader->LoadBitmap("16" + basename);
            if(&iconBmp != &wxNullBitmap) {
                wxIcon icn;
                icn.CopyFromBitmap(iconBmp);
                app_icons.AddIcon(icn);
            }
        }

        {
            const wxBitmap& iconBmp = bmpLoader->LoadBitmap("32" + basename);
            if(&iconBmp != &wxNullBitmap) {
                wxIcon icn;
                icn.CopyFromBitmap(iconBmp);
                app_icons.AddIcon(icn);
            }
        }

        {
            const wxBitmap& iconBmp = bmpLoader->LoadBitmap("64" + basename);
            if(&iconBmp != &wxNullBitmap) {
                wxIcon icn;
                icn.CopyFromBitmap(iconBmp);
                app_icons.AddIcon(icn);
            }
        }
        /*
        {
            const wxBitmap& iconBmp = bmpLoader->LoadBitmap("128" + basename);
            if(&iconBmp != &wxNullBitmap) {
                wxIcon icn;
                icn.CopyFromBitmap(iconBmp);
                app_icons.AddIcon(icn);
            }
        }
        {
            const wxBitmap& iconBmp = bmpLoader->LoadBitmap("256" + basename);
            if(&iconBmp != &wxNullBitmap) {
                wxIcon icn;
                icn.CopyFromBitmap(iconBmp);
                app_icons.AddIcon(icn);
            }
        }*/
        SetIcons(app_icons);
    }
    //-----------------------------------------------------------------
    void Frame::OnDuplicateTab(wxCommandEvent& event)
    {
        IEditor* currentFile = GetManager()->GetActiveEditor();
        if(currentFile) {
            IEditor* newEditor = GetManager()->NewEditor();
            if(newEditor) {
                newEditor->GetCtrl()->SetText(currentFile->GetCtrl()->GetText());
                // Open the 'Save As' dialog, with some sensible defaults
                if(!newEditor->SaveAs(currentFile->GetFileName().GetFullName(), currentFile->GetFileName().GetPath())) {
                    // If the "Save As" failed for any reason, remove the current editor
                    GetManager()->CloseEditor(newEditor, false);
                    // Set the editor back to the current editor
                    GetMainBook()->GetFindBar()->SetEditor(currentFile->GetCtrl());
                }
            }
        }
    }
    //-----------------------------------------------------------------
    void Frame::DoShowCaptions(bool show)
    {
        if(show){
            wxAuiPaneInfoArray& panes = mWinMag->GetAllPanes();
            for(size_t i = 0; i < panes.GetCount(); ++i) {
                // Editor is the center pane - don't add it a caption
                if(panes.Item(i).IsOk() && !panes.Item(i).IsToolbar() && panes.Item(i).name != "Editor")
                    panes.Item(i).CaptionVisible(true);
            }
        }
        else
        {
            wxAuiPaneInfoArray& panes = mWinMag->GetAllPanes();
            for (size_t i = 0; i < panes.GetCount(); ++i) {
                if (panes.Item(i).IsOk() && !panes.Item(i).IsToolbar()) panes.Item(i).CaptionVisible(false);
            }
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnToggleMinimalView(wxCommandEvent&)
    {
        // Hide the toolbars + captions
        // Hide the _native_ toolbar
        bool minimalView;
        gsgs_Config().Read("MinimalView", minimalView, true);
        if(minimalView) {
            if(m_frameHelper->IsToolbarShown()) {
                // Hide the toolbar
                DoShowToolbars(false, false);
            }
            if(m_frameHelper->IsCaptionsVisible()) { DoShowCaptions(false); }
    #ifndef __WXOSX__
            GetMenuBar()->Hide();
    #endif
        } else {
            if(!m_frameHelper->IsToolbarShown()) { DoShowToolbars(true, false); }
            if(!m_frameHelper->IsCaptionsVisible()) { DoShowCaptions(true); }
    #ifndef __WXOSX__
            GetMenuBar()->Show();
    #endif
        }

        // Update the various configurations
        gsgs_Config().Write(kConfigShowToolBar, !minimalView);
        gsgs_Config().Write("MinimalView", !minimalView); // for next time

        // Update the captions settings
        OptionsConfig * opts = gsgs_EditorConfig().GetOptions();
        opts->SetShowDockingWindowCaption(!minimalView);
        gsgs_EditorConfig().SetOptions(opts);

        mWinMag->UpdateAccels();
        PostSizeEvent();
    }
    //-----------------------------------------------------------------
    void Frame::OnToggleMinimalViewUI(wxUpdateUIEvent& event)
    {
        bool inMinimalView;
        gsgs_Config().Read("MinimalView", inMinimalView, false);
        event.Check(!inMinimalView);
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugStepInstUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        e.Enable(IsDebuggerRunning() && m_frameHelper->GetDebuggerFeatures() & DebugEvent::kStepInst);
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugJumpToCursorUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        e.Enable(IsDebuggerRunning() && m_frameHelper->GetDebuggerFeatures() & DebugEvent::kJumpToCursor);
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugRunToCursorUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        e.Enable(IsDebuggerRunning() && m_frameHelper->GetDebuggerFeatures() & DebugEvent::kRunToCursor);
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugInterruptUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        e.Enable(IsDebuggerRunning() && m_frameHelper->GetDebuggerFeatures() & DebugEvent::kInterrupt);
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugShowCursorUI(wxUpdateUIEvent& e)
    {
        CHECK_SHUTDOWN();
        e.Enable(IsDebuggerRunning() && m_frameHelper->GetDebuggerFeatures() & DebugEvent::kShowCursor);
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugRunToCursor(wxCommandEvent& e)
    {
        // Allow the plugins to handle this command first
        if(gsgs_Event().ProcessEvent(e)) { return; }

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        IEditor* editor = GetManager()->GetActiveEditor();
        if(editor && dbgr && dbgr->IsRunning() && gsgs_Manager()->DbgCanInteract()) {
            BreakpointInfo bp;
            bp.Create(editor->GetFileName().GetFullPath(), editor->GetCurrentLine() + 1,
                      gsgs_Manager()->GetBreakpointsMgr()->GetNextID());
            bp.bp_type = BP_type_tempbreak;
            dbgr->Break(bp);
            dbgr->Continue();
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnDebugJumpToCursor(wxCommandEvent& e)
    {
        // Allow the plugins to handle this command first
        if(gsgs_Event().ProcessEvent(e)) { return; }

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        IEditor* editor = GetManager()->GetActiveEditor();
        if(editor && dbgr && dbgr->IsRunning() && gsgs_Manager()->DbgCanInteract()) 
        {
            dbgr->Jump(editor->GetFileName().GetFullPath(), editor->GetCurrentLine() + 1);
        }
    }
    //-----------------------------------------------------------------
    GsGsApp* Frame::GetTheApp()
    {
        GsGsApp* app = dynamic_cast<GsGsApp*>(wxApp::GetInstance());
        wxASSERT(app);
        return app;
    }
    //-----------------------------------------------------------------
    void Frame::OnEnvironmentVariablesModified(Event& e)
    {
        e.Skip();
        SelectBestEnvSet();
        if(gsgs_CppWorker().IsOpen()) 
        {
            // mark all the projects as dirty
            wxArrayString projects;
            gsgs_CppWorker().GetProjectList(projects);
            for(size_t i = 0; i < projects.size(); i++) 
            {
                ProjectPtr proj = gsgs_CppWorker().GetProject(projects.Item(i));
                if(proj) { proj->SetModified(true); }
            }
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnWordComplete(wxCommandEvent &)
    {
        Editor* editor = GetMainBook()->GetActiveEditor(true);
        CHECK_PTR_RET(editor);

        // Get the filter
        wxStyledTextCtrl* stc = editor->GetCtrl();
        int curPos = stc->GetCurrentPos();
        int start = stc->WordStartPosition(stc->GetCurrentPos(), true);
        if(curPos < start) return;

        CodeCompleteEvent ccEvent(wxEVT_CC_WORD_COMPLETE);
        ccEvent.SetEditor(editor);
        ccEvent.SetEventObject(this);
        ccEvent.SetTriggerKind(LSP::CompletionItem::kTriggerUser);
        ccEvent.SetWord(stc->GetTextRange(start, curPos));
        ServiceManager::Get().ProcessEvent(ccEvent);

        const wxCodeCompletionBoxEntry::Vec_t& entries = ccEvent.GetEntries();
        if(entries.empty()) return;
        CodeBoxManager::Get().ShowCompletionBox(
            editor->GetCtrl(), entries,
            CodeBox::kNoShowingEvent, // Don't fire the "wxEVT_CCBOX_SHOWING event
            wxNOT_FOUND);
    }
    //-----------------------------------------------------------------
    void Frame::OnGotoAnything(wxCommandEvent&) 
    { 
        GotoEntryManager::Get().ShowDialog(); 
    }
    //-----------------------------------------------------------------
    void Frame::OnVersionCheckError(wxCommandEvent& e)
    {
        gsgsWARNING() << "New version check failed:" << e.GetString();
        wxDELETE(m_webUpdate);
    }
    //-----------------------------------------------------------------
    void Frame::OnMainBookNavigating(wxCommandEvent&)
    {
        GetMainBook()->ShowNavigationDialog();
    }
    //-----------------------------------------------------------------
    void Frame::OnMainBookMovePage(wxCommandEvent& e)
    {
        GetMainBook()->MovePage(e.GetId() == XRCID("wxEVT_BOOK_MOVE_TAB_RIGHT"));
    }
    //-----------------------------------------------------------------
    void Frame::OnFindSelection(wxCommandEvent& event)
    {
        event.Skip();
        Editor* editor = GetMainBook()->GetActiveEditor();
        CHECK_PTR_RET(editor);
        event.Skip(false);
        wxString selection =
            editor->GetSelection().IsEmpty() ? GetMainBook()->GetFindBar()->GetFindWhat() : editor->GetSelection();
        GetMainBook()->GetFindBar()->SetFindWhat(selection);
        if(GetMainBook()->GetFindBar()->IsShown()) {
            QuickFindBar::Search(editor->GetCtrl(), selection, QuickFindBar::kSearchForward, GetMainBook()->GetFindBar());
        } else {
            QuickFindBar::Search(editor->GetCtrl(), selection, QuickFindBar::kSearchForward, NULL);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnFindSelectionPrev(wxCommandEvent& event)
    {
        event.Skip();
        Editor* editor = GetMainBook()->GetActiveEditor();
        CHECK_PTR_RET(editor);

        event.Skip(false);
        wxString selection =
            editor->GetSelection().IsEmpty() ? GetMainBook()->GetFindBar()->GetFindWhat() : editor->GetSelection();
        GetMainBook()->GetFindBar()->SetFindWhat(selection);
        if(GetMainBook()->GetFindBar()->IsShown()) {
            QuickFindBar::Search(editor->GetCtrl(), selection, 0, GetMainBook()->GetFindBar());
        } else {
            QuickFindBar::Search(editor->GetCtrl(), selection, 0, NULL);
        }
    }
    //-----------------------------------------------------------------
    /*void Frame::OnFindWordAtCaret(wxCommandEvent& event)
    {
        event.Skip();
        Editor* editor = GetMainBook()->GetActiveEditor();
        CHECK_PTR_RET(editor);

        wxString selection;
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
        if(ctrl->GetSelectedText().IsEmpty()) {
            // Select the current word
            long pos = ctrl->GetCurrentPos();
            long start = ctrl->WordStartPosition(pos, true);
            long end = ctrl->WordEndPosition(pos, true);

            selection = ctrl->GetTextRange(start, end);
            if(!selection.IsEmpty()) { ctrl->SetCurrentPos(start); }
        }
        if(selection.IsEmpty()) return;
        OnFindSelection(event);
    }*/
    //-----------------------------------------------------------------
    /*void Frame::OnFindWordAtCaretPrev(wxCommandEvent& event)
    {
        event.Skip();
        Editor* editor = GetMainBook()->GetActiveEditor();
        CHECK_PTR_RET(editor);

        wxString selection;
        wxStyledTextCtrl* ctrl = editor->GetCtrl();
        if(ctrl->GetSelectedText().IsEmpty()) {
            // Select the current word
            long pos = ctrl->GetCurrentPos();
            long start = ctrl->WordStartPosition(pos, true);
            long end = ctrl->WordEndPosition(pos, true);

            selection = ctrl->GetTextRange(start, end);
            if(!selection.IsEmpty()) { ctrl->SetCurrentPos(start); }
        }
        if(selection.IsEmpty()) return;
        OnFindSelectionPrev(event);
    }*/
    //-----------------------------------------------------------------
    void Frame::OnCustomiseToolbar(wxCommandEvent&)
    {
        CustomiseToolBarDlg dlg(this, mToolBar);
        if (dlg.ShowModal() == wxID_OK)
        {
            mToolBar->Realize();

            wxArrayString hiddenItems;
            const std::vector<ToolBarButton*>& buttons = mToolBar->GetButtons();
            for (size_t i = 0; i < buttons.size(); ++i)
            {
                if (buttons[i]->IsHidden() && !buttons[i]->IsSeparator())
                {
                    hiddenItems.Add(buttons[i]->GetLabel());
                }
            }
            gsgs_Config().Write("ToolBarHiddenItems", hiddenItems);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnInfobarButton(wxCommandEvent& event)
    {
        event.Skip(); // needed to make sure that the bar is hidden
        int buttonID = event.GetId();
        if(buttonID == XRCID("restart-gsgs")) 
        {
            gsgs_Manager()->OnCmdRestart(event);
        } 
        else 
        {
            Event buttonEvent(wxEVT_INFO_BAR_BUTTON);
            buttonEvent.SetInt(buttonID);
            buttonEvent.SetEventObject(mInfoBar);
            gsgs_Event().AddPendingEvent(buttonEvent);
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnShowMenuBar(wxCommandEvent& event)
    {
        bool isShown = GetMenuBar()->IsShown();
        GetMenuBar()->Show(!isShown);
        GetSizer()->Layout();
        PostSizeEvent();
        gsgs_Config().Write(kConfigShowMenuBar, !isShown);
    }
    //-----------------------------------------------------------------
    void Frame::OnShowMenuBarUI(wxUpdateUIEvent& event)
    {
    #ifdef __WXGTK__
        event.Check(GetMenuBar()->IsShown());
    #else
        event.Check(true);
        event.Enable(false);
    #endif
    }
    //-----------------------------------------------------------------
    //void Frame::Raise() { wxFrame::Raise(); }
    //-----------------------------------------------------------------
    void Frame::OnFindReferences(RefactoringEvent& e)
    {
        e.Skip();
        // Show the results
        GetOutputPane()->GetShowUsageTab()->ShowUsage(e.GetMatches(), e.GetString());
    }
    //-----------------------------------------------------------------
    void Frame::OnRenameSymbol(RefactoringEvent& e)
    {
        e.Skip();
        // display the refactor dialog
        RenameSymbol dlg(this, e.GetMatches(), e.GetPossibleMatches(), e.GetString());
        if(dlg.ShowModal() == wxID_OK) {
            CppToken::Vec_t matches;
            dlg.GetMatches(matches);
            if(!matches.empty() && dlg.GetWord() != e.GetString()) { ContextCpp::ReplaceInFiles(dlg.GetWord(), matches); }
        }
    }
    //-----------------------------------------------------------------
    void Frame::OnGotoDownloadPage(wxCommandEvent&)
    {
        ::wxLaunchDefaultBrowser(m_DownloadPageURL);
        m_DownloadPageURL.Clear();
    }
    //-----------------------------------------------------------------
    void Frame::OnReportIssue(wxCommandEvent &)
    {
        ::wxLaunchDefaultBrowser("https://github.com/suzhengquan/MBCAF/issues");
    }
    //-----------------------------------------------------------------
}