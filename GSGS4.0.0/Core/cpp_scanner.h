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
#ifndef gsgs_CPPSCANNER_H
#define gsgs_CPPSCANNER_H

#include "gsgsPreInclude.h"
#include "FlexLexer.h"
#include "gsgsSharedPtr.h"

namespace gsgs
{
    class _gsgsAPI CppScanner : public flex::yyFlexLexer
    {
    public:
        CppScanner();
        ~CppScanner(void);

        /// Override the LexerInput function
        int LexerInput(char *buf, int max_size);
        void SetText(const char* data);
        void Reset();


        ///	Note about comment and line number:
        ///	If the last text consumed is a comment, the line number
        ///	returned is the line number of the last line of the comment
        ///	incase the comment spans over number of lines
        ///	(i.e. /* ... */ comment style)

        const int& LineNo() const { return yylineno; }
        inline void ClearComment() { m_comment = wxEmptyString; }
        inline const wxChar* GetComment() const { return m_comment.GetData(); }
        inline void KeepComment(const int& keep) { m_keepComments = keep; }
        inline void ReturnWhite(const int& rw) { m_returnWhite = rw; }
        void Restart();

    private:
        char *m_data;
        char *m_pcurr;
        int   m_total;
        int   m_curr;
    };
}
typedef gsgs::SharedPtr<gsgs::CppScanner> CppScannerPtr;
#endif 
