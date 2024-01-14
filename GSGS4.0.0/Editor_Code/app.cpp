
#include "gsgsPreProcess.h"
#include "app.h"
#include "gsgsStyleManager.h"
#include "SocketAPI/gsgsSocketClient.h"
#include "gsgsProcess.h"
#include "GsGsVersion.h"
#include "gsgsWindowManager.h"
#include "gsgsConfig.h"
#include "gsgsCommon.h"
#include "gsgsEditorConfig.h"
#include "gsgsPluginCommon.h"
#include "gsgsLogManager.h"
#include "gsgsFileUtil.h"
#include "globals.h"
#include "gsgsManager.h"
#include "gsgsBuildTab.h"
#include "InnerCompilerLocator.h"
#include "xmlutils.h"
#include "gsgsWorkspacePane.h"
#include "gsgsProfile.h"
#include "gsgsEvent.h"
#include "ExceptionManager.h"
#include "ImageEditorFrame.h"
#include "ImageDocument.h"
#include "ImageEditorView.h"
#include "UILayoutEditorFrame.h"
#include "UILayoutOptions.h"
#include "UILayoutDialogMain.h"
#include "StrHelper.h"
#include "NiiLogManager.h"
#include "wx/splash.h"
#include "wx/aboutdlg.h"

#include "bitmaps/icon32.xpm"
#include "bitmaps/icon48.xpm"
#include "StrHelper.h"
#include "NiiException.h"
#include <wx/object.h>
#include <wx/image.h>
#include <wx/imagjpeg.h>
#include <wx/persist.h>
#include <wx/regex.h>
#include <wx/fileconf.h>
#include <wx/stackwalk.h>
#include "wx/txtstrm.h"

#if defined(__WXMAC__) || defined(__WXGTK__)
#include <signal.h> // sigprocmask
#include <sys/wait.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/registry.h> //registry keys
void EnableDebugPriv()
{
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES tkp;

    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);

    LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = luid;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);

    CloseHandle(hToken);
}
#endif
#ifndef __WXMSW__
static void ChildTerminatedSingalHandler(int signo)
{
    int status;
    while(true) {
        pid_t pid = ::waitpid(-1, &status, WNOHANG);
        if(pid > 0) {
            // waitpid succeeded
            IProcess::SetProcessExitCode(pid, WEXITSTATUS(status));
            // gsgs_Debug("Process terminated. PID: %d, Exit Code: %d", pid, WEXITSTATUS(status));

        } else {
            break;
        }
    }
}

// Block/Restore sigchild
static struct sigaction old_behvior;
static struct sigaction new_behvior;
static void gsgsBlockSigChild()
{
    sigfillset(&new_behvior.sa_mask);
    new_behvior.sa_handler = ChildTerminatedSingalHandler;
    new_behvior.sa_flags = 0;
    sigaction(SIGCHLD, &new_behvior, &old_behvior);
}

#endif

#ifdef __WXMAC__
#include <mach-o/dyld.h>

// On Mac we determine the base path using system call
// _NSGetExecutablePath(path, &path_len);
wxString MacGetBasePath()
{
    char path[257];
    uint32_t path_len = 256;
    _NSGetExecutablePath(path, &path_len);

    // path now contains
    // gsgs.app/Contents/MacOS/
    wxFileName fname(wxString(path, wxConvUTF8));

    // remove he MacOS part of the exe path
    wxString file_name = fname.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR);
    wxString rest;
    file_name.EndsWith(wxT("MacOS/"), &rest);
    rest.Append(wxT("SharedSupport/"));

    return rest;
}
#endif

#ifdef __WXGTK__
//-------------------------------------------
// Signal Handlers for GTK
//-------------------------------------------
static void WaitForDebugger(int signo)
{
    wxString msg;
    wxString where;

    msg << wxT("gsgs crashed: you may attach to it using gdb\n") << wxT("or let it crash silently..\n")
        << wxT("Attach debugger?\n");

    int rc = wxMessageBox(msg, wxT("gsgs Crash Handler"), wxYES_NO | wxCENTER | wxICON_ERROR);
    if(rc == wxYES) {

        // Launch a shell command with the following command:
        // gdb -p <PID>

        char command[256];
        memset(command, 0, sizeof(command));

        if(ExeLocator::Locate(wxT("gnome-terminal"), where)) {
            sprintf(command, "gnome-terminal -t 'gdb' -e 'gdb -p %d'", getpid());
        } else if(ExeLocator::Locate(wxT("konsole"), where)) {
            sprintf(command, "konsole -T 'gdb' -e 'gdb -p %d'", getpid());
        } else if(ExeLocator::Locate(wxT("terminal"), where)) {
            sprintf(command, "terminal -T 'gdb' -e 'gdb -p %d'", getpid());
        } else if(ExeLocator::Locate(wxT("lxterminal"), where)) {
            sprintf(command, "lxterminal -T 'gdb' -e 'gdb -p %d'", getpid());
        } else {
            sprintf(command, "xterm -T 'gdb' -e 'gdb -p %d'", getpid());
        }

        if(system(command) == 0) {
            signal(signo, SIG_DFL);
            raise(signo);
        } else {
            // Go down without launching the debugger, ask the user to do it manually
            wxMessageBox(wxString::Format(wxT("Failed to launch the debugger\nYou may still attach to gsgs "
                                              "manually by typing this command in a terminal:\ngdb -p %d"),
                                          getpid()),
                         wxT("gsgs Crash Handler"), wxOK | wxCENTER | wxICON_ERROR);
            pause();
        }
    }

    signal(signo, SIG_DFL);
    raise(signo);
}
#endif

static const wxCmdLineEntryDesc cmdLineDesc[] = {
    { wxCMD_LINE_SWITCH, "v", "version", "Print current version", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "h", "help", "Print usage", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_SWITCH, "n", "no-plugins", "Start gsgs without any plugins", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "g", "dbg", "Start gsgs's GDB debugger to debug an executable", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "l", "line", "Open the file at a given line number", wxCMD_LINE_VAL_NUMBER,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "b", "basedir", "Use this path as the gsgs installation path (Windows only)",
      wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "d", "datadir", "Use this path as the gsgs data path", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "w", "dbg-dir", "When started with --dbg, set the working directory for the debugger",
      wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_OPTION, "p", "with-plugins", "Comma separated list of plugins to load", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_PARAM, NULL, NULL, "Input file", wxCMD_LINE_VAL_STRING,
      wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL },
    { wxCMD_LINE_NONE }
};

static void massCopy(const wxString& sourceDir, const wxString& spec, const wxString& destDir)
{
    wxArrayString files;
    wxDir::GetAllFiles(sourceDir, &files, spec, wxDIR_FILES);
    for(size_t i = 0; i < files.GetCount(); i++) 
    {
        wxFileName fn(files.Item(i));
        wxCopyFile(files.Item(i), destDir + wxT("/") + fn.GetFullName());
    }
}

namespace gsgs
{
    class InitializeDialog : public wxDialog
    {
    public:
        InitializeDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Initializing gsgs"), 
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(500,-1), long style = wxSTAY_ON_TOP|wxRESIZE_BORDER)
                : wxDialog(parent, id, title, pos, size, style)
        {
            wxBoxSizer* boxSizer26 = new wxBoxSizer(wxVERTICAL);
            this->SetSizer(boxSizer26);
            
            m_panel4 = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(500,-1), wxTAB_TRAVERSAL);
            
            boxSizer26->Add(m_panel4, 1, wxEXPAND, 5);
            
            wxBoxSizer* boxSizer6 = new wxBoxSizer(wxVERTICAL);
            m_panel4->SetSizer(boxSizer6);
            
