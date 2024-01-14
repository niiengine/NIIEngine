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
#include "gsgsProcess.h"
#include "gsgsEvent.h"
#include "gsgsLogManager.h"
#include "gsgsPathManager.h"
#include <wx/txtstrm.h>
#include <wx/sstream.h>
#include <wx/xrc/xmlres.h>

#include "process_impl.h"

#include <stdio.h>
#ifdef __WXMSW__
#include "wx/msw/private.h"
#include "wx/textbuf.h"
#ifndef pclose
#define pclose _pclose
#endif

#ifndef popen
#define popen _popen
#endif

#endif

#ifdef __FreeBSD__
#include <kvm.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/sysctl.h>
#include <sys/user.h>
#include <paths.h>
#endif

#ifdef __WXMSW__
#include "wx/filefn.h"
#include <memory>

/*static*/ 
WinProcess* WinProcess::Execute(const wxString& cmd, wxString &errMsg, const wxString &workingDir)
{
	SECURITY_ATTRIBUTES saAttr;
	BOOL fSuccess;
	
	wxString wd(workingDir);
	if(workingDir.IsEmpty()){
		wd = wxGetCwd();
	}
	
	// Set the bInheritHandle flag so pipe handles are inherited.
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// The steps for redirecting child process's STDOUT: 
	//     1. Save current STDOUT, to be restored later. 
	//     2. Create anonymous pipe to be STDOUT for child process. 
	//     3. Set STDOUT of the parent process to be write handle to 
	//        the pipe, so it is inherited by the child process. 
	//     4. Create a noninheritable duplicate of the read handle and
	//        close the inheritable read handle.  
	
	WinProcess *prc = new WinProcess();

	// Save the handle to the current STDOUT.  
	prc->hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE);  
	
	// Create a pipe for the child process's STDOUT.  
	if( !CreatePipe( &prc->hChildStdoutRd, &prc->hChildStdoutWr, &saAttr, 0) )
	{
		delete prc;
		return NULL;
	}

	// Set a write handle to the pipe to be STDOUT.  
	if( !SetStdHandle(STD_OUTPUT_HANDLE, prc->hChildStdoutWr) )
	{
		delete prc;
		return NULL;
	}
   
	// Create noninheritable read handle and close the inheritable read handle. 
    fSuccess = DuplicateHandle( GetCurrentProcess(), prc->hChildStdoutRd,
        GetCurrentProcess(),  &prc->hChildStdoutRdDup , 
		0,  FALSE,
        DUPLICATE_SAME_ACCESS );
	if( !fSuccess )
	{
		delete prc;
        return NULL;
	}
	CloseHandle( prc->hChildStdoutRd );
	
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
	if( !CreatePipe(&prc->hChildStdinRd, &prc->hChildStdinWr, &saAttr, 0) )
	{
		delete prc;
		return NULL;
	}
	// Set a read handle to the pipe to be STDIN.  
	if( !SetStdHandle(STD_INPUT_HANDLE, prc->hChildStdinRd) ) 
	{
		delete prc;
		return NULL;
	}
	// Duplicate the write handle to the pipe so it is not inherited.  
	fSuccess = DuplicateHandle(GetCurrentProcess(), prc->hChildStdinWr, 
		GetCurrentProcess(), &prc->hChildStdinWrDup, 
		0, FALSE,                  // not inherited       
		DUPLICATE_SAME_ACCESS ); 
	if( !fSuccess ) 
	{
		delete prc;
		return NULL;
	}
	CloseHandle(prc->hChildStdinWr);  

	// Execute the child process
	STARTUPINFO siStartInfo;  

	// Set up members of STARTUPINFO structure.  
	ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
	siStartInfo.cb = sizeof(STARTUPINFO);  
	
	siStartInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW; ;
	siStartInfo.hStdInput = prc->hChildStdinRd;
	siStartInfo.hStdOutput = prc->hChildStdoutWr;
	siStartInfo.hStdError = prc->hChildStdoutWr;

	// Set the window to hide
	siStartInfo.wShowWindow = SW_HIDE;
	BOOL ret = CreateProcess( NULL	,
#if wxVERSION_NUMBER < 2900
			(WCHAR*)cmd.GetData(),	
#else
			cmd.wchar_str(),						// shell line execution command
#endif
			NULL,										// process security attributes 
			NULL,										// primary thread security attributes 
			TRUE,										// handles are inherited 
			CREATE_NO_WINDOW,							// creation flags 
			NULL,										// use parent's environment 
			wd.c_str(),									// CD to tmp dir
			&siStartInfo,								// STARTUPINFO pointer 
			&prc->piProcInfo);							// receives PROCESS_INFORMATION 
	if( ret )
	{
		prc->dwProcessId = prc->piProcInfo.dwProcessId;
	}
	else
	{
		int err = GetLastError();
		wxUnusedVar(err);
		delete prc;
		return NULL;
	}
	
	// After process creation, restore the saved STDIN and STDOUT.  
	if( !SetStdHandle(STD_INPUT_HANDLE, prc->hSaveStdin) )
	{
		delete prc;
		return NULL;
	}
	if( !SetStdHandle(STD_OUTPUT_HANDLE, prc->hSaveStdout) )
	{
		
		delete prc;
		return NULL;
	}
	return prc;
}

WinProcess::WinProcess()
{
	hChildStdinRd = NULL;
	hChildStdoutWr = NULL;
	hChildStdinWrDup = NULL;
	hChildStdoutRdDup = NULL;
	piProcInfo.hProcess = NULL;
	piProcInfo.hThread = NULL;
}

WinProcess::~WinProcess()
{
}

