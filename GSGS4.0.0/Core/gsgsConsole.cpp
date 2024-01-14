#include "gsgsConsole.h"
#include "gsgsConfig.h"
#include "gsgsLogManager.h"
#include "gsgsPathManager.h"
#include "gsgsFileUtil.h"
#include "gsgsProcess.h"
#include "gsgsCommon.h"

namespace gsgs
{
    wxDEFINE_EVENT(wxEVT_TERMINAL_EXIT, gsgs::ProcessEvent);

    class ConsoleProcess : public wxProcess
    {
    public:
        wxEvtHandler* m_sink = nullptr;
        wxString m_uid;

    public:
        ConsoleProcess(wxEvtHandler* sink, const wxString& uid)
            : m_sink(sink)
            , m_uid(uid)
        {
        }

        virtual ~ConsoleProcess() { m_sink = NULL; }
        void OnTerminate(int pid, int status)
        {
            gsgs::ProcessEvent terminateEvent(wxEVT_TERMINAL_EXIT);
            terminateEvent.SetString(m_uid);
            terminateEvent.SetInt(status); // pass the exit code
            m_sink->AddPendingEvent(terminateEvent);
            delete this;
        }
    };

    class ConsoleCMD : public ConsoleBase
    {
    public:
        ConsoleCMD()
        {
        }
        virtual ~ConsoleCMD()
        {
        }

    public:
        virtual bool Start()
        {
            DirSaver ds; // restore the directory upon exiting this function
            wxFileName fn(GetWorkingDirectory(), "");
            if(fn.DirExists()) { ::wxSetWorkingDirectory(fn.GetPath()); }
            return StartProcess(PrepareCommand());
        }
        virtual bool StartForDebugger()
        {
            return false; 
        }
        virtual wxString PrepareCommand()
        {
            // Build the command to execute
            wxString commandToExecute;
            if(IsTerminalNeeded()) { commandToExecute << "cmd"; }

            // For testing purposes
            wxArrayString args = SplitArguments(GetCommandArgs());
            wxUnusedVar(args);
            wxString command = WrapWithQuotesIfNeeded(GetCommand());
            if(!command.IsEmpty()) {
                if(IsTerminalNeeded()) {
                    commandToExecute << " /C ";

                    // We prepend 'call' to the execution to make sure that the execution is always returning 0
                    if(IsWaitWhenDone()) { commandToExecute << " call "; }
                }
                commandToExecute << command;
                if(!GetCommandArgs().IsEmpty()) { commandToExecute << " " << GetCommandArgs(); }
                if(IsTerminalNeeded() && IsWaitWhenDone()) { commandToExecute << " && pause"; }
            }
            return commandToExecute;
        }
    };

    class ConsoleBash : public ConsoleBase
    {
    protected:
        /**
         * @brief return execution script and return its path
         */
        wxFileName PrepareExecScript() const
        {
        // Create a script
        #ifdef __WXOSX__
            wxFileName scriptPath("/tmp", "gsgs-exec.sh");
        #else
            wxFileName scriptPath(gsgs_Path().GetUserDataDir(), "gsgs-exec.sh");
            scriptPath.AppendDir("tmp");
            scriptPath.AppendDir(::wxGetUserId());
            scriptPath.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        #endif

            if(!GetCommand().IsEmpty()) {
                wxString fileContent;
                fileContent << "#!/bin/bash\n";
                fileContent << "command=\"" << GetCommand() << "\"\n";
                wxString cdCommand;
        #ifdef __WXOSX__
                // on OSX, we need to set the working directory via the script
                if(!GetWorkingDirectory().IsEmpty()) {
                    wxString workingDir = GetWorkingDirectory();
                    if(workingDir.Contains(" ")) {
                        workingDir.Prepend("\"").Append("\"");
                    }
                    cdCommand << "cd " << workingDir << " && ";
                }
        #endif
                if(!GetCommandArgs().IsEmpty()) {
                    // Split the arguments line
                    wxArrayString arr = SplitArguments(GetCommandArgs());
                    // Create an array in the script
                    for(size_t i = 0; i < arr.size(); ++i) {
                        fileContent << "args[" << i << "]=\"" << arr[i] << "\"\n";
                    }
                    fileContent << cdCommand << "${command} \"${args[@]}\"\n";
                } else {
                    fileContent << cdCommand << "${command}\n";
                }
                if(IsWaitWhenDone()) {
                    fileContent << "echo Hit any key to continue...\nread";
                }
                FileUtil::WriteFileContent(scriptPath, fileContent);
            }
        #ifdef __WXOSX__
            // The script must have exec permissions
            system("chmod +x /tmp/gsgs-exec.sh");
        #endif
            return scriptPath;
        }

