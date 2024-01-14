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

#ifndef gsgsEXCEPTION_H
#define gsgsEXCEPTION_H

#include "gsgsPreInclude.h"

namespace gsgs
{
    class _gsgsAPI Exception
    {
    public:
        Exception(const wxString& message) : m_message(message), m_errorCode(0) {}
        Exception(const wxString& message, int errorCode) : m_message(message), m_errorCode(errorCode) {}
        virtual ~Exception() {}
        
        inline const wxString & What() const { return m_message; }
        inline int ErrorCode() const { return m_errorCode; }
    private:
        wxString m_message;
        int      m_errorCode;
    };
}
#endif