bool WinProcess::Read(wxString& buff)
{
	DWORD dwRead;
	DWORD dwMode;
	DWORD dwTimeout;
	char *chBuf = new char [65536+1]; 	//64K should be sufficient buffer
	memset(chBuf, 0, 65536+1);
	
	std::auto_ptr<char> sp(chBuf);
	
	// Make the pipe to non-blocking mode
	dwMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
	dwTimeout = 1000;
	SetNamedPipeHandleState(hChildStdoutRdDup, 
							&dwMode,
							NULL,
							&dwTimeout);	// Timeout of 30 seconds
	if( ReadFile( hChildStdoutRdDup, chBuf, 65536, &dwRead, NULL) || dwRead == 0)
	{
		chBuf[dwRead/sizeof(char)] = 0;
		//printf("%s\n", chBuf);
		buff = wxString(chBuf, wxConvUTF8);
		if(buff.IsEmpty() && dwRead > 0){
			//conversion failed
			buff = wxString::From8BitData(chBuf);
		}
		return true;
	}
	return false;
}

bool WinProcess::Write(const wxString& buff)
{
	DWORD dwMode;
	DWORD dwTimeout;
	char  chBuf[4097]; 

	wxString tmpCmd = buff;
	tmpCmd = tmpCmd.Trim().Trim(false);
	tmpCmd += wxT("\n");

	strcpy(chBuf, tmpCmd.mb_str());

	// Make the pipe to non-blocking mode
	dwMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
	dwTimeout = 30000;
	SetNamedPipeHandleState(hChildStdinWrDup,
							&dwMode,
							NULL,
							NULL);				// Timeout of 30 seconds
	DWORD dwWritten;
	if(!WriteFile(hChildStdinWrDup, chBuf, (unsigned long)strlen(chBuf), &dwWritten, NULL))
		return false;
	return true;
}

bool WinProcess::IsAlive()
{
	DWORD dwExitCode;
	if(GetExitCodeProcess(piProcInfo.hProcess, &dwExitCode))
	{
		if(dwExitCode == STILL_ACTIVE)
			return true;
	}
	return false;
}


void WinProcess::Cleanup()
{
	if(IsAlive())
		TerminateProcess(piProcInfo.hProcess, 255);
	
	CloseHandle( hChildStdinRd);
	CloseHandle( hChildStdoutWr);
	CloseHandle( hChildStdinWrDup );
	CloseHandle( hChildStdoutRdDup );
	CloseHandle( piProcInfo.hProcess );
	CloseHandle( piProcInfo.hThread );

	hChildStdinRd = NULL;
	hChildStdoutWr = NULL;
	hChildStdinWrDup = NULL;
	hChildStdoutRdDup = NULL;
	piProcInfo.hProcess = NULL;
	piProcInfo.hThread = NULL;
}

#endif //__WXMSW__

#if defined(__WXGTK__) || defined(__WXOSX__)
#include <signal.h>
#include <sys/select.h>
#include <sys/types.h>
#include "processreaderthread.h"
#include "gsgsFileUtil.h"

UnixProcess::UnixProcess(wxEvtHandler* owner, const wxArrayString& args)
    : m_owner(owner)
{
    m_goingDown.store(false);

    // Open the pipes
    if(!m_childStdin.Open() || !m_childStderr.Open() || !m_childStdout.Open()) {
        gsgsERROR() << "Could not open redirection pipes." << strerror(errno);
        return;
    }

    child_pid = fork();
    if(child_pid == -1) { gsgsERROR() << _("Failed to start child process") << strerror(errno); }
    if(child_pid == 0) {
        // In child process
        dup2(m_childStdin.GetReadFd(), STDIN_FILENO);
        dup2(m_childStdout.GetWriteFd(), STDOUT_FILENO);
        dup2(m_childStderr.GetWriteFd(), STDERR_FILENO);
        m_childStdin.Close();
        m_childStdout.Close();
        m_childStderr.Close();

        char** argv = new char*[args.size() + 1];
        for(size_t i = 0; i < args.size(); ++i) {
            std::string cstr_arg = FileUtil::ToStdString(args[i]);
            argv[i] = new char[cstr_arg.length() + 1];
            strcpy(argv[i], cstr_arg.c_str());
            argv[i][cstr_arg.length()] = 0;
        }
        argv[args.size()] = 0;
        int result = execvp(argv[0], const_cast<char* const*>(argv));
        int errNo = errno;
        if(result == -1) {
            // Note: no point writing to stdout here, it has been redirected
            gsgsERROR() << "Error: Failed to launch program" << args << "." << strerror(errNo);
            exit(EXIT_FAILURE);
        }
    } else {
        // parent process
        m_childStdin.CloseReadFd();
        m_childStdout.CloseWriteFd();
        m_childStderr.CloseWriteFd();

        // Start the reader and writer threads
        StartWriterThread();
        StartReaderThread();
    }
}

UnixProcess::~UnixProcess()
{
    Detach();

    // Kill the child process (if it is still alive)
    Stop();
    Wait();
}

bool UnixProcess::ReadAll(int fd, std::string& content, int timeoutMilliseconds)
{
    fd_set rset;
    char buff[1024];
    FD_ZERO(&rset);
    FD_SET(fd, &rset);

    int seconds = timeoutMilliseconds / 1000;
    int ms = timeoutMilliseconds % 1000;

    struct timeval tv = { seconds, ms * 1000 }; //  10 milliseconds timeout
    int rc = ::select(fd + 1, &rset, nullptr, nullptr, &tv);
    if(rc > 0) {
        memset(buff, 0, sizeof(buff));
        if(read(fd, buff, (sizeof(buff) - 1)) > 0) {
            content.append(buff);
            return true;
        }
    } else if(rc == 0) {
        // timeout
        return true;
    }
    // error
    return false;
}

