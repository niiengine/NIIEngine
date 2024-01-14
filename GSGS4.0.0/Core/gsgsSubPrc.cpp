#include "gsgsProcess.h"
#include "gsgsSubPrc.h"
#include "gsgsLogManager.h"
#include "gsgsFileUtil.h"

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
    static wxString& wrap_with_quotes(wxString& str)
    {
        if(str.Contains(" ")) { str.Prepend("\"").Append("\""); }
        return str;
    }
    //------------------------------------------------------------------------------
    void SubPrc::Start(const wxArrayString& args)
    {
        if(args.IsEmpty()) { return; }
    #if USE_IPROCESS
        // Build command line from the array
        wxString command;
        command << args[0];

        wrap_with_quotes(command);
        for(size_t i = 1; i < args.size(); ++i) {
            wxString argument = args[i];
            wrap_with_quotes(argument);
            command << " " << argument;
        }

        // Launch the process
        m_process = ::CreateAsyncProcess(this, command, IProcessCreateDefault | IProcessStderrEvent);
    #else
        m_childProcess = new UnixProcess(this, args);
    #endif
    }
    //------------------------------------------------------------------------------
    void SubPrc::Write(const wxString & msg) 
    { 
        Write(FileUtil::ToStdString(msg)); 
    }
    //------------------------------------------------------------------------------
    void SubPrc::Write(const std::string & msg)
    {
    #if USE_IPROCESS
        m_process->WriteRaw(msg);
    #else
        m_childProcess->Write(FileUtil::ToStdString(msg));
    #endif
    }
    //------------------------------------------------------------------------------
}