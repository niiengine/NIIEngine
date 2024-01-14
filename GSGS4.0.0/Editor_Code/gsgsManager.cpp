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

#include "gsgsPreProcess.h"
#include "gsgsCodeCompletionManager.h"
#include "gsgsFileUtil.h"
#include "gsgsConfig.h"
#include "gsgsDebuggerManager.h"
#include "gsgsEditorConfig.h"
#include "gsgsMacroManager.h"
#include "CommonController.h"
#include "CommonController1.h"
#include "WSImporter.h"
#include "gsgsBuildConfig.h"
#include "gsgsToolsetManager.h"
#include "gsgsBuilderGnuMake.h"
#include "NewProjectDialog.h"
#include "gsgsConsole.h"
#include "gsgsWindowManager.h"
#include "gsgsCommonHandler.h"
#include "gsgsWorkspaceManager.h"
#include "gsgsEditor.h"
#include "gsgsContextManager.h"
#include "gsgsTagManager.h"
#include "gsgsCommon.h"
#include "gsgsLogManager.h"
#include "gsgsFrame.h"
#include "globals.h"
#include "gsgsShellCmd.h"
#include "gsgsJob.h"
#include "gsgsLanguage.h"
#include "gsgsManager.h"
#include "NewProjectDialog.h"
#include "gsgsMenuManager.h"
#include "gsgsBuildTab.h"
#include "gsgsParseWorker.h"
#include "gsgsPluginManager.h"
#include "process_impl.h"
#include "gsgsRefactor.h"
#include "gsgsSearchWorker.h"
#include "gsgsSessionManager.h"
#include "gsgsTabgroupManager.h"
#include "gsgsWorkspacePane.h"
#include "gsgsCodeBoxManager.h"
#include "imanager.h"
#include "gsgsWorkspace.h"
#include "plugin.h"

#include <wx/busyinfo.h>
#include <wx/progdlg.h>
#include <wx/regex.h>
#include <wx/aui/auibook.h>
#include <wx/xrc/xmlres.h>
#include <wx/stopwatch.h>
#include <wx/app.h>

#ifndef __WXMSW__
#include <sys/wait.h>
#endif
//----------------------------------------------------------------------------
const wxEventType wxEVT_CMD_RESTART_GSGS = wxNewEventType();
//----------------------------------------------------------------------------
const wxEventType wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED = XRCID("wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED");
//----------------------------------------------------------------------------
namespace gsgs
{
    class DbContentCacher : public Job
    {
        wxString m_filename;
    public:
        DbContentCacher(wxEvtHandler* parent, const wxChar* dbfilename) :
            Job(parent),
            m_filename(dbfilename){}
        virtual ~DbContentCacher() {}
    public:
        virtual void Process(wxThread* thread)
        {
            wxStopWatch sw;
            sw.Start();

            wxString fileContent;
            wxCSConv fontEncConv(wxFONTENCODING_ISO8859_1);
            FileUtil::ReadFileContent(m_filename, fileContent, fontEncConv);

            if (m_parent) {
                wxCommandEvent e(wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED);
                e.SetString(wxString::Format(_("Symbols file loaded into OS file system cache (%ld seconds)"), sw.Time() / 1000)
                    .c_str());
                m_parent->AddPendingEvent(e);
            }
        }
    };
    //----------------------------------------------------------------------------
    class CleanRequest : public ShellCommand {
    public:
	    /**
	     * Construct a compilation clean request. The compiler thread will clean the selected project and all
	     * its dependencies as appeard in the build order dialog
	     * \param projectName the selected project to build
	     */
	    CleanRequest(const QueueCommand &info)
            : ShellCommand(info)
        {
        }

        virtual ~CleanRequest()
        {
        }

	    //process the request
	    virtual void Process(IManager *manager = NULL)
        {
            wxString cmd;
            wxString errMsg;
            gsgsStringMap om;

            BuildSettingsConfig* bsc(manager ? manager->GetBuildSettingsConfigManager() : &gsgs_Settings());
            CxxWS* w(manager ? manager->GetWorkspace() : &gsgs_CppWorker());

            ProjectPtr proj = w->FindProjectByName(m_info.GetProject(), errMsg);
            if(!proj) {
                AppendLine(_("Cant find project: ") + m_info.GetProject());
                return;
            }
            wxString pname(proj->GetName());
            BuildConfigPtr bldConf = w->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
            if(bldConf) {
                // BuilderPtr builder = bm->GetBuilder("Default");
                BuilderPtr builder = bldConf->GetBuilder();
                wxString args = bldConf->GetBuildSystemArguments();
                if(m_info.GetProjectOnly()) {
                    cmd = builder->GetPOCleanCommand(m_info.GetProject(), m_info.GetConfiguration(), args);
                } else {
                    cmd = builder->GetCleanCommand(m_info.GetProject(), m_info.GetConfiguration(), args);
                }

                if(cmd.IsEmpty()) {
                    AppendLine(_("Sorry, there is no 'Clean' command available\n"));
                    return;
                }

                wxString cmpType = bldConf->GetCompilerType();
                if(bldConf) {
                    wxString cmpType = bldConf->GetCompilerType();
                    CompilerPtr cmp = bsc->GetCompiler(cmpType);
                    if(cmp) {
                        // Add the 'bin' folder of the compiler to the PATH environment variable
                        wxString scxx = cmp->GetTool("CXX");
                        scxx.Trim().Trim(false);
                        scxx.StartsWith("\"", &scxx);
                        scxx.EndsWith("\"", &scxx);
                        // Strip the double quotes
                        wxFileName cxx(scxx);
                        wxString pathvar;
                        pathvar << cxx.GetPath() << clPATH_SEPARATOR;

                        // If we have an additional path, add it as well
                        if(!cmp->GetPathVariable().IsEmpty()) {
                            pathvar << cmp->GetPathVariable() << clPATH_SEPARATOR;
                        }
                        pathvar << "$PATH";
                        om["PATH"] = pathvar;
                    }
                }
            } else {
                AppendLine(_("Sorry, couldn't find the Build configuration\n"));
                return;
            }

            // Notify plugins that a compile process is going to start
            BuildEvent event(wxEVT_BUILD_STARTING);
            event.SetProjectName(pname);
            event.SetConfigurationName(m_info.GetConfiguration());

            if(gsgs_Event().ProcessEvent(event)) {
                // the build is being handled by some plugin, no need to build it
                // using the standard way
                return;
            }

            SendStartMsg();
            // Expand the variables of the command
            cmd = ExpandAllVariables(cmd, w, m_info.GetProject(), m_info.GetConfiguration(), wxEmptyString);
            WrapInShell(cmd);
            DirSaver ds;
            DoSetWorkingDirectory(proj, false, false);

            if(m_info.GetProjectOnly()) {
                // need to change directory to project dir
                wxSetWorkingDirectory(proj->GetFileName().GetPath());
            }
            // print the build command
            AppendLine(cmd + wxT("\n"));

            // print the prefix message of the build start. This is important since the parser relies
            // on this message
            if(m_info.GetProjectOnly()) {
                wxString configName(m_info.GetConfiguration());

                // also, send another message to the main frame, indicating which project is being built
                // and what configuration
                wxString text;
                text << wxGetTranslation(CLEAN_PROJECT_PREFIX) << m_info.GetProject() << wxT(" - ") << configName << wxT(" ]");
                text << wxT("----------\n");
                AppendLine(text);
            }

            // apply environment settings
            EnvSetter env(NULL, &om, proj->GetName(), m_info.GetConfiguration());
            m_proc = CreateAsyncProcess(this, cmd);
            if(!m_proc) {

                // remove environment settings applied
                wxString message;
                message << _("Failed to start clean process, command: ") << cmd << _(", process terminated with exit code: 0");
                AppendLine(message);
                return;
            }
        }