bool UnixProcess::Write(int fd, const std::string& message, std::atomic_bool& shutdown)
{
    int bytes = 0;
    std::string tmp = message;
    const int chunkSize = 4096;
    while(!tmp.empty() && !shutdown.load()) {
        errno = 0;
        bytes = ::write(fd, tmp.c_str(), tmp.length() > chunkSize ? chunkSize : tmp.length());
        int errCode = errno;
        if(bytes < 0) {
            if((errCode == EWOULDBLOCK) || (errCode == EAGAIN)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            } else if(errCode == EINTR) {
                continue;
            } else {
                break;
            }
        } else if(bytes) {
            tmp.erase(0, bytes);
        }
    }
    gsgsDEBUG() << "Wrote message of size:" << message.length();
    return tmp.empty();
}

int UnixProcess::Wait()
{
    if(child_pid != wxNOT_FOUND) {
        int status = 0;
        waitpid(child_pid, &status, WNOHANG);
        return WEXITSTATUS(status);
    } else {
        return 0;
    }
}

void UnixProcess::Stop()
{
    if(child_pid != wxNOT_FOUND) { ::kill(child_pid, SIGTERM); }
}

void UnixProcess::Write(const std::string& message)
{
    if(!m_writerThread) { return; }
    m_outgoingQueue.Post(message);
}

void UnixProcess::StartWriterThread()
{
    m_writerThread = new std::thread(
        [](UnixProcess* process, int fd) {
            while(!process->m_goingDown.load()) {
                std::string buffer;
                if(process->m_outgoingQueue.ReceiveTimeout(10, buffer) == wxMSGQUEUE_NO_ERROR) {
                    UnixProcess::Write(fd, buffer, std::ref(process->m_goingDown));
                }
            }
            gsgsDEBUG() << "UnixProcess writer thread: going down";
        },
        this, m_childStdin.GetWriteFd());
}

void UnixProcess::StartReaderThread()
{
    m_readerThread = new std::thread(
        [](UnixProcess* process, int stdoutFd, int stderrFd) {
            while(!process->m_goingDown.load()) {
                std::string content;
                if(!ReadAll(stdoutFd, content, 10)) {
                    gsgs::ProcessEvent evt(wxEVT_ASYNC_PROCESS_TERMINATED);
                    process->m_owner->AddPendingEvent(evt);
                    break;
                } else if(!content.empty()) {
                    gsgs::ProcessEvent evt(wxEVT_ASYNC_PROCESS_OUTPUT);
                    evt.SetOutput(wxString() << content);
                    process->m_owner->AddPendingEvent(evt);
                }
                content.clear();
                if(!ReadAll(stderrFd, content, 10)) {
                    gsgs::ProcessEvent evt(wxEVT_ASYNC_PROCESS_TERMINATED);
                    process->m_owner->AddPendingEvent(evt);
                    break;
                } else if(!content.empty()) {
                    gsgs::ProcessEvent evt(wxEVT_ASYNC_PROCESS_STDERR);
                    evt.SetOutput(wxString() << content);
                    process->m_owner->AddPendingEvent(evt);
                }
            }
            gsgsDEBUG() << "UnixProcess reader thread: going down";
        },
        this, m_childStdout.GetReadFd(), m_childStderr.GetReadFd());
}

void UnixProcess::Detach()
{
    m_goingDown.store(true);
    if(m_writerThread) {
        m_writerThread->join();
        wxDELETE(m_writerThread);
    }
    if(m_readerThread) {
        m_readerThread->join();
        wxDELETE(m_readerThread);
    }
}

#endif // OSX & GTK

ProcUtils::ProcUtils()
{
}

ProcUtils::~ProcUtils()
{
}

void ProcUtils::GetProcTree(std::map<unsigned long, bool>& parentsMap, long pid)
{
#ifdef __WXMSW__
    OSVERSIONINFO osver;

    // Check to see if were running under Windows95 or
    // Windows NT.
    osver.dwOSVersionInfoSize = sizeof(osver);
    if(!GetVersionEx(&osver)) {
        return;
    }

    if(osver.dwPlatformId != VER_PLATFORM_WIN32_NT) {
        return;
    }

    // get child processes of this node
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(!hProcessSnap) {
        return;
    }

    // Fill in the size of the structure before using it.
    PROCESSENTRY32 pe;
    memset(&pe, 0, sizeof(pe));
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    if(!Process32First(hProcessSnap, &pe)) {
        // Can't get first process.
        CloseHandle(hProcessSnap);
        return;
    }

    parentsMap[pid] = true;

    do {
        if(parentsMap.find(pe.th32ParentProcessID) != parentsMap.end()) {
            // get the process handle
            HANDLE hProcess = ::OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION,
                                            FALSE, // not inheritable
                                            (DWORD)pid);
            if(hProcess != NULL) {
                CloseHandle(hProcess);
                // dont kill the process, just keep its ID
                parentsMap[pe.th32ProcessID] = true;
            }
        }
    } while(Process32Next(hProcessSnap, &pe));
    CloseHandle(hProcessSnap);
#else
    parentsMap[pid] = true;
#endif
}

wxString ProcUtils::GetProcessNameByPid(long pid)
{
#ifdef __WXMSW__
    // go over the process modules and get the full path of
    // the executeable
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    //  Take a snapshot of all modules in the specified process.
    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, (DWORD)pid);
    if(hModuleSnap == INVALID_HANDLE_VALUE) {
        return wxEmptyString;
    }

    //  Set the size of the structure before using it.
    me32.dwSize = sizeof(MODULEENTRY32);

    //  Retrieve information about the first module,
    //  and exit if unsuccessful
    if(!Module32First(hModuleSnap, &me32)) {
        CloseHandle(hModuleSnap); // Must clean up the
        // snapshot object!
        return wxEmptyString;
    }

    // get the name of the process (it is located in the first entry)
    CloseHandle(hModuleSnap);
    return me32.szExePath;