    public:
        ConsoleBash() {}
        virtual ~ConsoleBash() {}
    };

    class ConsoleGnomeTerminal : public ConsoleBash
    {
        wxString m_terminalCommand;
        wxString m_emptyTerminalCommand;

    protected:
        void SetTerminalCommand(const wxString& terminalCommand) { this->m_terminalCommand = terminalCommand; }
        const wxString& GetTerminalCommand() const { return m_terminalCommand; }

        void SetEmptyTerminalCommand(const wxString& emptyTerminalCommand)
        {
            this->m_emptyTerminalCommand = emptyTerminalCommand;
        }
        const wxString& GetEmptyTerminalCommand() const { return m_emptyTerminalCommand; }
        bool FindProcessByCommand(const wxString& name, wxString& tty, long& pid)
        {
            gsgsDEBUG() << "FindProcessByCommand is called";
            tty.Clear();
            pid = wxNOT_FOUND;

            // Run "ps -A -o pid,tty,command" to locate the terminal ID
            wxString psCommand;
            wxArrayString arrOutput;
            psCommand << "ps -A -o pid,tty,command";

            ProcUtils::SafeExecuteCommand(psCommand, arrOutput);

            for(size_t i = 0; i < arrOutput.GetCount(); ++i) {
                wxString curline = arrOutput.Item(i).Trim().Trim(false);
                wxArrayString tokens = ::wxStringTokenize(curline, " ", wxTOKEN_STRTOK);
                if(tokens.GetCount() < 3) { continue; }

                // replace tabs with spaces
                curline.Replace("\t", " ");

                // remove any duplicate spaces
                while(curline.Replace("  ", " ")) {}

                wxString tmp_pid = curline.BeforeFirst(' ');
                curline = curline.AfterFirst(' ');

                wxString tmp_tty = curline.BeforeFirst(' ');
                curline = curline.AfterFirst(' ');

                wxString command = curline; // the remainder
                command.Trim().Trim(false);

                if(command == name) {
                    // we got our match
                    tmp_tty = tmp_tty.AfterLast('/');
                    tmp_tty.Prepend("/dev/pts/");
                    tty = tmp_tty;
                    tmp_pid.Trim().Trim(false).ToCLong(&pid);
                    return true;
                }
            }
            return false;
        }

