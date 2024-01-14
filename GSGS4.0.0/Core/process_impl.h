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

#ifndef __processimpl__
#define __processimpl__

#include "gsgsEvent.h"
#include "wx/thread.h"

class ProcessReaderThread;

wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ASYNC_PROCESS_OUTPUT, gsgs::ProcessEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ASYNC_PROCESS_STDERR, gsgs::ProcessEvent);
wxDECLARE_EXPORTED_EVENT(_gsgsAPI, wxEVT_ASYNC_PROCESS_TERMINATED, gsgs::ProcessEvent);
#ifdef __WXMSW__
#include "gsgsPreInclude.h"
#include "gsgsProcess.h"
#include <Windows.h>
#include <thread>

class WinWriterThread;

class _gsgsAPI WinProcessImpl : public IProcess
{
public:
    WinProcessImpl(wxEvtHandler* parent);
    virtual ~WinProcessImpl();

    // Create process asynchronously and return a process object
    static IProcess * Execute(wxEvtHandler * parent, const wxString & cmd, wxString & errMsg,
        size_t flags = IProcessCreateDefault, const wxString & workingDir = wxEmptyString,
            IProcessCallback * cb = NULL);

    /**
     * @brief read data from stdout and error
     * @param buff check the buffer when true is returned
     * @return return true on success or timeout, flase otherwise, incase of false the reader thread will terminate
     */
    virtual bool Read(wxString & buff, wxString & buffErr);

    // Write to the process stdin
    virtual bool Write(const wxString& buff);
    virtual bool Write(const std::string& buff);
    virtual bool WriteRaw(const wxString& buff);
    virtual bool WriteRaw(const std::string& buff);
    virtual bool WriteToConsole(const wxString& buff);

    // Return true if the process is still alive
    virtual bool IsAlive();

    // Clean the process resources and kill the process if it is
    // still alive
    virtual void Cleanup();
    virtual void Terminate();
    virtual void Detach();
protected:
    void StartReaderThread();
    bool DoReadFromPipe(HANDLE pipe, wxString & buff);
private:
    ProcessReaderThread * m_thr;
    char m_buffer[65537];
    WinWriterThread * m_writerThread = nullptr;
    std::unordered_set<long> m_initialChildren;

    // Creating process related handles
    HANDLE hChildStdinRd, hChildStdinWr, hChildStdinWrDup, hChildStdoutRd, hChildStdoutWr, hChildStdoutRdDup,
        hChildStderrRd, hChildStderrWr, hChildStderrRdDup, hSaveStdin, hSaveStdout, hSaveStderr;

    // Child process id & information
    DWORD dwProcessId;
    PROCESS_INFORMATION piProcInfo;
};

#endif

#if defined(__WXMAC__) || defined(__WXGTK__)
#include "gsgsPreInclude.h"
#include "gsgsProcess.h"

class wxTerminal;
class _gsgsAPI UnixProcessImpl : public IProcess
{
    friend class wxTerminal;
public:
    UnixProcessImpl(wxEvtHandler* parent);
    virtual ~UnixProcessImpl();

    static IProcess* Execute(wxEvtHandler* parent, const wxString& cmd, size_t flags,
                             const wxString& workingDirectory = wxEmptyString, IProcessCallback* cb = NULL);

    inline void SetReadHandle(const int& readHandle)        { this->m_readHandle = readHandle; }
    inline void SetWriteHandler(const int& writeHandler)    { this->m_writeHandle = writeHandler; }
    inline int GetReadHandle() const                        { return m_readHandle; }
    inline int GetStderrHandle() const                      { return m_stderrHandle; }
    inline int GetWriteHandle() const                       { return m_writeHandle; }
    inline void SetStderrHandle(int stderrHandle)           { this->m_stderrHandle = stderrHandle; }

    inline void SetTty(const wxString& tty)                 { this->m_tty = tty; }
    inline const wxString& GetTty() const                   { return m_tty; }

    virtual void Cleanup();
    virtual bool IsAlive();
    virtual bool Read(wxString& buff, wxString& buffErr);
    virtual bool Write(const wxString& buff);
    virtual bool Write(const std::string& buff);
    virtual bool WriteRaw(const wxString& buff);
    virtual bool WriteRaw(const std::string& buff);
    virtual void Terminate();
    virtual bool WriteToConsole(const wxString& buff);
    virtual void Detach();
private:
    void StartReaderThread();
    bool ReadFromFd(int fd, fd_set& rset, wxString& output);
private:
    int m_readHandle;
    int m_stderrHandle = wxNOT_FOUND;
    int m_writeHandle;
    ProcessReaderThread* m_thr = nullptr;
    wxString m_tty;
};
#endif //#if defined(__WXMAC )||defined(__WXGTK__)

#endif // __unixprocessimpl__