            m_banner = new wxBannerWindow(m_panel4, wxID_ANY, wxTOP, wxDefaultPosition, wxSize(-1,-1), wxBORDER_THEME);
            m_banner->SetBitmap(wxNullBitmap);
            m_banner->SetText(_("Initializing gsgs"), wxT(""));
            m_banner->SetGradient(wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION), wxSystemSettings::GetColour(wxSYS_COLOUR_ACTIVECAPTION));
            m_banner->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_CAPTIONTEXT));
            
            boxSizer6->Add(m_banner, 0, wxALL|wxEXPAND, 5);
            
            m_gauge = new wxGauge(m_panel4, wxID_ANY, 100, wxDefaultPosition, wxSize(-1,-1), wxGA_HORIZONTAL);
            m_gauge->SetValue(10);
            
            boxSizer6->Add(m_gauge, 0, wxALL|wxEXPAND, 5);
            
            boxSizer6->Add(0, 0, 1, wxALL, 5);
            
            wxBoxSizer* boxSizer20 = new wxBoxSizer(wxHORIZONTAL);
            
            boxSizer6->Add(boxSizer20, 0, wxALL|wxALIGN_RIGHT, 5);
            m_panel4->SetMinSize(wxSize(500,-1));
            
            CentreOnParent(wxBOTH);
            CallAfter(&InitializeDialog::StartUpgrade);
            gsgs_Event().Connect(wxEVT_UPGRADE_LEXERS_START, gsgsEventHandler(InitializeDialog::OnLexersUpgradeStart), NULL, this);
            gsgs_Event().Connect(wxEVT_UPGRADE_LEXERS_END, gsgsEventHandler(InitializeDialog::OnLexersUpgradeEnd), NULL, this);
            gsgs_Event().Connect(wxEVT_UPGRADE_LEXERS_PROGRESS, gsgsEventHandler(InitializeDialog::OnLexersUpgradeProgress), NULL, this);
        }

        ~InitializeDialog()
        {
            gsgs_Event().Disconnect(wxEVT_UPGRADE_LEXERS_START, gsgsEventHandler(InitializeDialog::OnLexersUpgradeStart), NULL, this);
            gsgs_Event().Disconnect(wxEVT_UPGRADE_LEXERS_END, gsgsEventHandler(InitializeDialog::OnLexersUpgradeEnd), NULL, this);
            gsgs_Event().Disconnect(wxEVT_UPGRADE_LEXERS_PROGRESS, gsgsEventHandler(InitializeDialog::OnLexersUpgradeProgress), NULL, this);
        }
        void StartUpgrade()
        {
            gsgs_Style().Load(); // it will perform upgrade
        }

        wxBannerWindow* GetBanner() { return m_banner; }
        wxGauge* GetGauge() { return m_gauge; }
        wxPanel* GetPanel4() { return m_panel4; }
    protected:
        void OnLexersUpgradeEnd(gsgs::Event& e)
        {
            EndModal(wxID_OK);
        }

        void OnLexersUpgradeProgress(gsgs::Event& e)
        {
            e.Skip();
            m_gauge->SetValue(e.GetInt());
            m_banner->SetText(_("gsgs upgrade"), e.GetString());
            wxSafeYield(this);
        }

        void OnLexersUpgradeStart(gsgs::Event& e)
        {
            e.Skip();
            static bool rangeSet = false;
            if(!rangeSet) {
                m_gauge->SetRange(e.GetInt());
                rangeSet = true;
            }
        }
    private:
        wxPanel* m_panel4;
        wxBannerWindow* m_banner;
        wxGauge* m_gauge;
    };

    class Registry {
    public:
        Registry()
        {
            // m_filename is set in the static method SetFilename()
            m_fp.Open(m_filename.c_str(), wxT("rb"));
            if(m_fp.IsOpened()) {
                wxString fileContent;
                m_fp.ReadAll(&fileContent, wxConvUTF8);

                wxArrayString entries = wxStringTokenize(fileContent, wxT("\n\r"), wxTOKEN_STRTOK);
                for(size_t i=0; i<entries.GetCount(); i++) {
                    entries.Item(i).Trim().Trim(false);

                    // Empty line?
                    if(entries.Item(i).IsEmpty())
                        continue;

                    // Comment line?
                    if(entries.Item(i).StartsWith(wxT(";"))) {
                        continue;
                    }

                    if(entries.Item(i).StartsWith(wxT("["))) {
                        // Section
                        continue;
                    }

                    wxString key   = entries.Item(i).BeforeFirst(wxT('='));
                    wxString value = entries.Item(i).AfterFirst(wxT('='));

                    m_entries[key] = value;
                }
            }
        }

        bool Read (const wxString &key, wxString &val)
        {
            std::map<wxString, wxString>::iterator iter = m_entries.find(key);
            if(iter == m_entries.end()) {
                return false;
            }
            val = iter->second;
            return true;
        }
        
        static void SetFilename(const wxString &filename)
        {
            m_filename = filename.c_str();
        }
    private:
        static wxString              m_filename;
        wxFFile                      m_fp;
        std::map<wxString, wxString> m_entries;
    };
    
    wxString Registry::m_filename;
    
    class PersistenceManager : public wxPersistenceManager
    {
    public:
        PersistenceManager() : m_iniFile(new wxFileConfig("", "",
            gsgs_Path().GetUserDataDir() + gsgsPathSep + "config" +
                gsgsPathSep + "GsGs.ini", "", wxCONFIG_USE_LOCAL_FILE))
        {

        }
        virtual ~PersistenceManager()
        {
            m_iniFile->Flush();
            wxDELETE(m_iniFile);
        }
        virtual wxConfigBase * GetConfig() const
        {
            return m_iniFile;
        }
    private:
        wxFileConfig * m_iniFile;
    };
    
#if wxUSE_STACKWALKER
    //class wxTextOutputStream;
    class StackWalker : public wxStackWalker
    {
        wxTextOutputStream * m_output;

    public:
        StackWalker(wxTextOutputStream *output)
            : m_output(output)
        {
        }

        virtual ~StackWalker()
        {

        }
        void OnStackFrame(const wxStackFrame &frame)
        {
            *m_output << frame.GetModule() << wxT(" ") << frame.GetName() << wxT(" ") << frame.GetFileName() << wxT(" ")
                << (int)frame.GetLine() << wxT("\n");
        }
    };
#endif 
}

IMPLEMENT_APP(GsGsApp)

/*class clIniFile : public wxFileConfig
{
public:
    clIniFile(const wxFileName& iniFullPath)
        : wxFileConfig("", "", iniFullPath.GetFullPath(), "", wxCONFIG_USE_LOCAL_FILE)
    {
    }
    clIniFile(const wxString& fullname)
        : wxFileConfig("", "",
                    gsgs_Path().GetUserDataDir() + gsgsPathSep + "config" +
                       gsgsPathSep + "GsGs.ini", "", wxCONFIG_USE_LOCAL_FILE)
    {
    }
};
*/
//-----------------------------------------------------------------------
static wxLogNull NO_LOG;
// BEGIN_EVENT_TABLE(GsGsApp, wxApp)
//    EVT_ACTIVATE_APP(GsGsApp::OnAppAcitvated)
// END_EVENT_TABLE()