    public:
        ConsoleGnomeTerminal()
        {
            SetTerminalCommand("/usr/bin/gnome-terminal --working-directory=%WD% -e '%COMMAND%'");
            SetEmptyTerminalCommand("/usr/bin/gnome-terminal --working-directory=%WD%");
        }
        virtual ~ConsoleGnomeTerminal()
        {
        }
    public:
        virtual bool Start()
        {
            // Apply the environment variables before we launch the process
            ConsoleEnvironment env(GetEnvironment());
            env.Apply();
            return StartProcess(PrepareCommand());
        }
        virtual bool StartForDebugger()
        {
            // generate a random value to differntiate this instance of gsgs
            // from other instances

            time_t curtime = time(NULL);
            int randomSeed = (curtime % 947);
            wxString secondsToSleep;

            secondsToSleep << (85765 + randomSeed);

            wxString sleepCommand = "/bin/sleep";
            sleepCommand << " " << secondsToSleep;

            wxString homedir = wxGetHomeDir();
            if(homedir.Contains(" ")) { homedir.Prepend("\"").Append("\""); }
            wxString commandToExecute = GetTerminalCommand();
            commandToExecute.Replace("%WD%", homedir);
            commandToExecute.Replace("%COMMAND%", sleepCommand);
            ::wxExecute(commandToExecute);

            // Let it start ... (wait for it up to 5 seconds)
            for(size_t i = 0; i < 100; ++i) {
                if(FindProcessByCommand(sleepCommand, m_tty, m_pid)) {
                    // On GTK, redirection to TTY does not work with lldb
                    // as a workaround, we create a symlink with different name

                    // Keep the real tty
                    m_realPts = m_tty;

                    wxString symlinkName = m_tty;
                    symlinkName.Replace("/dev/pts/", "/tmp/pts");
                    wxString lnCommand;
                    lnCommand << "ln -sf " << m_tty << " " << symlinkName;
                    if(::system(lnCommand.mb_str(wxConvUTF8).data()) == 0) { m_tty.swap(symlinkName); }
                    break;
                }
                wxThread::Sleep(50);
            }
            return !m_tty.IsEmpty();
        }
        virtual wxString PrepareCommand()
        {
            wxString commandToExecute;
            bool hasCommand = !GetCommand().IsEmpty();
            commandToExecute = hasCommand ? GetTerminalCommand() : GetEmptyTerminalCommand();
            if(!IsTerminalNeeded()) { commandToExecute = "%COMMAND%"; }

            if(IsTerminalNeeded()) {
                // set the working directory
                wxString workingDirectory = WrapWithQuotesIfNeeded(GetWorkingDirectory());
                if(workingDirectory.IsEmpty()) { workingDirectory = "."; }
                commandToExecute.Replace("%WD%", workingDirectory);
            }

            if(hasCommand) {
                wxFileName scriptPath = PrepareExecScript();
                wxString rowCommand;
                rowCommand << "/bin/bash -f \"" << scriptPath.GetFullPath() << "\"";
                commandToExecute.Replace("%COMMAND%", rowCommand);
            }
            return commandToExecute;
        }
    };

    class ConsoleOSXTerminal : public ConsoleBash
    {
        wxString m_terminalApp;

    public:
        ConsoleOSXTerminal()
        {
            SetTerminalApp("Terminal");
        }
        virtual ~ConsoleOSXTerminal()
        {
        }

        void SetTerminalApp(const wxString& terminalApp) { this->m_terminalApp = terminalApp; }
        const wxString& GetTerminalApp() const { return m_terminalApp; }

    public:
        virtual bool Start()
        {
            wxString commandToExecute = PrepareCommand();
            int rc = system(commandToExecute.mb_str(wxConvUTF8).data());
            wxUnusedVar(rc);
            return true;
        }
        virtual bool StartForDebugger()
        {
            SetRealPts("");
            SetTty("");
            SetPid(wxNOT_FOUND);
            FileUtil::OSXOpenDebuggerTerminalAndGetTTY(GetWorkingDirectory(), GetTerminalApp(), m_tty, m_pid);
            SetRealPts(m_tty);
            return true;
        }
        virtual wxString PrepareCommand()
        {
            wxString commandToExecute;
            // osascript -e 'tell app "Terminal" to do script "echo hello"'
            wxFileName scriptPath = PrepareExecScript();

            if(IsTerminalNeeded()) {
                commandToExecute << "/usr/bin/open -n -a " << GetTerminalApp();
            }

            if(!GetCommand().IsEmpty()) {
                commandToExecute << " " << scriptPath.GetFullPath();
            }
            gsgsDEBUG() << commandToExecute;
            return commandToExecute;
        }
    };

    class ConsoleXfce4Terminal : public ConsoleGnomeTerminal
    {
    public:
        ConsoleXfce4Terminal()
        {
            SetTerminalCommand("/usr/bin/xfce4-terminal --working-directory=%WD% --execute %COMMAND%");
            SetEmptyTerminalCommand("/usr/bin/xfce4-terminal --working-directory=%WD%");
        }
        virtual ~ConsoleXfce4Terminal()
        {
            
        }
    };

