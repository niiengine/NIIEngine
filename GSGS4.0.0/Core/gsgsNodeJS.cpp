#include "gsgsNodeJS.h"
#include "NodeJSLocator.h"
#include "gsgsLogManager.h"
#include "gsgsProcess.h"
#include "process_impl.h"
#include "gsgsConsole.h"
#include "ieditor.h"
#include "globals.h"
#include "imanager.h"

namespace gsgs
{
    wxDEFINE_EVENT(wxEVT_NODE_COMMAND_TERMINATED, gsgs::ProcessEvent);

    NodeJS::NodeJS() {}

    NodeJS::~NodeJS() {}

    bool NodeJS::Initialise(const wxArrayString& hints)
    {
        if(!IsInitialised()) {
            // bind the events once
            BindEvents();
        }

        NodeJSLocator locator;
        locator.Locate(hints);
        m_node = locator.GetNodejs();
        m_npm = locator.GetNpm();
        m_initialised = m_node.FileExists() && m_npm.FileExists();
        return m_initialised;
    }

    bool NodeJS::NpmInstall(const wxString& package, const wxString& workingDirectory, const wxString& args,
                              wxEvtHandler* sink, const wxString& uid)
    {
        if(!IsInitialised()) { return false; }

        // Create the working directory if not exists
        wxFileName wd(workingDirectory, "");
        wd.Mkdir(wxS_DEFAULT, wxPATH_MKDIR_FULL);

        wxString _args;
        _args.Prepend("install ");
        _args << package;
        if(!args.IsEmpty()) {
            _args << " " << args; // --save
        }

        ConsoleBase::Ptr_t console = ConsoleBase::GetTerminal();
        console->SetWorkingDirectory(wd.GetPath());
        console->SetCommand(GetNpm().GetFullPath(), _args);
        console->SetWaitWhenDone(true);
        console->SetTerminalNeeded(true);
        console->SetSink(sink, uid);
        return console->Start();
    }

    void NodeJS::OnProcessOutput(gsgs::ProcessEvent& event)
    {
        IProcess* process = event.GetProcess();
        if(m_processes.count(process)) {
            ProcessData& d = m_processes[process];
            d.GetOutput() << event.GetOutput();
        }
    }

    void NodeJS::OnProcessTerminated(gsgs::ProcessEvent& event)
    {
        IProcess* process = event.GetProcess();
        if(m_processes.count(process)) {
            const ProcessData& d = m_processes[process];
            // Handle lint
            if(!d.GetOutput().IsEmpty() && d.GetUid() == "lint") { ProcessLintOuput(d.GetFilename(), d.GetOutput()); }
            if(d.GetSink()) {
                gsgs::ProcessEvent evt(wxEVT_NODE_COMMAND_TERMINATED);
                evt.SetOutput(d.GetOutput());
                evt.SetString(d.GetUid()); // pass the unique ID
                d.GetSink()->AddPendingEvent(evt);
            }
            m_processes.erase(process);
        }
        wxDELETE(process);
    }

    NodeJS& NodeJS::Get()
    {
        static NodeJS nodejs;
        return nodejs;
    }

    void NodeJS::Shutdown()
    {
        m_initialised = false;
        m_node.Clear();
        m_npm.Clear();

        // Unbind before we kill the process
        UnBindEvents();

        std::for_each(m_processes.begin(), m_processes.end(),
                      [&](const std::unordered_map<IProcess*, ProcessData>::value_type& vt) {
                          IProcess* p = vt.first;
                          // Terminate the process
                          p->Terminate();
                          wxDELETE(p);
                      });
        m_processes.clear();
    }

    void NodeJS::BindEvents()
    {
        Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &NodeJS::OnProcessOutput, this);
        Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeJS::OnProcessTerminated, this);
    }

    void NodeJS::UnBindEvents()
    {
        Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &NodeJS::OnProcessOutput, this);
        Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &NodeJS::OnProcessTerminated, this);
    }

    bool NodeJS::NpmInit(const wxString& workingDirectory, wxEvtHandler* sink)
    {
        if(!IsInitialised()) { return false; }

        // Create the working directory if not exists
        wxFileName wd(workingDirectory, "");
        wd.Mkdir(wxS_DEFAULT, wxPATH_MKDIR_FULL);

        ConsoleBase::Ptr_t console = ConsoleBase::GetTerminal();
        console->SetWorkingDirectory(wd.GetPath());
        console->SetCommand(GetNpm().GetFullPath(), "init");
        console->SetWaitWhenDone(true);
        console->SetTerminalNeeded(true);
        console->SetSink(sink);
        return console->Start();
    }

    void NodeJS::LintFile(const wxFileName& filename)
    {
        if(!IsInitialised()) { return; }
        wxString wd = filename.GetPath();

        wxString command;
        command << GetNode().GetFullPath();
        ::WrapWithQuotes(command);

        command << " -c " << filename.GetFullName();
        IProcess* process = ::CreateAsyncProcess(this, command, IProcessCreateDefault, wd);
        if(process) {
            ProcessData d;
            d.SetFilename(filename);
            d.SetUid("lint");
            m_processes.insert({ process, d });
        }
    }

    void NodeJS::ProcessLintOuput(const wxFileName& fn, const wxString& output)
    {
        // the first line in the output is "file:line\n"
        wxString where = output.BeforeFirst('\n');
        wxString line = where.AfterLast(':');
        wxString file = where.BeforeLast(':');
        wxString errorMessage = output.AfterFirst('\n');

        line.Trim().Trim(false);
        file.Trim().Trim(false);

        // Use the code font for this error message
        errorMessage.Prepend("<code>").Append("</code>");

        long nLineNumber = -1;
        line.ToCLong(&nLineNumber);

        // Mark the editor with error marker
        IEditor* editor = GetManager()->FindEditor(fn.GetFullPath());
        if(editor && (nLineNumber != wxNOT_FOUND)) { editor->SetErrorMarker(nLineNumber - 1, errorMessage); }
    }

    bool NodeJS::NpmSilentInstall(const wxString& package, const wxString& workingDirectory, const wxString& args,
                                    wxEvtHandler* sink, const wxString& uid)
    {
        if(!IsInitialised()) { return false; }
        wxString command;
        command << GetNpm().GetFullPath();
        ::WrapWithQuotes(command);

        command << " install " << package << " --silent --quiet " << args;
        IProcess* process = ::CreateAsyncProcess(this, command, IProcessCreateDefault, workingDirectory);
        if(process) {
            ProcessData d;
            d.SetUid(uid);
            d.SetSink(sink);
            m_processes.insert({ process, d });
        }
        return (process != nullptr);
    }

    wxProcess* NodeJS::RunScript(const wxArrayString& argv, const wxString& workingDirectory, size_t execFlags)
    {
        if(!IsInitialised() || argv.IsEmpty()) { return nullptr; }
        wxFileName scriptPath(argv.Item(0));
        if(!scriptPath.FileExists()) {
            gsgsERROR() << "NodeJS: cant run script:" << scriptPath << ". No such file";
            return nullptr;
        }

        wxString command;
        for(const wxString& arg : argv) {
            // Build the command line
            wxString tmp = std::move(arg);
            ::WrapWithQuotes(tmp);
            command << tmp << " ";
        }

        wxProcess* process = new wxProcess();
        process->Redirect();

        if(::wxExecute(command, execFlags, process) <= 0) {
            wxDELETE(process);
            return nullptr;
        }
        return process;
    }
}