	    //setters/getters
	    const wxString &GetProjectName() const { return m_info.GetProject(); }
    };
    //----------------------------------------------------------------------------
    static wxArrayString DoGetTemplateTypes(const wxString& tmplDecl)
    {
        wxArrayString types;
        int depth(0);
        wxString type;

        wxString tmpstr(tmplDecl);
        tmpstr.Trim().Trim(false);

        if(tmpstr.StartsWith(wxT("<"))) { tmpstr.Remove(0, 1); }

        if(tmpstr.EndsWith(wxT(">"))) { tmpstr.RemoveLast(); }
        tmpstr.Trim().Trim(false);

        for(size_t i = 0; i < tmpstr.Length(); i++) {
            wxChar ch = tmpstr.GetChar(i);
            switch(ch) {
            case wxT(','):
                if(depth > 0) {
                    type << wxT(",");
                } else {
                    type.Trim().Trim(false);
                    if(type.Contains(wxT("std::basic_string<char"))) {
                        type = wxT("string");
                    } else if(type.Contains(wxT("std::basic_string<wchar_t"))) {
                        type = wxT("wstring");
                    }
                    types.Add(type);
                    type.Empty();
                }
                break;
            case wxT('<'):
                depth++;
                type << wxT("<");
                break;
            case wxT('>'):
                depth--;
                type << wxT(">");
                break;
            default:
                type << tmpstr.GetChar(i);
                break;
            }
        }

        if(depth == 0 && type.IsEmpty() == false) {
            type.Trim().Trim(false);
            if(type.Contains(wxT("std::basic_string<char"))) {
                type = wxT("string");
            } else if(type.Contains(wxT("std::basic_string<wchar_t"))) {
                type = wxT("wstring");
            }
            types.Add(type);
        }

        return types;
    }
    //----------------------------------------------------------------------------
    Manager::Manager(void)
        : m_shellProcess(NULL)
        , m_programProcess(NULL)
        , m_breakptsmgr(new BreakPointManager)
        , m_isShutdown(false)
        , m_workspceClosing(false)
        , m_dbgCanInteract(false)
        , m_useTipWin(false)
        , m_tipWinPos(wxNOT_FOUND)
        , m_frameLineno(wxNOT_FOUND)
        , m_watchDlg(NULL)
        , m_retagInProgress(false)
        , m_workspaceOpen(false)
        , m_repositionEditor(true)
        , m_ShowAllPanes(true)
    {
        Bind(wxEVT_RESTART_GSGS, &Manager::OnRestart, this);
        Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &Manager::OnProcessOutput, this);
        Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &Manager::OnProcessEnd, this);

        Connect(wxEVT_CMD_RESTART_GSGS, wxCommandEventHandler(Manager::OnCmdRestart), NULL, this);
        Connect(wxEVT_PARSE_THREAD_SCAN_INCLUDES_DONE, wxCommandEventHandler(Manager::OnIncludeFilesScanDone), NULL, this);
        Connect(wxEVT_CMD_DB_CONTENT_CACHE_COMPLETED, wxCommandEventHandler(Manager::OnDbContentCacherLoaded), NULL, this);
        Connect(wxEVT_PARSE_THREAD_SUGGEST_COLOUR_TOKENS, gsgsEventHandler(Manager::OnParserThreadSuggestColourTokens), NULL, this);

        gsgs_Event().Connect(wxEVT_CMD_PROJ_SETTINGS_SAVED, clProjectSettingsEventHandler(Manager::OnProjectSettingsModified), NULL, this);
        gsgs_Event().Connect(wxEVT_BUILD_ENDED, gsgsBuildEventHandler(Manager::OnBuildEnded), NULL, this);
        gsgs_Event().Connect(wxEVT_BUILD_STARTING, gsgsBuildEventHandler(Manager::OnBuildStarting), NULL, this);
        gsgs_Event().Connect(wxEVT_PROJ_RENAMED, gsgsEventHandler(Manager::OnProjectRenamed), NULL, this);

        gsgs_Event().Bind(wxEVT_FINDINFILES_DLG_DISMISSED, &Manager::OnFindInFilesDismissed, this);
        gsgs_Event().Bind(wxEVT_FINDINFILES_DLG_SHOWING, &Manager::OnFindInFilesShowing, this);
        gsgs_Event().Bind(wxEVT_DEBUGGER_REFRESH_PANE, &Manager::OnUpdateDebuggerActiveView, this);
        gsgs_Event().Bind(wxEVT_DEBUGGER_SET_MEMORY, &Manager::OnDebuggerSetMemory, this);
        gsgs_Event().Bind(wxEVT_TOOLTIP_DESTROY, &Manager::OnHideGdbTooltip, this);

        // Add new workspace type
        gsgs_Workspace().RegisterWorkspace(new CxxWS());

        // Instantiate the profile manager
        ProfileHandler::Get();
    }
    //----------------------------------------------------------------------------
    Manager::~Manager(void)
    {
        gsgs_Event().Unbind(wxEVT_TOOLTIP_DESTROY, &Manager::OnHideGdbTooltip, this);
        Unbind(wxEVT_RESTART_GSGS, &Manager::OnRestart, this);
        Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &Manager::OnProcessOutput, this);
        Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &Manager::OnProcessEnd, this);

        Disconnect(wxEVT_CMD_RESTART_GSGS, wxCommandEventHandler(Manager::OnCmdRestart), NULL, this);

        gsgs_Event().Disconnect(wxEVT_CMD_PROJ_SETTINGS_SAVED,
                                         clProjectSettingsEventHandler(Manager::OnProjectSettingsModified), NULL, this);
        gsgs_Event().Disconnect(wxEVT_BUILD_ENDED, gsgsBuildEventHandler(Manager::OnBuildEnded), NULL, this);
        gsgs_Event().Disconnect(wxEVT_BUILD_STARTING, gsgsBuildEventHandler(Manager::OnBuildStarting), NULL, this);
        gsgs_Event().Disconnect(wxEVT_PROJ_RENAMED, gsgsEventHandler(Manager::OnProjectRenamed), NULL, this);
        gsgs_Event().Unbind(wxEVT_FINDINFILES_DLG_DISMISSED, &Manager::OnFindInFilesDismissed, this);
        gsgs_Event().Unbind(wxEVT_FINDINFILES_DLG_SHOWING, &Manager::OnFindInFilesShowing, this);
        gsgs_Event().Unbind(wxEVT_DEBUGGER_REFRESH_PANE, &Manager::OnUpdateDebuggerActiveView, this);
        gsgs_Event().Unbind(wxEVT_DEBUGGER_SET_MEMORY, &Manager::OnDebuggerSetMemory, this);

        // stop background processes
        IDebugger* debugger = DebuggerMgr::Get().GetActiveDebugger();

        if(debugger && debugger->IsRunning()) DbgStop();
        {
            JobQueueSingleton::Instance()->Stop();
            ParseThreadST::Get()->Stop();
            SearchThreadST::Get()->Stop();
        }

        PluginManager::Get()->UnLoad();
        ServiceManager::Get().UnregisterAll();
        DebuggerMgr::Free();
        JobQueueSingleton::Release();
        ParseThreadST::Free();
        TagsManagerST::Free();
        LanguageST::Free();
        gsgs_CppWorker().Release();
        ContextManager::Free();
        gsgs_Build().Release();
        gsgs_Settings().Release();
        SearchThreadST::Free();
        MenuManager::Free();
        gsgs_EditorConfig().Release();

        wxDELETE(m_shellProcess);
        wxDELETE(m_breakptsmgr);
        TabGroupsManager::Free();
        WindowManager::Release();
        CodeBoxManager::Free();
    }
    //----------------------------------------------------------------------------
    void Manager::CreateWorkspace(const wxString& name, const wxString& path)
    {
        // make sure that the workspace pane is visible
        ShowWorkspacePane(gsgs_Frame()->GetWorkspaceTab()->GetCaption());
        wxString errMsg;

        if(gsgs_CppWorker().CreateWorkspace(name, path, errMsg))
        {
            OpenWorkspace(path + gsgsPathSep + name + wxT(".gsgsws"));
        }
        else
        {
            wxMessageBox(errMsg, _("ｹ籃､ｹﾊﾊﾂ"), wxOK | wxICON_HAND);
        }
    }
    //----------------------------------------------------------------------------
    void Manager::OpenWorkspace(const wxString& path)
    {
        wxLogNull noLog;
        if(m_workspaceOpen)
            CloseWorkspace();

        wxString errMsg;
        bool res = gsgs_CppWorker().OpenWorkspace(path, errMsg);
        if(!res) 
        {
            // in case part of the workspace was opened, close the workspace
            CloseWorkspace();
            wxMessageBox(errMsg, _("ｹ籃､ｹﾊﾊﾂ"), wxOK | wxICON_HAND);
            return;
        }

        // Let the plugins a chance of handling this workspace first
        Event e(wxEVT_CMD_OPEN_WORKSPACE, gsgs_Frame()->GetId());
        e.SetEventObject(gsgs_Frame());
        e.SetFileName(path);
        if (gsgs_Event().ProcessEvent(e))
        {
            // the plugin processed it by itself
            return;
        }

        // Make sure that the 'Workspace' tab is visible
        wxCommandEvent showTabEvent(wxEVT_COMMAND_MENU_SELECTED, XRCID("show_workspace_tab"));
        showTabEvent.SetEventObject(gsgs_Frame());
        showTabEvent.SetInt(1);
        gsgs_Frame()->GetEventHandler()->ProcessEvent(showTabEvent);

        // OpenWorkspace returned true, but errMsg is not empty
        // this could only mean that we removed a fauly project
        if(errMsg.IsEmpty() == false) { gsgs_Frame()->GetMessageBar()->DisplayMessage(errMsg, wxICON_ERROR); }

        if(gsgs_CppWorker().GetActiveProjectName().IsEmpty()) {
            // This might happen if a removed faulty project was active
            wxArrayString list;
            gsgs_CppWorker().GetProjectList(list);
            if(!list.IsEmpty()) { gsgs_CppWorker().SetActiveProject(list.Item(0)); }
        }

        DoSetupWorkspace(path);
        m_workspaceOpen = true;
    }
    //----------------------------------------------------------------------------
    void Manager::ReloadWorkspace()
    {
        if(!gsgs_Workspace().IsOpen()) 
            return;

        // Save the current session before re-loading
        gsgs_Event().NotifyWorkspaceReloadStartEvet(gsgs_CppWorker().GetFileName().GetFullPath());

        DbgStop();
        gsgs_CppWorker().ReloadWorkspace();
        DoSetupWorkspace(gsgs_CppWorker().GetFileName().GetFullPath());

        gsgs_Event().NotifyWorkspaceReloadEndEvent(gsgs_CppWorker().GetFileName().GetFullPath());
    }
    //----------------------------------------------------------------------------
    void Manager::DoSetupWorkspace(const wxString& path)
    {
        wxString errMsg;
        wxBusyCursor cursor;
        gsgs_Workspace().AddRecently(path);

        // set the C++ workspace as the active one
        gsgs_Workspace().SetWorkspace(&gsgs_CppWorker());

        wxCommandEvent evtWorkspaceLoaded(wxEVT_WORKSPACE_LOADED);
        evtWorkspaceLoaded.SetString(path);
        gsgs_Event().ProcessEvent(evtWorkspaceLoaded);

        // Update the refactoring cache
        wxFileList_t allfiles;
        GetWorkspaceFiles(allfiles, true);

        {
            SessionEntry session;
            if(gsgs_Session().GetSession(path, session)) {
                gsgs_Session().SetLastSession(path);
                gsgs_Frame()->GetWorkspaceTab()->FreezeThaw(true); // Undo any workspace/editor link while loading
                gsgs_Frame()->GetMainBook()->RestoreSession(session);
                gsgs_Frame()->GetWorkspaceTab()->FreezeThaw(false);
                GetBreakpointsMgr()->LoadSession(session);
                gsgs_Frame()->GetDebuggerPane()->GetBreakpointView()->Initialize();
            }
        }

        // clear old paths
        ParseThreadST::Get()->ClearPaths();

        // Update the parser search paths
        UpdateParserPaths(false);
        gsgs_Frame()->SelectBestEnvSet();

        // send an event to the main frame indicating that a re-tag is required
        // we do this only if the "smart retagging" is on
        TagsOptionsData tagsopt = TagsManagerST::Get()->GetCtagsOptions();
        if(tagsopt.GetFlags() & CC_RETAG_WORKSPACE_ON_STARTUP) 
        {
            wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
            gsgs_Frame()->GetEventHandler()->AddPendingEvent(e);
        }

        // Set the encoding for the tags manager
        TagsManagerST::Get()->SetEncoding(gsgs_EditorConfig().GetOptions()->GetFileFontEncoding());

        // Load the tags file content (we load the file and then destroy it) this way the file is forced into
        // the file system cache and will prevent hangs when first using the tagging system
        if(TagsManagerST::Get()->GetDatabase()) {
            wxFileName dbfn = TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName();
            JobQueueSingleton::Instance()->PushJob(new DbContentCacher(this, dbfn.GetFullPath().c_str()));
        }

        // Ensure that the "C++" view is selected
        GetManager()->GetWorkspaceView()->SelectPage(gsgs_CppWorker().GetWorkspaceType());
    }
    //----------------------------------------------------------------------------
    void Manager::CloseWorkspace()
    {
        m_workspceClosing = true;
        if(!IsShutdownInProgress()) 
        { 
            SendCmdEvent(wxEVT_WORKSPACE_CLOSING); 
        }

        DbgClearWatches();

        // If we got a running debugging session - terminate it
        IDebugger* debugger = DebuggerMgr::Get().GetActiveDebugger();
        if(debugger && debugger->IsRunning()) DbgStop();

        // save the current session before closing
        SessionEntry session;
        session.SetWorkspaceName(gsgs_CppWorker().GetFileName().GetFullPath());
        gsgs_Frame()->GetMainBook()->SaveSession(session);

        // Store the session
        GetBreakpointsMgr()->SaveSession(session);
        gsgs_Session().Save(gsgs_CppWorker().GetFileName().GetFullPath(), session);

        // Delete any breakpoints belong to the current workspace
        GetBreakpointsMgr()->DelAllBreakpoints();

        // since we closed the workspace, we also need to set the 'LastActiveWorkspaceName' to be
        // default
        gsgs_Session().SetLastSession(wxT("Default"));

        gsgs_CppWorker().CloseWorkspace();

    #ifdef __WXMSW__
        // Under Windows, and in order to avoid locking the directory set the working directory back to the start up
        // directory
        wxSetWorkingDirectory(gsgs_Path().GetStartupDirectory());
    #endif

        EvnVarList vars;
        gsgs_EditorConfig().ReadObject(wxT("Variables"), &vars);

        if(vars.IsSetExist(wxT("Default"))) 
        { 
            vars.SetActiveSet(wxT("Default")); 
        }
        gsgs_EditorConfig().WriteObject(wxT("Variables"), &vars);
        gsgs_Frame()->SelectBestEnvSet();

        UpdateParserPaths(false);
        if(!IsShutdownInProgress()) { SendCmdEvent(wxEVT_WORKSPACE_CLOSED); }
        m_workspaceOpen = false;
        m_workspceClosing = false;
    }
    //----------------------------------------------------------------------------
    void Manager::CreateProject(ProjectData & data, const wxString& workspaceFolder)
    {
        if(gsgs_Workspace().IsOpen() == false)
        {
            // create a workspace before creating a project
            CreateWorkspace(data.m_name, data.m_path);
        }

        wxString errMsg;
        bool res = gsgs_CppWorker().CreateProject(data.m_name, data.m_path,
            data.m_srcProject->GetSettings()->GetProjectType(wxEmptyString), workspaceFolder, false, errMsg);
        
        if(!res) {
            wxMessageBox(errMsg, _("ｹ籃､ｹﾊﾊﾂ"), wxOK | wxICON_HAND);
            return;
        }
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(data.m_name, errMsg);

        // copy the project settings to the new one
        proj->SetSettings(data.m_srcProject->GetSettings());

        proj->SetProjectInternalType(data.m_srcProject->GetProjectInternalType());

        // now add the new project to the build matrix
        gsgs_CppWorker().AddProjectToBuildMatrix(proj);
        ProjectSettingsPtr settings = proj->GetSettings();

        // set the compiler type
        ProjectSettingsCookie cookie;
        BuildConfigPtr bldConf = settings->GetFirstBuildConfiguration(cookie);
        BuilderPtr builder = gsgs_Build().GetBuilder(data.m_builderName);
        wxString outputfile;
        if(builder) { outputfile = builder->GetOutputFile(); }
        while(bldConf) {
    #ifndef __WXMSW__
            // The -mwindows linker flag is at best useless in !MSW, and breaks linking in the latest g++ (fedora17)
            wxString linkoptions = bldConf->GetLinkOptions();
            if(linkoptions.Contains(wxT("-mwindows;"))) {
                linkoptions.Replace(wxT("-mwindows;"), wxT(""));
            }
            // And again, without the ';' to catch an end-of-line one
            else if(linkoptions.Contains(wxT(";-mwindows"))) {
                linkoptions.Replace(wxT(";-mwindows"), wxT(""));
            }

            bldConf->SetLinkOptions(linkoptions);
    #endif
            // Update the compiler according to the user selection
            bldConf->SetCompilerType(data.m_cmpType);

            // Update the debugger according to the user selection
            bldConf->SetDebuggerType(data.m_debuggerType);

            // Update the build system
            bldConf->SetBuildSystem(data.m_builderName);

            // Set the output file name
            if(!outputfile.IsEmpty()) {
                bldConf->SetOutputFileName(wxEmptyString);
                bldConf->SetCommand(outputfile);
            }

            // Make sure that the build configuration has a project type associated with it
            if(bldConf->GetProjectType().IsEmpty()) { bldConf->SetProjectType(settings->GetProjectType(wxEmptyString)); }
            bldConf = settings->GetNextBuildConfiguration(cookie);
        }
        proj->SetSettings(settings);

        // copy the files as they appear in the source project
        proj->SetFiles(data.m_srcProject);

        // copy plugins data
        std::map<wxString, wxString> pluginsData;
        data.m_srcProject->GetAllPluginsData(pluginsData);
        proj->SetAllPluginsData(pluginsData);

        {
            // copy the actual files from the template directory to the new project path
            DirSaver ds;
            wxSetWorkingDirectory(proj->GetFileName().GetPath());

            // get list of files
            std::vector<wxFileName> files;
            data.m_srcProject->GetFilesAsVectorOfFileName(files);
            for(size_t i = 0; i < files.size(); i++) {
                wxFileName f(files.at(i));
                wxCopyFile(f.GetFullPath(), f.GetFullName());
            }
        }

        wxString projectName = proj->GetName();
        RetagProject(projectName, true);

        // Update the parser search paths
        CallAfter(&Manager::UpdateParserPaths, false);

        // Notify that a project was added
        Event evtProjectAdded(wxEVT_PROJ_ADDED);
        evtProjectAdded.SetString(projectName);
        evtProjectAdded.SetEventObject(this);
        gsgs_Event().AddPendingEvent(evtProjectAdded);
    }
    //----------------------------------------------------------------------------
    void Manager::AddProject(const wxString& path)
    {
        // create a workspace if there is non
        if(gsgs_Workspace().IsOpen() == false) {

            wxFileName fn(path);

            // create a workspace before creating a project
            CreateWorkspace(fn.GetName(), fn.GetPath());
        }

        wxString errMsg;
        bool res = gsgs_CppWorker().AddProject(path, errMsg);
        if(!res) {
            wxMessageBox(errMsg, _("ｹ籃､ｹﾊﾊﾂ"), wxOK | wxICON_HAND);
            return;
        }

        wxFileName fn(path);
        wxString projectName(fn.GetName());
        RetagProject(projectName, true);

        Event evtProjectAdded(wxEVT_PROJ_ADDED);
        evtProjectAdded.SetString(projectName);
        evtProjectAdded.SetEventObject(this);
        gsgs_Event().AddPendingEvent(evtProjectAdded);
    }
    //----------------------------------------------------------------------------
    void Manager::ReconcileProject(const wxString& projectName)
    {
        wxCHECK_RET(gsgs_Workspace().IsOpen(), wxT("Trying to reconcile a project with no open workspace"));

        wxString projname = projectName;
        if(projname.empty()) {
            projname = gsgs_CppWorker().GetActiveProjectName();
            wxCHECK_RET(!projname.empty(), wxT("Failed to find the active project"));
        }

        ReconcileProjectDlg dlg(gsgs_Frame(),
                                projname.c_str()); // In theory the deep copy is unnecessary, but I got segs in the dtor...
        if(dlg.LoadData()) { dlg.ShowModal(); }
    }
    //----------------------------------------------------------------------------
    void Manager::ImportMSVSSolution(const wxString& path, const wxString& defaultCompiler)
    {
        wxFileName fn(path);
        if(fn.FileExists() == false) { return; }

        // Show some messages to the user
        wxBusyCursor busyCursor;
        wxBusyInfo info(_("Importing IDE solution/workspace..."), gsgs_Frame());

        wxString errMsg;
        WSImporter importer;
        importer.Load(path, defaultCompiler);
        if(importer.Import(errMsg)) {
            wxString wspfile(fn.GetPath() + wxT("/") + fn.GetName() + wxT(".gsgsws"));
            //wspfile << fn.GetPath() << wxT("/") << fn.GetName() << wxT(".gsgsws");
            OpenWorkspace(wspfile);

            // Retag workspace
            wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, XRCID("retag_workspace"));
            gsgs_Frame()->GetEventHandler()->AddPendingEvent(event);
        } else {
            wxMessageBox(wxT("Solution/workspace unsupported"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxSTAY_ON_TOP);
        }
    }
    //----------------------------------------------------------------------------
    bool Manager::RemoveProject(const wxString& name, bool notify)
    {
        if(name.IsEmpty()) { return false; }

        ProjectPtr proj = GetProject(name);

        wxString errMsg;
        bool res = gsgs_CppWorker().RemoveProject(name, errMsg, proj->GetWorkspaceFolder());
        if(!res) {
            wxMessageBox(errMsg, _("ｹ籃､ｹﾊﾊﾂ"), wxOK | wxICON_HAND);
            return false;
        }

        if(proj) {
            // remove symbols from the database
            std::vector<wxFileName> projectFiles;
            proj->GetFilesAsVectorOfFileName(projectFiles);
            TagsManagerST::Get()->DeleteFilesTags(projectFiles);
            wxArrayString prjfls;
            for(size_t i = 0; i < projectFiles.size(); i++) {
                prjfls.Add(projectFiles[i].GetFullPath());
            }

            if(notify) {
                Event evtFileRemoved(wxEVT_PROJ_FILE_REMOVED);
                evtFileRemoved.SetStrings(prjfls);
                evtFileRemoved.SetString(proj->GetName());
                evtFileRemoved.SetEventObject(this);
                gsgs_Event().ProcessEvent(evtFileRemoved);
            }
        }

        if(notify) {
            Event evnt(wxEVT_PROJ_REMOVED);
            evnt.SetString(name);
            evnt.SetEventObject(this);
            gsgs_Event().AddPendingEvent(evnt);
        }
        return true;
    }
    //----------------------------------------------------------------------------
    void Manager::GetProjectList(wxArrayString & list) 
    { 
        gsgs_CppWorker().GetProjectList(list); 
    }
    //----------------------------------------------------------------------------
    ProjectPtr Manager::GetProject(const wxString& name) const
    {
        wxString projectName(name);
        projectName.Trim().Trim(false);

        if(projectName.IsEmpty()) return NULL;

        wxString errMsg;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(name, errMsg);
        if(!proj) {
            gsgsLogMessage(errMsg);
            return NULL;
        }
        return proj;
    }
    //----------------------------------------------------------------------------
    wxString Manager::GetActiveProjectName()
    {
        return gsgs_CppWorker().GetActiveProjectName();
    }
    //----------------------------------------------------------------------------
    void Manager::SetActiveProject(const wxString& name)
    {
        gsgs_CppWorker().SetActiveProject(name);
        gsgs_Frame()->SelectBestEnvSet();

        // Notify about the change
        ProjectPtr activeProject = gsgs_CppWorker().GetActiveProject();
        if(activeProject) {
            ProjectSettingsEvent evt(wxEVT_ACTIVE_PROJECT_CHANGED);
            evt.SetProjectName(name);
            evt.SetFileName(activeProject->GetFileName().GetFullPath());
            gsgs_Event().AddPendingEvent(evt);
        }
    }
    //----------------------------------------------------------------------------
    BuildMatrixPtr Manager::GetWorkspaceBuildMatrix() const 
    { 
        return gsgs_CppWorker().GetBuildMatrix(); 
    }
    //----------------------------------------------------------------------------
    void Manager::SetWorkspaceBuildMatrix(BuildMatrixPtr matrix)
    {
        gsgs_CppWorker().SetBuildMatrix(matrix);

        // Notify about the configuration change to the plugins
        wxCommandEvent e(wxEVT_WORKSPACE_CONFIG_CHANGED);
        e.SetString(matrix->GetSelectedConfigurationName());
        gsgs_Event().ProcessEvent(e);
    }
    //----------------------------------------------------------------------------
    void Manager::GetWorkspaceFiles(wxArrayString& files)
    {
        // Send an event to the plugins to get a list of the workspace files.
        // If no plugin has replied, use the default GetWorkspaceFiles method
        wxCommandEvent e(wxEVT_CMD_GET_WORKSPACE_FILES);
        e.SetEventObject(this);
        e.SetClientData(&files);
        if(gsgs_Event().ProcessEvent(e)) { return; }

        if(!gsgs_Workspace().IsOpen()) { return; }

        wxArrayString projects;
        GetProjectList(projects);

        for(size_t i = 0; i < projects.GetCount(); i++) {
            GetProjectFiles(projects.Item(i), files);
        }
    }
    //----------------------------------------------------------------------------
    void Manager::GetWorkspaceFiles(std::vector<wxFileName>& files, bool absPath)
    {
        wxArrayString projects;
        GetProjectList(projects);
        for(size_t i = 0; i < projects.GetCount(); i++) {
            ProjectPtr p = GetProject(projects.Item(i));
            p->GetFilesAsVectorOfFileName(files, absPath);
        }
    }
    //----------------------------------------------------------------------------
    bool Manager::IsFileInWorkspace(const wxString& fileName)
    {
        std::set<wxString> files;
        GetWorkspaceFiles(files);
        return files.find(fileName) != files.end();
    }
    //----------------------------------------------------------------------------
    void Manager::GetWorkspaceFiles(std::set<wxString>& files)
    {
        wxArrayString filesArr;
        wxArrayString projects;
        GetProjectList(projects);

        for(size_t i = 0; i < projects.GetCount(); i++) {
            GetProjectFiles(projects.Item(i), filesArr);
        }

        for(size_t i = 0; i < filesArr.GetCount(); i++) {
            files.insert(filesArr.Item(i));
        }
    }
    //----------------------------------------------------------------------------
    wxFileName Manager::FindFile(const wxString& filename, const wxString& project)
    {
        wxString tmpfile(filename);
        tmpfile.Trim().Trim(false);

        if(tmpfile.IsEmpty()) { return wxFileName(); }

        wxFileName fn(filename);
        if(!fn.FileExists()) {
            // try to open the file as is
            fn.Clear();
        }
        if(!fn.IsOk() && !project.IsEmpty()) {
            // try to open the file in context of its project
            wxArrayString project_files;
            GetProjectFiles(project, project_files);
            fn = FindFile(project_files, filename);
        }
        if(!fn.IsOk()) {
            // no luck there.  try the whole workspace
            wxArrayString workspace_files;
            GetWorkspaceFiles(workspace_files);
            fn = FindFile(workspace_files, filename);
        }
        if(!fn.IsAbsolute()) { fn.MakeAbsolute(); }
        return fn;
    }
    //----------------------------------------------------------------------------
    // ATTN: Please do not change this code!
    wxFileName Manager::FindFile(const wxArrayString& files, const wxFileName& fn)
    {
        // Iterate over the files twice:
        // first, try to full path
        // if the first iteration failes, iterate the files again
        // and compare full name only
        if(fn.IsAbsolute() && !fn.GetFullPath().Contains(wxT(".."))) { return fn; }

        std::vector<wxFileName> matches;
        // Try to find a match in the workspace (by comparing full paths)
        for(size_t i = 0; i < files.GetCount(); i++) {
            wxFileName tmpFileName(files.Item(i));
            if(tmpFileName.GetFullPath().CmpNoCase(fn.GetFullPath()) == 0) {
                wxFileName tt(tmpFileName);
                if(tt.MakeAbsolute()) {
                    return tt;
                } else {
                    return tmpFileName;
                }
            }
            if(tmpFileName.GetFullName() == fn.GetFullName()) { matches.push_back(tmpFileName); }
        }

        wxString lastDir;
        wxArrayString dirs = fn.GetDirs();
        if(dirs.GetCount() > 0) { lastDir = dirs.Last(); }

        if(matches.size() == 1) {
            wxFileName tt(matches.at(0));
            if(tt.MakeAbsolute()) {
                return tt;
            } else {
                return matches.at(0);
            }

        } else if(matches.size() > 1) {
            // take the best match
            std::vector<wxFileName> betterMatches;
            for(size_t i = 0; i < matches.size(); i++) {

                wxFileName filename(matches.at(i));
                wxArrayString tmpdirs = filename.GetDirs();
                if(tmpdirs.GetCount() > 0) {
                    if(tmpdirs.Last() == lastDir) { betterMatches.push_back(filename); }
                }
            }

            if(betterMatches.size() == 1) {
                wxFileName tt(betterMatches.at(0));
                if(tt.MakeAbsolute()) {
                    return tt;
                } else {
                    return betterMatches.at(0);
                }
            } else {
                // open the first match
                wxFileName tt(matches.at(0));
                if(tt.MakeAbsolute()) {
                    return tt;
                } else {
                    return matches.at(0);
                }
            }
        } else {
            // try to convert it to absolute path
            wxFileName f1(fn);
            if(f1.MakeAbsolute() /*&& f1.FileExists()*/ && !f1.GetFullPath().Contains(wxT(".."))) { return f1; }
        }
        return wxFileName();
    }
    //----------------------------------------------------------------------------
    void Manager::RetagWorkspace(TagsManager::RetagType type)
    {
        SetRetagInProgress(true);

        // in the case of re-tagging the entire workspace and full re-tagging is enabled
        // it is faster to drop the tables instead of deleting
        if(type == TagsManager::Retag_Full) { TagsManagerST::Get()->GetDatabase()->RecreateDatabase(); }

        gsgsDEBUG() << "Fetching project list..." << clEndl;
        // Start the parsing by collecing list of files to parse
        wxArrayString projects;
        GetProjectList(projects);

        gsgsDEBUG() << "Fetching project list...done" << clEndl;
        std::vector<wxFileName> projectFiles;
        for(size_t i = 0; i < projects.GetCount(); i++) {
            ProjectPtr proj = GetProject(projects.Item(i));
            if(proj) {
                gsgsDEBUG1() << "Fetching files for project:" << proj->GetName() << clEndl;
                proj->GetFilesAsVectorOfFileName(projectFiles);
                gsgsDEBUG1() << "Fetching files for project:" << proj->GetName() << "...done" << clEndl;
            }
        }

        // Create a parsing request
        ParseRequest* parsingRequest = new ParseRequest(gsgs_Frame());
        gsgsDEBUG() << "Filtering non relevant files..." << clEndl;
        parsingRequest->_workspaceFiles.reserve(projectFiles.size());

        for(size_t i = 0; i < projectFiles.size(); i++) {
            // filter any non valid coding file
            const wxFileName& fn = projectFiles[i];
            parsingRequest->_workspaceFiles.push_back(fn.GetFullPath().ToAscii().data());
        }

        gsgsDEBUG() << "Filtering non relevant files...done" << clEndl;

        if(parsingRequest->_workspaceFiles.empty()) {
            SetRetagInProgress(false);
            delete parsingRequest;
            return;
        }

        if(type == TagsManager::Retag_Full || type == TagsManager::Retag_Quick) {
            parsingRequest->setType(ParseRequest::PR_PARSEINCLUDES);
            parsingRequest->setDbFile(TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName().GetFullPath().c_str());
            parsingRequest->_evtHandler = this;
            parsingRequest->_quickRetag = (type == TagsManager::Retag_Quick);
            ParseThreadST::Get()->Add(parsingRequest);
            gsgs_Frame()->GetMainStatusBar()->SetMessage("Scanning for include files to parse...");

        } else if(type == TagsManager::Retag_Quick_No_Scan) {
            parsingRequest->setType(ParseRequest::PR_PARSE_FILE_NO_INCLUDES);
            parsingRequest->setDbFile(TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName().GetFullPath().c_str());
            parsingRequest->_quickRetag = true;
            ParseThreadST::Get()->Add(parsingRequest);
        }
    }
    //----------------------------------------------------------------------------
    void Manager::RetagFile(const wxString& filename)
    {
        if(IsWorkspaceClosing()) { return; }

        // Is this a C++ file?
        if(!FileUtil::IsCxxFile(wxFileName(filename))) { return; }

        wxFileName absFile(filename);
        absFile.MakeAbsolute();

        // Put a request to the parsing thread
        ParseRequest* req = new ParseRequest(gsgs_Frame());
        req->setDbFile(TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName().GetFullPath().c_str());
        req->setFile(absFile.GetFullPath().c_str());
        req->setType(ParseRequest::PR_FILESAVED);
        ParseThreadST::Get()->Add(req);
        gsgs_Frame()->GetMainStatusBar()->SetMessage((wxString() << _("Parsing file:") << absFile.GetFullName()));
    }
    //----------------------------------------------------------------------------
    int Manager::AddVirtualDirectory(const wxString& virtualDirFullPath, bool createIt)
    {
        if(gsgs_CppWorker().IsVirtualDirectoryExists(virtualDirFullPath)) { return VD_EXISTS; }

        wxString errMsg;
        bool res = gsgs_CppWorker().CreateVirtualDirectory(virtualDirFullPath, errMsg, createIt);
        if(!res) {
            wxMessageBox(errMsg, _("ｹ籃､ｹﾊﾊﾂ"), wxOK | wxICON_HAND);
            return VD_ERROR;
        }
        return VD_OK;
    }
    //----------------------------------------------------------------------------
    void Manager::RemoveVirtualDirectory(const wxString& virtualDirFullPath)
    {
        wxString errMsg;
        wxString project = virtualDirFullPath.BeforeFirst(wxT(':'));
        ProjectPtr p = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if(!p) { return; }

        // Update symbol tree and database
        wxString vdPath = virtualDirFullPath.AfterFirst(wxT(':'));
        wxArrayString files;
        p->GetFilesByVirtualDir(vdPath, files);
        wxFileName tagsDb = TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName();
        for(size_t i = 0; i < files.Count(); i++) {
            TagsManagerST::Get()->Delete(tagsDb, files.Item(i));
        }

        // and finally, remove the virtual dir from the workspace
        bool res = gsgs_CppWorker().RemoveVirtualDirectory(virtualDirFullPath, errMsg);
        if(!res) {
            wxMessageBox(errMsg, _("ｹ籃､ｹﾊﾊﾂ"), wxOK | wxICON_HAND);
            return;
        }

        Event evtFileRemoved(wxEVT_PROJ_FILE_REMOVED);
        evtFileRemoved.SetStrings(files);
        evtFileRemoved.SetString(project);
        evtFileRemoved.SetEventObject(this);
        gsgs_Event().ProcessEvent(evtFileRemoved);
    }
    //----------------------------------------------------------------------------
    bool Manager::AddNewFileToProject(const wxString& fileName, const wxString& vdFullPath, bool openIt)
    {
        wxFile file;
        wxFileName fn(fileName);
        if(!fn.DirExists()) {
            // ensure that the path to the file exists
            fn.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }

        if(!file.Create(fileName.GetData(), true)) return false;

        if(file.IsOpened()) { file.Close(); }

        return AddFileToProject(fileName, vdFullPath, openIt);
    }
    //----------------------------------------------------------------------------
    bool Manager::AddFileToProject(const wxString& fileName, const wxString& vdFullPath, bool openIt)
    {
        wxString project;
        project = vdFullPath.BeforeFirst(wxT(':'));

        // Add the file to the project
        wxString errMsg;
        bool res = gsgs_CppWorker().AddNewFile(vdFullPath, fileName, errMsg);
        if(!res) {
            // file or virtual dir does not exist
            return false;
        }

        if(openIt) { gsgs_Frame()->GetMainBook()->OpenFile(fileName, project); }

        TagTreePtr ttp;
        if(project.IsEmpty() == false) {
            std::vector<CommentPtr> comments;
            if(TagsManagerST::Get()->GetParseComments()) {
                ttp = TagsManagerST::Get()->ParseSourceFile(fileName, &comments);
            } else {
                ttp = TagsManagerST::Get()->ParseSourceFile(fileName);
            }
            TagsManagerST::Get()->Store(ttp);
        }

        // send notification command event that files was added to
        // project
        wxArrayString files;
        files.Add(fileName);

        Event evtAddFiles(wxEVT_PROJ_FILE_ADDED);
        evtAddFiles.SetStrings(files);
        evtAddFiles.SetString(project);
        gsgs_Event().AddPendingEvent(evtAddFiles);
        return true;
    }
    //----------------------------------------------------------------------------
    void Manager::AddFilesToProject(const wxArrayString& files, const wxString& vdFullPath, wxArrayString& actualAdded)
    {
        wxString project;
        project = vdFullPath.BeforeFirst(wxT(':'));

        // Add the file to the project
        wxString errMsg;
        // bool res = true;
        size_t i = 0;

        // try to find this file in the workspace
        for(i = 0; i < files.GetCount(); i++) {
            wxString file = files.Item(i);
            wxString projName = this->GetProjectNameByFile(file);
            // allow adding the file, only if it does not already exist under the current project
            //(it can be already exist under the different project)
            if(projName.IsEmpty() || projName != project) {
                actualAdded.Add(file);
            }
    #if defined(__WXGTK__)
            else {
                // In Linux, files 'abc' and 'Abc' can happily co-exist, so see if that's what's happening
                wxString projName = this->GetProjectNameByFile(file, true); // 'true' is case-sensitive comparison
                if(projName.IsEmpty() || projName != project) {
                    wxString msg1(wxString::Format(_("There is already a file in this folder with a name:\n%s\nthat "
                                                     "matches using case-insensitive comparison"),
                                                   file));
                    wxString msg2(
                        _("\nThis won't be a problem on Linux, but it may be on other, case-insensitive platforms"));
                    wxString msg3(_("\n\nAdd the file anyway?"));
                    int ans = wxMessageBox(msg1 + msg2 + msg3, _("Possible name-clash"),
                                           wxICON_QUESTION | wxYES_NO | wxCANCEL, gsgs_Frame());
                    if(ans == wxYES) {
                        actualAdded.Add(file);
                    } else if(ans == wxCANCEL) {
                        return;
                    }
                }
            }
    #endif
        }

        for(i = 0; i < actualAdded.GetCount(); i++) {
            gsgs_CppWorker().AddNewFile(vdFullPath, actualAdded.Item(i), errMsg);
        }

        // convert wxArrayString to vector for the ctags api
        std::vector<wxFileName> vFiles;
        for(size_t i = 0; i < actualAdded.GetCount(); i++) {
            vFiles.push_back(actualAdded.Item(i));
        }

        // re-tag the added files
        if(vFiles.empty() == false) { TagsManagerST::Get()->RetagFiles(vFiles, TagsManager::Retag_Quick); }

        if(!actualAdded.IsEmpty()) {
            Event evtAddFiles(wxEVT_PROJ_FILE_ADDED);
            evtAddFiles.SetStrings(actualAdded);
            evtAddFiles.SetString(project);
            gsgs_Event().AddPendingEvent(evtAddFiles);
        }

        if(actualAdded.GetCount() < files.GetCount()) {
            wxString msg = wxString::Format(_("%u file(s) not added, probably due to a name-clash"),
                                            (unsigned int)(files.GetCount() - actualAdded.GetCount()));
            wxMessageBox(msg, _("ｹ籃､ｹﾊﾊﾂ"), wxOK, gsgs_Frame());
        }
    }
    //----------------------------------------------------------------------------
    bool Manager::RemoveFile(const wxString& fileName, const wxString& vdFullPath, wxString& fullpathRemoved, bool notify)
    {
        fullpathRemoved.Clear();
        wxString project = vdFullPath.BeforeFirst(wxT(':'));
        wxFileName absPath(fileName);
        absPath.MakeAbsolute(GetProjectCwd(project));

        gsgs_Frame()->GetMainBook()->ClosePage(absPath.GetFullPath());

        wxString errMsg;
        bool res = gsgs_CppWorker().RemoveFile(vdFullPath, fileName, errMsg);
        if(!res) {
            wxMessageBox(errMsg, _("ｹ籃､ｹﾊﾊﾂ"), wxOK | wxICON_HAND, gsgs_Frame());
            return false;
        }

        TagsManagerST::Get()->Delete(TagsManagerST::Get()->GetDatabase()->GetDatabaseFileName(), absPath.GetFullPath());

        // Set the fullpath of the removed file
        fullpathRemoved = absPath.GetFullPath();

        if(notify) {
            wxArrayString files(1, &fileName);
            Event evtFileRemoved(wxEVT_PROJ_FILE_REMOVED);
            evtFileRemoved.SetStrings(files);
            evtFileRemoved.SetString(project);
            evtFileRemoved.SetEventObject(this);
            gsgs_Event().ProcessEvent(evtFileRemoved);
        }
        return true;
    }
    //----------------------------------------------------------------------------
    bool Manager::RenameFile(const wxString& origName, const wxString& newName, const wxString& vdFullPath)
    {
        // Step: 1
        // remove the file from the workspace (this will erase it from the symbol database and will
        // also close the editor that it is currently opened in (if any)
        wxString pathRemoved;
        if(!RemoveFile(origName, vdFullPath, pathRemoved)) return false;

        // Step: 2
        // Notify the plugins, maybe they want to override the
        // default behavior (e.g. Subversion plugin)
        FileEvent renameEvent(wxEVT_FILE_RENAMED);
        renameEvent.SetPath(origName);
        renameEvent.SetNewpath(newName);
        if(!gsgs_Event().ProcessEvent(renameEvent)) {
            // rename the file on filesystem
            wxLogNull noLog;
            wxRenameFile(origName, newName);
        }

        // read file to project with the new name
        wxString projName = vdFullPath.BeforeFirst(wxT(':'));
        ProjectPtr proj = GetProject(projName);
        proj->FastAddFile(newName, vdFullPath.AfterFirst(wxT(':')));

        // Step 3: retag the new file
        RetagFile(newName);

        // Step 4: send an event about new file was added
        // to the workspace
        wxArrayString files;
        files.Add(newName);

        Event evtAddFiles(wxEVT_PROJ_FILE_ADDED);
        evtAddFiles.SetStrings(files);
        evtAddFiles.SetString(projName);
        gsgs_Event().AddPendingEvent(evtAddFiles);

        // Open the newly created file
        gsgs_Frame()->GetMainBook()->OpenFile(newName, projName);

        // Step 5: Change all include files refering to the old
        // file
        if(!gsgs_Workspace().IsOpen()) {
            // if there is no workspace opened, we are done
            return true;
        }

        wxArrayString workspaceFiles;
        GetWorkspaceFiles(workspaceFiles);
        std::vector<IncludeStatement> includes, matches;

        for(size_t i = 0; i < workspaceFiles.GetCount(); i++) {

            // Dont attempt to scan binary files
            // Skip binary files
            if(TagsManagerST::Get()->IsBinaryFile(workspaceFiles.Item(i))) { continue; }

            // Scan only C/C++/h files
            switch(FileUtil::GetType(workspaceFiles.Item(i))) {
            case FileUtil::TypeHeader:
            case FileUtil::TypeSourceC:
            case FileUtil::TypeSourceCpp:
                IncludeFinder(workspaceFiles.Item(i).mb_str(wxConvUTF8).data(), includes);
                break;
            default:
                // ignore any other type
                break;
            }
        }

        // Filter non-relevant matches
        wxFileName oldName(origName);
        for(size_t i = 0; i < includes.size(); i++) {

            wxString inclName(includes.at(i).file.c_str(), wxConvUTF8);
            wxFileName inclFn(inclName);

            if(oldName.GetFullName() == inclFn.GetFullName()) { matches.push_back(includes.at(i)); }
        }

        // Prompt the user with the list of files which are about to be modified
        wxFileName newFile(newName);
        if(matches.empty() == false) {
            RenameFileDlg dlg(gsgs_Frame(), newFile.GetFullName(), matches);
            if(dlg.ShowModal() == wxID_OK) {
                matches.clear();
                matches = dlg.GetMatches();

                for(size_t i = 0; i < matches.size(); i++) {
                    IncludeStatement includeStatement = matches.at(i);
                    wxString editorFileName(includeStatement.includedFrom.c_str(), wxConvUTF8);
                    wxString findWhat(includeStatement.pattern.c_str(), wxConvUTF8);
                    wxString oldIncl(includeStatement.file.c_str(), wxConvUTF8);

                    // We want to keep the original open/close braces
                    // "" or <>
                    wxFileName strippedOldInc(oldIncl);
                    wxString replaceWith(findWhat);

                    replaceWith.Replace(strippedOldInc.GetFullName(), newFile.GetFullName());

                    Editor* editor = gsgs_Frame()->GetMainBook()->OpenFile(editorFileName, wxEmptyString, 0);
                    if(editor && (editor->GetFileName().GetFullPath().CmpNoCase(editorFileName) == 0)) {
                        editor->ReplaceAllExactMatch(findWhat, replaceWith);
                    }
                }
            }
        }
        return true;
    }
    //----------------------------------------------------------------------------
    bool Manager::MoveFileToVD(const wxString& fileName, const wxString& srcVD, const wxString& targetVD)
    {
        // to move the file between targets, we need to change the file path, we do this
        // by changing the file to be in absolute path related to the src's project
        // and then making it relative to the target's project
        wxString srcProject, targetProject;
        srcProject = srcVD.BeforeFirst(wxT(':'));
        targetProject = targetVD.BeforeFirst(wxT(':'));
        wxFileName srcProjWd(GetProjectCwd(srcProject), wxEmptyString);

        // set a dir saver point
        wxFileName fn(fileName);
        wxArrayString files;
        files.Add(fn.GetFullPath());

        // remove the file from the source project
        wxString errMsg;
        bool res = gsgs_CppWorker().RemoveFile(srcVD, fileName, errMsg);
        if(!res) {
            wxMessageBox(errMsg, _("ｹ籃､ｹﾊﾊﾂ"), wxOK | wxICON_HAND);
            return false;
        }

        // Add the file to the project
        res = gsgs_CppWorker().AddNewFile(targetVD, fn.GetFullPath(), errMsg);
        if(!res) {
            // file or virtual dir does not exist
            return false;
        }
        return true;
    }
    //----------------------------------------------------------------------------
    void Manager::RetagProject(const wxString& projectName, bool quickRetag)
    {
        ProjectPtr proj = GetProject(projectName);
        if(!proj) return;

        SetRetagInProgress(true);

        std::vector<wxFileName> projectFiles;
        proj->GetFilesAsVectorOfFileName(projectFiles);
        TagsManagerST::Get()->RetagFiles(projectFiles, quickRetag ? TagsManager::Retag_Quick : TagsManager::Retag_Full);
        SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&projectFiles);
    }
    //----------------------------------------------------------------------------
    void Manager::GetProjectFiles(const wxString& project, wxArrayString& files)
    {
        ProjectPtr p = GetProject(project);
        if(p) { p->GetFilesAsStringArray(files); }
    }
    //----------------------------------------------------------------------------
    wxString Manager::GetProjectNameByFile(const wxString& fullPathFileName, bool caseSensitive /*= false*/)
    {
        wxString fPFN(fullPathFileName);
        return GetProjectNameByFile(fPFN, caseSensitive);
    }
    //----------------------------------------------------------------------------
    wxString Manager::GetProjectNameByFile(wxString& fullPathFileName, bool caseSensitive /*= false*/)
    {
        wxArrayString projects;
        GetProjectList(projects);

        // On gtk either fullPathFileName or the 'matching' project filename (or both) may be (or their paths contain)
        // symlinks
        wxString linkDestination = RealPath(fullPathFileName);
        for(size_t i = 0; i < projects.GetCount(); i++) {
            ProjectPtr proj = GetProject(projects.Item(i));
            // The second call copes with the searched-for file being a symlink
            if(proj->IsFileExist(fullPathFileName) || proj->IsFileExist(linkDestination)) { return proj->GetName(); }
    #if defined(__WXGTK__)
            wxString fileNameInProject; // Try again, checking if the _project_ filePath is a symlink
            if(proj->IsFileExist(fullPathFileName, fileNameInProject)) {
                fullPathFileName = fileNameInProject; // Hopefully the calling function will now use this
                return proj->GetName();
            }
    #endif
            // TODO:: add support for case insensitive search in project
        }

        return wxEmptyString;
    }
    //----------------------------------------------------------------------------
    wxString Manager::GetProjectCwd(const wxString& project) const
    {
        wxString errMsg;
        ProjectPtr p = gsgs_CppWorker().FindProjectByName(project, errMsg);
        if (p)
        {
            wxFileName projectfn(p->GetFileName());
            projectfn.MakeAbsolute();
            return projectfn.GetPath();
        }
        return wxGetCwd();
    }
    //----------------------------------------------------------------------------
    ProjectSettingsPtr Manager::GetProjectSettings(const wxString& projectName) const
    {
        wxString errMsg;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(projectName, errMsg);
        if(!proj) {
            gsgsLogMessage(errMsg);
            return NULL;
        }

        return proj->GetSettings();
    }
    //----------------------------------------------------------------------------
    void Manager::SetProjectSettings(const wxString& projectName, ProjectSettingsPtr settings)
    {
        wxString errMsg;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(projectName, errMsg);
        if(!proj) {
            gsgsLogMessage(errMsg);
            return;
        }

        proj->SetSettings(settings);
    }
    //----------------------------------------------------------------------------
    void Manager::SetProjectGlobalSettings(const wxString& projectName, BuildConfigCommonPtr settings)
    {
        wxString errMsg;
        ProjectPtr proj = gsgs_CppWorker().FindProjectByName(projectName, errMsg);
        if(!proj) {
            gsgsLogMessage(errMsg);
            return;
        }

        proj->SetGlobalSettings(settings);
    }
    //----------------------------------------------------------------------------
    wxString Manager::GetProjectExecutionCommand(const wxString& projectName, wxString& wd, bool considerPauseWhenExecuting)
    {
        ProjectPtr proj = GetProject(projectName);
        if(!proj) {
            gsgsWARNING() << "Manager::GetProjectExecutionCommand(): could not find project:" << projectName;
            return wxEmptyString;
        }

        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
        if(!bldConf) {
            gsgsWARNING() << "Manager::GetProjectExecutionCommand(): failed to find project configuration for project:"
                        << projectName;
            return wxEmptyString;
        }

        wxString projectPath = proj->GetFileName().GetPath();

        // expand variables
        wxString cmd = bldConf->GetCommand();
        cmd = MacroManager::Instance()->Expand(cmd, NULL, projectName);

        wxString cmdArgs = bldConf->GetCommandArguments();
        cmdArgs = MacroManager::Instance()->Expand(cmdArgs, NULL, projectName);

        // Execute command & cmdArgs
        wd = bldConf->GetWorkingDirectory();
        wd = MacroManager::Instance()->Expand(wd, NULL, projectName);

        wxFileName workingDir(wd, "");
        if(workingDir.IsRelative()) { workingDir.MakeAbsolute(projectPath); }

        wxFileName fileExe(cmd);
        if(fileExe.IsRelative()) { fileExe.MakeAbsolute(workingDir.GetPath()); }
        fileExe.Normalize();

    #ifdef __WXMSW__
        if(!fileExe.Exists() && fileExe.GetExt().IsEmpty()) {
            // Try with .exe
            wxFileName withExe(fileExe);
            withExe.SetExt("exe");
            if(withExe.Exists()) { fileExe = withExe; }
        }
    #endif
        wd = workingDir.GetPath();
        cmd = fileExe.GetFullPath();

        gsgsDEBUG() << "Command to execute:" << cmd;
        gsgsDEBUG() << "Working directory:" << wd;

        ConsoleBase::Ptr_t console = ConsoleBase::GetTerminal();
        console->SetWorkingDirectory(wd);
        console->SetCommand(cmd, cmdArgs);
        console->SetWaitWhenDone(considerPauseWhenExecuting);
        console->SetTerminalNeeded(!bldConf->IsGUIProgram());
        return console->PrepareCommand();
    }

    //--------------------------- Top Level Pane Management -----------------------------

    bool Manager::IsPaneVisible(const wxString& pane_name)
    {
        wxAuiPaneInfo& info = gsgs_Frame()->GetAuiManager()->GetPane(pane_name);
        if(info.IsOk() && info.IsShown()) 
        { 
            return true; 
        }
        return false;
    }
    //----------------------------------------------------------------------------
    bool Manager::DoFindDockInfo(const wxString& saved_perspective, const wxString& dock_name, wxString& dock_info)
    {
        // search for the 'Output View' perspective
        wxArrayString panes = wxStringTokenize(saved_perspective, wxT("|"), wxTOKEN_STRTOK);
        for(size_t i = 0; i < panes.GetCount(); i++) {
            if(panes.Item(i).StartsWith(dock_name)) {
                dock_info = panes.Item(i);
                return true;
            }
        }
        return false;
    }
    //----------------------------------------------------------------------------
    bool Manager::ShowOutputPane(wxString focusWin, bool commit)
    {
        // gsgs_Frame()->ViewPane(wxT("Output View"), true);

        // make the output pane visible
        GetPerspectiveManager().ToggleOutputPane(false, commit);

        // set the selection to focus win
        Notebook* book = gsgs_Frame()->GetOutputPane()->GetNotebook();
        int index = book->GetPageIndex(focusWin);
        if (index != wxNOT_FOUND && index != book->GetSelection())
        {
            Editor* editor = dynamic_cast<Editor*>(wxWindow::FindFocus());
            book->SetSelection((size_t)index);
            if (editor)
            {
                editor->SetFocus();
            }
        }

    #ifndef __WXMSW__
    #if wxVERSION_NUMBER >= 2900
        // This is needed in >=wxGTK-2.9, otherwise the current editor sometimes doesn't notice that the output pane has
        // appeared
        // resulting in an area at the bottom that can't be scrolled to
        gsgs_Frame()->SendSizeEvent(wxSEND_EVENT_POST);
    #endif
    #endif
        return true;
    }
    //----------------------------------------------------------------------------
    void Manager::ShowDebuggerPane(bool show)
    {
        wxArrayString dbgPanes;
        dbgPanes.Add(wxT("Debugger"));
        dbgPanes.Add(wxGetTranslation(DebuggerPane::LOCALS));
        dbgPanes.Add(wxGetTranslation(DebuggerPane::FRAMES));
        dbgPanes.Add(wxGetTranslation(DebuggerPane::WATCHES));
        dbgPanes.Add(wxGetTranslation(DebuggerPane::BREAKPOINT));
        dbgPanes.Add(wxGetTranslation(DebuggerPane::THREAD));
        dbgPanes.Add(wxGetTranslation(DebuggerPane::MEMORY));
        dbgPanes.Add(wxGetTranslation(DebuggerPane::ASCII_VIEWER));

        wxAuiManager* aui = gsgs_Frame()->GetAuiManager();
        if(show) 
        {
            for(size_t i = 0; i < dbgPanes.GetCount(); i++) 
            {
                wxAuiPaneInfo& info = gsgs_Frame()->GetAuiManager()->GetPane(dbgPanes.Item(i));
                // show all debugger related panes
                if(info.IsOk() && !info.IsShown()) 
                {
                    WindowManager::HackShowPane(info, aui);
                }
            }

        } 
        else
        {
            // hide all debugger related panes
            for(size_t i = 0; i < dbgPanes.GetCount(); i++) 
            {
                wxAuiPaneInfo& info = gsgs_Frame()->GetAuiManager()->GetPane(dbgPanes.Item(i));
                // show all debugger related panes
                if(info.IsOk() && info.IsShown()) 
                { 
                    WindowManager::HackHidePane(true, info, aui); 
                }
            }
        }
    }
    //----------------------------------------------------------------------------
    void Manager::ShowWorkspacePane(wxString focusWin, bool commit)
    {
        wxAuiPaneInfo& info = gsgs_Frame()->GetAuiManager()->GetPane(wxT("Workspace View"));
        if(info.IsOk() && !info.IsShown()) 
        {
            info.Show();
            if(commit) 
            { 
                gsgs_Frame()->GetAuiManager()->Update(); 
            }
        }

        // set the selection to focus win
        Notebook* book = gsgs_Frame()->GetWorkspacePane()->GetNotebook();
        int index = book->GetPageIndex(focusWin);
        if(index != wxNOT_FOUND && index != book->GetSelection()) 
        { 
            book->SetSelection((size_t)index); 
        }
    }
    //----------------------------------------------------------------------------
    void Manager::ShowPerspectivePane(wxString focusWin, bool commit)
    {
        wxAuiPaneInfo& info = gsgs_Frame()->GetAuiManager()->GetPane(wxT("Perspective View"));
        if (info.IsOk() && !info.IsShown())
        {
            info.Show();
            if (commit)
            {
                gsgs_Frame()->GetAuiManager()->Update();
            }
        }

        // set the selection to focus win
        Notebook* book = gsgs_Frame()->GetPerspectivePane()->GetNotebook();
        int index = book->GetPageIndex(focusWin);
        if (index != wxNOT_FOUND && index != book->GetSelection())
        {
            book->SetSelection((size_t)index);
        }
    }
    //----------------------------------------------------------------------------
    void Manager::ShowScenePane(bool commit)
    {
        wxAuiPaneInfo & info = gsgs_Frame()->GetAuiManager()->GetPane(wxT("Scene View"));
        if (info.IsOk() && !info.IsShown())
        {
            info.Show();
            if (commit)
            {
                gsgs_Frame()->GetAuiManager()->Update();
            }
        }
    }
    //----------------------------------------------------------------------------
    void Manager::HidePane(const wxString & paneName, bool commit)
    {
        wxAuiPaneInfo& info = gsgs_Frame()->GetAuiManager()->GetPane(paneName);
        if(info.IsOk() && info.IsShown()) 
        {
            wxAuiManager* aui = gsgs_Frame()->GetAuiManager();
            WindowManager::HackHidePane(commit, info, aui);
        }
    }
    //----------------------------------------------------------------------------
    bool Manager::TogglePanes()
    {
        static wxString savedLayout;
        if(savedLayout.IsEmpty()) 
        {
            savedLayout = gsgs_Frame()->GetAuiManager()->SavePerspective();
            wxAuiManager* aui = gsgs_Frame()->GetAuiManager();
            wxAuiPaneInfoArray& all_panes = aui->GetAllPanes();
            for(size_t i = 0; i < all_panes.GetCount(); ++i) 
            {
                if(all_panes.Item(i).IsOk() && all_panes.Item(i).IsShown() &&
                   all_panes.Item(i).dock_direction != wxAUI_DOCK_CENTER) 
                {
                    all_panes.Item(i).Hide();
                }
            }
            aui->Update();
            m_ShowAllPanes = false;
        } 
        else 
        {
            gsgs_Frame()->GetAuiManager()->LoadPerspective(savedLayout);
            savedLayout.Clear();
            m_ShowAllPanes = true;
        }
        return m_ShowAllPanes;
    }
    //----------------------------------------------------------------------------
    void Manager::UpdateMenuAccelerators(wxFrame* frame)
    {
        // Load the accelerators (this function merges the old settings with the new settings)
        gsgs_Window().UpdateAccels(frame);
    }
    //----------------------------------------------------------------------------
    bool Manager::IsProgramRunning() const { return m_programProcess; }
    //----------------------------------------------------------------------------
    void Manager::ExecuteNoDebug(const wxString& projectName)
    {
        // an instance is already running
        if(IsProgramRunning()) { return; }
        wxString wd;

        // we call it here once for the 'wd'
        wxString execLine;
        ProjectPtr proj;

        {
            EnvSetter env1(NULL, NULL, projectName, wxEmptyString);
            execLine = GetProjectExecutionCommand(projectName, wd, true);
            proj = GetProject(projectName);
        }

        DirSaver ds;

        // Build the working directory
        // Expand macros
        wd = MacroManager::Instance()->Expand(wd, NULL, projectName);
        wd = wd.Trim().Trim(false);
        if(!wd.IsEmpty()) {
            wxFileName projectWd(wd, "");
            if(!projectWd.IsAbsolute()) { projectWd.MakeAbsolute(proj->GetFileName().GetPath()); }
            wd = projectWd.GetPath();
        } else {
            // Set the working directory to the project path
            wd = proj->GetFileName().GetPath();
        }

        // execute the program:
        //- no hiding the console
        //- no redirection of the stdin/out
        wxString configName;
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) { configName = bldConf->GetName(); }
        EnvSetter env(NULL, NULL, projectName, configName);

        // call it again here to get the actual exection line - we do it here since
        // the environment has been applied
        size_t createProcessFlags = IProcessCreateDefault;
        if(!bldConf->IsGUIProgram()) { createProcessFlags = IProcessNoRedirect | IProcessCreateConsole; }
    #ifdef __WXMSW__
        createProcessFlags |= IProcessCreateConsole;
    #endif

        wxString dummy;
        execLine = GetProjectExecutionCommand(projectName, dummy, bldConf->GetPauseWhenExecEnds());
        wxUnusedVar(dummy);

        m_programProcess = ::CreateAsyncProcess(this, execLine, createProcessFlags, wd);
        if(m_programProcess) {
            GetManager()->AppendOutputTabText(kOutputTab_Output, wxString()
                                                                       << _("Working directory is set to: ") << wd << "\n");
            GetManager()->AppendOutputTabText(kOutputTab_Output, wxString() << _("Executing: ") << execLine << "\n");
        }
    }
    //----------------------------------------------------------------------------
    void Manager::KillProgram()
    {
        if(!IsProgramRunning()) return;
        m_programProcess->Terminate();
    }
    //----------------------------------------------------------------------------
    void Manager::OnProcessOutput(gsgs::ProcessEvent& event)
    {
        gsgs_Frame()->GetOutputPane()->GetOutputWindow()->AppendText(event.GetOutput());
    }
    //----------------------------------------------------------------------------
    void Manager::OnProcessEnd(gsgs::ProcessEvent& event)
    {
        wxDELETE(m_programProcess);
        GetManager()->AppendOutputTabText(kOutputTab_Output, _("Program exited\n"));
        // return the focus back to the editor
        if(gsgs_Frame()->GetMainBook()->GetActiveEditor()) {
            gsgs_Frame()->GetMainBook()->GetActiveEditor()->SetActive();
        }
    }

    //----------------------------------------------------------------------------

    static void DebugMessage(const wxString& msg)
    {
        gsgs_Frame()->GetDebuggerPane()->GetDebugWindow()->AppendLine(msg);
    }
    //----------------------------------------------------------------------------
    void Manager::UpdateDebuggerPane()
    {
        Event evtDbgRefreshViews(wxEVT_DEBUGGER_UPDATE_VIEWS);
        gsgs_Event().AddPendingEvent(evtDbgRefreshViews);

        DebuggerPane* pane = gsgs_Frame()->GetDebuggerPane();
        DoUpdateDebuggerTabControl(pane->GetNotebook()->GetCurrentPage());
    }
    //----------------------------------------------------------------------------
    void Manager::DoUpdateDebuggerTabControl(wxWindow* curpage)
    {
        // Update the debugger pane
        DebuggerPane* pane = gsgs_Frame()->GetDebuggerPane();

        // make sure that the debugger pane is visible
        if(!IsPaneVisible(wxT("Debugger"))) return;

        if(curpage == (wxWindow*)pane->GetBreakpointView() || IsPaneVisible(DebuggerPane::BREAKPOINT)) {
            pane->GetBreakpointView()->Initialize();
        }

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning() && DbgCanInteract()) {

            //--------------------------------------------------------------------
            // Lookup the selected tab in the debugger notebook and update it
            // once this is done, we need to go through the list of detached panes
            // and scan for another debugger tabs which are visible and need to be
            // updated
            //--------------------------------------------------------------------

            if(curpage == (wxWindow*)pane->GetLocalsTable() || IsPaneVisible(wxGetTranslation(DebuggerPane::LOCALS))) {
                // update the locals tree
                dbgr->QueryLocals();
            }

            if(curpage == (wxWindow*)pane->GetDisassemblyTab() ||
               IsPaneVisible(wxGetTranslation(DebuggerPane::DISASSEMBLY))) {
                dbgr->ListRegisters();
            }

            if(curpage == pane->GetWatchesTable() || IsPaneVisible(wxGetTranslation(DebuggerPane::WATCHES))) {
                pane->GetWatchesTable()->RefreshValues();
            }
            if(curpage == (wxWindow*)pane->GetFrameListView() || IsPaneVisible(wxGetTranslation(DebuggerPane::FRAMES))) {
                // update the stack call
                dbgr->ListFrames();
            }

            if(curpage == (wxWindow*)pane->GetBreakpointView() ||
               IsPaneVisible(wxGetTranslation(DebuggerPane::BREAKPOINT))) {

                // update the breakpoint view
                pane->GetBreakpointView()->Initialize();
            }
            if(curpage == (wxWindow*)pane->GetThreadsView() || IsPaneVisible(wxGetTranslation(DebuggerPane::THREAD))) {

                // update the thread list
                dbgr->ListThreads();
            }

            if(curpage == (wxWindow*)pane->GetMemoryView() || IsPaneVisible(wxGetTranslation(DebuggerPane::MEMORY))) {

                // Update the memory view tab
                MemoryView* memView = pane->GetMemoryView();
                if(memView->GetExpression().IsEmpty() == false) {

                    wxString output;
                    dbgr->WatchMemory(memView->GetExpression(), memView->GetSize(), memView->GetColumns());
                }
            }
        }
    }
    //----------------------------------------------------------------------------
    void Manager::SetMemory(const wxString& address, size_t count, const wxString& hex_value)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning() && DbgCanInteract()) { dbgr->SetMemory(address, count, hex_value); }
    }
    //----------------------------------------------------------------------------
    void Manager::DbgStart(long attachPid)
    {
        // set the working directory to the project directory
        DirSaver ds;
        wxString errMsg;
        wxString output;
        wxString debuggerName;
        wxString exepath;
        wxString wd;
        wxString args;
        BuildConfigPtr bldConf;
        ProjectPtr proj;
        DebuggerStartupInfo startup_info;
        long PID(-1);

        if(attachPid == -1) {
            // Start debugger ( when attachPid != -1 it means we are attaching to process )
            // Let the plugin know that we are about to start debugging
            DebugEvent dbgEvent(wxEVT_DBG_UI_START);
            ProjectPtr activeProject = gsgs_CppWorker().GetActiveProject();
            if(activeProject) {
                dbgEvent.SetProjectName(activeProject->GetName());
                BuildConfigPtr buildConfig = activeProject->GetBuildConfiguration();
                if(buildConfig) {
                    dbgEvent.SetDebuggerName(buildConfig->GetDebuggerType());
                    dbgEvent.SetConfigurationName(buildConfig->GetName());
                }
            }
            if(gsgs_Event().ProcessEvent(dbgEvent)) return;
        }

    #if defined(__WXGTK__)
        wxString where;
        wxString terminal;
        wxArrayString tokens;
        wxArrayString configuredTerminal;
        terminal = wxT("xterm");
        if(!gsgs_EditorConfig().GetOptions()->GetProgramConsoleCommand().IsEmpty()) {
            tokens =
                wxStringTokenize(gsgs_EditorConfig().GetOptions()->GetProgramConsoleCommand(), wxT(" "), wxTOKEN_STRTOK);
            if(!tokens.IsEmpty()) {
                configuredTerminal = wxStringTokenize(tokens.Item(0), wxT("/"), wxTOKEN_STRTOK);
                if(!configuredTerminal.IsEmpty()) {
                    terminal = configuredTerminal.Last();
                    tokens.Clear();
                    configuredTerminal.Clear();
                }
            }
        }
        if(!ExeLocator::Locate(terminal, where)) {
            wxMessageBox(_("Failed to locate the configured default terminal application required by gsgs, please "
                           "install it or check your configuration!"),
                         _("ｹ籃､ｹﾊﾊﾂ"), wxOK | wxCENTER | wxICON_WARNING, gsgs_Frame());
            return;
        }
        terminal.Clear();
    #endif

        if(attachPid == 1) { // attach to process
            AttachDbgProcDlg dlg(gsgs_Event().TopFrame());
            if(dlg.ShowModal() != wxID_OK) { return; }

            wxString processId = dlg.GetProcessId();
            exepath = dlg.GetExeName();
            debuggerName = dlg.GetDebugger();
            DebuggerMgr::Get().SetActiveDebugger(debuggerName);

            long processID(wxNOT_FOUND);
            if(!processId.ToCLong(&processID)) { processID = wxNOT_FOUND; }

            // Let the plugins process this first
            DebugEvent event(wxEVT_DBG_UI_ATTACH_TO_PROCESS);
            event.SetInt(processID);
            event.SetDebuggerName(debuggerName);
            if(gsgs_Event().ProcessEvent(event)) { return; }

            if(exepath.IsEmpty() == false) {
                wxFileName fn(exepath);
                wxSetWorkingDirectory(fn.GetPath());
                exepath = fn.GetFullName();
            }
            PID = processID;
            startup_info.pid = processID;
        }

        if(attachPid == wxNOT_FOUND) {
            // need to debug the current project
            proj = gsgs_CppWorker().FindProjectByName(gsgs_CppWorker().GetActiveProjectName(), errMsg);
            if(proj) {
                wxSetWorkingDirectory(proj->GetFileName().GetPath());
                bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), wxEmptyString);
                if(bldConf) {
                    debuggerName = bldConf->GetDebuggerType();
                    DebuggerMgr::Get().SetActiveDebugger(debuggerName);
                }
            }

            startup_info.project = gsgs_CppWorker().GetActiveProjectName();
        }

        // make sure we have an active debugger
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(!dbgr) {
            // No debugger available,
            wxString message;
            message << _("Failed to launch debugger '") << debuggerName << _("': debugger not loaded\n");
            message << _("Make sure that you have an open workspace and that the active project is of type 'Executable'");
            wxMessageBox(message, _("ｹ籃､ｹﾊﾊﾂ"), wxOK | wxICON_WARNING);
            return;
        }
        startup_info.debugger = dbgr;

        // Save the current layout
        GetPerspectiveManager().SavePerspective(NORMAL_LAYOUT);

        // set the debugger information
        DebuggerInformation dinfo;
        DebuggerMgr::Get().GetDebuggerInformation(debuggerName, dinfo);

        // if user override the debugger path, apply it
        wxString userDebuggr;
        if(bldConf) {
            userDebuggr = bldConf->GetDebuggerPath();
            userDebuggr.Trim().Trim(false);
            if(userDebuggr.IsEmpty() == false) {
                // expand project macros
                userDebuggr = MacroManager::Instance()->Expand(userDebuggr, PluginManager::Get(), proj->GetName(),
                                                               bldConf->GetName());

                // Convert any relative path to absolute path
                // see bug# https://sourceforge.net/p/codelite/bugs/871/
                wxFileName fnDebuggerPath(userDebuggr);
                if(fnDebuggerPath.IsRelative()) {
                    if(fnDebuggerPath.MakeAbsolute(proj->GetFileName().GetPath())) {
                        userDebuggr = fnDebuggerPath.GetFullPath();
                    }
                }
                dinfo.path = userDebuggr;
            } else if(bldConf->GetCompiler() && !bldConf->GetCompiler()->GetTool("Debugger").IsEmpty()) {
                // User specified a different compiler for this compiler - use it
                userDebuggr = bldConf->GetCompiler()->GetTool("Debugger");
                gsgs_Debug("Debugger is set to: '%s'", userDebuggr);
                dinfo.path = userDebuggr;
            }
        }

        // read the console command
        dinfo.consoleCommand = gsgs_EditorConfig().GetOptions()->GetProgramConsoleCommand();
        dbgr->SetDebuggerInformation(dinfo);

        // Apply the environment variables before starting
        EnvSetter env(NULL, NULL, proj ? proj->GetName() : wxString(), bldConf ? bldConf->GetName() : wxString());

        if(!bldConf && attachPid == wxNOT_FOUND) {
            wxString errmsg;
            errmsg << _("Could not find project configuration!\n")
                   << _("Make sure that everything is set properly in your project settings");
            ::wxMessageBox(errmsg, wxT("gsgs"), wxOK | wxICON_ERROR);
            return;

        } else if(attachPid == wxNOT_FOUND) {
            exepath = bldConf->GetCommand();

            // Get the debugging arguments.
            if(bldConf->GetUseSeparateDebugArgs()) {
                args = bldConf->GetDebugArgs();
            } else {
                args = bldConf->GetCommandArguments();
            }

            exepath.Prepend(wxT("\""));
            exepath.Append(wxT("\""));

            // Apply environment variables
            wd = bldConf->GetWorkingDirectory();

            // Expand variables before passing them to the debugger
            wd = MacroManager::Instance()->Expand(wd, NULL, proj->GetName());
            exepath = MacroManager::Instance()->Expand(exepath, NULL, proj->GetName());
        }

        wxString dbgname = dinfo.path;
        dbgname = gsgs_EditorConfig().ExpandVariables(dbgname, true);

        // set ourselves as the observer for the debugger class
        dbgr->SetObserver(this);

        // Set the 'Is remote debugging' flag'
        dbgr->SetIsRemoteDebugging(bldConf && bldConf->GetIsDbgRemoteTarget() && PID == wxNOT_FOUND);
        dbgr->SetIsRemoteExtended(bldConf && bldConf->GetIsDbgRemoteExtended() && PID == wxNOT_FOUND);

        if(proj) {
            dbgr->SetProjectName(proj->GetName());
        } else {
            dbgr->SetProjectName(wxT(""));
        }

        // Loop through the open editors and let each editor
        // a chance to update the debugger manager with any line
        // changes (i.e. file was edited and breakpoints were moved)
        // this loop must take place before the debugger start up
        // or else call to UpdateBreakpoint() will yield an attempt to
        // actually add the breakpoint before Run() is called - this can
        // be a problem when adding breakpoint to dll files.
        if(wxNOT_FOUND == attachPid) { gsgs_Frame()->GetMainBook()->UpdateBreakpoints(); }

        // We can now get all the gathered breakpoints from the manager
        std::vector<BreakpointInfo> bps;

        // since files may have been updated and the breakpoints may have been moved,
        // delete all the information
        GetBreakpointsMgr()->GetBreakpoints(bps);
        // Take the opportunity to store them in the pending array too
        GetBreakpointsMgr()->SetPendingBreakpoints(bps);

        // Launch the terminal
        if(bldConf && !bldConf->IsGUIProgram()) { // debugging a project and the project is not considered a "GUI" program
            m_debuggerTerminal.Clear();
    #ifndef __WXMSW__
            m_debuggerTerminal.Launch(DebuggerTerminalPOSIX::MakeExeTitle(exepath, args));
            if(!m_debuggerTerminal.IsValid()) {
                ::wxMessageBox(_("Could not launch terminal for debugger"), "gsgs", wxOK | wxCENTER | wxICON_ERROR, gsgs_Frame());
                return;
            }
    #endif
        }

        // notify plugins that we're about to start debugging
        {
            DebugEvent eventStarting(wxEVT_DEBUG_STARTING);
            eventStarting.SetClientData(&startup_info);
            if(gsgs_Event().ProcessEvent(eventStarting)) { return; }
        }

        // read
        wxArrayString dbg_cmds;
        DebugSessionInfo si;

        si.debuggerPath = dbgname;
        si.exeName = exepath;
        si.cwd = wd;
        si.bpList = bps;
        si.ttyName = m_debuggerTerminal.GetTty();
        si.PID = PID;
        si.enablePrettyPrinting = dinfo.enableGDBPrettyPrinting;
        si.cmds = ::wxStringTokenize(dinfo.startupCommands, "\r\n", wxTOKEN_STRTOK);

        if(bldConf) { si.searchPaths = bldConf->GetDebuggerSearchPaths(); }

        if(attachPid == wxNOT_FOUND) {
            // it is now OK to start the debugger...
            dbg_cmds = wxStringTokenize(bldConf->GetDebuggerStartupCmds(), wxT("\n"), wxTOKEN_STRTOK);

            // append project level commands to the global commands
            si.cmds.insert(si.cmds.end(), dbg_cmds.begin(), dbg_cmds.end());

            if(!dbgr->Start(si)) {
                wxString errMsg;
                errMsg << _("Failed to initialize debugger: ") << dbgname << wxT("\n");
                DebugMessage(errMsg);
                return;
            }
        } else {
            // append project level commands to the global commands
            si.cmds.insert(si.cmds.end(), dbg_cmds.begin(), dbg_cmds.end());

            // Attach to process...
            if(!dbgr->Attach(si)) {
                wxString errMsg;
                errMsg << _("Failed to initialize debugger: ") << dbgname << wxT("\n");
                DebugMessage(errMsg);
                return;
            }
        }

        // notify plugins that the debugger just started
        {
            DebugEvent eventStarted(wxEVT_DEBUG_STARTED);
            eventStarted.SetClientData(&startup_info);
            gsgs_Event().ProcessEvent(eventStarted);
        }

        // Clear the debugger output window
        gsgs_Frame()->GetDebuggerPane()->Clear();

        // Now the debugger has been fed the breakpoints, re-Initialise the breakpt view,
        // so that it uses debugger_ids instead of internal_ids
        // Hmm. The above comment is probably no longer true; but it'll do no harm to Initialise() anyway
        gsgs_Frame()->GetDebuggerPane()->GetBreakpointView()->Initialize();

        // Initialize the 'Locals' table. We do this for performane reason so we
        // wont need to read from the XML each time perform 'next' step
        gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->Initialize();

        // let the active editor get the focus
        Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor();
        if(editor) { editor->SetActive(); }

        // mark that we are waiting for the first GotControl()
        DebugMessage(output);
        DebugMessage(_("Debug session started successfully!\n"));

        if(dbgr->GetIsRemoteDebugging()) {

            // debugging remote target
            wxString comm;
            wxString port = bldConf->GetDbgHostPort();
            wxString host = bldConf->GetDbgHostName();

            comm << host;

            host = host.Trim().Trim(false);
            port = port.Trim().Trim(false);

            if(port.IsEmpty() == false) { comm << wxT(":") << port; }

            dbgr->Run(args, comm);

        } else if(attachPid == wxNOT_FOUND) {

            // debugging local target
            dbgr->Run(args, wxEmptyString);
        }

        GetPerspectiveManager().LoadPerspective(DEBUG_LAYOUT);

        // Hide the "Debugger Console" pane since we dont need it anymore
        wxAuiManager * aui = gsgs_Frame()->GetAuiManager();
        wxAuiPaneInfo& pi = aui->GetPane(wxT("Debugger Console"));
        if(pi.IsOk() && pi.IsShown()) {
            pi.Hide();
            aui->Update();
        }
    }
    //----------------------------------------------------------------------------
    void Manager::DbgStop()
    {
        {
            IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();

            if(dbgr && dbgr->IsRunning()) {
    // If the debugger is running assume that the current perspective is the
    // one that we want to use for our debugging purposes
    #ifndef __WXMAC__
                GetPerspectiveManager().SavePerspective(DEBUG_LAYOUT);
    #endif
            }
            GetPerspectiveManager().LoadPerspective(NORMAL_LAYOUT);
        }

        if(m_watchDlg) {
            m_watchDlg->Destroy();
            m_watchDlg = NULL;
        }

        m_debuggerTerminal.Clear();

        // remove all debugger markers
        DbgUnMarkDebuggerLine();

        // Mark the debugger as non interactive
        m_dbgCanInteract = false;

        // Keep the current watches for the next debug session
        m_dbgWatchExpressions = gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->GetExpressions();

        // clear the debugger pane
        gsgs_Frame()->GetDebuggerPane()->Clear();

        // Notify the breakpoint manager that the debugger has stopped
        GetBreakpointsMgr()->DebuggerStopped();

        gsgs_Frame()->GetDebuggerPane()->GetBreakpointView()->Initialize();

        // Clear the ascii viewer
        gsgs_Frame()->GetDebuggerPane()->GetAsciiViewer()->UpdateView(wxT(""), wxT(""));
        // update toolbar state
        UpdateStopped();

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(!dbgr) { return; }

        m_dbgCurrentFrameInfo.Clear();
        if(!dbgr->IsRunning()) {
            DebugEvent eventEnd(wxEVT_DEBUG_ENDED);
            gsgs_Event().ProcessEvent(eventEnd);
            return;
        }

        // notify plugins that the debugger is about to be stopped
        {
            DebugEvent eventEnding(wxEVT_DEBUG_ENDING);
            gsgs_Event().ProcessEvent(eventEnding);
        }

        if(dbgr->IsRunning()) dbgr->Stop();

        DebuggerMgr::Get().SetActiveDebugger(wxEmptyString);
        DebugMessage(_("Debug session ended\n"));

        // notify plugins that the debugger stopped
        DebugEvent eventEnd(wxEVT_DEBUG_ENDED);
        gsgs_Event().ProcessEvent(eventEnd);
    }
    //----------------------------------------------------------------------------
    void Manager::DbgMarkDebuggerLine(const wxString& fileName, int lineno)
    {
        DbgUnMarkDebuggerLine();
        if(lineno < 0) { return; }

        // try to open the file
        wxFileName fn(fileName);
        Editor* editor = gsgs_Frame()->GetMainBook()->GetActiveEditor(true);
        if(editor && editor->GetFileName().GetFullPath().CmpNoCase(fn.GetFullPath()) == 0 && lineno > 0) {
            editor->HighlightLine(lineno);
            editor->SetEnsureCaretIsVisible(editor->PositionFromLine(lineno - 1), false);

        } else {
            editor = gsgs_Frame()->GetMainBook()->OpenFile(fn.GetFullPath(), wxEmptyString, lineno - 1, wxNOT_FOUND);
            if(editor && lineno > 0) {
                editor->HighlightLine(lineno);
                editor->SetEnsureCaretIsVisible(
                    editor->PositionFromLine(lineno - 1), false,
                    true); // The 'true' says to delay; needed with wxGTK-3.1 else EnsureVisible() fails
            }
        }
    }
    //----------------------------------------------------------------------------
    void Manager::DbgUnMarkDebuggerLine()
    {
        // remove all debugger markers from all editors
        gsgs_Frame()->GetMainBook()->UnHighlightAll();
    }
    //----------------------------------------------------------------------------
    void Manager::DbgDoSimpleCommand(int cmd)
    {
        // Hide tooltip dialog if its ON
        if(GetDebuggerTip() && GetDebuggerTip()->IsShown()) { GetDebuggerTip()->HideDialog(); }

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning()) {
            switch(cmd) {
            case DBG_PAUSE:
                GetBreakpointsMgr()->SetExpectingControl(true);
                dbgr->Interrupt();
                dbgr->ListFrames();
                break;
            case DBG_NEXT:
                gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->ResetTableColors();
                gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->ResetTableColors();
                dbgr->Next();
                break;
            case DBG_STEPIN:
                gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->ResetTableColors();
                gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->ResetTableColors();
                dbgr->StepIn();
                break;
            case DBG_STEPI:
                gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->ResetTableColors();
                gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->ResetTableColors();
                dbgr->StepInInstruction();
                break;
            case DBG_STEPOUT:
                gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->ResetTableColors();
                gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->ResetTableColors();
                dbgr->StepOut();
                break;
            case DBG_SHOW_CURSOR:
                dbgr->QueryFileLine();
                break;
            case DBG_NEXTI:
                gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->ResetTableColors();
                gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->ResetTableColors();
                dbgr->NextInstruction();
                break;
            default:
                break;
            }
        }
    }
    //----------------------------------------------------------------------------
    void Manager::DbgSetFrame(int frame, int lineno)
    {
        wxAuiPaneInfo& info = gsgs_Frame()->GetAuiManager()->GetPane(wxT("Debugger"));
        if(info.IsShown()) {
            IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
            if(dbgr && dbgr->IsRunning() && DbgCanInteract()) {
                // set the frame
                dbgr->SetFrame(frame);
                m_frameLineno = lineno;
            }
        }
    }
    //----------------------------------------------------------------------------
    void Manager::DbgSetThread(long threadId)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning() && DbgCanInteract()) {
            // set the frame
            dbgr->SelectThread(threadId);
            dbgr->QueryFileLine();
        }
    }
    //----------------------------------------------------------------------------
    void Manager::UpdateAddLine(const wxString& line, const bool OnlyIfLoggingOn /*=false*/)
    {
        // There are a few messages that are only worth displaying if full logging is enabled
        if(OnlyIfLoggingOn) {
            IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
            if(dbgr && (dbgr->IsLoggingEnabled() == false)) { return; }
        }

        DebugMessage(line + wxT("\n"));
    }
    //----------------------------------------------------------------------------
    void Manager::UpdateFileLine(const wxString & filename, int lineno, bool repositionEditor)
    {
        wxString fileName = filename;
        long lineNumber = lineno;
        if(m_frameLineno != wxNOT_FOUND) {
            lineNumber = m_frameLineno;
            m_frameLineno = wxNOT_FOUND;
        }

        if(repositionEditor) { DbgMarkDebuggerLine(fileName, lineNumber); }

        UpdateDebuggerPane();
    }
    //----------------------------------------------------------------------------
    void Manager::UpdateGotControl(const DebuggerEventData & e)
    {
        int reason = e.m_controlReason;
        bool userTriggered = GetBreakpointsMgr()->GetExpectingControl();
        GetBreakpointsMgr()->SetExpectingControl(false);

        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr == NULL) {
            wxLogDebug(wxT("Active debugger not found :/"));
            return;
        }

        DebuggerInformation dinfo;
        DebuggerMgr::Get().GetDebuggerInformation(dbgr->GetName(), dinfo);
        // Raise gsgs (unless the cause is a hit bp, and we're configured not to
        //   e.g. because there's a command-list breakpoint ending in 'continue')
        if((reason != DBG_BP_HIT) || dinfo.whenBreakpointHitRaiseGSGS) {
            if(gsgs_Frame()->IsIconized() || !gsgs_Frame()->IsShown()) { gsgs_Frame()->Restore(); }
            long curFlags = gsgs_Frame()->GetWindowStyleFlag();
            gsgs_Frame()->SetWindowStyleFlag(curFlags | wxSTAY_ON_TOP);
            gsgs_Frame()->Raise();
            gsgs_Frame()->SetWindowStyleFlag(curFlags);
            m_dbgCanInteract = true;
        }

        SendCmdEvent(wxEVT_DEBUG_EDITOR_GOT_CONTROL);

        if(dbgr && dbgr->IsRunning()) {
            // in case we got an error, try and clear the Locals view
            if(reason == DBG_CMD_ERROR) { gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->Clear(); }
        }

        switch(reason) {
        case DBG_RECV_SIGNAL_SIGTRAP:        // DebugBreak()
        case DBG_RECV_SIGNAL_EXC_BAD_ACCESS: // SIGSEGV on Mac
        case DBG_RECV_SIGNAL_SIGABRT:        // assert() ?
        case DBG_RECV_SIGNAL_SIGPIPE:
        case DBG_RECV_SIGNAL_SIGSEGV: {      // program received signal sigsegv

            // Clear the 'Locals' view
            gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->Clear();

            wxString signame = wxT("SIGSEGV");

            // show the dialog only if the signal is not sigtrap
            // since sigtap might be triggered by user inserting a breakpoint
            // into an already running debug session
            bool showDialog(true);
            if(reason == DBG_RECV_SIGNAL_EXC_BAD_ACCESS) {
                signame = wxT("EXC_BAD_ACCESS");

            } else if(reason == DBG_RECV_SIGNAL_SIGABRT) {
                signame = wxT("SIGABRT");

            } else if(reason == DBG_RECV_SIGNAL_SIGTRAP) {
                signame = wxT("SIGTRAP");
                showDialog = !userTriggered;
            } else if(reason == DBG_RECV_SIGNAL_SIGPIPE) {
                signame = "SIGPIPE";
            }

            DebugMessage(_("Program Received signal ") + signame + wxT("\n"));
            if(showDialog) {
                wxMessageDialog dlg(gsgs_Frame(),
                                    _("Program Received signal ") + signame + wxT("\n") +
                                        _("Stack trace is available in the 'Call Stack' tab\n"),
                                    _("ｹ籃､ｹﾊﾊﾂ"), wxICON_ERROR | wxOK);
                dlg.ShowModal();
            }

            // Print the stack trace
            if(showDialog) {
                // select the "Call Stack" tab
                gsgs_Frame()->GetDebuggerPane()->SelectTab(DebuggerPane::FRAMES);
            }

            if(!userTriggered) {
                if(dbgr && dbgr->IsRunning()) {
                    dbgr->QueryFileLine();
                    dbgr->BreakList();
                    // Apply all previous watches
                    DbgRestoreWatches();
                }
            }
        } break;
        case DBG_BP_ASSERTION_HIT: {
            // Clear the 'Locals' view
            gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->Clear();

            wxMessageDialog dlg(gsgs_Frame(),
                                _("Assertion failed!\nStack trace is available in the 'Call Stack' tab\n"), _("ｹ籃､ｹﾊﾊﾂ"),
                                wxICON_ERROR | wxOK);
            dlg.ShowModal();

            // Print the stack trace
            wxAuiPaneInfo& info = gsgs_Frame()->GetAuiManager()->GetPane(wxT("Debugger"));
            if(info.IsShown()) {
                gsgs_Frame()->GetDebuggerPane()->SelectTab(DebuggerPane::FRAMES);
                CallAfter(&Manager::UpdateDebuggerPane);
            }
        } break;
        case DBG_BP_HIT: // breakpoint reached
            // Clear the 'Locals' view
            gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->Clear();
        // fall through...
        case DBG_END_STEPPING: // finished one of the following: next/step/nexti/stepi
        case DBG_FUNC_FINISHED:
        case DBG_UNKNOWN: // the most common reason: temporary breakpoint
            if(dbgr && dbgr->IsRunning()) {
                dbgr->QueryFileLine();
                dbgr->BreakList();
                // Apply all previous watches
                DbgRestoreWatches();
            }
            break;

        case DBG_DBGR_KILLED:
            m_dbgCanInteract = false;
            break;

        case DBG_EXIT_WITH_ERROR: {
            wxMessageBox(wxString::Format(_("Debugger exited with the following error string:\n%s"), e.m_text.c_str()),
                         _("gsgs"), wxOK | wxICON_ERROR);
            // fall through
        }

        case DBG_EXITED_NORMALLY:
            // debugging finished, stop the debugger process
            DbgStop();
            break;
        default:
            break;
        }
    }
    //----------------------------------------------------------------------------
    void Manager::UpdateLostControl()
    {
        // debugger lost control
        // hide the marker
        DbgUnMarkDebuggerLine();
        m_dbgCanInteract = false;
        DebugMessage(_("Continuing...\n"));

        // Reset the debugger call-stack pane
        gsgs_Frame()->GetDebuggerPane()->GetFrameListView()->Clear();
        gsgs_Frame()->GetDebuggerPane()->GetFrameListView()->SetCurrentLevel(0);

        SendCmdEvent(wxEVT_DEBUG_EDITOR_LOST_CONTROL);
    }
    //----------------------------------------------------------------------------
    void Manager::UpdateTypeReolsved(const wxString& expr, const wxString& type_name)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        // Sanity
        if(dbgr == NULL) { return; }
        if(dbgr->IsRunning() == false) { return; }
        if(DbgCanInteract() == false) { return; }

        // gdb returns usually expression like:
        // const string &, so in order to get the actual type
        // we construct a valid expression by appending a valid identifier followed by a semi colon.
        wxString expression;
        wxString command(expr);
        wxString dbg_command(wxT("print"));
        wxString expression_type;

        DebuggerSettingsPreDefMap data;
        gsgs_Config().Read(wxT("DebuggerCommands"), &data);
        DebuggerPreDefinedTypes preDefTypes = data.GetActiveSet();
        DebuggerCmdDataVec cmds = preDefTypes.GetCmds();

        expression << wxT("/^");
        expression << type_name;
        expression << wxT(" someValidName;");
        expression << wxT("$/");

        Variable variable;
        if(LanguageST::Get()->VariableFromPattern(expression, wxT("someValidName"), variable)) {
            expression_type = _U(variable.m_type.c_str());
            for(size_t i = 0; i < cmds.size(); i++) {
                DebuggerCmdData cmd = cmds.at(i);
                if(cmd.GetName() == expression_type) {
                    // prepare the string to be evaluated
                    command = cmd.GetCommand();
                    command = MacroManager::Instance()->Replace(command, wxT("variable"), expr, true);

                    dbg_command = cmd.GetDbgCommand();

                    //---------------------------------------------------
                    // Special handling for the templates
                    //---------------------------------------------------

                    wxArrayString types = DoGetTemplateTypes(_U(variable.m_templateDecl.c_str()));
                    // Case 1: list
                    // The user defined scripts requires that we pass info like this:
                    // plist <list name> <T>
                    if(expression_type == wxT("list") && types.GetCount() > 0) { command << wxT(" ") << types.Item(0); }
                    // Case 2: map & multimap
                    // The user defined script requires that we pass the TLeft & TRight
                    // pmap <list name> TLeft TRight
                    if((expression_type == wxT("map") || expression_type == wxT("multimap")) && types.GetCount() > 1) {
                        command << wxT(" ") << types.Item(0) << wxT(" ") << types.Item(1);
                    }

                    break;
                }
            }
        }

        wxString output;
        bool get_tip(false);

        Notebook* book = gsgs_Frame()->GetDebuggerPane()->GetNotebook();
        if(book->GetPageText(book->GetSelection()) == DebuggerPane::ASCII_VIEWER ||
           IsPaneVisible(DebuggerPane::ASCII_VIEWER)) {
            get_tip = true;
        }

        if(get_tip) {
            dbgr->GetAsciiViewerContent(dbg_command, command); // Will trigger a call to UpdateTip()
        }
    }
    //----------------------------------------------------------------------------
    void Manager::UpdateAsciiViewer(const wxString& expression, const wxString& tip)
    {
        gsgs_Frame()->GetDebuggerPane()->GetAsciiViewer()->UpdateView(expression, tip);
    }
    //----------------------------------------------------------------------------
    void Manager::UpdateRemoteTargetConnected(const wxString& line)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning() && gsgs_Workspace().IsOpen()) {
            // we currently do not support this feature when debugging using 'Quick debug'
            wxString errMsg;
            ProjectPtr proj = gsgs_CppWorker().FindProjectByName(gsgs_CppWorker().GetActiveProjectName(), errMsg);
            BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(proj->GetName(), wxEmptyString);
            if(bldConf) {
                wxArrayString dbg_cmds =
                    wxStringTokenize(bldConf->GetDebuggerPostRemoteConnectCmds(), wxT("\n"), wxTOKEN_STRTOK);
                for(size_t i = 0; i < dbg_cmds.GetCount(); i++) {
                    dbgr->ExecuteCmd(dbg_cmds.Item(i));
                }
            }
        }
        // log the line
        UpdateAddLine(line);
    }
    //----------------------------------------------------------------------------
    bool Manager::IsBuildInProgress() const
    {
        BuildEvent buildEvent(wxEVT_GET_IS_BUILD_IN_PROGRESS);
        gsgs_Event().ProcessEvent(buildEvent);
        return buildEvent.IsRunning() || (m_shellProcess && m_shellProcess->IsBusy());
    }
    //----------------------------------------------------------------------------
    void Manager::StopBuild()
    {
        BuildEvent stopEvent(wxEVT_STOP_BUILD);
        if(gsgs_Event().ProcessEvent(stopEvent)) { return; }

        // Mark this build as 'interrupted'
        gsgs_Frame()->GetOutputPane()->GetBuildTab()->SetBuildInterrupted(true);

        if(m_shellProcess && m_shellProcess->IsBusy()) { m_shellProcess->Stop(); }
        m_buildQueue.clear();
    }
    //----------------------------------------------------------------------------
    void Manager::PushQueueCommand(const QueueCommand& buildInfo) 
    { 
        m_buildQueue.push_back(buildInfo); 
    }
    //----------------------------------------------------------------------------
    void Manager::ProcessCommandQueue()
    {
        if(m_buildQueue.empty()) { return; }

        // pop the next build build and process it
        QueueCommand qcmd = m_buildQueue.front();
        m_buildQueue.pop_front();

        if(qcmd.GetCheckBuildSuccess() && !IsBuildEndedSuccessfully()) {
            // build failed, remove command from the queue
            return;
        }

        switch(qcmd.GetKind()) {
        case QueueCommand::kExecuteNoDebug:
            ExecuteNoDebug(qcmd.GetProject());
            break;

        case QueueCommand::kCustomBuild:
            DoCustomBuild(qcmd);
            break;

        case QueueCommand::kClean:
            DoCleanProject(qcmd);
            break;

        case QueueCommand::kRebuild:
        case QueueCommand::kBuild:
            DoBuildProject(qcmd);
            break;

        case QueueCommand::kDebug:
            DbgStart(wxNOT_FOUND);
            break;
        }
    }
    //----------------------------------------------------------------------------
    void Manager::BuildWorkspace()
    {
        DoCmdWorkspace(QueueCommand::kBuild);
        ProcessCommandQueue();
    }
    //----------------------------------------------------------------------------
    void Manager::CleanWorkspace()
    {
        DoCmdWorkspace(QueueCommand::kClean);
        ProcessCommandQueue();
    }
    //----------------------------------------------------------------------------
    void Manager::RebuildWorkspace()
    {
        DoCmdWorkspace(QueueCommand::kClean);
        DoCmdWorkspace(QueueCommand::kBuild);
        ProcessCommandQueue();
    }
    //----------------------------------------------------------------------------
    void Manager::RunCustomPreMakeCommand(const wxString& project)
    {
        if(m_shellProcess && m_shellProcess->IsBusy()) { return; }

        wxString conf;
        // get the selected configuration to be built
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(project, wxEmptyString);
        if(bldConf) { conf = bldConf->GetName(); }
        QueueCommand info(project, conf, false, QueueCommand::kBuild);

        if(m_shellProcess) { delete m_shellProcess; }
        m_shellProcess = new CompileRequest(info,
                                            wxEmptyString, // no file name (valid only for build file only)
                                            true);         // run premake step only
        m_shellProcess->Process(PluginManager::Get());
    }
    //----------------------------------------------------------------------------
    void Manager::CompileFile(const wxString& projectName, const wxString& fileName, bool preprocessOnly)
    {
        if(m_shellProcess && m_shellProcess->IsBusy()) { return; }

        DoSaveAllFilesBeforeBuild();

        // If a debug session is running, stop it.
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning()) {
            if(wxMessageBox(_("This would terminate the current debug session, continue?"), _("Confirm"),
                            wxICON_QUESTION | wxYES_NO | wxCANCEL) != wxYES)
                return;
            DbgStop();
        }

        wxString conf;
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) { conf = bldConf->GetName(); }

        QueueCommand info(projectName, conf, false, QueueCommand::kBuild);
        if(bldConf && bldConf->IsCustomBuild()) {
            info.SetCustomBuildTarget(preprocessOnly ? _("Preprocess File") : _("Compile Single File"));
            info.SetKind(QueueCommand::kCustomBuild);
        }

        if(m_shellProcess) { delete m_shellProcess; }
        switch(info.GetKind()) {
        case QueueCommand::kBuild:
            m_shellProcess = new CompileRequest(info, fileName, false, preprocessOnly);
            break;
        case QueueCommand::kCustomBuild:
            m_shellProcess = new CustomBuildRequest(info, fileName);
            break;
        default:
            m_shellProcess = NULL;
            break;
        }
        m_shellProcess->Process(PluginManager::Get());
    }
    //----------------------------------------------------------------------------
    bool Manager::IsBuildEndedSuccessfully() const
    {
        // return the result of the last build
        return gsgs_Frame()->GetOutputPane()->GetBuildTab()->GetBuildEndedSuccessfully();
    }
    //----------------------------------------------------------------------------
    void Manager::DoBuildProject(const QueueCommand& buildInfo)
    {
        if(m_shellProcess && m_shellProcess->IsBusy()) return;

        DoSaveAllFilesBeforeBuild();

        // If a debug session is running, stop it.
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning()) {
            if(wxMessageBox(_("This would terminate the current debug session, continue?"), _("Confirm"),
                            wxICON_QUESTION | wxYES_NO | wxCANCEL) != wxYES)
                return;
            DbgStop();
        }

        if(m_shellProcess) { delete m_shellProcess; }

        m_shellProcess = new CompileRequest(buildInfo);
        m_shellProcess->Process(PluginManager::Get());
    }
    //----------------------------------------------------------------------------
    void Manager::DoCleanProject(const QueueCommand& buildInfo)
    {
        if(m_shellProcess && m_shellProcess->IsBusy()) { return; }

        if(m_shellProcess) { delete m_shellProcess; }
        m_shellProcess = new CleanRequest(buildInfo);
        m_shellProcess->Process(PluginManager::Get());
    }

    void Manager::DoCustomBuild(const QueueCommand& buildInfo)
    {
        if(m_shellProcess && m_shellProcess->IsBusy()) { return; }

        DoSaveAllFilesBeforeBuild();

        // If a debug session is running, stop it.
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        if(dbgr && dbgr->IsRunning()) {
            if(wxMessageBox(_("This would terminate the current debug session, continue?"), _("Confirm"),
                            wxICON_QUESTION | wxYES_NO | wxCANCEL) != wxYES)
                return;
            DbgStop();
        }

        wxDELETE(m_shellProcess);
        m_shellProcess = new CustomBuildRequest(buildInfo, wxEmptyString);
        m_shellProcess->Process(PluginManager::Get());
    }

    void Manager::DoCmdWorkspace(int cmd)
    {
        // get list of projects
        wxArrayString projects;
        wxArrayString optimizedList;

        ManagerST::Get()->GetProjectList(projects);

        for(size_t i = 0; i < projects.GetCount(); i++) {
            ProjectPtr p = GetProject(projects.Item(i));
            BuildConfigPtr buildConf = gsgs_CppWorker().GetProjBuildConf(projects.Item(i), wxEmptyString);
            if(p && buildConf && buildConf->IsProjectEnabled()) {
                wxArrayString deps = p->GetDependencies(buildConf->GetName());
                for(size_t j = 0; j < deps.GetCount(); j++) {
                    // add a project only if it does not exist yet
                    if(optimizedList.Index(deps.Item(j)) == wxNOT_FOUND) { optimizedList.Add(deps.Item(j)); }
                }
                // add the project itself now, again only if it is not included yet
                if(optimizedList.Index(projects.Item(i)) == wxNOT_FOUND) { optimizedList.Add(projects.Item(i)); }
            }
        }

        // add a build/clean project only command for every project in the optimized list
        for(size_t i = 0; i < optimizedList.GetCount(); i++) {
            BuildConfigPtr buildConf = gsgs_CppWorker().GetProjBuildConf(optimizedList.Item(i), wxEmptyString);
            if(buildConf && buildConf->IsProjectEnabled()) {
                QueueCommand bi(optimizedList.Item(i), buildConf->GetName(), true, cmd);
                if(buildConf->IsCustomBuild()) {
                    bi.SetKind(QueueCommand::kCustomBuild);
                    switch(cmd) {
                    case QueueCommand::kBuild:
                        bi.SetCustomBuildTarget(wxT("Build"));
                        break;
                    case QueueCommand::kClean:
                        bi.SetCustomBuildTarget(wxT("Clean"));
                        break;
                    }
                }
                bi.SetCleanLog(i == 0);
                PushQueueCommand(bi);
            }
        }
    }

    void Manager::DbgClearWatches() { m_dbgWatchExpressions.Clear(); }

    void Manager::DebuggerUpdate(const DebuggerEventData& event)
    {
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        DebuggerInformation dbgInfo = dbgr ? dbgr->GetDebuggerInformation() : DebuggerInformation();
        switch(event.m_updateReason) {

        case DBG_UR_GOT_CONTROL:
            // keep the current function name
            m_dbgCurrentFrameInfo.func = event.m_frameInfo.function;
            UpdateGotControl(event);
            break;

        case DBG_UR_LOST_CONTROL:
            UpdateLostControl();
            break;

        case DBG_UR_FILE_LINE:
            // in some cases we don't physically reposition the file+line position, such as during updates made by user
            // actions (like add watch)
            // but since this app uses a debugger refresh to update newly added watch values, it automatically
            // repositions the editor always. this isn't always desirable behavior, so we pass a parameter indicating
            // for certain operations if an override was used
            UpdateFileLine(event.m_file, event.m_line, true);
            // raise the flag for the next call, as this "override" is only used once per consumption
            ManagerST::Get()->SetRepositionEditor(true);
            break;

        case DBG_UR_FRAMEDEPTH: {
            long frameDepth(0);
            event.m_frameInfo.level.ToLong(&frameDepth);
            m_dbgCurrentFrameInfo.depth = frameDepth;
            // gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->UpdateFrameInfo();
            break;
        }

        case DBG_UR_VAROBJUPDATE:
            // notify the 'Locals' view to remove all
            // out-of-scope variable objects
            gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->OnVariableObjUpdate(event);
            gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->OnUpdateVariableObject(event);
            break;

        case DBG_UR_ADD_LINE:
            UpdateAddLine(event.m_text, event.m_onlyIfLogging);
            break;

        case DBG_UR_BP_ADDED:
            GetBreakpointsMgr()->SetBreakpointDebuggerID(event.m_bpInternalId, event.m_bpDebuggerId);
            break;

        case DBG_UR_STOPPED:
            // Nothing to do here
            break;

        case DBG_UR_LOCALS:
            gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->UpdateLocals(event.m_locals);
    #ifdef __WXMAC__
            {
                for(size_t i = 0; i < event.m_locals.size(); i++) {
                    LocalVariable v = event.m_locals.at(i);
                    if(v.gdbId.IsEmpty() == false) { dbgr->DeleteVariableObject(v.gdbId); }
                }
            }
    #endif
            break;

        case DBG_UR_FUNC_ARGS:
            gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->UpdateFuncArgs(event.m_locals);
    #ifdef __WXMAC__
            {
                for(size_t i = 0; i < event.m_locals.size(); i++) {
                    LocalVariable v = event.m_locals.at(i);
                    if(v.gdbId.IsEmpty() == false) { dbgr->DeleteVariableObject(v.gdbId); }
                }
            }
    #endif
            break;

        case DBG_UR_EXPRESSION:
            // gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->UpdateExpression ( event.m_expression,
            // event.m_evaluated );
            break;

        case DBG_UR_FUNCTIONFINISHED:
            gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->UpdateFuncReturnValue(event.m_expression);
            break;

        case DBG_UR_REMOTE_TARGET_CONNECTED:
            UpdateRemoteTargetConnected(event.m_text);
            break;

        case DBG_UR_RECONCILE_BPTS:
            GetBreakpointsMgr()->ReconcileBreakpoints(event.m_bpInfoList);
            break;

        case DBG_UR_BP_HIT:
            GetBreakpointsMgr()->BreakpointHit(event.m_bpDebuggerId);
            break;

        case DBG_UR_TYPE_RESOLVED:
            if(event.m_userReason == DBG_USERR_WATCHTABLE) {
                gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->OnTypeResolved(event);

            } else if(event.m_userReason == DBG_USERR_QUICKWACTH) {
                wxString expr = ::DbgPrependCharPtrCastIfNeeded(event.m_expression, event.m_evaluated);
                dbgr->CreateVariableObject(expr, false, DBG_USERR_QUICKWACTH);

            } else {
                // Default
                UpdateTypeReolsved(event.m_expression, event.m_evaluated);
            }
            break;

        case DBG_UR_ASCII_VIEWER:
            UpdateAsciiViewer(event.m_expression, event.m_text);
            break;

        case DBG_UR_LISTTHRAEDS:
            gsgs_Frame()->GetDebuggerPane()->GetThreadsView()->PopulateList(event.m_threads);
            break;

        case DBG_UR_WATCHMEMORY:
            gsgs_Frame()->GetDebuggerPane()->GetMemoryView()->SetViewString(event.m_evaluated);
            break;

        case DBG_UR_VARIABLEOBJUPDATEERR:
            // Variable object update fail!
            break;

        case DBG_UR_VARIABLEOBJCREATEERR:
            // Variable creation error, remove it from the relevant table
            if(event.m_userReason == DBG_USERR_WATCHTABLE) {
                gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->OnCreateVariableObjError(event);

            } else if(event.m_userReason == DBG_USERR_QUICKWACTH) {
                GetDebuggerTip()->OnCreateVariableObjError(event);
            }
            break;

        case DBG_UR_VARIABLEOBJ: {
            // CreateVariableObject callback
            if(event.m_userReason == DBG_USERR_QUICKWACTH) {

                /////////////////////////////////////////////
                // Handle Tooltips
                /////////////////////////////////////////////
                DoShowQuickWatchDialog(event);

                // Handle ASCII Viewer
                wxString expression(event.m_expression);
                if(event.m_variableObject.isPtr && !event.m_expression.StartsWith(wxT("*"))) {
                    if(event.m_variableObject.typeName.Contains(wxT("char *")) ||
                       event.m_variableObject.typeName.Contains(wxT("wchar_t *")) ||
                       event.m_variableObject.typeName.Contains(wxT("QChar *")) ||
                       event.m_variableObject.typeName.Contains(wxT("wxChar *"))) {
                        // dont de-reference
                    } else {
                        expression.Prepend(wxT("(*"));
                        expression.Append(wxT(")"));
                    }
                }
                UpdateTypeReolsved(expression, event.m_variableObject.typeName);
            } else if(event.m_userReason == DBG_USERR_WATCHTABLE) {
                // Double clicked on the 'Watches' table
                gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->OnCreateVariableObject(event);

            } else if(event.m_userReason == DBG_USERR_LOCALS) {
                gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->OnCreateVariableObj(event);
            }
        } break;
        case DBG_UR_LISTCHILDREN: {

            if(event.m_userReason == QUERY_NUM_CHILDS || event.m_userReason == LIST_WATCH_CHILDS) {
                // Watch table
                gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->OnListChildren(event);

            } else if(event.m_userReason == QUERY_LOCALS_CHILDS || event.m_userReason == LIST_LOCALS_CHILDS ||
                      event.m_userReason == QUERY_LOCALS_CHILDS_FAKE_NODE) {
                // Locals table
                gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->OnListChildren(event);

            } else {
                // Tooltip
                IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
                if(dbgr && dbgr->IsRunning() && DbgCanInteract()) {
                    if(GetDebuggerTip() && !GetDebuggerTip()->IsShown()) {
                        GetDebuggerTip()->BuildTree(event.m_varObjChildren, dbgr);
                        GetDebuggerTip()->m_mainVariableObject = event.m_expression;
                        GetDebuggerTip()->ShowDialog(
                            (event.m_userReason == DBG_USERR_WATCHTABLE || event.m_userReason == DBG_USERR_LOCALS));

                    } else if(GetDebuggerTip()) {
                        // The dialog is shown
                        GetDebuggerTip()->AddItems(event.m_expression, event.m_varObjChildren);
                    }
                }
            }
        } break;
        case DBG_UR_EVALVARIABLEOBJ:

            // EvaluateVariableObject callback
            if(event.m_userReason == DBG_USERR_WATCHTABLE) {
                gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->OnEvaluateVariableObj(event);

            } else if(event.m_userReason == DBG_USERR_LOCALS) {
                gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->OnEvaluateVariableObj(event);

            } else if(GetDebuggerTip()) {
                GetDebuggerTip()->UpdateValue(event.m_expression, event.m_evaluated);
            }
            break;
        case DBG_UR_INVALID:
        default:
            break;
        }
    }
    //----------------------------------------------------------------------------
    void Manager::DbgRestoreWatches()
    {
        // restore any saved watch expressions from previous debug sessions
        if(m_dbgWatchExpressions.empty() == false) {
            for(size_t i = 0; i < m_dbgWatchExpressions.GetCount(); i++) {
                DebugMessage(wxT("Restoring watch: ") + m_dbgWatchExpressions.Item(i) + wxT("\n"));
                wxCommandEvent e(wxEVT_COMMAND_MENU_SELECTED, XRCID("add_watch"));
                e.SetString(m_dbgWatchExpressions.Item(i));
                gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->GetEventHandler()->ProcessEvent(e);
            }
            m_dbgWatchExpressions.Clear();
        }
    }
    //----------------------------------------------------------------------------
    void Manager::DoRestartGSGS()
    {
        wxString restartGSGSCommand;
    #if defined(__WXGTK__) || defined(__WXMSW__)
        // The Shell is our friend
        restartGSGSCommand << gsgs_Path().GetExecutablePath();

        // Restore the original working dir and any paramters
        for(int i = 1; i < wxTheApp->argc; ++i) 
        {
            wxString cmdArg = wxTheApp->argv[i];
            ::WrapWithQuotes(cmdArg);
            restartGSGSCommand << wxT(" ") << cmdArg;
        }
        wxSetWorkingDirectory(gsgs_Path().GetOriginalCwd());

        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxID_EXIT);
        gsgs_Frame()->GetEventHandler()->AddPendingEvent(event);

        ::wxExecute(restartGSGSCommand, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER);
    #else // OSX

        // on OSX, we use the open command
        wxFileName bundlePath(gsgs_Path().GetBinFolder(), "");
        bundlePath.RemoveLastDir(); // MacOS
        bundlePath.RemoveLastDir(); // Contents
        wxString bundlePathStr = bundlePath.GetPath();
        ::WrapWithQuotes(bundlePathStr);
        restartGSGSCommand << "sleep 2 && /usr/bin/open " << bundlePathStr;
        ::WrapInShell(restartGSGSCommand);
        wxCommandEvent event(wxEVT_COMMAND_MENU_SELECTED, wxID_EXIT);
        gsgs_Frame()->GetEventHandler()->AddPendingEvent(event);
        gsgsSYSTEM() << "Restarting gsgs:" << restartGSGSCommand;
        wxExecute(restartGSGSCommand, wxEXEC_ASYNC | wxEXEC_NOHIDE | wxEXEC_MAKE_GROUP_LEADER);
    #endif
    }
    //----------------------------------------------------------------------------
    void Manager::OnRestart(Event& event)
    {
        wxUnusedVar(event);
        DoRestartGSGS();
    }
    //----------------------------------------------------------------------------
    void Manager::DoShowQuickWatchDialog(const DebuggerEventData& event)
    {
        /////////////////////////////////////////////
        // Handle Tooltips
        /////////////////////////////////////////////

        bool useDialog = (event.m_userReason == DBG_USERR_WATCHTABLE || event.m_userReason == DBG_USERR_LOCALS);
        IDebugger* dbgr = DebuggerMgr::Get().GetActiveDebugger();
        bool canInteract = (dbgr && dbgr->IsRunning() && DbgCanInteract());
        DisplayVariableDlg* view = NULL;

        if(canInteract) {
            // First see if this type has a user-defined alternative
            // If so, don't do anything here, just create a new  for the u-d type
            // That'll bring us back here, but with the correct data
            DebuggerSettingsPreDefMap data;
            gsgs_Config().Read(wxT("DebuggerCommands"), &data);
            DebuggerPreDefinedTypes preDefTypes = data.GetActiveSet();

            wxString preDefinedType =
                preDefTypes.GetPreDefinedTypeForTypename(event.m_variableObject.typeName, event.m_expression);
            wxString exp, pdt;
            pdt = preDefinedType;
            exp = event.m_expression;

            pdt.Trim().Trim(false);
            exp.Trim().Trim(false);

            if(!pdt.IsEmpty() && pdt != exp) {
                dbgr->CreateVariableObject(pdt, false, DBG_USERR_QUICKWACTH);
    #ifdef __WXMAC__
                if(!event.m_variableObject.gdbId.IsEmpty()) { dbgr->DeleteVariableObject(event.m_variableObject.gdbId); }
    #endif
                return;
            }

            // Editor Tooltip
            view = GetDebuggerTip();
            view->m_mainVariableObject = event.m_variableObject.gdbId;
            view->m_variableName = event.m_expression;
            view->m_expression = event.m_expression;

            if(event.m_evaluated.IsEmpty() == false) { view->m_variableName << wxT(" = ") << event.m_evaluated; }

            if(event.m_variableObject.typeName.IsEmpty() == false) {
                view->m_variableName << wxT(" [") << event.m_variableObject.typeName << wxT("] ");
            }

            if(event.m_variableObject.numChilds > 0 || event.m_variableObject.has_more) {
                // Complex type
                dbgr->ListChildren(event.m_variableObject.gdbId, event.m_userReason);

            } else {
                // Simple type, no need for further calls, show the dialog
                if(!view->IsShown()) {
                    view->BuildTree(event.m_varObjChildren, dbgr);
                    // If the reason for showing the dialog was the 'Watches' table being d-clicked,
                    // center the dialog
                    view->ShowDialog(useDialog);
                }
            }
        }
    }
    //----------------------------------------------------------------------------
    void Manager::UpdateParserPaths(bool notify)
    {
        // Parser paths are now updated via the compile_commands.json which is auto generated when:
        // 1. Build process is ended
        // 2. Workspace is loaded
        wxBusyCursor bc;

        // If we have an opened workspace, get its search paths
        if(gsgs_Workspace().IsOpen()) {
            wxArrayString projects;
            gsgs_CppWorker().GetProjectList(projects);
            for(size_t i = 0; i < projects.GetCount(); ++i) {
                ProjectPtr pProj = gsgs_CppWorker().GetProject(projects.Item(i));
                if(pProj) {
                    wxArrayString compilerIncPaths = pProj->GetIncludePaths();
                    ParseThreadST::Get()->AddPaths(compilerIncPaths, {});
                }
            }

            // get the workspace search paths and append them to the current paths
            wxArrayString localInc, localExc;
            gsgs_CppWorker().GetLocalWorkspace()->GetParserPaths(localInc, localExc);
            ParseThreadST::Get()->AddPaths(localInc, localExc);

            // get the build configuration specific paths and add them as well
            BuildConfigPtr buildConf = GetCurrentBuildConf();
            if(buildConf) {
                wxString projSearchPaths = buildConf->GetCcSearchPaths();
                wxArrayString projectInc = wxStringTokenize(projSearchPaths, wxT("\r\n"), wxTOKEN_STRTOK);

                // Expand macros
                for(wxString& projectPath : projectInc) {
                    projectPath =
                        MacroManager::Instance()->Expand(projectPath, PluginManager::Get(), gsgs_CppWorker().GetActiveProjectName());
                }
                ParseThreadST::Get()->AddPaths(projectInc, {});
            }
        }

        // Get the global paths from the configuration (settings->code completion->ctags->search paths)
        const TagsOptionsData& tod = gsgs_Frame()->GetTagsOptions();
        const wxArrayString& globalInc = tod.GetParserSearchPaths();
        const wxArrayString& globalExc = tod.GetParserExcludePaths();
        ParseThreadST::Get()->AddPaths(globalInc, globalExc);

        {
            wxArrayString inc, exc;
            ParseThreadST::Get()->GetSearchPaths(inc, exc);
            gsgsDEBUG() << "Parser paths are now set to:" << inc;
        }
    }
    //----------------------------------------------------------------------------
    void Manager::OnIncludeFilesScanDone(wxCommandEvent& event)
    {
        gsgs_Frame()->GetMainStatusBar()->SetMessage(_("Retagging..."));

        wxBusyCursor busyCursor;
        std::set<wxString>* fileSet = (std::set<wxString>*)event.GetClientData();

        wxArrayString projects;
        GetProjectList(projects);

        gsgsDEBUG() << "Scan for include files is done";

        gsgsDEBUG() << "Building project file list...";
        std::vector<wxFileName> projectFiles;
        for(size_t i = 0; i < projects.GetCount(); i++) {
            ProjectPtr proj = GetProject(projects.Item(i));
            if(proj) { proj->GetFilesAsVectorOfFileName(projectFiles); }
        }

        // add to this set the workspace files to create a unique list of
        // files
        for(size_t i = 0; i < projectFiles.size(); i++) {
            wxString fn(projectFiles.at(i).GetFullPath());
            fileSet->insert(fn);
        }
        gsgsDEBUG() << "Building project file list...done";

        gsgsDEBUG() << "Converting set to vector...";
        // recreate the list in the form of vector (the API requirs vector)
        projectFiles.clear();
        std::set<wxString>::iterator iter = fileSet->begin();
        projectFiles.reserve(fileSet->size());

        for(; iter != fileSet->end(); ++iter) {
            wxFileName fn(*iter);
            if(fn.IsRelative()) { fn.MakeAbsolute(); }
            projectFiles.push_back(fn);
        }
        gsgsDEBUG() << "Converting set to vector...done";

    #if !USE_PARSER_TREAD_FOR_RETAGGING_WORKSPACE
        wxStopWatch sw;
        sw.Start();
    #endif

        // -----------------------------------------------
        // tag them
        // -----------------------------------------------
        TagsManagerST::Get()->RetagFiles(projectFiles, event.GetInt() ? TagsManager::Retag_Quick : TagsManager::Retag_Full);

    #if !USE_PARSER_TREAD_FOR_RETAGGING_WORKSPACE
        long end = sw.Time();
        gsgs_Frame()->SetStatusMessage(_("Done"), 0);
        gsgsLogMessage(wxT("INFO: Retag workspace completed in %d seconds (%d files were scanned)"), (end) / 1000,
                     projectFiles.size());
        SendCmdEvent(wxEVT_FILE_RETAGGED, (void*)&projectFiles);
    #endif

        wxDELETE(fileSet);
    }
    //----------------------------------------------------------------------------
    void Manager::DoSaveAllFilesBeforeBuild()
    {
        // Save all files before compiling, but dont saved new documents
        SendCmdEvent(wxEVT_FILE_SAVE_BY_BUILD_START);
        if(!gsgs_Frame()->GetMainBook()->SaveAll(false, false)) {
            SendCmdEvent(wxEVT_FILE_SAVE_BY_BUILD_END);
            return;
        }
        SendCmdEvent(wxEVT_FILE_SAVE_BY_BUILD_END);
    }
    //----------------------------------------------------------------------------
    DisplayVariableDlg* Manager::GetDebuggerTip()
    {
        if(!m_watchDlg) { m_watchDlg = new DisplayVariableDlg(gsgs_Frame()->GetMainBook()); }
        return m_watchDlg;
    }
    //----------------------------------------------------------------------------
    void Manager::OnProjectSettingsModified(ProjectSettingsEvent& event)
    {
        event.Skip();
        // Get the project settings
        gsgs_Frame()->SelectBestEnvSet();
    }
    //----------------------------------------------------------------------------
    void Manager::OnDbContentCacherLoaded(wxCommandEvent& event) { gsgsLogMessage(event.GetString()); }
    //----------------------------------------------------------------------------
    void Manager::GetActiveProjectAndConf(wxString& project, wxString& conf)
    {
        if(!gsgs_Workspace().IsOpen()) {
            project.Clear();
            conf.Clear();
            return;
        }

        project = gsgs_CppWorker().GetActiveProjectName();
        BuildMatrixPtr matrix = gsgs_CppWorker().GetBuildMatrix();
        if(!matrix) { return; }

        wxString workspaceConf = matrix->GetSelectedConfigurationName();
        matrix->GetProjectSelectedConf(workspaceConf, project);
    }
    //----------------------------------------------------------------------------
    void Manager::UpdatePreprocessorFile(Editor* editor) { wxUnusedVar(editor); }
    //----------------------------------------------------------------------------
    BuildConfigPtr Manager::GetCurrentBuildConf()
    {
        wxString project, conf;
        GetActiveProjectAndConf(project, conf);
        if(project.IsEmpty()) return NULL;

        return gsgs_CppWorker().GetProjBuildConf(project, conf);
    }
    //----------------------------------------------------------------------------
    void Manager::GetActiveFileProjectFiles(wxArrayString& files)
    {
        // Send an event to the plugins to get a list of the current file's project files
        // If no plugin has replied, use the default GetProjectFiles method
        wxCommandEvent e(wxEVT_CMD_GET_CURRENT_FILE_PROJECT_FILES);
        e.SetEventObject(this);
        e.SetClientData(&files);
        if(!gsgs_Event().ProcessEvent(e)) {
            // Set default project name
            wxString project = gsgs_CppWorker().GetActiveProjectName();
            if(gsgs_Frame()->GetMainBook()->GetActiveEditor()) {
                // use the active file's project
                wxFileName activeFile = gsgs_Frame()->GetMainBook()->GetActiveEditor()->GetFileName();
                project = GetProjectNameByFile(activeFile.GetFullPath());
            }
            GetProjectFiles(project, files);
        }
    }
    //----------------------------------------------------------------------------
    void Manager::GetActiveProjectFiles(wxArrayString& files)
    {
        wxCommandEvent e(wxEVT_CMD_GET_ACTIVE_PROJECT_FILES);
        e.SetEventObject(this);
        e.SetClientData(&files);
        if(!gsgs_Event().ProcessEvent(e)) { GetProjectFiles(gsgs_CppWorker().GetActiveProjectName(), files); }
    }
    //----------------------------------------------------------------------------
    bool Manager::DbgCanInteract()
    {
        /// First, we also propogate this question to the plugins
        DebugEvent de(wxEVT_DBG_CAN_INTERACT);
        if(gsgs_Event().ProcessEvent(de)) {
            // a plugin answered this question, we assume that a debug session is running by
            // another plugin so avoid using our built-in debugger
            return de.IsAnswer();
        }
        return m_dbgCanInteract;
    }
    //----------------------------------------------------------------------------
    void Manager::OnAddWorkspaceToRecentlyUsedList(wxCommandEvent& e)
    {
        // We don't call e.Skip() here
        wxFileName fn(e.GetString());
        if(fn.FileExists()) { gsgs_Workspace().AddRecently(fn.GetFullPath()); }
    }
    //----------------------------------------------------------------------------
    void Manager::GenerateCompileCommands() {}
    //----------------------------------------------------------------------------
    void Manager::OnBuildEnded(BuildEvent& event) { event.Skip(); }
    //----------------------------------------------------------------------------
    void Manager::DbgContinue()
    {
        IDebugger* debugger = DebuggerMgr::Get().GetActiveDebugger();
        if(debugger && debugger->IsRunning()) {
            // debugger is already running, so issue a 'cont' command
            gsgs_Frame()->GetDebuggerPane()->GetLocalsTable()->ResetTableColors();
            gsgs_Frame()->GetDebuggerPane()->GetWatchesTable()->ResetTableColors();
            debugger->Continue();
        }
    }
    //----------------------------------------------------------------------------
    void Manager::OnBuildStarting(BuildEvent& event)
    {
        // Always Skip it
        event.Skip();

        if(!gsgs_CppWorker().IsOpen()) return;

        gsgsStringSet usedCompilers, deletedCompilers;
        gsgs_CppWorker().GetCompilers(usedCompilers);

        // Check to see if any of the compilers were deleted
        gsgsStringSet::iterator iter = usedCompilers.begin();
        wxString strDeletedCompilers;
        for(; iter != usedCompilers.end(); ++iter) {
            if(!gsgs_Settings().IsCompilerExist(*iter)) {
                deletedCompilers.insert(*iter);
                strDeletedCompilers << "'" << *iter << "'\n";
            }
        }

        if(deletedCompilers.empty())
            // nothing more to be done here
            return;

        strDeletedCompilers.RemoveLast(); // remove last \n

        // Prompt the user and suggest an alternative
        CompilersModifiedDlg dlg(NULL, deletedCompilers);
        if(dlg.ShowModal() != wxID_OK) {
            event.Skip(false);
            wxString message;
            message << _("Build cancelled. The following compilers referred by the workspace could not be found:\n")
                    << strDeletedCompilers << "\n"
                    << _("Please fix your project settings by selecting a valid compiler");
            ::wxMessageBox(message, "Build Aborted", wxOK | wxCENTER | wxICON_ERROR, gsgs_Event().TopFrame());
            return;

        } else {

            // User mapped the old compilers with new ones -> create an alias between the actual compiler and the
            gsgsStringMap table = dlg.GetReplacementTable();

            // Clone each compiler
            gsgsStringMap::iterator iterTable = table.begin();
            for(; iterTable != table.end(); ++iterTable) {
                CompilerPtr pCompiler = gsgs_Settings().GetCompiler(iterTable->second);
                pCompiler->SetName(iterTable->first);
                gsgs_Settings().SetCompiler(pCompiler);
            }

            // Prompt the user and cancel the build
            ::wxMessageBox(_("Compilers updated successfully!\nYou can now build your workspace"), "gsgs",
                           wxOK | wxCENTER | wxICON_INFORMATION);
            event.Skip(false);
        }
    }
    //----------------------------------------------------------------------------
    bool Manager::StartTTY(const wxString& title, wxString& tty)
    {
    #ifndef __WXMSW__
        m_debuggerTerminal.Clear();
        m_debuggerTerminal.Launch(title);
        if(m_debuggerTerminal.IsValid()) { tty = m_debuggerTerminal.GetTty(); }
        return m_debuggerTerminal.IsValid();
    #else
        wxUnusedVar(title);
        wxUnusedVar(tty);
        return false;
    #endif
    }
    //----------------------------------------------------------------------------
    void Manager::OnParserThreadSuggestColourTokens(Event& event)
    {
        const wxArrayString& tokens = event.GetStrings();
        wxString locals, classes;
        if(tokens.size() == 2) {
            classes = tokens.Item(0);
            locals = tokens.Item(1);
        }

        wxString originatingFile = event.GetFileName();

        Editor* editor = gsgs_Frame()->GetMainBook()->FindEditor(originatingFile);
        if(editor) { editor->GetContext()->ColourContextTokens(classes, locals); }
    }
    //----------------------------------------------------------------------------
    void Manager::OnProjectRenamed(Event & event)
    {
        event.Skip();
        if(gsgs_CppWorker().IsOpen()) 
        { 
            ReloadWorkspace(); 
        }
    }
    //----------------------------------------------------------------------------
    void Manager::OnFindInFilesDismissed(FindInFilesEvent& event)
    {
        event.Skip();
        if(gsgs_CppWorker().IsOpen()) {
            gsgs_Config().Write("FindInFiles/CXX/Mask", event.GetFileMask());
            gsgs_Config().Write("FindInFiles/CXX/LookIn", event.GetPaths());
        }
    }
    //----------------------------------------------------------------------------
    void Manager::OnCmdRestart(wxCommandEvent& event)
    {
        wxUnusedVar(event);
        DoRestartGSGS();
    }
    //----------------------------------------------------------------------------
    bool Manager::IsDebuggerViewVisible(const wxString& name)
    {
        DebuggerPane* debuggerPane = gsgs_Frame()->GetDebuggerPane();
        if(debuggerPane) {
            int sel = debuggerPane->GetNotebook()->GetSelection();
            if(sel != wxNOT_FOUND) {
                if(debuggerPane->GetNotebook()->GetPageText(sel) == name) { return true; }
            }
        }

        // Also test if the pane is detached
        return IsPaneVisible(name);
    }
    //----------------------------------------------------------------------------
    void Manager::ShowNewProjectWizard(const wxString& workspaceFolder)
    {
        wxFrame* mainFrame = gsgs_Event().TopFrame();
        NewProjectEvent newProjectEvent(wxEVT_NEW_PROJECT_WIZARD_SHOWING);
        newProjectEvent.SetEventObject(mainFrame);
        if(gsgs_Event().ProcessEvent(newProjectEvent)) { return; }

        NewProjectDialog dlg(mainFrame);
        if(dlg.ShowModal() == wxID_OK) {

            ProjectData data = dlg.GetProjectData();
            // Ensure that the project directory exists
            if(!wxFileName::DirExists(data.m_path)) {
                wxFileName::Mkdir(data.m_path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }
        
            // Try the plugins first
            NewProjectEvent finishEvent(wxEVT_NEW_PROJECT_WIZARD_FINISHED);
            finishEvent.SetEventObject(mainFrame);
            finishEvent.SetToolchain(data.m_cmpType);
            finishEvent.SetDebugger(data.m_debuggerType);
            finishEvent.SetProjectName(data.m_name);
            finishEvent.SetProjectFolder(data.m_path);
            finishEvent.SetTemplateName(data.m_sourceTemplate);
            if(gsgs_Event().ProcessEvent(finishEvent)) { return; }

            // Carry on with the default behaviour
            CreateProject(data, workspaceFolder);
        }
    }
    //----------------------------------------------------------------------------
    void Manager::OnUpdateDebuggerActiveView(DebugEvent& event)
    {
        if(DebuggerMgr::Get().IsNativeDebuggerRunning()) {
            UpdateDebuggerPane();

        } else {
            event.Skip();
        }
    }
    //----------------------------------------------------------------------------
    void Manager::OnDebuggerSetMemory(DebugEvent& event)
    {
        if(DebuggerMgr::Get().IsNativeDebuggerRunning()) {
            SetMemory(event.GetMemoryAddress(), event.GetMemoryBlockSize(), event.GetMemoryBlockValue());
        } else {
            event.Skip();
        }
    }
    //----------------------------------------------------------------------------
    void Manager::OnHideGdbTooltip(Event& event)
    {
        event.Skip();
        if(GetDebuggerTip()) { GetDebuggerTip()->HideDialog(); }
    }
    //----------------------------------------------------------------------------
    void Manager::OnFindInFilesShowing(FindInFilesEvent& event)
    {
        event.Skip();
        if(gsgs_CppWorker().IsOpen()) {
            wxString temp;

            // Load the C++ workspace values from the configuration
            gsgs_Config().Read("FindInFiles/CXX/Mask", temp,
                wxString("*.c;*.cpp;*.cxx;*.cc;*.h;*.hpp;*.inc;*.mm;*.m;*.xrc;"
                    "*.xml;*.json;*.sql;*.txt;*.plist;CMakeLists.txt;*.rc;*.iss"));
            event.SetFileMask(temp);
            gsgs_Config().Read("FindInFiles/CXX/LookIn", temp, wxString("<Entire Workspace>"));
            event.SetPaths(temp);
        }
    }
    //----------------------------------------------------------------------------
}