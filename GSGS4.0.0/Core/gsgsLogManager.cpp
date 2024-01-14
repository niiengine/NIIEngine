
#include "gsgsPathManager.h"
#include "gsgsLogManager.h"
#include <wx/crt.h>
#include <wx/log.h>

namespace gsgs
{
    int FileLogger::m_verbosity = FileLogger::Error;
    wxString FileLogger::m_logfile;
    std::unordered_map<wxThreadIdType, wxString> FileLogger::m_threads;
    wxCriticalSection FileLogger::m_cs;

    FileLogger::FileLogger(int requestedVerbo)
        : _requestedLogLevel(requestedVerbo)
        , m_fp(NULL)
    {
        m_fp = wxFopen(m_logfile, wxT("a+"));
    }

    FileLogger::~FileLogger()
    {
        if(m_fp) {
            // flush any content that remain
            Flush();
            fclose(m_fp);
            m_fp = NULL;
        }
    }

    void FileLogger::AddLogLine(const wxString& msg, int verbosity)
    {
        if(msg.IsEmpty()) return;
        if((m_verbosity >= verbosity) && m_fp) {
            wxString formattedMsg = Prefix(verbosity);
            formattedMsg << " " << msg;
            formattedMsg.Trim().Trim(false);
            formattedMsg << wxT("\n");
            wxFprintf(m_fp, wxT("%s"), formattedMsg.c_str());
            fflush(m_fp);
        }
    }

    void FileLogger::SetVerbosity(int level)
    {
        if(level > FileLogger::Warning) {
            gsgsSYSTEM() << "Log verbosity is now set to:" << FileLogger::GetVerbosityAsString(level) << clEndl;
        }
        m_verbosity = level;
    }

    int FileLogger::GetVerbosityAsNumber(const wxString& verbosity)
    {
        if(verbosity == wxT("Debug")) {
            return FileLogger::Dbg;

        } else if(verbosity == wxT("Error")) {
            return FileLogger::Error;

        } else if(verbosity == wxT("Warning")) {
            return FileLogger::Warning;

        } else if(verbosity == wxT("System")) {
            return FileLogger::System;

        } else if(verbosity == wxT("Developer")) {
            return FileLogger::Developer;

        } else {
            return FileLogger::Error;
        }
    }

    wxString FileLogger::GetVerbosityAsString(int verbosity)
    {
        switch(verbosity) {
        case FileLogger::Dbg:
            return wxT("Debug");

        case FileLogger::Error:
            return wxT("Error");

        case FileLogger::Warning:
            return wxT("Warning");

        case FileLogger::Developer:
            return wxT("Developer");

        default:
            return wxT("Error");
        }
    }

    void FileLogger::SetVerbosity(const wxString& verbosity) { SetVerbosity(GetVerbosityAsNumber(verbosity)); }

    void FileLogger::OpenLog(const wxString& fullName, int verbosity)
    {
        m_logfile.Clear();
        m_logfile << gsgs_Path().GetUserDataDir() << gsgsPathSep << fullName;
        m_verbosity = verbosity;
    }

    void FileLogger::AddLogLine(const wxArrayString& arr, int verbosity)
    {
        for(size_t i = 0; i < arr.GetCount(); ++i) {
            AddLogLine(arr.Item(i), verbosity);
        }
    }

    void FileLogger::Flush()
    {
        if(m_buffer.IsEmpty()) { return; }
        if(m_fp) {
            wxFprintf(m_fp, "%s\n", m_buffer);
            fflush(m_fp);
        }
        m_buffer.Clear();
    }

    wxString FileLogger::Prefix(int verbosity)
    {
        wxString prefix;
        //timeval tim;
        //gettimeofday(&tim, NULL);
        //int ms = (int)tim.tv_usec / 1000.0;

        //wxString msStr = wxString::Format(wxT("%03d"), ms);
        prefix << wxDateTime::UNow().FormatISOTime() << wxT(" ["); // << wxT(":") << msStr;
        switch(verbosity) {
        case System:
            prefix << wxT("SYSTEM]");
            break;

        case Error:
            prefix << wxT("ERROR]");
            break;

        case Warning:
            prefix << wxT("WRNNING]");
            break;

        case Dbg:
            prefix << wxT("DEBUG]");
            break;

        case Developer:
            prefix << wxT("DVL]");
            break;
        }

        wxString thread_name = GetCurrentThreadName();
        if(!thread_name.IsEmpty()) { prefix << " [" << thread_name << "]"; }
        return prefix;
    }

    wxString FileLogger::GetCurrentThreadName()
    {
        if(wxThread::IsMain()) { return "Main"; }
        wxCriticalSectionLocker locker(m_cs);
        std::unordered_map<wxThreadIdType, wxString>::iterator iter = m_threads.find(wxThread::GetCurrentId());
        if(iter != m_threads.end()) { return iter->second; }
        return "";
    }

    void FileLogger::RegisterThread(wxThreadIdType id, const wxString& name)
    {
        wxCriticalSectionLocker locker(m_cs);
        std::unordered_map<wxThreadIdType, wxString>::iterator iter = m_threads.find(id);
        if(iter != m_threads.end()) { m_threads.erase(iter); }
        m_threads[id] = name;
    }

    void FileLogger::UnRegisterThread(wxThreadIdType id)
    {
        wxCriticalSectionLocker locker(m_cs);
        std::unordered_map<wxThreadIdType, wxString>::iterator iter = m_threads.find(id);
        if(iter != m_threads.end()) { m_threads.erase(iter); }
    }
}