#ifndef CHILDPROCESS_H
#define CHILDPROCESS_H

#if defined(__WXMSW__)
#define USE_IPROCESS 1
#else
#define USE_IPROCESS 0
#endif

#include "gsgsEvent.h"
#include <gsgsProcess.h>

namespace gsgs
{
    class SubPrc : public wxEvtHandler
    {
    #if USE_IPROCESS
        IProcess* m_process = nullptr;
    #else
        UnixProcess * m_childProcess = nullptr;
    #endif
    public:
        SubPrc();
        virtual ~SubPrc();

        void Start(const wxArrayString& args);
        void Write(const wxString& message);
        void Write(const std::string& message);
    };
}
#endif // CHILDPROCESS_H