    class ConsoleRXVTTerminal : public ConsoleGnomeTerminal
    {
    public:
        ConsoleRXVTTerminal()
        {
            SetTerminalCommand("/usr/bin/rxvt-unicode -cd %WD% -e /bin/bash -c '%COMMAND%'");
            SetEmptyTerminalCommand("/usr/bin/rxvt-unicode -cd %WD%");
        }
        virtual ~ConsoleRXVTTerminal()
        {
        }
    };

    class ConsoleQTerminal : public ConsoleGnomeTerminal
    {
    public:
        ConsoleQTerminal()
        {
            SetTerminalCommand("/usr/bin/qterminal -w %WD% -e %COMMAND%");
            SetEmptyTerminalCommand("/usr/bin/qterminal -w %WD%");
        }
        virtual ~ConsoleQTerminal()
        {
        }
    };

    class ConsoleMateTerminal : public ConsoleGnomeTerminal
    {
    public:
        ConsoleMateTerminal()
        {
            SetTerminalCommand("/usr/bin/mate-terminal --working-directory=%WD% -e '%COMMAND%'");
            SetEmptyTerminalCommand("/usr/bin/mate-terminal --working-directory=%WD%");
        }
        ~ConsoleMateTerminal()
        {
            
        }
    };

    class ConsoleLXTerminal : public ConsoleGnomeTerminal
    {
    public:
        ConsoleLXTerminal()
        {
            SetTerminalCommand("/usr/bin/lxterminal --working-directory=%WD% -e %COMMAND%");
            SetEmptyTerminalCommand("/usr/bin/lxterminal --working-directory=%WD%");
        }
        virtual ~ConsoleLXTerminal()
        {
        }
    };

    class ConsoleKonsole : public ConsoleGnomeTerminal
    {
    public:
        ConsoleKonsole()
        {
            SetTerminalCommand("/usr/bin/konsole --separate --workdir %WD% -e '%COMMAND%'");
            SetEmptyTerminalCommand("/usr/bin/konsole --separate --workdir %WD%");
        }
        virtual ~ConsoleKonsole()
        {
            
        }
    };

    class ConsoleGSGSTerminal : public ConsoleGnomeTerminal
    {
    public:
        ConsoleGSGSTerminal()
        {
            wxString cmd = GetBinary();
        #ifndef __WXOSX__
            WrapWithQuotesIfNeeded(cmd);
        #endif
            SetTerminalCommand(wxString() << cmd << " --working-directory=%WD% --file=%COMMANDFILE%");
            SetEmptyTerminalCommand(wxString() << cmd << " --working-directory=%WD%");
        }
        ~ConsoleGSGSTerminal()
        {
        }
        
