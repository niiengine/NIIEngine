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

#ifndef CXXUSINGNAMESPACECOLLECTOR_H
#define CXXUSINGNAMESPACECOLLECTOR_H

#include "gsgsPreInclude.h"
#include "CxxScannerBase.h"

namespace gsgs
{
    class _gsgsAPI CxxUsingNamespaceCollector : public CxxScannerBase
    {
        wxArrayString m_usingNamespaces;

    private:
        void ParseUsingNamespace();

    public:
        virtual void OnToken(CxxLexerToken& token);

        void SetUsingNamespaces(const wxArrayString& usingNamespaces) { this->m_usingNamespaces = usingNamespaces; }
        const wxArrayString& GetUsingNamespaces() const { return m_usingNamespaces; }
        CxxUsingNamespaceCollector(CxxPreProcessor* preProcessor, const wxFileName& filename);
        virtual ~CxxUsingNamespaceCollector();
    };
}
#endif
