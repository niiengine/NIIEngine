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

#ifndef _SIMPLESYMBOLENGINE_H_
#define _SIMPLESYMBOLENGINE_H_

#include "gsgsPreInclude.h"
#include "UILayoutConfig.h"

#if defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES

#include <iosfwd>
#include <string>
#include <windows.h>
#include <dbghelp.h>

#include <wx/thread.h>

namespace gsgs
{
    //////////////////////////////////////////////////////////////////////////
    // SIMPLE SYMBOL ENGINE
    //////////////////////////////////////////////////////////////////////////

    /** Symbol Engine wrapper to assist with processing PDB information. 
     * Only works under windows platforms and it is thread safe.
     *
     * The code is based on 'SimpleSymbolEngine' V1.4 class created by 
     * Roger Orr <rogero@howzatt.demon.co.uk> (2004). Some code is also
     * inspired on http://www.codeproject.com/KB/threads/StackWalker.aspx
     * by Jochen Kalmbach.
     */
    class SimpleSymbolEngine
    {
    public:
        ~SimpleSymbolEngine();

        static SimpleSymbolEngine &instance();
        
        /** Provide a stack trace for the specified stack frame.
         * @access public 
         * @qualifier const
         * @param context Context record
         * @param exceptionCode Exception code
         * @param exceptionFlags Exception flags
         * @param exceptionType Exception class
         * @return wxString String with the stack trace
         */
        wxString StackTrace(PCONTEXT context, const DWORD exceptionCode, const DWORD exceptionFlags, const wxString &exceptionType=wxT("")) const;

    private:
        /** Critical section used to lock the @see StackTrace function **/
        static wxCriticalSection m_locker;

        SimpleSymbolEngine(SimpleSymbolEngine const &);
        SimpleSymbolEngine& operator=(SimpleSymbolEngine const &);
        SimpleSymbolEngine();

        /** Convert an address to a string.
        * @access public 
        * @qualifier const
        * @param address Address to convert
        * @return wxString Address as string
        */
        wxString AddressToString(PVOID address) const;

        /** Obtains the description of the supplied exception code (text).
        * @access public 
        * @qualifier const
        * @param exceptionCode Exception code to provide the description 
        * @return wxString Last exception's code description
        * @remarks Only relevant on Windows platforms
        */
        wxString GetExceptionCodeDescription(const DWORD exceptionCode) const;

        /** Obtains the description of the supplied exception flags (text).
        * @access public 
        * @qualifier const
        * @param exceptionFlags Exception flags to provide the description 
        * @return wxString Last exception's flags description
        * @remarks Only relevant on Windows platforms
        */
        wxString GetExceptionFlagsDescription(const DWORD exceptionFlags) const;
    private:
    };
}
#endif // defined(WIN32) && defined(_DEBUG) && CHECK_STACK_TRACES 

#endif