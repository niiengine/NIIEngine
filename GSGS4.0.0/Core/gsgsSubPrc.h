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

#ifndef gsgsSubPrc_h
#define gsgsSubPrc_h

#if defined(__WXMSW__)
#define USE_IPROCESS 1
#else
#define USE_IPROCESS 0
#endif

#include "gsgsEvent.h"
#include <gsgsProcess.h>

namespace gsgs
{
    class _gsgsAPI SubPrc : public wxEvtHandler
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
#endif