#elif defined(__FreeBSD__)
    kvm_t* kvd;
    struct kinfo_proc* ki;
    int nof_procs;
    wxString cmd;

    if(!(kvd = kvm_openfiles(_PATH_DEVNULL, _PATH_DEVNULL, NULL, O_RDONLY, NULL))) return wxEmptyString;

    if(!(ki = kvm_getprocs(kvd, KERN_PROC_PID, pid, &nof_procs))) {
        kvm_close(kvd);
        return wxEmptyString;
    }

    cmd = wxString(ki->ki_ocomm, wxConvUTF8);
    kvm_close(kvd);

    return (cmd);
#else
    wxArrayString output;
    ExecuteCommand(wxT("ps -A -o pid,command --no-heading"), output);
    // parse the output and search for our process ID
    for(size_t i = 0; i < output.GetCount(); i++) {
        wxString line = output.Item(i);
        // remove whitespaces
        line = line.Trim();
        line = line.Trim(false);
        // get the process ID
        wxString spid = line.BeforeFirst(wxT(' '));
        long cpid(0);
        spid.ToLong(&cpid);
        if(cpid == pid) {
            // we got a match, extract the command, it is in the second column
            wxString command = line.AfterFirst(wxT(' '));
            return command;
        }
    }
    return wxEmptyString; // Not implemented yet
#endif
}

void ProcUtils::ExecuteCommand(const wxString& command, wxArrayString& output, long flags)
{
#ifdef __WXMSW__
    wxExecute(command, output, flags);
#else
    FILE* fp;
    char line[512];
    memset(line, 0, sizeof(line));
    fp = popen(command.mb_str(wxConvUTF8), "r");
    if(fp) {
        while(fgets(line, sizeof(line), fp)) {
            output.Add(wxString(line, wxConvUTF8));
            memset(line, 0, sizeof(line));
        }
        pclose(fp);
    }
#endif
}

void ProcUtils::ExecuteInteractiveCommand(const wxString& command)
{
    wxShell(command);
}

void ProcUtils::GetProcessList(std::vector<ProcessEntry>& proclist)
{
#ifdef __WXMSW__
    OSVERSIONINFO osver;

    // Check to see if were running under Windows95 or
    // Windows NT.
    osver.dwOSVersionInfoSize = sizeof(osver);
    if(!GetVersionEx(&osver)) {
        return;
    }

    if(osver.dwPlatformId != VER_PLATFORM_WIN32_NT) {
        return;
    }

    // get child processes of this node
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(!hProcessSnap) {
        return;
    }

    // Fill in the size of the structure before using it.
    PROCESSENTRY32 pe;
    memset(&pe, 0, sizeof(pe));
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    if(!Process32First(hProcessSnap, &pe)) {
        // Can't get first process.
        CloseHandle(hProcessSnap);
        return;
    }

    do {
        ProcessEntry entry;
        entry.name = pe.szExeFile;
        entry.pid = (long)pe.th32ProcessID;
        proclist.push_back(entry);
    } while(Process32Next(hProcessSnap, &pe));
    CloseHandle(hProcessSnap);

#elif defined(__FreeBSD__)
    kvm_t* kvd;
    struct kinfo_proc* ki;
    int nof_procs, i;

    if(!(kvd = kvm_openfiles(_PATH_DEVNULL, _PATH_DEVNULL, NULL, O_RDONLY, NULL))) return;

    if(!(ki = kvm_getprocs(kvd, KERN_PROC_PROC, 0, &nof_procs))) {
        kvm_close(kvd);
        return;
    }

    for(i = 0; i < nof_procs; i++) {
        ProcessEntry entry;
        entry.pid = ki[i].ki_pid;
        entry.name = wxString(ki[i].ki_ocomm, wxConvUTF8);
        proclist.push_back(entry);
    }
    kvm_close(kvd);

#else
    // GTK and other
    wxArrayString output;
#if defined(__WXGTK__)
    ExecuteCommand(wxT("ps -A -o pid,command  --no-heading"), output);
#elif defined(__WXMAC__)
    // Mac does not like the --no-heading...
    ExecuteCommand(wxT("ps -A -o pid,command "), output);
#endif
    for(size_t i = 0; i < output.GetCount(); i++) {
        wxString line = output.Item(i);
        // remove whitespaces
        line = line.Trim().Trim(false);

        // get the process ID
        ProcessEntry entry;
        wxString spid = line.BeforeFirst(wxT(' '));
        spid.ToLong(&entry.pid);
        entry.name = line.AfterFirst(wxT(' '));

        if(entry.pid == 0 && i > 0) {
            // probably this line belongs to the provious one
            ProcessEntry e = proclist.back();
            proclist.pop_back();
            e.name << entry.name;
            proclist.push_back(e);
        } else {
            proclist.push_back(entry);
        }
    }
#endif
}

