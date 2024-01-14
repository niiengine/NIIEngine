#include "TerminalEmulator.h"
#include "gsgsCommon.h"
#include "gsgsLogManager.h"
#include "process_impl.h"
#include "gsgsConsole.h"

#ifndef __WXMSW__
#include <signal.h>
#endif

namespace gsgs
{
#if wxUSE_GUI

    TerminalEmulatorUI::TerminalEmulatorUI(wxWindow* parent)
        : TerminalEmulatorUIBase(parent)
        , m_terminal(NULL)
    {
    }

    TerminalEmulatorUI::~TerminalEmulatorUI() {}

    void TerminalEmulatorUI::OnSendCommand(wxCommandEvent& event) {}

    void TerminalEmulatorUI::SetTerminal(TerminalEmulator* terminal)
    {
        if(m_terminal) {
            DoUnBindTerminal(m_terminal);
        }
        this->m_terminal = terminal;
        if(m_terminal) {
            DoBindTerminal(m_terminal);
        }
    }

    void TerminalEmulatorUI::OnProcessExit(gsgs::Event& e)
    {
        e.Skip();
        if(m_terminal) {
            DoUnBindTerminal(m_terminal);
            m_terminal = NULL;
        }
    }

    void TerminalEmulatorUI::OnProcessOutput(gsgs::Event& e)
    {
        e.Skip();
        m_stc->SetReadOnly(false);
        m_stc->AppendText(e.GetString());
        m_stc->SetReadOnly(true);
        int lastPos = m_stc->GetLastPosition();
        m_stc->SetCurrentPos(lastPos);
        m_stc->SetSelectionStart(lastPos);
        m_stc->SetSelectionEnd(lastPos);
        m_stc->ScrollToEnd();
    }

    void TerminalEmulatorUI::DoBindTerminal(TerminalEmulator* terminal)
    {
        if(terminal) {
            terminal->Bind(wxEVT_TERMINAL_COMMAND_EXIT, &TerminalEmulatorUI::OnProcessExit, this);
            terminal->Bind(wxEVT_TERMINAL_COMMAND_OUTPUT, &TerminalEmulatorUI::OnProcessOutput, this);
        }
    }

    void TerminalEmulatorUI::DoUnBindTerminal(TerminalEmulator* terminal)
    {
        if(terminal) {
            terminal->Unbind(wxEVT_TERMINAL_COMMAND_EXIT, &TerminalEmulatorUI::OnProcessExit, this);
            terminal->Unbind(wxEVT_TERMINAL_COMMAND_OUTPUT, &TerminalEmulatorUI::OnProcessOutput, this);
        }
    }

    void TerminalEmulatorUI::Clear()
    {
        m_textCtrl->ChangeValue("");
        m_stc->SetReadOnly(false);
        m_stc->ClearAll();
        m_stc->SetReadOnly(true);
    }

    TerminalEmulatorFrame::TerminalEmulatorFrame(wxWindow* parent)
        : TerminalEmulatorFrameBase(parent)
    {
        m_terminal = new TerminalEmulatorUI(this);
        GetSizer()->Add(m_terminal, 1, wxEXPAND|wxALL);
        
    }

    TerminalEmulatorFrame::~TerminalEmulatorFrame()
    {
    }
#endif

    wxDEFINE_EVENT(wxEVT_TERMINAL_COMMAND_EXIT, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_TERMINAL_COMMAND_OUTPUT, gsgs::Event);

    class MyProcess : public wxProcess
    {
    public:
        TerminalEmulator* m_parent;

    public:
        MyProcess(TerminalEmulator* parent)
            : wxProcess(parent)
            , m_parent(parent)
        {
            if(m_parent) { m_parent->m_myProcesses.push_back(this); }
        }

