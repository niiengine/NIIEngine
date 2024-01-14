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

#ifndef CXXTEMPLATEFUNCTION_H
#define CXXTEMPLATEFUNCTION_H

#include "gsgsPreInclude.h"
#include "gsgsTag.h"
#include "CxxLexerAPI.h"

namespace gsgs
{
    class _gsgsAPI CxxTemplateFunction
    {
    public:
        CxxTemplateFunction(gsgs::TagEntryPtr tag);
        virtual ~CxxTemplateFunction();

        /**
         * @brief parse the template definition list
         */
        virtual void ParseDefinitionList();
        const wxArrayString& GetList() const { return m_list; }
        
        /**
         * @brief can the template arguments be deduced from the function 
         * signature?
         */
        bool CanTemplateArgsDeduced();
    private:
        Scanner_t m_scanner;
        Scanner_t m_sigScanner;
        wxArrayString m_list;
    };
}
#endif