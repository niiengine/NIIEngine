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

#ifndef gsgsLogManager_h
#define gsgsLogManager_h

#include "gsgsPreInclude.h"
#include <wx/stopwatch.h>
#include <wx/thread.h>

namespace gsgs
{
    class FileLogger;
    typedef FileLogger& (*FileLoggerFunction)(FileLogger&);

    class _gsgsAPI FileLogger
    {
    public:
        enum { System = -1, Error = 0, Warning = 1, Dbg = 2, Developer = 3 };
    public:
        FileLogger(int requestedVerbo);
        ~FileLogger();

        FileLogger& SetRequestedLogLevel(int level)
        {
            _requestedLogLevel = level;
            return *this;
        }

        int GetRequestedLogLevel() const { return _requestedLogLevel; }

        /**
         * @brief give a thread-id a unique name which will be displayed in log
         */
        static void RegisterThread(wxThreadIdType id, const wxString& name);
        static void UnRegisterThread(wxThreadIdType id);

        /**
         * @brief create log entry prefix
         */
        static wxString Prefix(int verbosity);

        void AddLogLine(const wxString& msg, int verbosity);

        /**
         * @brief print array into the log file
         * @param arr
         * @param verbosity
         */
        void AddLogLine(const wxArrayString& arr, int verbosity);

        static void SetVerbosity(int level);

        // Set the verbosity as string
        static void SetVerbosity(const wxString& verbosity);

        ///----------------------------------
        /// Statics
        ///----------------------------------
        /**
         * @brief open the log file
         */
        static void OpenLog(const wxString& fullName, int verbosity);
        // Various util methods
        static wxString GetVerbosityAsString(int verbosity);
        static int GetVerbosityAsNumber(const wxString& verbosity);

        inline FileLogger& operator<<(FileLoggerFunction f)
        {
            Flush();
            return *this;
        }

        /**
         * @brief special wxArrayString printing
         */
        inline FileLogger& operator<<(const wxArrayString& arr)
        {
            if(GetRequestedLogLevel() > m_verbosity) { return *this; }
            if(!m_buffer.IsEmpty()) { m_buffer << " "; }
            m_buffer << "[";
            if(!arr.IsEmpty()) {
                for(size_t i = 0; i < arr.size(); ++i) {
                    m_buffer << arr.Item(i) << ", ";
                }
                m_buffer.RemoveLast(2);
            }
            m_buffer << "]";
            return *this;
        }

        /**
         * @brief special wxString printing
         * Without this overload operator, on some compilers, the "gsgsDEBUG()<< wxString" might be "going" to the one
         * that handles wxFileName...
         */
        inline FileLogger& operator<<(const wxString& str)
        {
            if(GetRequestedLogLevel() > m_verbosity) { return *this; }
            if(!m_buffer.IsEmpty()) { m_buffer << " "; }
            m_buffer << str;
            return *this;
        }

        /**
         * @brief special wxFileName printing
         */
        inline FileLogger& operator<<(const wxFileName& fn)
        {
            if(GetRequestedLogLevel() > m_verbosity) { return *this; }
            if(!m_buffer.IsEmpty()) { m_buffer << " "; }
            m_buffer << fn.GetFullPath();
            return *this;
        }

        /**
         * @brief append any type to the buffer, take log level into consideration
         */
        template <typename T> FileLogger& Append(const T& elem, int level)
        {
            if(level > m_verbosity) { return *this; }
            if(!m_buffer.IsEmpty()) { m_buffer << " "; }
            m_buffer << elem;
            return *this;
        }

        /**
         * @brief flush the logger content
         */
        void Flush();
    protected:
        static wxString GetCurrentThreadName();
    protected:
        static int m_verbosity;
        static wxString m_logfile;
        int _requestedLogLevel;
        FILE* m_fp;
        wxString m_buffer;
        static std::unordered_map<wxThreadIdType, wxString> m_threads;
        static wxCriticalSection m_cs;
    };
    inline FileLogger & clEndl(FileLogger& d)
    {
        d.Flush();
        return d;
    }

    template <typename T> FileLogger& operator<<(FileLogger& logger, const T& obj)
    {
        logger.Append(obj, logger.GetRequestedLogLevel());
        return logger;
    }
    class _gsgsAPI FileLoggerNameRegistrar
    {
    public:
        FileLoggerNameRegistrar(const wxString& name) { FileLogger::RegisterThread(wxThread::GetCurrentId(), name); }
        ~FileLoggerNameRegistrar() { FileLogger::UnRegisterThread(wxThread::GetCurrentId()); }
    };
}

#define gsgs_System(...)    gsgs::FileLogger(gsgs::FileLogger::System).AddLogLine(wxString::Format(__VA_ARGS__), gsgs::FileLogger::System);
#define gsgs_Error(...)     gsgs::FileLogger(gsgs::FileLogger::Error).AddLogLine(wxString::Format(__VA_ARGS__), gsgs::FileLogger::Error);
#define gsgs_Warning(...)   gsgs::FileLogger(gsgs::FileLogger::Warning).AddLogLine(wxString::Format(__VA_ARGS__), gsgs::FileLogger::Warning);
#define gsgs_Debug(...)     gsgs::FileLogger(gsgs::FileLogger::Dbg).AddLogLine(wxString::Format(__VA_ARGS__), gsgs::FileLogger::Dbg);
#define gsgs_Debugs(s)      gsgs::FileLogger(gsgs::FileLogger::Dbg).AddLogLine(s, gsgs::FileLogger::Dbg);
#define gsgs_Debug1(...)    gsgs::FileLogger(gsgs::FileLogger::Developer).AddLogLine(wxString::Format(__VA_ARGS__), gsgs::FileLogger::Developer);
#define gsgs_DEBUG_ARR(arr) gsgs::FileLogger(gsgs::FileLogger::Dbg).AddLogLine(arr, gsgs::FileLogger::Dbg);
#define gsgs_DEBUG1_ARR(arr) gsgs::FileLogger(gsgs::FileLogger::Developer).AddLogLine(arr, gsgs::FileLogger::Developer);

#define gsgsDEBUG()         gsgs::FileLogger(gsgs::FileLogger::Dbg) << gsgs::FileLogger::Prefix(gsgs::FileLogger::Dbg)
#define gsgsDEBUG1()        gsgs::FileLogger(gsgs::FileLogger::Developer) << gsgs::FileLogger::Prefix(gsgs::FileLogger::Developer)
#define gsgsERROR()         gsgs::FileLogger(gsgs::FileLogger::Error) << gsgs::FileLogger::Prefix(gsgs::FileLogger::Error)
#define gsgsWARNING()       gsgs::FileLogger(gsgs::FileLogger::Warning) << gsgs::FileLogger::Prefix(gsgs::FileLogger::Warning)
#define gsgsSYSTEM()        gsgs::FileLogger(gsgs::FileLogger::System) << gsgs::FileLogger::Prefix(gsgs::FileLogger::System)

#define gsgsLogMessage(msg)   gsgsDEBUG() << msg
#endif