void ProcUtils::GetChildren(long pid, std::vector<long>& proclist)
{
#ifdef __WXMSW__
    OSVERSIONINFO osver;

    // Check to see if were running under Windows95 or
    // Windows NT.
    osver.dwOSVersionInfoSize = sizeof(osver);
    if(!GetVersionEx(&osver)) {
        return;
    }

    if(osver.dwPlatformId != VER_PLATFORM_WIN32_NT) {
        return;
    }

    // get child processes of this node
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if(!hProcessSnap) {
        return;
    }

    // Fill in the size of the structure before using it.
    PROCESSENTRY32 pe;
    memset(&pe, 0, sizeof(pe));
    pe.dwSize = sizeof(PROCESSENTRY32);

    // Walk the snapshot of the processes, and for each process,
    // kill it if its parent is pid.
    if(!Process32First(hProcessSnap, &pe)) {
        // Can't get first process.
        CloseHandle(hProcessSnap);
        return;
    }

    // loop over all processes and collect all the processes their parent
    // pid matches PID
    do {
        if((long)pe.th32ParentProcessID == pid) {
            proclist.push_back((long)pe.th32ProcessID);
        }
    } while(Process32Next(hProcessSnap, &pe));
    CloseHandle(hProcessSnap);

#elif defined(__FreeBSD__)
    kvm_t* kvd;
    struct kinfo_proc* ki;
    int nof_procs, i;

    if(!(kvd = kvm_openfiles(_PATH_DEVNULL, _PATH_DEVNULL, NULL, O_RDONLY, NULL))) return;

    if(!(ki = kvm_getprocs(kvd, KERN_PROC_PROC, pid, &nof_procs))) {
        kvm_close(kvd);
        return;
    }

    for(i = 0; i < nof_procs; i++) {
        if(ki[i].ki_ppid == pid) proclist.push_back(ki[i].ki_pid);
    }

    kvm_close(kvd);

#else
    // GTK and other
    wxArrayString output;
#ifdef __WXGTK__
    ExecuteCommand(wxT("ps -A -o pid,ppid  --no-heading"), output);
#else
    ExecuteCommand(wxT("ps -A -o pid,ppid "), output);
#endif
    // parse the output and search for our process ID
    for(size_t i = 0; i < output.GetCount(); i++) {
        long lpid(0);
        long lppid(0);
        wxString line = output.Item(i);

        // remove whitespaces
        line = line.Trim().Trim(false);

        // get the process ID
        wxString spid = line.BeforeFirst(wxT(' '));
        spid.ToLong(&lpid);

        // get the process Parent ID
        wxString sppid = line.AfterFirst(wxT(' '));
        sppid.ToLong(&lppid);
        if(lppid == pid) {
            proclist.push_back(lpid);
        }
    }
#endif
}

bool ProcUtils::Shell(const wxString& programConsoleCommand)
{
    wxString cmd;
#ifdef __WXMSW__
    wxChar* shell = wxGetenv(wxT("COMSPEC"));
    if(!shell) {
        shell = (wxChar*)wxT("CMD.EXE");
    }

    // just the shell
    cmd = shell;
#elif defined(__WXMAC__)
    wxString path = wxGetCwd();
    cmd = wxString(wxT("osascript -e 'tell application \"Terminal\"' -e 'activate' -e 'do script with command \"cd ") +
                   path + wxT("\"' -e 'end tell'"));
#else // non-windows
    // try to locate the configured terminal
    wxString terminal;
    wxString where;
    wxArrayString tokens;
    wxArrayString configuredTerminal;
    /*if (Locate(wxT("gnome-terminal"), where)) {
        terminal = where;
    } else if (Locate(wxT("konsole"), where)) {
        wxString path = wxGetCwd();
        terminal << where << wxT(" --workdir \"") << path << wxT("\"");
    } else if (Locate(wxT("terminal"), where)) {
        terminal = where;
    } else if (Locate(wxT("lxterminal"), where)) {
        terminal = where;
    } else if (Locate(wxT("xterm"), where)) {
        terminal = where;
    }
    cmd = terminal;*/
    terminal = wxT("xterm");
    if(!programConsoleCommand.IsEmpty()) {
        tokens = wxStringTokenize(programConsoleCommand, wxT(" "), wxTOKEN_STRTOK);
        if(!tokens.IsEmpty()) {
            configuredTerminal = wxStringTokenize(tokens.Item(0), wxT("/"), wxTOKEN_STRTOK);
            if(!configuredTerminal.IsEmpty()) {
                terminal = configuredTerminal.Last();
                tokens.Clear();
                configuredTerminal.Clear();
            }
        }
    }
    if(Locate(terminal, where)) {
        if(terminal == wxT("konsole")) {
            wxString path = wxGetCwd();
            terminal.Clear();
            terminal << where << wxT(" --workdir \"") << path << wxT("\"");
        } else {
            terminal = where;
        }
    } else {
        return false;
    }
    cmd = terminal;
    terminal.Clear();
#endif
    return wxExecute(cmd, wxEXEC_ASYNC) != 0;
}

bool ProcUtils::Locate(const wxString& name, wxString& where)
{
    wxString command;
    wxArrayString output;
    command << wxT("which \"") << name << wxT("\"");
    ProcUtils::ExecuteCommand(command, output);

    if(output.IsEmpty() == false) {
        wxString interstingLine = output.Item(0);

        if(interstingLine.Trim().Trim(false).IsEmpty()) {
            return false;
        }

        if(!interstingLine.StartsWith(wxT("which: no "))) {
            where = output.Item(0);
            where = where.Trim().Trim(false);
            return true;
        }
    }
    return false;
}

void ProcUtils::SafeExecuteCommand(const wxString& command, wxArrayString& output)
{
#ifdef __WXMSW__
    wxString errMsg;
    WinProcess* proc = WinProcess::Execute(command, errMsg);
    if(!proc) {
        return;
    }

    // wait for the process to terminate
    wxString tmpbuf;
    wxString buff;

    while(proc->IsAlive()) {
        tmpbuf.Clear();
        proc->Read(tmpbuf);
        buff << tmpbuf;
        wxThread::Sleep(100);
    }
    tmpbuf.Clear();

    // Read any unread output
    proc->Read(tmpbuf);
    while(!tmpbuf.IsEmpty()) {
        buff << tmpbuf;
        tmpbuf.Clear();
        proc->Read(tmpbuf);
    }

    // Convert buff into wxArrayString
    buff.Trim().Trim(false);
    wxString s;
    int where = buff.Find(wxT("\n"));
    while(where != wxNOT_FOUND) {
        // use c_str() to make sure we create a unique copy
        s = buff.Mid(0, where).c_str();
        s.Trim().Trim(false);
        output.Add(s.c_str());
        buff.Remove(0, where + 1);

        where = buff.Find(wxT("\n"));
    }

    if(buff.empty() == false) {
        s = buff.Trim().Trim(false);
        output.Add(s.c_str());
    }

    proc->Cleanup();
    delete proc;

#else
    ProcUtils::ExecuteCommand(command, output);
#endif
}

