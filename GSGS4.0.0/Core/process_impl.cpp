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

#include "gsgsLogManager.h"
#include "process_impl.h"
#include "gsgsProcess.h"
#include "gsgsFileUtil.h"
#include "SocketAPI/gsgsSocketBase.h"
#include "gsgsSharedPtr.h"
#include <thread>
#include <cstring>

wxDEFINE_EVENT(wxEVT_ASYNC_PROCESS_OUTPUT, gsgs::ProcessEvent);
wxDEFINE_EVENT(wxEVT_ASYNC_PROCESS_STDERR, gsgs::ProcessEvent);
wxDEFINE_EVENT(wxEVT_ASYNC_PROCESS_TERMINATED, gsgs::ProcessEvent);

/**
 * Worker Thread class
 * usually user should define the ProcessRequest method
 */
class _gsgsAPI ProcessReaderThread : public wxThread
{
public:
    ProcessReaderThread();
    virtual ~ProcessReaderThread();

    /**
     * Thread execution point.
     */
    virtual void* Entry();

    /**
     * Called when the thread exits
     * whether it terminates normally or is stopped with Delete() (but not when it is Kill()'ed!)
     */
    virtual void OnExit(){};
    
    inline void SetProcess(IProcess* proc)                 { m_process = proc; }

    /**
     * Set the window to be notified when a change was done
     * between current source file tree and the actual tree.
     * \param evtHandler
     */
    inline void SetNotifyWindow(wxEvtHandler* evtHandler)  { m_notifiedWindow = evtHandler; }

    /**
     * Stops the thread
     * This function returns only when the thread is terminated.
     * \note This call must be called from the context of other thread (e.g. main thread)
     */
    void Stop();

    /**
     * Start the thread as joinable thread.
     * \note This call must be called from the context of other thread (e.g. main thread)
     */
    void Start(int priority = WXTHREAD_DEFAULT_PRIORITY);
protected:
    void NotifyTerminated();
protected:
    wxEvtHandler* m_notifiedWindow;
    IProcess* m_process;
};

#if defined(__WXGTK__) || defined(__WXMAC__)
#include <sys/wait.h>
#endif
//-----------------------------------------------------------------------------------
ProcessReaderThread::ProcessReaderThread()
    : wxThread(wxTHREAD_JOINABLE)
    , m_notifiedWindow(NULL)
    , m_process(NULL)
{
}
//-----------------------------------------------------------------------------------
ProcessReaderThread::~ProcessReaderThread() { m_notifiedWindow = NULL; }
//-----------------------------------------------------------------------------------
void* ProcessReaderThread::Entry()
{
    while(true) {
        // Did we get a request to terminate?
        if(TestDestroy()) { break; }

        if(m_process) {
            wxString buff;
            wxString buffErr;
            if(m_process->IsRedirect()) {
                if(m_process->Read(buff, buffErr)) {
                    if(!buff.IsEmpty() || !buffErr.IsEmpty()) {
                        // If we got a callback object, use it
                        if(m_process && m_process->GetCallback()) {
                            m_process->GetCallback()->CallAfter(&IProcessCallback::OnProcessOutput, buff);

                        } else {
                            // We fire an event per data (stderr/stdout)
                            if(!buff.IsEmpty()) {
                                // fallback to the event system
                                // we got some data, send event to parent
                                gsgs::ProcessEvent e(wxEVT_ASYNC_PROCESS_OUTPUT);
                                e.SetOutput(buff);
                                e.SetProcess(m_process);
                                if(m_notifiedWindow) { m_notifiedWindow->AddPendingEvent(e); }
                            }
                            if(!buffErr.IsEmpty()) {
                                // we got some data, send event to parent
                                gsgs::ProcessEvent e(wxEVT_ASYNC_PROCESS_STDERR);
                                e.SetOutput(buffErr);
                                e.SetProcess(m_process);
                                if(m_notifiedWindow) { m_notifiedWindow->AddPendingEvent(e); }
                            }
                        }
                    }
                } else {

                    // Process terminated, exit
                    // If we got a callback object, use it
                    NotifyTerminated();
                    break;
                }
            } else {
                // Check if the process is alive
                if(!m_process->IsAlive()) {
                    // Notify about termination
                    NotifyTerminated();
                    break;
                } else {
                    wxThread::Sleep(10);
                }
            }
        } else {
            // No process??
            break;
        }
    }
    m_process = NULL;
    return NULL;
}
//-----------------------------------------------------------------------------------
void ProcessReaderThread::Stop()
{
    // Notify the thread to exit and
    // wait for it
    if(IsAlive()) {
        Delete(NULL, wxTHREAD_WAIT_BLOCK);
    } else {
        Wait(wxTHREAD_WAIT_BLOCK);
    }
}
//-----------------------------------------------------------------------------------
void ProcessReaderThread::Start(int priority)
{
    Create();
    SetPriority(priority);
    Run();
}
//-----------------------------------------------------------------------------------
void ProcessReaderThread::NotifyTerminated()
{
    // Process terminated, exit
    // If we got a callback object, use it
    if(m_process && m_process->GetCallback()) {
        m_process->GetCallback()->CallAfter(&IProcessCallback::OnProcessTerminated);

    } else {
        // fallback to the event system
        gsgs::ProcessEvent e(wxEVT_ASYNC_PROCESS_TERMINATED);
        e.SetProcess(m_process);
        if(m_notifiedWindow) { m_notifiedWindow->AddPendingEvent(e); }
    }
}
//-----------------------------------------------------------------------------------