        wxString PrepareCommand()
        {
            // Build the command to execute
            wxString commandToExecute;
            if(IsTerminalNeeded()) {
                wxFileName tmpfile(gsgs_Path().GetTempDir(), "gsgs-terminal.txt");
                bool hasCommand = !GetCommand().IsEmpty();
                commandToExecute = hasCommand ? GetTerminalCommand() : GetEmptyTerminalCommand();

                // For testing purposes
                wxString command = WrapWithQuotesIfNeeded(GetCommand());
                if(!command.IsEmpty()) {
        #ifdef __WXMSW__
                    command.Prepend("start /B /WAIT "); // start the application in the foreground
        #endif
                    if(!GetCommandArgs().IsEmpty()) {
                        wxString cmdArgs = GetCommandArgs();
                        command << " " << GetCommandArgs();
                    }

                    // Write the content of the command into a file
                    FileUtil::WriteFileContent(tmpfile, command);
                }

                wxString wd = GetWorkingDirectory();
                if(wd.IsEmpty() || !wxFileName::DirExists(wd)) { wd = WrapWithQuotesIfNeeded(wxGetCwd()); }
                if(IsWaitWhenDone()) { commandToExecute << " --wait "; }

                wxString commandFile = tmpfile.GetFullPath();
                commandFile = WrapWithQuotesIfNeeded(commandFile);

                commandToExecute.Replace("%COMMANDFILE%", commandFile);
                commandToExecute.Replace("%WD%", wd);

            } else {
                commandToExecute = WrapWithQuotesIfNeeded(GetCommand());
                if(!GetCommandArgs().IsEmpty()) { commandToExecute << " " << GetCommandArgs(); }
            }
            return commandToExecute;
        }
        bool StartForDebugger()
        {
        #ifdef __WXMSW__
            return false;
        #else
            // generate a random value to differntiate this instance of gsgs
            // from other instances

            time_t curtime = time(NULL);
            int randomSeed = (curtime % 947);
            wxString secondsToSleep;

            secondsToSleep << (85765 + randomSeed);

            wxString sleepCommand = "/bin/sleep";
            sleepCommand << " " << secondsToSleep;

            wxString homedir = wxGetHomeDir();
            if(homedir.Contains(" ")) { homedir.Prepend("\"").Append("\""); }
            wxString commandToExecute;
            commandToExecute << GetBinary();
        #ifndef __WXOSX__
            WrapWithQuotesIfNeeded(commandToExecute);
        #endif
            wxFileName tmpfile("/tmp", "gsgs-terminal.txt");
            tmpfile.SetFullName(wxString() << "gsgs-terminal." << secondsToSleep << ".txt");

            commandToExecute << " --print-tty=" << tmpfile.GetFullPath() << " --working-directory=" << homedir
                             << " --command=\"" << sleepCommand << "\"";
            m_pid = ::wxExecute(commandToExecute);

            // Let it start ... (wait for it up to 5 seconds)
            for(size_t i = 0; i < 100; ++i) {
                if(tmpfile.FileExists()) {
                    FileUtil::ReadFileContent(tmpfile.GetFullPath(), m_tty);
                    // On GTK, redirection to TTY does not work with lldb
                    // as a workaround, we create a symlink with different name

                    // Keep the real tty
                    m_realPts = m_tty;

                    wxString symlinkName = m_tty;
                    symlinkName.Replace("/dev/pts/", "/tmp/pts");
                    wxString lnCommand;
                    lnCommand << "ln -sf " << m_tty << " " << symlinkName;
                    if(::system(lnCommand.mb_str(wxConvUTF8).data()) == 0) { m_tty.swap(symlinkName); }
                    break;
                }
                wxThread::Sleep(50);
            }
        #ifdef __WXOSX__
            // on OSX, wxExecute return the PID of the open command
            // we want the sleep command PID (when we kill the 'sleep' the terminal will 
            // close itself)
            m_pid = wxNOT_FOUND;
            wxString psCommand;
            psCommand << "ps -A -o pid,command";
            wxString psOutput = ProcUtils::SafeExecuteCommand(psCommand);
            gsgsDEBUG1() << "ps command output:\n" << psOutput;
            wxArrayString lines = ::wxStringTokenize(psOutput, "\n", wxTOKEN_STRTOK);
            for(size_t u = 0; u < lines.GetCount(); ++u) {
                wxString l = lines.Item(u);
                l.Trim().Trim(false);
                if(l.Contains("sleep") && l.Contains(secondsToSleep) && !l.Contains("gsgs-terminal")) {
                    // we got a match
                    wxString ppidString = l.BeforeFirst(' ');
                    ppidString.ToCLong(&m_pid);
                    gsgsDEBUG() << "gsgs-terminal process ID is:" << m_pid;
                    break;
                }
            }
        #endif

            return !m_tty.IsEmpty();
        #endif
        }
    protected:
        wxString GetBinary() const
        {
            wxFileName gsgsTerminal(gsgs_Path().GetBinFolder(), "gsgs-terminal");
        #ifdef __WXMSW__
            gsgsTerminal.SetExt("exe");
        #elif defined(__WXOSX__)
            gsgsTerminal.AppendDir("gsgs-terminal.app");
        #endif

        #ifdef __WXOSX__
            wxString cmd = gsgsTerminal.GetPath();
            cmd = WrapWithQuotesIfNeeded(cmd);
            cmd.Prepend("/usr/bin/open ");
            cmd << " --args ";
            return cmd;
        #else
            return gsgsTerminal.GetFullPath();
        #endif
        }
    };

