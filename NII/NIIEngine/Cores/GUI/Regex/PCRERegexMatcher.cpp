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
#include "CEGUI/PCRERegexMatcher.h"
#include "NiiUIException.h"

namespace NII
{
    //------------------------------------------------------------------------
    PCRERegexMatcher::PCRERegexMatcher() :
        d_regex(0)
    {
    }
    //------------------------------------------------------------------------
    PCRERegexMatcher::~PCRERegexMatcher()
    {
        release();
    }
    //------------------------------------------------------------------------
    void PCRERegexMatcher::setRegex(const String & regex)
    {
        // release old regex string.
        release();
        d_string.clear();
        // try to compile this new regex string
        const char * prce_error;
        int pcre_erroff;
        d_regex = pcre_compile(regex.c_str(), PCRE_UTF8, &prce_error, &pcre_erroff, 0);

        // handle failure
        if(!d_regex)
            N_EXCEPT(InvalidRequest, _I18n("Bad RegEx set: '") + regex +
                _I18n("'.  Additional Information: ") + prce_error));

        // set this last so that upon failure object is in consistant state.
        d_string = regex;
    }
    //------------------------------------------------------------------------
    const String & PCRERegexMatcher::getRegex() const
    {
        return d_string;
    }
    //------------------------------------------------------------------------
    RegexMatcher::MatchState PCRERegexMatcher::match(const String & str) const
    {
        // if the regex is not valid, then an exception is thrown
        if (!d_regex)
            N_EXCEPT(InvalidRequest,
                _I18n("Attempt to use invalid RegEx '") + d_string + "'."));

        int match[3];
        const char * utf8_str = str.c_str();
        const int len = static_cast<int>(strlen(utf8_str));

    #ifdef PCRE_PARTIAL_SOFT
        // we are using a new version of pcre
        const int result = pcre_exec(d_regex, 0, utf8_str, len, 0,
                                     PCRE_PARTIAL_SOFT | PCRE_ANCHORED, match, 3);
    #else
        // PCRE_PARTIAL is a backwards compatible synonym for PCRE_PARTIAL_SOFT
        // using it is a requirement if we want to support pcre < 8.0

        // Older versions of pcre have problems doing partial matching of
        // single repeated characters if using pcre_exec,
        // It is suggested to use pcre_dfa_exec instead.
        int workspace[100]; // FIXME: persist the workspace between match attempts
        const int result = pcre_dfa_exec(d_regex, 0, utf8_str, len, 0,
            PCRE_PARTIAL | PCRE_ANCHORED, match, 3, workspace, 100);
    #endif

        if(result == PCRE_ERROR_PARTIAL)
            return MS_PARTIAL;

        // a match must be for the entire string
        if(result >= 0)
            return (match[1] - match[0] == len) ? MS_VALID : MS_INVALID;

        // no match found or if test string or regex is 0
        if(result == PCRE_ERROR_NOMATCH || result == PCRE_ERROR_NULL)
            return MS_INVALID;

        // anything else is an error
        N_EXCEPT(InvalidRequest,
            _I18n("PCRE Error: ") + StrConv::conv((Ni32)result) +
            _I18n(" occurred while attempting to match the RegEx '") + d_string + "'."));
    }
    //------------------------------------------------------------------------
    void PCRERegexMatcher::release()
    {
        if(d_regex)
        {
            pcre_free(d_regex);
            d_regex = 0;
        }
    }
    //------------------------------------------------------------------------
}