#ifdef __WXMSW__

#include <memory>
#include <wx/filefn.h>
#include <wx/msgqueue.h>
#include <atomic>

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT 0x0501 // Make AttachConsole(DWORD) visible

class MyDirGuard
{
    wxString _d;

public:
    MyDirGuard()
        : _d(wxGetCwd())
    {
    }
    ~MyDirGuard() { wxSetWorkingDirectory(_d); }
};

/**
 * @class ConsoleAttacher
 * @date 11/03/10
 * @brief a helper class to attach this process to a process' console
 * this allows us to write directly into that process console-input-buffer
 * One should check isAttached once this object is constructed
 */
class ConsoleAttacher
{
public:
    bool isAttached;

public:
    ConsoleAttacher(long pid) { isAttached = AttachConsole(pid); }

    ~ConsoleAttacher()
    {
        if (isAttached) { FreeConsole(); }
        isAttached = false;
    }
};
//-----------------------------------------------------------------------------------
static bool CheckIsAlive(HANDLE hProcess)
{
    DWORD dwExitCode;
    if (GetExitCodeProcess(hProcess, &dwExitCode)) {
        if (dwExitCode == STILL_ACTIVE) return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------
template <typename T> bool WriteStdin(const T& buffer, HANDLE hStdin, HANDLE hProcess)
{
    DWORD dwMode;

    // Make the pipe to non-blocking mode
    dwMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
    SetNamedPipeHandleState(hStdin, &dwMode, NULL, NULL);
    DWORD bytesLeft = buffer.length();
    long offset = 0;
    size_t retryCount = 0;
    while (bytesLeft > 0 && (retryCount < 100)) {
        DWORD dwWritten = 0;
        if (!WriteFile(hStdin, buffer.c_str() + offset, bytesLeft, &dwWritten, NULL)) {
            int errorCode = GetLastError();
            gsgsERROR() << ">> WriteStdin: (WriteFile) error:" << errorCode;
            return false;
        }
        if (!CheckIsAlive(hProcess)) { return false; }
        if (dwWritten == 0) { std::this_thread::sleep_for(std::chrono::milliseconds(10)); }
        bytesLeft -= dwWritten;
        offset += dwWritten;
        ++retryCount;
    }
    return true;
}
//-----------------------------------------------------------------------------------
class _gsgsAPI WinWriterThread
{
    std::thread* m_thread = nullptr;
    wxMessageQueue<std::string> m_outgoingQueue;
    std::atomic_bool m_shutdown;
    HANDLE m_hProcess = INVALID_HANDLE_VALUE;
    HANDLE m_hStdin = INVALID_HANDLE_VALUE;

public:
    WinWriterThread(HANDLE hProcess, HANDLE hStdin)
        : m_hProcess(hProcess)
        , m_hStdin(hStdin)
    {
        m_shutdown.store(false);
    }

    ~WinWriterThread() {}

    void Start() { m_thread = new std::thread(&WinWriterThread::Entry, this, m_hStdin); }
    void Stop()
    {
        if (m_thread) {
            m_shutdown.store(true);
            m_thread->join();
            wxDELETE(m_thread);
        }
    }
    static void Entry(WinWriterThread* thr, HANDLE hStdin)
    {
        auto& Q = thr->m_outgoingQueue;
        while (!thr->m_shutdown.load()) {
            std::string cstr;
            if (Q.ReceiveTimeout(50, cstr) == wxMSGQUEUE_NO_ERROR) {
                if (!WriteStdin(cstr, hStdin, thr->m_hProcess)) {
                    gsgsERROR() << "WriteFile error:" << GetLastError();
                }
                else {
                    gsgsDEBUG1() << "Writer thread: wrote buffer of" << cstr.length() << "bytes";
                }
            }
        }
        gsgsDEBUG1() << "Write thread going down";
    }

    void Write(const std::string& buffer) { m_outgoingQueue.Post(buffer); }
};
//-----------------------------------------------------------------------------------
IProcess* WinProcessImpl::Execute(wxEvtHandler* parent, const wxString& cmd, wxString& errMsg, size_t flags,
    const wxString& workingDir, IProcessCallback* cb)
{
    SECURITY_ATTRIBUTES saAttr;
    BOOL fSuccess;

    MyDirGuard dg;

    wxString wd(workingDir);
    if (workingDir.IsEmpty()) { wd = wxGetCwd(); }
    wxSetWorkingDirectory(wd);

    // Set the bInheritHandle flag so pipe handles are inherited.
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;
    WinProcessImpl* prc = new WinProcessImpl(parent);
    prc->m_callback = cb;
    prc->m_flags = flags;

    bool redirectOutput = !(flags & IProcessNoRedirect);

    // The steps for redirecting child process's STDOUT:
    //     1. Save current STDOUT, to be restored later.
    //     2. Create anonymous pipe to be STDOUT for child process.
    //     3. Set STDOUT of the parent process to be write handle to
    //        the pipe, so it is inherited by the child process.
    //     4. Create a noninheritable duplicate of the read handle and
    //        close the inheritable read handle.

    if (redirectOutput) {
        // Save the handle to the current STDOUT.
        prc->hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE);

        // Create a pipe for the child process's STDOUT.
        if (!CreatePipe(&prc->hChildStdoutRd, &prc->hChildStdoutWr, &saAttr, 0)) {
            delete prc;
            return NULL;
        }

        // Set a write handle to the pipe to be STDOUT.
        if (!SetStdHandle(STD_OUTPUT_HANDLE, prc->hChildStdoutWr)) {
            delete prc;
            return NULL;
        }

        // Create noninheritable read handle and close the inheritable read handle.
        fSuccess = DuplicateHandle(GetCurrentProcess(), prc->hChildStdoutRd, GetCurrentProcess(),
            &prc->hChildStdoutRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
        if (!fSuccess) {
            delete prc;
            return NULL;
        }
        CloseHandle(prc->hChildStdoutRd);

        // The steps for redirecting child process's STDERR:
        //     1. Save current STDERR, to be restored later.
        //     2. Create anonymous pipe to be STDERR for child process.
        //     3. Set STDERR of the parent process to be write handle to
        //        the pipe, so it is inherited by the child process.
        //     4. Create a noninheritable duplicate of the read handle and
        //        close the inheritable read handle.

        // Save the handle to the current STDERR.
        prc->hSaveStderr = GetStdHandle(STD_ERROR_HANDLE);

        // Create a pipe for the child process's STDERR.
        if (!CreatePipe(&prc->hChildStderrRd, &prc->hChildStderrWr, &saAttr, 0)) {
            delete prc;
            return NULL;
        }

        // Set a write handle to the pipe to be STDERR.
        if (!SetStdHandle(STD_ERROR_HANDLE, prc->hChildStderrWr)) {
            delete prc;
            return NULL;
        }

        // Create noninheritable read handle and close the inheritable read handle.
        fSuccess = DuplicateHandle(GetCurrentProcess(), prc->hChildStderrRd, GetCurrentProcess(),
            &prc->hChildStderrRdDup, 0, FALSE, DUPLICATE_SAME_ACCESS);
        if (!fSuccess) {
            delete prc;
            return NULL;
        }
        CloseHandle(prc->hChildStderrRd);

        // The steps for redirecting child process's STDIN:
        //     1.  Save current STDIN, to be restored later.
        //     2.  Create anonymous pipe to be STDIN for child process.
        //     3.  Set STDIN of the parent to be the read handle to the
        //         pipe, so it is inherited by the child process.
        //     4.  Create a noninheritable duplicate of the write handle,
        //         and close the inheritable write handle.

        // Save the handle to the current STDIN.
        prc->hSaveStdin = GetStdHandle(STD_INPUT_HANDLE);

        // Create a pipe for the child process's STDIN.
        if (!CreatePipe(&prc->hChildStdinRd, &prc->hChildStdinWr, &saAttr, 0)) {
            delete prc;
            return NULL;
        }
        // Set a read handle to the pipe to be STDIN.
        if (!SetStdHandle(STD_INPUT_HANDLE, prc->hChildStdinRd)) {
            delete prc;
            return NULL;
        }
        // Duplicate the write handle to the pipe so it is not inherited.
        fSuccess =
            DuplicateHandle(GetCurrentProcess(), prc->hChildStdinWr, GetCurrentProcess(), &prc->hChildStdinWrDup, 0,
                FALSE, // not inherited
                DUPLICATE_SAME_ACCESS);
        if (!fSuccess) {
            delete prc;
            return NULL;
        }
        CloseHandle(prc->hChildStdinWr);
    }

    // Execute the child process
    STARTUPINFO siStartInfo;

    // Set up members of STARTUPINFO structure.
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);

    siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
    if (redirectOutput) {
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
        siStartInfo.hStdInput = prc->hChildStdinRd;
        siStartInfo.hStdOutput = prc->hChildStdoutWr;
        siStartInfo.hStdError = prc->hChildStderrWr;
    }

    // Set the window to hide
    siStartInfo.wShowWindow = flags & IProcessCreateConsole ? SW_SHOW : SW_HIDE;
    DWORD creationFlags = flags & IProcessCreateConsole ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW;

    if (flags & IProcessCreateWithHiddenConsole) {
        siStartInfo.wShowWindow = SW_HIDE;
        creationFlags = CREATE_NEW_CONSOLE | CREATE_NEW_PROCESS_GROUP;
    }

    BOOL ret = CreateProcess(NULL,
        cmd.wchar_str(),   // shell line execution command
        NULL,              // process security attributes
        NULL,              // primary thread security attributes
        TRUE,              // handles are inherited
        creationFlags,     // creation flags
        NULL,              // use parent's environment
        NULL,              // CD to tmp dir
        &siStartInfo,      // STARTUPINFO pointer
        &prc->piProcInfo); // receives PROCESS_INFORMATION
    if (ret) {
        prc->dwProcessId = prc->piProcInfo.dwProcessId;
    }
    else {
        int err = GetLastError();
        wxUnusedVar(err);
        delete prc;
        return NULL;
    }

    if (redirectOutput) {
        // After process creation, restore the saved STDIN and STDOUT.
        if (!SetStdHandle(STD_INPUT_HANDLE, prc->hSaveStdin)) {
            delete prc;
            return NULL;
        }
        if (!SetStdHandle(STD_OUTPUT_HANDLE, prc->hSaveStdout)) {
            delete prc;
            return NULL;
        }
        if (!SetStdHandle(STD_OUTPUT_HANDLE, prc->hSaveStderr)) {
            delete prc;
            return NULL;
        }
    }

    if ((prc->m_flags & IProcessCreateConsole) || (prc->m_flags & IProcessCreateWithHiddenConsole)) {
        ConsoleAttacher ca(prc->GetPid());
        if (ca.isAttached) {
            freopen("CONOUT$", "wb", stdout); // reopen stout handle as console window output
            freopen("CONOUT$", "wb", stderr); // reopen stderr handle as console window output
        }
    }
    prc->SetPid(prc->dwProcessId);
    if (!(prc->m_flags & IProcessCreateSync)) { prc->StartReaderThread(); }
    prc->m_writerThread = new WinWriterThread(prc->piProcInfo.hProcess, prc->hChildStdinWrDup);
    prc->m_writerThread->Start();
    return prc;
}
//-----------------------------------------------------------------------------------
WinProcessImpl::WinProcessImpl(wxEvtHandler* parent)
    : IProcess(parent)
    , m_thr(NULL)
{
    hChildStdinRd = NULL;
    hChildStdinWrDup = NULL;
    hChildStdoutWr = NULL;
    hChildStdoutRdDup = NULL;
    hChildStderrWr = NULL;
    hChildStderrRdDup = NULL;
    piProcInfo.hProcess = NULL;
    piProcInfo.hThread = NULL;
}
//-----------------------------------------------------------------------------------
WinProcessImpl::~WinProcessImpl() { Cleanup(); }
//-----------------------------------------------------------------------------------
bool WinProcessImpl::Read(wxString& buff, wxString& buffErr)
{
    DWORD le1(-1);
    DWORD le2(-1);
    buff.Clear();
    buffErr.Clear();

    // Sanity
    if (!IsRedirect()) { return false; }

    // Read data from STDOUT and STDERR
    if (!DoReadFromPipe(hChildStderrRdDup, ((m_flags & IProcessStderrEvent) ? buffErr : buff))) { le2 = GetLastError(); }
    if (!DoReadFromPipe(hChildStdoutRdDup, buff)) { le1 = GetLastError(); }
    if ((le1 == ERROR_NO_DATA) && (le2 == ERROR_NO_DATA)) {
        if (IsAlive()) {
            wxThread::Sleep(15);
            return true;
        }
    }
    bool success = !buff.IsEmpty() || !buffErr.IsEmpty();
    if (!success) {
        DWORD dwExitCode;
        if (GetExitCodeProcess(piProcInfo.hProcess, &dwExitCode)) { SetProcessExitCode(GetPid(), (int)dwExitCode); }
    }
    return success;
}
//-----------------------------------------------------------------------------------
bool WinProcessImpl::Write(const wxString& buff)
{
    // Sanity
    return Write(FileUtil::ToStdString(buff));
}
//-----------------------------------------------------------------------------------
bool WinProcessImpl::Write(const std::string& buff) { return WriteRaw(buff + "\r\n"); }
//-----------------------------------------------------------------------------------
bool WinProcessImpl::WriteRaw(const wxString& buff) { return WriteRaw(FileUtil::ToStdString(buff)); }
//-----------------------------------------------------------------------------------
bool WinProcessImpl::WriteRaw(const std::string& buff)
{
    // Sanity
    if (!IsRedirect()) { return false; }
    m_writerThread->Write(buff);
    return true;
}
//-----------------------------------------------------------------------------------
bool WinProcessImpl::IsAlive()
{
    DWORD dwExitCode;
    if (GetExitCodeProcess(piProcInfo.hProcess, &dwExitCode)) {
        if (dwExitCode == STILL_ACTIVE) return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------
void WinProcessImpl::Cleanup()
{
    if (m_writerThread) {
        m_writerThread->Stop();
        wxDELETE(m_writerThread);
    }

    // Under windows, the reader thread is detached
    if (m_thr) {
        // Stop the reader thread
        m_thr->Stop();
        delete m_thr;
    }
    m_thr = NULL;

    // terminate the process
    if (IsAlive()) {
        std::map<unsigned long, bool> tree;
        ProcUtils::GetProcTree(tree, GetPid());

        for (const auto& vt : tree) {
            // don't kill ourself
            if ((long)vt.first == GetPid()) { continue; }
            wxLogNull NoLog;
            wxKillError rc;
            wxKill(vt.first, wxSIGKILL, &rc);
        }
        ::TerminateProcess(piProcInfo.hProcess, 0);
    }

    if (IsRedirect()) {
        CloseHandle(hChildStdinRd);
        CloseHandle(hChildStdinWrDup);
        CloseHandle(hChildStdoutWr);
        CloseHandle(hChildStdoutRdDup);
        CloseHandle(hChildStderrWr);
        CloseHandle(hChildStderrRdDup);
    }

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    hChildStdinRd = NULL;
    hChildStdoutWr = NULL;
    hChildStdinWrDup = NULL;
    hChildStdoutRdDup = NULL;
    hChildStderrWr = NULL;
    hChildStderrRdDup = NULL;
    piProcInfo.hProcess = NULL;
    piProcInfo.hThread = NULL;
}
//-----------------------------------------------------------------------------------
void WinProcessImpl::StartReaderThread()
{
    // Launch the 'Reader' thread
    m_thr = new ProcessReaderThread();
    m_thr->SetProcess(this);
    m_thr->SetNotifyWindow(m_parent);
    m_thr->Start();
}
//-----------------------------------------------------------------------------------
bool WinProcessImpl::DoReadFromPipe(HANDLE pipe, wxString& buff)
{
    DWORD dwRead;
    DWORD dwMode;
    DWORD dwTimeout;
    memset(m_buffer, 0, sizeof(m_buffer));

    // Make the pipe to non-blocking mode
    dwMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
    dwTimeout = 1000;
    SetNamedPipeHandleState(pipe, &dwMode, NULL, &dwTimeout);

    BOOL bRes = ReadFile(pipe, m_buffer, 65536, &dwRead, NULL);
    if (bRes) {
        wxString tmpBuff;
        // Success read
        m_buffer[dwRead / sizeof(char)] = 0;
        tmpBuff = wxString(m_buffer, wxConvUTF8);
        if (tmpBuff.IsEmpty() && dwRead > 0) {
            // conversion failed
            tmpBuff = wxString::From8BitData(m_buffer);
        }
        buff << tmpBuff;
        return true;
    }

    return false;
}
//-----------------------------------------------------------------------------------
void WinProcessImpl::Terminate()
{
    // terminate the process
    if (IsAlive()) {
        std::map<unsigned long, bool> tree;
        ProcUtils::GetProcTree(tree, GetPid());

        for (const auto& vt : tree) {
            if ((long)vt.first == GetPid()) { continue; }
            wxLogNull NoLOG;
            wxKillError rc;
            wxKill(vt.first, wxSIGKILL, &rc);
        }
        TerminateProcess(piProcInfo.hProcess, 0);
    }
}
//-----------------------------------------------------------------------------------
bool WinProcessImpl::WriteToConsole(const wxString& buff)
{
    wxString pass(buff);
    pass.Trim().Trim(false);

    // To write password, we need to attach to the child process console
    if (!(m_flags & (IProcessCreateWithHiddenConsole | IProcessCreateConsole))) return false;

    ConsoleAttacher ca(GetPid());
    if (ca.isAttached == false) return false;

    HANDLE hStdIn = ::CreateFile(L"CONIN$", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        OPEN_EXISTING, 0, 0);
    if (hStdIn == INVALID_HANDLE_VALUE) { return false; }

    pass += wxT("\r\n");
    SharedPtr<INPUT_RECORD> pKeyEvents(new INPUT_RECORD[pass.Len()]);

    for (size_t i = 0; i < pass.Len(); i++) {
        (pKeyEvents.Get())[i].EventType = KEY_EVENT;
        (pKeyEvents.Get())[i].Event.KeyEvent.bKeyDown = TRUE;
        (pKeyEvents.Get())[i].Event.KeyEvent.wRepeatCount = 1;
        (pKeyEvents.Get())[i].Event.KeyEvent.wVirtualKeyCode = LOBYTE(::VkKeyScan(pass[i]));
        (pKeyEvents.Get())[i].Event.KeyEvent.wVirtualScanCode = 0;
        (pKeyEvents.Get())[i].Event.KeyEvent.uChar.UnicodeChar = pass[i];
        (pKeyEvents.Get())[i].Event.KeyEvent.dwControlKeyState = 0;
    }

    DWORD dwTextWritten;
    if (::WriteConsoleInput(hStdIn, pKeyEvents.Get(), pass.Len(), &dwTextWritten) == FALSE) {
        CloseHandle(hStdIn);
        return false;
    }
    CloseHandle(hStdIn);
    return true;
}
//-----------------------------------------------------------------------------------
void WinProcessImpl::Detach()
{
    if (m_thr) {
        // Stop the reader thread
        m_thr->Stop();
        delete m_thr;
    }
    m_thr = NULL;
}
//-----------------------------------------------------------------------------------
#endif //__WXMSW__

#if defined(__WXMAC__) || defined(__WXGTK__)

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#ifdef __WXGTK__
#ifdef __FreeBSD__
#include <libutil.h>
#include <sys/ioctl.h>
#include <termios.h>
#elif defined(__NetBSD__)
#include <util.h>
#include <sys/ioctl.h>
#include <termios.h>
#else
#include <pty.h>
#include <utmp.h>
#endif
#else
#include <util.h>
#endif

static char** argv = NULL;
static int argc = 0;

// ----------------------------------------------
#define ISBLANK(ch) ((ch) == ' ' || (ch) == '\t')
#define BUFF_SIZE 1024 * 64

/*  Routines imported from standard C runtime libraries. */

#ifdef __STDC__

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#else /* !__STDC__ */

#if !defined _WIN32 || defined __GNUC__
extern char* memcpy();  /* Copy memory region */
extern int strlen();    /* Count length of string */
extern char* malloc();  /* Standard memory allocater */
extern char* realloc(); /* Standard memory reallocator */
extern void free();     /* Free malloc'd memory */
extern char* strdup();  /* Duplicate a string */
#endif

#endif /* __STDC__ */

#ifndef NULL
#define NULL 0
#endif

#ifndef EOS
#define EOS '\0'
#endif

#define INITIAL_MAXARGC 8 /* Number of args + NULL in initial argv */
//-----------------------------------------------------------------------------------
static void freeargv(char** vector)
{
    register char** scan;

    if(vector != NULL) {
        for(scan = vector; *scan != NULL; scan++) {
            free(*scan);
        }
        free(vector);
    }
}
//-----------------------------------------------------------------------------------
char** dupargv(char** argv)
{
    int argc;
    char** copy;

    if(argv == NULL) return NULL;

    /* the vector */
    for(argc = 0; argv[argc] != NULL; argc++)
        ;
    copy = (char**)malloc((argc + 1) * sizeof(char*));
    if(copy == NULL) return NULL;

    /* the strings */
    for(argc = 0; argv[argc] != NULL; argc++) {
        int len = strlen(argv[argc]);
        copy[argc] = (char*)malloc(sizeof(char*) * (len + 1));
        if(copy[argc] == NULL) {
            freeargv(copy);
            return NULL;
        }
        strcpy(copy[argc], argv[argc]);
    }
    copy[argc] = NULL;
    return copy;
}
//-----------------------------------------------------------------------------------
char** buildargv(const char* input)
{
    char* arg;
    char* copybuf;
    int squote = 0;
    int dquote = 0;
    int bsquote = 0;
    int argc = 0;
    int maxargc = 0;
    char** argv = NULL;
    char** nargv;

    if(input != NULL) {
        copybuf = (char*)alloca(strlen(input) + 1);
        /* Is a do{}while to always execute the loop once.  Always return an
        argv, even for null strings.  See NOTES above, test case below. */
        do {
            /* Pick off argv[argc] */
            while(ISBLANK(*input)) {
                input++;
            }
            if((maxargc == 0) || (argc >= (maxargc - 1))) {
                /* argv needs initialization, or expansion */
                if(argv == NULL) {
                    maxargc = INITIAL_MAXARGC;
                    nargv = (char**)malloc(maxargc * sizeof(char*));
                } else {
                    maxargc *= 2;
                    nargv = (char**)realloc(argv, maxargc * sizeof(char*));
                }
                if(nargv == NULL) {
                    if(argv != NULL) {
                        freeargv(argv);
                        argv = NULL;
                    }
                    break;
                }
                argv = nargv;
                argv[argc] = NULL;
            }
            /* Begin scanning arg */
            arg = copybuf;
            while(*input != EOS) {
                if(ISBLANK(*input) && !squote && !dquote && !bsquote) {
                    break;
                } else {
                    if(bsquote) {
                        bsquote = 0;
                        *arg++ = *input;
                    } else if(*input == '\\') {
                        bsquote = 1;
                    } else if(squote) {
                        if(*input == '\'') {
                            squote = 0;
                        } else {
                            *arg++ = *input;
                        }
                    } else if(dquote) {
                        if(*input == '"') {
                            dquote = 0;
                        } else {
                            *arg++ = *input;
                        }
                    } else {
                        if(*input == '\'') {
                            squote = 1;
                        } else if(*input == '"') {
                            dquote = 1;
                        } else {
                            *arg++ = *input;
                        }
                    }
                    input++;
                }
            }
            *arg = EOS;
            argv[argc] = strdup(copybuf);
            if(argv[argc] == NULL) {
                freeargv(argv);
                argv = NULL;
                break;
            }
            argc++;
            argv[argc] = NULL;

            while(ISBLANK(*input)) {
                input++;
            }
        } while(*input != EOS);
    }
    return (argv);
}
//-----------------------------------------------------------------------------------
static void make_argv(const wxString& cmd)
{
    if(argc) {
        freeargv(argv);
        argc = 0;
    }

    argv = buildargv(cmd.mb_str(wxConvUTF8).data());
    argc = 0;

    for(char** targs = argv; *targs != NULL; targs++) {
        argc++;
    }
}
//-----------------------------------------------------------------------------------
static void RemoveTerminalColoring(char* buffer)
{
    std::string cinput = buffer;
    std::string coutout;
    StringUtils::StripTerminalColouring(cinput, coutout);

    // coutout is ALWAYS <= cinput, so we can safely copy the content to the buffer
    if(coutout.length() < cinput.length()) {
        strcpy(buffer, coutout.c_str());
    }
}
//-----------------------------------------------------------------------------------
UnixProcessImpl::UnixProcessImpl(wxEvtHandler* parent)
    : IProcess(parent)
    , m_readHandle(-1)
    , m_writeHandle(-1)
    , m_thr(NULL)
{
}
//-----------------------------------------------------------------------------------
UnixProcessImpl::~UnixProcessImpl() 
{ 
    Cleanup(); 
}
//-----------------------------------------------------------------------------------
void UnixProcessImpl::Cleanup()
{
    close(GetReadHandle());
    close(GetWriteHandle());
    if(GetStderrHandle() != wxNOT_FOUND) 
    { 
        close(GetStderrHandle()); 
    }
    if(m_thr) 
    {
        // Stop the reader thread
        m_thr->Stop();
        delete m_thr;
    }
    m_thr = NULL;

    if(GetPid() != wxNOT_FOUND) {
        wxKill(GetPid(), GetHardKill() ? wxSIGKILL : wxSIGTERM, NULL, wxKILL_CHILDREN);
        // The Zombie cleanup is done in app.cpp in ::ChildTerminatedSingalHandler() signal handler
        int status(0);
        waitpid(-1, &status, WNOHANG);
    }
}
//-----------------------------------------------------------------------------------
bool UnixProcessImpl::IsAlive() { return kill(m_pid, 0) == 0; }
//-----------------------------------------------------------------------------------
bool UnixProcessImpl::ReadFromFd(int fd, fd_set& rset, wxString& output)
{
    if(fd == wxNOT_FOUND) { return false; }
    if(FD_ISSET(fd, &rset)) {
        // there is something to read
        char buffer[BUFF_SIZE + 1]; // our read buffer
        int bytesRead = read(fd, buffer, sizeof(buffer));
        if(bytesRead > 0) {
            buffer[bytesRead] = 0; // always place a terminator

            // Remove coloring chars from the incomnig buffer
            // colors are marked with ESC and terminates with lower case 'm'
            if(!(this->m_flags & IProcessRawOutput)) { RemoveTerminalColoring(buffer); }
            wxString convBuff = wxString(buffer, wxConvUTF8);
            if(convBuff.IsEmpty()) { convBuff = wxString::From8BitData(buffer); }

            output = convBuff;
            return true;
        }
    }
    return false;
}
//-----------------------------------------------------------------------------------
bool UnixProcessImpl::Read(wxString& buff, wxString& buffErr)
{
    fd_set rs;
    timeval timeout;

    memset(&rs, 0, sizeof(rs));
    FD_SET(GetReadHandle(), &rs);
    if(m_stderrHandle != wxNOT_FOUND) { FD_SET(m_stderrHandle, &rs); }

    timeout.tv_sec = 0;      // 0 seconds
    timeout.tv_usec = 50000; // 50 ms

    int errCode(0);
    errno = 0;

    buff.Clear();
    int maxFd = wxMax(GetStderrHandle(), GetReadHandle());
    int rc = select(maxFd + 1, &rs, NULL, NULL, &timeout);
    errCode = errno;
    if(rc == 0) {
        // timeout
        return true;

    } else if(rc > 0) {
        // We differentiate between stdout and stderr?
        bool stderrRead = ReadFromFd(GetStderrHandle(), rs, buffErr);
        bool stdoutRead = ReadFromFd(GetReadHandle(), rs, buff);
        return stderrRead || stdoutRead;

    } else {

        if(errCode == EINTR || errCode == EAGAIN) { return true; }

        // Process terminated
        // the exit code will be set in the sigchld event handler
        return false;
    }
}
//-----------------------------------------------------------------------------------
bool UnixProcessImpl::Write(const std::string& buff) { return WriteRaw(buff + "\n"); }
//-----------------------------------------------------------------------------------
bool UnixProcessImpl::Write(const wxString& buff) { return Write(FileUtil::ToStdString(buff)); }
//-----------------------------------------------------------------------------------
bool UnixProcessImpl::WriteRaw(const wxString& buff) { return WriteRaw(FileUtil::ToStdString(buff)); }
bool UnixProcessImpl::WriteRaw(const std::string& buff)
{
    std::string tmpbuf = buff;
    const int chunk_size = 1024;
    while(!tmpbuf.empty()) {
        int bytes_written =
            ::write(GetWriteHandle(), tmpbuf.c_str(), tmpbuf.length() > chunk_size ? chunk_size : tmpbuf.length());
        if(bytes_written <= 0) { return false; }
        tmpbuf.erase(0, bytes_written);
    }
    return true;
}
//-----------------------------------------------------------------------------------
IProcess* UnixProcessImpl::Execute(wxEvtHandler* parent, const wxString& cmd, size_t flags,
                                   const wxString& workingDirectory, IProcessCallback* cb)
{
    wxString newCmd = cmd;
    if((flags & IProcessCreateAsSuperuser)) {
        if(wxFileName::Exists("/usr/bin/sudo")) {
            newCmd.Prepend("/usr/bin/sudo --askpass ");
            gsgsDEBUG1() << "Executing command:" << newCmd << clEndl;

        } else {
            gsgsWARNING() << "Unable to run command: '" << cmd
                        << "' as superuser: /usr/bin/sudo: no such file or directory" << clEndl;
        }
    } else {
        gsgsDEBUG1() << "Executing command:" << newCmd << clEndl;
    }

    make_argv(newCmd);
    if(argc == 0) { return NULL; }

    // fork the child process
    wxString curdir = wxGetCwd();

    // Prentend that we are a terminal...
    int master, slave;
    char pts_name[1024];
    memset(pts_name, 0x0, sizeof(pts_name));
    openpty(&master, &slave, pts_name, NULL, NULL);

    // Create a one-way communication channel (pipe).
    // If successful, two file descriptors are stored in stderrPipes;
    // bytes written on stderrPipes[1] can be read from stderrPipes[0].
    // Returns 0 if successful, -1 if not.
    int stderrPipes[2] = { 0, 0 };
    if(flags & IProcessStderrEvent) {
        errno = 0;
        if(pipe(stderrPipes) < 0) {
            gsgsERROR() << "Failed to create pipe for stderr redirecting." << strerror(errno);
            flags &= ~IProcessStderrEvent;
        }
    }

    int rc = fork();
    if(rc == 0) {
        //===-------------------------------------------------------
        // Child process
        //===-------------------------------------------------------
        struct termios termio;
        tcgetattr(slave, &termio);
        cfmakeraw(&termio);
        termio.c_lflag = ICANON;
        termio.c_oflag = ONOCR | ONLRET;
        tcsetattr(slave, TCSANOW, &termio);

        // Set 'slave' as STD{IN|OUT|ERR} and close slave FD
        login_tty(slave);
        close(master); // close the un-needed master end

        // Incase the user wants to get separate events for STDERR, dup2 STDERR to the PIPE write end
        // we opened earlier
        if(flags & IProcessStderrEvent) {
            // Dup stderrPipes[1] into stderr
            close(STDERR_FILENO);
            dup2(stderrPipes[1], STDERR_FILENO);
            close(stderrPipes[0]); // close the read end
        }
        close(slave);

        // at this point, slave is used as stdin/stdout/stderr
        // Child process
        if(workingDirectory.IsEmpty() == false) { wxSetWorkingDirectory(workingDirectory); }

        // execute the process
        errno = 0;
        if(execvp(argv[0], argv) < 0) { gsgsERROR() << "execvp('" << newCmd << "') error:" << strerror(errno) << clEndl; }

        // if we got here, we failed...
        exit(0);

    } else if(rc < 0) {
        // Error

        // restore the working directory
        wxSetWorkingDirectory(curdir);

        return NULL;

    } else {
        //===-------------------------------------------------------
        // Parent
        //===-------------------------------------------------------
        close(slave);
        freeargv(argv);
        argc = 0;

        // disable ECHO
        struct termios termio;
        tcgetattr(master, &termio);
        cfmakeraw(&termio);
        termio.c_lflag = ICANON;
        termio.c_oflag = ONOCR | ONLRET;
        tcsetattr(master, TCSANOW, &termio);

        // restore the working directory
        wxSetWorkingDirectory(curdir);

        UnixProcessImpl* proc = new UnixProcessImpl(parent);
        proc->m_callback = cb;
        if(flags & IProcessStderrEvent) {
            close(stderrPipes[1]); // close the write end
            // set the stderr handle
            proc->SetStderrHandle(stderrPipes[0]);
        }

        proc->SetReadHandle(master);
        proc->SetWriteHandler(master);
        proc->SetPid(rc);
        proc->m_flags = flags; // Keep the creation flags
        
        // Keep the terminal name, we will need it
        proc->SetTty(pts_name);
        
        if(!(proc->m_flags & IProcessCreateSync)) { proc->StartReaderThread(); }
        return proc;
    }
}
//-----------------------------------------------------------------------------------
void UnixProcessImpl::StartReaderThread()
{
    // Launch the 'Reader' thread
    m_thr = new ProcessReaderThread();
    m_thr->SetProcess(this);
    m_thr->SetNotifyWindow(m_parent);
    m_thr->Start();
}
//-----------------------------------------------------------------------------------
void UnixProcessImpl::Terminate()
{
    wxKill(GetPid(), GetHardKill() ? wxSIGKILL : wxSIGTERM, NULL, wxKILL_CHILDREN);
    int status(0);
    // The real cleanup is done inside ::ChildTerminatedSingalHandler() signal handler (app.cpp)
    waitpid(-1, &status, WNOHANG);
}
//-----------------------------------------------------------------------------------
bool UnixProcessImpl::WriteToConsole(const wxString& buff)
{
    wxString tmpbuf = buff;
    tmpbuf.Trim().Trim(false);

    tmpbuf << wxT("\n");
    int bytes = write(GetWriteHandle(), tmpbuf.mb_str(wxConvUTF8).data(), tmpbuf.Length());
    return bytes == (int)tmpbuf.length();
}
//-----------------------------------------------------------------------------------
void UnixProcessImpl::Detach()
{
    if(m_thr) {
        // Stop the reader thread
        m_thr->Stop();
        delete m_thr;
    }
    m_thr = NULL;
}
//-----------------------------------------------------------------------------------
#endif //#if defined(__WXMAC )||defined(__WXGTK__)