    ConsoleBase::ConsoleBase() {}

    ConsoleBase::~ConsoleBase() {}

    ConsoleBase::Ptr_t ConsoleBase::GetTerminal()
    {
        ConsoleBase::Ptr_t terminal;
        wxString terminalName = GetSelectedTerminalName();
    #ifdef __WXMSW__
        if(terminalName.CmpNoCase("gsgs-terminal") == 0) {
            terminal.reset(new ConsoleGSGSTerminal());
        } else {
            terminal.reset(new ConsoleCMD());
        }
    #elif defined(__WXGTK__)
        if(terminalName.CmpNoCase("konsole") == 0) {
            terminal.reset(new ConsoleKonsole());
        } else if(terminalName.CmpNoCase("lxterminal") == 0) {
            terminal.reset(new ConsoleLXTerminal());
        } else if(terminalName.CmpNoCase("mate-terminal") == 0) {
            terminal.reset(new ConsoleMateTerminal());
        } else if(terminalName.CmpNoCase("xfce4-terminal") == 0) {
            terminal.reset(new ConsoleXfce4Terminal());
        } else if(terminalName.CmpNoCase("qterminal") == 0) {
            terminal.reset(new ConsoleQTerminal());
        } else if(terminalName.CmpNoCase("rxvt-unicode") == 0) {
            terminal.reset(new ConsoleRXVTTerminal());
        } else if(terminalName.CmpNoCase("gsgs-terminal") == 0) {
            terminal.reset(new ConsoleGSGSTerminal());
        } else {
            // the default terminal is "gnome-terminal"
            terminal.reset(new ConsoleGnomeTerminal());
        }
    #else
        if(terminalName.CmpNoCase("gsgs-terminal") == 0) {
            terminal.reset(new ConsoleGSGSTerminal());
        } else {
            ConsoleOSXTerminal* t = new ConsoleOSXTerminal();
            terminal.reset(t);
            if(terminalName.CmpNoCase("iTerm2") == 0) { t->SetTerminalApp("iTerm"); }
        }
    #endif
        return terminal;
    }

    wxArrayString ConsoleBase::GetAvailaleTerminals()
    {
        wxArrayString terminals;
    #ifdef __WXMSW__
        terminals.Add("CMD");
    #elif defined(__WXGTK__)
        terminals.Add("konsole");
        terminals.Add("gnome-terminal");
        terminals.Add("lxterminal");
        terminals.Add("mate-terminal");
        terminals.Add("qterminal");
        terminals.Add("xfce4-terminal");
        terminals.Add("rxvt-unicode");
    #else
        terminals.Add("Terminal");
        terminals.Add("iTerm2");
    #endif
        terminals.Add("gsgs-terminal");
        return terminals;
    }

    void ConsoleBase::AddEnvVariable(const wxString& name, const wxString& value)
    {
        m_environment.erase(name);
        m_environment.insert({ name, value });
    }

    wxString ConsoleBase::GetEnvironmentPrefix() const
    {
        wxString strline;
        std::for_each(m_environment.begin(), m_environment.end(),
                      [&](const gsgsStringMap::value_type& vt) { strline << vt.first << "=" << vt.second << " "; });
        return strline;
    }

    wxString ConsoleBase::WrapWithQuotesIfNeeded(const wxString& s) const
    {
        wxString strimmed = s;
        strimmed.Trim().Trim(false);
        if(strimmed.Contains(" ")) { strimmed.Prepend("\"").Append("\""); }
        return strimmed;
    }

    wxString ConsoleBase::EscapeString(const wxString& str, const wxString& c) const
    {
        wxString escaped = str;
        escaped.Replace(c, wxString() << "\\" << c);
        return escaped;
    }

    bool ConsoleBase::StartProcess(const wxString& command)
    {
        wxProcess* callback = nullptr;
        if(m_callback) {
            // user provided callback
            callback = m_callback;
        } else if(m_sink) {
            // using events. This object will get deleted when the process exits
            callback = new ConsoleProcess(m_sink, m_callbackUID);
        }

        SetPid(::wxExecute(command, wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER | GetExecExtraFlags(), callback));
        // reset the m_callback (it will auto-delete itself)
        m_callback = nullptr;
        m_sink = nullptr;
        m_callbackUID.clear();
        return (GetPid() > 0);
    }

