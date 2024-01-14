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

#ifndef _NII_UI_PCRERegexMatcher_H_
#define _NII_UI_PCRERegexMatcher_H_

#include "NiiRegexManager.h"
#include <pcre.h>

namespace NII
{
namespace NII_MEDIA
{
    /** NII专用PCRE库实现正则表达式
        Details about the pattern syntax used by PCRE can be found on unix-like
        systems by way of <tt>man pcrepattern</tt> (or online at
        http://www.pcre.org/pcre.txt (scroll / search "PCREPATTERN(3)").
        Alternatively, see the perl regex documentation at
        http://perldoc.perl.org/perlre.html
    */
    class PCRERegexMatcher : public RegexMatcher
    {
    public:
        PCRERegexMatcher();
        ~PCRERegexMatcher();

        /// @copydetials RegexMatcher::setRegex
        void setRegex(const String & regex);
        
        /// @copydetials RegexMatcher::getRegex
        const String & getRegex() const;
        
        /// @copydetials RegexMatcher::match
        MatchState match(const String & str) const;
    private:
        void release();
    private:
        String d_string;
        pcre * d_regex;
    };
}
}
#endif