        virtual ~MyProcess() { m_parent = NULL; }
        void OnTerminate(int pid, int status)
        {
            if(m_parent) {
                gsgs::Event terminateEvent(wxEVT_TERMINAL_COMMAND_EXIT);
                m_parent->AddPendingEvent(terminateEvent);
                m_parent->m_pid = wxNOT_FOUND;

                std::list<wxProcess*>::iterator iter =
                    std::find_if(m_parent->m_myProcesses.begin(), m_parent->m_myProcesses.end(),
                                 [&](wxProcess* proc) { return proc == this; });
                if(iter != m_parent->m_myProcesses.end()) { m_parent->m_myProcesses.erase(iter); }
                delete this;
            }
        }
    };

    TerminalEmulator::TerminalEmulator()
        : m_process(NULL)
        , m_pid(wxNOT_FOUND)
    {
        Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &TerminalEmulator::OnProcessOutput, this);
        Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &TerminalEmulator::OnProcessTerminated, this);
    }

    TerminalEmulator::~TerminalEmulator()
    {
        Unbind(wxEVT_ASYNC_PROCESS_OUTPUT, &TerminalEmulator::OnProcessOutput, this);
        Unbind(wxEVT_ASYNC_PROCESS_TERMINATED, &TerminalEmulator::OnProcessTerminated, this);
        std::for_each(m_myProcesses.begin(), m_myProcesses.end(), [&](wxProcess* proc) {
            MyProcess* myproc = dynamic_cast<MyProcess*>(proc);
            myproc->m_parent = NULL;
        });
    }

    bool TerminalEmulator::ExecuteConsole(const wxString& command, bool waitOnExit, const wxString& command_args,
                                          const wxString& workingDirectory, const wxString& title)
    {
        wxUnusedVar(title);
        gsgs::ConsoleBase::Ptr_t console = gsgs::ConsoleBase::GetTerminal();
        console->SetCommand(command, command_args);
        console->SetWaitWhenDone(waitOnExit);
        console->SetWorkingDirectory(workingDirectory);
        console->SetCallback(new MyProcess(this));

        wxString strTitle = title;
        if(strTitle.IsEmpty()) {
            strTitle << "'" << command << "'";
        } else {
            strTitle.Prepend("'").Append("'");
        }
        bool res = console->Start();
        m_pid = console->GetPid();
        return res;
    }

    void TerminalEmulator::OnProcessTerminated(gsgs::ProcessEvent& event)
    {
        // Process terminated
        wxDELETE(m_process);
        m_pid = wxNOT_FOUND;
        // Notify that the terminal has terminated
        gsgs::Event terminateEvent(wxEVT_TERMINAL_COMMAND_EXIT);
        AddPendingEvent(terminateEvent);
    }

    void TerminalEmulator::Terminate()
    {
        if(IsRunning()) {
            if(m_process) { m_process->Terminate(); }
            if(m_pid != wxNOT_FOUND) {
                wxKill(m_pid, wxSIGKILL, NULL, wxKILL_CHILDREN);
                m_pid = wxNOT_FOUND;
            }
        }
    }

    bool TerminalEmulator::IsRunning() const { return (m_process != NULL) || (m_pid != wxNOT_FOUND); }

    void TerminalEmulator::OnProcessOutput(gsgs::ProcessEvent& event)
    {
        gsgs::Event evtOutput(wxEVT_TERMINAL_COMMAND_OUTPUT);
        evtOutput.SetString(event.GetOutput());
        AddPendingEvent(evtOutput);
    }

    bool TerminalEmulator::ExecuteNoConsole(const wxString& commandToRun, const wxString& workingDirectory)
    {
        if(m_process) {
            // another process is running
            return false;
        }

        wxString command;
    #ifdef __WXMSW__
        wxString shell = wxGetenv("COMSPEC");
        if(shell.IsEmpty()) { shell = "CMD"; }

        command << shell << wxT(" /c \"");
        command << commandToRun << wxT("\"");

    #else
        wxString tmpCmd = commandToRun;
        command << "/bin/sh -c '";
        // escape any single quoutes
        tmpCmd.Replace("'", "\\'");
        command << tmpCmd << "'";
    #endif
        gsgsLogMessage("TerminalEmulator::ExecuteNoConsole: " + command);
        m_process = ::CreateAsyncProcess(this, command, IProcessCreateWithHiddenConsole, workingDirectory);
        return m_process != NULL;
    }
}