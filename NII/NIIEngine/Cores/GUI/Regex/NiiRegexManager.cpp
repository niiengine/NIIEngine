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

#include "NiiPreProcess.h"
#include "NiiRegexManager.h"

namespace NII
{
    template<> RegexManager * Singleton<RegexManager>::mOnly = 0;
    //------------------------------------------------------------------------
    RegexManager::RegexManager()
    {
    }
    //------------------------------------------------------------------------
    RegexManager::~RegexManager()
    {
    }
    //------------------------------------------------------------------------
    RegexMatcher * RegexManager::create() const
    {
#ifdef NII_PCRE_REGEX
        return N_new PCRERegexMatcher();
#else
        return 0;
#endif
    }
    //------------------------------------------------------------------------
    void RegexManager::destroy(RegexMatcher * rm) const
    {
        N_delete rm;
    }
    //------------------------------------------------------------------------
    /*RegexManager & RegexManager::getOnly()
    {
        return *mOnly;
    }
    //------------------------------------------------------------------------
    RegexManager * RegexManager::getOnlyPtr()
    {
        return mOnly;
    }*/
    //------------------------------------------------------------------------
}