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

#ifndef gsgs_PROCESS_H
#define gsgs_PROCESS_H

#include "gsgsPreInclude.h"
#include <wx/process.h>

#ifdef __WXMSW__

#include <windows.h>
#include <SetupAPI.h>
#include <Psapi.h>
#include <tlhelp32.h>
#include <wx/msw/winundef.h>

class WinProcess
{
public:
	WinProcess();
	virtual ~WinProcess();

	// Create process asynchronously and return a process object
	static WinProcess* Execute(const wxString& cmd, wxString &errMsg, const wxString &workingDir = wxEmptyString);

	// Read from process stdout - return immediately if no data is available
	bool Read(wxString& buff);

	// Write to the process stdin
	bool Write(const wxString& buff);

	// Return true if the process is still alive
	bool IsAlive();

	// Clean the process resources and kill the process if it is
	// still alive
	void Cleanup();
	
private:
	// WINDOWS implementation
	// Creating process related handles
	HANDLE	hChildStdinRd, hChildStdinWr, hChildStdinWrDup, 
			hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup, 
			hSaveStdin, hSaveStdout;

	// Child process id & information 
	DWORD dwProcessId;
	PROCESS_INFORMATION piProcInfo;
};

#endif

#if defined(__WXGTK__) || defined(__WXOSX__)
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <sys/wait.h>
#include <thread>
#include <unistd.h>
#include <wx/thread.h>
#include <wx/msgqueue.h>
#include <atomic>

// Wrapping pipe in a class makes sure they are closed when we leave scope
#define CLOSE_FD(fd)        \
    if(fd != wxNOT_FOUND) { \
        ::close(fd);        \
        fd = wxNOT_FOUND;   \
    }

class CPipe
{
private:
    int m_readFd = wxNOT_FOUND;
    int m_writeFd = wxNOT_FOUND;

public:
    const inline int GetReadFd() const  { return m_readFd; }
    const inline int GetWriteFd() const { return m_writeFd; }
    CPipe() {}
    void Close()
    {
        CLOSE_FD(m_readFd);
        CLOSE_FD(m_writeFd);
    }
    ~CPipe() { Close(); }
    bool Open()
    {
        int fd[2];
        if(pipe(fd) == 0) {
            m_readFd = fd[0];
            m_writeFd = fd[1];
            return true;
        }
        return false;
    }
    void CloseWriteFd()                 { CLOSE_FD(m_writeFd); }
    void CloseReadFd()                  { CLOSE_FD(m_readFd); }
};

class UnixProcess
{
protected:
    // sync operations
    static bool ReadAll(int fd, std::string& content, int timeoutMilliseconds);
    static bool Write(int fd, const std::string& message, std::atomic_bool& shutdown);

    void StartWriterThread();
    void StartReaderThread();

public:
    int child_pid = -1;

    UnixProcess(wxEvtHandler* owner, const wxArrayString& args);
    ~UnixProcess();

    // wait for process termination
    int Wait();

    // Async calls with callbacks
    void Write(const std::string& message);

    // stop the running process
    void Stop();

    /**
     * @brief stop sending events from the process
     */
    void Detach();
private:
    CPipe m_childStdin;
    CPipe m_childStdout;
    CPipe m_childStderr;
    std::thread* m_writerThread = nullptr;
    std::thread* m_readerThread = nullptr;
    wxMessageQueue<std::string> m_outgoingQueue;
    std::atomic_bool m_goingDown;
    wxEvtHandler* m_owner = nullptr;
};
#endif // defined(__WXGTK__)||defined(__WXOSX__)

struct ProcessEntry {
    wxString name;
    long pid;
};

class _gsgsAPI ProcUtils
{
public:
    ProcUtils();
    ~ProcUtils();

    static void GetProcTree(std::map<unsigned long, bool> &parentsMap, long pid);
    static void ExecuteCommand(const wxString &command, wxArrayString &output, long flags = wxEXEC_NODISABLE| wxEXEC_SYNC);
    static void ExecuteInteractiveCommand(const wxString &command);
    static wxString GetProcessNameByPid(long pid);
    static void GetProcessList(std::vector<ProcessEntry> &proclist);
    static void GetChildren(long pid, std::vector<long> &children);
    static bool Shell(const wxString& programConsoleCommand);
    static bool Locate(const wxString &name, wxString &where);
    
    /**
     * \brief a safe function that executes 'command' and returns its output. This function
     * is safed to be called from secondary thread (hence, SafeExecuteCommand)
     * \param command
     * \param output
     */
    static void SafeExecuteCommand(const wxString &command, wxArrayString &output);
    
    /**
     * @brief execute a command and return its output as plain string
     */
    static wxString SafeExecuteCommand(const wxString &command);
};