wxString ProcUtils::SafeExecuteCommand(const wxString& command)
{
    wxString strOut;
    wxArrayString arr;
    SafeExecuteCommand(command, arr);

    for(size_t i = 0; i < arr.GetCount(); ++i) {
        strOut << arr.Item(i) << "\n";
    }

    if(!strOut.IsEmpty()) {
        strOut.RemoveLast();
    }
    return strOut;
}
/*++
    CreatePipe-like function that lets one or both handles be overlapped
Author:
    Dave Hart  Summer 1997
--*/
#ifdef __WXMSW__
ULONG PipeSerialNumber = 0;

/*++
Routine Description:
    The MyCreatePipe API is used to create an anonymous pipe I/O device.
    Unlike CreatePipe FILE_FLAG_OVERLAPPED may be specified for one or
    both handles.
    Two handles to the device are created.  One handle is opened for
    reading and the other is opened for writing.  These handles may be
    used in subsequent calls to ReadFile and WriteFile to transmit data
    through the pipe.
Arguments:
    lpReadPipe - Returns a handle to the read side of the pipe.  Data
        may be read from the pipe by specifying this handle value in a
        subsequent call to ReadFile.
    lpWritePipe - Returns a handle to the write side of the pipe.  Data
        may be written to the pipe by specifying this handle value in a
        subsequent call to WriteFile.
    lpPipeAttributes - An optional parameter that may be used to specify
        the attributes of the new pipe.  If the parameter is not
        specified, then the pipe is created without a security
        descriptor, and the resulting handles are not inherited on
        process creation.  Otherwise, the optional security attributes
        are used on the pipe, and the inherit handles flag effects both
        pipe handles.
    nSize - Supplies the requested buffer size for the pipe.  This is
        only a suggestion and is used by the operating system to
        calculate an appropriate buffering mechanism.  A value of zero
        indicates that the system is to choose the default buffering
        scheme.
Return Value:
    TRUE - The operation was successful.
    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.
--*/
BOOL APIENTRY MyCreatePipe(OUT LPHANDLE lpReadPipe, OUT LPHANDLE lpWritePipe, IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
    IN DWORD nSize)

{
    HANDLE ReadPipeHandle, WritePipeHandle;
    DWORD dwError;
    CHAR PipeNameBuffer[MAX_PATH];

    //
    // Only one valid OpenMode flag - FILE_FLAG_OVERLAPPED
    //
    DWORD dwReadMode = FILE_FLAG_OVERLAPPED;
    DWORD dwWriteMode = FILE_FLAG_OVERLAPPED;

    //
    //  Set the default timeout to 120 seconds
    //
    if (nSize == 0) { nSize = 4096; }

    // Increment the pipe number, in a MT safe way
    InterlockedIncrement(&PipeSerialNumber);
    sprintf(PipeNameBuffer, "\\\\.\\Pipe\\LOCAL\\RemoteExeAnon.%08x.%08x", (unsigned int)GetCurrentProcessId(),
        (unsigned int)PipeSerialNumber);

    ReadPipeHandle = CreateNamedPipeA(PipeNameBuffer, PIPE_ACCESS_INBOUND | dwReadMode, PIPE_TYPE_BYTE | PIPE_WAIT,
        1,          // Number of pipes
        nSize,      // Out buffer size
        nSize,      // In buffer size
        120 * 1000, // Timeout in ms
        lpPipeAttributes);

    if (!ReadPipeHandle) { return FALSE; }

    WritePipeHandle = CreateFileA(PipeNameBuffer, GENERIC_WRITE,
        0, // No sharing
        lpPipeAttributes, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | dwWriteMode,
        NULL // Template file
    );

    if (INVALID_HANDLE_VALUE == WritePipeHandle) {
        dwError = GetLastError();
        CloseHandle(ReadPipeHandle);
        SetLastError(dwError);
        return FALSE;
    }

    *lpReadPipe = ReadPipeHandle;
    *lpWritePipe = WritePipeHandle;
    return (TRUE);
}
#endif // __WXMSW__

IProcess * CreateAsyncProcess(wxEvtHandler* parent, const wxString& cmd, size_t flags, const wxString& workingDir)
{
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(parent, cmd, errMsg, flags, workingDir);
#else
    return UnixProcessImpl::Execute(parent, cmd, flags, workingDir);
#endif
}

IProcess* CreateAsyncProcessCB(wxEvtHandler* parent, IProcessCallback* cb, const wxString& cmd, size_t flags,
                               const wxString& workingDir)
{
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(parent, cmd, errMsg, flags, workingDir, cb);
#else
    return UnixProcessImpl::Execute(parent, cmd, flags, workingDir, cb);
#endif
}

IProcess* CreateSyncProcess(const wxString& cmd, size_t flags, const wxString& workingDir)
{
#ifdef __WXMSW__
    wxString errMsg;
    return WinProcessImpl::Execute(NULL, cmd, errMsg, flags | IProcessCreateSync, workingDir);
#else
    return UnixProcessImpl::Execute(NULL, cmd, flags | IProcessCreateSync, workingDir);
#endif
}

// Static methods:
bool IProcess::GetProcessExitCode(int pid, int& exitCode)
{
    wxUnusedVar(pid);
    wxUnusedVar(exitCode);

    exitCode = 0;
    return true;
}

void IProcess::SetProcessExitCode(int pid, int exitCode)
{
    wxUnusedVar(pid);
    wxUnusedVar(exitCode);
}

