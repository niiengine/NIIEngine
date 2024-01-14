#include "ExternalToolsProcessManager.h"
#include "gsgsEvent.h"
#include "globals.h"
#include "imanager.h"
#include "gsgsProcess.h"
#include "gsgsMacroManager.h"
#include "gsgsEditorConfig.h"
#include "process_impl.h"
#include <wx/utils.h>
#include <wx/process.h>
#include "gsgsLogManager.h"
#ifndef __WXMSW__
#include <signal.h>
#endif

/**
 * @class MyProcess
 * @brief handler class for capturing non-redirected processes termination
 */
class ExtToolsMyProcess : public wxProcess
{
public:
    ExtToolsMyProcess() {}
    virtual ~ExtToolsMyProcess() {}
    void OnTerminate(int pid, int status)
    {
        ToolsTaskManager::Instance()->ProcessTerminated(pid);
        delete this;
    }
};

ToolsTaskManager* ToolsTaskManager::gInstance = 0;

ToolsTaskManager::ToolsTaskManager()
{
    Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &ToolsTaskManager::OnProcessOutput, this);
    Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &ToolsTaskManager::OnProcessEnd, this);
}

ToolsTaskManager::~ToolsTaskManager()
{
    Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &ToolsTaskManager::OnProcessOutput, this);
    Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &ToolsTaskManager::OnProcessEnd, this);
}

ToolsTaskManager* ToolsTaskManager::Instance()
{
    if(gInstance == 0) 
    {
        gInstance = new ToolsTaskManager();
    }
    return gInstance;
}

void ToolsTaskManager::Release()
{
    if(gInstance) {
        delete gInstance;
    }
    gInstance = 0;
}

void ToolsTaskManager::OnProcessEnd(gsgs::ProcessEvent& event)
{
    GetManager()->AppendOutputTabText(kOutputTab_Output, event.GetOutput());
    GetManager()->AppendOutputTabText(kOutputTab_Output, "\n");

    // delete the process
    IProcess* proc = event.GetProcess();
    ProcessTerminated(proc->GetPid());
    delete proc;

    // Notify codelite to test for any modified bufferes
    gsgs_Event().PostReloadExternallyModifiedEvent();
}

void ToolsTaskManager::OnProcessOutput(gsgs::ProcessEvent& event)
{
    GetManager()->AppendOutputTabText(kOutputTab_Output, event.GetOutput());
}

void ToolsTaskManager::StartTool(const ToolInfo& ti, const wxString& filename)
{
    wxString command, working_dir;
    command << ti.GetPath();
    //::WrapWithQuotes(command);
    if(!filename.IsEmpty()) {
        // If an input file was given, append it to the command
        wxString fileName = filename;
        ::WrapWithQuotes(fileName);
        command << " " << fileName;
    }
    working_dir = ti.GetWd();
    command = MacroManager::Instance()->Expand(
        command,
        GetManager(),
        (GetManager()->GetWorkspace() ? GetManager()->GetWorkspace()->GetActiveProjectName() : ""));
    working_dir = MacroManager::Instance()->Expand(
        working_dir,
        GetManager(),
        (GetManager()->GetWorkspace() ? GetManager()->GetWorkspace()->GetActiveProjectName() : ""));

    wxString projectName;
    wxString configName;
    if(gsgs_CppWorker().IsOpen()) {
        projectName = gsgs_CppWorker().GetActiveProjectName();
        BuildConfigPtr bldConf = gsgs_CppWorker().GetProjBuildConf(projectName, wxEmptyString);
        if(bldConf) {
            configName = bldConf->GetName();
        }
    }

    EnvSetter envGuard(GetManager()->GetEnv(), NULL, projectName, configName);
    ::WrapInShell(command);
    
    gsgsDEBUG() << "Running command:" << command << clEndl;
    
    int pid = wxNOT_FOUND;
    if(ti.GetCaptureOutput()) 
    {
        IProcess* proc = ::CreateAsyncProcess(this, command, IProcessCreateConsole, working_dir);
        if(!proc) {
            ::wxMessageBox(_("Failed to launch tool\n'") + command + "'",
                           "gsgs",
                           wxICON_ERROR | wxOK | wxCENTER,
                           gsgs_Event().TopFrame());
            return;
        }

        pid = proc->GetPid();

    } else {
        pid = ::wxExecute(command, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER, new ExtToolsMyProcess());
    }
    
    if(pid > 0) {
        ExternalToolItemData item(command, pid);
        m_tools.insert(std::make_pair(pid, item));
    }
}

void ToolsTaskManager::ProcessTerminated(int pid)
{
    if(m_tools.find(pid) != m_tools.end()) {
        m_tools.erase(pid);
    }
}

void ToolsTaskManager::StopAll()
{
    std::for_each(m_tools.begin(), m_tools.end(), [&](const std::pair<int, ExternalToolItemData> &p) 
    {
        ::wxKill(p.second.m_pid, wxSIGKILL, NULL, wxKILL_CHILDREN);
    });
}

void ToolsTaskManager::Stop(int pid)
{
    if(m_tools.find(pid) != m_tools.end()) 
    {
        ::wxKill(pid, wxSIGKILL, NULL, wxKILL_CHILDREN);
    }
}

ExternalToolItemData::Map_t& ToolsTaskManager::GetTools()
{
#ifdef __WXOSX__
    // Check that the processes are still alive before we continue
    ExternalToolItemData::Map_t tools;
    std::for_each(m_tools.begin(), m_tools.end(), [&](const std::pair<int, ExternalToolItemData> &p) {
        if(kill(p.first, 0) == 0) {
            // alive
            tools.insert(p);
        }
    });
    m_tools.swap(tools);
#endif
    return m_tools;
}