enum IProcessCreateFlags {
    IProcessCreateDefault = (1 << 0),           // Default: create process with no console window
    IProcessCreateConsole = (1 << 1),           // Create with console window shown
    IProcessCreateWithHiddenConsole = (1 << 2), // Create process with a hidden console
    IProcessCreateSync =
        (1 << 3), // Create a synchronous process (i.e. there is no background thread that performs the reads)
    IProcessCreateAsSuperuser = (1 << 4), // On platforms that support it, start the process as superuser
    IProcessNoRedirect = (1 << 5),
    IProcessStderrEvent = (1 << 6), // fire a separate event for stderr output
    IProcessRawOutput = (1 << 7),   // return the process output as is, don't strip anything. By default gsgs strips
                                    // terminal colours escape sequences
};

class _gsgsAPI IProcessCallback : public wxEvtHandler
{
public:
    virtual void OnProcessOutput(const wxString& str) { wxUnusedVar(str); }
    virtual void OnProcessTerminated() {}
};

class _gsgsAPI IProcess
{
public:
    typedef wxSharedPtr<IProcess> Ptr_t;

public:
    IProcess(wxEvtHandler* parent)
        : m_parent(parent)
        , m_pid(-1)
        , m_hardKill(false)
        , m_callback(NULL)
        , m_flags(0)
    {
    }
    virtual ~IProcess() {}
public:
    // Handle process exit code. This is done this way this
    // under Linux / Mac the exit code is returned only after the signal child has been
    // handled by gsgs
    static void SetProcessExitCode(int pid, int exitCode);
    static bool GetProcessExitCode(int pid, int& exitCode);

    // Stop notifying the parent window about input/output from the process
    // this is useful when we wish to terminate the process onExit but we don't want
    // to know about its termination
    virtual void Detach() = 0;

    // Read from process stdout - return immediately if no data is available
    virtual bool Read(wxString& buff, wxString& buffErr) = 0;

    // Write to the process stdin
    // This version add LF to the buffer
    virtual bool Write(const wxString& buff) = 0;

    // ANSI version
    // This version add LF to the buffer
    virtual bool Write(const std::string& buff) = 0;

    // Write to the process stdin
    virtual bool WriteRaw(const wxString& buff) = 0;

    // ANSI version
    virtual bool WriteRaw(const std::string& buff) = 0;

    /**
     * @brief wait for process to terminate and return all its output to the caller
     * Note that this function is blocking
     */
    virtual void WaitForTerminate(wxString& output);

    /**
     * @brief this method is mostly needed on MSW where writing a password
     * is done directly on the console buffer rather than its stdin
     */
    virtual bool WriteToConsole(const wxString& buff) = 0;

    // Return true if the process is still alive
    virtual bool IsAlive() = 0;

    // Clean the process resources and kill the process if it is
    // still alive
    virtual void Cleanup() = 0;

    // Terminate the process. It is recommended to use this method
    // so it will invoke the 'Cleaup' procedure and the process
    // termination event will be sent out
    virtual void Terminate() = 0;

    void SetPid(int pid)                { this->m_pid = pid; }

    int GetPid() const                  { return m_pid; }

    void SetHardKill(bool hardKill)     { this->m_hardKill = hardKill; }
    bool GetHardKill() const            { return m_hardKill; }
    IProcessCallback* GetCallback()     { return m_callback; }

    /**
     * @brief do we have process redirect enabled?
     */
    bool IsRedirect() const             { return !(m_flags & IProcessNoRedirect); }
protected:
    wxEvtHandler* m_parent;
    int m_pid;
    bool m_hardKill;
    IProcessCallback* m_callback;
    size_t m_flags; // The creation flags
};

#ifdef __WXMSW__
#include <windows.h>
#include <stdio.h>
BOOL APIENTRY MyCreatePipe(OUT LPHANDLE lpReadPipe, OUT LPHANDLE lpWritePipe, IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
    IN DWORD nSize);
#endif

// Help method
/**
 * @brief start process
 * @param parent the parent. all events will be sent to this object
 * @param cmd command line to execute
 * @param flags possible creation flag
 * @param workingDir set the working directory of the executed process
 * @return
 */
_gsgsAPI IProcess* CreateAsyncProcess(wxEvtHandler * parent, const wxString & cmd,
    size_t flags = IProcessCreateDefault, const wxString & workingDir = wxEmptyString);

/**
 * @brief create synchronus process
 * @param cmd command to execute
 * @param flags process creation flags
 * @param workingDir working directory for the new process
 * @return IPorcess handle on succcess
 */
_gsgsAPI IProcess* CreateSyncProcess(const wxString & cmd, size_t flags = IProcessCreateDefault,
    const wxString& workingDir = wxEmptyString);

