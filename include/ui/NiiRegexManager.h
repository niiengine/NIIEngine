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

#ifndef _NII_REGEX_MANAGER_H_
#define _NII_REGEX_MANAGER_H_

#include "NiiPreInclude.h"

namespace NII
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RegexMatcher : public UIAlloc
    {
    public:
        /// 
        enum MatchState
        {
            MS_VALID,
            MS_INVALID,
            MS_PARTIAL
        };

        virtual ~RegexMatcher() {}
        
        /**
        @version NIIEngine 3.0.0
        */
        virtual void setRegex(const String & regex) = 0;
        
        /**
        @version NIIEngine 3.0.0
        */
        virtual const String & getRegex() const = 0;
        
        /**
        @version NIIEngine 3.0.0
        */
        virtual MatchState match(const String & str) const = 0;
    };
    
    class RegexManager : public Singleton<RegexManager>, public DataAlloc
    {
    public:
        RegexManager();
        ~RegexManager();

        /**
        @version NIIEngine 3.0.0
        */
        RegexMatcher * create() const;

        /**
        @version NIIEngine 3.0.0
        */
        void destroy(RegexMatcher * obj) const;

        /// @copydetails Singleton::getOnly
        static RegexManager & getOnly();

        /// @copydetails Singleton::getOnlyPtr
        static RegexManager * getOnlyPtr();
    };
}
#endif