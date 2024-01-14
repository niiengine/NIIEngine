#include "gsgsSubPrc.h"
#include <gsgsProcess.h>
#include "gsgsLogManager.h"
#include "fileutils.h"
#include "processreaderthread.h"
#include <globals.h>

#if !USE_IPROCESS
#include "UnixProcess.h"
#endif

namespace gsgs
{
    //------------------------------------------------------------------------------
    SubPrc::SubPrc() {}
    //------------------------------------------------------------------------------
    SubPrc::~SubPrc()
    {
    #if USE_IPROCESS
        m_process->Detach();
        wxDELETE(m_process);
    #else
        m_childProcess->Detach();
        wxDELETE(m_childProcess);
    #endif
    }
    //------------------------------------------------------------------------------
    void SubPrc::Start(const wxArrayString & args)
    {
        if(args.IsEmpty()) { return; }
    #if USE_IPROCESS
        // Build command line from the array
        wxString command;
        command << args[0];

        ::WrapWithQuotes(command);
        for(size_t i = 1; i < args.size(); ++i) {
            wxString argument = args[i];
            ::WrapWithQuotes(argument);
            command << " " << argument;
        }

        // Launch the process
        m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault | IProcessStderrEvent);
    #else
        m_childProcess = new UnixProcess(this, args);
    #endif
    }
    //------------------------------------------------------------------------------
    void SubPrc::Write(const wxString & message) 
    { 
        Write(FileUtil::ToStdString(message)); 
    }
    //------------------------------------------------------------------------------
    void SubPrc::Write(const std::string& message)
    {
    #if USE_IPROCESS
        m_process->WriteRaw(message);
    #else
        m_childProcess->Write(FileUtil::ToStdString(message));
    #endif
    }
    //------------------------------------------------------------------------------
}