/**
 * @brief start process
 * @brief cb callback object. Instead of events, OnProcessOutput and OnProcessTerminated will be called respectively
 * @param parent the parent. all events will be sent to this object
 * @param cmd command line to execute
 * @param flags possible creation flag
 * @param workingDir set the working directory of the executed process
 * @return
 */
_gsgsAPI IProcess* CreateAsyncProcessCB(wxEvtHandler * parent, IProcessCallback* cb, const wxString & cmd,
    size_t flags = IProcessCreateDefault, const wxString & workingDir = wxEmptyString);

class _gsgsAPI CmdProcess : public wxProcess
{
public:
    /**
     * Constructs a process object. id is only used in the case you want to use wxWidgets events.
     * It identifies this object, or another window that will receive the event.
     */
    CmdProcess(int id, const wxString& cmdLine, bool redirect = true);

    virtual ~CmdProcess();

    //void SetPid(long pid);
    //long GetPid();

    long Start(bool hide = true);

    void Terminate(wxSignal signalNo = wxSIGKILL);

    int GetUid()                        { return m_uid; }
    void SetType(int type)              { m_type = type; }
    int GetType() const                 { return m_type; }
    void SetCommand(const wxString& cmd) { m_cmd = cmd; }
    bool HasInput(wxString& input, wxString& errors);

    /**
     * Read all process output & error
     * \param input [output]
     * \param errors [output]
     * \return
     */
    bool ReadAll(wxString& input, wxString& errors);

    /**
     * Write text to the process
     * \param text
     * \return true on success, false otherwise
     */
    bool Write(const wxString& text);

    // Getters
    const bool& GetRedirect() const     { return m_redirect; }
private:
    //long m_pid;
    int m_uid;
    int m_type;
    wxString m_cmd;
    bool m_redirect;
};

DECLARE_EXPORTED_EVENT_TYPE(_gsgsAPI, wxEVT_ASYNC_PROC_ADDLINE, wxID_ANY)
DECLARE_EXPORTED_EVENT_TYPE(_gsgsAPI, wxEVT_ASYNC_PROC_ADDERRLINE, wxID_ANY)
DECLARE_EXPORTED_EVENT_TYPE(_gsgsAPI, wxEVT_ASYNC_PROC_STARTED, wxID_ANY)
DECLARE_EXPORTED_EVENT_TYPE(_gsgsAPI, wxEVT_ASYNC_PROC_ENDED, wxID_ANY)

class _gsgsAPI ExecCmd : public wxEvtHandler
{
protected:
    virtual void OnTimer(wxTimerEvent &event);
    virtual void PrintOutput();
    virtual void DoPrintOutput(const wxString &out, const wxString &err);

#ifndef __WXMSW__
    // A process was terminated and caught by the ZombieReaper - check if it 
    // is our process
    void OnZombieReaperProcessTerminated(wxProcessEvent &event);
#endif

public:
    inline bool IsBusy() const {
        return m_busy;
    }
    inline void SetBusy(bool busy) {
        m_busy = busy;
    }
    void Stop();
    void ProcessEnd(wxProcessEvent& event);
    CmdProcess *GetProcess() {
        return m_proc;
    }

public:
    //construct a compiler action
    // \param owner the window owner for this action
    ExecCmd(wxEvtHandler *owner);

    virtual ~ExecCmd();
    virtual void Execute(const wxString &cmdLine, bool hide = true, bool redirect = true);

    wxOutputStream *GetOutputStream() {
        if (m_proc) {
            return m_proc->GetOutputStream();
        }
        return NULL;
    }

    void AppendLine(const wxString &line, bool isErr = false);
    void SendStartMsg();
    void SendEndMsg(int exitCode);
    void Terminate();
    DECLARE_EVENT_TABLE()
protected:
    CmdProcess *m_proc;
    wxEvtHandler *m_owner;
    wxTimer *m_timer;
    bool m_busy;
    bool m_stop;
    wxString m_cmdLine;
};

class _gsgsAPI PipedProcess : public wxProcess
{
public:
	/**
	 * Constructs a process object. id is only used in the case you want to use wxWidgets events. 
	 * It identifies this object, or another window that will receive the event.
	 */
	PipedProcess(int id, const wxString &cmdLine);
	virtual ~PipedProcess();

	/*void SetPid(long pid);

	long GetPid();*/

	/**
	 * Kill the process
	 */
	void Terminate();

	/** 
	 * Start the process
	 * \return the process id
	 */
	virtual long Start(bool hide = true);
	
	virtual bool HasInput(wxString &input);
	
	/**
	 * \brief read all input from the process output stream
	 * \param input
	 * \return true if some data was read, false otherwise
	 */
	virtual bool ReadAll(wxString &input);
private:
	//long m_pid;
	wxString m_cmd;
};

#endif