    wxString ConsoleBase::GetSelectedTerminalName()
    {
        wxString terminalName;
        gsgs_Config().Read("Terminal", terminalName, wxString());
        if(terminalName.IsEmpty()) {
    #ifdef __WXGTK__
            terminalName = "gnome-terminal";
    #elif defined(__WXOSX__)
            terminalName = "Terminal";
    #else
            terminalName = "CMD";
    #endif
        }
        return terminalName;
    }

    ConsoleEnvironment::ConsoleEnvironment() {}

    ConsoleEnvironment::~ConsoleEnvironment() { UnApply(); }

    void ConsoleEnvironment::Add(const wxString& name, const wxString& value)
    {
        m_environment.erase(name);
        m_environment.insert({ name, value });
    }

    void ConsoleEnvironment::Apply()
    {
        if(!m_oldEnvironment.empty()) {
            gsgsWARNING() << "Refusing to apply environment. Already in a dirty state";
            return;
        }
        if(m_environment.empty()) { return; }

        // keep a copy of the old environment before we apply the new values
        m_oldEnvironment.clear();
        std::for_each(m_environment.begin(), m_environment.end(), [&](const gsgsStringMap::value_type& vt) {
            wxString envvalue;
            if(::wxGetEnv(vt.first, &envvalue)) {
                m_oldEnvironment[vt.first] = envvalue;
            } else {
                m_oldEnvironment[vt.first] = "__no_such_env__";
            }
            ::wxSetEnv(vt.first, vt.second);
        });
    }

    void ConsoleEnvironment::UnApply()
    {
        if(m_oldEnvironment.empty()) { return; }
        std::for_each(m_oldEnvironment.begin(), m_oldEnvironment.end(), [&](const gsgsStringMap::value_type& vt) {
            if(vt.second == "__no_such_env__") {
                ::wxUnsetEnv(vt.second);
            } else {
                ::wxSetEnv(vt.first, vt.second);
            }
        });
        m_oldEnvironment.clear();
    }

    ConsoleEnvironment::ConsoleEnvironment(const gsgsStringMap& env)
        : m_environment(env)
    {
    }

    #define ADD_CURRENT_TOKEN()                                  \
        if(!curtoken.IsEmpty()) {                                \
            curtoken.Trim().Trim(false);                         \
            if(!curtoken.IsEmpty()) { outputArr.Add(curtoken); } \
            curtoken.Clear();                                    \
        }

    wxArrayString ConsoleBase::SplitArguments(const wxString& args)
    {
        const int STATE_NORMAL = 0;
        const int STATE_STRING = 1;

        int state = STATE_NORMAL;
        wxArrayString outputArr;
        wxString curtoken;
        wxChar prevChar = 0;
        for(size_t i = 0; i < args.size(); ++i) {
            wxChar ch = args[i];
            switch(state) {
            case STATE_NORMAL: {
                switch(ch) {
                case ' ':
                case '\t':
                    if(prevChar == '\\') {
                        curtoken << ch;
                    } else {
                        ADD_CURRENT_TOKEN();
                    }
                    break;
                case '"':
                case '\'':
                    // we dont want to keep the string markers
                    state = STATE_STRING;
                    break;
                default:
                    curtoken << ch;
                    break;
                }
            } break;
            case STATE_STRING: {
                switch(ch) {
                case '"':
                case '\'':
                    if(prevChar == '\\') {
                        curtoken << ch;
                    } else {
                        // we dont want to keep the string markers
                        state = STATE_NORMAL;
                    }
                    break;
                default:
                    curtoken << ch;
                    break;
                }
            } break;
            default:
                break;
            }
            prevChar = ch;
        }
        // if we still got some unprocessed token, add it
        ADD_CURRENT_TOKEN();
        return outputArr;
    }
}