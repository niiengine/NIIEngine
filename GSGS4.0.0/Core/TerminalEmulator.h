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

#ifndef TERMINALEMULATOR_H
#define TERMINALEMULATOR_H

#include "gsgsPreInclude.h"
#include "gsgsProcess.h"
#include "gsgsEvent.h"

#if wxUSE_GUI
#include "TerminalEmulatorUIBase.h"
#endif

namespace gsgs
{
    class MyProcess;
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TERMINAL_COMMAND_EXIT, gsgs::Event);
    wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_TERMINAL_COMMAND_OUTPUT, gsgs::Event);

    class _gsgsAPI TerminalEmulator : public wxEvtHandler
    {
        IProcess* m_process;
        long m_pid;
        friend class MyProcess;
        std::list<wxProcess*> m_myProcesses;

    protected:
        void OnProcessTerminated(gsgs::ProcessEvent& event);
        void OnProcessOutput(gsgs::ProcessEvent& event);

    public:
        TerminalEmulator();
        virtual ~TerminalEmulator();

        /**
         * @brief terminate the running process
         */
        void Terminate();

        /**
         * @brief is the process currently running?
         */
        bool IsRunning() const;

        /**
         * @brief Execute a command in the OS terminal.
         * @param command the command to execute
         * @param waitOnExit when set to true, pause the terminal with the message
         * @param command_args arguments to be appended to the command
         * @param workingDirectory working directory for the new process
         * "Hit any key to continue"
         * @param title set the terminal title. If an empty string is provided, use the command as the title
         */
        bool ExecuteConsole(const wxString& command, bool waitOnExit = false, const wxString& command_args = "",
                            const wxString& workingDirectory = "", const wxString& title = "");
        /**
         * @brief execute a command without attaching console
         */
        bool ExecuteNoConsole(const wxString& command, const wxString& workingDirectory);
    };

#if wxUSE_GUI
    class _gsgsAPI TerminalEmulatorUI : public TerminalEmulatorUIBase
    {
        TerminalEmulator* m_terminal;
    private:
        void DoBindTerminal(TerminalEmulator* terminal);
        void DoUnBindTerminal(TerminalEmulator* terminal);

    public:
        TerminalEmulatorUI(wxWindow* parent);
        virtual ~TerminalEmulatorUI();

        void Clear();
        void SetTerminal(TerminalEmulator* terminal);
        TerminalEmulator* GetTerminal() { return m_terminal; }
        wxStyledTextCtrl* GetTerminalOutputWindow() { return m_stc; }

    protected:
        virtual void OnSendCommand(wxCommandEvent& event);
        void OnProcessExit(gsgs::Event& e);
        void OnProcessOutput(gsgs::Event& e);
    };

    class _gsgsAPI TerminalEmulatorFrame : public TerminalEmulatorFrameBase
    {
        TerminalEmulatorUI* m_terminal;
    public:
        TerminalEmulatorFrame(wxWindow* parent);
        virtual ~TerminalEmulatorFrame();
        TerminalEmulatorUI* GetTerminalUI() { return m_terminal; }
    };
#endif
}
#endif
