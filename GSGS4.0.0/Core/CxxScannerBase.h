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

#ifndef CXXSCANNERBASE_H
#define CXXSCANNERBASE_H

#include <wx/filename.h>
#include "CxxLexerAPI.h"

namespace gsgs
{
    class CxxPreProcessor;
    class _gsgsAPI CxxScannerBase
    {
    protected:
        Scanner_t m_scanner;
        wxFileName m_filename;
        size_t m_options;
        CxxPreProcessor* m_preProcessor;

    public:
        CxxScannerBase(CxxPreProcessor* preProcessor, const wxFileName& filename);
        virtual ~CxxScannerBase();
        
        /**
         * @brief main loop
         */
        void Parse();
        
        /**
         * @brief
         * @param token
         */
        virtual void OnToken(CxxLexerToken & token) { wxUnusedVar(token); }
    };

    class _gsgsAPI CIncludeStatementCollector : public CxxScannerBase
    {
    public:
        CIncludeStatementCollector(CxxPreProcessor* pp, const wxFileName& filename);
        virtual ~CIncludeStatementCollector();
        void OnToken(CxxLexerToken & token);
    };
}
#endif // CXXSCANNERBASE_H