void IProcess::WaitForTerminate(wxString& output)
{
    if(IsRedirect()) 
    {
        wxString buff;
        wxString buffErr;
        while(Read(buff, buffErr)) 
        {
            output << buff;
            if(!buff.IsEmpty() && !buffErr.IsEmpty()) { output << "\n"; }
            output << buffErr;
        }
    } 
    else 
    {
        // Just wait for the process to terminate in a busy loop
        while(IsAlive()) 
        {
            wxThread::Sleep(10);
        }
    }
}

CmdProcess::CmdProcess(int id, const wxString& cmdLine, bool redirect)
    : wxProcess(NULL, id)
    //, m_pid(-1)
    , m_uid(id)
    , m_cmd(cmdLine)
    , m_redirect(redirect)
{
}

CmdProcess::~CmdProcess() {}

//long CmdProcess::GetPid() { return m_pid; }

//void CmdProcess::SetPid(long pid) { m_pid = pid; }

void CmdProcess::Terminate(wxSignal signalNo)
{
    wxKillError rc;
    wxKill(GetPid(), signalNo, &rc, wxKILL_CHILDREN);

    // Sleep for 150 ms to allow the process to be killed and
    // the main frame to handle the event or else we can get
    // memory leak
    wxMilliSleep(150);
}

long CmdProcess::Start(bool hide)
{
    if(m_redirect) {
        Redirect();
    }

    long flags = wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER;
    if(!hide) {
        flags |= wxEXEC_NOHIDE;
    }

    m_pid = wxExecute(m_cmd, flags, this);
    return m_pid;
}

bool CmdProcess::HasInput(wxString& input, wxString& errors)
{
    if(m_redirect == false) {
        wxASSERT_MSG(false, wxT("Process is not redirected"));
        return false;
    }

    bool hasInput = false;
    while(IsInputAvailable()) {
        wxTextInputStream tis(*GetInputStream());
        wxChar ch = tis.GetChar();
        input << ch;
        hasInput = true;
        if(ch == wxT('\n'))
            break;
    }

    while(IsErrorAvailable()) {
        wxTextInputStream tis(*GetErrorStream());
        wxChar ch = tis.GetChar();
        errors << ch;
        hasInput = true;
        if(ch == wxT('\n'))
            break;
    }
    return hasInput;
}

bool CmdProcess::ReadAll(wxString& input, wxString& errors)
{
    if(m_redirect == false) {
        wxASSERT_MSG(false, wxT("Process is not redirected"));
        return false;
    }

    bool hasInput = false;
    while(IsInputAvailable()) {
        wxTextInputStream tis(*GetInputStream());
        input << tis.GetChar();
        hasInput = true;
    }

    while(IsErrorAvailable()) {
        wxTextInputStream tis(*GetErrorStream());
        errors << tis.GetChar();
        hasInput = true;
    }
    return hasInput;
}

bool CmdProcess::Write(const wxString& text)
{
    if(m_redirect == false) {
        wxASSERT_MSG(false, wxT("Process is not redirected"));
        return false;
    }
    wxTextOutputStream tos(*GetOutputStream());
    tos.WriteString(text);
    return true;
}

const wxEventType wxEVT_ASYNC_PROC_ADDLINE = XRCID("async_proc_addline");
const wxEventType wxEVT_ASYNC_PROC_ADDERRLINE = XRCID("async_proc_adderrline");
const wxEventType wxEVT_ASYNC_PROC_STARTED = XRCID("async_proc_started");
const wxEventType wxEVT_ASYNC_PROC_ENDED = XRCID("async_proc_ended");

static int AsyncExeTimerID = wxNewId();

BEGIN_EVENT_TABLE(ExecCmd, wxEvtHandler)
EVT_TIMER(AsyncExeTimerID, ExecCmd::OnTimer)
END_EVENT_TABLE()

ExecCmd::ExecCmd(wxEvtHandler *owner)
    : m_proc(NULL)
    , m_owner(owner)
    , m_busy(false)
    , m_stop(false)
{
#ifndef __WXMSW__
    gsgs_Event().Bind(wxEVT_CL_PROCESS_TERMINATED, &ExecCmd::OnZombieReaperProcessTerminated, this);
#endif
    m_timer = new wxTimer(this, AsyncExeTimerID);
}

ExecCmd::~ExecCmd()
{
#ifndef __WXMSW__
    gsgs_Event().Unbind(wxEVT_CL_PROCESS_TERMINATED, &ExecCmd::OnZombieReaperProcessTerminated, this);
#endif
    wxDELETE(m_timer);
    wxDELETE(m_proc);
};

void ExecCmd::AppendLine(const wxString &line, bool isErr)
{
    if (!m_owner)
        return;

    wxCommandEvent event(isErr ? wxEVT_ASYNC_PROC_ADDERRLINE : wxEVT_ASYNC_PROC_ADDLINE);
    event.SetEventObject(this);
    event.SetString(line);
    m_owner->ProcessEvent(event);
}

void ExecCmd::Stop()
{
    m_stop = true;
    //kill the build process
    if (m_proc) {
        m_proc->Terminate();
    }
}

void ExecCmd::SendStartMsg()
{
    if (!m_owner)
        return;

    wxCommandEvent event(wxEVT_ASYNC_PROC_STARTED);
    event.SetEventObject(this);

    wxString msg;
    msg << _("Current working directory: ") << wxGetCwd() << wxT("\n");
    msg << _("Running program: ") << m_cmdLine << wxT("\n");
    event.SetString(msg);
    m_owner->ProcessEvent(event);
}

void ExecCmd::SendEndMsg(int exitCode)
{
    if (!m_owner)
        return;

    wxCommandEvent event(wxEVT_ASYNC_PROC_ENDED);
    event.SetEventObject(this);
    wxString message(_("Program exited with return code: "));
    message << exitCode << wxT("\n");
    event.SetString(message);
    m_owner->ProcessEvent(event);
}

