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

#ifndef gsgsCONSOLEBASE_H
#define gsgsCONSOLEBASE_H

#include "gsgsPreInclude.h"
#include "gsgsEvent.h"
#include <wx/process.h>

namespace gsgs
{
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TERMINAL_EXIT, gsgs::ProcessEvent);

    class _gsgsAPI ConsoleEnvironment
    {
    public:
        ConsoleEnvironment(const gsgsStringMap& env);
        ConsoleEnvironment();
        ~ConsoleEnvironment();

        void Add(const wxString& name, const wxString& value);
        void Apply();
        void UnApply();
    private:
        gsgsStringMap m_environment;
        gsgsStringMap m_oldEnvironment;
    };

    class _gsgsAPI ConsoleBase
    {
    public:
        typedef wxSharedPtr<ConsoleBase> Ptr_t;

    protected:
        gsgsStringMap m_environment;
        wxString m_workingDirectory;
        wxString m_command;
        wxString m_commandArgs;
        wxString m_tty;
        wxString m_realPts;
        long m_pid = wxNOT_FOUND;
        bool m_waitWhenDone = false;
        bool m_autoTerminate = false;
        int m_execExtraFlags = 0;
        bool m_terminalNeeded = true;
        wxProcess* m_callback = nullptr;
        wxEvtHandler* m_sink = nullptr;
        wxString m_callbackUID;

    protected:
        /**
         * @brief create an environment list to be used before we execute our terminal
         */
        wxString GetEnvironmentPrefix() const;

        wxString WrapWithQuotesIfNeeded(const wxString& s) const;
        wxString EscapeString(const wxString& str, const wxString& c = "\"") const;
        virtual bool StartProcess(const wxString& command);

    public:
        ConsoleBase();
        virtual ~ConsoleBase();

        /**
         * @brief when sink is provided, the terminal will send event when its done
         * The event sent is wxEVT_TERMINAL_EXIT of type gsgs::ProcessEvent
         * @param sink
         * @param if provided the uid is sent back along with the termination event
         */
        void SetSink(wxEvtHandler* sink, const wxString& uid = "")
        {
            this->m_sink = sink;
            this->m_callbackUID = uid;
        }

        /**
         * @brief split command line arguments taking double quotes and escaping into account
         */
        static wxArrayString SplitArguments(const wxString& args);

        /**
         * @brief add an environment variable to be applied before we start the terminal
         */
        void AddEnvVariable(const wxString& name, const wxString& value);

        /**
         * @brief start terminal with a given command and an optional working directory
         */
        virtual bool Start() = 0;

        /**
         * @brief start terminal for debugger (i.e. launch an empty terminal so we can obtain its tty and return it
         */
        virtual bool StartForDebugger() = 0;

        /**
         * @brief prepare the execution command
         */
        virtual wxString PrepareCommand() = 0;

        /**
         * @brief return the best terminal for the OS. Pass an empty string to return the default temrinal for the OS
         */
        static ConsoleBase::Ptr_t GetTerminal();

        /**
         * @brief return the default name for the OS
         */
        static wxString GetSelectedTerminalName();

        /**
         * @brief return list of known terminals
         */
        static wxArrayString GetAvailaleTerminals();

        // Setters/Getters
        void SetCommand(const wxString& command, const wxString& args)
        {
            this->m_command = command;
            this->m_commandArgs = args;
        }
        void SetEnvironment(const gsgsStringMap& environment) { this->m_environment = environment; }
        void SetPid(long pid) { this->m_pid = pid; }
        void SetRealPts(const wxString& realPts) { this->m_realPts = realPts; }
        void SetTty(const wxString& tty) { this->m_tty = tty; }
        void SetWaitWhenDone(bool waitWhenDone) { this->m_waitWhenDone = waitWhenDone; }
        void SetWorkingDirectory(const wxString& workingDirectory) { this->m_workingDirectory = workingDirectory; }
        const wxString& GetCommand() const { return m_command; }
        const wxString& GetCommandArgs() const { return m_commandArgs; }
        const gsgsStringMap& GetEnvironment() const { return m_environment; }
        long GetPid() const { return m_pid; }
        const wxString& GetRealPts() const { return m_realPts; }
        const wxString& GetTty() const { return m_tty; }
        bool IsWaitWhenDone() const { return m_waitWhenDone; }
        const wxString& GetWorkingDirectory() const { return m_workingDirectory; }
        void SetExecExtraFlags(int execExtraFlags) { this->m_execExtraFlags = execExtraFlags; }
        int GetExecExtraFlags() const { return m_execExtraFlags; }
        void SetAutoTerminate(bool autoTerminate) { this->m_autoTerminate = autoTerminate; }
        void SetTerminalNeeded(bool terminalNeeded) { this->m_terminalNeeded = terminalNeeded; }
        bool IsAutoTerminate() const { return m_autoTerminate; }
        bool IsTerminalNeeded() const { return m_terminalNeeded; }
        void SetCallback(wxProcess* callback)
        {
            wxDELETE(m_callback);
            this->m_callback = callback;
        }
    };
}
#endif
