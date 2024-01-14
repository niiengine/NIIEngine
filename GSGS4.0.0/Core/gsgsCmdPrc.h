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
#ifndef GSGS_CMDPRC_H_
#define GSGS_CMDPRC_H_

#include "gsgsPreInclude.h"
#include "gsgsProcess.h"
#include "gsgsEvent.h"

namespace gsgs
{
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_COMMAND_PROCESSOR_ENDED, gsgs::Event);

    // A process printed output to stdout/err (the process output is accessible via event.GetString())
    // The caller may handle this event, incase the process is waiting for input the caller
    // may send its input via event.SetString()
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_COMMAND_PROCESSOR_OUTPUT, gsgs::Event);

    typedef bool (wxEvtHandler::*clCommandProcessorFunc)(const gsgs::CmdPrc * processor);
    
    class _gsgsAPI CmdPrc : public wxEvtHandler
    {
    public:
        CmdPrc(const wxString& command, const wxString & wd, size_t processFlags = IProcessCreateDefault);
        virtual ~CmdPrc();

        void ExecuteCommand();

        inline const wxString& GetCommand() const               { return m_command; }
        inline void SetCommand(const wxString& command)         { this->m_command = command; }

        inline const wxString& GetOutput() const                { return m_output; }
        inline const wxString& GetWorkingDirectory() const      { return m_workingDirectory; }

        inline void SetWorkingDirectory(const wxString& dir)    { this->m_workingDirectory = dir; }
        inline void SetPrev(CmdPrc* prev)                       { this->m_prev = prev; }
        inline CmdPrc* GetPrev()                                { return m_prev; }
        inline CmdPrc* GetNext()                                { return m_next; }
        inline void SetNext(CmdPrc* next)                       { this->m_next = next; }

        inline IProcess* GetProcess()                           { return m_process; }
        /**
         * @brief set a callback to be called by the processor when the current command execution
         * is completed. The callback signature is:
         * bool Class::CallbackName(const CmdPrc* processor)
         * if the callback returns false, the execution of the rest of the chain is stopped
         */
        inline void SetPostExecCallback(wxEvtHandler * handler, clCommandProcessorFunc func)
        {
            m_obj = handler;
            m_postExecCallback = func;
        }

        /**
         * @brief link two command so they will be executed one after the other
         * @return a pointer to the next command ("next")
         */
        CmdPrc * Link(CmdPrc * next);

        /**
         * @brief terminate the process. If we are part of the chain, search the active process
         * within the chain and terminate it (this will cause the next process to start)
         */
        void Terminate();

        void OnProcessOutput(gsgs::ProcessEvent& event);
        void OnProcessTerminated(gsgs::ProcessEvent& event);
    protected:
        void DeleteChain();
        CmdPrc * GetFirst();
        CmdPrc * GetActiveProcess();
    protected:
        CmdPrc* m_next;
        CmdPrc* m_prev;
        IProcess* m_process;
        wxString m_command;
        wxString m_workingDirectory;
        wxString m_output;

        clCommandProcessorFunc m_postExecCallback;
        wxEvtHandler* m_obj;
        size_t m_processFlags;
    };
}
#endif