void ExecCmd::OnTimer(wxTimerEvent &event)
{
    wxUnusedVar(event);
    if (m_stop) {
        m_proc->Terminate();
        return;
    }
    PrintOutput();
}

void ExecCmd::PrintOutput()
{
    if (m_proc->GetRedirect()) {
        wxString data, errors;
        m_proc->HasInput(data, errors);
        DoPrintOutput(data, errors);
    }
}

void ExecCmd::DoPrintOutput(const wxString &out, const wxString &err)
{
    if (!out.IsEmpty()) {
        wxStringTokenizer tt(out, wxT("\n"));
        while (tt.HasMoreTokens()) {
            AppendLine(tt.NextToken() + wxT("\n"));
        }
    }

    if (!err.IsEmpty()) {
        wxStringTokenizer tt(err, wxT("\n"));
        while (tt.HasMoreTokens()) {
            AppendLine(tt.NextToken() + wxT("\n"), true);
        }
    }
}

void ExecCmd::ProcessEnd(wxProcessEvent& event)
{
    //read all input before stopping the timer
    if (!m_stop) {
        if (m_proc->GetRedirect()) {
            wxString err;
            wxString out;
            m_proc->ReadAll(out, err);
            DoPrintOutput(out, err);
            out.Empty();
            err.Empty();
        }
    }

    //stop the timer if needed
    if (m_proc->GetRedirect()) {
        m_timer->Stop();
    }

    m_busy = false;
    m_stop = false;

    SendEndMsg(event.GetExitCode());
}

void ExecCmd::Execute(const wxString &cmdLine, bool hide, bool redirect)
{
    m_cmdLine = cmdLine;
    SetBusy(true);
    SendStartMsg();

    m_proc = new CmdProcess(wxNewId(), m_cmdLine, redirect);
    if (m_proc) {
        if (m_proc->Start(hide) == 0) {
            wxDELETE(m_proc);
            SetBusy(false);

        }
        else {
            //process started successfully, start timer if needed
            if (m_proc->GetRedirect()) {
                m_timer->Start(10);
            }
        }
    }
}

void ExecCmd::Terminate()
{
    m_proc->Terminate();
}

#ifndef __WXMSW__
void ExecCmd::OnZombieReaperProcessTerminated(wxProcessEvent& event)
{
    gsgs_Debug("ExecCmd: process %d terminated. (reported by OnZombieReaperProcessTerminated)", event.GetPid());
    event.Skip();
    if (m_proc && (event.GetPid() == m_proc->GetPid())) {
        gsgs_Debug("ExecCmd: this is our process! performing cleanup");
        ProcessEnd(event);
        event.Skip(false);
    }
}
#endif


PipedProcess::PipedProcess(int id, const wxString &cmdLine) : 
    wxProcess(NULL, id), 
    //m_pid(-1), 
    m_cmd(cmdLine)
{
}

PipedProcess::~PipedProcess()
{
}
/*
long PipedProcess::GetPid()
{
	return m_pid;
}

void PipedProcess::SetPid(long pid)
{
	m_pid = pid;
}*/

void PipedProcess::Terminate()
{
#ifdef __WXGTK__
	wxString cmd;
	wxFileName exePath(gsgs_Path().GetExecutablePath());
	wxFileName script(exePath.GetPath(), wxT("gsgs_kill_children"));
	cmd << wxT("/bin/sh -f ") << script.GetFullPath() << wxT(" ") << GetPid();
	wxExecute(cmd, wxEXEC_ASYNC);
#else
	wxKillError rc;
	wxKill(GetPid(), wxSIGKILL, &rc, wxKILL_CHILDREN);
#endif
}

long PipedProcess::Start(bool hide)
{
	Redirect();
	long flags = wxEXEC_ASYNC | wxEXEC_MAKE_GROUP_LEADER ;
	if(!hide){
		flags |= wxEXEC_NOHIDE;
	}

	m_pid = wxExecute(m_cmd, flags, this);
	return m_pid;
}

//read single line from input/error of the debugger
bool PipedProcess::HasInput(wxString &input)
{
	bool hasInput = false;
	bool cont1(true), cont2(true);
	while(cont1 || cont2){
		cont1 = false;
		cont2 = false;
		while( IsInputAvailable() )
		{
			wxTextInputStream tis(*GetInputStream());
			// this assumes that the output is always line buffered
			wxChar ch = tis.GetChar();
			input << ch;
			hasInput = true;
			if(ch == wxT('\n')){
				cont1 = false;
				break;
			}else{
				cont1 = true;
			}
		}

		while( IsErrorAvailable() )
		{
			wxTextInputStream tis(*GetErrorStream());
			// this assumes that the output is always line buffered
			wxChar ch = tis.GetChar();
			input << ch;
			hasInput = true;
			if(ch == wxT('\n')){
				cont2 = false;
				break;
			}else{
				cont2 = true;
			}
		}

	}
	return hasInput;
}

bool PipedProcess::ReadAll(wxString &input)
{
	bool hasInput = false;
	bool cont1(true), cont2(true);

	wxTextInputStream tis(*GetInputStream());
	wxTextInputStream tie(*GetErrorStream());
	while(cont1 || cont2){
		cont1 = false;
		cont2 = false;
		while( IsInputAvailable() )
		{
			// this assumes that the output is always line buffered
			wxChar ch = tis.GetChar();
			input << ch;
			hasInput = true;
			cont1 = true;
		}

		while( IsErrorAvailable() )
		{
			// this assumes that the output is always line buffered
			wxChar ch = tie.GetChar();
			input << ch;
			hasInput = true;
			cont2 = true;
		}
	}
	return hasInput;
}
