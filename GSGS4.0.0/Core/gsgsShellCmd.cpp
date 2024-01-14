
#include "gsgsShellCmd.h"
#include "gsgsEditorConfig.h"
#include "process_impl.h"
#include "globals.h"
#include "gsgsWorkspace.h"
#include "gsgsBuildConfig.h"
#include "gsgsEvent.h"
#include "gsgsProcess.h"
#include "gsgsBuilderGnuMake.h"
#include "gsgsToolsetManager.h"
#include "gsgsCommon.h"
#include "gsgsLogManager.h"
#include "gsgsMacroManager.h"
#include "plugin.h"
#include <wx/xrc/xmlres.h>
#include <wx/app.h>
#include <wx/regex.h>

#ifdef __WXMSW__
#define ECHO_CMD wxT("@echo ")
#define ECHO_OFF wxT("@echo off")
#define SILENCE_OP wxT("@")
#define SCRIPT_EXT wxT(".bat")
#else
#define ECHO_CMD wxT("echo ")
#define ECHO_OFF wxT("")
#define SILENCE_OP wxT("")
#define SCRIPT_EXT wxT(".sh")
#endif

namespace gsgs
{
    const wxEventType wxEVT_SHELL_COMMAND_ADDLINE = XRCID("wxEVT_SHELL_COMMAND_ADDLINE");
    const wxEventType wxEVT_SHELL_COMMAND_STARTED = XRCID("wxEVT_SHELL_COMMAND_STARTED");
    const wxEventType wxEVT_SHELL_COMMAND_PROCESS_ENDED = XRCID("wxEVT_SHELL_COMMAND_PROCESS_ENDED");
    const wxEventType wxEVT_SHELL_COMMAND_STARTED_NOCLEAN = XRCID("wxEVT_SHELL_COMMAND_STARTED_NOCLEAN");
    //------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------
    // QueueCommand
    //------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------
    QueueCommand::QueueCommand(const wxString & project, const wxString& configuration, bool projectOnly, int kind)
        : m_project(project)
        , m_configuration(configuration)
        , m_projectOnly(projectOnly)
        , m_kind(kind)
        , m_cleanLog(true)
        , m_checkBuildSuccess(false)
    {
    }
    //------------------------------------------------------------------------------------
    QueueCommand::QueueCommand(int kind)
        : m_projectOnly(false)
        , m_kind(kind)
        , m_cleanLog(true)
        , m_checkBuildSuccess(false)
    {
        // Fill with default values
        if (gsgs_CppWorker().IsOpen()) {
            m_project = gsgs_CppWorker().GetActiveProjectName();
            BuildConfigPtr buildPtr = gsgs_CppWorker().GetProjBuildConf(m_project, "");
            wxASSERT_MSG(buildPtr, "No active project");

            // If a 'Build' or 'Clean' kinds where requested 
            // and the project build configuration is Custom build
            // change the kind to CustomBuild and set the proper build
            // targets
            if (m_kind == kBuild && buildPtr->IsCustomBuild()) {
                // change the type to CustomBuild
                m_kind = kCustomBuild;
                SetCustomBuildTarget("Build");

            }
            else if (m_kind == kClean && buildPtr->IsCustomBuild()) {
                // change the type to CustomBuild
                m_kind = kCustomBuild;
                SetCustomBuildTarget("Clean");

            }
            else {
                m_configuration = buildPtr->GetName();

            }
        }
    }
    //------------------------------------------------------------------------------------
    QueueCommand::~QueueCommand()
    {
    }
    //------------------------------------------------------------------------------------
    void QueueCommand::SetConfiguration(const wxString& configuration)
    {
        m_configuration = configuration;
    }
    //------------------------------------------------------------------------------------
    void QueueCommand::SetProject(const wxString& project)
    {
        m_project = project;
    }
    //------------------------------------------------------------------------------------
    const wxString& QueueCommand::GetConfiguration() const
    {
        return m_configuration;
    }
    //------------------------------------------------------------------------------------
    const wxString& QueueCommand::GetProject() const
    {
        return m_project;
    }
    //------------------------------------------------------------------------------------
    void QueueCommand::SetProjectOnly(bool set)
    {
        m_projectOnly = set;
    }
    //------------------------------------------------------------------------------------
    const bool& QueueCommand::GetProjectOnly() const
    {
        return m_projectOnly;
    }
    //------------------------------------------------------------------------------------
    void QueueCommand::SetKind(int kind)
    {
        m_kind = kind;
    }
    //------------------------------------------------------------------------------------
    const int& QueueCommand::GetKind() const
    {
        return m_kind;
    }
    //------------------------------------------------------------------------------------
    void QueueCommand::SetCleanLog(bool cleanLog)
    {
        this->m_cleanLog = cleanLog;
    }
    //------------------------------------------------------------------------------------
    const bool& QueueCommand::GetCleanLog() const
    {
        return m_cleanLog;
    }
    //------------------------------------------------------------------------------------
    wxString QueueCommand::DeriveSynopsis() const
    {
        wxString synopsis;
        switch (m_kind) {
        case kBuild:
            synopsis << wxT("Building ");
            break;
        case kClean:
            synopsis << wxT("Cleaning ");
            break;
        case kCustomBuild:
            synopsis << wxT("Making '") << m_customBuildTarget << wxT("' In ");
            break;
        case kDebug:
            synopsis << wxT("Debugging ");
            break;
        case kExecuteNoDebug:
            synopsis << "Executing ";
            break;
        default:
            synopsis << wxT("In ");
            break;
        }
        synopsis << m_project << wxT(" (") << m_configuration << wxT(")");
        return synopsis;
    }
    //------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------
    // ShellCommand
    //------------------------------------------------------------------------------------
    //------------------------------------------------------------------------------------
    ShellCommand::ShellCommand(const QueueCommand &buildInfo)
        : m_proc(NULL)
        , m_info(buildInfo)
    {
        Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ShellCommand::OnProcessOutput, this);
        Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ShellCommand::OnProcessTerminated, this);
    }
    //------------------------------------------------------------------------------------
    void ShellCommand::AppendLine(const wxString &line)
    {
        wxCommandEvent event(wxEVT_SHELL_COMMAND_ADDLINE);
        event.SetString(line);
        event.SetInt(m_info.GetKind());
        gsgs_Event().AddPendingEvent(event);
    }
    //------------------------------------------------------------------------------------
    void ShellCommand::Stop()
    {
        //kill the build process
        CleanUp();
    }
    //------------------------------------------------------------------------------------
    void ShellCommand::SendStartMsg()
    {
        gsgs::Event event(m_info.GetCleanLog() ? wxEVT_SHELL_COMMAND_STARTED : wxEVT_SHELL_COMMAND_STARTED_NOCLEAN);
        event.SetString(m_info.GetSynopsis());

        BuildEventDetails *eventData = new BuildEventDetails();
        eventData->SetProjectName(m_info.GetProject());
        eventData->SetConfiguration(m_info.GetConfiguration());
        eventData->SetIsCustomProject(m_info.GetKind() == QueueCommand::kCustomBuild);
        eventData->SetIsClean(m_info.GetKind() == QueueCommand::kClean || (m_info.GetKind() == QueueCommand::kCustomBuild && m_info.GetCustomBuildTarget() == wxT("clean")));

        event.SetClientObject(eventData);
        gsgs_Event().AddPendingEvent(event);
    }
    //------------------------------------------------------------------------------------
    void ShellCommand::SendEndMsg()
    {
        wxCommandEvent event(wxEVT_SHELL_COMMAND_PROCESS_ENDED);
        event.SetString(m_info.GetSynopsis());
        gsgs_Event().AddPendingEvent(event);
    }
    //------------------------------------------------------------------------------------
    void ShellCommand::DoPrintOutput(const wxString &out)
    {
        // Write the buffer as-is to the build tab !!
        AppendLine(out);
    }
    //------------------------------------------------------------------------------------
    void ShellCommand::CleanUp()
    {
        wxDELETE(m_proc);
        SendEndMsg();
    }
    //------------------------------------------------------------------------------------
    void ShellCommand::DoSetWorkingDirectory(ProjectPtr proj, bool isCustom, bool isFileOnly)
    {
        //when using custom build, user can select different working directory
        if (proj) {
            if (isCustom) {
                //first set the path to the project working directory
                ::wxSetWorkingDirectory(proj->GetFileName().GetPath());

                BuildConfigPtr buildConf = gsgs_CppWorker().GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
                if (buildConf) {
                    wxString wd = buildConf->GetCustomBuildWorkingDir();
                    if (wd.IsEmpty()) {
                        // use the project path
                        wd = proj->GetFileName().GetPath();
                    }
                    else {
                        // expand macros from path
                        wd = ExpandAllVariables(wd, &gsgs_CppWorker(), proj->GetName(), buildConf->GetName(), wxEmptyString);
                    }
                    ::wxSetWorkingDirectory(wd);
                }
            }
            else {
                if (m_info.GetProjectOnly() || isFileOnly) {
                    //first set the path to the project working directory
                    ::wxSetWorkingDirectory(proj->GetFileName().GetPath());
                }
            }
        }
    }
    //------------------------------------------------------------------------------------
    void ShellCommand::OnProcessOutput(gsgs::ProcessEvent& e)
    {
        DoPrintOutput(e.GetOutput());
    }
    //------------------------------------------------------------------------------------
    void ShellCommand::OnProcessTerminated(gsgs::ProcessEvent& e)
    {
        wxUnusedVar(e);
        CleanUp();
    }
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    // CompileRequest
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    CompileRequest::CompileRequest(
        const QueueCommand& buildInfo, const wxString& fileName, bool runPremakeOnly, bool preprocessOnly)
        : ShellCommand(buildInfo)
        , m_fileName(fileName)
        , m_premakeOnly(runPremakeOnly)
        , m_preprocessOnly(preprocessOnly)
    {
    }
    //--------------------------------------------------------------------------------
    CompileRequest::~CompileRequest()
    {
        // no need to delete the process, it will be deleted by the wx library
    }
    //--------------------------------------------------------------------------------
    // do the actual cleanup
    void CompileRequest::Process(IManager* manager)
    {
        wxString cmd;
        wxString errMsg;
        gsgsStringMap om;

        BuildSettingsConfig* bsc(manager ? manager->GetBuildSettingsConfigManager() : &gsgs_Settings());
        CxxWS* w(manager ? manager->GetWorkspace() : &gsgs_CppWorker());
        EditorConfig* env(manager ? manager->GetEnv() : &gsgs_EditorConfig());

        ProjectPtr proj = w->FindProjectByName(m_info.GetProject(), errMsg);
        if (!proj) {
            AppendLine(_("Cant find project: ") + m_info.GetProject());
            return;
        }

        wxString pname(proj->GetName());
        wxArrayString unresolvedVars;
        proj->GetUnresolvedMacros(m_info.GetConfiguration(), unresolvedVars);
        if (!unresolvedVars.IsEmpty()) {
            // We can't continue
            wxString msg;
            msg << _("The following environment variables are used in the project, but are not defined:\n");
            for (size_t i = 0; i < unresolvedVars.size(); ++i) {
                msg << unresolvedVars.Item(i) << "\n";
            }
            msg << _("Build anyway?");
            wxStandardID res = ::PromptMessageDialogWithoutCancel(msg, "UnresolvedMacros", _("Yes"), _("No"),
                _("Remember my answer and don't ask me again"), wxYES_NO | wxICON_WARNING | wxYES_DEFAULT);
            if (res != wxID_YES) {
                ::wxMessageBox(_("Build Cancelled!"), "gsgs", wxICON_ERROR | wxOK | wxCENTER);
                return;
            }
        }

        // Notify plugins that a compile process is going to start
        gsgs::BuildEvent event(wxEVT_BUILD_STARTING);
        event.SetProjectName(pname);
        event.SetConfigurationName(m_info.GetConfiguration());

        if (gsgs_Event().ProcessEvent(event)) {

            // the build is being handled by some plugin, no need to build it
            // using the standard way
            return;
        }

        // Send the EVENT_STARTED : even if this event is sent, next event will
        // be post, so no way to be sure the the build process has not started
        SendStartMsg();

        // if we require to run the makefile generation command only, replace the 'cmd' with the
        // generation command line
        BuildConfigPtr bldConf = w->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
        if (bldConf) {
            // BuilderPtr builder = bm->GetBuilder("Default");
            BuilderPtr builder = bldConf->GetBuilder();
            wxString args = bldConf->GetBuildSystemArguments();
            if (m_fileName.IsEmpty() == false) {
                // we got a complie request of a single file
                cmd = m_preprocessOnly ?
                    builder->GetPreprocessFileCmd(
                        m_info.GetProject(), m_info.GetConfiguration(), args, m_fileName, errMsg) :
                    builder->GetSingleFileCmd(m_info.GetProject(), m_info.GetConfiguration(), args, m_fileName);
            }
            else if (m_info.GetProjectOnly()) {

                switch (m_info.GetKind()) {
                case QueueCommand::kRebuild:
                    cmd = builder->GetPORebuildCommand(m_info.GetProject(), m_info.GetConfiguration(), args);
                    break;
                default:
                case QueueCommand::kBuild:
                    cmd = builder->GetPOBuildCommand(m_info.GetProject(), m_info.GetConfiguration(), args);
                    break;
                }

            }
            else {
                cmd = builder->GetBuildCommand(m_info.GetProject(), m_info.GetConfiguration(), args);
            }

            wxString cmpType = bldConf->GetCompilerType();
            CompilerPtr cmp = bsc->GetCompiler(cmpType);
            if (cmp) {
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
                if (!cmp->GetPathVariable().IsEmpty()) {
                    pathvar << cmp->GetPathVariable() << clPATH_SEPARATOR;
                }
                pathvar << "$PATH";
                om["PATH"] = pathvar;
            }
        }

        if (cmd.IsEmpty()) {
            // if we got an error string, use it
            if (errMsg.IsEmpty() == false) {
                AppendLine(errMsg);
            }
            else {
                AppendLine(_("Command line is empty. Build aborted."));
            }
            return;
        }

        WrapInShell(cmd);
        DirSaver ds;

        DoSetWorkingDirectory(proj, false, m_fileName.IsEmpty() == false);

        // expand the variables of the command
        cmd = ExpandAllVariables(cmd, w, m_info.GetProject(), m_info.GetConfiguration(), m_fileName);

        // print the build command
        AppendLine(cmd + wxT("\n"));
        if (m_info.GetProjectOnly() || m_fileName.IsEmpty() == false) {
            // set working directory
            DoSetWorkingDirectory(proj, false, m_fileName.IsEmpty() == false);
        }

        // print the prefix message of the build start. This is important since the parser relies
        // on this message
        if (m_info.GetProjectOnly() || m_fileName.IsEmpty() == false) {
            wxString configName(m_info.GetConfiguration());

            // also, send another message to the main frame, indicating which project is being built
            // and what configuration
            wxString text;
            text << wxGetTranslation(BUILD_PROJECT_PREFIX) << m_info.GetProject() << wxT(" - ") << configName << wxT(" ]");
            if (m_fileName.IsEmpty()) {
                text << wxT("----------\n");
            }
            else if (m_preprocessOnly) {
                text << wxT(" (Preprocess Single File)----------\n");
            }
            else {
                text << wxT(" (Single File Build)----------\n");
            }
            AppendLine(text);
        }

        // Avoid Unicode chars coming from the compiler by setting LC_ALL to "C"
        om["LC_ALL"] = "C";

        EnvSetter envir(env, &om, proj->GetName(), m_info.GetConfiguration());
        m_proc = CreateAsyncProcess(this, cmd);
        if (!m_proc) {
            wxString message;
            message << _("Failed to start build process, command: ") << cmd << _(", process terminated with exit code: 0");
            AppendLine(message);
            return;
        }
    }
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    // CustomBuildRequest
    //--------------------------------------------------------------------------------
    //--------------------------------------------------------------------------------
    CustomBuildRequest::CustomBuildRequest(const QueueCommand& buildInfo, const wxString& fileName)
        : ShellCommand(buildInfo)
        , m_fileName(fileName)
    {
    }
    //--------------------------------------------------------------------------------
    CustomBuildRequest::~CustomBuildRequest() {}
    //--------------------------------------------------------------------------------
    void CustomBuildRequest::Process(IManager* manager)
    {
        wxString cmd;
        wxString errMsg;
        gsgsStringMap om;

        CxxWS* w(manager->GetWorkspace());
        EditorConfig* env(manager->GetEnv());

        ProjectPtr proj = w->FindProjectByName(m_info.GetProject(), errMsg);
        if (!proj) {
            AppendLine(_("Cant find project: ") + m_info.GetProject());
            return;
        }

        // Notify plugins that a compile process is going to start
        gsgs::BuildEvent event(wxEVT_BUILD_STARTING);
        event.SetProjectName(proj->GetName());
        event.SetConfigurationName(m_info.GetConfiguration());

        if (gsgs_Event().ProcessEvent(event)) {
            // the build is being handled by some plugin, no need to build it
            // using the standard way
            return;
        }

        SendStartMsg();

        BuildConfigPtr bldConf = w->GetProjBuildConf(m_info.GetProject(), m_info.GetConfiguration());
        if (!bldConf) {
            gsgsLogMessage(wxString::Format(wxT("Failed to find build configuration for project '%s' and configuration '%s'"),
                m_info.GetProject().c_str(), m_info.GetConfiguration().c_str()));
            return;
        }

        // try the special targets first:
        bool isClean(false);
        if (m_info.GetCustomBuildTarget() == wxT("Build")) {
            cmd = bldConf->GetCustomBuildCmd();

        }
        else if (m_info.GetCustomBuildTarget() == wxT("Clean")) {
            cmd = bldConf->GetCustomCleanCmd();
            isClean = true;
        }
        else if (m_info.GetCustomBuildTarget() == wxT("Rebuild")) {
            cmd = bldConf->GetCustomRebuildCmd();

        }
        else if (m_info.GetCustomBuildTarget() == wxT("Compile Single File")) {
            cmd = bldConf->GetSingleFileBuildCommand();

        }
        else if (m_info.GetCustomBuildTarget() == wxT("Preprocess File")) {
            cmd = bldConf->GetPreprocessFileCommand();
        }

        // if still no luck, try with the other custom targets
        if (cmd.IsEmpty()) {
            std::map<wxString, wxString> targets = bldConf->GetCustomTargets();
            std::map<wxString, wxString>::iterator iter = targets.find(m_info.GetCustomBuildTarget());
            if (iter != targets.end()) {
                cmd = iter->second;
            }
        }

        if (cmd.IsEmpty()) {
            // if we got an error string, use it
            if (errMsg.IsEmpty() == false) {
                AppendLine(errMsg);
            }
            else {
                AppendLine(_("Command line is empty. Build aborted."));
            }
            return;
        }

        // Working directory:
        // By default we use the project path
        //////////////////////////////////////////////////////

        DirSaver ds;

        // first set the path to the project working directory
        ::wxSetWorkingDirectory(proj->GetFileName().GetPath());

        // If a working directory was specified, use it instead
        wxString wd = bldConf->GetCustomBuildWorkingDir();
        wd.Trim().Trim(false);

        wxString filename = m_fileName;
        if (filename.IsEmpty() && manager->GetActiveEditor()) {
            filename = manager->GetActiveEditor()->GetFileName().GetFullPath();
        }

        if (wd.IsEmpty()) {
            // use the project path
            wd = proj->GetFileName().GetPath();

        }
        else {

            // expand macros from the working directory
            wd = ExpandAllVariables(wd, &gsgs_CppWorker(), proj->GetName(), bldConf->GetName(), filename);
        }

        {
            // Ensure that the path to the working directory exist
            wxFileName fnwd(wd, "");
            if (!fnwd.DirExists()) {
                fnwd.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
            }
        }

        ::wxSetWorkingDirectory(wd);

        // Print message to the build tab
        AppendLine(wxString::Format(_("MESSAGE: Entering directory `%s'\n"), wd.c_str()));

        // Command handling:
        //////////////////////////////////////////////////////

        // expand the variables of the command
        cmd = ExpandAllVariables(cmd, w, m_info.GetProject(), m_info.GetConfiguration(), filename);

        // in case our configuration includes post/pre build commands
        // we generate a makefile to include them as well and we update
        // the build command
        bool bCommandAltered = DoUpdateCommand(manager, cmd, proj, bldConf, isClean);

#ifdef __WXMSW__
        // Windows CD command requires the paths to be backslashe
        if (cmd.Find(wxT("cd ")) != wxNOT_FOUND) cmd.Replace(wxT("/"), wxT("\\"));
#endif

        // Wrap the build command in the shell, so it will be able
        // to perform 'chain' commands like
        // cd SOMEWHERE && make && ...

        // Dont wrap the command if it was altered previously
        if (!bCommandAltered) {
            WrapInShell(cmd);
        }

        // print the build command
        AppendLine(cmd + wxT("\n"));
        wxString configName(m_info.GetConfiguration());

        // also, send another message to the main frame, indicating which project is being built
        // and what configuration
        wxString text;
        if (isClean) {
            text << wxGetTranslation(CLEAN_PROJECT_PREFIX);
        }
        else {
            text << wxGetTranslation(BUILD_PROJECT_PREFIX);
        }
        text << m_info.GetProject() << wxT(" - ") << configName << wxT(" ]----------\n");

        AppendLine(text);

        // Avoid Unicode chars coming from the compiler by setting LC_ALL to "C"
        om["LC_ALL"] = "C";
        EnvSetter environment(env, &om, proj->GetName(), m_info.GetConfiguration());

        m_proc = CreateAsyncProcess(this, cmd);
        if (!m_proc) {
            wxString message;
            message << _("Failed to start build process, command: ") << cmd << _(", process terminated with exit code: 0");
            AppendLine(message);
            return;
        }
    }
    //--------------------------------------------------------------------------------
    bool CustomBuildRequest::DoUpdateCommand(IManager* manager, wxString& cmd, ProjectPtr proj, BuildConfigPtr bldConf,
        bool isClean)
    {
        BuildCommandList preBuildCmds, postBuildCmds;
        wxArrayString pre, post;
        bldConf->GetPreBuildCommands(preBuildCmds);
        bldConf->GetPostBuildCommands(postBuildCmds);

        // collect all enabled commands
        BuildCommandList::iterator iter = preBuildCmds.begin();
        for (; iter != preBuildCmds.end(); iter++) {
            BuildCommand command = *iter;
            if (command.GetEnabled()) {
                pre.Add(command.GetCommand());
            }
        }

        iter = postBuildCmds.begin();
        for (; iter != postBuildCmds.end(); iter++) {
            BuildCommand command = *iter;
            if (command.GetEnabled()) {
                post.Add(command.GetCommand());
            }
        }

        if (pre.empty() && post.empty()) {
            return false;
        }

        // we need to create a makefile which includes all the pre-build, the actual build command and the post-build
        // commands
        // (in this exact order).

        wxString script;
        script << ECHO_OFF << wxT("\n");

        if (pre.IsEmpty() == false && !isClean) {
            script << ECHO_CMD << wxT("Executing Pre Build commands ...\n");
            for (size_t i = 0; i < pre.GetCount(); i++) {
                script << SILENCE_OP << pre.Item(i) << wxT("\n");
            }
            script << ECHO_CMD << wxT("Done\n");
        }

        // add the command
        script << cmd << wxT("\n");

        if (post.IsEmpty() == false && !isClean) {
            script << ECHO_CMD << wxT("Executing Post Build commands ...\n");
            for (size_t i = 0; i < post.GetCount(); i++) {
                script << SILENCE_OP << post.Item(i) << wxT("\n");
            }
            script << ECHO_CMD << wxT("Done\n");
        }

        // write the makefile
        wxFFile output;
        wxString fn;
        fn << proj->GetName() << SCRIPT_EXT;

        output.Open(fn, wxT("w+"));
        if (output.IsOpened()) {
            output.Write(script);
            output.Close();
        }

        cmd.Clear();
#ifdef __WXMSW__
        cmd << wxT("\"") << fn << wxT("\"");
#else
        // *nix
        cmd << wxT("/bin/sh './") << fn << wxT("'");
#endif
        return true;
    }
    //--------------------------------------------------------------------------------
}