extern void InitXmlResource();
GsGsApp::GsGsApp(void): 
    mMainFrame(NULL), 
    m_only(NULL), 
    m_pluginLoadPolicy(PP_All), 
    mPersistencManager(NULL), 
    m_docManager(NULL),
    mImageEditorFrame(NULL),
    mUILayoutEditorFrame(NULL),
    mLogIndex(0),
    m_startedInDebuggerMode(false)
{
}
//-----------------------------------------------------------------------
GsGsApp::~GsGsApp(void)
{
    wxImage::CleanUpHandlers();
    if(m_only) 
    { 
        delete m_only; 
    }
    wxDELETE(mPersistencManager);
}
//-----------------------------------------------------------------------
gsgs::ImageEditorFrame * GsGsApp::createImageEditor()
{
    try
    {
        mImageEditorFrame = new gsgs::ImageEditorFrame(m_docManager, mMainFrame, wxID_ANY, wxT("ImageEditor"), wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE);
        mImageEditorFrame->Centre(wxBOTH);
        mImageEditorFrame->Hide();
        mImageEditorFrame->AttachCanvas(0);
        //SetTopWindow(mImageEditorFrame);
    }
    catch (NII::Exception & e)
    {
        // If something goes wrong, show user
        wxLogError(wxT("ImagesetEditor - Error: %s"), gsgs::StrHelper::ToWXString(e.getVerbose()).c_str());
    }
    return mImageEditorFrame;
}
//-----------------------------------------------------------------------
gsgs::UILayoutEditorFrame* GsGsApp::createLayoutEditor()
{
    try
    {
        // Create Single-Document main frame, just look default position and dimensions
        mUILayoutEditorFrame = new gsgs::UILayoutEditorFrame(m_docManager, mMainFrame, wxID_ANY, wxT("UILayoutEditor"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE);
        mUILayoutEditorFrame->SetIcon(wxIcon(icon32_xpm));
        mUILayoutEditorFrame->Centre(wxBOTH);
        //mUILayoutEditorFrame->Show(true);
        mUILayoutEditorFrame->Hide();
        // This needs to be in the try-block!
        // NOTE: This operation is quite SLOW!
        mUILayoutEditorFrame->AttachCanvas(NULL);
        //SetTopWindow(mUILayoutEditorFrame);

        // When a current layout was set, load it
        const wxString layout = UILayoutOptions::GetInstancePtr()->GetCurrentLayout();
        if (layout != wxT(""))
            (void)m_docManager->CreateDocument(layout, wxDOC_SILENT);

        // Process all existing widget types and for each of them store their possible Look'N'Feels
        // for later use. Also, keep a global stash with all existing Look'N'Feels (skins).
        // NOTE: This needs to be done after the OpenGL canvas is attached, because we need to have
        // the falagard types mapped in the system. Therefore, it can't be done inside the main dialog
        // constructor.
        // NOTE2: This is only done once. This means that if the user adds new skins or modifies skins while
        // the editor is running, those changes won't be detected and the application will need to be 
        // restarted in order to recognize them. 
        if (mUILayoutEditorFrame->GetDialogMain())
        {
            mUILayoutEditorFrame->GetDialogMain()->StoreWidgetLooks();
        }
    }
    catch (NII::Exception & e)
    {
        // If something goes wrong, show user
        wxLogError(wxT("UILayoutEditor - Error: %s"), gsgs::StrHelper::ToWXString(e.getVerbose()).c_str());
    }
    return mUILayoutEditorFrame;
}
//-----------------------------------------------------------------------
void GsGsApp::createEditorLog()
{
    mLogIndex = N_Only(Log).create(_T("UILayout"))->getIndex();
}
//-----------------------------------------------------------------------
bool GsGsApp::OnInit()
{
#if defined(__WXMSW__) && CL_DEBUG_BUILD
    SetAppName(wxT("GsGs-debug"));
#else
    SetAppName(wxT("GsGs"));
#endif

#ifdef __WXGTK__
    // We need to set the installation prefix on GTK for some reason (mainly debug builds)
    wxString installationDir(INSTALL_DIR);
    installationDir.RemoveLast(sizeof("/share/gsgs") - 1);
    wxStandardPaths::Get().SetInstallPrefix(installationDir);
#endif
#if defined(__WXGTK__) || defined(__WXMAC__)

    // block signal pipe
    sigset_t mask_set;
    sigemptyset(&mask_set);
    sigaddset(&mask_set, SIGPIPE);
    sigaddset(&mask_set, SIGTTIN);
    sigprocmask(SIG_SETMASK, &mask_set, NULL);

    // Handle sigchld
    gsgsBlockSigChild();

#ifdef __WXGTK__
    // Insall signal handlers
    signal(SIGSEGV, WaitForDebugger);
    signal(SIGABRT, WaitForDebugger);
#endif

#endif
    wxSocketBase::Initialize();
    wxLog::SetActiveTarget(new wxLogStderr());

#if wxUSE_ON_FATAL_EXCEPTION
    // trun on fatal exceptions handler
    wxHandleFatalExceptions(true);
#endif

#ifdef __WXMSW__
    typedef BOOL (*SetProcessDPIAwareFunc)();
    HINSTANCE user32Dll = LoadLibrary(wxT("User32.dll"));
    if(user32Dll) 
    {
        SetProcessDPIAwareFunc pFunc = (SetProcessDPIAwareFunc)GetProcAddress(user32Dll, "SetProcessDPIAware");
        if(pFunc) 
        {
            pFunc(); 
        }
        FreeLibrary(user32Dll);
    }
#endif

    // Init resources and add the PNG handler
    wxSystemOptions::SetOption(_T("msw.remap"), 0);
    wxSystemOptions::SetOption("msw.notebook.themed-background", 1);
    wxXmlResource::Get()->InitAllHandlers();
    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxCURHandler);
    wxImage::AddHandler(new wxICOHandler);
    wxImage::AddHandler(new wxXPMHandler);
    wxImage::AddHandler(new wxGIFHandler);
    wxImage::AddHandler(new wxJPEGHandler);
    InitXmlResource();

    wxLog::EnableLogging(false);
    wxString homeDir(wxEmptyString);

    // parse command line
    wxCmdLineParser parser;
    parser.SetDesc(cmdLineDesc);
    parser.SetCmdLine(wxAppBase::argc, wxAppBase::argv);
    if(parser.Parse(false) != 0) 
    {
        PrintUsage(parser);
        return false;
    }

    wxString newDataDir(wxEmptyString);
    if(parser.Found("g", &m_exeToDebug)) 
    {
        SetStartedInDebuggerMode(true);
        // Check to see if the user also passed "--working-directory" switch
        parser.Found("w", &m_debuggerWorkingDirectory);
        for(size_t i = 0; i < parser.GetParamCount(); ++i) 
        {
            m_debuggerArgs << parser.GetParam(i) << " ";
        }
    }

    if(parser.Found(wxT("d"), &newDataDir)) 
    { 
        gsgs_Path().SetUserDataDir(newDataDir); 
    }

    wxBitmap bitmap;
    bitmap.LoadFile(gsgs_Path().GetBinFolder()+ _T("/images/splashscreen.png"), wxBITMAP_TYPE_PNG);
    if (bitmap.IsOk())
    {
        m_splash = new wxSplashScreen(bitmap, wxSPLASH_CENTRE_ON_SCREEN | wxSPLASH_TIMEOUT, 2000, 0, -1);
    }

    // check for single instance
    if(!IsSingleInstance(parser)) 
    { 
        return false; 
    }

    if(parser.Found(wxT("h"))) 
    {
        PrintUsage(parser);
        return false;
    }

    if(parser.Found(wxT("v"))) 
    {
#ifdef __WXMSW__
        ::wxMessageBox(wxString() << "gsgs IDE v" << GSGS_VERSION, "gsgs");
#else
        wxPrintf("gsgs IDE v%s\n", GSGS_VERSION);
#endif
        return false;
    }

    // When launching gsgs as a debugger interface, disable the plugins
    if(parser.Found(wxT("n")) || m_startedInDebuggerMode)
    {
        // Load gsgs without plugins
        SetPluginLoadPolicy(PP_None);
    }

    wxString plugins;
    if(parser.Found(wxT("p"), &plugins)) 
    {
        wxArrayString pluginsArr = ::wxStringTokenize(plugins, wxT(","));
        // Trim and make lower case
        for(size_t i = 0; i < pluginsArr.GetCount(); i++) {
            pluginsArr.Item(i).Trim().Trim(false).MakeLower();
        }

        // Load gsgs without plugins
        SetAllowedPlugins(pluginsArr);
        SetPluginLoadPolicy(PP_FromList);
    }

    wxString newBaseDir(wxEmptyString);
    if(parser.Found(wxT("b"), &newBaseDir)) {
#if defined(__WXMSW__)
        homeDir = newBaseDir;
#else
        wxLogDebug("Ignoring the Windows-only --basedir option as not running Windows");
#endif
    }

    // Set the log file verbosity. NB Doing this earlier seems to break wxGTK debug output when debugging gsgs
    // itself :/
    int temp;
    gsgs_Config().Read(kConfigLogVerbosity, temp, FileLogger::Error);
    FileLogger::OpenLog("gsgs.log", temp);
    gsgs_Debug(wxT("Starting gsgs..."));

    // Copy gdb pretty printers from the installation folder to a writeable location
    // this is  needed because python complies the files and in most cases the user
    // running gsgs has no write permissions to /usr/share/gsgs/...
    DoCopyGdbPrinters();

    // Since GCC 4.8.2 gcc has a default colored output
    // which breaks gsgs output parsing
    // to disable this, we need to set GCC_COLORS to an empty
    // string.
    ::wxSetEnv("GCC_COLORS", "");

#if defined(__WXGTK__)
    if(homeDir.IsEmpty()) {
        homeDir = gsgs_Path().GetUserDataDir(); // By default, ~/Library/Application Support/gsgs or ~/.gsgs
        if(!wxFileName::Exists(homeDir)) {
            wxLogNull noLog;
            wxFileName::Mkdir(homeDir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            wxCHECK_MSG(wxFileName::DirExists(homeDir), false, "Failed to create the requested data dir");
        }

        // Create the directory structure
        wxLogNull noLog;
        wxMkdir(homeDir);
        wxMkdir(homeDir + wxT("/lexers/"));
        wxMkdir(homeDir + wxT("/rc/"));
        wxMkdir(homeDir + wxT("/images/"));
        wxMkdir(homeDir + wxT("/templates/"));
        wxMkdir(homeDir + wxT("/config/"));
        wxMkdir(homeDir + wxT("/tabgroups/"));

        // copy the settings from the global location if needed
        wxString installPath(INSTALL_DIR, wxConvUTF8);
        if(!CopySettings(homeDir, installPath)) return false;
        gsgs_Manager()->SetInstallDir(installPath);

    } else {
        wxFileName fn(homeDir);
        fn.MakeAbsolute();
        gsgs_Manager()->SetInstallDir(fn.GetFullPath());
    }

#elif defined(__WXMAC__)
    homeDir = gsgs_Path().GetUserDataDir();
    if(!wxFileName::Exists(homeDir)) {
        wxLogNull noLog;
        wxFileName::Mkdir(homeDir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        wxCHECK_MSG(wxFileName::DirExists(homeDir), false, "Failed to create the requested data dir");
    }

    {
        wxLogNull noLog;

        // Create the directory structure
        wxMkdir(homeDir);
        wxMkdir(homeDir + wxT("/lexers/"));
        wxMkdir(homeDir + wxT("/rc/"));
        wxMkdir(homeDir + wxT("/images/"));
        wxMkdir(homeDir + wxT("/templates/"));
        wxMkdir(homeDir + wxT("/config/"));
        wxMkdir(homeDir + wxT("/tabgroups/"));
    }

    wxString installPath(MacGetBasePath());
    gsgs_Manager()->SetInstallDir(installPath);
    // copy the settings from the global location if needed
    CopySettings(homeDir, installPath);

#else //__WXMSW__
    if(homeDir.IsEmpty()) 
    { // did we got a basedir from user?
#ifdef USE_POSIX_LAYOUT
        homeDir = gsgs_Path().GetDataDir();
#else
        homeDir = ::wxGetCwd();
#endif
    }
    wxFileName fnHomdDir(homeDir + wxT("/"));

    // try to locate the menu/rc.xrc file
    wxFileName fn(homeDir + wxT("/config"), wxT("menu.xrc"));
    if(!fn.FileExists()) 
    {
        // we got wrong home directory
        wxFileName appFn(wxAppBase::argv[0]);
        homeDir = appFn.GetPath();
    }

    if(fnHomdDir.IsRelative()) 
    {
        fnHomdDir.MakeAbsolute();
        homeDir = fnHomdDir.GetPath();
    }

    gsgs_Path().SetInstallDir(homeDir);
#endif

    // Use our persistence manager (which uses wxFileConfig instead of the registry...)
    mPersistencManager = new gsgs::PersistenceManager();
    wxPersistenceManager::Set(*mPersistencManager);

    // Make sure we have an instance if the keyboard manager allocated before we create the main frame class
    // (the keyboard manager needs to connect to the main frame events)
    gsgs_Window();
    PluginManager::Get();
    gsgs_Path().SetOriginalCwd(wxGetCwd());
    ::wxSetWorkingDirectory(homeDir);
    // Load all of the XRC files that will be used. You can put everything
    // into one giant XRC file if you wanted, but then they become more
    // diffcult to manage, and harder to reuse in later projects.
    // The menubar
    if(!wxXmlResource::Get()->Load(DoFindMenuFile(gsgs_Path().GetInstallDir(), wxT("2.0"))))
        return false;

    // keep the startup directory
    gsgs_Path().SetStartupDirectory(::wxGetCwd());

    // set the performance output file name
    PERF_OUTPUT(wxString::Format(wxT("%s/gsgs.perf"), wxGetCwd().c_str()).mb_str(wxConvUTF8));

    // set the CTAGS_REPLACEMENT environment variable
    wxFileName ctagsReplacements(gsgs_Path().GetUserDataDir(), "ctags.replacements");
    wxSetEnv(wxT("CTAGS_REPLACEMENTS"), ctagsReplacements.GetFullPath());

// read the last frame size from the configuration file
// Initialise editor configuration files
#ifdef __WXMSW__
    {
        wxLogNull noLog;
        wxFileName::Mkdir(gsgs_Path().GetUserDataDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
    }
#endif
    
    gsgs_Style(); // Initialise the custom settings _before_ we start constructing the main frame

    gsgs::EditorConfig * cfg = &gsgs_EditorConfig();
    cfg->SetInstallDir(gsgs_Path().GetInstallDir());
    cfg->Init(GSGS_VERSION, wxT("2.0.2"));
    if(!cfg->Load()) 
    {
        gsgs_Error(wxT("Failed to load configuration file: %s/config/gsgs.xml"), wxGetCwd().c_str());
        return false;
    }

#ifdef __WXGTK__
    bool redirect = gsgs_Config().Read(kConfigRedirectLogOutput, true);
    if(redirect) {
        // Redirect stdout/error to a file
        wxFileName stdout_err(gsgs_Path().GetUserDataDir(), "gsgs-stdout-stderr.log");
        FILE* new_stdout = ::freopen(stdout_err.GetFullPath().mb_str(wxConvISO8859_1).data(), "a+b", stdout);
        FILE* new_stderr = ::freopen(stdout_err.GetFullPath().mb_str(wxConvISO8859_1).data(), "a+b", stderr);
        wxUnusedVar(new_stderr);
        wxUnusedVar(new_stdout);
    }
#endif
    EvnVarList vars;
    gsgs_EditorConfig().ReadObject(wxT("Variables"), &vars);
    vars.InsertVariable(wxT("Default"), wxT("gsgsDir"), gsgs_Path().GetInstallDir());
    gsgs_EditorConfig().WriteObject(wxT("Variables"), &vars);

#ifdef __WXMSW__
    MSWReadRegistry();
    //wxSystemOptions::SetOption(wxT("msw.window.no-clip-children"), 1);
#endif

    // Set up the locale if appropriate
    if(gsgs_EditorConfig().GetOptions()->GetUseLocale()) 
    {
        int preferredLocale = wxLANGUAGE_ENGLISH;
        // The locale had to be saved as the canonical locale name, as the wxLanguage enum wasn't consistent between wx
        // versions
        wxString preferredLocalename = gsgs_EditorConfig().GetOptions()->GetPreferredLocale();
        if(!preferredLocalename.IsEmpty()) {
            const wxLanguageInfo* info = wxLocale::FindLanguageInfo(preferredLocalename);
            if(info) {
                preferredLocale = info->Language;
                if(preferredLocale == wxLANGUAGE_UNKNOWN) { preferredLocale = wxLANGUAGE_ENGLISH; }
            }
        }

#if defined(__WXGTK__)
        // Cater for a --prefix= build. This gets added automatically to the search path for catalogues.
        // So hack in the standard ones too, otherwise wxstd.mo will be missed
        wxLocale::AddCatalogLookupPathPrefix(wxT("/usr/share/locale"));
        wxLocale::AddCatalogLookupPathPrefix(wxT("/usr/local/share/locale"));

#elif defined(__WXMSW__)
#ifdef USE_POSIX_LAYOUT
        wxLocale::AddCatalogLookupPathPrefix(gsgs_Path().GetPosixInstallDir() + wxT("\\share\\locale"));
#else
        wxLocale::AddCatalogLookupPathPrefix(gsgs_Path().GetInstallDir() + wxT("\\locale"));
#endif
#endif
        // This has to be done before the catalogues are added, as otherwise the wrong one (or none) will be found
        m_locale.Init(preferredLocale);

        bool mo = m_locale.AddCatalog(wxT("gsgs"));
        if(!mo) {
            m_locale.AddCatalog(wxT("GSGS")); // Hedge bets re our spelling
        }
    } else {
        // For proper encoding handling by system libraries it's needed to inialize locale even if UI translation is
        // turned off
        m_locale.Init(wxLANGUAGE_ENGLISH, wxLOCALE_DONT_LOAD_DEFAULT);
    }

    checkRunPath();

    gsgs_Style().Load();

    mMainFrame = gsgs::Frame::setup((parser.GetParamCount() == 0) && !m_startedInDebuggerMode);

    delete m_splash;
    m_splash = 0;

    mMainFrame->Show(TRUE);
    mMainFrame->Layout();
    SetTopWindow(mMainFrame);

    long lineNumber(0);
    parser.Found(wxT("l"), &lineNumber);
    if(lineNumber > 0) 
    {
        lineNumber--;
    } 
    else 
    {
        lineNumber = 0;
    }

    if(!m_startedInDebuggerMode)
    {
        for(size_t i = 0; i < parser.GetParamCount(); i++) 
        {
            OpenItem(parser.GetParam(i), lineNumber);
        }
    }

    gsgsLogMessage(wxString::Format(wxT("Install path: %s"), gsgs_Path().GetInstallDir().c_str()));
    gsgsLogMessage(wxString::Format(wxT("Startup Path: %s"), gsgs_Path().GetStartupDirectory().c_str()));

#ifdef __WXGTK__
    // Needed on GTK
    if(gsgs_Frame()->GetMainBook()->GetActiveEditor() == NULL) 
    {
        gsgs_Frame()->GetOutputPane()->GetBuildTab()->SetFocus();
    }
#endif

    ///////////////////////////////////////////////////////////////
    m_docManager = new wxDocManager;

    // Create a template relating drawing documents to their views
    new wxDocTemplate(m_docManager, wxT("Pixel Map"), wxT("*.xml;*.imageset"), wxT(""), wxT("xml"), wxT("Editor Document"), wxT("Editor View"),
        CLASSINFO(gsgs::ImageDocument), CLASSINFO(gsgs::ImageEditorView));

    // Create templates relating drawing documents to their views
    new wxDocTemplate(m_docManager, wxT("GUI layouts"), wxT("*.layout"), wxT(""), wxT("layout"), wxT("Editor Document"), wxT("Editor View"),
        CLASSINFO(EditorDocument), CLASSINFO(EditorView));

    new wxDocTemplate(m_docManager, wxT("Xml files"), wxT("*.xml"), wxT(""), wxT("layout"), wxT("Editor Document"), wxT("Editor View"),
        CLASSINFO(EditorDocument), CLASSINFO(EditorView));

    new wxDocTemplate(m_docManager, wxT("All files"), wxT("*.*"), wxT(""), wxT("layout"), wxT("Editor Document"), wxT("Editor View"),
        CLASSINFO(EditorDocument), CLASSINFO(EditorView));

    // If we've only got one window, we only get to edit
    // one document at a time.
    m_docManager->SetMaxDocsOpen(1);

#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES
    // Register VEH exception handler that will be responsible for obtaining
    // the thrown exceptions (both CPP & win32 fatal ones) stack traces, codes and flags
    // NOTE: Only applicable on windows platforms
    ExceptionManager::GetInstancePtr()->RegisterExceptionHandler();
#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

#if wxUSE_ON_FATAL_EXCEPTION
    // Make sure that we enable fatal exceptions in debug and under VS. Fatal exceptions (also 
    // known as general protection faults under Windows or segmentation violations in the 
    // Unix world) will be caught and passed to GsGsApp::OnFatalException. By default, 
    // i.e. before this function is called, they will be handled in the normal way 
    // which usually just means that the application will be terminated.
    wxHandleFatalExceptions(true);
#endif // wxUSE_ON_FATAL_EXCEPTION

    // Force the application to exit when we delete the top-level frame (i.e. GetTopWindow()).
    // NOTE: This allows us to kill the application gracefully when an unhandled exception occurs
    SetExitOnFrameDelete(true);

    // Pre-load the ini file
    UILayoutOptions::GetInstancePtr();

    // Especially with the OutputView open, gsgs was consuming 50% of a cpu, mostly in updateui
    // The next line limits the frequency of UpdateUI events to every 100ms
    //wxUpdateUIEvent::SetUpdateInterval(200);
    return TRUE;
}
//-----------------------------------------------------------------------
int GsGsApp::OnExit()
{
    gsgs_EditorConfig().Release();
    gsgs_Style().Release();
    gsgs_Config().Release();
    gsgs_Image().Release();
    if (m_docManager)
        delete m_docManager;
    delete UILayoutOptions::GetInstancePtr();
    return 0;
}
//-----------------------------------------------------------------------
void GsGsApp::SetModificationStateAllDocuments(bool modificationState) const
{
    // Validations
    wxASSERT_MSG(m_docManager != NULL, wxT("Document Manager wasn't properly initialized"));

    try
    {
        // Obtain a list of all currently open documents
        wxList openDocuments = m_docManager->GetDocuments();

        // Iterate previous list
        wxList::iterator iter;
        for (iter = openDocuments.begin(); iter != openDocuments.end(); ++iter)
        {
            // Obtain current document
            wxDocument* const currentDoc = wx_static_cast(wxDocument*, *iter);

            // If the document's current state doesn't match the desired one,
            // change it
            if (currentDoc && (currentDoc->IsModified() != modificationState))
                currentDoc->Modify(modificationState);
        }
    }
    // If something goes wrong, log and show user
    SHOW_EXCEPTION("GsGsApp::SetModificationStateAllDocuments", "Error", "Unexpected standard exception while setting modification state", "Unexpected non-standard exception while setting modification state");
}
//-----------------------------------------------------------------------
void GsGsApp::OnUnhandledException()
{
    // Log unhandled exception (unfortunately, exception type is lost by now)
    LogSilentError(wxT("GsGsApp::OnUnhandledException - Unhandled exception caught, program will terminate."));

    // Show the user the unhandled exception if stipulated
#if SHOW_UNHANDLED
    // Exception type is lost by now, so inform user that an unhandled exception
    // occurred and exit the application. Depending on current configuration,
    // we use modal or modeless message boxes.
    ExceptionMessageBox(wxT("GsGsApp::OnUnhandledException - Unhandled exception caught, program will terminate."),
        wxT("GsGsApp Unhandled Exception"),
        LE_MB_TYPE_OK | LE_MB_ICON_STOP | LE_MB_SYSTEMMODAL);
#endif // SHOW_UNHANDLED
}

//-----------------------------------------------------------------------
#if wxUSE_DEBUGREPORT
void GsGsApp::GenerateDebugReport(wxDebugReport::Context ctx, const bool showPreview) const
{
    // Create a debug report with the program's current state. All the files
    // that we'll be added to it will be compressed into a single .ZIP file.
    wxDebugReportCompress *report = new wxDebugReportCompress;

    // Adds all available information to the report. Currently this includes a 
    // text (XML) file describing the process context (system info + stack trace) 
    // and, under Win32, a minidump file.
    report->AddAll(ctx);

    // Create a timestamp file which contains the date and time of the crash.
    wxFileName fn(report->GetDirectory(), wxT("timestamp.my"));
    wxFFile file(fn.GetFullPath(), wxT("w"));
    if (file.IsOpened())
    {
        const wxDateTime dt = wxDateTime::Now();
        (void)file.Write(dt.FormatISODate() + wxT(' ') + dt.FormatISOTime());
        (void)file.Close();
    }

    // Add timestamp file to debug report
    report->AddFile(fn.GetFullName(), wxT("timestamp of this report"));

    // We also add the current log file (GsGsApp.log) and configuration 
    // file (GsGsApp.ini) to the debug report
#ifdef __WINDOWS__
    report->AddFile(wxGetCwd() + wxT("\\GsGsApp.log"), wxT("GsGsApp log file"));
    report->AddFile(wxGetCwd() + wxT("\\GsGsApp.ini"), wxT("GsGsApp configuration file"));
#else
    report->AddFile(wxGetCwd() + wxT("/GsGsApp.log"), wxT("GsGsApp log file"));
    report->AddFile(wxGetCwd() + wxT("/GsGsApp.ini"), wxT("GsGsApp configuration file"));
#endif // __WINDOWS__

    // Display a preview window if so desired to the let the user review the report and 
    // decide what to do (delete files and/or add notes). The user can also abort the 
    // report generation by pressing the cancel button. If no preview is required, the report 
    // will be created automatically
    bool generateReport = true;
    if (showPreview && !wxDebugReportPreviewStd().Show(*report))
        generateReport = false;

    // Did the user allow the report or are we to generate it automatically?
    if (generateReport)
    {
        // Save all report data to disk
        if (report->Process())
        {
            // Inform user that operation was successful
            // NOTE: Some exceptions, like some access violations, prevent somehow the log
            // message from executing correctly and thus no log is written to disk. The report
            // is correctly generated and saved though.
            LogDebug(wxT("Report generated in \"%s\"."), report->GetCompressedFileName().c_str());
            report->Reset();
        }
        else
            // Ops, something went wrong
            LogError(wxT("GsGsApp::GenerateDebugReport - Failed to generate debug report!"));
    }

    // The report is no longer needed
    delete report;
}
#endif

//-----------------------------------------------------------------------
void GsGsApp::HandleEvent(wxEvtHandler *handler, wxEventFunction func, wxEvent& event) const
{
    try
    {
        wxApp::HandleEvent(handler, func, event);
    }
    catch (NII::Exception& e)
    {
        ProcessUnhandledException(wxT("CEGUI"), StrHelper::ToWXString(e.getVerbose()), StrHelper::ToWXString(e.getFile()), e.getLine());
    }
    catch (std::exception& ex)\
    {
        ProcessUnhandledException(wxT("standard"), ex.what());
    }
    catch (...)
    {
        ProcessUnhandledException(wxT("non-standard"), wxT("Exception message not available!"));
    }
}
//-----------------------------------------------------------------------
// NOTE: We cannot use debug reports here because they are only meant for fatal exceptions...
void GsGsApp::ProcessUnhandledException(const wxString& exceptionType, const wxString& exceptionText, const wxString& exceptionFileName, const int exceptionLine) const
{
    // Disable all windows in the application
    // NOTE: This is done in the class's constructor
    wxWindowDisabler *disableAllWindows = new wxWindowDisabler();

    // Check if we are to exit the application when an unhandled exception is encountered
    // NOTE: This setting only has any impact in debug builds and should only
    // be used when debugging a specific problem. If we're doing a release
    // build, the application ALWAYS terminates (this avoids endless loops problems that
    // could occur otherwise).
#if TERMINATE_ON_UNHANDLED || !defined(_DEBUG)   
    // Obtain exception info, composed of basic info (description, location, message), line and 
    // file info (for CEGUI exceptions) and VEH info if under Windows (stack trace, exception code, type & flags)
    const wxString exceptionInfo = ExceptionManager::GetInstancePtr()->GetExceptionInfo(wxT("GsGsApp::ProcessUnhandledException"), wxString::Format(wxT("GsGsApp has encountered an unhandled %s exception and needs to terminate."), exceptionType.c_str()), exceptionText, exceptionFileName, exceptionLine);

    // Log exception
    LogSilentError(exceptionInfo);

    // Display exception to user if so desired (using modal or modeless message boxes)
#if SHOW_UNHANDLED
    ExceptionMessageBox(exceptionInfo,
        wxT("GsGsApp Unhandled Exception"),
        LE_MB_TYPE_OK | LE_MB_ICON_STOP | LE_MB_SYSTEMMODAL);
#endif // SHOW_UNHANDLED

    // If we need to prevent the saving of the file, make sure all open documents have the 
    // modify flag set to false
    // NOTE: Be careful if you allow the user to decide if the file should be saved since some 
    // exceptions might create an unstable system state that might invalidate the file save,
    // thus corrupting the layout file (rare).
#if PREVENT_SAVE_ON_UNHANDLED
    SetModificationStateAllDocuments(false);
#endif // PREVENT_SAVE_ON_UNHANDLED

    // Close the top window
    // NOTE: Since we are setting the SetExitOnFrameDelete(true) in 
    // the GsGsApp::OnInit() function, this operation will
    // gracefully cleanup the application before exit
    (void)this->GetTopWindow()->Close();

    if (m_docManager)
        delete m_docManager;
    delete UILayoutOptions::GetInstancePtr();

    // Reactivate all the windows
    // NOTE: This is done in the class's destructor
    delete disableAllWindows;
    disableAllWindows = NULL;

    // Exit the application with a failure code
    exit(EXIT_FAILURE);
#else

    // Obtain exception info, composed of basic info (description, location, message), line and 
    // file info (for CEGUI exceptions) and VEH info if under Windows (stack trace, exception code, type & flags)
    const wxString exceptionInfo = ExceptionManager::GetInstancePtr()->GetExceptionInfo(wxT("GsGsApp::ProcessUnhandledException"), wxString::Format(wxT("GsGsApp has encountered an unhandled %s exception."), exceptionType), exceptionText, exceptionFileName, exceptionLine);

    // The application will keep on running, so inform the user about it (or just 
    // log the exception)
#if SHOW_UNHANDLED
    LogError(exceptionInfo);
#else
    LogSilentError(exceptionInfo);
#endif // SHOW_UNHANDLED

    // Reactivate all the windows
    // NOTE: This is done in the class's destructor
    delete disableAllWindows;
    disableAllWindows = NULL;
#endif // TERMINATE_ON_UNHANDLED
}
//-----------------------------------------------------------------------
bool GsGsApp::CopySettings(const wxString& destDir, wxString& installPath)
{
    wxLogNull noLog;

    ///////////////////////////////////////////////////////////////////////////////////////////
    // copy new settings from the global installation location which is currently located at
    // /usr/local/share/gsgs/ (Linux) or at gsgs.app/Contents/SharedSupport
    ///////////////////////////////////////////////////////////////////////////////////////////
    CopyDir(installPath + wxT("/templates/"), destDir + wxT("/templates/"));
    massCopy(installPath + wxT("/images/"), wxT("*.png"), destDir + wxT("/images/"));
    // wxCopyFile(installPath + wxT("/config/menu.xrc"), destDir + wxT("/config/menu.xrc"));
    wxCopyFile(installPath + wxT("/index.html"), destDir + wxT("/index.html"));
    wxCopyFile(installPath + wxT("/svnreport.html"), destDir + wxT("/svnreport.html"));
    wxCopyFile(installPath + wxT("/astyle.sample"), destDir + wxT("/astyle.sample"));
    wxCopyFile(installPath + wxT("/php.sample"), destDir + wxT("/php.sample"));
    return true;
}

void GsGsApp::OnFatalException()
{
#if wxUSE_STACKWALKER
    wxString startdir;
    startdir << gsgs_Path().GetUserDataDir() << wxT("/gsgscrash.log");

    wxFileOutputStream outfile(startdir);
    wxTextOutputStream out(outfile);
    out.WriteString(wxDateTime::Now().FormatDate() + wxT(" - ") + wxDateTime::Now().FormatTime() + wxT("\n"));
    gsgs::StackWalker walker(&out);
    walker.Walk();
    wxAppBase::ExitMainLoop();
#endif

    // Check if wxWidgets was built with debug report support
#if wxUSE_DEBUGREPORT
    // We have debug reports, so so inform the user of the fatal error and the possibility of a
    // debug report generation
    // NOTE: Unlike unhandled exceptions which can be customized, fatal exceptions 
    // are ALWAYS shown to user
    ExceptionMessageBox(wxT("GsGsApp::OnFatalException - GsGsApp has encountered a fatal exception and needs to terminate. A debug report will be generated if you desire."),
        wxT("GsGsApp Fatal Exception"), LE_MB_TYPE_OK | LE_MB_ICON_STOP | LE_MB_SYSTEMMODAL);

    // Create a debug report (with the current system state, stack trace, log file and configuration file) that
    // the user can save or ignore
    GenerateDebugReport(wxDebugReport::Context_Exception);
#else
    // No debug report, so inform user of fatal error and exit application
    // NOTE: Unlike unhandled exceptions which can be customized, fatal exceptions 
    // are ALWAYS shown to user
    CELEMessageBox(wxT("GsGsApp::OnFatalException - GsGsApp has encountered a fatal exception and needs to terminate. We're sorry for the inconvenience!"),
        wxT("GsGsApp Fatal Exception"), CELE_MB_TYPE_OK | CELE_MB_ICON_STOP | CELE_MB_SYSTEMMODAL);
#endif // wxUSE_DEBUGREPORT  
}

bool GsGsApp::IsSingleInstance(const wxCmdLineParser& parser)
{
    bool tempb;
    gsgs_Config().Read(kConfigSingleInstance, tempb, false);
    if(tempb) {
        wxString name = wxString::Format(wxT("GuangShanGuShi-%s"), wxGetUserId().c_str());

        wxString path;
#ifndef __WXMSW__
        path = "/tmp";
#endif
        m_only = new wxSingleInstanceChecker(name, path);
        if(m_only->IsAnotherRunning()) {
            // prepare commands file for the running instance
            wxArrayString files;
            for(size_t i = 0; i < parser.GetParamCount(); i++) {
                wxString argument = parser.GetParam(i);

                // convert to full path and open it
                wxFileName fn(argument);
                fn.MakeAbsolute();
                files.Add(fn.GetFullPath());
            }

            try {
                gsgs::SocketClient client;
                bool dummy;
                client.ConnectRemote("127.0.0.1", SINGLE_INSTANCE_PORT, dummy);

                JSON json(cJSON_Object);
                json.getObject().addProperty("args", files);
                wxString targ;
                json.getObject().getString(targ);
                client.WriteMessage(targ);
                return false;

            } catch(gsgs::SocketException& e) {
                gsgs_Error("Failed to send single instance request: %s", e.what());
            }
        }
    }
    return true;
}

void GsGsApp::MacOpenFile(const wxString& fileName)
{
    switch(FileUtil::GetType(fileName)) {
    case FileUtil::TypeWorkspace:
        gsgs_Manager()->OpenWorkspace(fileName);
        break;
    default:
        gsgs_Frame()->GetMainBook()->OpenFile(fileName);
        break;
    }
}

void GsGsApp::MSWReadRegistry()
{
#ifdef __WXMSW__

    EvnVarList vars;
    //gsgs_EditorConfig().Load();
    gsgs_EditorConfig().ReadObject(wxT("Variables"), &vars);

    /////////////////////////////////////////////////////////////////
    // New way of registry:
    // Read the registry INI file
    /////////////////////////////////////////////////////////////////

    // Update PATH environment variable with the install directory and
    // MinGW default installation (if exists)
    wxString pathEnv;
    wxGetEnv(wxT("PATH"), &pathEnv);

    wxString gsgsInstallDir;
    gsgsInstallDir << gsgs_Path().GetInstallDir() << wxT(";");
    pathEnv.Prepend(gsgsInstallDir);

    // Load the registry file
    wxString iniFile;
    iniFile << gsgs_Path().GetInstallDir() << gsgsPathSep << wxT("registry.ini");

    if(wxFileName::FileExists(iniFile)) {
        gsgs::Registry::SetFilename(iniFile);
        gsgs::Registry registry;

        m_parserPaths.Clear();
        wxString strWx, strMingw, strUnitTestPP;
        // registry.Read(wxT("wx"),         strWx);
        registry.Read(wxT("mingw"), strMingw);
        registry.Read(wxT("unittestpp"), strUnitTestPP);

        // Supprot for wxWidgets
        if(strWx.IsEmpty() == false) 
        {
            // we have WX installed on this machine, set the path of WXWIN & WXCFG to point to it
            EnvMap envs = vars.GetVariables(wxT("Default"), false, wxEmptyString, wxEmptyString);

            if(!envs.Contains(wxT("WXWIN"))) 
            {
                vars.AddVariable(wxT("Default"), wxT("WXWIN"), strWx);
                vars.AddVariable(wxT("Default"), wxT("PATH"), wxT("$(WXWIN)\\lib\\gcc_dll;$(PATH)"));
            }

            if(!envs.Contains(wxT("WXCFG"))) vars.AddVariable(wxT("Default"), wxT("WXCFG"), wxT("gcc_dll\\mswu"));

            gsgs_EditorConfig().WriteObject(wxT("Variables"), &vars);
            wxSetEnv(wxT("WX_INCL_HOME"), strWx + wxT("\\include"));
        }

        // Support for UnitTest++
        if(strUnitTestPP.IsEmpty() == false) 
        {
            // we have UnitTest++ installed on this machine
            EnvMap envs = vars.GetVariables(wxT("Default"), false, wxEmptyString, wxEmptyString);

            if(!envs.Contains(wxT("UNIT_TEST_PP_SRC_DIR"))) 
            {
                vars.AddVariable(wxT("Default"), wxT("UNIT_TEST_PP_SRC_DIR"), strUnitTestPP);
            }

            gsgs_EditorConfig().WriteObject(wxT("Variables"), &vars);
        }

        // Support for MinGW
        if(strMingw.IsEmpty() == false) 
        {
            // Make sure that gsgs's MinGW comes first before any other
            // MinGW installation that might exist on the machine
            strMingw << gsgsPathSep << wxT("bin;");
            pathEnv.Prepend(strMingw);
        }
        wxSetEnv(wxT("PATH"), pathEnv);
    }
#endif
}

wxString GsGsApp::DoFindMenuFile(const wxString& installDirectory, const wxString& requiredVersion)
{
    wxString dMenu = installDirectory + gsgsPathSep + wxT("config") + gsgsPathSep + wxT("menu.xrc");
    wxFileName menuFile(gsgs_Path().GetUserDataDir() + gsgsPathSep + wxT("config") +
                        gsgsPathSep + wxT("menu.xrc"));
    if(menuFile.FileExists()) {
        // if we find the user's file menu, check that it has the required version
        {
            wxLogNull noLog;
            wxXmlDocument doc;
            if(doc.Load(menuFile.GetFullPath())) {
                wxString version = doc.GetRoot()->XmlGetPropVal(wxT("version"), wxT("1.0"));
                if(version != requiredVersion) { return dMenu; }
            }
        }
        return menuFile.GetFullPath();
    }
    return dMenu;
}

void GsGsApp::DoCopyGdbPrinters()
{
    wxFileName printersInstallDir;
#ifdef __WXGTK__
    printersInstallDir = wxFileName(wxString(INSTALL_DIR, wxConvUTF8), "gdb_printers");
#else
    printersInstallDir = wxFileName(gsgs_Path().GetDataDir(), "gdb_printers");
#endif

    // copy the files to ~/.gsgs/gdb_printers
    wxLogNull nolog;
    wxFileName targetDir(gsgs_Path().GetUserDataDir(), "gdb_printers");
    ::wxMkdir(targetDir.GetFullPath());
    ::CopyDir(printersInstallDir.GetFullPath(), targetDir.GetFullPath());
}
void GsGsApp::checkRunPath()
{
#if defined(__WXMSW__)
    wxChar pathsep(wxT(';'));
#else
    wxChar pathsep(wxT(':'));
#endif
    wxString oldpath;
    wxString newpath;
    wxGetEnv(wxT("PATH"), &oldpath);
    wxFileName execfpath(gsgs_Path().GetExecutablePath());
    wxSetEnv(wxT("PATH"), oldpath + pathsep + execfpath.GetPath());
    wxGetEnv(wxT("PATH"), &newpath);

#ifdef __WXMSW__
    if (::IsCygwinEnv()) {
        gsgs_System("Cygwin environment detected");
        wxString cygwinRootDir;
        CompilerLocatorCygwin cygwin;
        if (cygwin.Locate()) {
            // this will return the base folder for cygwin (e.g. D:\cygwin)
            cygwinRootDir = (*cygwin.GetCompilers().begin())->GetInstallationPath();
        }

        // Running under Cygwin
        // Adjust the PATH environment variable
        wxString pathEnv;
        ::wxGetEnv("PATH", &pathEnv);

        // Always add the default paths
        wxArrayString paths;
        if (!cygwinRootDir.IsEmpty()) {
            gsgs_System("Cygwin root folder is: %s", cygwinRootDir);
            wxFileName cygwinBinFolder(cygwinRootDir, "");
            cygwinBinFolder.AppendDir("bin");
            paths.Add(cygwinBinFolder.GetPath());
        }

        paths.Add("/usr/local/bin");
        paths.Add("/usr/bin");
        paths.Add("/usr/sbin");
        paths.Add("/bin");
        paths.Add("/sbin");

        // Append the paths from the environment variables
        wxArrayString userPaths = ::wxStringTokenize(pathEnv, ";", wxTOKEN_STRTOK);
        paths.insert(paths.end(), userPaths.begin(), userPaths.end());

        wxString fixedPath;
        for (size_t i = 0; i < paths.GetCount(); ++i) {
            wxString& curpath = paths.Item(i);
            static wxRegEx reCygdrive("/cygdrive/([A-Za-z])");
            if (reCygdrive.Matches(curpath)) {
                // Get the drive letter
                wxString volume = reCygdrive.GetMatch(curpath, 1);
                volume << ":";
                reCygdrive.Replace(&curpath, volume);
            }

            fixedPath << curpath << ";";
        }
        ::wxSetEnv("PATH", fixedPath);
    }

    if (IsMSYSEnv()) {
        gsgs_System("MSYS environment detected");
        wxString pathEnv;
        ::wxGetEnv("PATH", &pathEnv);

        // Always add the default paths
        wxArrayString paths;

        wxString rootDir = "/"; // Default
        // determine the baseroot of the MSYS installation
        wxString msysRoot = ProcUtils::SafeExecuteCommand("sh -c 'cd / && pwd -W'");
        if (!msysRoot.IsEmpty()) {
            gsgs_System("MSYS Root folder is set to: %s", msysRoot);
            msysRoot.Trim().Trim(false);
            rootDir.Clear();
            rootDir << msysRoot << "/";
        }

        paths.Add(rootDir + "usr/local/bin");
        paths.Add(rootDir + "usr/bin");
        paths.Add(rootDir + "usr/sbin");
        paths.Add(rootDir + "bin");
        paths.Add(rootDir + "sbin");

        // Append the paths from the environment variables
        wxArrayString userPaths = ::wxStringTokenize(pathEnv, ";", wxTOKEN_STRTOK);
        paths.insert(paths.end(), userPaths.begin(), userPaths.end());

        wxString fixedPath = ::wxJoin(paths, ';');
        gsgs_Debug("Setting PATH environment variable to:\n%s", fixedPath);
        ::wxSetEnv("PATH", fixedPath);
    }
#endif
}

void GsGsApp::PrintUsage(const wxCmdLineParser& parser)
{
#ifdef __WXMSW__
    parser.Usage();
#else
    wxString usageString = parser.GetUsageString();
    std::cout << usageString.mb_str(wxConvUTF8).data() << std::endl;
#endif
}

void GsGsApp::OpenFolder(const wxString& path)
{
    wxArrayString files;
    if(wxDir::GetAllFiles(path, &files, "*.workspace", wxDIR_FILES) == 1) {
        gsgs_Manager()->OpenWorkspace(files.Item(0));
    } else {
        gsgs_Frame()->GetFileExplorer()->OpenFolder(path);
        gsgs_Frame()->GetWorkspacePane()->SelectTab(_("Resource"));
    }
}

void GsGsApp::OpenFile(const wxString& path, long lineNumber)
{
    wxFileName fn(path);
    if(fn.GetExt() == wxT("workspace")) {
        gsgs_Manager()->OpenWorkspace(fn.GetFullPath());
    } else {
        gsgs_Frame()->GetMainBook()->OpenFile(fn.GetFullPath(), wxEmptyString, lineNumber);
    }
}

void GsGsApp::OpenItem(const wxString& path, long lineNumber)
{
    // convert to full path and open it
    if(path == ".") 
    {
        // Open the current folder
        OpenFolder(gsgs_Path().GetOriginalCwd());
    } 
    else 
    {
        wxFileName fn(path);
        fn.MakeAbsolute(gsgs_Path().GetOriginalCwd());
        if(wxFileName::DirExists(fn.GetFullPath())) 
        {
            OpenFolder(fn.GetFullPath());

        } 
        else 
        {
            OpenFile(fn.GetFullPath(), lineNumber);
        }
    }
}
