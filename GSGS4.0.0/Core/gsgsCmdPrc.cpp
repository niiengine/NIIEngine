#include "gsgsCmdPrc.h"
#include "process_impl.h"

namespace gsgs
{
    //-----------------------------------------------------------------------------------
    wxDEFINE_EVENT(wxEVT_COMMAND_PROCESSOR_ENDED, gsgs::Event);
    wxDEFINE_EVENT(wxEVT_COMMAND_PROCESSOR_OUTPUT, gsgs::Event);
    //-----------------------------------------------------------------------------------
    CmdPrc::CmdPrc(const wxString& command, const wxString& wd, size_t processFlags)
        : m_next(NULL)
        , m_prev(NULL)
        , m_process(NULL)
        , m_command(command)
        , m_workingDirectory(wd)
        , m_processFlags(processFlags)
        , m_postExecCallback(NULL)
        , m_obj(NULL)
    {
        Bind(wxEVT_ASYNC_PROCESS_OUTPUT, &CmdPrc::OnProcessOutput, this);
        Bind(wxEVT_ASYNC_PROCESS_TERMINATED, &CmdPrc::OnProcessTerminated, this);
        
    }
    //-----------------------------------------------------------------------------------
    CmdPrc::~CmdPrc() 
    { 
        wxDELETE(m_process); 
    }
    //-----------------------------------------------------------------------------------
    void CmdPrc::ExecuteCommand()
    {
        wxString message;
        message << _("Executing: ") << m_command << " [ wd: " << m_workingDirectory << " ]";

        gsgs::Event eventStart(wxEVT_COMMAND_PROCESSOR_OUTPUT);
        eventStart.SetString(message);
        GetFirst()->ProcessEvent(eventStart);
        
        m_output.Clear();
        m_process = ::CreateAsyncProcess(this, m_command, m_processFlags, m_workingDirectory);
        if(!m_process) {
            gsgs::Event eventEnd(wxEVT_COMMAND_PROCESSOR_ENDED);
            eventEnd.SetString(wxString::Format(_("Failed to execute command: %s"), m_command));
            GetFirst()->ProcessEvent(eventEnd);
            DeleteChain();
        }
        m_process->SetHardKill(true);
    }
    //-----------------------------------------------------------------------------------
    void CmdPrc::OnProcessOutput(gsgs::ProcessEvent& event)
    {
        gsgs::Event eventStart(wxEVT_COMMAND_PROCESSOR_OUTPUT);
        m_output << event.GetOutput();
        eventStart.SetString(event.GetOutput());
        GetFirst()->ProcessEvent(eventStart);
        if(eventStart.GetString() != event.GetOutput()) {
            // user provided some input, write it to the running process
            m_process->WriteToConsole(eventStart.GetString());
        }
    }
    //-----------------------------------------------------------------------------------
    void CmdPrc::OnProcessTerminated(gsgs::ProcessEvent& event)
    {
        if(m_obj && m_postExecCallback) {
            // Call the user callback, if the user returns false
            // stop the processor
            if(!(m_obj->*m_postExecCallback)(this)) {
                gsgs::Event eventEnd(wxEVT_COMMAND_PROCESSOR_ENDED);
                GetFirst()->ProcessEvent(eventEnd);
                DeleteChain();
                return;
            }
        }

        if(m_next) {
            wxDELETE(m_process);
            // more commands, don't report an 'END' event
            m_next->ExecuteCommand();

        } else {
            // no more commands to execute, delete the entire chain
            gsgs::Event eventEnd(wxEVT_COMMAND_PROCESSOR_ENDED);
            GetFirst()->ProcessEvent(eventEnd);
            DeleteChain();
        }
    }
    //-----------------------------------------------------------------------------------
    CmdPrc* CmdPrc::Link(CmdPrc* next)
    {
        this->m_next = next;
        if(m_next) {
            m_next->m_prev = this;
        }
        return next;
    }
    //-----------------------------------------------------------------------------------
    void CmdPrc::DeleteChain()
    {
        // Move to the first one in the list
        CmdPrc* first = GetFirst();

        // delete
        while(first) {
            CmdPrc* next = first->m_next;
            wxDELETE(first);
            first = next;
        }
    }
    //-----------------------------------------------------------------------------------
    CmdPrc* CmdPrc::GetFirst()
    {
        CmdPrc* first = this;
        while(first->m_prev) {
            first = first->m_prev;
        }
        return first;
    }
    //-----------------------------------------------------------------------------------
    void CmdPrc::Terminate()
    {
        CmdPrc* first = GetFirst();
        while(first) 
        {
            if(first->m_process) {
                first->m_process->Terminate();
                break;
            }
            first = first->m_next;
        }
    }
    //-----------------------------------------------------------------------------------
    CmdPrc* CmdPrc::GetActiveProcess()
    {
        CmdPrc* first = GetFirst();
        while(first) {
            if(first->m_process) {
                return first;
            }
            first = first->m_next;
        }
        return NULL;
    }
    //-----------------------------------------------------------------------------------
}